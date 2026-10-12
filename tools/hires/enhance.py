"""Extract redrawn sprite frames from enhanced contact sheets and pack them as 4x RGBA key files.

enhance.py [--sheets extracted/images/enhance] [--k 4] [--art-dir working/game/art] [--only 0498,...]

Input: the user's enhanced versions of extracted/images/_sheets/sprites_<bank>.png (tools/viewer/contact_sheet.py:
the first 48 frames of a bank, 8 per row, each labelled f<NNNN>). A sheet that is
pixel-identical to its _sheets original is skipped. The enhanced sheets carry their own alpha matte
(background removed), but their layout and scale are free: the grid, spacing and figure size changed.

Per sheet (the enhanced sheets keep the 8-column grid and row count, but labels may sit above or below a
figure, in any colour, so they are not used to place cells):
  1. cells: the original sheet's cells are identified by pixel match with the frame files (sheet_frames);
     the enhanced sheet is cut into the same 8 x rows grid at the emptiest alpha lines near the even split;
  2. figure: the cell's alpha without its label (text-sized blobs at the top or bottom of the cell);
  3. fit: scale + offset that map the figure's silhouette onto the original frame's (max IoU, coarse-to-fine),
     then premultiplied bicubic warp into the frame canvas at k x;
  4. snap: alpha limited to the original silhouette grown by one texel (fit.snap), so the logical outline,
     size and pivot stay the original's;
  5. pack: each manifest block of the frame is cut at rect x k and written as <art_dir>/enhance/<key>_<w>x<h>.png
     (true colour; first frame wins when a key is shared).
Canvases go to remastered/enhance/<bank>/f<NNNN>.png, the per-frame fit to remastered/enhance/report.json.
"""
import argparse
import json
import re
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).parent))
import common  # noqa: E402
import fit  # noqa: E402

SHEETS = common.ROOT / "extracted" / "images" / "_sheets"
OUT = common.REMASTERED / "enhance"


def dilate(mask: np.ndarray, rx: int, ry: int) -> np.ndarray:
    """Binary dilation by a (2*ry+1) x (2*rx+1) box."""
    pad = np.pad(mask, ((ry, ry), (rx, rx)))
    acc = np.zeros_like(mask)
    for dy in range(2 * ry + 1):
        for dx in range(2 * rx + 1):
            acc |= pad[dy:dy + mask.shape[0], dx:dx + mask.shape[1]]
    return acc


def components(mask: np.ndarray) -> list:
    """Bounding boxes (y0, y1, x0, x1) of the 8-connected components of a sparse mask (row runs + union-find)."""
    parent = []

    def find(i):
        while parent[i] != i:
            parent[i] = parent[parent[i]]
            i = parent[i]
        return i

    runs, prev = [], []
    for y in range(mask.shape[0]):
        row = mask[y]
        if not row.any():
            prev = []
            continue
        d = np.diff(np.concatenate([[0], row.astype(np.int8), [0]]))
        starts, ends = np.nonzero(d == 1)[0], np.nonzero(d == -1)[0]
        cur = []
        for x0, x1 in zip(starts, ends):
            i = len(parent)
            parent.append(i)
            runs.append((y, x0, x1))
            for j, (_, px0, px1) in prev:
                if px0 <= x1 and x0 <= px1:                 # touching, diagonals included
                    a, b = find(i), find(j)
                    if a != b:
                        parent[b] = a
            cur.append((i, (y, x0, x1)))
        prev = cur
    boxes = {}
    for i, (y, x0, x1) in enumerate(runs):
        r = find(i)
        b = boxes.get(r)
        boxes[r] = (y, y + 1, x0, x1) if b is None else (min(b[0], y), max(b[1], y + 1), min(b[2], x0), max(b[3], x1))
    return list(boxes.values())




def sheet_frames(bank: str) -> list:
    """Frame stems shown on the original sheet, in reading order. The sheet may predate the current frame list
    (contact_sheet.py: sorted f*.png, first 48, 8 per row, cell = max frame size + (4, 12), drawn 2x), so each
    cell is identified by its pixels: the frame image (2x nearest) at cell + (2, 2) that matches it exactly."""
    sheet = np.asarray(Image.open(SHEETS / f"sprites_{bank}.png").convert("RGB")).astype(int)
    r, g, b = sheet[..., 0], sheet[..., 1], sheet[..., 2]
    lab = (np.abs(r - g) <= 2) & (r >= 200) & (r - b >= 50)            # label text (255, 255, 160), antialiased
    ys, xs = np.nonzero(lab)
    if not len(ys):
        return []
    rows = [int(v) for v in np.unique(ys)]
    row_tops = [rows[0]] + [r for p, r in zip(rows, rows[1:]) if r - p > 20]   # first label pixel row per row of cells
    frames = {p.stem: np.asarray(Image.open(p).convert("RGBA")) for p in
              sorted((common.ROOT / "extracted" / "sprites" / bank).glob("f*.png"))}
    cw = sheet.shape[1] // 16                            # 8 columns at 2x
    ch = sheet.shape[0] // (2 * len(row_tops))
    out = []
    for ly in row_tops:
        cy = (int(ly) // 2 // ch) * ch                   # cell row of this label row
        band = lab[ly:ly + 16]
        cols = sorted({int(x) // 2 // cw for x in np.nonzero(band.any(0))[0]})
        for c in cols:
            x, y = c * cw * 2, cy * 2
            best, score = None, -1.0
            for stem, f in frames.items():
                h, w = f.shape[:2]
                reg = sheet[y + 4:y + 4 + 2 * h, x + 4:x + 4 + 2 * w]
                if reg.shape[:2] != (2 * h, 2 * w):
                    continue
                big = np.kron(f, np.ones((2, 2, 1), int))
                m = big[..., 3] > 0
                if not m.any():
                    continue
                v = (reg[m] == big[m][:, :3]).all(-1).mean()
                if v > score:
                    best, score = stem, v
            out.append(best if score > 0.98 else None)
    return out


def boundaries(occ: np.ndarray, n: int) -> list:
    """n cells along one axis: each inner boundary is the emptiest line (least alpha) within +-1/4 cell of
    its even position."""
    L = len(occ)
    sm = np.convolve(occ, np.ones(9) / 9, mode="same")
    out = [0]
    for i in range(1, n):
        c, r = i * L / n, L / n / 4
        lo, hi = int(c - r), int(c + r) + 1
        out.append(lo + int(np.argmin(sm[lo:hi])))
    return out + [L]


def strip_labels(cell: np.ndarray) -> np.ndarray:
    """Clear frame labels from a cell's alpha: text-sized word blobs in the top or bottom fifth of the cell."""
    m = cell[..., 3] > 127
    H, W = m.shape
    out = cell.copy()
    for y0, y1, x0, x1 in components(dilate(m, max(2, W // 60), 1)):
        h, w = y1 - y0, x1 - x0
        cy = (y0 + y1) / 2
        if h < 0.14 * H and w < 0.7 * W and w > 1.5 * h and (cy < 0.2 * H or cy > 0.8 * H):
            out[max(0, y0 - 2):y1 + 2, max(0, x0 - 2):x1 + 2, 3] = 0
    return out


def iou(a, b):
    u = (a | b).sum()
    return float((a & b).sum() / u) if u else 0.0


def place(mask, s, t, size):
    """Nearest warp of a figure mask into a canvas: canvas p = s * figure p + t."""
    H, W = size
    ys, xs = np.mgrid[0:H, 0:W]
    sx = np.floor((xs + 0.5 - t[0]) / s).astype(int)
    sy = np.floor((ys + 0.5 - t[1]) / s).astype(int)
    ok = (sx >= 0) & (sx < mask.shape[1]) & (sy >= 0) & (sy < mask.shape[0])
    out = np.zeros(size, bool)
    out[ok] = mask[sy[ok], sx[ok]]
    return out


def fit_figure(fig_mask, ref_mask, d=2):
    """Scale s and offset t (canvas = s * figure + t) with max IoU, searched on a 1/d canvas."""
    r = ref_mask[::d, ::d]
    yf, xf = np.nonzero(fig_mask)
    yr, xr = np.nonzero(r)
    s = np.sqrt(len(yr) / len(yf))
    t = np.array([xr.mean() - s * xf.mean(), yr.mean() - s * yf.mean()])
    best = (iou(place(fig_mask, s, t, r.shape), r), s, t)
    for ds, dt, n in ((0.06, 3.0, 3), (0.02, 1.0, 2), (0.007, 0.5, 2)):
        s0, t0 = best[1], best[2]
        for i in range(-n, n + 1):
            for jx in range(-n, n + 1):
                for jy in range(-n, n + 1):
                    ss = s0 * (1 + i * ds)
                    tt = t0 + np.array([jx, jy]) * dt
                    v = iou(place(fig_mask, ss, tt, r.shape), r)
                    if v > best[0]:
                        best = (v, ss, tt)
    return best[0], best[1] * d, best[2] * d


def warp(fig_rgba, s, t, size):
    """Premultiplied bicubic warp of the figure into the (H, W) canvas."""
    H, W = size
    coeffs = (1 / s, 0, -t[0] / s, 0, 1 / s, -t[1] / s)
    al = fig_rgba[..., 3:4].astype(float) / 255
    pre = Image.fromarray((fig_rgba[..., :3] * al).astype(np.uint8))
    rgb = np.array(pre.transform((W, H), Image.AFFINE, coeffs, Image.BICUBIC)).astype(float)
    a2 = np.array(Image.fromarray(fig_rgba[..., 3]).transform((W, H), Image.AFFINE, coeffs, Image.BICUBIC)).astype(float) / 255
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(a2[..., None] > 0.004, rgb / a2[..., None], 0)
    return np.dstack([np.clip(rgb, 0, 255), a2 * 255]).astype(np.uint8)


def do_sheet(path: Path, k: int, art: Path, ents: dict, written: set, report: dict) -> str:
    bank = path.stem[len("sprites_"):]
    orig = SHEETS / path.name
    img = Image.open(path).convert("RGBA")
    if orig.exists() and Image.open(orig).size == img.size and \
            np.array_equal(np.asarray(Image.open(orig).convert("RGBA")), np.asarray(img)):
        return "unchanged"
    rgba = np.asarray(img)
    if (rgba[..., 3] < 255).mean() < 0.05:
        return "skipped: no alpha matte"
    frames = sheet_frames(bank)
    if not frames:
        return "skipped: original sheet not readable"
    idx = int(bank[:4])
    rows = (len(frames) + 7) // 8
    cols = 8 if len(frames) >= 8 else len(frames)
    occ = rgba[..., 3] > 127
    xb = boundaries(occ.sum(0).astype(float), cols)
    yb = boundaries(occ.sum(1).astype(float), rows)
    done = 0
    for n, stem in enumerate(frames):
        e = ents.get(f"sprite/{idx:04d}/{stem}") if stem else None
        if e is None:
            continue
        r, c = divmod(n, 8)
        cell = strip_labels(rgba[yb[r]:yb[r + 1], xb[c]:xb[c + 1]])
        ref = np.array(common.reference_image(e))[..., 3] > 0
        W, H = e["geometry"]["size"]
        if (cell[..., 3] > 127).sum() < 16 or ref.sum() < 4:
            report[e["id"]] = {"status": "empty"}
            continue
        ys, xs = np.nonzero(cell[..., 3] > 8)
        cell = cell[ys.min():ys.max() + 1, xs.min():xs.max() + 1]
        fm = cell[..., 3] > 127
        rk = np.kron(ref, np.ones((k, k), bool))
        v, s, t = fit_figure(fm, rk)
        out = fit.snap(warp(cell, s, t, (H * k, W * k)), np.array(common.reference_image(e))[..., 3], k, k)
        dst = OUT / f"{idx:04d}" / f"{stem}.png"
        dst.parent.mkdir(parents=True, exist_ok=True)
        Image.fromarray(out, "RGBA").save(dst)
        report[e["id"]] = {"iou": round(v, 3), "scale": round(s, 3)}
        for b in e["geometry"]["blocks"]:
            if not b["key"]:
                continue
            bx, by, bw, bh = b["rect"]
            name = f"{b['key']}_{bw}x{bh}.png"
            if name in written:
                continue
            Image.fromarray(out[by * k:(by + bh) * k, bx * k:(bx + bw) * k], "RGBA").save(art / name, optimize=True)
            written.add(name)
        done += 1
    return f"{done}/{len(frames)} frames"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--sheets", type=Path, default=common.ROOT / "extracted" / "images" / "enhance")
    ap.add_argument("--k", type=int, default=4)
    ap.add_argument("--art-dir", type=Path, default=common.ART_DIR)
    ap.add_argument("--only", default="", help="comma-separated bank indices")
    ap.add_argument("--verbose", action="store_true")
    a = ap.parse_args(argv)
    only = {int(x) for x in a.only.split(",") if x}
    art = a.art_dir / "enhance"
    art.mkdir(parents=True, exist_ok=True)
    if not only:
        for f in art.glob("*.png"):
            f.unlink()
    ents = common.entries_by_id()
    written, report, status = set(), {}, {}
    for p in sorted(a.sheets.glob("sprites_*.png")):
        m = re.match(r"sprites_(\d{4})_", p.name)
        if not m or (only and int(m.group(1)) not in only):
            continue
        status[p.stem] = do_sheet(p, a.k, art, ents, written, report)
        if a.verbose:
            print(p.stem, status[p.stem])
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "report.json").write_text(json.dumps({"k": a.k, "sheets": status, "frames": report}, indent=1), encoding="utf-8")
    fits = [r["iou"] for r in report.values() if "iou" in r]
    used = [s for s in status.values() if s[0].isdigit()]
    skipped = {n: s for n, s in status.items() if s.startswith("skipped")}
    size = sum(f.stat().st_size for f in art.glob("*.png"))
    print(f"sheets: {len(used)} extracted, {sum(1 for s in status.values() if s == 'unchanged')} unchanged, {len(skipped)} skipped; "
          f"frames {len(fits)}, IoU median {np.median(fits) if fits else 0:.3f}, < 0.6: {sum(1 for x in fits if x < 0.6)}; "
          f"{len(written)} key files, {size / 2**20:.1f} MiB -> {art}")
    for n, s in skipped.items():
        print(f"  {n}: {s}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
