"""Contact sheet of a level's T28 models (S2.01): extracted/full-game/levels/<LEVEL>/models.png.

Models come from level_decode.decode_models (GFX_DrawModel ENG1 0x31540). Each model is drawn in its own
space, front view (x right, y down), quads far to near by mean z. Packet order (0x31540): xy0 = index 0,
xy1 = index 1, xy2 = index 3, xy3 = index 2, so PSX vertex order is (p0, p1, p3, p2). Textures: T30 words
(tpage, UV); the draw ORs CLUT 0x7830 + fog level * 0x40 in, level 0 here. Flat quads (texture word high
byte != 0) are filled with palette[colour]. Back-face culling and semi-transparency are not applied.

    render_models.py LEVEL [--dir extracted/full-game/levels] [--cell 200]
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from PIL import Image, ImageDraw

sys.path.insert(0, str(Path(__file__).parent))
from render_preview import ROOT, Vram, draw_quad  # noqa: E402


def render_model(m: dict, tex: dict, vram: Vram, pal, size: int) -> Image.Image:
    quads = [q for g in m["groups"] for q in g["quads"]]
    pts = [v for q in quads for v in q["p"]]
    img = Image.new("RGBA", (size, size), (24, 24, 32, 255))
    if not pts:
        return img
    x0, x1 = min(v[0] for v in pts), max(v[0] for v in pts)
    y0, y1 = min(v[1] for v in pts), max(v[1] for v in pts)
    scale = min((size - 16) / max(1, x1 - x0), (size - 28) / max(1, y1 - y0), 2.0)
    ox, oy = (size - (x1 - x0) * scale) / 2 - x0 * scale, 20 + (size - 28 - (y1 - y0) * scale) / 2 - y0 * scale
    dr = ImageDraw.Draw(img)
    for q in sorted(quads, key=lambda q: -sum(v[2] for v in q["p"])):
        p = q["p"]
        psx = [p[0], p[1], p[3], p[2]]
        if "colour" in q:
            c = tuple(pal[q["colour"]]) + (255,)
            dr.polygon([(v[0] * scale + ox, v[1] * scale + oy) for v in (p[0], p[1], p[2], p[3])], fill=c)
            continue
        tx = tex.get(str(q["tex"]))
        if not tx:
            continue
        layer = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw_quad(layer, vram.page(tx["tpage"], 0), tx["uv"], [((v[0] * scale + ox), (v[1] * scale + oy)) for v in psx], 1.0)
        img.alpha_composite(layer)
        dr = ImageDraw.Draw(img)
    return img


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("level")
    ap.add_argument("--dir", default=str(ROOT / "extracted" / "full-game" / "levels"))
    ap.add_argument("--cell", type=int, default=200)
    a = ap.parse_args(argv)
    d = Path(a.dir) / a.level
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    pal = json.loads((d / "palette.json").read_text())
    vram = Vram((d / "vram.bin").read_bytes(), pal, (d / "fog.bin").read_bytes())
    models = L.get("models", [])
    cols = 8
    rows = (len(models) + cols - 1) // cols
    sheet = Image.new("RGBA", (cols * a.cell, max(1, rows) * a.cell), (0, 0, 0, 255))
    drawn = 0
    for i, m in enumerate(models):
        if "error" in m or any("error" in g for g in m["groups"]):
            tile = Image.new("RGBA", (a.cell, a.cell), (60, 20, 20, 255))
        else:
            tile = render_model(m, L["textures"], vram, pal, a.cell)
            drawn += 1
        ImageDraw.Draw(tile).text((4, 2), f"model {m['k']} r{m.get('radius')}", fill=(255, 255, 255, 255))
        sheet.paste(tile, ((i % cols) * a.cell, (i // cols) * a.cell))
    sheet.save(d / "models.png")
    print(f"{a.level}: {drawn}/{len(models)} models -> {d / 'models.png'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
