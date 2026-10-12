"""Kind -> type-table entry -> handler names for the ENG3 levels (L02/L07/L09 RE.02).

    eng3_types.py            writes extracted/meta/full-game/{L02,L07,L09}-types.json
Static (ENG3.DLL from backups/originals):
  config      table 0x3b900, 0x48-byte records keyed by u16 level id (999 ends), copied to globals by
              LVL_SetLevelConfig 0x1d9b0 (gs+0 lookup). Same layout as ENG1 0x68af0:
              +0x18 actor anim (16 per kind) / +0x1c actor funcs (12: spawn, stub, update)
              +0x20 marker anim (8) / +0x24 marker funcs (8: init, tick)
              +0x28 pickup anim (8) / +0x2c pickup funcs (8: init, tick)
              +0x30 +0x34 +0x38 +0x40 +0x44 tables (consumers open; ENG1: anim events, attribute funcs, spawn
              templates, attribute flags)
  spawn       LVL_SpawnActor 0x1c9f0 (kind & 0x7000 == 0), LVL_SpawnMarker 0x1d050 (0x4000), LVL_SpawnPickup
              0x1d570 (0x2000): handler = funcs[kind & 0xfff], obj+0x22 bank = actor anim +0xc / marker,pickup +6
Kinds listed: every table entry whose handlers differ from the table's most common (stub) entry.
Runtime (hook "spawn" kind / "anim" type+bank events after a level_load of that level, every trace under
analysis/traces/**): spawns seen, banks and frames drawn. obj+0x22 can change in the init handler, so the
drawn bank can differ from the table byte. Names are LEAD unless the handler is named in Ghidra from its code.
Map placement: the track spawn lists in extracted/full-game/levels/<NAME>/level.json (type = template +8;
LVL_InitSpawnLists 0x1c010): count and wx range per type, and the tracks it appears on.
Identification: analysis/exports/full-game/eng3_kind_handlers.json (what / evidence / damage / gives, from the
handler decompilation; evidence grades the naming only). "category" (scene-asset category, tools/meta/scene_assets.py)
is derived from that text by the keyword rule CATEGORY_RULES below, first match wins.
"""
from __future__ import annotations

import json
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "level"))
sys.path.insert(0, str(ROOT / "tools" / "ghidra"))
from eng3_decode import eng3_rva_reader  # noqa: E402

LEVELS = {2: ("GAUNTLET", "L02"), 8: ("CYCLOPS", "L07"), 10: ("TORMENT", "L09")}
CODE = range(0x10001000, 0x10030000)


CATEGORY_RULES = [
    ("props_pickups", ("bonus item", "bonus figure")), ("map_layers_tiles", ("backdrop",)), ("checkpoint_state", ("checkpoint",)), ("collision_camera", ("camera marker",)), ("hero_animation", ("hero, pool a",)),
    ("fx_projectiles", ("shadow", "dust", "debris", "sparkle", "explosion", "rope sprite", "spitting fire")),
    ("boss_animation", ("cyclops",)), ("enemy_animation", ("skeleton", "dummy", "dummies", "shark", "beast", "swooper", "flyer")),
    ("npc_animation", ("citizen", "goat", "phil", "soul", "spirit", "bystander", "crowd")),
    ("props_pickups", ("bonus", "scenery", "decoration", "cloth", "gate", "raft", "street section", "end post")),
]


def category(info):
    if not info:
        return None
    w = info.get("what", "").lower()
    for cat, keys in CATEGORY_RULES:
        if any(k in w for k in keys):
            return cat
    return "chase_hazards" if info.get("damage") else None


def names(addrs):
    try:
        import mcp_http as m
        out = {}
        for i in range(0, len(addrs), 20):
            r = m.get("get_functions", "ENG3.DLL", functions=",".join(f"0x{a:x}" for a in addrs[i:i + 20]))
            for k, v in r.get("functions", {}).items():
                if isinstance(v, dict) and v.get("name"):
                    out[int(k, 16)] = v["name"]
        return out
    except Exception as e:  # noqa: BLE001 - Ghidra not running: RVAs only
        print(f"(no Ghidra names: {e})")
        return {}


def config(rd):
    out, o = {}, 0x3B900
    while True:
        r = rd(o, 0x48)
        lv = struct.unpack_from("<H", r, 0)[0]
        if lv == 999:
            return out
        out[lv] = {"record_rva": f"0x{o:x}", "u16_02_16": [struct.unpack_from("<H", r, i)[0] for i in range(2, 0x18, 2)],
                   **{k: struct.unpack_from("<I", r, off)[0] for k, off in (
                       ("actor_anim", 0x18), ("actor_funcs", 0x1C), ("marker_anim", 0x20), ("marker_funcs", 0x24),
                       ("pickup_anim", 0x28), ("pickup_funcs", 0x2C), ("t30", 0x30), ("t34", 0x34), ("t38", 0x38),
                       ("t3c", 0x3C), ("t40", 0x40), ("t44", 0x44))}}
        o += 0x48


def table(rd, va, width):
    rows = []
    while True:
        f = struct.unpack(f"<{width}I", rd(va - 0x10000000 + 4 * width * len(rows), 4 * width))
        if not all(x in CODE for x in f):
            return rows
        rows.append(f)


def runtime():
    seen = defaultdict(lambda: defaultdict(lambda: {"spawns": 0, "banks": defaultdict(set)}))
    runs = defaultdict(set)
    for f in (ROOT / "analysis" / "traces").rglob("resource-usage.jsonl"):
        lv = None
        for line in f.open(encoding="utf-8", errors="replace"):
            if '"level_load"' in line or '"module_load"' in line:
                e = json.loads(line)
                lv = e.get("level") if e["ev"] == "level_load" and e.get("engine") == "ENG3" else None
                continue
            if lv not in LEVELS:
                continue
            if '"ev":"spawn"' in line:
                e = json.loads(line)
                seen[lv][int(e["kind"], 16)]["spawns"] += 1
                runs[lv].add(str(f.parent.relative_to(ROOT)).replace("\\", "/"))
            elif '"ev":"anim"' in line:
                e = json.loads(line)
                seen[lv][e["type"]]["banks"][e["bank"]].add(e["frame"])
    return seen, runs


def main() -> int:
    rd = eng3_rva_reader()
    cfg = config(rd)
    cov = json.loads((ROOT / "extracted/meta/full-game/animation-coverage.json").read_text(encoding="utf-8"))
    bank_path = {b["id"]: b["path"] for b in cov["banks"] if b["engine"] == "ENG3"}
    seen, runs = runtime()
    hf = ROOT / "analysis/exports/full-game/eng3_kind_handlers.json"
    ident = json.loads(hf.read_text(encoding="utf-8")) if hf.exists() else {}
    cy = ROOT / "analysis/exports/full-game/eng3_cyclops.json"          # L07 Cyclops and its children
    if cy.exists():
        ident.setdefault("L07", {}).update(json.loads(cy.read_text(encoding="utf-8"))["kinds"])
    for lv, (name, slug) in LEVELS.items():
        c = cfg[lv]
        lvl = json.loads((ROOT / "extracted/full-game/levels" / name / "level.json").read_text(encoding="utf-8"))
        placed = defaultdict(list)
        for t in lvl["tracks"]:
            for sp in t["spawns"]:
                placed[int(sp["type"], 16)].append((t["index"], sp["wx"]))
        rows = []
        for cls, width, base, anim_w, bank_off in (("actor", 3, 0, 16, 0xC), ("marker", 2, 0x4000, 8, 6), ("pickup", 2, 0x2000, 8, 6)):
            funcs = table(rd, c[f"{cls}_funcs"], width)
            stub = Counter(funcs).most_common(1)[0][0] if funcs else None
            for i, f in enumerate(funcs):
                kind = base | i
                rt = seen[lv].get(kind)
                pl = placed.get(kind, [])
                if f == stub and not rt and not pl:
                    continue
                a = rd(c[f"{cls}_anim"] - 0x10000000 + anim_w * i, anim_w)
                roles = ("spawn", "stub", "update") if width == 3 else ("init", "tick")
                rows.append({"kind": f"0x{kind:04x}", "class": cls, "entry": dict(zip(roles, f)),
                             "anim_entry": a.hex(), "bank": a[bank_off],
                             "runtime": {"spawns": rt["spawns"], "drawn": {bank_path.get(b, str(b)): len(fr) for b, fr in rt["banks"].items()}}
                             if rt else None, "stub_entry": f == stub,
                             "map": {"count": len(pl), "tracks": sorted({t for t, _ in pl}),
                                     "wx_range": [min(x for _, x in pl), max(x for _, x in pl)]} if pl else None})
        other = sorted(k for k in seen[lv] if not any(int(r["kind"], 16) == k for r in rows))
        unplaced = sorted(k for k in placed if not any(int(r["kind"], 16) == k for r in rows))
        addrs = sorted({a for r in rows for a in r["entry"].values()})
        nm = names(addrs)
        for r in rows:
            info = ident.get(slug, {}).get(r["kind"])
            if not info:                     # same handler code identified in another level
                mine = {f"0x{a - 0x10000000:x}" for a in r["entry"].values()} - {"0x2390"}
                info = next((v for lvk, kinds in ident.items() if lvk.startswith("L") for v in kinds.values()
                             if mine and mine <= set(v.get("handlers", {}))), None)
            if info:
                r["what"], r["evidence"] = info.get("what"), info.get("evidence")
                r["damage"], r["gives"] = info.get("damage"), info.get("gives")
            r["category"] = category(info)
            r["entry"] = {k: {"rva": f"0x{a - 0x10000000:x}", "name": nm.get(a)} for k, a in r["entry"].items()}
            r["bank"] = {"id": r["bank"], "path": bank_path.get(r["bank"])} if r["bank"] else None
        doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/level/eng3_types.py for the rules.",
               "level": {"id": lv, "dir": name, "config_record": c["record_rva"]},
               "config": {k: (f"0x{v - 0x10000000:x}" if isinstance(v, int) and v > 0x10000000 else v) for k, v in c.items()},
               "start_and_level_banks": cov.get("eng3_levels", {}).get(str(lv)),
               "runtime_runs": sorted(runs[lv]),
               "runtime_other_types": {f"0x{k:04x}": {bank_path.get(b, str(b)): len(fr) for b, fr in seen[lv][k]["banks"].items()}
                                       for k in other},
               "map_types_without_table_entry": [f"0x{k:04x}" for k in unplaced],
               "kinds": rows}
        out = ROOT / "extracted/meta/full-game" / f"{slug}-types.json"
        out.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
        named = sum(1 for r in rows if any(v["name"] and not v["name"].startswith("FUN_") for v in r["entry"].values()))
        print(f"{slug} {name}: {len(rows)} kinds ({sum(1 for r in rows if r['map'])} placed, "
              f"{sum(1 for r in rows if r['runtime'])} seen at runtime), "
              f"{named} with named handlers, other runtime types {[f'0x{k:x}' for k in other]} -> {out.name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
