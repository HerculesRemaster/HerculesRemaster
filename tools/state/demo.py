"""Decode DEMO*.DAT attract-mode recordings (rules from ENG1.DLL, docs/demo-format.md).

Loader ENG1 0x5ea0 (from MOD_Tick): reads up to 0x1800 bytes of M:\\DEMO\\<name>.DAT (table
0x4d938, index = game state +8, or 0 with flag 0x2000000):
  byte 0 -> game state +1 and +2 = start index (HERO_Init 0x15090 reads the level start table
  0x5a69c[level] + 0x18*index; DEMO1_1/1_2/1_3 = 1/3/6 all play TRAINING), u16 at 2 -> OR'ed into
  flags +0x30, byte 4 -> +0xe.
Playback ENG1 0x6005, once per tick while the demo runs: index i starts at 3; held = u16[i+1]
(word index), pressed = (u16[i] ^ held) & held -> game state +0x5c / +0x5e; ends at i = 0x374
(884 ticks = 29.5 s at 30 Hz) or on real input (state +3 = 0x13).
Button bits are the pad bits written by the EXE (keyboard map EXE 0x7f30, keys from Config):
  0x10 up, 0x40 down, 0x80 left, 0x20 right, 0x8000 key X, 0x4000 key Z, 0x2000 key C,
  0x1000 key D, 0x800 key S (Space/Enter/Esc masks are set by the DLL through slot 0x24c).

    demo.py  -> extracted/demos/<name>.json, prints one line per demo
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "manifests" / "archive-manifest.json"
BITS = {0x10: "up", 0x40: "down", 0x80: "left", 0x20: "right", 0x8000: "X", 0x4000: "Z", 0x2000: "C",
        0x1000: "D", 0x800: "S"}


def names(mask: int) -> list[str]:
    out = [n for b, n in BITS.items() if mask & b]
    rest = mask & ~sum(BITS)
    if rest:
        out.append(f"0x{rest:04x}")
    return out


def decode(d: bytes) -> dict:
    words = struct.unpack_from(f"<{len(d) // 2}H", d, 0)
    ticks = []
    i = 3
    while i < 0x374:
        prev, held = words[i], words[i + 1] if i + 1 < len(words) else 0
        ticks.append({"tick": i - 3, "held": held, "pressed": (prev ^ held) & held})
        i += 1
    # run-length summary of held states
    runs = []
    for t in ticks:
        if runs and runs[-1]["held"] == t["held"]:
            runs[-1]["ticks"] += 1
        else:
            runs.append({"from_tick": t["tick"], "ticks": 1, "held": t["held"], "buttons": names(t["held"])})
    return {"start_index": d[0], "flags_or": struct.unpack_from("<H", d, 2)[0],
            "byte4": d[4], "ticks": len(ticks), "runs": runs,
            "pressed_counts": {n: sum(1 for t in ticks if t["pressed"] & b) for b, n in BITS.items()}}


def main() -> int:
    out = ROOT / "extracted" / "demos"
    out.mkdir(parents=True, exist_ok=True)
    for e in json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]:
        n = (e.get("name_candidate") or "")
        if not (n.upper().startswith("M:\\DEMO\\") and n.upper().endswith(".DAT")):
            continue
        name = n.rsplit("\\", 1)[-1][:-4]
        dem = decode((ROOT / e["file"]).read_bytes())
        dem.update({"index": e["index"], "name": name})
        (out / f"{name}.json").write_text(json.dumps(dem, indent=1), encoding="utf-8")
        busy = sorted(dem["pressed_counts"].items(), key=lambda kv: -kv[1])[:4]
        print(f"{name}: start_index={dem['start_index']} flags|=0x{dem['flags_or']:04x} byte4={dem['byte4']} "
              f"runs={len(dem['runs'])} presses={busy}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
