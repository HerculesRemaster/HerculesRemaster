"""Frame cadence of a capture: decode to small grey frames and time the frames that change.

Prints JSON: frames, unique frames, unique fps, median/p95 interval between changes (ms),
histogram of intervals in recorded-frame units.
"""
import json
import subprocess
import sys
from collections import Counter

W, H = 160, 120


def main(path: str) -> int:
    probe = subprocess.run(["ffprobe", "-v", "error", "-select_streams", "v:0", "-show_entries",
                            "stream=r_frame_rate", "-of", "csv=p=0", path], capture_output=True, text=True)
    num, den = (int(x) for x in probe.stdout.strip().split("/"))
    fps = num / den
    proc = subprocess.Popen(["ffmpeg", "-v", "error", "-i", path, "-vf", f"scale={W}:{H}", "-f", "rawvideo",
                             "-pix_fmt", "gray", "-"], stdout=subprocess.PIPE)
    prev = None
    n = 0
    changes = []
    while True:
        buf = proc.stdout.read(W * H)
        if len(buf) < W * H:
            break
        if prev is not None and sum(abs(a - b) > 8 for a, b in zip(buf[::7], prev[::7])) > 20:
            changes.append(n)
        prev = buf
        n += 1
    proc.wait()
    gaps = [b - a for a, b in zip(changes, changes[1:])]
    gaps_ms = sorted(g * 1000 / fps for g in gaps)
    out = {
        "frames": n, "recorded_fps": fps, "changed_frames": len(changes),
        "unique_fps": round(len(changes) / (n / fps), 2) if n else 0,
        "median_interval_ms": round(gaps_ms[len(gaps_ms) // 2], 1) if gaps_ms else None,
        "p95_interval_ms": round(gaps_ms[int(len(gaps_ms) * 0.95)], 1) if gaps_ms else None,
        "interval_histogram_frames": dict(sorted(Counter(gaps).items())[:12]),
    }
    print(json.dumps(out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
