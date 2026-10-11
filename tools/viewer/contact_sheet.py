"""Contact sheets for decoded images: one PNG per sprite bank (first N frames) and one for the TGAs.

    contact_sheet.py [--max 48] [--scale 2]
Reads extracted/sprites/*/f*.png and extracted/images/tga/*.png, writes extracted/images/_sheets/.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2]


def checker(w: int, h: int, s: int = 8) -> Image.Image:
    im = Image.new("RGBA", (w, h), (70, 70, 80, 255))
    d = ImageDraw.Draw(im)
    for y in range(0, h, s):
        for x in range((y // s) % 2 * s, w, 2 * s):
            d.rectangle([x, y, x + s - 1, y + s - 1], fill=(95, 95, 105, 255))
    return im


def sheet(paths: list[Path], cols: int, scale: int, label: bool = True) -> Image.Image:
    ims = [Image.open(p).convert("RGBA") for p in paths]
    cw = max(i.width for i in ims) + 4
    ch = max(i.height for i in ims) + (12 if label else 4)
    rows = (len(ims) + cols - 1) // cols
    out = checker(cw * cols, ch * rows)
    d = ImageDraw.Draw(out)
    for k, (p, im) in enumerate(zip(paths, ims)):
        x, y = (k % cols) * cw, (k // cols) * ch
        out.alpha_composite(im, (x + 2, y + 2))
        if label:
            d.text((x + 2, y + ch - 11), p.stem, fill=(255, 255, 160, 255))
    return out.resize((out.width * scale, out.height * scale), Image.NEAREST) if scale > 1 else out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--max", type=int, default=48)
    ap.add_argument("--scale", type=int, default=2)
    a = ap.parse_args(argv)
    out = ROOT / "extracted" / "images" / "_sheets"
    out.mkdir(parents=True, exist_ok=True)
    n = 0
    for bank in sorted((ROOT / "extracted" / "sprites").iterdir()):
        frames = sorted(bank.glob("f*.png"))[: a.max]
        if not frames:
            continue
        sheet(frames, 8, a.scale).save(out / f"sprites_{bank.name}.png")
        n += 1
    tgas = sorted((ROOT / "extracted" / "images" / "tga").glob("*.png"))
    if tgas:
        try:
            sheet(tgas, 8, 1, label=True).save(out / "tga_all.png")
        except OSError as ex:  # e.g. the file is open in a viewer
            print(f"tga_all.png not written: {ex}")
    print(f"sprite_sheets={n} tga_sheet={'yes' if tgas else 'no'} -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
