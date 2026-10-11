"""Fit a generated image back onto the game's exact canvases (manual route).

fit.py <pack> <generated.png> [--grow 4] [--no-snap]

Reads remastered/prompts/<pack>/layout.json. The generated image is resized to the pack's sheet size
if the model returned another resolution with the same aspect. Magenta #FF00FF becomes transparent
(soft key with despill). Each cell is resampled (Lanczos, premultiplied alpha) from the pack scale to
the target scale and written into remastered/incoming/<pack>/<entry>.png, which jobs.py picks up with the
'manual' generator (jobs.py take <pack> runs fit + checks + review in one go):
  frame   one file per sprite frame: the frame canvas cut out of the pivot-aligned cell
  region  pasted into the entry's full canvas (level VRAM image, HUD sheet); parts not generated yet
          keep the original texels scaled by nearest neighbour
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
    m = Image.fromarray((ref_alpha > 0).astype(np.uint8) * 255).resize((ref_alpha.shape[1] * k, ref_alpha.shape[0] * k), Image.NEAREST)
    if grow:
        m = m.filter(ImageFilter.MaxFilter(2 * grow + 1))
    out = rgba.copy()
    out[..., 3] = (out[..., 3].astype(float) * (np.array(m) / 255)).astype(np.uint8)
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("pack")
    ap.add_argument("image", type=Path)
    ap.add_argument("--grow", type=int, default=4)
    ap.add_argument("--no-snap", action="store_true")
    a = ap.parse_args(argv)
    fit(a.pack, a.image, a.grow, a.no_snap)
    return 0


def fit(pack_name: str, image: Path, grow: int = 4, no_snap: bool = False) -> list[str]:
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
