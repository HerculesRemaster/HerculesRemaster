"""Render a decoded level (extracted/levels/<LEVEL>/level.json + vram.bin) as straightened side views.

Projection: orthographic front view along the level path (the camera path is unrolled, so arcs
become straight). Cell (column c, row r) occupies x = c*64 .. +64, y = r*64 .. +64; a vertex
(vx, vy, vz) lands at (c*64 + vx, r*64 + vy) (ENG1 0x2e3b0; vx -> 64 - vx when the cell's
mirror bit is set, 0x2ed10). Collision surface: y = r*64 + 32 - h (ENG1 0x20b50). Quads are drawn far to near by their OT depth field (higher first),
layers from back (highest index) to front. Textures: tpage (x = (tp & 15)*64 units,
y = (tp >> 4 & 1)*256, mode (tp >> 7) & 3, here 8 bpp), UV bytes, CLUT = 0x7830 + level*0x40
(the renderer ORs this in). The PC rasteriser maps texel t to palette[fog[level][t]]
(MAP_PSX.CIF); level 0 is used here and texel 0 is transparent. Flat quads (0x80) are skipped.

    render_preview.py LEVEL [--dir DIR] [--cols a:b] [--scale 0.5] [--layers 0,1,...] [--collision]
Writes <DIR>/<LEVEL>/preview_layer<l>.png, preview_composite.png (DIR default extracted/levels;
full game: extracted/full-game/levels) and with --collision preview_collision.png (collision
surface lines at mid-depth per layer, ENG1 0x20990, spawn markers with kind and name: blue class A
actors (kind & 0x7000 = 0), orange 0x2000 pickups, green 0x4000 markers/checkpoints, magenta other
classes (0x1000.., not spawned by LVL_SpawnVisible 0x262a0);
camera records: a tick on the top edge at each record's start column, labelled with the record;
cell attributes: outlined cells labelled a<id> (T40 grids / level-config +0x44 triggers)).
Flat quads (flag 0x80) are filled with palette[colour] (S2.04).
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


KIND_NAMES = {0x2002: "max health", 0x200b: "letter", 0x2012: "extra life", 0x2013: "health",
              0x2014: "gauge 2c", 0x2015: "gauge 26", 0x2016: "gauge 28", 0x2017: "gauge 2a",
              0x2018: "counter 18", 0x2019: "counter 14", 0x201a: "item bit 1", 0x201b: "item bit 2",
              0x201c: "item bit 4", 0x201d: "item bit 8", 0x0038: "bird"}   # HERO_CollectPickup 0x12ef0


# Model objects placed by their spawn code (S2.04): level -> kind -> spawn -> (world x, y, z, model k).
# Only rules read from the spawn functions are listed; other model objects are set up by update code.
MODEL_SPAWNS = {
    "HYDRA": {0x31: lambda sp: (0xDEA, sp["wy"], 0x80, 10) if sp["wx"] > 0x8B0 else (0x394, sp["wy"], 0x80, 9)},  # 0x1dd00
}


def draw_model(canvas: Image.Image, m: dict, tex: dict, vram: "Vram", ox: float, oy: float, scale: float) -> int:
    """Front view of a decoded model at (ox, oy) in pixels; PSX vertex order (p0, p1, p3, p2), 0x31540."""
    n = 0
    for q in sorted((q for g in m["groups"] for q in g["quads"]), key=lambda q: -sum(v[2] for v in q["p"])):
        p = q["p"]
        if "colour" in q:
            ImageDraw.Draw(canvas).polygon([(ox + v[0] * scale, oy + v[1] * scale) for v in p],
                                           fill=tuple(vram.pal[q["colour"]]) + (255,))
        elif str(q["tex"]) in tex:
            tx = tex[str(q["tex"])]
            draw_quad(canvas, vram.page(tx["tpage"], 0), tx["uv"],
                      [(ox / scale + v[0], oy / scale + v[1]) for v in (p[0], p[1], p[3], p[2])], scale)
        n += 1
    return n


def kind_name(kind: int) -> str:
    if 0x400a <= kind <= 0x4011:
        return f"checkpoint {(kind + 6) & 15}"              # LVL_CheckpointTick 0x139f0
    return KIND_NAMES.get(kind, "")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("level")
    ap.add_argument("--cols", default="0:120")
    ap.add_argument("--scale", type=float, default=0.5)
    ap.add_argument("--layers", default="")
    ap.add_argument("--collision", action="store_true")
    ap.add_argument("--dir", default=str(ROOT / "extracted" / "levels"))
    a = ap.parse_args(argv)
    d = Path(a.dir) / a.level
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
                    flat = q["flags"] & 0x80
                    if not flat and str(q["tex"]) not in tex:
                        continue
                    vs = [m["verts"][i] for i in q["v"] if i < len(m["verts"])]
                    if len(vs) < 4:
                        continue
                    pts = [((c - c0) * 64 + (64 - vx if mirror else vx), r * 64 + vy) for vx, vy, vz in vs]
                    items.append((q["depth"], pts, None if flat else tex[str(q["tex"])], q["colour"]))
        items.sort(key=lambda it: -it[0])
        for _, pts, tx, colour in items:
            if tx is None:          # flat colour quad (0x80): palette[low byte of the texture word]
                ImageDraw.Draw(canvas).polygon([(pts[i][0] * a.scale, pts[i][1] * a.scale) for i in (0, 1, 3, 2)],
                                               fill=tuple(vram.pal[colour]) + (255,))
            else:
                draw_quad(canvas, vram.page(tx["tpage"], level), tx["uv"], pts, a.scale)
            drawn += 1
        rules = MODEL_SPAWNS.get(a.level, {})
        for sp in sec.get("spawns", []):
            rule = rules.get(sp.get("kind"))
            if not rule:
                continue
            x, y, z, k = rule(sp)
            mo = next((mo for mo in L.get("models", []) if mo["k"] == k and "groups" in mo), None)
            if mo and (z + 0x40) >> 7 == l and c0 * 64 <= x < c1 * 64:
                drawn += draw_model(canvas, mo, tex, vram, (x - c0 * 64) * a.scale, y * a.scale, a.scale)
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
        for l in layers:                                   # cell attributes (bits 24-30 -> T40; triggers, S2.04)
            for c in range(c0, c1):
                for r in range(rows):
                    attr = (sec["grid"][l][c][r] >> 24) & 0x7F
                    if attr:
                        x0_, y0_ = (c - c0) * 64 * a.scale, r * 64 * a.scale
                        dr.rectangle([x0_ + 1, y0_ + 1, x0_ + 64 * a.scale - 1, y0_ + 64 * a.scale - 1],
                                     outline=(255, 255, 255, 90))
                        dr.text((x0_ + 2, y0_ + 2), f"a{attr}", fill=(255, 255, 160, 200))
        for sp in sec.get("spawns", []):
            x = sp.get("wx", sp["x"])
            if not (c0 * 64 <= x < c1 * 64):
                continue
            px, py = (x - c0 * 64) * a.scale, sp.get("wy", sp["y"]) * a.scale
            kind = sp.get("kind", 0)
            cls = kind & 0x7000
            fill = ((0, 120, 255, 200) if cls == 0 else (0, 220, 90, 220) if cls == 0x4000 else
                    (255, 80, 0, 200) if cls == 0x2000 else (255, 0, 255, 200))
            dr.ellipse([px - 4, py - 4, px + 4, py + 4], outline=(255, 255, 255, 255), fill=fill)
            dr.text((px + 5, py - 6), f"{kind:04x} {kind_name(kind)}".rstrip(), fill=(255, 255, 255, 255))
        for ci, cam in L.get("cameras", {}).items():
            x = cam["start_column"]
            if c0 <= x < c1:
                px = (x - c0) * 64 * a.scale
                dr.line([px, 0, px, 10], fill=(255, 255, 0, 255), width=2)
                dr.text((px + 2, 10), f"c{cam['record']}", fill=(255, 255, 0, 255))
        col.save(d / "preview_collision.png")
    print(f"{a.level}: columns {c0}:{c1} layers {layers} quads={drawn} -> {d}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
