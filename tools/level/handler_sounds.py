"""Add "sounds" to every kind in extracted/meta/full-game/L*-types.json (L*.RE.02).

Static: decompiles each handler (spawn/update/init/tick) through ghidra-mcp and collects the constant sound ids
passed to the ENG1 sound request FUN_10006da0(id, obj) (`FUN_10006da0(0x...` in the decompiler text). Ids made at
runtime (variables) are not listed; runtime sfx_play events are S5.01.

    handler_sounds.py
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "ghidra"))
import mcp_http as m  # noqa: E402

PAT = re.compile(r"FUN_10006da0\(\(?(?:ushort\)|uint\)|short\))?(0x[0-9a-fA-F]+|\d+)")


def main() -> int:
    cache: dict[str, list[int]] = {}
    files = sorted((ROOT / "extracted/meta/full-game").glob("L*-types.json"))
    for f in files:
        t = json.loads(f.read_text(encoding="utf-8"))
        for k in t["kinds"]:
            sounds = set()
            for role, e in k["entry"].items():
                rva = e.get("rva")
                if not rva or rva == "0xc720" or role == "stub":
                    continue
                if rva not in cache:
                    try:
                        r = m.get("force_decompile", "ENG1.DLL", function=f"0x{int(rva, 16) + 0x10000000:x}")
                        code = r.get("decompiled", "") if isinstance(r, dict) else str(r)
                    except m.GhidraError:
                        code = ""
                    cache[rva] = sorted({int(x, 0) for x in PAT.findall(code)})
                sounds |= set(cache[rva])
            k["sounds"] = [f"0x{s:x}" for s in sorted(sounds)]
        f.write_text(json.dumps(t, indent=1) + "\n", encoding="utf-8")
        print(f.name, sum(1 for k in t["kinds"] if k["sounds"]), "kinds with sounds")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
