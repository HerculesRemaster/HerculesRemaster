"""Spawn kind -> type-table entry -> handler names for one ENG1 level (L*.RE.01 / L*.RE.02).

    level_types.py LEVEL_DIR SLUG      e.g.  level_types.py FOREST L03
Writes extracted/meta/full-game/<SLUG>-types.json from
  extracted/full-game/levels/<LEVEL_DIR>/level.json   spawns (kind, template flags, world position)
  extracted/meta/full-game/eng1-level-config.json      per-level config 0x68af0 (S2.05)
  extracted/meta/full-game/animation-coverage.json     static bank lists (S4.01)
  ENG1.DLL from backups/originals (pefile)             table entries
  ghidra-mcp get_functions                             current handler names (optional; RVAs only without it)
Class = kind & 0x7000 (spawn dispatch ENG1 0x262a0 / 0x26480):
  actor   0       0x270c0: config +0x1c entry kind*12 = {spawn, stub, update}; +0x18 kind*16 anim
  marker  0x4000  0x27720: config +0x24 entry (kind & 0xfff)*8 = {init, tick}; +0x20 anim (checkpoints 0x400a..)
  pickup  0x2000  0x27c40: config +0x2c entry (kind & 0xfff)*8 = {init, tick}; +0x28 anim (also effects/children)
  other   0x1000, 0x3000, ... not spawned by the dispatchers (read by other code; consumer open)
Bank per kind: the anim-table byte (actors +0xc, markers/pickups +6) becomes obj+0x22, the sprite bank id
(TRAINING bird 0x38 -> 46 STYBIRD, FOREST Nessus 0x2f -> 68 NESSUS\\Basic); 0 = no bank (model or code-drawn).
Names are LEAD unless a handler is named in Ghidra from its code.
"""
from __future__ import annotations

import json
import struct
import sys
from collections import Counter
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "ghidra"))
sys.path.insert(0, str(ROOT / "tools" / "level"))
from render_preview import kind_name  # noqa: E402


def names(addrs: list[int]) -> dict[int, str]:
    try:
        import mcp_http as m
        out = {}
        for i in range(0, len(addrs), 20):
            chunk = addrs[i:i + 20]
            r = m.get("get_functions", "ENG1.DLL", functions=",".join(f"0x{a:x}" for a in chunk))
            for k, v in r.get("functions", {}).items():
                if isinstance(v, dict) and v.get("name"):
                    out[int(k, 16)] = v["name"]
        return out
    except Exception as e:  # noqa: BLE001 - Ghidra not running: RVAs only
        print(f"(no Ghidra names: {e})")
        return {}


CAT_RULES = [("BOSS\\", "boss_animation"), ("NESSUS", "boss_animation"), ("PEGASUS", "flight_animation"),
             ("THEBIANS", "npc_animation"), ("\\PHIL", "npc_animation"), ("FANGIRLS", "npc_animation"),
             ("\\Meg", "npc_animation"), ("Aphrod", "npc_animation"), ("Apollo", "npc_animation"),
             ("\\Ares", "npc_animation"), ("Athena", "npc_animation"), ("Bacchus", "npc_animation"),
             ("\\Zeus", "npc_animation"), ("FPHIL", "npc_animation"), ("\\Water", "fx_projectiles"),
             ("RcRock", "fx_projectiles"), ("RtRock", "fx_projectiles"), ("TRAINFX", "fx_projectiles"),
             ("XTRA_DB1", "fx_projectiles")]


def category(cls: str, bank_path: str | None) -> str | None:
    """Scene-asset category (tools/meta/scene_assets.py) from class and bank path: a naming rule, LEAD."""
    if cls in ("marker", "pickup"):
        return "props_pickups"
    if cls != "actor":
        return None
    if not bank_path:
        return "props_pickups"
    for key, cat in CAT_RULES:
        if key.lower() in bank_path.lower():
            return cat
    return "enemy_animation"


def main(argv: list[str]) -> int:
    level_dir, slug = argv[1], argv[2]
    L = json.loads((ROOT / "extracted/full-game/levels" / level_dir / "level.json").read_text(encoding="utf-8"))
    cfg = json.loads((ROOT / "extracted/meta/full-game/eng1-level-config.json").read_text(encoding="utf-8"))
    cov = json.loads((ROOT / "extracted/meta/full-game/animation-coverage.json").read_text(encoding="utf-8"))
    bank_by_id = {b["id"]: b for b in cov["banks"] if b["engine"] == "ENG1"}
    lv = next(k for k, v in cfg["levels"].items() if v["name"] == level_dir)
    c = cfg["levels"][lv]
    pe = pefile.PE(str(ROOT / "backups" / "originals" / "ENG1.DLL"), fast_load=True)
    img = pe.get_memory_mapped_image()
    rd = lambda rva, n: img[rva:rva + n]                                   # noqa: E731
    p = lambda f: int(c[f], 16) if isinstance(c[f], str) else 0           # noqa: E731
    spawns = [s for sec in L["sections"] for s in sec["spawns"] if "kind" in s]
    count = Counter(s["kind"] for s in spawns)
    rows = []
    for kind in sorted(count):
        cls_bits = kind & 0x7000
        if cls_bits in (0x2000, 0x4000):
            cls = "pickup" if cls_bits == 0x2000 else "marker"
            ft, at, i = p(f"{cls}_kind_funcs"), p(f"{cls}_kind_anim"), kind & 0xFFF
            f = struct.unpack("<II", rd(ft + 8 * i, 8)) if ft else (0, 0)
            entry = {"init": f[0], "tick": f[1]}
            anim = struct.unpack("<I", rd(at + 8 * i, 4))[0] if at else 0
            bank = rd(at + 8 * i + 6, 1)[0] if at else None          # obj+0x22 = bank id (0x27720/0x27c40)
        elif cls_bits:
            cls, entry, anim, bank = "other", {}, 0, None
        else:
            cls, ft, at, i = "actor", p("actor_kind_funcs"), p("actor_kind_anim"), kind
            f = struct.unpack("<III", rd(ft + 12 * i, 12)) if ft else (0, 0, 0)
            entry = {"spawn": f[0], "stub": f[1], "update": f[2]}
            anim = struct.unpack("<I", rd(at + 16 * i, 4))[0] if at else 0
            bank = rd(at + 16 * i + 0xC, 1)[0] if at else None       # obj+0x22 = bank id (0x270c0)
        xs = sorted(s["wx"] for s in spawns if s["kind"] == kind)
        rows.append({"kind": f"0x{kind:04x}", "class": cls, "count": count[kind], "name": kind_name(kind) or None,
                     "entry": entry, "anim_script": anim, "bank": bank, "flags": sorted({s["flags"] for s in spawns if s["kind"] == kind}),
                     "x_range": [xs[0], xs[-1]]})
    # actor kinds whose handlers no other level's type table uses (spawned by code, e.g. LVL_InitLevelActors 0x1fd20)
    def actor_table(lvk):
        ft = int(cfg["levels"][lvk]["actor_kind_funcs"], 16)
        out = {}
        for k in range(0x100):
            f = struct.unpack("<3I", rd(ft + 12 * k, 12))
            if not all(0x10001000 <= x < 0x10050000 for x in f):
                break
            out[k] = f
        return out
    mine = actor_table(lv)
    others = {x for o in cfg["levels"] if o != lv for f in actor_table(o).values() for x in (f[0], f[2])}
    mapped = {int(r["kind"], 16) for r in rows}
    for k, f in mine.items():
        if k in mapped or (f[0] in others and f[2] in others) or (f[0] == 0x1000C720 and f[2] == 0x1000C720):
            continue
        rows.append({"kind": f"0x{k:04x}", "class": "actor", "count": 0, "name": None, "source": "code (not in the map)",
                     "entry": {"spawn": f[0], "stub": f[1], "update": f[2]}, "anim_script": 0,
                     "bank": rd(int(cfg["levels"][lv]["actor_kind_anim"], 16) + 16 * k + 0xC, 1)[0],
                     "flags": [], "x_range": None})
    addrs = sorted({a for r in rows for a in r["entry"].values() if a} | {r["anim_script"] for r in rows if r["anim_script"]})
    nm = names([a for a in addrs if 0x10001000 <= a < 0x10050000])
    for r in rows:
        r["entry"] = {k: {"rva": f"0x{a - 0x10000000:x}" if a else None, "name": nm.get(a)} for k, a in r["entry"].items()}
        r["anim_script"] = f"0x{r['anim_script'] - 0x10000000:x}" if r["anim_script"] else None
        b = bank_by_id.get(r.get("bank"))
        r["bank"] = {"id": r.get("bank"), "path": b["path"] if b else None} if r.get("bank") else None
        hnames = " ".join(v.get("name") or "" for v in r["entry"].values())
        r["category"] = ("boss_animation" if any(n in hnames for n in ("Hydra", "Medusa", "Hades", "Nessus"))
                         and r["class"] == "actor" else category(r["class"], (r["bank"] or {}).get("path")))
        r["category_rule"] = "class + bank path (tools/level/level_types.py CAT_RULES; LEAD)"
    lvid = int(lv)
    # start points: ENG1 0x5a690 + 0x18 * id, +0xc -> 0x18-byte records read by HERO_Init 0x15090 / 0x374c0:
    # u16 x, u16 y, u8 z (<< 0x17), ..., +8 ptr bank list (S4.01)
    rec = 0x1005A690 + 0x18 * lvid
    starts_va = struct.unpack("<I", img[rec - 0x10000000 + 0xC:rec - 0x10000000 + 0x10])[0]
    n_starts = len(cov["eng1_levels"].get(lv, {}).get("start_lists") or [])
    starts = []
    for k in range(n_starts):
        r = starts_va - 0x10000000 + 0x18 * k
        x, y, z = struct.unpack_from("<HHB", img, r)
        starts.append({"index": k, "x": x, "y": y, "z": z << 7, "raw": bytes(img[r:r + 0x18]).hex()})
    banks = [{"id": b["id"], "path": b["path"], "archive_index": b["archive_index"]} for b in cov["banks"]
             if b["engine"] == "ENG1" and level_dir in b["levels"]]
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/level/level_types.py.",
           "level": {"id": lvid, "dir": level_dir, "config_record": c["record_rva"]},
           "config": {k: c[k] for k in c if k not in ("name",)},
           "starts": {"table_rva": f"0x{starts_va - 0x10000000:x}", "points": starts},
           "start_and_level_banks": {"level_list": cov["eng1_levels"].get(lv, {}).get("level_list"),
                                     "start_lists": cov["eng1_levels"].get(lv, {}).get("start_lists"), "banks": banks},
           "kinds": rows}
    out = ROOT / "extracted/meta/full-game" / f"{slug}-types.json"
    out.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    named = sum(1 for r in rows if any(v["name"] and not v["name"].startswith("FUN_") for v in r["entry"].values()))
    print(f"{slug} {level_dir}: {len(rows)} kinds, {len(spawns)} spawns, {named} with named handlers -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
