"""Checks for one generated replacement image (docs/hires-pipeline.md).

checks.py <entry id> [<image>]     (default image: the entry's output path)

  size    output = source size x scale (logical size unchanged)
  alpha   per logical pixel: opaque where the source texel is opaque, transparent where it is not
          (coverage of the k x k cell >= 0.5); at most ALPHA_MAX of the source's opaque pixels may differ
  pivot   the coverage centroid stays within PIVOT_MAX logical pixels of the source's (origin x scale)
  seams   level textures: a block whose source tiles horizontally or vertically (matching opposite
          edges) must tile in the output too
Frame count per bank is checked when packing (atlas.py).
"""
import sys
from pathlib import Path

import numpy as np
from PIL import Image

import common

ALPHA_MAX = 0.03
PIVOT_MAX = 1.0
SEAM_TILE = 12.0         # source edges this close (mean abs RGB, 0..255) count as tiling
SEAM_TOL = 24.0          # output edges may differ by max(SEAM_TOL, 2 x the source difference)


def check(entry: dict, img: Image.Image, k: int, path=None) -> list[tuple[str, bool, str]]:
    """path: the image's file, so seams can test per-block sidecar files (common.blocks_dir)."""
    res = []
    w, h = entry["geometry"]["size"]
    ok = img.size == (w * k, h * k)
    res.append(("size", ok, f"{img.size} want {(w * k, h * k)}"))
    if not ok:
        return res
    rgba = np.array(common.reference_image(entry))
    ref = rgba[:, :, 3] > 0
    if entry["family"] == "sprite" and img.mode == "P":   # indexed art: the runtime silhouette is texel != 0
        ref = common.sprite_indices(entry, rgba) != 0
    cov = common.coverage(img, k) >= 0.5
    diff = (ref != cov).sum()
    ratio = diff / max(ref.sum(), 1)
    res.append(("alpha", ratio <= ALPHA_MAX, f"{diff} logical pixels differ ({ratio:.2%})"))
    if ref.any() and cov.any():
        ys, xs = np.nonzero(ref)
        yo, xo = np.nonzero(cov)
        d = float(np.hypot(xs.mean() - xo.mean(), ys.mean() - yo.mean()))
        res.append(("pivot", d <= PIVOT_MAX, f"centroid moved {d:.2f} px (origin {entry['geometry']['origin']} x {k})"))
    if entry["family"] == "level":
        res.append(seams(entry, img, k, path))
    return res


def seams(entry, img, k, path=None):
    ref = np.array(common.reference_image(entry)).astype(float)
    out = np.array(img.convert("RGBA"))                  # uint8: a level canvas is 18432x4608
    bad = tiled = 0

    def edge_diff(a, b):                       # mean RGB difference over pixels opaque on both edges
        a, b = a.astype(float), b.astype(float)
        m = (a[..., 3] > 0) & (b[..., 3] > 0)
        return np.abs(a[m][:, :3] - b[m][:, :3]).mean() if m.sum() >= 4 else None

    for b in entry["geometry"]["blocks"]:
        x, y, w, h = b["rect"]
        if w < 4 or h < 4:
            continue
        r = ref[y:y + h, x:x + w]
        side = path and b["key"] and common.blocks_dir(Path(path)) / f"{b['key']}_{w}x{h}.png"
        o = np.array(Image.open(side).convert("RGBA")) if side and side.is_file() else out[y * k:(y + h) * k, x * k:(x + w) * k]
        for ra, rb, oa, ob in ((r[:, 0], r[:, -1], o[:, 0], o[:, -1]), (r[0], r[-1], o[0], o[-1])):
            rd = edge_diff(ra, rb)
            if rd is not None and rd <= SEAM_TILE:           # the source tiles across this pair of edges
                tiled += 1
                od = edge_diff(oa, ob)
                if od is not None and od > max(SEAM_TOL, 2 * rd):
                    bad += 1
    return ("seams", bad == 0, f"{bad} of {tiled} tiling edge pairs broken")


def run(entry_id: str, path=None) -> list:
    e = common.entries_by_id()[entry_id]
    path = path or common.output_path(e)
    return check(e, Image.open(path), common.manifest()["scale"], path)


if __name__ == "__main__":
    out = run(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
    for name, ok, detail in out:
        print(f"{'ok  ' if ok else 'FAIL'} {name}: {detail}")
    sys.exit(0 if all(ok for _, ok, _ in out) else 1)
