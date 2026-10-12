"""ESF sound effects -> 16-bit PCM WAV + sidecar JSON (rules from HERCULES.EXE, see docs/formats/esf.md).

Format (AUD_CreateSfx 0xf3d0, AUD_DecodeAdpcm 0x2410):
  0  char[4] "ESF\\x03"   (checked as u32 0x03465345, else fatal "PC_CreateSFX(1)")
  4  u32 pcm_bytes        decoded size; ADPCM data = pcm_bytes/4 bytes from offset 16
  8  u16 rate             DirectSound buffer frequency
  10 u16 (unused)
  12 u8  loop             stored per SFX (0xa98a18[id]); passed to IDirectSoundBuffer::Play as DSBPLAY_LOOPING
  13 u8  bps8             1 = 8-bit DirectSound buffer, filled with 0x7f - (s16 >> 8)
  14 u16 (unused)
IMA-ADPCM, mono, high nibble first, predictor 0 and step index 0 at the start, tables read
from the EXE (index 0x435e08, step 0x435e48). pcm_bytes/2 samples; an odd count reads one
nibble past the data (the game takes the next buffer byte; here it is 0).

    esf.py [--out extracted/audio] [--survey-only]
Prints totals; writes extracted/meta/audio-coverage.json and esf-survey.json.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import wave
from collections import Counter
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT / "backups" / "originals" / "HERCULES.EXE"
MANIFEST = ROOT / "manifests" / "archive-manifest.json"
MAGIC = b"ESF\x03"


def exe_tables():
    pe = pefile.PE(str(EXE), fast_load=True)
    idx = struct.unpack("<16i", pe.get_data(0x435E08 - 0x400000, 64))
    step = struct.unpack("<89i", pe.get_data(0x435E48 - 0x400000, 89 * 4))
    return idx, step


def decode(src: bytes, pcm_bytes: int, idx_tab, step_tab) -> bytes:
    """Port of AUD_DecodeAdpcm (EXE 0x2410)."""
    index, pred, step = 0, 0, step_tab[0]
    n = pcm_bytes // 2
    data = src + bytes(1)
    out = bytearray(n * 2)
    pos, byte, high = 0, 0, True
    for k in range(n):
        if high:
            byte = data[pos] if pos < len(data) else 0
            pos += 1
            code = byte >> 4
        else:
            code = byte & 0xF
        high = not high
        index += idx_tab[code]
        index = 0 if index < 0 else 88 if index > 88 else index
        diff = step >> 3
        if code & 4:
            diff += step
        if code & 2:
            diff += step >> 1
        if code & 1:
            diff += step >> 2
        if code & 8:
            diff = -diff
        pred += diff
        pred = -0x8000 if pred < -0x8000 else 0x7FFF if pred > 0x7FFF else pred
        step = step_tab[index]
        struct.pack_into("<h", out, k * 2, pred)
    return bytes(out)


def header(data: bytes) -> dict:
    pcm, rate, pad1, loop, bps8, pad2 = struct.unpack_from("<IHHBBH", data, 4)
    return {"pcm_bytes": pcm, "rate": rate, "loop": loop, "bps8": bps8, "pad10": pad1, "pad14": pad2}


def safe_name(name: str | None, index: int) -> str:
    base = (name or f"esf_{index:04d}").replace("\\", "/").rsplit("/", 1)[-1]
    return f"{index:04d}_{base.rsplit('.', 1)[0]}"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=ROOT / "extracted" / "audio")
    ap.add_argument("--survey-only", action="store_true")
    a = ap.parse_args(argv)
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    esf = [e for e in entries if e["magic"] == MAGIC.hex()]
    idx_tab, step_tab = exe_tables()
    survey, cov = [], []
    stats = Counter()
    a.out.mkdir(parents=True, exist_ok=True)
    for e in esf:
        data = (ROOT / e["file"]).read_bytes()
        h = header(data)
        need = 16 + h["pcm_bytes"] // 4          # the game reads pcm_bytes >> 2 bytes
        row = {"index": e["index"], "id": e["id"], "name": e["name_candidate"], "size": len(data), **h,
               "adpcm_bytes": len(data) - 16, "size_vs_expected": len(data) - need,
               "odd_samples": (h["pcm_bytes"] // 2) % 2 == 1}
        survey.append(row)
        stats[f"rate_{h['rate']}"] += 1
        stats[f"loop_{h['loop']}"] += 1
        stats[f"bps8_{h['bps8']}"] += 1
        stats["size_exact" if row["size_vs_expected"] == 0 else "size_mismatch"] += 1
        if a.survey_only:
            continue
        try:
            pcm = decode(data[16:], h["pcm_bytes"], idx_tab, step_tab)
            stem = safe_name(e["name_candidate"], e["index"])
            with wave.open(str(a.out / f"{stem}.wav"), "wb") as w:
                w.setnchannels(1)
                w.setsampwidth(2)
                w.setframerate(h["rate"])
                w.writeframes(pcm)
            meta = {**row, "wav": f"{stem}.wav", "samples": len(pcm) // 2,
                    "seconds": round(len(pcm) / 2 / h["rate"], 3) if h["rate"] else None,
                    "decoder": "HERCULES.EXE+0x2410 (port)", "playback": "8-bit DSound buffer" if h["bps8"] else "16-bit DSound buffer"}
            (a.out / f"{stem}.json").write_text(json.dumps(meta, indent=1), encoding="utf-8")
            cov.append({"index": e["index"], "status": "ok", "wav": f"{stem}.wav"})
            stats["ok"] += 1
        except Exception as ex:  # noqa: BLE001
            cov.append({"index": e["index"], "status": "error", "error": str(ex)})
            stats["error"] += 1
    meta_dir = ROOT / "extracted" / "meta"
    meta_dir.mkdir(parents=True, exist_ok=True)
    (meta_dir / "esf-survey.json").write_text(json.dumps(survey, indent=1), encoding="utf-8")
    if not a.survey_only:
        (meta_dir / "audio-coverage.json").write_text(json.dumps(
            {"family": "esf", "total": len(esf), "counts": dict(stats), "entries": cov}, indent=1), encoding="utf-8")
    odd = [r for r in survey if r["size_vs_expected"]]
    print(f"esf={len(esf)} " + " ".join(f"{k}={v}" for k, v in sorted(stats.items())))
    if odd:
        print("size mismatches:", [(r["index"], r["size_vs_expected"]) for r in odd[:10]])
    return 0


if __name__ == "__main__":
    sys.exit(main())
