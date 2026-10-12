"""Reference sheets for the pilot set (tools/hires/pilot.json) -> remastered/reference/.

refsheet.py [--trace analysis/traces/runs/artdump/resource-usage.jsonl]

Sprite sequences: <name>.png (frames at 3x on one baseline with frame number, display ticks, canvas
size and pivot) and <name>.gif at game speed (33 ms per tick, frames aligned on the pivot).
Display ticks per frame come from the runtime anim events (ANIM_CacheFrame hook: one event per
frame change per object; ticks = gap to that object's next change, median over the trace).
Regions: <name>.png, the source at 1x and 3x (tile: repeated 3 x 3 to show the seams).
"""
import argparse
import json
import statistics
from collections import defaultdict
from pathlib import Path

from PIL import Image, ImageDraw

import common

TICK_MS = 33


def runtime_ticks(trace: Path) -> dict:
    """(bank path tail, frame) -> median display ticks."""
    last, gaps = {}, defaultdict(list)
    for line in trace.open(encoding="utf-8"):
        if '"anim"' not in line:
            continue
        d = json.loads(line)
        key = d["obj"]
        if key in last:
            name, frame, tick = last[key]
            if d["tick"] > tick:
                gaps[(name, frame)].append(d["tick"] - tick)
        last[key] = (d["bank_name"].upper().replace("P:\\", ""), d["frame"], d["tick"])
    return {k: int(statistics.median(v)) for k, v in gaps.items() if v}


def bank_tail(name: str) -> str:
    n = name.upper().replace("M:\\GRAFIX\\CHOP\\", "").replace("M:\\LANGUAGE\\GRAFIX\\CHOP\\", "")
    return n.replace("\\ANIMPSX.BIN", "")


def sequence(seq, ents, ticks, out: Path):
    frames = []
    for f in seq["frames"]:
        e = ents[f"sprite/{seq['bank']:04d}/f{f:04d}"]
        t = ticks.get((bank_tail(e["source"]["name"]), f))
        frames.append((f, e, common.reference_image(e), t))
    left = max(e["geometry"]["origin"][0] for _, e, _, _ in frames)
    top = max(e["geometry"]["origin"][1] for _, e, _, _ in frames)
    right = max(e["geometry"]["size"][0] - e["geometry"]["origin"][0] for _, e, _, _ in frames)
    bottom = max(e["geometry"]["size"][1] - e["geometry"]["origin"][1] for _, e, _, _ in frames)
    cw, ch = left + right, top + bottom
    s = 3
    sheet = Image.new("RGBA", ((cw * s + 8) * len(frames), ch * s + 64), (48, 48, 60, 255))
    d = ImageDraw.Draw(sheet)
    gif = []
    for i, (f, e, im, t) in enumerate(frames):
        ox, oy = e["geometry"]["origin"]
        cell = Image.new("RGBA", (cw, ch), (48, 48, 60, 255))
        cell.alpha_composite(im, (left - ox, top - oy))
        x = i * (cw * s + 8)
        sheet.alpha_composite(cell.resize((cw * s, ch * s), Image.NEAREST), (x, 0))
        d.line([(x + left * s - 4, top * s), (x + left * s + 4, top * s)], fill=(255, 60, 60, 255))
        d.line([(x + left * s, top * s - 4), (x + left * s, top * s + 4)], fill=(255, 60, 60, 255))
        w, h = e["geometry"]["size"]
        d.text((x + 2, ch * s + 4), f"f{f}  {t if t else '?'} ticks", fill=(255, 255, 160, 255))
        d.text((x + 2, ch * s + 20), f"{w}x{h} pivot {ox},{oy}", fill=(200, 200, 200, 255))
        gif.append((cell.resize((cw * s, ch * s), Image.NEAREST).convert("RGB"), (t or 2) * TICK_MS))
    sheet.save(out / f"{seq['name']}.png")
    gif[0][0].save(out / f"{seq['name']}.gif", save_all=True, append_images=[g for g, _ in gif[1:]],
                   duration=[ms for _, ms in gif], loop=0 if seq.get("loop") else 1)
    return [(f, t) for f, _, _, t in frames]


def region(reg, ents, out: Path):
    ref = common.reference_image(ents[reg["entry"]])
    if "rect" in reg:
        x, y, w, h = reg["rect"]
        tile = ref.crop((x, y, x + w, y + h))
        rep = Image.new("RGBA", (w * 3, h * 3))
        for i in range(3):
            for j in range(3):
                rep.alpha_composite(tile, (i * w, j * h))
        ref = rep
    ref.resize((ref.width * 3, ref.height * 3), Image.NEAREST).save(out / f"{reg['name']}.png")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--trace", type=Path, default=common.ROOT / "analysis/traces/runs/artdump/resource-usage.jsonl")
    a = ap.parse_args(argv)
    pilot = json.loads((Path(__file__).with_name("pilot.json")).read_text(encoding="utf-8"))
    ents = common.entries_by_id()
    ticks = runtime_ticks(a.trace) if a.trace.exists() else {}
    out = common.REMASTERED / "reference"
    out.mkdir(parents=True, exist_ok=True)
    timing = {}
    for seq in pilot["sequences"]:
        timing[seq["name"]] = sequence(seq, ents, ticks, out)
    for reg in pilot["regions"]:
        region(reg, ents, out)
    (out / "timing.json").write_text(json.dumps(timing, indent=1), encoding="utf-8")
    for n, ft in timing.items():
        print(n, " ".join(f"{f}:{t or '?'}" for f, t in ft))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
