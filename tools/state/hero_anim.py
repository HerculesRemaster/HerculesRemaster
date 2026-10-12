"""Decode Hercules' animation scripts from ENG1.DLL data (interpreter ENG1 0x15d00, docs/gameplay-spec.md).

Script = array of {s16 a, s16 b}:
  a >= 0      show frame a for b+1 ticks (hero +0x34 = a, timer +0x30 = b, stepped when it goes below 0)
  a < 0       opcode op = a + 0x34 with argument b, executed without consuming a tick:
              0x33 loop (back to the script start or the last 0x32), 0x32 loop point = the next entry,
              0x31 stop (hold the frame),
              0x17 play SFX b (0x6da0) unless muted, 0x00 footstep SFX, 0x22/0x23 spawn effect b,
              0x2e flip x, 0x30 turn around, 0x24 set 0x7183c, 0x27 set bank (hero +0x22) = b,
              0x0d/0x11..0x16/0x19/0x28/0x2f conditional branches on held buttons, others set flags.
Script addresses are collected from the decompiled hero functions (ENG1 0x15000-0x1a800) as
references "&DAT_1005xxxx" assigned to hero +0x64.

    hero_anim.py -> extracted/sprites/0560_SRC1_HERO_HERCULES/hero_scripts.json, prints a summary
"""
from __future__ import annotations

import json
import re
import struct
import sys
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "ghidra"))
import mcp_http as m  # noqa: E402

OPS = {0x33: "loop", 0x32: "loop_point", 0x31: "stop", 0x17: "sfx", 0x00: "footstep_sfx", 0x22: "spawn_fx", 0x23: "spawn_fx2",
       0x2e: "flip", 0x30: "turn", 0x24: "set_7183c", 0x27: "set_bank", 0x0d: "if_released_goto_state_0x2e",
       0x11: "if_not_held_goto_state_0x29", 0x12: "if_down_goto_state_0xb", 0x13: "if_held_goto_state_0xe",
       0x14: "if_released_goto_state_0x2e_b", 0x15: "if_released_goto_state_0x2e_c", 0x16: "charge",
       0x19: "if_released_skip", 0x28: "if_up_hold", 0x2f: "if_down_hold", 0x1c: "nudge_x", 0x2c: "idle_pick",
       0x2d: "idle_random", 0x01: "state3_2", 0x29: "level_end", 0x2b: "state3_1", 0x2a: "fade"}
END = {0x33, 0x31}


def main() -> int:
    pe = pefile.PE(str(ROOT / "backups" / "originals" / "ENG1.DLL"), fast_load=True)
    img = pe.get_memory_mapped_image()
    rd = lambda va, n: img[va - 0x10000000:va - 0x10000000 + n]
    funcs = m.get("find_functions", "ENG1.DLL", limit=2000)["functions"]
    hero = [f["address"] for f in funcs if 0x10015000 <= int(f["address"], 16) < 0x1001a800]
    addrs = set()
    for a in hero:
        code = m.get("force_decompile", "ENG1.DLL", function="0x" + a)
        code = code if isinstance(code, str) else (code.get("decompiled") or "")
        for s in re.findall(r"\[0x19\] = \(int\)&DAT_(1005[0-9a-f]{4})", code) + \
                re.findall(r"\+ 100\) = &DAT_(1005[0-9a-f]{4})", code):
            addrs.add(int(s, 16))
    scripts = {}
    for va in sorted(addrs):
        steps, total = [], 0
        for i in range(200):
            a, b = struct.unpack("<hh", rd(va + 4 * i, 4))
            if a >= 0:
                steps.append({"frame": a, "ticks": b + 1, "arg": b})
                total += max(b + 1, 1)
                continue
            op = a + 0x34
            steps.append({"op": OPS.get(op, f"op_{op:02x}"), "code": op, "arg": b})
            if op in END:
                break
        scripts[f"0x{va:08x}"] = {"steps": steps, "frames": [s["frame"] for s in steps if "frame" in s],
                                  "ticks_total": total, "end": steps[-1].get("op") if steps else None}
    out = ROOT / "extracted" / "sprites" / "0560_SRC1_HERO_HERCULES" / "hero_scripts.json"
    out.write_text(json.dumps(scripts, indent=1), encoding="utf-8")
    for va, s in scripts.items():
        print(f"{va}: {len(s['frames'])} frames {s['frames'][:12]} ticks={s['ticks_total']} end={s['end']}")
    print(f"scripts={len(scripts)} from {len(hero)} hero functions -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
