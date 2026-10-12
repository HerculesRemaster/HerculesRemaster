"""xBRZ generator: the original art upscaled to k x with index-preserving xBRZ (tools/hires/xbrz_index.py).

Writes remastered/incoming/xbrz/<id with '/' -> '_'>.png and the entry's output path: an 8-bit indexed
PNG (the original palette indices, viewer palette from the reference colours).

  sprite  the whole frame canvas in one pass, so outlines run on across the frame's parts. Indices are
          the frame's parts pasted in draw order (texels from the i1.4 bank, as contract.py keys them);
          colours are the decoded reference frame.
  level, ui, screen
          neighbouring rects in these images are unrelated textures, so each manifest block is upscaled
          on its own (clamped edges; wrapped edges where the source tiles) and pasted smallest first:
          an enclosing rect (UV bounding box, full gauge strip) wins over the rects inside it. Texels
          outside every block keep a nearest-neighbour copy.

Sprites whose xBRZ silhouette differs by more than checks.ALPHA_MAX (dots, 1-texel lines) use Scale3x twice.

settings: {"tolerance": float} (xBRZ equalColorTolerance, default 30).
"""
import os
import shutil

import numpy as np
from PIL import Image

import checks
import common
import xbrz_index

SEAM_TILE = 12.0          # as checks.py: source edges this close (mean abs RGB) count as tiling


def indices(entry: dict, rgba: np.ndarray) -> np.ndarray:
    fam, src = entry["family"], entry["source"]
    if fam == "sprite":
        return common.sprite_indices(entry, rgba)
    if fam == "level":
        return np.frombuffer((common.ROOT / src["file"]).read_bytes(), np.uint8).reshape(512, 2048).copy()
    if fam == "ui":
        return np.frombuffer((common.ROOT / src["file"]).read_bytes()[:256 * 512], np.uint8).reshape(512, 256).copy()
    if fam == "screen":
        return common.screen_indices(entry)
    raise ValueError(f"no index source for family {fam!r}")


def _tiles(rgba: np.ndarray):
    """(vertical, horizontal): whether the block's opposite edges match, as checks.seams tests them."""
    def close(a, b):
        a, b = a.astype(float), b.astype(float)
        m = (a[..., 3] > 0) & (b[..., 3] > 0)
        return m.sum() >= 4 and np.abs(a[m][:, :3] - b[m][:, :3]).mean() <= SEAM_TILE
    h, w = rgba.shape[:2]
    return (h >= 4 and w >= 4 and close(rgba[0], rgba[-1]), h >= 4 and w >= 4 and close(rgba[:, 0], rgba[:, -1]))


def upscale_entry(entry: dict, reference: Image.Image, k: int, tolerance: float, block_sink=None) -> Image.Image:
    """The k x indexed image. For per-block families, block_sink(name, array) also receives each
    block's own result (overlapping rects of one canvas cannot all keep their exact edges)."""
    rgba = np.array(reference.convert("RGBA"))
    idx = indices(entry, rgba)
    if idx.shape != rgba.shape[:2]:
        raise ValueError(f"index image {idx.shape} does not match the reference {rgba.shape[:2]}")
    # the runtime drops texel index 0 only; the decoded references also drop PSX colour 0x0000, which
    # some ENG3 palettes use for other indices: those texels are drawn (colour from the reference: black)
    if entry["family"] == "sprite":
        rgba[..., 3] = np.where(idx != 0, 255, 0)
    pal = xbrz_index.palette_of(idx, rgba)
    if entry["family"] == "sprite":
        out = xbrz_index.upscale(idx, rgba, k, tolerance=tolerance)
        cov = (out != 0).reshape(idx.shape[0], k, idx.shape[1], k).mean((1, 3)) >= 0.5
        if (cov != (idx != 0)).sum() > checks.ALPHA_MAX * max((idx != 0).sum(), 1):
            # tiny sprites (dots, 1-texel lines) that xBRZ rounds away: Scale3x twice keeps every texel
            import smooth
            out = smooth.scale3x(smooth.scale3x(idx[None]))[0] if k == 9 else np.kron(idx, np.ones((k, k), np.uint8))
    else:
        out = np.kron(idx, np.ones((k, k), np.uint8))
        blocks = {}
        for b in entry["geometry"]["blocks"]:
            if b["key"]:
                blocks.setdefault(tuple(b["rect"]), f"{b['key']}_{b['rect'][2]}x{b['rect'][3]}.png")
        jobs = [((x, y, w, h), _tiles(rgba[y:y + h, x:x + w])) for x, y, w, h in blocks]
        jobs.sort(key=lambda j: (any(j[1]), j[0][2] * j[0][3]))         # tiling blocks last: keep their wrap
        for (x, y, w, h), wrap in jobs:
            bi, bc = idx[y:y + h, x:x + w], rgba[y:y + h, x:x + w]
            up = xbrz_index.upscale(bi, bc, k, wrap, tolerance)
            out[y * k:(y + h) * k, x * k:(x + w) * k] = up
            if block_sink:
                block_sink(blocks[(x, y, w, h)], up, pal)
    im = Image.fromarray(out, "P")
    im.putpalette(pal)
    return im


def _publish(src, dst):
    dst.unlink(missing_ok=True)
    try:
        os.link(src, dst)                         # same volume: no second copy on disk
    except OSError:
        shutil.copyfile(src, dst)


def generate(entry, reference, out_path, scale, settings) -> bool:
    inc = common.INCOMING / "xbrz" / (entry["id"].replace("/", "_") + ".png")
    inc_blocks, out_blocks = common.blocks_dir(inc), common.blocks_dir(out_path)
    for d in (inc_blocks, out_blocks):
        shutil.rmtree(d, ignore_errors=True)
    inc.parent.mkdir(parents=True, exist_ok=True)

    def sink(name, arr, pal):
        inc_blocks.mkdir(exist_ok=True)
        out_blocks.mkdir(exist_ok=True)
        im = Image.fromarray(arr, "P")
        im.putpalette(pal)
        im.save(inc_blocks / name, compress_level=6)
        _publish(inc_blocks / name, out_blocks / name)

    im = upscale_entry(entry, reference, scale, float(settings.get("tolerance", xbrz_index.TOLERANCE)), sink)
    im.save(inc, compress_level=6)
    _publish(inc, out_path)
    return True
