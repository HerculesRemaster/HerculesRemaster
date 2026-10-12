"""Fallback 4K art for every block without redrawn art, 9x on palette indices.

smooth.py [--out working/game/art/smooth] [--limit N] [--jobs N] [--method xbrz|scale3x]

xbrz (default): runtime catalog blocks (their PNG carries the palette they were dumped with) go through
index-preserving xBRZ (xbrz_index.py; wrapped edges where the block tiles); blocks without colours
(manifest-only keys, normally replaced by the xbrz generator's family art) fall back to Scale3x twice.

Sources: every keyed block of the manifest's sprite and level entries (texels from the i1.4 banks and the
level VRAM images, as contract.py keys them) plus every runtime catalog block (extracted/art_blocks: UI,
screens, scaled quads). Output: <out>/<key>_<w>x<h>.png, 8-bit indexed (the runtime reads the indices and
applies the live CLUT, so colours, fades and semi-transparency stay exact; Scale3x only copies neighbouring
indices, so no new colours and index 0 stays the transparent texel). Edges are replicated at block borders.
The renderer takes the first folder that has a key (art_dir, then art_families in order), so 'smooth' is
listed last in art_families: redrawn art (sprite, level, ui, screen) always wins. Existing files are kept
(resumable).
"""
import argparse
import sys
from collections import defaultdict
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

import numpy as np
from PIL import Image

import common
import contract
import xbrz_index

CATALOG = common.ROOT / "extracted" / "art_blocks"

GRAY = [v for i in range(256) for v in (i, i, i)]


def scale3x(a: np.ndarray) -> np.ndarray:
    """AdvMAME3x on a batch of index images (N, h, w) -> (N, 3h, 3w)."""
    p = np.pad(a, ((0, 0), (1, 1), (1, 1)), mode="edge")
    A, B, C = p[:, :-2, :-2], p[:, :-2, 1:-1], p[:, :-2, 2:]
    D, E, F = p[:, 1:-1, :-2], p[:, 1:-1, 1:-1], p[:, 1:-1, 2:]
    G, H, I = p[:, 2:, :-2], p[:, 2:, 1:-1], p[:, 2:, 2:]
    db, bf, dh, hf = (D == B) & (D != H) & (B != F), (B == F) & (B != D) & (F != H), \
        (D == H) & (D != B) & (H != F), (H == F) & (D != H) & (B != F)
    out = np.empty((a.shape[0], a.shape[1], 3, a.shape[2], 3), a.dtype)
    out[:, :, 0, :, 0] = np.where(db, D, E)
    out[:, :, 0, :, 1] = np.where((db & (E != C)) | (bf & (E != A)), B, E)
    out[:, :, 0, :, 2] = np.where(bf, F, E)
    out[:, :, 1, :, 0] = np.where((db & (E != G)) | (dh & (E != A)), D, E)
    out[:, :, 1, :, 1] = E
    out[:, :, 1, :, 2] = np.where((bf & (E != I)) | (hf & (E != C)), F, E)
    out[:, :, 2, :, 0] = np.where(dh, D, E)
    out[:, :, 2, :, 1] = np.where((dh & (E != I)) | (hf & (E != G)), H, E)
    out[:, :, 2, :, 2] = np.where(hf, F, E)
    return out.reshape(a.shape[0], a.shape[1] * 3, a.shape[2] * 3)


def collect() -> dict:
    """key -> (w, h, texels) for every block we know."""
    import json
    entries = json.loads(contract.MANIFEST_IN.read_text(encoding="utf-8"))["entries"]
    keyer = contract.Keyer()                        # groups[(w, h)] = [(texels, block)]
    contract.sprite_entries(entries, keyer, 9)
    contract.level_entries(keyer)
    blocks = {}
    for (w, h), items in keyer.groups.items():
        arr = np.frombuffer(b"".join(t for t, _ in items), np.uint8).reshape(len(items), w * h)
        for key, (tex, _) in zip(contract.fnv_batch(w, h, arr), items):
            blocks.setdefault(key, (w, h, tex))
    for key, v in contract.load_catalog().items():
        blocks.setdefault(key, v)
    return blocks


def xbrz_block(name: str, tex: np.ndarray):
    """Index-preserving xBRZ of a catalog block, or None when the catalog has no colours for it."""
    src = CATALOG / name
    if not src.is_file():
        return None
    im = Image.open(src)
    if im.mode != "P":
        return None
    pal = np.array(im.getpalette()[:768] + [0] * (768 - len(im.getpalette()[:768])), np.uint8).reshape(256, 3)
    rgba = np.dstack([pal[tex], np.where(tex == 0, 0, 255).astype(np.uint8)])
    h, w = tex.shape
    edge = lambda a, b: np.abs(a.astype(float) - b).mean() <= 12.0 if min(h, w) >= 4 and (tex != 0).all() else False
    wrap = (edge(rgba[0, :, :3], rgba[-1, :, :3]), edge(rgba[:, 0, :3], rgba[:, -1, :3]))
    return xbrz_index.upscale(tex, rgba, 9, wrap), im.getpalette()


def write_group(args):
    out, w, h, items, method = args
    a = np.frombuffer(b"".join(t for _, t in items), np.uint8).reshape(len(items), h, w)
    big = scale3x(scale3x(a))
    for (key, _), img, tex in zip(items, big, a):
        name = f"{key:016x}_{w}x{h}.png"
        x = xbrz_block(name, tex) if method == "xbrz" else None
        im = Image.fromarray(x[0] if x else img, "P")
        im.putpalette(x[1] if x else GRAY)
        im.save(out / name, optimize=False, compress_level=6)
    return len(items)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=common.ART_DIR / "smooth")
    ap.add_argument("--limit", type=int)
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--method", choices=("xbrz", "scale3x"), default="xbrz")
    a = ap.parse_args(argv)
    a.out.mkdir(parents=True, exist_ok=True)
    have = {p.name for p in a.out.glob("*.png")}
    blocks = collect()
    todo = defaultdict(list)
    for key, (w, h, tex) in blocks.items():
        if f"{key:016x}_{w}x{h}.png" not in have and len(tex) == w * h:
            todo[(w, h)].append((key, tex))
    groups = []
    for (w, h), items in todo.items():
        step = max(1, 4_000_000 // (w * h * 81))          # ~4 MB of output per task
        groups += [(a.out, w, h, items[i:i + step], a.method) for i in range(0, len(items), step)]
    if a.limit:
        groups = groups[:a.limit]
    n = 0
    with ProcessPoolExecutor(a.jobs) as ex:
        for c in ex.map(write_group, groups):
            n += c
    total = sum(p.stat().st_size for p in a.out.glob("*.png"))
    print(f"known blocks {len(blocks)}, existing {len(have)}, written {n} -> {a.out} ({total / 2 ** 20:.0f} MB)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
