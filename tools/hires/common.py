"""Shared helpers for the replacement-art tools (docs/hires-pipeline.md)."""
import json
from functools import lru_cache
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
REMASTERED = ROOT / "remastered"
MANIFEST = REMASTERED / "manifest.json"
JOBS = REMASTERED / "jobs.json"
PROMPTS = REMASTERED / "prompts"
INCOMING = REMASTERED / "incoming"
ART_DIR = ROOT / "working" / "game" / "art"


@lru_cache(maxsize=1)
def manifest() -> dict:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def entries_by_id() -> dict:
    return {e["id"]: e for e in manifest()["entries"]}


@lru_cache(maxsize=16)
def level_palette(level: str) -> np.ndarray:
    return np.array(json.loads((ROOT / "extracted" / "levels" / level / "palette.json").read_text()), np.uint8)


def reference_image(entry: dict) -> Image.Image:
    """The source at 1x as RGBA (alpha 0 = transparent texel)."""
    fam = entry["family"]
    if fam == "level":
        src = entry["source"]
        idx = np.frombuffer((ROOT / src["file"]).read_bytes(), np.uint8).reshape(512, 2048)
        rgb = level_palette(src["level"])[idx]
        a = np.zeros(idx.shape, np.uint8)
        for x, y, w, h in src["uploads"]:
            a[y:y + h, x:x + w] = 255
        a[idx == 0] = 0
        return Image.fromarray(np.dstack([rgb, a]), "RGBA")
    if fam == "ui":                                         # raw 8-bit sheet, TRAINING palette (shared HUD range)
        idx = np.frombuffer((ROOT / entry["source"]["file"]).read_bytes()[:256 * 512], np.uint8).reshape(512, 256)
        return Image.fromarray(np.dstack([level_palette("TRAINING")[idx], np.where(idx == 0, 0, 255).astype(np.uint8)]), "RGBA")
    if fam == "screen":                                     # colour-mapped TGA: 256 x BGR24 palette, 8-bit pixels
        idx = screen_indices(entry)
        data = (ROOT / "extracted" / "raw" / (entry["id"].split("/")[1] + ".bin")).read_bytes()
        pal = np.frombuffer(data[18:786], np.uint8).reshape(256, 3)[:, ::-1]
        return Image.fromarray(np.dstack([pal[idx], np.full(idx.shape, 255, np.uint8)]), "RGBA")
    return Image.open(ROOT / entry["reference"]).convert("RGBA")


def screen_indices(entry: dict) -> np.ndarray:
    """The 320x256 8-bit pixels of a screen TGA (extracted/raw/<index>_<hash>.bin) in file row order,
    as contract.screen_entries searched the block rects (and as the game uploads them)."""
    data = (ROOT / "extracted" / "raw" / (entry["id"].split("/")[1] + ".bin")).read_bytes()
    return np.frombuffer(data[786:786 + 320 * 256], np.uint8).reshape(256, 320).copy()


def layout(pack: str) -> dict:
    """remastered/prompts/<pack>/layout.json (written by prompts.py, read by fit.py)."""
    return json.loads((PROMPTS / pack / "layout.json").read_text(encoding="utf-8"))


def download_path(pack: str) -> Path:
    """Where the model's result for a pack is saved: remastered/prompts/<pack>/<pack>.png."""
    return PROMPTS / pack / f"{pack}.png"


def is_pack(name: str) -> bool:
    return bool(name) and (PROMPTS / name / "layout.json").is_file()


def incoming_file(entry_id: str):
    """Newest remastered/incoming/<pack>/<id>.png (or the old flat incoming/<id>.png), else None."""
    name = entry_id.replace("/", "_") + ".png"
    found = [p for p in [INCOMING / name, *INCOMING.glob("*/" + name)] if p.is_file()]
    return max(found, key=lambda p: p.stat().st_mtime) if found else None


def output_path(entry: dict) -> Path:
    return ROOT / entry["output"]


def blocks_dir(image: Path) -> Path:
    """Optional per-block images next to an output image: <stem>.blocks/<key>_<w>x<h>.png. When a block's
    file is there, atlas.py packs it (and checks.py tests it) instead of cutting the rect from the image."""
    return image.with_suffix(".blocks")


def block_image(image: Path, img: Image.Image, rect, name: str, k: int) -> Image.Image:
    """The k x art of one block: its sidecar file if present, else the rect cut from img."""
    side = blocks_dir(image) / name
    if side.is_file():
        return Image.open(side)
    x, y, w, h = rect
    return img.crop((x * k, y * k, (x + w) * k, (y + h) * k))


def coverage(im: Image.Image, k: int) -> np.ndarray:
    """Per logical pixel coverage in [0, 1] of an output image at scale k."""
    if im.mode == "P":
        a = (np.array(im) != 0).astype(float)
    else:
        a = np.array(im.convert("RGBA"))[:, :, 3].astype(float) / 255
    h, w = a.shape
    return a.reshape(h // k, k, w // k, k).mean(axis=(1, 3))


@lru_cache(maxsize=1)
def archive_entries() -> list:
    """manifests/archive-manifest.json entries (the archive files contract.py reads)."""
    return json.loads((ROOT / "manifests" / "archive-manifest.json").read_text(encoding="utf-8"))["entries"]


@lru_cache(maxsize=4)
def sprite_bank(index: int) -> dict:
    """entry id -> [(rect, texels or None)] for one i1.4 bank, decoded as contract.py keys it."""
    import contract
    keyer = contract.Keyer()
    ents = contract.sprite_entries([e for e in archive_entries() if e["index"] == index], keyer, manifest()["scale"])
    tex = {id(b): t for items in keyer.groups.values() for t, b in items}
    return {e["id"]: [(b["rect"], tex.get(id(b))) for b in e["geometry"]["blocks"]] for e in ents}


def sprite_indices(entry: dict, rgba: np.ndarray) -> np.ndarray:
    """The frame canvas as palette indices: its parts pasted in draw order (index 0 = transparent, as
    the runtime draws it). rgba: the decoded reference, used only for parts without texels."""
    w, h = entry["geometry"]["size"]
    idx = np.zeros((h, w), np.uint8)
    for (x, y, bw, bh), tex in sprite_bank(entry["source"]["index"])[entry["id"]]:
        if tex is None:
            continue
        t = np.frombuffer(tex, np.uint8).reshape(bh, bw)
        dst = idx[y:y + bh, x:x + bw]
        m = t != 0
        dst[m] = t[m]
    # parts without a key (block outside its source) have no texels: take the index of the same colour
    gap = (idx == 0) & (rgba[..., 3] > 0)
    if gap.any():
        known = (idx != 0) & (rgba[..., 3] > 0)
        lut = {tuple(c): i for c, i in zip(rgba[known][:, :3].tolist(), idx[known].tolist())}
        for y, x in zip(*np.nonzero(gap)):
            idx[y, x] = lut.get(tuple(rgba[y, x, :3].tolist()), 0)
    return idx

