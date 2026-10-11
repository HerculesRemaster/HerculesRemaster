"""Export data for the P6 renderer spike (runtime/prototypes/spike).

From extracted/levels/<LEVEL>/ (level.json, vram.bin, palette.json, fog.bin) and the decoded hero
bank, writes runtime/prototypes/spike/data/:
  atlas.png      every used 8-bit texture page as RGBA (palette[fog[0][t]], t=0 transparent),
                 pages side by side (256 x 256 each)
  quads.bin      u32 count, then per quad (far to near): 4 x (f32 x, f32 y) world units,
                 4 x (f32 u, f32 v) atlas coords, f32 depth (layer*1000 + OT depth)
  hero.png       hero run frames in a strip; hero.txt: per frame "x y w h ox oy ticks"
  meta.txt       level columns, rows, layers, quad count, run speed (16.16), tick ms
Run speed and frame ticks come from reference/tuning.json and hero_scripts.json.

    export_spike.py [LEVEL] [--cols a:b]
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "runtime" / "prototypes" / "spike" / "data"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("level", nargs="?", default="TRAINING")
    ap.add_argument("--cols", default="0:500")
    a = ap.parse_args(argv)
    d = ROOT / "extracted" / "levels" / a.level
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    vram = np.frombuffer((d / "vram.bin").read_bytes(), dtype="<u2").reshape(512, 1024)
    pal = np.array(json.loads((d / "palette.json").read_text()), np.uint8)
    fog = np.frombuffer((d / "fog.bin").read_bytes(), np.uint8).reshape(-1, 256)
    sec = L["sections"][0]
    c0, c1 = (int(x) for x in a.cols.split(":"))
    c1 = min(c1, sec["columns"])
    types, tex = L["types"], L["textures"]
    pages: dict[int, int] = {}
    quads = []
    for lay in range(sec["layers"] - 1, -1, -1):
        for c in range(c0, c1):
            for r in range(sec["rows"]):
                cell = sec["grid"][lay][c][r]
                t = types.get(str((cell >> 1) & 0x3FF))
                if not t or not t["mesh"]:
                    continue
                m = t["mesh"]
                for q in m["quads"]:
                    if q["flags"] & 0x80 or str(q["tex"]) not in tex or max(q["v"]) >= len(m["verts"]):
                        continue
                    tx = tex[str(q["tex"])]
                    tp = tx["tpage"] & 0x1F
                    slot = pages.setdefault(tp, len(pages))
                    pos = []
                    for vi in q["v"]:
                        vx, vy, _ = m["verts"][vi]
                        pos.append(((c * 64) + (64 - vx if cell & 1 else vx), r * 64 + vy))
                    uv = [((slot * 256 + u + 0.5), (v + 0.5)) for u, v in tx["uv"]]
                    quads.append((lay * 1000 + (1023 - q["depth"]), pos, uv))
    quads.sort(key=lambda qd: -qd[0])
    atlas = np.zeros((256, 256 * len(pages), 4), np.uint8)
    for tp, slot in pages.items():
        px, py = (tp & 15) * 64, (tp >> 4 & 1) * 256
        idx = vram[py:py + 256, px:px + 128].view(np.uint8).reshape(256, 256)
        rgb = pal[fog[0][idx]]
        atlas[:, slot * 256:(slot + 1) * 256, :3] = rgb
        atlas[:, slot * 256:(slot + 1) * 256, 3] = np.where(idx == 0, 0, 255)
    OUT.mkdir(parents=True, exist_ok=True)
    Image.fromarray(atlas, "RGBA").save(OUT / "atlas.png")
    aw = atlas.shape[1]
    with (OUT / "quads.bin").open("wb") as f:
        f.write(struct.pack("<I", len(quads)))
        for depth, pos, uv in quads:
            for x, y in pos:
                f.write(struct.pack("<ff", x, y))
            for u, v in uv:
                f.write(struct.pack("<ff", u / aw, v / 256))
            f.write(struct.pack("<f", depth))
    # hero run cycle from the script data
    scripts = json.loads((ROOT / "extracted" / "sprites" / "0560_SRC1_HERO_HERCULES" / "hero_scripts.json").read_text())
    bank = json.loads((ROOT / "extracted" / "sprites" / "0560_SRC1_HERO_HERCULES" / "bank.json").read_text())
    meta = {f["frame"]: f for f in bank["frames"]}
    steps = scripts["0x10053a20"]["steps"]                 # run state script (HERO_SetRun 0x16a60)
    lp = max(i for i, s in enumerate(steps) if s.get("op") == "loop_point")
    cycle = [s for s in steps[lp + 1:] if "frame" in s]     # the looping part: 10-15, 4-9
    frames = [(s["frame"], max(s["ticks"], 1)) for s in cycle if meta.get(s["frame"], {}).get("png")]
    ims = [Image.open(ROOT / "extracted" / "sprites" / "0560_SRC1_HERO_HERCULES" / meta[f]["png"]) for f, _ in frames]
    W, H = sum(i.width for i in ims), max(i.height for i in ims)
    strip = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    lines, x = [], 0
    for (f, ticks), im in zip(frames, ims):
        strip.paste(im, (x, 0))
        ox, oy = meta[f]["origin"]
        lines.append(f"{x} 0 {im.width} {im.height} {ox} {oy} {ticks}")
        x += im.width
    strip.save(OUT / "hero.png")
    (OUT / "hero.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    tun = json.loads((ROOT / "reference" / "tuning.json").read_text())
    (OUT / "meta.txt").write_text(
        f"{c1 - c0} {sec['rows']} {sec['layers']} {len(quads)} {int(tun['run']['target_speed']['raw'], 16)} {tun['tick_ms']} {c0}\n",
        encoding="utf-8")
    print(f"{a.level}: quads={len(quads)} pages={len(pages)} atlas={aw}x256 hero_frames={len(frames)} -> {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
