"""Build a static HTML animation viewer for every bank that has anim.json.

    build.py  -> extracted/sprites/<bank>/viewer.html  (open from disk; data embedded)
Plays the observed clips at the original tick length (33 ms x ticks shown), with the frame
pivot marked and the frame's raw box bytes listed. Frames that were never seen at runtime can be
stepped through as a strip.
"""
from __future__ import annotations

import html
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SPRITES = ROOT / "extracted" / "sprites"

PAGE = """<!doctype html><html><head><meta charset="utf-8"><title>%(title)s</title>
<style>
body{background:#1d1f24;color:#ddd;font:13px system-ui,sans-serif;margin:16px}
#stage{background:repeating-conic-gradient(#3a3d44 0 25%%,#33363c 0 50%%) 0 0/16px 16px;width:%(sw)dpx;height:%(sh)dpx;position:relative;image-rendering:pixelated;border:1px solid #555}
#stage img{position:absolute;transform-origin:0 0}
#pivot{position:absolute;width:9px;height:9px;margin:-4px 0 0 -4px;border:1px solid #f44;border-radius:50%%;pointer-events:none}
select,button,input{background:#2b2e35;color:#ddd;border:1px solid #555;padding:3px 6px}
#info{white-space:pre;font-family:ui-monospace,monospace;margin-top:8px}
.row{margin:8px 0}
</style></head><body>
<h3>%(title)s</h3>
<div class="row">clip <select id="clip"></select> <button id="play">play</button>
 scale <input id="scale" type="number" value="3" min="1" max="8" style="width:3em">
 frame <input id="fnum" type="number" min="1" style="width:5em"> <button id="show">show</button></div>
<div id="stage"><img id="img"><div id="pivot"></div></div>
<div id="info"></div>
<script>
const D=%(data)s;
const clipSel=document.getElementById('clip'),img=document.getElementById('img'),piv=document.getElementById('pivot'),info=document.getElementById('info');
D.clips.forEach((c,i)=>{const o=document.createElement('option');o.value=i;o.text=`#${i} ${c.run} ${c.obj} t${c.start_tick} (${c.frames.length} frames)`;clipSel.add(o)});
let timer=null;
function show(f,x,y,flip){const m=D.meta[f];const s=+document.getElementById('scale').value;
 if(!m||!m.png){info.textContent='frame '+f+': no image';img.src='';return}
 img.src=m.png;const ox=(x??%(cx)d)*s,oy=(y??%(cy)d)*s;
 img.style.left=(ox-m.origin[0]*s*(flip?-1:1))+'px';img.style.top=(oy-m.origin[1]*s)+'px';
 img.style.transform=`scale(${flip?-s:s},${s})`;piv.style.left=ox+'px';piv.style.top=oy+'px';
 const fr=D.frames[f]||{};info.textContent=`frame ${f}  size ${m.size}  pivot ${m.origin}  dx,dy ${m.dx},${m.dy}  palette ${m.palette_used??m.palette}\\nshown ${fr.ticks_median??'-'} ticks (median, ${fr.seen??0}x)  boxes ${JSON.stringify(m.boxes)}`}
function play(){clearInterval(timer);const c=D.clips[+clipSel.value];if(!c)return;let i=0,left=0;const x0=c.frames[0][2],y0=c.frames[0][3];
 timer=setInterval(()=>{if(left>0){left--;return}const f=c.frames[i%%c.frames.length];
 show(f[0],%(cx)d+(f[2]-x0)/1,%(cy)d+(f[3]-y0)/1,f[4]);left=(f[1]||2)-1;i++},33)}
document.getElementById('play').onclick=play;
document.getElementById('show').onclick=()=>{clearInterval(timer);show(+document.getElementById('fnum').value)};
if(D.clips.length)play();
</script></body></html>"""


def main() -> int:
    n = 0
    for anim in sorted(SPRITES.glob("*/anim.json")):
        a = json.loads(anim.read_text(encoding="utf-8"))
        bank = json.loads((anim.parent / "bank.json").read_text(encoding="utf-8"))
        meta = {f["frame"]: {k: f.get(k) for k in ("png", "origin", "size", "dx", "dy", "boxes", "palette", "palette_used")}
                for f in bank["frames"] if f.get("png")}
        labelled = [{"run": l["label"], "obj": "label", "start_tick": 0,
                     "frames": [[f, int(t or 2), 0, 0, 0, 255] for f, t in zip(l["frames"], l["ticks"])]}
                    for l in a.get("labels", [])]
        data = {"clips": labelled + a["clips"], "frames": a["frames"], "meta": meta}
        page = PAGE % {"title": html.escape(a["bank"]), "data": json.dumps(data), "sw": 960, "sh": 600,
                       "cx": 160, "cy": 170}
        (anim.parent / "viewer.html").write_text(page, encoding="utf-8")
        n += 1
    print(f"viewers={n}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
