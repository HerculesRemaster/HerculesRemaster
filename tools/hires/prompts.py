"""Prompt packs for an online image model (manual route, docs/ai-image-guide.md).

prompts.py [--pilot] [--sizes 1536x1024,1024x1536,1024x1024] [--max-scale 9]

For each pilot item writes remastered/prompts/<pack>/:
  input.png    the source on flat magenta #FF00FF, laid out in a grid at the largest integer
               scale that fits one of the model's output sizes (the image size to request)
  prompt.txt   the prompt to paste with input.png
  layout.json  where each cell sits (read back by fit.py)
Sprite sequences become packs of frames on a common canvas aligned on the pivot (so the model keeps
scale and ground line). Level textures become one pack per VRAM page used by the level's quads, the
repeating tile one pack, the HUD clouds one pack.
"""
import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image

import common

KEY = (255, 0, 255)
GAP = 16


def parse_sizes(s):
    return [tuple(int(v) for v in t.split("x")) for t in s.split(",")]


def plan_grid(n, cw, ch, sizes, max_scale):
    """Best (scale, cols, rows, W, H): largest scale, then fewest packs; one pack holds `per` cells."""
    best = None
    for W, H in sizes:
        for cols in range(1, n + 1):
            rows = -(-n // cols)
            s = min((W - GAP * (cols + 1)) // (cw * cols), (H - GAP * (rows + 1)) // (ch * rows), max_scale)
            if s >= 1 and (best is None or s > best[0]):
                best = (s, cols, rows, W, H)
    return best


def split_grid(n, cw, ch, sizes, max_scale, min_scale=6):
    """Split n cells into packs so each pack reaches at least min_scale (or max_scale if smaller)."""
    for per in range(n, 0, -1):
        g = plan_grid(per, cw, ch, sizes, max_scale)
        if g and g[0] >= min(min_scale, max_scale):
            return per, g
    return 1, plan_grid(1, cw, ch, sizes, max_scale)


def write_pack(out: Path, cells, g, prompt, meta):
    s, cols, rows, W, H = g
    out.mkdir(parents=True, exist_ok=True)
    sheet = Image.new("RGB", (W, H), KEY)
    lay = {"sheet": [W, H], "scale": s, "cells": [], **meta}
    for i, (img, info) in enumerate(cells):
        cx, cy = i % cols, i // cols
        x, y = GAP + cx * (img.width * s + GAP), GAP + cy * (img.height * s + GAP)
        big = img.resize((img.width * s, img.height * s), Image.NEAREST)
        bg = Image.new("RGBA", big.size, KEY + (255,))
        bg.alpha_composite(big)
        sheet.paste(bg.convert("RGB"), (x, y))
        lay["cells"].append({**info, "at": [x, y], "size": [img.width * s, img.height * s]})
    sheet.save(out / "input.png")
    (out / "layout.json").write_text(json.dumps(lay, indent=1), encoding="utf-8")
    (out / "prompt.txt").write_text(prompt.format(W=W, H=H, cols=cols, rows=rows, n=len(cells)), encoding="utf-8")
    return out.name


COMMON_RULES = """
Strict rules:
- Output exactly {W}x{H} pixels with the same {cols}x{rows} grid: every cell stays where it is.
- Keep each subject's exact pose, outline, size and position inside its cell. Do not move, rotate, crop, mirror or rescale anything.
- Everything outside the subjects stays flat pure magenta #FF00FF: no shadows, floor, glow, sparkles, text, borders or frame numbers.
- Clean hard edges against the magenta; no magenta tint on the subject.
"""

SPRITE_PROMPT = """Remaster this video-game animation sheet in 4K CGI quality.
Input: {n} frames of {subject}, {action}, from Disney's Hercules (1997 PC game), in a {cols}x{rows} grid on flat magenta.
Redraw every frame as a high-detail 3D-rendered character with realistic skin, cloth and metal materials and soft studio light from the top left, keeping the colours and the hand-animated Disney character design.
The frames are consecutive animation: the character must look identical in every frame (same face, proportions, costume and colours); only the pose changes, exactly as in the input.
If a second image (style reference) is attached, match its character design, materials and colours exactly.
""" + COMMON_RULES

PAGE_PROMPT = """Remaster this video-game texture atlas in 4K quality.
Input: part of a texture page from Disney's Hercules (1997 PC game), level TRAINING (Greek training grounds: rock, earth, grass, trees, marble columns, wooden training rigs), scaled up on flat magenta.
Each rectangular region is a separate texture. Redraw every region in place with detailed painted-realistic surfaces (stone grain, bark, foliage, worn marble), the same colours, lighting from the top left, and the same composition.
Do not blend one region into its neighbour. A texture that repeats (its left and right or top and bottom edges match) must still tile seamlessly.
""" + COMMON_RULES

TILE_PROMPT = """Remaster this seamless game texture in 4K quality.
Input: one texture from Disney's Hercules (1997 PC game), level TRAINING, on flat magenta.
Redraw it as a detailed painted-realistic surface with the same colours, layout and lighting from the top left. It is repeated vertically in the game: the top and bottom edges must match so it tiles seamlessly.
""" + COMMON_RULES

UI_PROMPT = """Remaster these game HUD frames in 4K quality.
Input: {n} status-panel ornaments from Disney's Hercules (1997 PC game): silver-grey stylised Greek clouds with a lightning bolt, on flat magenta.
Redraw them as polished sculpted silver ornaments with crisp detail and soft light from the top left. Keep the circular opening empty (the game draws the icon orb into it) and keep the exact outline.
""" + COMMON_RULES


def sprite_packs(pilot, ents, sizes, max_scale, root: Path):
    names = []
    for seq in pilot["sequences"]:
        frames = [ents[f"sprite/{seq['bank']:04d}/f{f:04d}"] for f in seq["frames"]]
        left = max(e["geometry"]["origin"][0] for e in frames)
        top = max(e["geometry"]["origin"][1] for e in frames)
        cw = left + max(e["geometry"]["size"][0] - e["geometry"]["origin"][0] for e in frames)
        ch = top + max(e["geometry"]["size"][1] - e["geometry"]["origin"][1] for e in frames)
        per, _ = split_grid(len(frames), cw, ch, sizes, max_scale)
        for p0 in range(0, len(frames), per):
            part = frames[p0:p0 + per]
            g = plan_grid(len(part), cw, ch, sizes, max_scale)
            cells = []
            for e in part:
                ox, oy = e["geometry"]["origin"]
                cell = Image.new("RGBA", (cw, ch), (0, 0, 0, 0))
                cell.alpha_composite(common.reference_image(e), (left - ox, top - oy))
                cells.append((cell, {"entry": e["id"], "kind": "frame", "offset": [left - ox, top - oy],
                                     "canvas": e["geometry"]["size"]}))
            name = f"{seq['name']}_{p0 // per + 1}" if per < len(frames) else seq["name"]
            prompt = SPRITE_PROMPT.replace("{subject}", pilot["subjects"][seq["subject"]]).replace("{action}", seq["action"])
            names.append(write_pack(root / name, cells, g, prompt, {"family": "sprite"}))
    return names


def region_pack(name, entry, rects, prompt, sizes, max_scale, root: Path):
    ref = common.reference_image(entry)
    cells = [(ref.crop((x, y, x + w, y + h)), {"entry": entry["id"], "kind": "region", "rect": [x, y, w, h]})
             for x, y, w, h in rects]
    cw, ch = max(c.width for c, _ in cells), max(c.height for c, _ in cells)
    g = plan_grid(len(cells), cw, ch, sizes, max_scale)
    return write_pack(root / name, cells, g, prompt, {"family": entry["family"]})


def guillotine(piece, rects, limit):
    """Split piece (x, y, w, h) into parts no larger than limit along cut lines no rect crosses."""
    x, y, w, h = piece
    inner = [r for r in rects if r[0] < x + w and r[0] + r[2] > x and r[1] < y + h and r[1] + r[3] > y]
    if not inner:
        return []
    for axis in ((0, 1) if w >= h else (1, 0)):
        lo, size = (x, w) if axis == 0 else (y, h)
        if size <= limit:
            continue
        mid = lo + size // 2
        cuts = [c for c in range(lo + 8, lo + size - 7)
                if not any(r[axis] < c < r[axis] + r[axis + 2] for r in inner)]
        if cuts:
            c = min(cuts, key=lambda v: abs(v - mid))
            a_, b_ = ((x, y, c - x, h), (c, y, x + w - c, h)) if axis == 0 else ((x, y, w, c - y), (x, c, w, y + h - c))
            return guillotine(a_, inner, limit) + guillotine(b_, inner, limit)
    # tighten to the rects actually present
    x0 = max(x, min(r[0] for r in inner)); y0 = max(y, min(r[1] for r in inner))
    x1 = min(x + w, max(r[0] + r[2] for r in inner)); y1 = min(y + h, max(r[1] + r[3] for r in inner))
    return [(x0, y0, x1 - x0, y1 - y0)]


def level_pages(entry):
    """256x256 pages inside the level uploads that hold at least one manifest block."""
    pages = set()
    for b in entry["geometry"]["blocks"]:
        x, y, w, h = b["rect"]
        pages.add((x // 256, y // 256))
    up = entry["source"]["uploads"]
    def inside(px, py):
        return any(ux <= px * 256 and px * 256 + 256 <= ux + uw and uy <= py * 256 and py * 256 + 256 <= uy + uh
                   for ux, uy, uw, uh in up)
    return sorted(p for p in pages if inside(*p))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--pilot", action="store_true", default=True)
    ap.add_argument("--sizes", default="1536x1024,1024x1536,1024x1024")
    ap.add_argument("--max-scale", type=int, default=9)
    a = ap.parse_args(argv)
    sizes = parse_sizes(a.sizes)
    pilot = json.loads(Path(__file__).with_name("pilot.json").read_text(encoding="utf-8"))
    ents = common.entries_by_id()
    root = common.REMASTERED / "prompts"
    names = sprite_packs(pilot, ents, sizes, a.max_scale, root)
    for reg in pilot["regions"]:
        e = ents[reg["entry"]]
        if reg["name"] == "background_training":
            rects = [tuple(b["rect"]) for b in e["geometry"]["blocks"]]
            for px, py in level_pages(e):
                for i, piece in enumerate(guillotine((px * 256, py * 256, 256, 256), rects, 160)):
                    names.append(region_pack(f"training_page_{px}_{py}_{i + 1}", e, [piece],
                                             PAGE_PROMPT, sizes, a.max_scale, root))
        elif "rect" in reg:
            names.append(region_pack(reg["name"], e, [tuple(reg["rect"])], TILE_PROMPT, sizes, a.max_scale, root))
        elif reg["name"] == "ui_hud":
            names.append(region_pack("ui_hud_clouds", e, [(0, 0, 80, 37), (136, 0, 64, 31)], UI_PROMPT, sizes, a.max_scale, root))
    lines = ["# Prompt packs (generate in this order; see docs/ai-image-guide.md)", "",
             "| # | pack | request size | cells | scale |", "|---:|---|---|---:|---:|"]
    for i, n in enumerate(names, 1):
        lay = json.loads((root / n / "layout.json").read_text())
        lines.append(f"| {i} | {n} | {lay['sheet'][0]}x{lay['sheet'][1]} | {len(lay['cells'])} | {lay['scale']} |")
        print(f"{n}: {lay['sheet'][0]}x{lay['sheet'][1]}, scale {lay['scale']}, {len(lay['cells'])} cells")
    (root / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
