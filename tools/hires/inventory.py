"""Per-level art inventory (full-game S7.01) -> extracted/meta/full-game/art-inventory/<DIR>.json.

inventory.py [--index working/game/art/dump/index.csv]

Input: the GPU renderer's art_dump catalog index (gpu_renderer.cpp ArtWorker: one row per key at its first draw:
key, w, h, tpage, alt, u, v, screen_x, screen_y, frame, module) and the key -> source links below.
Every key is FNV-1a 64 of (w, h, w x h VRAM texels) (gpu_renderer.cpp BlockKey, contract.fnv_batch).
Links, in order:
  map     the block recomputed from a level's offline VRAM (extracted/full-game/levels/<DIR>/vram.bin, 1024 x 512
          16-bit units = 2048 x 512 8-bit texels) at x = (tpage & 15) * 128 + u, y = (tpage >> 4 & 1) * 256 + v
          gives the same key -> that level's map texture at (x, y). Exact content match, no guessing.
  sprite  remastered/manifest.json (tools/hires/contract.py) sprite part keys -> bank / frame / part rect;
          the bank's levels come from extracted/meta/full-game/animation-coverage.json (static level lists).
  screen / ui / level   the other manifest families (TGA screens, UI, ENG1 level meshes).
  hud     alt = 1 rows address the second VRAM half (y + 512): the ENG3 HUD sheet STPANEL2.DAT (archive 575,
          128 x 512 units uploaded at (0, 512) by GFX_LoadPanel2 0x25060) recomputed the same way.
  upload  runtime VRAM uploads captured as decoder pairs (analysis/traces/full-game/**/pairs/s*_vram_*/upload.bin
          with their rect, source archive index and offset): the block recomputed from the composed uploads of
          each traced scene (level from pair.json) gives the key -> that upload's source file.
Keys drawn by a level engine but linked to nothing are listed per engine under "_unlinked_<ENGINE>.json"
(for example runtime-composed VRAM such as HUD digits or animated textures). The index has no tick or level
column, only the presented frame number and module, so scenes are attributed by the links above.
"""
import argparse
import csv
import json
from collections import defaultdict
from pathlib import Path

import numpy as np

import sys
sys.path.insert(0, str(Path(__file__).parent))
from contract import fnv_batch  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
LV = ROOT / "extracted" / "full-game" / "levels"
OUT = ROOT / "extracted" / "meta" / "full-game" / "art-inventory"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--index", default=str(ROOT / "working/game/art/dump/index.csv"))
    a = ap.parse_args(argv)
    rows = {}
    with open(a.index, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f):
            if r.get("key") and r["key"] != "key":
                rows.setdefault(r["key"], r)
    # map links
    vrams = {}
    for d in sorted(LV.iterdir()):
        v = d / "vram.bin"
        if v.exists():
            vrams[d.name] = np.frombuffer(v.read_bytes(), np.uint8).reshape(512, 2048)
    link = defaultdict(list)
    by_shape = defaultdict(list)
    for k, r in rows.items():
        by_shape[(int(r["w"]), int(r["h"]))].append(k)
    for name, img in vrams.items():
        for (w, h), keys in by_shape.items():
            blocks, ks = [], []
            for k in keys:
                r = rows[k]
                t, u, v = int(r["tpage"]), int(r["u"]), int(r["v"])
                x, y = (t & 15) * 128 + u, ((t >> 4) & 1) * 256 + v
                if x + w <= 2048 and y + h <= 512:
                    blocks.append(img[y:y + h, x:x + w].reshape(-1)); ks.append((k, x, y))
            if not blocks:
                continue
            got = fnv_batch(w, h, np.stack(blocks))
            for (k, x, y), g in zip(ks, got):
                if f"{g:016x}" == k:
                    link[k].append({"family": "map", "level": name, "vram_xy": [x, y]})
    # HUD sheet (alt = 1 -> y + 512)
    arch = {e["index"]: e for e in json.loads((ROOT / "manifests/archive-manifest.json").read_text(encoding="utf-8"))["entries"]}
    panel = np.frombuffer((ROOT / arch[575]["file"]).read_bytes(), np.uint8).reshape(512, 256)
    for (w, h), keys in by_shape.items():
        cand = []
        for k in keys:
            r = rows[k]
            if r.get("alt") != "1" or link.get(k):
                continue
            t, u, v = int(r["tpage"]), int(r["u"]), int(r["v"])
            x, y = (t & 15) * 128 + u, ((t >> 4) & 1) * 256 + v
            if x + w <= 256 and y + h <= 512:
                cand.append((k, x, y))
        if cand:
            got = fnv_batch(w, h, np.stack([panel[y:y + h, x:x + w].reshape(-1) for _, x, y in cand]))
            for (k, x, y), g in zip(cand, got):
                if f"{g:016x}" == k:
                    link[k].append({"family": "hud", "archive_index": 575, "name": arch[575]["name_candidate"],
                                    "sheet_xy": [x, y], "levels": ["GAUNTLET", "CYCLOPS", "TORMENT"]})
    # captured runtime uploads, composed per traced scene
    scenes = defaultdict(list)
    for pj in (ROOT / "analysis/traces/full-game").rglob("pairs/s*_vram_*/pair.json"):
        q = json.loads(pj.read_text())
        scenes[(pj.parent.parent, q["scene"], q.get("level"))].append((pj, q))
    names = {2: "GAUNTLET", 8: "CYCLOPS", 10: "TORMENT", 1: "TRAINING", 3: "FOREST", 5: "THEBES", 6: "HYDRA", 7: "MEDUSA",
             9: "TITANS", 11: "VORTEX"}
    for (_, _, lvl), ups in scenes.items():
        if lvl not in names:
            continue
        img = np.zeros((1024, 2048), np.uint8)
        src = np.full((1024, 2048), -1, np.int32)
        for i, (pj, q) in enumerate(sorted(ups, key=lambda z: z[0].parent.name)):
            r = q["rect"]
            b = np.frombuffer((pj.parent / "upload.bin").read_bytes(), np.uint8)
            rows_ = len(b) // (r["w16"] * 2)
            img[r["y"]:r["y"] + rows_, r["x"] * 2:r["x"] * 2 + r["w16"] * 2] = b[:rows_ * r["w16"] * 2].reshape(rows_, -1)
            src[r["y"]:r["y"] + rows_, r["x"] * 2:r["x"] * 2 + r["w16"] * 2] = i
        order = sorted(ups, key=lambda z: z[0].parent.name)
        for (w, h), keys in by_shape.items():
            cand = []
            for k in keys:
                r = rows[k]
                if link.get(k):
                    continue
                t, u, v = int(r["tpage"]), int(r["u"]), int(r["v"])
                x, y = (t & 15) * 128 + u, ((t >> 4) & 1) * 256 + v + (512 if r.get("alt") == "1" else 0)
                if x + w <= 2048 and y + h <= 1024 and src[y, x] >= 0:
                    cand.append((k, x, y))
            if cand:
                got = fnv_batch(w, h, np.stack([img[y:y + h, x:x + w].reshape(-1) for _, x, y in cand]))
                for (k, x, y), g in zip(cand, got):
                    if f"{g:016x}" == k:
                        q = order[src[y, x]][1]
                        link[k].append({"family": "upload", "level": names[lvl], "archive_index": q.get("from_index"),
                                        "from_pos": q.get("from_pos"), "vram_xy": [x, y]})
    # manifest links
    cov = json.loads((ROOT / "extracted/meta/full-game/animation-coverage.json").read_text(encoding="utf-8"))
    bank_levels = {b["archive_index"]: b["levels"] for b in cov["banks"] if b.get("archive_index") is not None}
    man = json.loads((ROOT / "remastered/manifest.json").read_text(encoding="utf-8"))
    for e in man["entries"]:
        src = e.get("source", {})
        for b in e.get("geometry", {}).get("blocks", []):
            k = b.get("key")
            if k in rows:
                ent = {"family": e["family"], "id": e["id"], "rect": b.get("rect")}
                if e["family"] == "sprite":
                    ent.update({"bank": src.get("name"), "archive_index": src.get("index"), "frame": src.get("frame"),
                                "levels": bank_levels.get(src.get("index"), [])})
                link[k].append(ent)
    per = defaultdict(dict)
    for k, r in rows.items():
        ls = link.get(k, [])
        item = {"key": k, "w": int(r["w"]), "h": int(r["h"]), "file": f"{k}_{r['w']}x{r['h']}.png", "module": r["module"],
                "first_frame": int(r["frame"]), "screen_xy": [int(r["screen_x"]), int(r["screen_y"])], "seen": True,
                "sources": ls}
        levels = ({l["level"] for l in ls if l["family"] in ("map", "upload")}
                  | {lv for l in ls if l["family"] in ("sprite", "hud") for lv in l["levels"]})
        if levels:
            for lv in levels:
                per[lv][k] = item
        elif ls:
            per["_shared_" + ls[0]["family"]][k] = item
        else:
            per[f"_unlinked_{r['module']}"][k] = item
    # static completion: every part key of every frame of the level's banks (manifest), drawn or not, so routes and
    # frames the runs did not reach are inventoried too ("seen": false)
    lvl_banks = defaultdict(set)
    for b in cov["banks"]:
        for lv in b.get("levels", []):
            if b.get("archive_index") is not None:
                lvl_banks[lv].add(b["archive_index"])
    for e in man["entries"]:
        if e["family"] != "sprite":
            continue
        idx = e["source"].get("index")
        for lv, banks in lvl_banks.items():
            if idx not in banks:
                continue
            for b in e.get("geometry", {}).get("blocks", []):
                k = b.get("key")
                if k and k not in per[lv]:
                    w, h = b["rect"][2], b["rect"][3]
                    per[lv][k] = {"key": k, "w": w, "h": h, "file": f"{k}_{w}x{h}.png", "module": None, "first_frame": None,
                                  "screen_xy": None, "seen": False,
                                  "sources": [{"family": "sprite", "id": e["id"], "rect": b["rect"], "bank": e["source"].get("name"),
                                               "archive_index": idx, "frame": e["source"].get("frame")}]}
    # static ENG3 map keys: every textured cell-mesh face (level.json "mesh_textures", tools/level/eng3_decode.py)
    # gives the rects the renderer can key, as contract.level_entries does for ENG1: UV bounding box, exact 1:1
    # rect and the mirrored rect; texels from vram.bin at x = (tpage & 15) * 128 + u, y = (tpage >> 4 & 1) * 256 + v
    check = {}
    for name in ("GAUNTLET", "CYCLOPS", "TORMENT"):
        lj = LV / name / "level.json"
        if not lj.exists() or name not in vrams:
            continue
        img = vrams[name]
        rects = set()
        for t in json.loads(lj.read_text(encoding="utf-8")).get("mesh_textures", []):
            tp = t["tpage"]
            us, vs = [uv[0] for uv in t["uv"]], [uv[1] for uv in t["uv"]]
            bx, by = (tp & 15) * 128, ((tp >> 4) & 1) * 256
            u0, u1, v0, v1 = min(us), max(us), min(vs), max(vs)
            rects.add((bx + u0, by + v0, u1 - u0 + 1, v1 - v0 + 1))
            if u1 > u0 and v1 > v0:
                rects.add((bx + u0, by + v0, u1 - u0, v1 - v0))
                rects.add((bx + u0 + 1, by + v0, u1 - u0, v1 - v0))
        groups = defaultdict(list)
        for x, y, w, h in rects:
            if x + w <= 2048 and y + h <= 512:
                groups[(w, h)].append((x, y))
        statics = set()
        for (w, h), xy in groups.items():
            for (x, y), g in zip(xy, fnv_batch(w, h, np.stack([img[y:y + h, x:x + w].reshape(-1) for x, y in xy]))):
                k = f"{g:016x}"
                statics.add(k)
                if k not in per[name]:
                    per[name][k] = {"key": k, "w": w, "h": h, "file": f"{k}_{w}x{h}.png", "module": None, "first_frame": None,
                                    "screen_xy": None, "seen": False,
                                    "sources": [{"family": "map", "level": name, "vram_xy": [x, y], "static": True}]}
        seen_map = {k for k, it in per[name].items() if it.get("seen") and any(s_["family"] == "map" for s_ in it["sources"])}
        check[name] = {"static_rects": len(rects), "static_keys": len(statics), "runtime_map_keys": len(seen_map),
                       "runtime_map_keys_reproduced": len(seen_map & statics)}
        print(f"{name}: static map keys {len(statics)}, runtime map keys {len(seen_map)}, reproduced {len(seen_map & statics)}")
    OUT.mkdir(parents=True, exist_ok=True)
    for name, items in sorted(per.items()):
        fam = defaultdict(int)
        for it in items.values():
            for s in it["sources"] or [{"family": "unlinked"}]:
                fam[s["family"]] += 1
        (OUT / f"{name}.json").write_text(json.dumps({"_doc": __doc__.split("\n\n")[0] + " See tools/hires/inventory.py.",
                                                      "name": name, "keys": len(items), "by_family": dict(fam),
                                                      "seen_at_runtime": sum(1 for i in items.values() if i.get("seen")),
                                                      "items": sorted(items.values(), key=lambda i: (i["first_frame"] is None, i["first_frame"] or 0, i["key"]))}, indent=1),
                                          encoding="utf-8")
        print(f"{name:28} {len(items):6d} keys ({sum(1 for i in items.values() if i.get('seen'))} seen)  {dict(fam)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
