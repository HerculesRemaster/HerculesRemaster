"""Pack approved replacement images into runtime key files and report texture memory.

atlas.py [--art-dir working/game/art] [--allow-partial]   (normally called by jobs.py pack)

Each manifest block is cut from the approved image at rect x scale and written as
<art_dir>/<family>/<key>_<w>x<h>.png (families: sprite, level, ui, screen; the ini's art_families picks
which the game loads) (indexed images stay indexed, others RGBA). Every block is its own texture
and the renderer reads it with integer loads (no filtering, no neighbours), so no padding or
extrusion is needed. A sprite bank is packed only when all its frames are approved (frame-count
check), unless --allow-partial. A key that appears in several entries is written once (first wins).
"""
import argparse
import json
import shutil
from collections import defaultdict

import numpy as np
from PIL import Image, ImageFilter

import checks
import common


def _tiles(a: np.ndarray, b: np.ndarray) -> bool:
    m = (a[..., 3] > 0) & (b[..., 3] > 0)
    return m.sum() >= 4 and np.abs(a[m][:, :3].astype(float) - b[m][:, :3]).mean() <= checks.SEAM_TILE


FLAT_STD = 10.0


def flat_fill(ref: np.ndarray, k: int):
    """Smooth k x upscale of a flat, opaque source block (ref: RGBA texels), or None if it has detail."""
    h, w = ref.shape[:2]
    if h < 4 or w < 4 or (ref[..., 3] == 0).any():
        return None
    p = 4
    pad = np.pad(ref[..., :3], ((p, p), (p, p), (0, 0)), mode="wrap")
    blur = np.array(Image.fromarray(pad.astype(np.uint8)).filter(ImageFilter.GaussianBlur(1.5)))
    if blur[p + 1:-p - 1, p + 1:-p - 1].reshape(-1, 3).astype(float).std(0).max() > FLAT_STD:
        return None
    big = Image.fromarray(blur).resize(((w + 2 * p) * k, (h + 2 * p) * k), Image.BICUBIC)
    return big.crop((p * k, p * k, (p + w) * k, (p + h) * k)).convert("RGBA")


def make_periodic(img: Image.Image, ref: np.ndarray, k: int) -> Image.Image:
    """Blend the edges of a redrawn level block so it tiles where its source block tiles (ref: RGBA texels).
    Returns img itself when the source does not tile."""
    a = np.array(img.convert("RGBA")).astype(float)
    changed = False
    for axis in (1, 0):                                   # 1: left/right edges, 0: top/bottom edges
        if ref.shape[axis] < 4 or not _tiles(np.take(ref, 0, axis), np.take(ref, -1, axis)):
            continue
        n = a.shape[axis]
        band = max(2, min(n // 3, 4 * k))
        d = (np.take(a, 0, axis) - np.take(a, -1, axis))[:, :3] / 2          # (edge length, 3)
        g = np.exp(-0.5 * (np.arange(-3 * k, 3 * k + 1) / (1.5 * k)) ** 2)
        g /= g.sum()
        d = np.stack([np.convolve(np.pad(d[:, c], 3 * k, mode="edge"), g, "valid") for c in range(3)], -1)
        for i in range(band):
            w = (1 - i / band) ** 2                       # 1 at the edge -> 0 inside the band
            lo, hi = [slice(None)] * 3, [slice(None)] * 3
            lo[axis], hi[axis], lo[2], hi[2] = i, n - 1 - i, slice(0, 3), slice(0, 3)
            a[tuple(lo)] -= d * w
            a[tuple(hi)] += d * w
        changed = True
    return Image.fromarray(np.clip(a, 0, 255).astype(np.uint8), "RGBA") if changed else img


def pack(approved_ids: list[str], art_dir=common.ART_DIR, allow_partial=False, write_report=True) -> dict:
    ents = common.entries_by_id()
    k = common.manifest()["scale"]
    banks = defaultdict(set)
    for e in ents.values():
        if e["family"] == "sprite":
            banks[e["source"]["index"]].add(e["id"])
    seq_of = {}
    for sq in json.loads((common.ROOT / "tools" / "hires" / "pilot.json").read_text(encoding="utf-8"))["sequences"]:
        ids = {f"sprite/{sq['bank']:04d}/f{f:04d}" for f in sq["frames"]}
        for i in ids:
            seq_of[i] = (sq["name"], ids)
    ok_ids, skipped = [], defaultdict(int)
    for i in approved_ids:
        e = ents[i]
        if i in seq_of and not seq_of[i][1] <= set(approved_ids):
            skipped[f"sequence {seq_of[i][0]} incomplete"] += 1
            continue
        if e["family"] == "sprite" and not allow_partial and not banks[e["source"]["index"]] <= set(approved_ids):
            skipped[f"bank {e['source']['index']:04d} incomplete"] += 1
            continue
        ok_ids.append(i)
    art_dir.mkdir(parents=True, exist_ok=True)
    for fam in ("sprite", "level", "ui", "screen"):
        shutil.rmtree(art_dir / fam, ignore_errors=True)
    periodic = flats = 0
    written, conflicts, mem, refs = set(), 0, defaultdict(int), {}
    for i in ok_ids:
        e = ents[i]
        img = Image.open(common.output_path(e))
        rgba = img.mode != "P"
        if rgba:
            img = img.convert("RGBA")
        for b in e["geometry"]["blocks"]:
            if not b["key"]:
                continue
            x, y, w, h = b["rect"]
            name = f"{b['key']}_{w}x{h}.png"
            if name in written:
                conflicts += 1
                continue
            (art_dir / e["family"]).mkdir(parents=True, exist_ok=True)
            crop = img.crop((x * k, y * k, (x + w) * k, (y + h) * k))
            if e["family"] == "level" and rgba:
                ref = refs.setdefault(e["id"], np.array(common.reference_image(e).convert("RGBA")))[y:y + h, x:x + w]
                bg = any(ux <= x and uy <= y and x + w <= ux + uw and y + h <= uy + uh
                         for ux, uy, uw, uh in e["source"].get("uploads", [])[1:])
                flat = flat_fill(ref, k) if bg else None
                if flat is not None:
                    crop, flats = flat, flats + 1
                else:
                    fixed = make_periodic(crop, ref, k)
                    periodic += fixed is not crop
                    crop = fixed
            crop.save(art_dir / e["family"] / name)
            written.add(name)
            mem[e["family"]] += w * h * k * k * (4 if rgba else 1)
    report = {"entries": len(ok_ids), "skipped": dict(skipped), "files": len(written), "duplicate_keys": conflicts,
              "periodic_level_blocks": periodic, "flat_level_blocks": flats,
              "texture_mb": {f: round(v / 2 ** 20, 1) for f, v in mem.items()},
              "texture_mb_total": round(sum(mem.values()) / 2 ** 20, 1)}
    if write_report:
        (common.REMASTERED / "pack-report.json").write_text(json.dumps(report, indent=1), encoding="utf-8")
    return {"packed": ok_ids, "report": report}


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--art-dir", default=str(common.ART_DIR))
    ap.add_argument("--allow-partial", action="store_true")
    a = ap.parse_args()
    jobs = json.loads(common.JOBS.read_text(encoding="utf-8"))["jobs"]
    ids = [j["entry"] for j in jobs.values() if j["state"] in ("approved", "packed")]
    from pathlib import Path
    print(json.dumps(pack(ids, Path(a.art_dir), a.allow_partial)["report"], indent=1))
