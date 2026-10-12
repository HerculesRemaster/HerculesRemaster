"""Decode the PC "Sonix" banks: SFX.SFX and MUSIC.MUS (rules from HERCULES.EXE, docs/formats/sonix.md).

Both files share one layout (AUD_LoadSfx 0x1330 / AUD_LoadMusic 0x1110, VAB helpers 0xc040..0xc090):
  0x00 u32 body offset, 0x04 u32 head offset (both 0 in every file: the head is the file start)
  0x08..0x1b MUSIC.MUS only: u32 offsets of 5 sequence records (5 * 8 bytes at 0x1c..0x43)
  head+0x44  s16[128] ESF id per program (0 = none) -> m:\\language\\sonix\\pc_<id>.esf
  head+0x144 s8[128]  program volume (used by the play call 0xbf20: ((v*vol)^2 >> 14) * 0x81 >> 7)
A sequence record is two u32 CD positions (start, end); each u32 is bytes [unused, sec, min, track]
and is played as MCI "play cdaudio from <tmsf> to <tmsf>" (0x9e60). Music is Red Book audio.

    sonix.py  -> extracted/meta/sonix.json, prints totals
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "manifests" / "archive-manifest.json"


def cd_pos(raw: bytes) -> dict:
    _, sec, minute, track = raw
    return {"track": track, "min": minute, "sec": sec, "text": f"{track}:{minute:02d}:{sec:02d}"}


def parse(data: bytes, music: bool) -> dict:
    body, head = struct.unpack_from("<II", data, 0)
    ids = struct.unpack_from("<128h", data, head + 0x44)
    vols = struct.unpack_from("<128b", data, head + 0x144)
    progs = [{"program": i, "esf_id": ids[i], "volume": vols[i]} for i in range(128) if ids[i]]
    out = {"body_offset": body, "head_offset": head, "size": len(data), "programs": progs}
    if music:
        offs = struct.unpack_from("<5I", data, 8)
        seqs = []
        for k, o in enumerate(offs):
            start, end = cd_pos(data[o:o + 4]), cd_pos(data[o + 4:o + 8])
            seqs.append({"seq": k, "offset": o, "start": start, "end": end,
                         "silent": start["track"] == 0 or data[o:o + 8] == data[o + 4:o + 8] * 2})
        out["sequences"] = seqs
    return out


def main() -> int:
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    banks = []
    esf_ids = {int(Path(e["name_candidate"]).stem.split("_")[1]) for e in entries
               if e.get("name_candidate") and e["name_candidate"].upper().endswith(".ESF")}
    missing = set()
    for e in entries:
        name = (e.get("name_candidate") or "").upper()
        if not name.endswith((".SFX", ".MUS")):
            continue
        data = (ROOT / e["file"]).read_bytes()
        b = parse(data, name.endswith(".MUS"))
        b.update({"index": e["index"], "name": e["name_candidate"]})
        missing |= {p["esf_id"] for p in b["programs"]} - esf_ids
        banks.append(b)
    out = ROOT / "extracted" / "meta" / "sonix.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({"banks": banks, "esf_ids_referenced_but_missing": sorted(missing)}, indent=1),
                   encoding="utf-8")
    mus = [b for b in banks if "sequences" in b]
    tracks = sorted({s["start"]["track"] for b in mus for s in b["sequences"] if not s["silent"]})
    print(f"banks={len(banks)} sfx={len(banks) - len(mus)} mus={len(mus)} "
          f"programs={sum(len(b['programs']) for b in banks)} cd_tracks_used={tracks} "
          f"missing_esf={len(missing)} -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
