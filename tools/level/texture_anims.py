"""Animated and scrolling level textures (ENG1 maps, L*.AS.02).

    texture_anims.py [--runs RUN_DIR ...]  -> extracted/full-game/levels/<LEVEL>/texture-anims.json (+ anim_<i>_f<n>.png)

Tables (LVL_ParseHeader 0x20670): after the 4 camera/shade rels, T34 holds n_types + n_extra u32, then
T38 (n_anim = header +0x10) and T3c (n_scroll = +0x12) u32 entries, each self-relative (entry + value).
  T38 record (LVL_AnimateTextures 0x2cb0, called per frame by 0x205fc): u16 frames, u16 period, u16 x, y, w, h
      (VRAM units, 16-bit), then frames x (w*h) u16 texel pairs. When the game frame counter gs+0x38 is a
      multiple of period, frame (counter / period) % frames is uploaded to (x, y) (0x2c7f0 = LoadImage).
  T3c record (LVL_ScrollTexture 0x2d80, args index, dx, dy): 6 bytes, then u16 dst x, dst y, w, h, s16 src x, src y.
      The byte offsets (0x70c58 + 2*index) advance by (dx, dy) modulo (w, h); the source rect is copied to
      (dst x, dst y) rotated by those offsets (4 VRAM-to-VRAM moves, 0x2c60). Nothing scrolls by itself: the
      callers (static, every E8 call in ENG1) are listed in CALLERS.
With --runs, each run's vram_<tick>.bin (hook HERC_HOOK_VRAM_TICKS) is checked: which anim frame is in VRAM, and
the scroll offset (ox, oy) for which the destination equals the rotated source.
"""
from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
LEVELS = ROOT / "extracted" / "full-game" / "levels"
MANIFEST = ROOT / "manifests" / "archive-manifest.json"

# every E8 call to LVL_ScrollTexture 0x2d80 in ENG1 (args pushed as immediates unless named)
CALLERS = [
    {"call": "0xd47d", "in": "HERO_WaterUpdate3 0xd460 (FOREST kind 0x4b update)", "index": 0, "dx": 0, "dy": -16,
     "when": "once per game frame (gs+0x38 changed since the last call)"},
    *[{"call": c, "in": "TRAINING device actors 0x3ca80..0x3e5d0 (kinds 0x2a/0x30/0x32/0x33/0x35/0x39/0x3e/0x43/0x4d)",
       "index": i, "dx": dx, "dy": dy, "when": "while the device is driven (actor state)"}
      for c, i, dx, dy in [("0x3cc7f", 0, 0, 1), ("0x3cc95", 0, 0, 2), ("0x3cd13", 0, 0, 1), ("0x3d026", "esi", 5, 0),
                           ("0x3d18d", 5, 0, 2), ("0x3d1a3", 5, 0, 2), ("0x3d231", 5, 0, 2), ("0x3d561", 0, 0, 1),
                           ("0x3d57a", 0, 0, 2), ("0x3d5f8", 0, 0, 1), ("0x3da23", 0, 0, 1), ("0x3da39", 0, 0, 2),
                           ("0x3dab4", 0, 0, 1), ("0x3dde7", 1, 0, "ecx"), ("0x3df50", 5, 0, 1), ("0x3df69", 5, 0, 2),
                           ("0x3dfe7", 5, 0, 1), ("0x3e422", "eax", 0, "edx")]],
]
CALLER_LEVELS = {"FOREST": ["0xd47d"], "TRAINING": [c["call"] for c in CALLERS[1:]]}


def u16(d, o): return struct.unpack_from("<H", d, o)[0]
def s16(d, o): return struct.unpack_from("<h", d, o)[0]
def u32(d, o): return struct.unpack_from("<I", d, o)[0]


def tables(d: bytes):
    sections, n_types, n_extra, n_anim, n_scroll = u16(d, 8), u16(d, 0xc), u16(d, 0xe), u16(d, 0x10), u16(d, 0x12)
    t34 = 0x30 + 4 * sections + 16
    t38 = t34 + 4 * (n_types + n_extra)
    t3c = t38 + 4 * n_anim
    anims, scrolls = [], []
    for i in range(n_anim):
        e = t38 + 4 * i
        r = e + u32(d, e)
        n, period, x, y, w, h = struct.unpack_from("<6H", d, r)
        frames = [np.frombuffer(d, np.uint8, w * h * 2, r + 12 + f * w * h * 2).reshape(h, w * 2) for f in range(n)]
        anims.append({"index": i, "record": r, "frames": n, "period": period, "rect_units": [x, y, w, h], "_px": frames})
    for i in range(n_scroll):
        e = t3c + 4 * i
        r = e + u32(d, e)
        head = list(d[r:r + 6])
        dx, dy, w, h = struct.unpack_from("<4H", d, r + 6)
        sx, sy = s16(d, r + 14), s16(d, r + 16)
        scrolls.append({"index": i, "record": r, "head_bytes": head, "dst_units": [dx, dy], "size_units": [w, h],
                        "src_units": [sx, sy]})
    return anims, scrolls


def unswizzle(raw: bytes) -> np.ndarray:                     # as tools/level/vram_compare.py
    v = np.frombuffer(raw, np.uint8)
    lin = np.zeros((512, 2048), np.uint8)
    for half in range(2):
        for c in range(8):
            o = half * 0x80000 + c * 0x10000
            lin[half * 256:(half + 1) * 256, c * 256:(c + 1) * 256] = v[o:o + 0x10000].reshape(256, 256)
    return lin


def check_run(run: Path, anims, scrolls, file_vram: np.ndarray) -> dict:
    out = {}
    for p in sorted(run.glob("vram_*.bin")):
        lin = unswizzle(p.read_bytes())
        r = {"anims": [], "scrolls": []}
        for a in anims:
            x, y, w, h = a["rect_units"]
            cur = lin[y:y + h, x * 2:(x + w) * 2]
            hit = [f for f, px in enumerate(a["_px"]) if np.array_equal(cur, px)]
            r["anims"].append({"index": a["index"], "frame_in_vram": hit[0] if hit else None,
                               "equals_file": bool(np.array_equal(cur, file_vram[y:y + h, x * 2:(x + w) * 2]))})
        for s in scrolls:
            (dx, dy), (w, h), (sx, sy) = s["dst_units"], s["size_units"], s["src_units"]
            src = lin[sy:sy + h, sx * 2:(sx + w) * 2]
            dst = lin[dy:dy + h, dx * 2:(dx + w) * 2]
            found = None
            if src.shape == dst.shape and src.size:
                for oy in range(h):
                    rolled = np.roll(src, -oy, axis=0)
                    for ox in range(w):
                        if np.array_equal(np.roll(rolled, -2 * ox, axis=1), dst):
                            found = [ox, oy]
                            break
                    if found:
                        break
            r["scrolls"].append({"index": s["index"], "offset_units": found,
                                 "dst_equals_file": bool(np.array_equal(dst, file_vram[dy:dy + h, dx * 2:(dx + w) * 2]))})
        out[p.name] = r
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--runs", nargs="*", default=[])
    a = ap.parse_args(argv)
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    runs = [Path(r).resolve() for r in a.runs]
    for e in entries:
        nm = (e.get("name_candidate") or "").upper()
        if not nm.endswith("MAP_PSX.BIN"):
            continue
        level = nm.replace("\\", "/").split("/")[-2]
        out = LEVELS / level
        if not (out / "level.json").is_file():
            continue
        d = (ROOT / e["file"]).read_bytes()
        if d[:4] != b"i2.5":
            continue
        anims, scrolls = tables(d)
        pal = np.array(json.loads((out / "palette.json").read_text()), np.uint8)
        for f in out.glob("anim_*_f*.png"):
            f.unlink()
        for an in anims:
            for f, px in enumerate(an["_px"]):
                Image.fromarray(pal[px]).save(out / f"anim_{an['index']}_f{f}.png")
        file_vram = np.frombuffer((out / "vram.bin").read_bytes(), np.uint8).reshape(512, 2048)
        res = {"_doc": __doc__.split("\n\n")[1].strip(), "level": level, "archive_index": e["index"],
               "anims": [{k: v for k, v in an.items() if k != "_px"} for an in anims], "scrolls": scrolls,
               "scroll_callers": [c for c in CALLERS if c["call"] in CALLER_LEVELS.get(level, [])],
               "runtime": {}}
        for run in runs:
            if (run / "run.json").is_file() and level in (run / "resource-usage.jsonl").read_text(encoding="utf-8", errors="replace"):
                res["runtime"][str(run.relative_to(ROOT)).replace("\\", "/")] = check_run(run, anims, scrolls, file_vram)
        (out / "texture-anims.json").write_text(json.dumps(res, indent=1) + "\n", encoding="utf-8")
        print(level, f"anims {[(x['frames'], x['period'], x['rect_units']) for x in res['anims']]}",
              f"scrolls {[(x['dst_units'], x['size_units'], x['src_units']) for x in scrolls]}",
              json.dumps(res["runtime"])[:400])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
