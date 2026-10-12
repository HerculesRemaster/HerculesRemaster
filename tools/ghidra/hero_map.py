"""Map the hero code: for every ENG1 function in 0x15000-0x1a800 record the states it sets
(0x10072108 = N), the animation scripts it assigns (hero +0x64), the pad masks it tests
(game state +0x5c/+0x5e), and writes to the hero motion fields (+0x00..+0x08 position,
+0xa8..+0xc0, +0x104/+0x108) with their constant values.

    hero_map.py -> analysis/exports/hero_map.json + hero_funcs/<addr>.c
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    funcs = m.get("find_functions", "ENG1.DLL", limit=2000)["functions"]
    hero = sorted(f["address"] for f in funcs if 0x10015000 <= int(f["address"], 16) < 0x1001a800)
    out_dir = ROOT / "analysis" / "exports" / "hero_funcs"
    out_dir.mkdir(parents=True, exist_ok=True)
    result = {}
    for a in hero:
        code = m.get("force_decompile", "ENG1.DLL", function="0x" + a)
        code = code if isinstance(code, str) else (code.get("decompiled") or "")
        (out_dir / f"{a}.c").write_text(code, encoding="utf-8")
        states = sorted({int(x, 0) for x in re.findall(r"DAT_10072108 = (0x[0-9a-f]+|\d+);", code)})
        scripts = sorted(set(re.findall(r"&DAT_(1005[0-9a-f]{4})", code)))
        masks = sorted(set(re.findall(r"\(DAT_10070f24 \+ 0x5[cde]\) & (0x[0-9a-f]+)", code) +
                           re.findall(r"DAT_10070f24\[0x5[cdef]\] & (0x[0-9a-f]+)U?", code)))
        motion = re.findall(r"\*\(\w+ \*\)\((?:DAT_10072860|DAT_1007217c) \+ (0x[0-9a-f]+)\) = (-?0x[0-9a-f]+|-?\d+);", code)
        motion += [(f"[{i}]", v) for i, v in re.findall(r"DAT_1007(?:2860|217c)\[(0x[0-9a-f]+|\d+)\] = (-?0x[0-9a-f]+|-?\d+);", code)]
        calls = sorted(set(re.findall(r"\b(FUN_1001[5-9a][0-9a-f]{3})\(", code)))
        result[a] = {"states": states, "scripts": scripts, "pad_masks": masks, "motion_writes": motion,
                     "calls": calls, "lines": code.count("\n")}
    (ROOT / "analysis" / "exports" / "hero_map.json").write_text(json.dumps(result, indent=1), encoding="utf-8")
    setters = {a: r for a, r in result.items() if r["states"]}
    print(f"hero functions={len(result)} state setters={len(setters)}")
    for a, r in setters.items():
        print(f"  {a}: states={r['states']} scripts={r['scripts'][:4]} pad={r['pad_masks']} motion={r['motion_writes'][:4]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
