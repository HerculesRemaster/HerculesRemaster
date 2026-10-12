"""Preview of the ENG3 maps (S3.05): extracted/full-game/levels/<NAME>/preview.png.

eng3_preview.py [NAME ...]

One band per track (track table hdr+0x30, docs/full-game/eng3-tables.md), top to bottom in track order.
Each band shows the track's block list ({block, height} pairs; heights 0/4/8/12 are stacked layers) as
12-row strips of 128-unit cells (column-major blocks, tools/level/eng3_decode.py), 1 pixel per cell, scaled 4x:
  red   = collision profile (bits 1-10), green = light zone (bits 11-23), blue = signed height (top byte)
  black = empty cell (bit 0)
Above the strips, a spawn row: one tick per spawn at column wx >> 7 (x + template dx), coloured by the
kind's category from extracted/meta/full-game/<Lxx>-types.json (CAT colours below; grey = unclassified).
The depth position of a spawn is not drawn. Start records (HERO_Init 0x1c720; "section" = track)
are white boxes on the spawn row, and the label gives the parent track and branch column.
"""
import json
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2]
LV = ROOT / "extracted" / "full-game" / "levels"
CAT = {"chase_hazards": (255, 40, 40), "enemy_animation": (255, 140, 0), "boss_animation": (255, 0, 255),
       "npc_animation": (80, 160, 255), "props_pickups": (255, 230, 0), "checkpoint_state": (0, 255, 120),
       "fx_projectiles": (200, 200, 200), "collision_camera": (0, 255, 255)}
SLUG = {"GAUNTLET": "L02", "CYCLOPS": "L07", "TORMENT": "L09"}
S = 4


def strip(b):
    c = np.array(b["cells"], dtype=np.uint32).reshape(b["columns"], b["rows"]).T
    prof, zone, h = (c >> 1) & 0x3ff, (c >> 11) & 0x1fff, (c >> 24).astype(np.int8).astype(np.int32)
    rgb = np.dstack([(prof * 29) % 200 + 55, (zone * 53) % 200 + 55, np.clip(h * 2 + 128, 0, 255)]).astype(np.uint8)
    rgb[(c & 1) == 1] = 0
    return rgb


def preview(name):
    d = json.loads((LV / name / "level.json").read_text())
    tf = ROOT / "extracted/meta/full-game" / f"{SLUG[name]}-types.json"
    cat = {int(k["kind"], 16): k.get("category") for k in json.loads(tf.read_text())["kinds"]} if tf.exists() else {}
    blocks, tracks = d["blocks"], d["tracks"]
    W = max(b["columns"] for b in blocks)
    bands = []
    for t in tracks:
        rows = [np.zeros((4, W, 3), np.uint8)]                    # spawn row (ticks drawn later)
        for blk, _h in t["blocks"]:
            s = np.zeros((13, W, 3), np.uint8)
            s[:12, :blocks[blk]["columns"]] = strip(blocks[blk])
            rows.append(s)
        rows.append(np.full((3, W, 3), 40, np.uint8))
        bands.append(np.vstack(rows))
    img = Image.fromarray(np.vstack(bands)).resize((W * S, sum(b.shape[0] for b in bands) * S), Image.NEAREST)
    dr = ImageDraw.Draw(img)
    y0 = 0
    for t, band in zip(tracks, bands):
        for sp in t["spawns"]:
            x = (sp["wx"] >> 7) * S
            dr.rectangle([x, y0 * S, x + S - 1, (y0 + 4) * S - 1], fill=CAT.get(cat.get(int(sp["type"], 16)), (110, 110, 110)))
        for st in d["starts"]:
            if st["section"] == t["index"]:
                x = (st["x"] >> 7) * S
                dr.rectangle([x - 2, y0 * S, x + S + 1, (y0 + 4) * S + 2], outline=(255, 255, 255))
        dr.text((2, (y0 + 4) * S + 2), f"track {t['index']} parent {t['parent']} col {t['branch_column']}", fill=(255, 255, 255))
        y0 += band.shape[0]
    img.save(LV / name / "preview.png")
    print(f"{name}: {len(tracks)} tracks, {sum(len(t['spawns']) for t in tracks)} spawns, width {W} -> {LV / name / 'preview.png'}")


if __name__ == "__main__":
    for n in sys.argv[1:] or ["GAUNTLET", "CYCLOPS", "TORMENT"]:
        preview(n)
