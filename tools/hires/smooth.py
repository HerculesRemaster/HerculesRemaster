"""Fallback 4K art for every block without redrawn art: Scale3x applied twice (exactly 9x) on palette indices.

smooth.py [--out working/game/art/smooth] [--limit N] [--jobs N]

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


def write_group(args):
    out, w, h, items = args
    a = np.frombuffer(b"".join(t for _, t in items), np.uint8).reshape(len(items), h, w)
    big = scale3x(scale3x(a))
    for (key, _), img in zip(items, big):
        im = Image.fromarray(img, "P")
        im.putpalette(GRAY)
        im.save(out / f"{key:016x}_{w}x{h}.png", optimize=False, compress_level=6)
    return len(items)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=common.ART_DIR / "smooth")
    ap.add_argument("--limit", type=int)
    ap.add_argument("--jobs", type=int, default=8)
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
        groups += [(a.out, w, h, items[i:i + step]) for i in range(0, len(items), step)]
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
