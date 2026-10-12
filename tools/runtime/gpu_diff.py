"""Compare a GPU-renderer frame with the EXE's own frame of the same tick.

gpu_diff.py <dir> <frame> [<frame> ...]
Reads <dir>/gpu_<n>.png (back buffer, scale s, centred) and <dir>/soft_<n>.png (320x240). The EXE
samples each pixel at its top-left corner, so the GPU frame is sampled at the first sub-pixel of each
s x s block. Writes <dir>/diff_<n>.png (soft frame dimmed, mismatching pixels red) and prints the
mismatch count.
"""
import sys
from pathlib import Path

from PIL import Image


def diff(d: Path, n: str) -> None:
    gpu = Image.open(d / f'gpu_{n}.png').convert('RGB')
    soft = Image.open(d / f'soft_{n}.png').convert('RGB')
    gw, gh = gpu.size
    s = min(gw // 320, gh // 240)
    ox, oy = (gw - 320 * s) // 2, (gh - 240 * s) // 2
    g, sp = gpu.load(), soft.load()
    out = Image.new('RGB', (320, 240))
    o = out.load()
    bad = 0
    for y in range(240):
        for x in range(320):
            a = sp[x, y]
            if g[ox + x * s, oy + y * s] != a:
                bad += 1
                o[x, y] = (255, 0, 0)
            else:
                o[x, y] = tuple(c // 3 for c in a)
    out.resize((960, 720), Image.NEAREST).save(d / f'diff_{n}.png')
    print(f'frame {n}: scale {s}, {bad} of 76800 pixels differ ({bad / 768:.2f}%)')


if __name__ == '__main__':
    for n in sys.argv[2:]:
        diff(Path(sys.argv[1]), n)
