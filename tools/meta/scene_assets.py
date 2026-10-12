"""Runtime asset use per level and per scenario (P4.ALL.07-08, L*.AS.06/AS.07).

    scene_assets.py
Reads every run under analysis/traces/full-game/<slug>/<scenario>/<run>/ (run.json + resource-usage.jsonl) and
writes
  extracted/meta/full-game/level-assets.json          one row per (level id, archive index) opened while that
                                                      level was loaded: name, id, category, consumer (dll+rva of
                                                      the open), first tick, runs, scenarios, decoder and export
  extracted/meta/full-game/scene-asset-coverage.json  per scenario: expected categories (scenario JSON) vs the
                                                      categories observed in its runs and whether they are exported
A level is "loaded" from its level_load event (ENG1/ENG3, field level) until the next module_load.
Observation rules (from events only; anything else stays "pending"):
  map_layers_tiles  MAP_PSX.BIN open            palette_clut   .CIF open
  collision_camera  MAP_PSX.BIN open (collision/camera live in the map; exported = level.json)
  hero_animation    anim events of a bank path containing \\HERO\\
  hud_score         an open whose name has PANEL or LETTERS
  sfx_esf_sonix     sfx_play / esf_decode events  music_transport  audio events AUD_StartMusic/AUD_StopMusic
  checkpoint_state  a state event whose restart index changes during the level
  sprite categories (chase_hazards, npc_animation, enemy_animation, props_pickups, fx_projectiles,
  boss_animation, flight_animation): "kinds_seen" lists the object kinds drawn; the category is marked observed
  only when the level's L*-types.json gives that kind a category (field "category"), else needs_classification.
"""
from __future__ import annotations

import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TR = ROOT / "analysis" / "traces" / "full-game"
SC = ROOT / ".plan" / "full-game-remaster" / "reference" / "scenarios"
META = ROOT / "extracted" / "meta" / "full-game"
SPRITE_CATS = {"chase_hazards", "npc_animation", "enemy_animation", "props_pickups", "fx_projectiles",
               "boss_animation", "flight_animation"}
ENG3 = {2: "GAUNTLET", 8: "CYCLOPS", 10: "TORMENT"}


def category(name: str) -> str:
    n = name.upper()
    if n.endswith("MAP_PSX.BIN"):
        return "map_layers_tiles"
    if n.endswith(".CIF"):
        return "palette_clut"
    if "PANEL" in n or "LETTERS" in n:
        return "hud_score"
    if n.endswith("ANIMPSX.BIN"):
        return "sprite_bank"
    if n.endswith(".ESF") or n.endswith(".SFX"):
        return "sfx_esf_sonix"
    if n.endswith(".MUS"):
        return "music_transport"
    return "other"


def decoder(cat: str, level: int) -> str | None:
    return {"map_layers_tiles": "tools/level/eng3_decode.py" if level in ENG3 else "tools/level/level_decode.py",
            "palette_clut": "tools/level/eng3_decode.py" if level in ENG3 else "tools/level/level_decode.py",
            "sprite_bank": "tools/decode/ifam.py (extracted/sprites/)", "hud_score": "tools/decode/ifam.py / raw VRAM sheet",
            "sfx_esf_sonix": "tools/decode/sonix.py", "music_transport": "tools/decode/sonix.py"}.get(cat)


def kind_categories(slug_id: str) -> dict[int, str]:
    f = META / f"{slug_id}-types.json"
    if not f.exists():
        return {}
    out = {}
    for k in json.loads(f.read_text(encoding="utf-8")).get("kinds", []):
        if k.get("category"):
            out[int(k["kind"], 16)] = k["category"]
    return out


def scan(run: Path):
    lv, last_restart = None, None
    per = defaultdict(lambda: {"opens": {}, "cats": set(), "kinds": set()})
    for line in (run / "resource-usage.jsonl").open(encoding="utf-8", errors="replace"):
        if not line.startswith("{"):
            continue
        try:
            e = json.loads(line)
        except json.JSONDecodeError:
            continue
        ev = e["ev"]
        if ev == "module_load":
            lv, last_restart = None, None
            continue
        if ev == "level_load":
            lv = e["level"]
        if lv is None:
            continue
        p = per[lv]
        if ev in ("open", "level_load", "level_palette") and e.get("index", -1) >= 0:
            name = e.get("name") or e.get("path")
            p["opens"].setdefault(e["index"], {"name": name, "id": e.get("id"), "first_tick": e["tick"],
                                               "consumer": e.get("dll") or e.get("loader")})
            c = category(name)
            if c == "map_layers_tiles":
                p["cats"].update({"map_layers_tiles", "collision_camera"})
            elif c in ("palette_clut", "hud_score"):
                p["cats"].add(c)
        elif ev in ("sfx_play", "esf_decode"):
            p["cats"].add("sfx_esf_sonix")
        elif ev == "audio" and e.get("call") in ("AUD_StartMusic", "AUD_StopMusic"):
            p["cats"].add("music_transport")
        elif ev == "anim":
            if "\\HERO\\" in (e.get("bank_name") or "").upper():
                p["cats"].add("hero_animation")
            else:
                p["kinds"].add(e.get("type"))
        elif ev == "state" and e.get("module") != "TITLE":
            if last_restart is not None and e["restart"] != last_restart:
                p["cats"].add("checkpoint_state")
            last_restart = e["restart"]
    return per


def main() -> int:
    scen = {}
    for f in SC.glob("*/*.json"):
        d = json.loads(f.read_text(encoding="utf-8"))
        scen[d["scenario_id"]] = d
    rows, cover = {}, {}
    for rj in sorted(TR.glob("*/*/*/run.json")):
        meta = json.loads(rj.read_text(encoding="utf-8"))
        sid = meta.get("scenario_id")
        if sid not in scen or not (rj.parent / "resource-usage.jsonl").exists():
            continue
        run_rel = str(rj.parent.relative_to(ROOT)).replace("\\", "/")
        debug = bool(meta.get("entry_method", {}).get("debug_only"))
        target = scen[sid].get("internal_level_id")
        for lv, p in scan(rj.parent).items():
            for idx, o in p["opens"].items():
                r = rows.setdefault((lv, idx), {"level": lv, "archive_index": idx, "name": o["name"], "id": o["id"],
                                                "category": category(o["name"]), "consumer": o["consumer"],
                                                "first_tick": o["first_tick"], "runs": [], "scenarios": []})
                r["decoder"] = decoder(r["category"], lv)
                if run_rel not in r["runs"]:
                    r["runs"].append(run_rel)
                if sid not in r["scenarios"]:
                    r["scenarios"].append(sid)
            if lv != target:
                continue
            c = cover.setdefault(sid, {"scenario_id": sid, "level": lv, "expected": scen[sid].get("expected_asset_categories", []),
                                       "runs": [], "debug_only_runs": [], "observed": set(), "kinds_seen": set()})
            (c["debug_only_runs"] if debug else c["runs"]).append(run_rel)
            c["observed"] |= p["cats"]
            c["kinds_seen"] |= {k for k in p["kinds"] if k is not None}
    lvl_dir = {}
    for d in scen.values():
        lvl_dir[d["internal_level_id"]] = d["level_id"]
    out_cov = []
    for sid, c in sorted(cover.items()):
        kc = kind_categories(lvl_dir.get(c["level"], ""))
        cats_from_kinds = {kc[k] for k in c["kinds_seen"] if k in kc}
        status = {}
        for cat in c["expected"]:
            if cat in c["observed"] or cat in cats_from_kinds:
                status[cat] = "observed"
            elif cat in SPRITE_CATS and c["kinds_seen"]:
                status[cat] = "needs_classification"
            else:
                status[cat] = "pending"
        out_cov.append({"scenario_id": sid, "level": c["level"], "runs": c["runs"], "debug_only_runs": c["debug_only_runs"],
                        "categories": status, "kinds_seen": [f"0x{k:04x}" for k in sorted(c["kinds_seen"])],
                        "missing": [k for k, v in status.items() if v != "observed"]})
    for sid, d in sorted(scen.items()):
        if sid not in cover:
            out_cov.append({"scenario_id": sid, "level": d.get("internal_level_id"), "runs": [], "debug_only_runs": [],
                            "categories": {k: "pending" for k in d.get("expected_asset_categories", [])}, "kinds_seen": [],
                            "missing": d.get("expected_asset_categories", [])})
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/meta/scene_assets.py for the rules.",
           "rows": sorted(rows.values(), key=lambda r: (r["level"], r["archive_index"]))}
    (META / "level-assets.json").write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    (META / "scene-asset-coverage.json").write_text(json.dumps({"_doc": doc["_doc"], "scenarios": out_cov}, indent=1) + "\n",
                                                    encoding="utf-8")
    run_sc = [c for c in out_cov if c["runs"] or c["debug_only_runs"]]
    print(f"{len(doc['rows'])} level-asset rows; {len(run_sc)}/{len(out_cov)} scenarios with runs; "
          f"categories observed {sum(v == 'observed' for c in out_cov for v in c['categories'].values())}/"
          f"{sum(len(c['categories']) for c in out_cov)}")
    for c in run_sc:
        print(f"  {c['scenario_id']:34} missing {c['missing']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
