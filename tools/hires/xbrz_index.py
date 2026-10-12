"""Index-preserving xBRZ upscaling for 8-bit art (docs/hires-pipeline.md, "xBRZ route").

xBRZ (tools/hires/xbrz_lib, built on first use) runs on the colours to find smooth outlines; every output
pixel then takes the palette INDEX of one source texel in its 3x3 neighbourhood (the one whose colour is
closest to the xBRZ colour, the centre texel first on ties). The result is an 8-bit index image at k x:
no new indices, index 0 stays the transparent texel, and the runtime applies the live CLUT, so palette
swaps, fades and semi-transparency stay exact.

xBRZ scales by at most 6; k = 9 is reached by a linear 6 -> 9 resample before the index pick.

upscale(idx, rgba, k, wrap=(False, False)) -> (k*h, k*w) uint8
    idx   (h, w) uint8 palette indices
    rgba  (h, w, 4) uint8 colour of each texel (alpha 0 where idx == 0)
    wrap  (vertical, horizontal): pad by wrapping instead of clamping (textures that tile)
"""
import ctypes
import subprocess
from functools import lru_cache
from pathlib import Path

import cv2
import numpy as np

XBRZ_DIR = Path(__file__).resolve().parent / "xbrz_lib"
MAX_FACTOR = 6
PAD = 3                                   # texels of context around a block (xBRZ looks 2 texels out)
TOLERANCE = 30.0                          # xBRZ equalColorTolerance (its default)
ALPHA_WEIGHT = 2.0                        # alpha difference counts double in the index pick


@lru_cache(maxsize=1)
def _dll():
    dll = XBRZ_DIR / "xbrz.dll"
    if not dll.exists():
        subprocess.run(["cmd", "/c", str(XBRZ_DIR / "build.bat")], check=True, capture_output=True)
    lib = ctypes.CDLL(str(dll))
    lib.xbrz_scale.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_double]
    lib.xbrz_scale.restype = None
    return lib


def xbrz(rgba: np.ndarray, factor: int, tolerance: float = TOLERANCE) -> np.ndarray:
    """(h, w, 4) RGBA -> (factor*h, factor*w, 4) RGBA."""
    h, w = rgba.shape[:2]
    src = np.ascontiguousarray(rgba[..., [2, 1, 0, 3]]).view(np.uint32).reshape(h, w)   # ARGB words
    out = np.zeros((h * factor, w * factor), np.uint32)
    _dll().xbrz_scale(factor, src.ctypes.data, out.ctypes.data, w, h, tolerance)
    return out.view(np.uint8).reshape(h * factor, w * factor, 4)[..., [2, 1, 0, 3]]


def _resample(rgba: np.ndarray, size) -> np.ndarray:
    """Linear resize with premultiplied alpha, float32 result."""
    a = rgba.astype(np.float32)
    a[..., :3] *= a[..., 3:4] / 255
    a = cv2.resize(a, size, interpolation=cv2.INTER_LINEAR)
    a[..., :3] = np.where(a[..., 3:4] > 0, a[..., :3] * 255 / np.maximum(a[..., 3:4], 1e-6), 0)
    return a


def _pad(a: np.ndarray, wrap) -> np.ndarray:
    for axis, w in enumerate(wrap):
        width = [(0, 0)] * a.ndim
        width[axis] = (PAD, PAD)
        a = np.pad(a, width, mode="wrap" if w else "edge")
    return a


def upscale(idx: np.ndarray, rgba: np.ndarray, k: int, wrap=(False, False), tolerance: float = TOLERANCE) -> np.ndarray:
    h, w = idx.shape
    if h == 0 or w == 0:
        return np.zeros((h * k, w * k), np.uint8)
    col = rgba.copy()
    col[idx == 0] = 0
    pi, pc = _pad(idx, wrap), _pad(col, wrap)
    H, W = pi.shape
    f = min(k, MAX_FACTOR)
    big = xbrz(pc, f, tolerance) if f > 1 else pc
    big = _resample(big, (W * k, H * k)) if f != k else big.astype(np.float32)
    big = big[PAD * k:(PAD + h) * k, PAD * k:(PAD + w) * k].reshape(h, k, w, k, 4)   # k x k cell per texel
    weight = np.sqrt(np.array([1, 1, 1, ALPHA_WEIGHT], np.float32))
    big = big * weight
    pcf = pc.astype(np.float32) * weight
    best = np.full((h, k, w, k), np.inf, np.float32)
    out = np.zeros((h, k, w, k), np.uint8)
    for dy, dx in [(0, 0)] + [(dy, dx) for dy in (-1, 0, 1) for dx in (-1, 0, 1) if dy or dx]:
        ys, xs = slice(PAD + dy, PAD + dy + h), slice(PAD + dx, PAD + dx + w)
        diff = big - pcf[ys, xs][:, None, :, None, :]      # each candidate texel against its whole cell
        d = np.einsum("...c,...c->...", diff, diff)
        sel = d < best
        best[sel] = d[sel]
        out[sel] = np.broadcast_to(pi[ys, xs][:, None, :, None], out.shape)[sel]
    return out.reshape(h * k, w * k)


def palette_of(idx: np.ndarray, rgba: np.ndarray) -> list:
    """A 256-entry RGB palette from the texels (index -> its colour; unused entries grey), so the
    indexed PNG looks right in a viewer. The runtime ignores it and uses the live CLUT."""
    pal = np.repeat(np.arange(256, dtype=np.uint8)[:, None], 3, 1)
    flat_i, flat_c = idx.reshape(-1), rgba.reshape(-1, 4)
    opaque = flat_c[:, 3] > 0
    used, first = np.unique(flat_i[opaque], return_index=True)
    pal[used] = flat_c[opaque][first, :3]
    return pal.reshape(-1).tolist()
