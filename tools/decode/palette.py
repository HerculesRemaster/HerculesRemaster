"""PSX colour and CLUT helpers shared by the decoders (rules from ENG1.DLL 0x5780 and the PSX GPU).

- psx_rgba(c): 15-bit BGR555 -> RGBA; 0x0000 is transparent (bit 15 = STP is ignored for colour).
- sprite_clut(P, off): the 256-entry CLUT that ENG1 0x5780 builds from a bank palette at P+off
  (u16 n, u16, n colours from +4): n == 256 -> as is; else entry 0 = 0 and entries 1..n-1 =
  colour | 0x8000, repeated to fill 256.
- tga_palette(data, off, n): 24-bit BGR TGA colour map -> list of RGB tuples.
"""
from __future__ import annotations

import struct


def psx_rgba(c: int) -> tuple[int, int, int, int]:
    c &= 0x7FFF
    if c == 0:
        return (0, 0, 0, 0)
    r, g, b = c & 31, (c >> 5) & 31, (c >> 10) & 31
    return ((r << 3) | (r >> 2), (g << 3) | (g >> 2), (b << 3) | (b >> 2), 255)


def sprite_clut(P: bytes, off: int) -> list[tuple[int, int, int, int]]:
    n = struct.unpack_from("<H", P, off)[0]
    cols = struct.unpack_from(f"<{n}H", P, off + 4) if n else ()
    if n == 256:
        raw = list(cols)
    else:
        base = [0] + [c | 0x8000 for c in cols[1:n]] if n else [0]
        raw = [base[i % len(base)] for i in range(256)]
    return [psx_rgba(c) for c in raw]


def tga_palette(data: bytes, off: int, n: int) -> list[tuple[int, int, int]]:
    return [(data[off + 3 * i + 2], data[off + 3 * i + 1], data[off + 3 * i]) for i in range(n)]
