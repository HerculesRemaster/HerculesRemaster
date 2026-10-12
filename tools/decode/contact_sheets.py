"""Contact sheets per bank and an index page per level (S4.06).

Reads extracted/meta/full-game/animation-coverage.json (bank_index.py) and the decoded frames in
extracted/sprites/<bank>/f*.png (P4; palettes as decoded there). Writes
  extracted/full-game/sprites/<bank>/contact.png   every frame on a checker grid, labelled with its frame id
  extracted/full-game/sprites/index.html           per level (ENG1 level/start lists, VERIFIED-STATIC) and
                                                    "other banks" (LEAD), with source ids, frame counts and
                                                    links to the sheet and the anim viewer when one exists

    contact_sheets.py [--cell 96]
"""
from __future__ import annotations

import argparse
import html
import json
import os
import re
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2]
SPRITES = ROOT / "extracted" / "sprites"
OUT = ROOT / "extracted" / "full-game" / "sprites"


def sheet(bank_dir: Path, cell: int) -> tuple[Image.Image, int]:
    frames = sorted(bank_dir.glob("f*.png"), key=lambda p: int(re.sub(r"\D", "", p.stem) or 0))
    cols = 12
    rows = max(1, (len(frames) + cols - 1) // cols)
    img = Image.new("RGBA", (cols * cell, rows * (cell + 12)), (40, 42, 48, 255))
    dr = ImageDraw.Draw(img)
    for i, f in enumerate(frames):
        x, y = (i % cols) * cell, (i // cols) * (cell + 12)
        for cy in range(0, cell, 8):
            for cx in range(0, cell, 8):
                dr.rectangle([x + cx, y + cy, x + cx + 7, y + cy + 7],
                             fill=(58, 61, 68, 255) if (cx + cy) // 8 % 2 else (51, 54, 60, 255))
        fr = Image.open(f).convert("RGBA")
        fr.thumbnail((cell - 4, cell - 4), Image.NEAREST)
        img.alpha_composite(fr, (x + (cell - fr.width) // 2, y + (cell - fr.height) // 2))
        dr.text((x + 2, y + cell), f.stem[1:].lstrip("0") or "0", fill=(220, 220, 220, 255))
    return img, len(frames)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--cell", type=int, default=96)
    a = ap.parse_args(argv)
    cov = json.loads((ROOT / "extracted/meta/full-game/animation-coverage.json").read_text(encoding="utf-8"))
    rows = {}
    for b in cov["banks"]:
        if not b.get("extracted"):
            continue
        src = SPRITES / b["extracted"]
        img, n = sheet(src, a.cell)
        out = OUT / b["extracted"]
        out.mkdir(parents=True, exist_ok=True)
        img.save(out / "contact.png")
        viewer = src / "viewer.html"
        rows[(b["engine"], b["id"])] = (b, n, os.path.relpath(viewer, OUT).replace("\\", "/") if viewer.exists() else None)
    groups: dict[str, list] = {}
    for (eng, bid), (b, n, viewer) in sorted(rows.items()):
        for lv in b["levels"] or [f"{eng} other banks (LEAD: {b['level_evidence']})"]:
            groups.setdefault(lv if b["levels"] else lv, []).append((b, n, viewer))
    parts = ["<!doctype html><html><head><meta charset='utf-8'><title>Sprite banks</title><style>"
             "body{background:#1d1f24;color:#ddd;font:13px system-ui;margin:16px}a{color:#8cf}"
             "td,th{padding:3px 8px;border-bottom:1px solid #333;text-align:left}</style></head><body>"
             "<h1>Sprite banks by level</h1><p>From <code>tools/decode/bank_index.py</code> (ENG1 level and start "
             "lists, VERIFIED-STATIC) and <code>contact_sheets.py</code>.</p>"]
    for lv in sorted(groups, key=lambda s: (s.startswith("ENG"), s)):
        parts.append(f"<h2>{html.escape(lv)}</h2><table><tr><th>engine</th><th>id</th><th>path</th>"
                     "<th>archive</th><th>frames (table / decoded)</th><th>sheet</th><th>viewer</th></tr>")
        for b, n, viewer in groups[lv]:
            sheet_rel = f"{b['extracted']}/contact.png"
            parts.append(f"<tr><td>{b['engine']}</td><td>{b['id']}</td><td>{html.escape(b['path'])}</td>"
                         f"<td>{b['archive_index']}</td><td>{b['frames']} / {n}</td>"
                         f"<td><a href='{sheet_rel}'>contact.png</a></td>"
                         f"<td>{f'<a href={chr(39)}{viewer}{chr(39)}>viewer</a>' if viewer else '-'}</td></tr>")
        parts.append("</table>")
    parts.append("</body></html>")
    (OUT / "index.html").write_text("\n".join(parts), encoding="utf-8")
    print(f"{len(rows)} sheets, {len(groups)} groups -> {OUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
