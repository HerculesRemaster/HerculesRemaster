"""Render a level's T44 parallax background (S2.04): extracted/full-game/levels/<LEVEL>/background.png.

Rules (ENG1 `LVL_DrawBackgroundTiles` 0x2ff0, draw modes set by 0x1f320):
  T44 = {s16 width, s16 height, s16 tile[width*height]} (header table after T50), 16x16 pixel tiles
  tile: u = t & 0xf8, v = (t >> 8) & 0xf8, page = 0x9c + 2 * ((t >> 8) & 1) (VRAM (768,256) / (896,256), 8 bpp),
        mode = t & 3: 0 = sprite 16x16; bit 0 = the texture is half as wide (2x horizontal), bit 1 = half as high
        (2x vertical) (FT4 with uv1 = u + (w >> bit0) - 1, uv2 = v + 15 - 8*bit1)
  texel 0 = transparent; colours = palette[texel] (no fog row; the CLUT word 0x73480 has no static writer)
  The map wraps horizontally (column index mod width). Clear colour: the level's 0x5df08 bytes (or the hook's
  height-dependent colours, see docs/level-format.md § Background hooks).

    render_background.py LEVEL [--dir extracted/full-game/levels] [--tiles-x N]
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

import numpy as np
import pefile
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).parent))
import level_decode as LD  # noqa: E402

LEVEL_IDS = LD.LEVEL_IDS


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("level")
    ap.add_argument("--dir", default=str(ROOT / "extracted" / "full-game" / "levels"))
    ap.add_argument("--tiles-x", type=int, default=0, help="columns to render (default: 2 x map width)")
    a = ap.parse_args(argv)
    d = Path(a.dir) / a.level
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    entries = json.loads(LD.MANIFEST.read_text(encoding="utf-8"))["entries"]
    e = next(x for x in entries if (x.get("name_candidate") or "").upper().endswith(f"\\{a.level}\\MAP_PSX.BIN"))
    raw = (ROOT / e["file"]).read_bytes()
    t44 = L["header"]["t44"]
    w, h = struct.unpack_from("<hh", raw, t44)
    if w <= 0 or h <= 0:
        print(f"{a.level}: no T44 background")
        return 0
    tiles = struct.unpack_from(f"<{w * h}H", raw, t44 + 4)
    vram = np.frombuffer((d / "vram.bin").read_bytes(), np.uint8).reshape(512, 2048)
    pal = np.array(json.loads((d / "palette.json").read_text()), np.uint8)
    pe = pefile.PE(str(ROOT / "backups" / "originals" / "ENG1.DLL"), fast_load=True)
    img = pe.get_memory_mapped_image()
    lv = LEVEL_IDS[a.level]
    clear = tuple(img[0x5DF08 + 0x1C * lv:0x5DF08 + 0x1C * lv + 3])
    cols = a.tiles_x or 2 * w
    out = np.zeros((h * 16, cols * 16, 4), np.uint8)
    out[:, :, :3] = clear
    out[:, :, 3] = 255
    pages = {0x9C: vram[256:512, 1536:1792], 0x9E: vram[256:512, 1792:2048]}
    for r in range(h):
        for c in range(cols):
            t = tiles[r * w + c % w]
            u, v, mode, page = t & 0xF8, (t >> 8) & 0xF8, t & 3, 0x9C + 2 * ((t >> 8) & 1)
            tw, th = 16 >> (mode & 1), 16 >> ((mode >> 1) & 1)
            src = pages[page][v:v + th, u:u + tw]
            if src.shape != (th, tw):
                continue
            src = np.repeat(np.repeat(src, 16 // th, axis=0), 16 // tw, axis=1)
            mask = src != 0
            blk = out[r * 16:(r + 1) * 16, c * 16:(c + 1) * 16]
            blk[mask, :3] = pal[src[mask]]
    Image.fromarray(out, "RGBA").save(d / "background.png")
    print(f"{a.level}: T44 {w}x{h} tiles, clear colour {clear} -> {d / 'background.png'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
