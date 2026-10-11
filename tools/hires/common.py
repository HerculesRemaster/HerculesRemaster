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
    im = Image.open(ROOT / entry["reference"]).convert("RGBA")
    if fam == "screen":
        im = im.crop((0, 0, *entry["geometry"]["size"]))
    return im


def layout(pack: str) -> dict:
    """remastered/prompts/<pack>/layout.json (written by prompts.py, read by fit.py)."""
    return json.loads((PROMPTS / pack / "layout.json").read_text(encoding="utf-8"))


def is_pack(name: str) -> bool:
    return bool(name) and (PROMPTS / name / "layout.json").is_file()


def incoming_file(entry_id: str):
    """Newest remastered/incoming/<pack>/<id>.png (or the old flat incoming/<id>.png), else None."""
    name = entry_id.replace("/", "_") + ".png"
    found = [p for p in [INCOMING / name, *INCOMING.glob("*/" + name)] if p.is_file()]
    return max(found, key=lambda p: p.stat().st_mtime) if found else None


def output_path(entry: dict) -> Path:
    return ROOT / entry["output"]


def coverage(im: Image.Image, k: int) -> np.ndarray:
    """Per logical pixel coverage in [0, 1] of an output image at scale k."""
    if im.mode == "P":
        a = (np.array(im) != 0).astype(float)
    else:
        a = np.array(im.convert("RGBA"))[:, :, 3].astype(float) / 255
    h, w = a.shape
    return a.reshape(h // k, k, w // k, k).mean(axis=(1, 3))
