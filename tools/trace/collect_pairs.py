"""Collect verified input/output pairs from a hook run into analysis/traces/pairs/<index>/.

    collect_pairs.py analysis/traces/runs/<run>

For each pair the archive payload is copied (payload.bin, from extracted/raw) and checked:
- esf_<index>: adpcm_in.bin must equal payload[16:16+n]; pcm_out.s16le must equal an
  independent IMA-ADPCM decode of the payload with the tables read from HERCULES.EXE
  (index table 0x435e08, step table 0x435e48), high nibble first, as AUD_DecodeAdpcm (0x2410).
- vram_<n> with from_index >= 0: upload.bin must equal payload[from_pos:from_pos+bytes];
  for TGA payloads the upload is also rendered with the TGA palette to preview.png.
Unverified pairs are skipped (listed in the summary).
"""
from __future__ import annotations

import json
import shutil
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "decode"))
from esf import decode as ima_decode, exe_tables  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT / "backups" / "originals" / "HERCULES.EXE"
RAW = ROOT / "extracted" / "raw"
OUT = ROOT / "analysis" / "traces" / "pairs"


def payload(index: int) -> Path | None:
    hits = sorted(RAW.glob(f"{index:04d}_*.bin"))
    return hits[0] if hits else None


def main(run: Path) -> int:
    idx_tab, step_tab = exe_tables()
    ok, skipped = [], []
    for d in sorted((run / "pairs").iterdir()):
        meta = json.loads((d / "pair.json").read_text(encoding="utf-8"))
        if d.name.startswith("esf_"):
            index = meta["archive_index"]
            src = payload(index)
            data = src.read_bytes()
            adpcm = (d / "adpcm_in.bin").read_bytes()
            pcm = (d / "pcm_out.s16le").read_bytes()
            in_ok = data[16:16 + len(adpcm)] == adpcm
            pcm_bytes = struct.unpack_from("<I", data, 4)[0]
            mine = ima_decode(data[16:] + bytes(1), pcm_bytes, idx_tab, step_tab)
            # an odd sample count makes the game read one nibble past the data (next buffer byte),
            # so the last sample is excluded from the comparison in that case
            n = len(pcm) - 2 if (pcm_bytes // 2) % 2 else len(pcm)
            out_ok = mine[:n] == pcm[:n]
            if not (in_ok and out_ok):
                skipped.append(f"{d.name}: input_match={in_ok} decode_match={out_ok}")
                continue
            meta.update({"payload": "payload.bin", "verified": {"input_equals_payload_16": True,
                         "independent_ima_decode_equals_output": True}})
        else:
            index = meta.get("from_index", -1)
            if index < 0:
                skipped.append(f"{d.name}: source not linked to an archive read")
                continue
            src = payload(index)
            data = src.read_bytes()
            up = (d / "upload.bin").read_bytes()
            pos = meta["from_pos"]
            if data[pos:pos + len(up)] != up:
                skipped.append(f"{d.name}: upload != payload[{pos}:] (stale link)")
                continue
            meta.update({"payload": "payload.bin", "verified": {"upload_equals_payload_slice": True}})
            if data[:3] == b"\x00\x01\x01" and pos == 18 + 768:
                from PIL import Image
                w, h = meta["rect"]["w16"] * 2, meta["rect"]["h"]
                pal = data[18:18 + 768]
                im = Image.frombytes("P", (w, h), up)
                im.putpalette(b"".join(bytes((pal[i * 3 + 2], pal[i * 3 + 1], pal[i * 3])) for i in range(256)))
                dst_dir = OUT / f"{index:04d}"
                dst_dir.mkdir(parents=True, exist_ok=True)
                im.save(dst_dir / "preview.png")
                meta["format"] = f"8bpp indexed {w}x{h}, TGA palette (BGR) at payload 18..785, pixels from 786"
        dst = OUT / f"{index:04d}"
        if dst.exists() and (dst / "pair.json").exists() and d.name.startswith("vram_"):
            continue  # first verified pair per entry wins
        dst.mkdir(parents=True, exist_ok=True)
        for f in d.iterdir():
            if f.name != "pair.json":
                shutil.copy2(f, dst / f.name)
        shutil.copy2(src, dst / "payload.bin")
        meta["source_run"] = str(run.relative_to(ROOT))
        (dst / "pair.json").write_text(json.dumps(meta, indent=1), encoding="utf-8")
        ok.append(f"{d.name}->{index:04d}")
    print(f"verified={len(ok)} skipped={len(skipped)} -> {OUT}")
    print("  ok: " + ", ".join(ok))
    for s in skipped:
        print("  skip: " + s)
    return 0


if __name__ == "__main__":
    sys.exit(main(Path(sys.argv[1]).resolve()))
