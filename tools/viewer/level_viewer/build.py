"""Static level viewer: layer images + collision/spawn overlay with toggles.

    build.py LEVEL  -> extracted/levels/<LEVEL>/viewer.html (uses the preview PNGs made by
    tools/level/render_preview.py --collision; open from disk)
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main(level: str) -> int:
    d = ROOT / "extracted" / "levels" / level
    layers = sorted(int(p.stem.replace("preview_layer", "")) for p in d.glob("preview_layer*.png"))
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    sec = L["sections"][0]
    imgs = "".join(f'<img id="l{l}" src="preview_layer{l}.png" style="z-index:{10 - l}">' for l in layers)
    boxes = "".join(f'<label><input type="checkbox" checked onchange="tog(\'l{l}\',this)"> layer {l}</label> ' for l in layers)
    page = f"""<!doctype html><html><head><meta charset="utf-8"><title>{level}</title>
<style>body{{background:#111;color:#ddd;font:13px system-ui;margin:12px}}#v{{position:relative;overflow:auto;border:1px solid #444;height:80vh}}
#v img{{position:absolute;left:0;top:0;image-rendering:pixelated}}label{{margin-right:10px}}</style></head><body>
<b>{level}</b> - {sec['columns']} columns x {sec['rows']} rows x {sec['layers']} layers, {len(sec.get('spawns', []))} spawns,
{len(L['types'])} block types (rendered columns as in the preview PNGs).<br>
{boxes}<label><input type="checkbox" onchange="tog('ov',this)"> collision + spawns</label>
<div id="v"><img src="preview_composite.png" style="z-index:0;opacity:0">{imgs}<img id="ov" src="preview_collision.png" style="z-index:50;display:none"></div>
<script>function tog(id,cb){{document.getElementById(id).style.display=cb.checked?'block':'none'}}</script></body></html>"""
    (d / "viewer.html").write_text(page, encoding="utf-8")
    print(f"viewer -> {d / 'viewer.html'}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
