"""Add the ENG3 maps (GAUNTLET, CYCLOPS, TORMENT) to the replacement manifest as "level" entries (full-game L*.RM.03).

contract_eng3.py [--manifest remastered/manifest.json]

contract.py builds the level family from the ENG1 exports (extracted/levels/<DIR>); the ENG3 maps are exported by
tools/level/eng3_decode.py to extracted/full-game/levels/<NAME>/ instead. This step adds or replaces only the entries
"level/<NAME>" for the three ENG3 maps and leaves every other entry untouched. Entry layout as contract.level_entries:
  source    vram.bin (1024 x 512 16-bit units = 2048 x 512 8-bit texels), uploads [0,0,1536,512] and
            [1536,256,512,256] (LVL_UploadVram 0x1aa30, same rects as ENG1)
  blocks    rects the renderer can key for the map:
              - every textured cell-mesh face (level.json "mesh_textures"): UV bounding box, exact 1:1 rect,
                mirrored rect (contract.level_entries rules), texels at x = (tpage & 15) * 128 + u, y = (tpage >> 4 & 1) * 256 + v
              - the map rects drawn at runtime that the mesh rule does not produce (model faces and other polygons
                on the map VRAM): extracted/meta/full-game/art-inventory/<NAME>.json items with a "map" source and
                seen = true (tools/hires/inventory.py)
            rects crossing a 256-texel page are skipped (as contract.py)
  palette   common.reference_image reads extracted/levels/<NAME>/palette.json; a copy of the ENG3 CIF palette is
            written there (the ENG1 export folder holds no ENG3 data otherwise).
"""
import argparse
import hashlib
import json
import shutil
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from contract import Keyer  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
NAMES = ("GAUNTLET", "CYCLOPS", "TORMENT")


def rects(name):
    lv = json.loads((ROOT / "extracted/full-game/levels" / name / "level.json").read_text(encoding="utf-8"))
    out = set()
    for t in lv.get("mesh_textures", []):
        tp = t["tpage"]
        us, vs = [uv[0] for uv in t["uv"]], [uv[1] for uv in t["uv"]]
        bx, by = (tp & 15) * 128, ((tp >> 4) & 1) * 256
        u0, u1, v0, v1 = min(us), max(us), min(vs), max(vs)
        out.add((bx + u0, by + v0, u1 - u0 + 1, v1 - v0 + 1))
        if u1 > u0 and v1 > v0:
            out.add((bx + u0, by + v0, u1 - u0, v1 - v0))
            out.add((bx + u0 + 1, by + v0, u1 - u0, v1 - v0))
    inv = ROOT / "extracted/meta/full-game/art-inventory" / f"{name}.json"
    if inv.exists():
        for it in json.loads(inv.read_text(encoding="utf-8"))["items"]:
            if it.get("seen"):
                for s in it["sources"]:
                    if s["family"] == "map" and s.get("level") == name:
                        x, y = s["vram_xy"]
                        out.add((x, y, it["w"], it["h"]))
    return sorted(r for r in out if r[0] + r[2] <= 2048 and r[1] + r[3] <= 512 and r[0] // 256 == (r[0] + r[2] - 1) // 256)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--manifest", type=Path, default=ROOT / "remastered" / "manifest.json")
    a = ap.parse_args(argv)
    man = json.loads(a.manifest.read_text(encoding="utf-8"))
    keyer, new = Keyer(), []
    for name in NAMES:
        src = ROOT / "extracted/full-game/levels" / name
        vram = (src / "vram.bin").read_bytes()
        img = np.frombuffer(vram, np.uint8).reshape(512, 2048)
        pal_dst = ROOT / "extracted/levels" / name
        pal_dst.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(src / "palette.json", pal_dst / "palette.json")
        ent = {"id": f"level/{name}", "family": "level",
               "source": {"kind": "vram", "level": name, "engine": "ENG3", "file": f"extracted/full-game/levels/{name}/vram.bin",
                          "uploads": [[0, 0, 1536, 512], [1536, 256, 512, 256]], "sha256": hashlib.sha256(vram).hexdigest()},
               "geometry": {"size": [2048, 512], "origin": [0, 0], "blocks": []},
               "timing": {"note": "ENG3 animated (n10) and scrolling (n12) VRAM rects change at runtime and keep the original"},
               "reference": f"extracted/full-game/levels/{name}/vram.png",
               "output": f"remastered/art/level/{name}.png"}
        for x, y, w, h in rects(name):
            b = {"rect": [x, y, w, h], "key": None}
            ent["geometry"]["blocks"].append(b)
            keyer.add(w, h, img[y:y + h, x:x + w].tobytes(), b)
        new.append(ent)
    for key, block in keyer.run():
        block["key"] = f"{key:016x}"
    ids = {e["id"] for e in new}
    man["entries"] = [e for e in man["entries"] if e["id"] not in ids] + new
    a.manifest.write_text(json.dumps(man, indent=0), encoding="utf-8")
    for e in new:
        print(f"{e['id']}: {len(e['geometry']['blocks'])} blocks")
    print(f"-> {a.manifest} ({len(man['entries'])} entries)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
