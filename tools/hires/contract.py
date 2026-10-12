"""Build the replacement manifest: source images, their runtime keys and slice rects.

contract.py [--scale 9] [--out remastered/manifest.json]

Families (docs/hires-pipeline.md):
  sprite  one entry per i1.4 bank frame. Canvas and parts as ENG1 ANIM_BuildSpriteQuads (0x4010)
          draws them; a part's key = FNV-1a 64 (w, h, w x h texels of its RLE block).
  level   one entry per level: the VRAM image (vram.bin, 2048x512 8-bit). Each mesh quad gives
          the keys the renderer can see for it: UV bounding box (general polygon), exact rect
          (1:1 quad drawn as a sprite) and the mirrored-rect variant.
  screen  one entry per TGA (320x256). Blocks come from the runtime catalog (extracted/art_blocks)
          by exact content search, since screen draw rects are chosen by game code.
Catalog keys not covered by any entry are listed in the summary.
"""
import argparse
import hashlib
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "decode"))
import ifam  # noqa: E402

FNV0, FNVP = np.uint64(1469598103934665603), np.uint64(1099511628211)
MANIFEST_IN = ROOT / "manifests" / "archive-manifest.json"


def fnv_batch(w: int, h: int, blocks: np.ndarray) -> list[int]:
    """Keys for N blocks of w x h texels (N, w*h uint8), as gpu_renderer.cpp BlockKey."""
    k = np.full(len(blocks), FNV0, np.uint64)
    with np.errstate(over="ignore"):
        for b in (w & 255, w >> 8, h & 255, h >> 8):
            k = (k ^ np.uint64(b)) * FNVP
        for j in range(blocks.shape[1]):
            k = (k ^ blocks[:, j].astype(np.uint64)) * FNVP
    return [int(x) for x in k]


class Keyer:
    """Collects (w, h, texels, owner) and hashes them per size."""

    def __init__(self):
        self.groups = defaultdict(list)

    def add(self, w, h, texels: bytes, owner):
        if w > 0 and h > 0:
            self.groups[(w, h)].append((texels, owner))

    def run(self):
        for (w, h), items in self.groups.items():
            arr = np.frombuffer(b"".join(t for t, _ in items), np.uint8).reshape(len(items), w * h)
            for key, (_, owner) in zip(fnv_batch(w, h, arr), items):
                yield key, owner


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


# ---------------- sprites ----------------
def sprite_entries(entries, keyer, scale):
    out = []
    for e in entries:
        if not e["magic"] or bytes.fromhex(e["magic"])[:4] != b"i1.4":
            continue
        data = (ROOT / e["file"]).read_bytes()
        P = data[4:]
        count = struct.unpack_from("<H", P, 0)[0]
        bdir = ifam.bank_dir(e["name_candidate"], e["index"])
        for k in range(1, count):
            off = struct.unpack_from("<I", P, 4 * k)[0] >> 8
            if not off:
                continue
            try:
                parts, boxes = P[off], struct.unpack_from("<b", P, off + 1)[0]
                H = off + 2 + 6 * max(boxes, 0) + 4
                dx, dy = struct.unpack_from("<bb", P, H)
                flags, recs = [P[H + 2]], []
                for i in range(parts):
                    s, x, y, fn = struct.unpack_from("<BbbB", P, H + 3 + 4 * i)
                    recs.append((s, x, y))
                    flags.append(fn)
                pos, prev, quads = H + 2 + 4 * parts, None, []
                for i in range(parts):
                    f = flags[i]
                    if f & 0x80:
                        pb, pw, ph = prev
                        blk = (pb, pw, ph, (f & 15) * 8, (f >> 1) & 0x38)
                    else:
                        bw, bh = (f & 3) * 8 + 8, (f & 0xC) * 2 + 8
                        px, pos = ifam.rle_block(P, pos, bw, bh)
                        prev = (px, bw, bh)
                        blk = (px, bw, bh, 0, 0)
                    s, x, y = recs[i]
                    quads.append((dx + 2 * x, dy + 2 * y, (s & 15) * 8, (s >> 4) * 8, blk))
            except (IndexError, struct.error, ValueError, TypeError):
                continue
            if not quads:
                continue
            x0, y0 = min(q[0] for q in quads), min(q[1] for q in quads)
            x1, y1 = max(q[0] + q[2] for q in quads), max(q[1] + q[3] for q in quads)
            ent = {"id": f"sprite/{e['index']:04d}/f{k:04d}", "family": "sprite",
                   "source": {"kind": "i1.4", "index": e["index"], "name": e["name_candidate"], "frame": k,
                              "sha256": sha(data)},
                   "geometry": {"size": [x1 - x0, y1 - y0], "origin": [-x0, -y0], "blocks": []},
                   "timing": {"note": "frame shown by ENG1 animation scripts; never changed"},
                   "reference": f"extracted/sprites/{bdir}/f{k:04d}.png",
                   "output": f"remastered/art/sprite/{e['index']:04d}/f{k:04d}.png"}
            for qx, qy, w, h, (pb, pw, ph, sx, sy) in quads:
                b = {"rect": [qx - x0, qy - y0, w, h], "key": None}
                ent["geometry"]["blocks"].append(b)
                if sx + w <= pw and sy + h <= ph:
                    tex = bytes(pb[(sy + yy) * pw + sx + xx] for yy in range(h) for xx in range(w))
                    keyer.add(w, h, tex, b)
            out.append(ent)
    return out


# ---------------- levels ----------------
def level_entries(keyer):
    out = []
    for ld in sorted((ROOT / "extracted" / "levels").iterdir()):
        vram = (ld / "vram.bin").read_bytes()
        img = np.frombuffer(vram, np.uint8).reshape(512, 2048)
        lv = json.loads((ld / "level.json").read_text(encoding="utf-8"))
        tex = lv["textures"]
        rects = set()
        for t in lv["types"].values():
            mesh = t.get("mesh") or {}
            for q in mesh.get("quads", []):
                if q["flags"] & 0x80 or str(q["tex"]) not in tex:      # flat colour quads have no texture
                    continue
                tw = tex[str(q["tex"])]
                tp = tw["tpage"] & 0x1F
                if (tw["tpage"] & 0x180) == 0x180:                    # second VRAM MB: not in vram.bin
                    continue
                us, vs = [uv[0] for uv in tw["uv"]], [uv[1] for uv in tw["uv"]]
                bx, by = (tp >> 1 & 7) * 256, ((tp >> 4) & 1) * 256
                u0, u1, v0, v1 = min(us), max(us), min(vs), max(vs)
                rects.add((bx + u0, by + v0, u1 - u0 + 1, v1 - v0 + 1))        # UV bounding box
                if u1 > u0 and v1 > v0:
                    rects.add((bx + u0, by + v0, u1 - u0, v1 - v0))            # exact 1:1 rect
                    rects.add((bx + u0 + 1, by + v0, u1 - u0, v1 - v0))        # mirrored rect
        ent = {"id": f"level/{ld.name}", "family": "level",
               "source": {"kind": "vram", "level": ld.name, "file": f"extracted/levels/{ld.name}/vram.bin",
                          "uploads": [[0, 0, 1536, 512], [1536, 256, 512, 256]], "sha256": sha(vram)},
               "geometry": {"size": [2048, 512], "origin": [0, 0], "blocks": []},
               "timing": {"note": "animated (T38) and scrolling (T3c) VRAM rects change every frame and keep the original"},
               "reference": f"extracted/levels/{ld.name}/vram.png",
               "output": f"remastered/art/level/{ld.name}.png"}
        for x, y, w, h in sorted(rects):
            if x + w > 2048 or y + h > 512 or (x // 256) != ((x + w - 1) // 256):
                continue                                                  # wraps the page: skip
            b = {"rect": [x, y, w, h], "key": None}
            ent["geometry"]["blocks"].append(b)
            keyer.add(w, h, img[y:y + h, x:x + w].tobytes(), b)
        out.append(ent)
    return out


def find_blocks(img: np.ndarray, pending: dict) -> list:
    """Exact content search of catalog blocks (key -> (w, h, texels)) in an 8-bit image."""
    H, W = img.shape
    rows = [img[y].tobytes() for y in range(H)]
    blocks = []
    for key, (w, h, tex) in list(pending.items()):
        if w > W or h > H:
            continue
        first, hit = tex[:w], None
        for y in range(H - h + 1):
            x = rows[y].find(first)
            while x != -1 and hit is None:
                if x + w <= W and img[y:y + h, x:x + w].tobytes() == tex:
                    hit = [x, y, w, h]
                x = rows[y].find(first, x + 1)
            if hit:
                break
        if hit:
            blocks.append({"rect": hit, "key": f"{key:016x}"})
            pending.pop(key)
    return blocks


def add_level_search(ents, pending):
    """Blocks the runtime drew from a level's VRAM image outside the mesh quads (sky, background)."""
    for e in ents:
        if e["family"] == "level":
            img = np.frombuffer((ROOT / e["source"]["file"]).read_bytes(), np.uint8).reshape(512, 2048)
            e["geometry"]["blocks"] += find_blocks(img, pending)


# ---------------- UI panels ----------------
def gauge_variants(img: np.ndarray, blocks: list) -> list:
    """Gauges draw growing slices of one strip: rects sharing (x, y, h) with several widths are
    left-anchored bars, rects sharing (x, w, y + h) with several heights are bottom-anchored. Add
    every width / height so values not seen at runtime are covered too."""
    have = {tuple(b["rect"]) for b in blocks}
    bars, cols = defaultdict(set), defaultdict(set)
    for x, y, w, h in have:
        bars[(x, y, h)].add(w)
        cols[(x, w, y + h)].add(h)
    new = set()
    for (x, y, h), ws in bars.items():
        if len(ws) > 2:
            new |= {(x, y, w, h) for w in range(1, max(ws) + 1)}
    for (x, w, bot), hs in cols.items():
        if len(hs) > 2:
            new |= {(x, bot - h, w, h) for h in range(1, max(hs) + 1)}
    out = []
    for x, y, w, h in sorted(new - have):
        key = fnv_batch(w, h, img[y:y + h, x:x + w].reshape(1, -1))[0]
        out.append({"rect": [x, y, w, h], "key": f"{key:016x}"})
    return out


# ---------------- UI panels ----------------
def ui_entries(entries, pending):
    """STPANEL*.DAT: raw 256x512 8-bit sheets (HUD) uploaded into VRAM page 0; blocks by content search."""
    out = []
    for e in entries:
        name = e.get("name_candidate") or ""
        if "STPANEL" not in name.upper():
            continue
        data = (ROOT / e["file"]).read_bytes()
        img = np.frombuffer(data[:256 * 512], np.uint8).reshape(512, 256)
        blocks = find_blocks(img, pending)
        blocks += gauge_variants(img, blocks)
        stem = Path(name.replace("\\", "/")).stem
        out.append({"id": f"ui/{stem}", "family": "ui",
                    "source": {"kind": "raw8", "index": e["index"], "name": name, "file": e["file"], "size": [256, 512],
                               "sha256": sha(data)},
                    "geometry": {"size": [256, 512], "origin": [0, 0], "blocks": blocks},
                    "timing": {"note": "HUD; health bar drawn as prefixes of one strip"},
                    "reference": f"extracted/images/ui/{stem}.png",
                    "output": f"remastered/art/ui/{stem}.png"})
    return out


# ---------------- screens (TGA) ----------------
def screen_entries(entries, pending):
    out = []
    tgas = [e for e in entries if e["magic"] == "00010100"]
    for e in tgas:
        data = (ROOT / e["file"]).read_bytes()
        px = data[786:786 + 320 * 256]
        if len(px) < 320 * 256:
            continue
        blocks = find_blocks(np.frombuffer(px, np.uint8).reshape(256, 320), pending)
        if blocks:
            name = Path(e["file"]).stem
            out.append({"id": f"screen/{name}", "family": "screen",
                        "source": {"kind": "tga", "index": e["index"], "name": e.get("name_candidate"),
                                   "sha256": sha(data)},
                        "geometry": {"size": [320, 256], "origin": [0, 0], "blocks": blocks},
                        "timing": {"note": "static screen"},
                        "reference": f"extracted/images/tga/{name}.png",
                        "output": f"remastered/art/screen/{name}.png"})
    return out


def load_catalog():
    cat = {}
    d = ROOT / "extracted" / "art_blocks"
    for f in d.glob("*.png") if d.exists() else []:
        m = re.match(r"([0-9a-f]{16})_(\d+)x(\d+)\.png", f.name)
        if m:
            w, h = int(m[2]), int(m[3])
            cat[int(m[1], 16)] = (w, h, Image.open(f).tobytes())
    return cat


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--scale", type=int, default=9)
    ap.add_argument("--out", type=Path, default=ROOT / "remastered" / "manifest.json")
    a = ap.parse_args(argv)
    entries = json.loads(MANIFEST_IN.read_text(encoding="utf-8"))["entries"]
    keyer = Keyer()
    ents = sprite_entries(entries, keyer, a.scale) + level_entries(keyer)
    for key, block in keyer.run():
        block["key"] = f"{key:016x}"
    catalog = load_catalog()
    covered = {int(b["key"], 16) for e in ents for b in e["geometry"]["blocks"] if b["key"]}
    pending = {k: v for k, v in catalog.items() if k not in covered}
    add_level_search(ents, pending)
    ents += ui_entries(entries, pending)
    ents += screen_entries(entries, pending)
    covered = set(catalog) - set(pending)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps({"version": 1, "scale": a.scale, "entries": ents}, indent=0), encoding="utf-8")
    fam = Counter(e["family"] for e in ents)
    nblk = Counter()
    for e in ents:
        nblk[e["family"]] += sum(1 for b in e["geometry"]["blocks"] if b["key"])
    miss = [v[:2] for v in pending.values()]
    print(f"entries {dict(fam)} keyed blocks {dict(nblk)}")
    print(f"catalog {len(catalog)} keys, covered {len(catalog) - len(miss)}, uncovered {len(miss)}: "
          f"{Counter(miss).most_common(8)}")
    print(f"-> {a.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
