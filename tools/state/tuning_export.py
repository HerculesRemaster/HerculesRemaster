"""Difficulty-keyed constants -> extracted/meta/full-game/tuning.json (S6.07).

tuning_export.py

Schema (one entry per constant set): {"id", "program", "rva", "entity", "scenario" (level or "all"),
"representation" (how the bytes are read), "by_difficulty": {"Beginner": ..., "Medium": ..., "Herculean": ...},
"source" (code that reads it), "status"}. Values are read from the canonical binaries in
backups/originals/ in their original representation; nothing is converted.
Code-derived rules without a table (e.g. "enemy HP = difficulty*2 + 6") are listed with "formula".
"""
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
D = ["Beginner", "Medium", "Herculean"]


def reader(name):
    b = (ROOT / "backups" / "originals" / name).read_bytes()
    pe = struct.unpack_from("<I", b, 0x3c)[0]
    n = struct.unpack_from("<H", b, pe + 6)[0]
    opt = struct.unpack_from("<H", b, pe + 20)[0]
    secs, o = [], pe + 24 + opt
    for _ in range(n):
        vs, va, raw, ptr = struct.unpack_from("<IIII", b, o + 8)
        secs.append((va, vs, raw, ptr))
        o += 40

    def rd(rva, size):
        for va, vs, raw, ptr in secs:
            if va <= rva < va + max(vs, raw):
                return b[ptr + rva - va:ptr + rva - va + size]
        raise KeyError(hex(rva))
    return rd


def table(prog, rva, stride, fmt):
    rd = reader(prog)
    return {d: list(struct.unpack(fmt, rd(rva + i * stride, struct.calcsize(fmt)))) for i, d in enumerate(D)}


# HERO_ApplyDamage 0x19410 jump table 0x1957c: hit code -> damage columns of 0x380a0 (docs/full-game/gameplay/chase.md)
HIT_COLUMNS = {**{c: [0] for c in (4, 0x22, 0x23, 0x24, 0x25, 0x27, 0x34, 0x35)},
               **{c: [1] for c in (0x21, 0x26, 0x29, 0x2b, 0x2c, 0x2d, 0x80)},
               **{c: [2] for c in (0x28, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x36)},
               **{c: [3] for c in (2, 0x200, 0x400, 0x800)}, 0x100: [4], 0x40: [1, 0], 0x2a: [1, 1, 0],
               **{c: [] for c in (1, 8, 0x10, 0x20)}}


def main():
    e1 = "ENG1.DLL"
    out = [
        {"id": "eng1.hero_damage", "program": e1, "rva": "0x55348", "entity": "hero (ENG1 levels)", "scenario": "all ENG1",
         "representation": "s32 x 8 per difficulty (stride 0x20), index = hit strength - 1",
         "by_difficulty": table(e1, 0x55348, 0x20, "<8i"), "source": "HERO_ApplyDamage ENG1 0x18c50", "status": "VERIFIED-STATIC"},
        {"id": "eng1.enemy_hp", "program": e1, "rva": "0x24270", "entity": "enemy (ENG1 HERO_EnemyInitHp)", "scenario": "all ENG1",
         "representation": "formula", "formula": "obj+0xee = difficulty*2 + 6",
         "by_difficulty": {d: i * 2 + 6 for i, d in enumerate(D)}, "source": "HERO_EnemyInitHp ENG1 0x24270", "status": "VERIFIED-STATIC"},
        {"id": "eng3.hero_damage", "program": "ENG3.DLL", "rva": "0x380a0", "entity": "hero (runner levels)", "scenario": "L02 L07 L09",
         "representation": "u32 x 8 per difficulty (stride 0x20)",
         "by_difficulty": table("ENG3.DLL", 0x380a0, 0x20, "<8I"), "source": "HERO_ApplyDamage ENG3 0x19410", "status": "VERIFIED-STATIC"},
        {"id": "title.tally_extra_life", "program": "TITLE.DLL", "rva": "0x1d0f0..0x1d127", "entity": "tally screen", "scenario": "all",
         "representation": "score threshold for +1 life (lives < 9)",
         "by_difficulty": {"Beginner": "0x28", "Medium": "0x3c", "Herculean": "0x50"},
         "source": "HERO_TallyScreen TITLE 0x1c500 (docs/full-game/gameplay/progression.md)", "status": "VERIFIED-STATIC"},
        {"id": "title.beginner_end", "program": "TITLE.DLL", "rva": "0x167c8", "entity": "progression", "scenario": "L08 end",
         "representation": "rule", "by_difficulty": {"Beginner": "ending after level 9 (TITANS)", "Medium": "continue to 10",
                                                       "Herculean": "continue to 10"},
         "source": "MOD_FrontEndStateMachine; runtime: Beginner password slots 9-11 -> ending", "status": "VERIFIED-STATIC + RUNTIME (password path)"},
    ]
    # per-level ENG1 tables found by session gamehercules-22 (pointers selected by gs+0xe)
    for ident, rvas, src, scen in (("eng1.hydra_tables", (0x56048, 0x560e0, 0x56168), "ENG1 0x1ac10 -> 0x6f3cc", "L05 HYDRA"),
                                   ("eng1.vortex_tables", (0x69180, 0x69220, 0x692b8), "ENG1 0x44120 -> 0x70198/0x7018c", "L10 VORTEX")):
        out.append({"id": ident, "program": e1, "rva": [hex(r) for r in rvas], "entity": scen.split()[1].lower() + " boss",
                    "scenario": scen, "representation": "table address per difficulty (contents: see the level plan)",
                    "by_difficulty": {d: hex(r) for d, r in zip(D, rvas)}, "source": src, "status": "VERIFIED-STATIC (gamehercules-22)"})
    out.append({"id": "eng1.nessus_attacks", "program": e1, "rva": "0xbc50", "entity": "Nessus (FOREST kind 0x2f)", "scenario": "L03",
                "representation": "formula", "formula": "attack count = rnd(3) + 1 + difficulty",
                "by_difficulty": {d: f"rnd(3)+{1 + i}" for i, d in enumerate(D)}, "source": "HERO_NessusUpdate ENG1 0xbc50",
                "status": "VERIFIED-STATIC (gamehercules-22)"})
    # ENG3 per-kind hit codes (L02/L07/L09 GP.06): the object's HERO_Hit code picks the damage column of 0x380a0
    # (jump table 0x1957c, docs/full-game/gameplay/chase.md); identification from L*-types.json
    import re
    t3 = table("ENG3.DLL", 0x380a0, 0x20, "<8I")
    for slug, lvl in (("L02", "GAUNTLET"), ("L07", "CYCLOPS"), ("L09", "TORMENT")):
        f = ROOT / "extracted" / "meta" / "full-game" / f"{slug}-types.json"
        if not f.exists():
            continue
        for k in json.loads(f.read_text(encoding="utf-8"))["kinds"]:
            dmg = k.get("damage") or ""
            mm = re.search(r"hit code (0x[0-9a-f]+)", dmg)
            if not mm or not (k.get("map") or k.get("runtime")):
                continue
            cols = HIT_COLUMNS.get(int(mm.group(1), 16))
            if cols is None:
                continue
            out.append({"id": f"eng3.{slug}.kind{k['kind']}.damage", "program": "ENG3.DLL", "rva": "0x380a0",
                        "entity": (k.get("what") or k["kind"])[:80], "scenario": f"{slug} {lvl}",
                        "representation": f"hit code {mm.group(1)} -> columns {'+'.join('T%d' % c for c in cols)} of u32 table 0x380a0",
                        "by_difficulty": {d: sum(t3[d][c] for c in cols) for d in D},
                        "source": ", ".join(v["name"] or v["rva"] for r, v in k["entry"].items() if r != "stub"),
                        "status": "VERIFIED-STATIC (handler decompilation; identification " + (k.get("evidence") or "LEAD") + ")"})
    dst = ROOT / "extracted" / "meta" / "full-game" / "tuning.json"
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(json.dumps({"schema_version": 1, "difficulty_field": "game state +0x0e (0 Beginner, 1 Medium, 2 Herculean)",
                               "entries": out}, indent=1), encoding="utf-8")
    print(f"{len(out)} entries -> {dst.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
