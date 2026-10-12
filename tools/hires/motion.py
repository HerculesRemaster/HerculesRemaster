"""Side-by-side motion review of the pilot sequences: original (nearest, 9x) | new art from remastered/incoming/.

motion.py [--scale 0.5] [sequence ...]   -> remastered/review/motion/<sequence>.gif

Frames are aligned on the manifest pivot (geometry.origin) and shown for the runtime display ticks of
remastered/reference/timing.json (refsheet.py; 33 ms per tick), so the GIF plays at game speed. A frame
without new art shows the original on both sides (labelled "orig").
"""
import argparse
import json

import numpy as np
from PIL import Image, ImageDraw

import common

TICK_MS = 33
BG = (38, 38, 46, 255)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("sequences", nargs="*")
    ap.add_argument("--scale", type=float, default=0.5, help="display scale of the 9x art (default 0.5)")
    a = ap.parse_args(argv)
    k = common.manifest()["scale"]
    ents = common.entries_by_id()
    pilot = json.loads((common.ROOT / "tools" / "hires" / "pilot.json").read_text(encoding="utf-8"))
    timing = json.loads((common.REMASTERED / "reference" / "timing.json").read_text(encoding="utf-8"))
    out_dir = common.REMASTERED / "review" / "motion"
    out_dir.mkdir(parents=True, exist_ok=True)
    for seq in pilot["sequences"]:
        if a.sequences and seq["name"] not in a.sequences:
            continue
        ticks = dict((f, t) for f, t in timing.get(seq["name"], []))
        frames = []
        for f in seq["frames"]:
            e = ents[f"sprite/{seq['bank']:04d}/f{f:04d}"]
            ref = common.reference_image(e)
            orig = ref.resize((ref.width * k, ref.height * k), Image.NEAREST)
            p = common.incoming_file(e["id"])
            new = Image.open(p).convert("RGBA") if p else orig
            frames.append((f, e["geometry"]["origin"], orig, new, p is not None, ticks.get(f, 4)))
        # common pivot box over all frames
        left = max(o[0] for _, o, *_ in frames) * k
        top = max(o[1] for _, o, *_ in frames) * k
        right = max(im.width - o[0] * k for _, o, im, *_ in frames)
        bottom = max(im.height - o[1] * k for _, o, im, *_ in frames)
        w, h = left + right, top + bottom
        gif = []
        for f, (ox, oy), orig, new, has, t in frames:
            canvas = Image.new("RGBA", (w * 2 + 8, h + 40), BG)
            for i, im in enumerate((orig, new)):
                canvas.alpha_composite(im, (i * (w + 8) + left - ox * k, 40 + top - oy * k))
            d = ImageDraw.Draw(canvas)
            d.text((6, 6), f"{seq['name']}  frame {f}  {t} ticks", fill=(255, 255, 0, 255))
            d.text((w + 14, 6), "new" if has else "orig (no new art)", fill=(255, 255, 0, 255))
            canvas = canvas.resize((max(1, int(canvas.width * a.scale)), max(1, int(canvas.height * a.scale))), Image.LANCZOS)
            gif.append((canvas.convert("RGB"), t * TICK_MS))
        target = out_dir / f"{seq['name']}.gif"
        gif[0][0].save(target, save_all=True, append_images=[g for g, _ in gif[1:]],
                       duration=[d for _, d in gif], loop=0 if seq.get("loop") else 1, disposal=2)
        print(f"{seq['name']}: {len(gif)} frames, {sum(d for _, d in gif)} ms -> {target.relative_to(common.ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
