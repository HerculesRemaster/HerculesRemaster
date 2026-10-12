"""Turn a hook frame dump (frames.bin) into a lossless video and a cadence report.

frames.bin records (runtime/hooks/hercules_hook.c, HERC_HOOK_FRAMES):
  "FRM0", u32 tick, f64 ms, RGBQUAD[256] (B,G,R,0), u8[320*240] palette indices, top-down.

    frames_to_video.py frames.bin out.mkv [--start-ms T] [--seconds S] [--scale N]

Writes out.mkv (ffmpeg, libx264rgb -qp 0, 30 fps, one video frame per presented frame) and
prints a JSON summary: frames, present rate, interval stats, ticks per present, changed frames.
"""
from __future__ import annotations

import argparse
import json
import struct
import subprocess
import sys
from pathlib import Path

W, H = 320, 240
REC = 4 + 4 + 8 + 1024 + W * H


def frames(path: Path):
    with path.open("rb") as f:
        while True:
            rec = f.read(REC)
            if len(rec) < REC or rec[:4] != b"FRM0":
                return
            tick, ms = struct.unpack_from("<Id", rec, 4)
            yield tick, ms, rec[16:16 + 1024], rec[16 + 1024:]


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("frames", type=Path)
    ap.add_argument("out", type=Path)
    ap.add_argument("--start-ms", type=float, default=0.0)
    ap.add_argument("--seconds", type=float, default=0.0)
    ap.add_argument("--scale", type=int, default=1, help="nearest-neighbour upscale in the video")
    a = ap.parse_args(argv)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    vf = ["-vf", f"scale={W * a.scale}:{H * a.scale}:flags=neighbor"] if a.scale > 1 else []
    enc = subprocess.Popen(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", f"{W}x{H}",
                            "-r", "30", "-i", "-", *vf, "-c:v", "libx264rgb", "-qp", "0", "-preset", "veryfast",
                            str(a.out)], stdin=subprocess.PIPE)
    times, ticks, changed = [], [], 0
    prev = None
    lut_cache: dict[bytes, bytes] = {}
    for tick, ms, pal, px in frames(a.frames):
        if ms < a.start_ms:
            continue
        if a.seconds and ms > a.start_ms + a.seconds * 1000:
            break
        lut = lut_cache.get(pal)
        if lut is None:
            lut = b"".join(bytes((pal[i * 4 + 2], pal[i * 4 + 1], pal[i * 4])) for i in range(256))
            lut_cache = {pal: lut}
        rgb = bytearray(W * H * 3)
        for i, c in enumerate(px):
            j = c * 3
            rgb[i * 3:i * 3 + 3] = lut[j:j + 3]
        enc.stdin.write(rgb)
        if prev is not None and (px, pal) != prev:
            changed += 1
        prev = (px, pal)
        times.append(ms)
        ticks.append(tick)
    enc.stdin.close()
    enc.wait()
    gaps = sorted(b - a_ for a_, b in zip(times, times[1:]))
    dticks = [b - a_ for a_, b in zip(ticks, ticks[1:])]
    dur = (times[-1] - times[0]) / 1000 if len(times) > 1 else 0
    out = {
        "video": str(a.out), "frames": len(times), "seconds": round(dur, 2),
        "present_fps": round((len(times) - 1) / dur, 2) if dur else 0,
        "median_interval_ms": round(gaps[len(gaps) // 2], 1) if gaps else None,
        "p95_interval_ms": round(gaps[int(len(gaps) * 0.95)], 1) if gaps else None,
        "max_interval_ms": round(gaps[-1], 1) if gaps else None,
        "ticks_per_present": {str(k): dticks.count(k) for k in sorted(set(dticks))},
        "changed_frames": changed, "first_tick": ticks[0] if ticks else None, "last_tick": ticks[-1] if ticks else None,
    }
    print(json.dumps(out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
