"""i-family containers. i1.4 = sprite animation banks (ANIMPSX.BIN) -> RGBA PNG per frame + JSON.

Rules from ENG1.DLL (docs/formats/i-family.md):
  file  = "i1.4" + body;  P = file + 4 ("puVar2" in ENG1 0x3cd0)
  P+0   u16 count, P+2 u16 palettes
  P+4k  u32 entry for frame k (k = 1 .. count-1; entry 0 is the count word): offset<<8 | flags
        offset is relative to P; flags & 0x1f = palette, 0x40 = keep in VRAM cache
  P+4*count+4, +8   two u32 (copied to a global by 0x3cd0)
  P+4*count+12      u16 palette offsets[palettes] (relative to P)
  palette           u16 n, u16 (n again), then n PSX colours (BGR555, bit 15 = STP) from +4.
                    The game takes the palette from the object (obj+0x1c) unless that is 0xff.
                    Frames are rendered with their entry palette (flags & 0x1f), or with an
                    object palette listed in tools/decode/sprite_palettes.json. A palette with
                    no opaque colour falls back to the first usable one ("fallback").
                    The CLUT is 256 entries
                    (ENG1 0x5780): n == 256 -> as is; else entry 0 = 0 (transparent), entries
                    1..n-1 = colour | 0x8000, then repeated to fill 256 (index i -> i mod n).
  frame record      u8 parts, s8 boxes, boxes*6 bytes, 4 bytes clip box,
                    H: s8 dx, s8 dy, u8 flags[0], then per part i: u8 size (w=(s&15)*8, h=(s>>4)*8),
                    s8 x (*2), s8 y (*2), u8 flags[i+1]; then the pixel stream (ENG1 0x4ca0):
  part flags        bit7 = no new pixels: texture is the previous block at
                    ((f&15)*8, (f>>1&0x38)); else block = ((f&3)*8+8) x ((f&0xc)*2+8) bytes,
                    byte RLE: signed byte <= 0 literal pixel; 2 = end of block; else run
                    length followed by the value.
  draw (ENG1 0x4010) part at (dx + 2x, dy + 2y), size w x h from its texture block.
PSX colour 0x0000 is transparent.

    ifam.py [--only INDEX ...] [--out extracted/sprites]
Prints totals per magic; writes extracted/meta/sprites-coverage.json.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from collections import Counter
from pathlib import Path

from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from palette import sprite_clut as clut  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "manifests" / "archive-manifest.json"


class UnsupportedVariant(Exception):
    pass


def rle_block(data: bytes, pos: int, w: int, h: int) -> tuple[bytes, int]:
    out = bytearray()
    limit = 1024
    while True:
        b = data[pos]
        pos += 1
        if b == 0 or b >= 0x80:          # signed <= 0: literal pixel
            out.append(b)
            continue
        if b == 2:
            break
        out += bytes((data[pos],)) * b
        pos += 1
        if len(out) > limit:
            raise ValueError("RLE overflow")
    need = w * h
    if len(out) < need:
        out += bytes(need - len(out))
    return bytes(out[:need]), pos


def decode_frame(P: bytes, off: int, pal):
    parts, boxes = P[off], struct.unpack_from("<b", P, off + 1)[0]
    q = off + 2
    box_list = [list(P[q + 6 * i:q + 6 * i + 6]) for i in range(max(boxes, 0))]
    q += 6 * boxes
    clip = list(struct.unpack_from("<4b", P, q))
    H = q + 4
    dx, dy = struct.unpack_from("<bb", P, H)
    flags = [P[H + 2]]
    recs = []
    for i in range(parts):
        s, x, y, fn = struct.unpack_from("<BbbB", P, H + 3 + 4 * i)
        recs.append((s, x, y))
        flags.append(fn)
    pos = H + 2 + 4 * parts
    blocks = []
    prev = None
    for i in range(parts):
        f = flags[i]
        if f & 0x80:
            if prev is None:
                raise UnsupportedVariant("sub-part without a previous block")
            pb, pw, ph = prev
            sx, sy = (f & 15) * 8, (f >> 1) & 0x38
            blocks.append((pb, pw, ph, sx, sy))
        else:
            bw, bh = (f & 3) * 8 + 8, (f & 0xC) * 2 + 8
            px, pos = rle_block(P, pos, bw, bh)
            prev = (px, bw, bh)
            blocks.append((px, bw, bh, 0, 0))
    # layout
    quads = []
    for (s, x, y), (px, bw, bh, sx, sy) in zip(recs, blocks):
        w, h = (s & 15) * 8, (s >> 4) * 8
        quads.append((dx + 2 * x, dy + 2 * y, w, h, px, bw, bh, sx, sy))
    if not quads:
        return None, {"parts": 0}
    x0 = min(q[0] for q in quads)
    y0 = min(q[1] for q in quads)
    x1 = max(q[0] + q[2] for q in quads)
    y1 = max(q[1] + q[3] for q in quads)
    im = Image.new("RGBA", (max(x1 - x0, 1), max(y1 - y0, 1)), (0, 0, 0, 0))
    pix = im.load()
    for qx, qy, w, h, px, bw, bh, sx, sy in quads:
        for yy in range(h):
            ty = sy + yy
            if ty >= bh:
                break
            for xx in range(w):
                tx = sx + xx
                if tx >= bw:
                    break
                rgba = pal[px[ty * bw + tx]]
                if rgba[3]:
                    pix[qx - x0 + xx, qy - y0 + yy] = rgba
    meta = {"parts": parts, "boxes": box_list, "clip": clip, "dx": dx, "dy": dy,
            "origin": [-x0, -y0], "size": [x1 - x0, y1 - y0], "stream_end": pos - off}
    return im, meta


PALETTE_MAP = json.loads((Path(__file__).with_name("sprite_palettes.json")).read_text(encoding="utf-8"))


def palette_strip(pals, path: Path) -> None:
    im = Image.new("RGBA", (256 * 2, len(pals) * 8), (0, 0, 0, 0))
    px = im.load()
    for r, pl in enumerate(pals):
        for i, c in enumerate(pl):
            for dy in range(7):
                px[i * 2, r * 8 + dy] = c
                px[i * 2 + 1, r * 8 + dy] = c
    im.save(path)


def decode_bank(data: bytes, out_dir: Path, name: str, index: int = -1) -> dict:
    if data[:4] != b"i1.4":
        raise UnsupportedVariant(data[:4].decode("latin-1"))
    P = data[4:]
    count, npal = struct.unpack_from("<HH", P, 0)
    extra = struct.unpack_from("<II", P, 4 * count + 4)
    pal_offs = struct.unpack_from(f"<{npal}H", P, 4 * count + 12)
    pals = [clut(P, o) for o in pal_offs]
    empty = [all(c[3] == 0 for c in pl) for pl in pals]
    entry = PALETTE_MAP.get(str(index)) if str(index) in PALETTE_MAP else None
    obj_pal = entry["palette"] if entry else None
    out_dir.mkdir(parents=True, exist_ok=True)
    palette_strip(pals, out_dir / "palettes.png")
    frames, errors = [], 0
    for k in range(1, count):
        e = struct.unpack_from("<I", P, 4 * k)[0]
        off, fl = e >> 8, e & 0xFF
        rec = {"frame": k, "offset": off, "flags": fl, "palette": fl & 0x1F}
        if off == 0:
            rec["empty"] = True
            frames.append(rec)
            continue
        try:
            pi = fl & 0x1F
            if obj_pal is not None:
                pi = obj_pal
                rec["palette_source"] = "object"
            elif pi >= len(pals) or empty[pi]:
                pi = next((i for i in range(len(pals)) if not empty[i]), 0)
                rec["palette_source"] = "fallback"
            rec["palette_used"] = pi
            pal = pals[pi]
            im, meta = decode_frame(P, off, pal)
            rec.update(meta)
            if im is not None:
                im.save(out_dir / f"f{k:04d}.png")
                rec["png"] = f"f{k:04d}.png"
        except (UnsupportedVariant, ValueError, IndexError, struct.error) as ex:
            rec["error"] = str(ex)
            errors += 1
        frames.append(rec)
    bank = {"name": name, "count": count, "palettes": npal, "palette_offsets": list(pal_offs),
            "empty_palettes": [i for i, e in enumerate(empty) if e],
            "object_palette": entry,
            "header_u32": list(extra), "frames": frames, "errors": errors}
    (out_dir / "bank.json").write_text(json.dumps(bank), encoding="utf-8")
    return bank


def bank_dir(name: str | None, index: int) -> str:
    if not name:
        return f"{index:04d}"
    parts = name.replace("\\", "/").split("/")
    keep = [p for p in parts[3:-1]] if len(parts) > 4 else parts[:-1]
    return f"{index:04d}_" + "_".join(keep)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=ROOT / "extracted" / "sprites")
    ap.add_argument("--only", type=int, nargs="*")
    a = ap.parse_args(argv)
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    ifam = [e for e in entries if e["magic"] and bytes.fromhex(e["magic"])[:2] in (b"i1", b"i2")]
    if a.only:
        ifam = [e for e in ifam if e["index"] in a.only]
    stats, cov = Counter(), []
    for e in ifam:
        magic = bytes.fromhex(e["magic"]).decode("latin-1")
        try:
            b = decode_bank((ROOT / e["file"]).read_bytes(), a.out / bank_dir(e["name_candidate"], e["index"]),
                            e["name_candidate"] or "", e["index"])
            n_ok = sum(1 for f in b["frames"] if "png" in f)
            stats[f"{magic}_ok"] += 1
            stats["frames"] += n_ok
            stats["frame_errors"] += b["errors"]
            cov.append({"index": e["index"], "magic": magic, "status": "ok", "frames": n_ok, "errors": b["errors"]})
        except UnsupportedVariant as ex:
            stats[f"{magic}_unsupported"] += 1
            cov.append({"index": e["index"], "magic": magic, "status": "unsupported", "reason": str(ex)})
        except Exception as ex:  # noqa: BLE001
            stats[f"{magic}_error"] += 1
            cov.append({"index": e["index"], "magic": magic, "status": "error", "error": repr(ex)})
    meta = ROOT / "extracted" / "meta"
    meta.mkdir(parents=True, exist_ok=True)
    (meta / "sprites-coverage.json").write_text(json.dumps({"family": "i-family", "counts": dict(stats),
                                                            "entries": cov}, indent=1), encoding="utf-8")
    print(" ".join(f"{k}={v}" for k, v in sorted(stats.items())))
    for c in cov:
        if c["status"] == "error" or c.get("errors"):
            print("  ", c)
    return 0


if __name__ == "__main__":
    sys.exit(main())
