"""Colour-mapped TGA screens -> indexed PNG + palette JSON (rules from the game, docs/formats/tga.md).

All 65 archive TGAs: type 1, 256-entry 24-bit (BGR) colour map, 320x256, 8 bpp, descriptor 0.
The game (EXE 0xa5xx upload via slot 0x204, traced in P3) copies the pixel bytes from offset
18 + 768 straight into VRAM rows y, y+1, ... so row 0 of the file is the TOP row, whatever the
descriptor says (a spec-following viewer shows these files upside down). A file shorter than
the header promises is padded with index 0 rows at the bottom and flagged.

    tga.py [--out extracted/images/tga]
Writes <index>_<name>.png (mode P, palette from the file) and <index>_<name>.json; coverage in
extracted/meta/images-tga-coverage.json. Prints totals.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from collections import Counter
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "manifests" / "archive-manifest.json"


def decode(data: bytes) -> tuple[Image.Image, dict]:
    idl, cmtype, itype, cmo, cml, cmd, xo, yo, w, h, bpp, desc = struct.unpack_from("<BBBHHBHHHHBB", data, 0)
    if itype != 1 or cmtype != 1 or bpp != 8 or cmd != 24:
        raise ValueError(f"unsupported TGA variant type={itype} cm={cmtype} bpp={bpp} cmd={cmd}")
    pal_off = 18 + idl
    pal = data[pal_off:pal_off + cml * 3]
    px_off = pal_off + cml * 3
    px = data[px_off:px_off + w * h]
    short = w * h - len(px)
    if short:
        px += bytes(short)
    im = Image.frombytes("P", (w, h), px)  # file row 0 = top row (game rule)
    rgb = bytearray()
    for i in range(cml):
        b, g, r = pal[i * 3:i * 3 + 3]
        rgb += bytes((r, g, b))
    im.putpalette(bytes(rgb) + bytes(768 - len(rgb)))
    meta = {"width": w, "height": h, "bpp": 8, "palette_entries": cml, "colour_map_origin": cmo,
            "descriptor": desc, "orientation": "file row 0 = top (game uploads rows in file order)",
            "pixel_offset": px_off, "missing_bytes": short, "missing_rows": short // w,
            "palette_rgb": [list(rgb[i * 3:i * 3 + 3]) for i in range(cml)]}
    return im, meta


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=ROOT / "extracted" / "images" / "tga")
    a = ap.parse_args(argv)
    a.out.mkdir(parents=True, exist_ok=True)
    entries = [e for e in json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"] if e["magic"] == "00010100"]
    cov, stats = [], Counter()
    for e in entries:
        name = (e["name_candidate"] or f"tga_{e['index']}").replace("\\", "/")
        stem = f"{e['index']:04d}_{name.rsplit('/', 1)[-1].rsplit('.', 1)[0]}"
        try:
            im, meta = decode((ROOT / e["file"]).read_bytes())
            im.save(a.out / f"{stem}.png")
            meta.update({"index": e["index"], "name": e["name_candidate"], "png": f"{stem}.png"})
            (a.out / f"{stem}.json").write_text(json.dumps(meta), encoding="utf-8")
            status = "ok" if not meta["missing_bytes"] else "ok_truncated"
            cov.append({"index": e["index"], "status": status, "png": f"{stem}.png"})
            stats[status] += 1
        except Exception as ex:  # noqa: BLE001
            cov.append({"index": e["index"], "status": "error", "error": str(ex)})
            stats["error"] += 1
    meta_dir = ROOT / "extracted" / "meta"
    meta_dir.mkdir(parents=True, exist_ok=True)
    (meta_dir / "images-tga-coverage.json").write_text(json.dumps(
        {"family": "tga", "total": len(entries), "counts": dict(stats), "entries": cov}, indent=1), encoding="utf-8")
    print(f"tga={len(entries)} " + " ".join(f"{k}={v}" for k, v in sorted(stats.items())) + f" -> {a.out}")
    for c in cov:
        if c["status"] != "ok":
            print("  ", c)
    return 0


if __name__ == "__main__":
    sys.exit(main())
