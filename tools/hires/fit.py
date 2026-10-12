"""Fit a generated image back onto the game's exact canvases (manual route).

fit.py <pack> <generated.png> [--grow 9] [--no-snap]

Reads remastered/prompts/<pack>/layout.json. The generated image is resized to the pack's sheet size
if the model returned another resolution with the same aspect. Magenta #FF00FF becomes transparent
(soft key with despill). Each cell is resampled (Lanczos, premultiplied alpha) from the pack scale to
the target scale and written into remastered/incoming/<pack>/<entry>.png, which jobs.py picks up with the
'manual' generator (jobs.py take <pack> runs fit + checks + review in one go):
  frame   one file per sprite frame: the frame canvas cut out of the pivot-aligned cell
  region  pasted into the entry's full canvas (level VRAM image, HUD sheet); parts not generated yet
          keep the original texels scaled by nearest neighbour
Align (default for frames, --no-align to skip): models redraw figures larger or shifted inside the cell;
each frame cell is first scaled (±30 %) and moved so its silhouette best overlaps the original one (IoU).
Snap (default): alpha is limited to the original silhouette grown by --grow output pixels, which
removes stray background and keeps the logical outline (checks.py: alpha, pivot).
"""
import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image, ImageFilter

import common

KEY = np.array([255, 0, 255], float)


def key_out(img: Image.Image) -> np.ndarray:
    a = np.array(img.convert("RGB")).astype(float)
    d = np.sqrt(((a - KEY) ** 2).sum(-1))
    alpha = np.clip((d - 60) / 60, 0, 1)
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(alpha[..., None] > 0, (a - (1 - alpha[..., None]) * KEY) / alpha[..., None], 0)
    # magenta spill the model paints into opaque pixels along the edge: within 6 px of the
    # background, red and blue may not both exceed green (pink -> neutral)
    near = np.array(Image.fromarray(((alpha < 0.99) * 255).astype(np.uint8)).filter(ImageFilter.MaxFilter(13))) > 0
    spill = np.clip(np.minimum(rgb[..., 0], rgb[..., 2]) - rgb[..., 1], 0, None) * near
    rgb[..., 0] -= spill
    rgb[..., 2] -= spill
    return np.dstack([np.clip(rgb, 0, 255), alpha * 255]).astype(np.uint8)


def resample(rgba: np.ndarray, size) -> np.ndarray:
    """Lanczos resize with premultiplied alpha (no dark or magenta fringes)."""
    a = rgba[..., 3:4].astype(float) / 255
    pre = Image.fromarray((rgba[..., :3] * a).astype(np.uint8))
    rgb = np.array(pre.resize(size, Image.LANCZOS)).astype(float)
    al = np.array(Image.fromarray(rgba[..., 3]).resize(size, Image.LANCZOS)).astype(float) / 255
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(al[..., None] > 0.004, rgb / al[..., None], 0)
    return np.dstack([np.clip(rgb, 0, 255), al * 255]).astype(np.uint8)


def snap(rgba: np.ndarray, ref_alpha: np.ndarray, k: int, grow: int) -> np.ndarray:
    """Limit alpha to the original silhouette grown by `grow` output pixels. The k x nearest mask is
    blurred and re-thresholded so the limit follows a rounded outline instead of k-pixel stair steps,
    with a soft 1-2 px edge."""
    m = Image.fromarray((ref_alpha > 0).astype(np.uint8) * 255).resize((ref_alpha.shape[1] * k, ref_alpha.shape[0] * k), Image.NEAREST)
    if grow:
        m = m.filter(ImageFilter.MaxFilter(2 * grow + 1))
    m = np.array(m.filter(ImageFilter.GaussianBlur(k * 0.6))).astype(float) / 255
    m = np.clip((m - 0.5) * 3 + 0.5, 0, 1)
    out = rgba.copy()
    out[..., 3] = (out[..., 3].astype(float) * m).astype(np.uint8)
    return out


def iou(a: np.ndarray, b: np.ndarray) -> float:
    u = (a | b).sum()
    return float((a & b).sum() / u) if u else 0.0


def shift_scale(m: np.ndarray, sigma: float, cg, cr, d) -> np.ndarray:
    """Nearest-neighbour warp of a mask: p_out = sigma * (p_in - cg) + cr + d."""
    h, w = m.shape
    ys, xs = np.mgrid[0:h, 0:w]
    sx = np.round((xs - cr[0] - d[0]) / sigma + cg[0]).astype(int)
    sy = np.round((ys - cr[1] - d[1]) / sigma + cg[1]).astype(int)
    ok = (sx >= 0) & (sx < w) & (sy >= 0) & (sy < h)
    out = np.zeros_like(m)
    out[ok] = m[sy[ok], sx[ok]]
    return out


def align(cell: np.ndarray, ref_mask: np.ndarray, max_scale: float = 0.3):
    """Models often redraw a figure larger or shifted inside its cell. Find the scale + offset that maps the
    generated silhouette (alpha > 50 %) onto the original one (max IoU, coarse-to-fine search on a 1/2-size
    grid) and warp the cell with it. Returns (cell, info)."""
    g = cell[..., 3] > 127
    r = ref_mask
    if g.sum() < 16 or r.sum() < 16:
        return cell, None
    g2, r2 = g[::2, ::2], r[::2, ::2]
    yg, xg = np.nonzero(g2)
    yr, xr = np.nonzero(r2)
    cg, cr = (xg.mean(), yg.mean()), (xr.mean(), yr.mean())
    s0 = float(np.clip(np.sqrt(r2.sum() / g2.sum()), 1 - max_scale, 1 + max_scale))
    best = (iou(g2, r2), 1.0, (0.0, 0.0), True)                  # identity first
    s, d = s0, (0.0, 0.0)
    for step_s, step_d, n in ((0.04, 4, 3), (0.01, 1, 3)):
        cand = []
        for i in range(-n, n + 1):
            for j in range(-n, n + 1):
                for k in range(-n, n + 1):
                    ss = s + i * step_s
                    if abs(ss - 1) <= max_scale:
                        cand.append((ss, (d[0] + j * step_d, d[1] + k * step_d)))
        for ss, dd in cand:
            v = iou(shift_scale(g2, ss, cg, cr, dd), r2)
            if v > best[0]:
                best = (v, ss, dd, False)
        if not best[3]:
            s, d = best[1], best[2]
    before = iou(g2, r2)
    if best[3] or best[0] < before + 0.02:                       # not worth a resample
        return cell, {"iou": round(before, 3), "aligned": False}
    sigma, dd = best[1], best[2]
    cgf, crf, ddf = (cg[0] * 2, cg[1] * 2), (cr[0] * 2, cr[1] * 2), (dd[0] * 2, dd[1] * 2)
    # inverse affine for PIL: src = (dst - cr - d) / sigma + cg
    coeffs = (1 / sigma, 0, cgf[0] - (crf[0] + ddf[0]) / sigma, 0, 1 / sigma, cgf[1] - (crf[1] + ddf[1]) / sigma)
    al = cell[..., 3:4].astype(float) / 255
    pre = Image.fromarray((cell[..., :3] * al).astype(np.uint8))
    size = (cell.shape[1], cell.shape[0])
    rgb = np.array(pre.transform(size, Image.AFFINE, coeffs, Image.BICUBIC)).astype(float)
    a2 = np.array(Image.fromarray(cell[..., 3]).transform(size, Image.AFFINE, coeffs, Image.BICUBIC)).astype(float) / 255
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(a2[..., None] > 0.004, rgb / a2[..., None], 0)
    out = np.dstack([np.clip(rgb, 0, 255), a2 * 255]).astype(np.uint8)
    return out, {"iou": round(before, 3), "aligned": True, "iou_after": round(best[0], 3),
                 "scale": round(sigma, 3), "shift_px": [ddf[0], ddf[1]]}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("pack")
    ap.add_argument("image", type=Path)
    ap.add_argument("--grow", type=int, default=9)
    ap.add_argument("--no-snap", action="store_true")
    ap.add_argument("--no-align", action="store_true")
    a = ap.parse_args(argv)
    fit(a.pack, a.image, a.grow, a.no_snap, a.no_align)
    return 0


def fit(pack_name: str, image: Path, grow: int = 9, no_snap: bool = False, no_align: bool = False) -> list[str]:
    """Fit one generated sheet; returns the entry ids written."""
    a = argparse.Namespace(pack=pack_name, image=Path(image), grow=grow, no_snap=no_snap)
    lay = common.layout(a.pack)
    k = common.manifest()["scale"]
    gen = Image.open(a.image)
    W, H = lay["sheet"]
    if gen.size != (W, H):
        if abs(gen.width / gen.height - W / H) > 0.02:
            raise SystemExit(f"generated image is {gen.size}, the pack expects {W}x{H} (aspect differs)")
        gen = gen.convert("RGB").resize((W, H), Image.LANCZOS)
    keyed = key_out(gen)
    ents = common.entries_by_id()
    inc = common.INCOMING / a.pack
    done = []
    inc.mkdir(parents=True, exist_ok=True)
    s = lay["scale"]
    for c in lay["cells"]:
        e = ents[c["entry"]]
        x, y = c["at"]
        w, h = c["size"]
        cell = keyed[y:y + h, x:x + w]
        cw, ch = w // s, h // s
        if c["kind"] == "frame" and not no_align:
            ox, oy = c["offset"]
            ra = np.array(common.reference_image(e))[..., 3] > 0
            rm = np.zeros((ch, cw), bool)
            rm[oy:oy + ra.shape[0], ox:ox + ra.shape[1]] = ra[:ch - oy, :cw - ox]
            rm = np.kron(rm, np.ones((s, s), bool))[:h, :w]
            if rm.shape == cell.shape[:2]:
                cell, info = align(cell, rm)
                if info and info["aligned"]:
                    print(f"{e['id']}: aligned {info}")
        big = resample(cell, (cw * k, ch * k))
        target = inc / (e["id"].replace("/", "_") + ".png")
        if c["kind"] == "frame":
            ox, oy = c["offset"]
            fw, fh = c["canvas"]
            out = big[oy * k:(oy + fh) * k, ox * k:(ox + fw) * k]
            if not a.no_snap:
                out = snap(out, np.array(common.reference_image(e))[..., 3], k, a.grow)
            Image.fromarray(out, "RGBA").save(target)
        else:
            rx, ry, rw, rh = c["rect"]
            ref = common.reference_image(e)
            prev = common.incoming_file(e["id"])        # pieces from earlier packs add up
            if prev is not None:
                canvas = Image.open(prev).convert("RGBA")
            else:
                canvas = ref.resize((ref.width * k, ref.height * k), Image.NEAREST)
            part = big
            if not a.no_snap:
                part = snap(part, np.array(ref.crop((rx, ry, rx + rw, ry + rh)))[..., 3], k, a.grow)
            canvas.paste(Image.fromarray(part, "RGBA"), (rx * k, ry * k))
            canvas.save(target)
        print(f"{e['id']} -> {target.relative_to(common.ROOT)}")
        done.append(e["id"])
    return done


if __name__ == "__main__":
    raise SystemExit(main())
