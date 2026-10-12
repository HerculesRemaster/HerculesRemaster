"""Name ENG1 type-table handlers from their sprite bank (L*.RE.02) -> analysis/annotations/full_game_types.json.

A handler (spawn/update or init/tick RVA in extracted/meta/full-game/L*-types.json) gets the name
HERO_<Bank><Role> when every kind that uses it, in every level, has the same anim-table bank id
(the bank the object draws from: actor anim table +0xc / marker+pickup +6 -> obj+0x22). Handlers
already named in Ghidra (not FUN_*) are kept; shared handlers with different banks or no bank stay
unnamed and are listed. Apply with tools/ghidra/apply_annotations.py.

    name_handlers.py [--apply]
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ROLE = {"spawn": "Spawn", "update": "Update", "init": "Init", "tick": "Tick"}


def bank_word(path: str) -> str:
    leaf = path.split("\\")[-1]
    return re.sub(r"[^A-Za-z0-9]", "", leaf[:1].upper() + leaf[1:].lower())


def main(argv: list[str]) -> int:
    use = defaultdict(lambda: {"banks": set(), "roles": set(), "names": set(), "kinds": set()})
    for f in sorted((ROOT / "extracted/meta/full-game").glob("L*-types.json")):
        t = json.loads(f.read_text(encoding="utf-8"))
        for k in t["kinds"]:
            bank = (k.get("bank") or {}).get("path")
            for role, e in k["entry"].items():
                if role == "stub" or not e["rva"] or e["rva"] == "0xc720":
                    continue
                u = use[e["rva"]]
                u["banks"].add(bank)
                u["roles"].add(role)
                u["names"].add(e["name"])
                u["kinds"].add(f"{t['level']['dir']}:{k['kind']}")
    funcs, skipped = {}, []
    taken = {n for u in use.values() for n in u["names"] if n and not n.startswith("FUN_")}   # never reuse a name
    for rva, u in sorted(use.items()):
        cur = next(iter(u["names"])) if len(u["names"]) == 1 else None
        if cur and not cur.startswith("FUN_"):
            continue
        banks = {b for b in u["banks"] if b}
        if len(banks) != 1 or len(u["banks"]) != 1 or len(u["roles"]) != 1:
            skipped.append({"rva": rva, "banks": sorted(b or "-" for b in u["banks"]), "roles": sorted(u["roles"]),
                            "kinds": sorted(u["kinds"])[:6]})
            continue
        name = "HERO_" + bank_word(next(iter(banks))) + ROLE[next(iter(u["roles"]))]   # existing names: see taken
        base, n = name, 2
        while name in taken:
            name, n = f"{base}{n}", n + 1
        taken.add(name)
        funcs[f"0x{int(rva, 16) + 0x10000000:x}"] = [name, f"type-table {next(iter(u['roles']))} for {', '.join(sorted(u['kinds']))}; draws from bank {next(iter(banks))} (anim-table bank id)"]
    out = ROOT / "analysis/annotations/full_game_types.json"
    out.write_text(json.dumps({"ENG1.DLL": {"functions": funcs}, "_skipped": skipped}, indent=1, ensure_ascii=False) + "\n",
                   encoding="utf-8")
    print(f"{len(funcs)} handlers named, {len(skipped)} left (shared or no bank) -> {out}")
    if "--apply" in argv:
        tmp = ROOT / "analysis/annotations/_types_apply.json"
        tmp.write_text(json.dumps({"ENG1.DLL": {"functions": funcs}}, indent=1), encoding="utf-8")
        subprocess.run([sys.executable, str(ROOT / "tools/ghidra/apply_annotations.py"), str(tmp)], check=False)
        tmp.unlink()
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
