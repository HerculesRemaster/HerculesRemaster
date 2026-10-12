"""Static digest of a level's actor handlers (L*.RE enemy tasks) -> extracted/meta/full-game/<SLUG>-enemies.json.

For every actor kind in <SLUG>-types.json with a spawn/update handler, decompiles both through ghidra-mcp and
extracts (regex over the decompiler text; VERIFIED-STATIC facts, meaning still needs reading):
  hits         HERO_RequestDamage(obj, N, ...) strengths N -> hero damage index N&7 (ENG1 0x55348 table; RequestDamage stores N+1,
               HERO_ApplyDamage 0x18c50 indexes (stored-1)&7; runtime: THEBES Minotaur N=3 -> Beginner drop 12)
  hp           constants written to +0xee (hit points) and HERO_EnemyInitHp calls (difficulty*2 + 6)
  hit_sources  HERO_FindHitSource / 0x20250 calls (the actor can be hit) and 0x19b40 (punch strength)
  difficulty   reads of game state +0xe
  states       case labels in the update (behaviour states +0x2a)
  sounds       constant FUN_10006da0 ids
  spawns       constant kinds passed to the spawn helpers 0x271d0 / 0x27db0 / 0x300c0
Damage per difficulty comes from the ENG1 table 0x55348 (Beginner/Medium/Herculean rows).

    enemy_digest.py SLUG
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

pe = pefile.PE(str(ROOT / "backups" / "originals" / "ENG1.DLL"), fast_load=True)
IMG = pe.get_memory_mapped_image()
DMG = [struct.unpack_from("<8i", IMG, 0x55348 + 32 * d) for d in range(3)]   # int[3][8]
NUM = r"(0x[0-9a-fA-F]+|\d+)"


def code(rva: str) -> str:
    try:
        r = m.get("force_decompile", "ENG1.DLL", function=f"0x{int(rva, 16) + 0x10000000:x}")
        return r.get("decompiled", "") if isinstance(r, dict) else str(r)
    except m.GhidraError:
        return ""


def main(argv: list[str]) -> int:
    slug = argv[1]
    t = json.loads((ROOT / f"extracted/meta/full-game/{slug}-types.json").read_text(encoding="utf-8"))
    rows = []
    for k in t["kinds"]:
        if k["class"] != "actor":
            continue
        src = "\n".join(code(e["rva"]) for r, e in k["entry"].items() if r != "stub" and e.get("rva") and e["rva"] != "0xc720")
        if not src:
            continue
        hits = sorted({int(x, 0) for x in re.findall(r"HERO_RequestDamage\([^,]+,\s*" + NUM, src)})
        hp = sorted({int(x, 0) for x in re.findall(r"\+ 0xee\) = " + NUM + ";", src)})
        rows.append({
            "kind": k["kind"], "count": k["count"], "bank": (k.get("bank") or {}).get("path"),
            "handlers": {r: e["name"] for r, e in k["entry"].items() if r != "stub"},
            "hits": [{"strength": h, "index": h & 7, "damage_beginner_medium_herculean": [DMG[d][h & 7] for d in range(3)]} for h in hits],
            "hp_constants": hp, "enemy_init_hp": "HERO_EnemyInitHp" in src or "FUN_10024270" in src,
            "can_be_hit": any(s in src for s in ("HERO_FindHitSource", "FUN_10020250", "FUN_10026de0")),
            "punch_strength": "FUN_10019b40" in src,
            "difficulty_reads": src.count("+ 0xe)"),
            "states": sorted({int(x, 0) for x in re.findall(r"case " + NUM + ":", src)})[:40],
            "sounds": sorted({f"0x{int(x, 0):x}" for x in re.findall(r"FUN_10006da0\(\(?(?:ushort\)|uint\))?" + NUM, src)}),
            "spawns": sorted({f"0x{int(x, 0):x}" for x in re.findall(r"FUN_10(?:0271d0|027db0|0300c0)\([^;]*?," + NUM + r"\)", src)}),
        })
    out = ROOT / f"extracted/meta/full-game/{slug}-enemies.json"
    out.write_text(json.dumps({"_doc": __doc__.split("\n\n")[0], "level": t["level"], "kinds": rows}, indent=1) + "\n",
                   encoding="utf-8")
    for r in rows:
        print(r["kind"], (r["bank"] or "-").split("\\")[-1], [h["damage_beginner_medium_herculean"] for h in r["hits"]],
              "hp", r["hp_constants"], "init_hp" if r["enemy_init_hp"] else "", "hit" if r["can_be_hit"] else "",
              "diff" if r["difficulty_reads"] else "", len(r["states"]), "states")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
