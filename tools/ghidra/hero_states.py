"""Collect the hero state handlers (ENG1 HERO_Init 0x15090 fills the table at 0x10072040) and
summarise each from its decompile: frames written (hero +0x34), state transitions (0x10072108 =),
pad masks tested, called functions and numeric constants. Creates missing functions.

    hero_states.py -> analysis/exports/hero_states.json (+ hero_states/<state>.c), one line per state
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
P = "ENG1.DLL"


def decomp(addr: str) -> str:
    try:
        m.post("create_function", P, {"address": "0x" + addr})
    except m.GhidraError:
        pass
    r = m.get("force_decompile", P, function="0x" + addr)
    return r if isinstance(r, str) else (r.get("decompiled") or r.get("code") or json.dumps(r))


def main() -> int:
    init = decomp("10015090")
    table = {}
    for slot, target in re.findall(r"_DAT_(100720[0-9a-f]{2}) = &LAB_([0-9a-f]{8});", init):
        table[(int(slot, 16) - 0x10072040) // 4] = target
    out_dir = ROOT / "analysis" / "exports" / "hero_states"
    out_dir.mkdir(parents=True, exist_ok=True)
    states = {}
    cache = {}
    for st in sorted(table):
        addr = table[st]
        code = cache.get(addr) or decomp(addr)
        cache[addr] = code
        (out_dir / f"state_{st:02d}_{addr}.c").write_text(code, encoding="utf-8")
        frames = re.findall(r"\(DAT_10072860 \+ 0xd\) = (0x[0-9a-f]+|\d+)", code) + \
                 re.findall(r"\(DAT_1007217c \+ 0xd\) = (0x[0-9a-f]+|\d+)", code)
        goes = re.findall(r"DAT_10072108 = (0x[0-9a-f]+|\d+)", code)
        masks = re.findall(r"\(DAT_10070f24 \+ 0x5[ce]\) & (0x[0-9a-f]+)", code) + \
                re.findall(r"DAT_10070f24\[0x5[cd]\] & (0x[0-9a-f]+)", code)
        calls = sorted(set(re.findall(r"\b(FUN_[0-9a-f]+|[A-Z]{2,5}_[A-Za-z0-9]+)\(", code)))
        consts = sorted(set(re.findall(r"0x[0-9a-f]{5,8}", code)))
        states[st] = {"handler": "ENG1+0x%x" % (int(addr, 16) - 0x10000000), "frames": [int(f, 0) for f in frames],
                      "next_states": sorted({int(g, 0) for g in goes}), "pad_masks": sorted(set(masks)),
                      "calls": calls, "constants": consts[:20], "lines": code.count("\n")}
        s = states[st]
        print(f"{st:2d} {s['handler']}: frames={s['frames'][:8]} next={s['next_states']} pad={s['pad_masks']} calls={len(calls)}")
    (ROOT / "analysis" / "exports" / "hero_states.json").write_text(json.dumps(states, indent=1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
