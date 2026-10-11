"""Render a decoded level (extracted/levels/<LEVEL>/level.json + vram.bin) as straightened side views.

Projection: orthographic front view along the level path (the camera path is unrolled, so arcs
become straight). Cell (column c, row r) occupies x = c*64 .. +64, y = r*64 .. +64; a vertex
(vx, vy, vz) lands at (c*64 + vx, r*64 + vy) (ENG1 0x2e3b0; vx -> 64 - vx when the cell's
mirror bit is set, 0x2ed10). Collision surface: y = r*64 + 32 - h (ENG1 0x20b50). Quads are drawn far to near by their OT depth field (higher first),
layers from back (highest index) to front. Textures: tpage (x = (tp & 15)*64 units,
y = (tp >> 4 & 1)*256, mode (tp >> 7) & 3, here 8 bpp), UV bytes, CLUT = 0x7830 + level*0x40
(the renderer ORs this in). The PC rasteriser maps texel t to palette[fog[level][t]]
(MAP_PSX.CIF); level 0 is used here and texel 0 is transparent. Flat quads (0x80) are skipped.

    render_preview.py LEVEL [--cols a:b] [--scale 0.5] [--layers 0,1,...] [--collision]
Writes extracted/levels/<LEVEL>/preview_layer<l>.png, preview_composite.png and with
--collision preview_collision.png (collision surface lines at mid-depth per layer, ENG1 0x20990,
plus spawn markers: kind in hex; orange = kind & 0x2000).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2]


class Vram:
    """8-bit texture pages shown the way the PC rasteriser does: palette[fog[level][texel]],
    texel 0 transparent (PSX: CLUT colour 0)."""

    def __init__(self, raw: bytes, palette, fog: bytes):
        self.u16 = np.frombuffer(raw, dtype="<u2").reshape(512, 1024)
        self.pal = np.array(palette, np.uint8)
        self.fog = np.frombuffer(fog, np.uint8).reshape(-1, 256)
        self.pages: dict[tuple[int, int], Image.Image] = {}

    def page(self, tpage: int, level: int) -> Image.Image:
        key = (tpage & 0x1F, level)
        if key in self.pages:
            return self.pages[key]
        px, py = (tpage & 15) * 64, (tpage >> 4 & 1) * 256
        idx = self.u16[py:py + 256, px:px + 128].view(np.uint8).reshape(256, 256)
        mapped = self.fog[min(level, len(self.fog) - 1)][idx]
        rgba = np.dstack([self.pal[mapped], np.where(idx == 0, 0, 255).astype(np.uint8)])
        img = Image.fromarray(rgba, "RGBA")
        self.pages[key] = img
        return img


def homography(src, dst):
    """Coefficients mapping dst (x, y) -> src (u, v) for PIL PERSPECTIVE."""
    A, B = [], []
    for (u, v), (x, y) in zip(src, dst):
        A += [[x, y, 1, 0, 0, 0, -u * x, -u * y], [0, 0, 0, x, y, 1, -v * x, -v * y]]
        B += [u, v]
    return np.linalg.lstsq(np.array(A, float), np.array(B, float), rcond=None)[0]


def draw_quad(canvas: Image.Image, page: Image.Image, uv, pts, scale):
    pts = [(x * scale, y * scale) for x, y in pts]
    xs, ys = [p[0] for p in pts], [p[1] for p in pts]
    x0, y0, x1, y1 = int(min(xs)), int(min(ys)), int(max(xs)) + 1, int(max(ys)) + 1
    w, h = x1 - x0, y1 - y0
    if w <= 0 or h <= 0 or w > 4000 or h > 4000:
        return
    order = [0, 1, 3, 2]  # PSX quad v0 v1 v2 v3 = TL TR BL BR -> polygon order
    poly = [(pts[i][0] - x0, pts[i][1] - y0) for i in order]
    try:
        coeffs = homography([uv[i] for i in order], poly)
    except np.linalg.LinAlgError:
        return
    patch = page.transform((w, h), Image.PERSPECTIVE, tuple(coeffs), Image.NEAREST)
    mask = Image.new("L", (w, h), 0)
    ImageDraw.Draw(mask).polygon(poly, fill=255)
    alpha = Image.fromarray(np.minimum(np.array(mask), np.array(patch)[:, :, 3]))
    canvas.paste(patch, (x0, y0), alpha)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("level")
    ap.add_argument("--cols", default="0:120")
    ap.add_argument("--scale", type=float, default=0.5)
    ap.add_argument("--layers", default="")
    ap.add_argument("--collision", action="store_true")
    a = ap.parse_args(argv)
    d = ROOT / "extracted" / "levels" / a.level
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    vram = Vram((d / "vram.bin").read_bytes(), json.loads((d / "palette.json").read_text()),
                (d / "fog.bin").read_bytes())
    sec = L["sections"][0]
    c0, c1 = (int(x) for x in a.cols.split(":"))
    c1 = min(c1, sec["columns"])
    rows = sec["rows"]
    W, H = int((c1 - c0) * 64 * a.scale), int(rows * 64 * a.scale)
    layers = [int(x) for x in a.layers.split(",")] if a.layers else list(range(sec["layers"]))
    types, tex = L["types"], L["textures"]
    level = 0  # fog row (CLUT 0x7830 + level*0x40); 0 = nearest
    composite = Image.new("RGBA", (W, H), (0, 0, 0, 255))
    drawn = 0
    for l in sorted(layers, reverse=True):
        canvas = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        items = []
        for c in range(c0, c1):
            for r in range(rows):
                cell = sec["grid"][l][c][r]
                t = types.get(str((cell >> 1) & 0x3FF))
                if not t or not t["mesh"]:
                    continue
                mirror = cell & 1
                m = t["mesh"]
                for q in m["quads"]:
                    if q["flags"] & 0x80 or str(q["tex"]) not in tex:
                        continue
                    vs = [m["verts"][i] for i in q["v"] if i < len(m["verts"])]
                    if len(vs) < 4:
                        continue
                    pts = [((c - c0) * 64 + (64 - vx if mirror else vx), r * 64 + vy) for vx, vy, vz in vs]
                    items.append((q["depth"], pts, tex[str(q["tex"])]))
        items.sort(key=lambda it: -it[0])
        for _, pts, tx in items:
            draw_quad(canvas, vram.page(tx["tpage"], level), tx["uv"], pts, a.scale)
            drawn += 1
        canvas.save(d / f"preview_layer{l}.png")
        composite.alpha_composite(canvas)
    composite.save(d / "preview_composite.png")
    if a.collision:
        col = composite.copy()
        dr = ImageDraw.Draw(col, "RGBA")
        colours = [(255, 60, 60, 230), (255, 230, 0, 230), (0, 230, 255, 230), (160, 255, 120, 230), (255, 120, 255, 230)]
        for l in layers:
            for c in range(c0, c1):
                for r in range(rows):
                    cell = sec["grid"][l][c][r]
                    t_ = types.get(str((cell >> 1) & 0x3FF))
                    if not t_ or not t_["collision"]:
                        continue
                    hs = t_["collision"]["heights_z0_32_63"][1]          # z = 32 (mid-slice)
                    if cell & 1:
                        hs = hs[::-1]
                    xs = list(range(0, 64, 8)) + [63]
                    pts = [((c - c0) * 64 + x, r * 64 + 32 - hgt) for x, hgt in zip(xs, hs) if hgt >= 0]
                    for (x0_, y0_), (x1_, y1_) in zip(pts, pts[1:]):
                        dr.line([x0_ * a.scale, y0_ * a.scale, x1_ * a.scale, y1_ * a.scale], fill=colours[l % 5], width=2)
        for sp in sec.get("spawns", []):
            x = sp.get("wx", sp["x"])
            if not (c0 * 64 <= x < c1 * 64):
                continue
            px, py = (x - c0 * 64) * a.scale, sp.get("wy", sp["y"]) * a.scale
            kind = sp.get("kind", 0)
            dr.ellipse([px - 4, py - 4, px + 4, py + 4], outline=(255, 255, 255, 255), fill=(255, 80, 0, 200) if kind & 0x2000 else (0, 120, 255, 200))
            dr.text((px + 5, py - 6), f"{kind:04x}", fill=(255, 255, 255, 255))
        col.save(d / "preview_collision.png")
    print(f"{a.level}: columns {c0}:{c1} layers {layers} quads={drawn} -> {d}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
