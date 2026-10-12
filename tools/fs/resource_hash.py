"""CTPAX name hash for HERCULES.FS ids, with name candidates from hercstls namelist.c.

    id = sum(toupper(c) << ((i & 3) * 8)) + strlen   (u32, full path such as M:\\GRAFIX\\MAPS\\...)

Confirmed in HERCULES.EXE: FS_HashName (RVA 0x4fe0) sums *signed* chars of the path that
FS_NormalizePath (RVA 0x4e10) produced; see docs/resource-id-scheme.md.

The packed name list is read at runtime from hercstls/sources/namelist.c (SZDD/LZSS,
then a prefix-coded string list) and never copied into the repo. Writes
manifests/name-candidates.json ({id hex: [names]}) for the archive ids.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fs_index import DEFAULT_FS, read_index  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_NAMELIST = ROOT.parent / "hercstls" / "sources" / "namelist.c"
SZDD_HEADER = 14  # COMPRESS.EXE header skipped by unhercfs
NAME_BUF = 64     # unhercfs: char s[64]


def herchash(name: str) -> int:
    """FS_HashName over an already normalized name (uppercases a-z like FS_NormalizePath)."""
    h = 0
    for i, c in enumerate(name.encode("latin-1")):
        if 0x61 <= c <= 0x7A:
            c -= 0x20
        if c >= 0x80:  # the game sums signed chars
            c -= 0x100
        h += c << ((i & 3) * 8)
    return (h + len(name)) & 0xFFFFFFFF


def normalize(name: str) -> str:
    """FS_NormalizePath: game-side name -> the archive path that FS_HashName hashes."""
    s = "".join(chr(ord(c) - 0x20) if "a" <= c <= "z" else c for c in name)
    if not s:
        return s
    if s[1:2] != ":":
        s = "M:" + (s if s[0] == "\\" else "\\" + s)
    drive = s[0]
    if drive == "M":
        return s
    if drive == "O":
        return "M:\\GRAFIX\\MAPS\\" + s[3:]
    if drive == "P":
        return "M:\\GRAFIX\\CHOP\\" + s[3:]
    if drive in "QR":
        return "M:\\LANGUAGE\\SONIX\\" + s[3:]
    k = 3  # any other drive: drop the drive and the first directory
    while k < len(s) and s[k] != "\\":
        k += 1
    return "M:" + s[k:]


def lzss_unpack(p: bytes, out_size: int) -> bytes:
    """MS COMPRESS (SZDD) LZSS as in namelist.c: 4 KiB window filled with spaces, start 0xFF0."""
    buf = bytearray(b" " * 0x1000)
    i = 0x1000 - 16
    out = bytearray()
    pi = 0
    while pi < len(p) and len(out) < out_size:
        b = p[pi]; pi += 1
        for _ in range(8):
            if not b & 1:
                if pi + 1 >= len(p):
                    return bytes(out)
                j = p[pi] | ((p[pi + 1] & 0xF0) << 4)
                n = (p[pi + 1] & 0x0F) + 3
                pi += 2
                for _ in range(n):
                    buf[i] = buf[j]
                    if len(out) >= out_size:
                        return bytes(out)
                    out.append(buf[i])
                    j = (j + 1) & 0xFFF
                    i = (i + 1) & 0xFFF
            else:
                if pi >= len(p):
                    return bytes(out)
                buf[i] = p[pi]; pi += 1
                if len(out) >= out_size:
                    return bytes(out)
                out.append(buf[i])
                i = (i + 1) & 0xFFF
            b >>= 1
    return bytes(out)


def unpack_names(packed: bytes) -> list[str]:
    """Replay unhercfs hercname(): 0x00 ends a name, 0x80|n advances, 0xC0|n rolls back, else a char."""
    limit = NAME_BUF - 1
    s = bytearray(NAME_BUF)
    j = 0
    names = []
    for b in packed:
        if b == 0:
            s[j] = 0
            names.append(bytes(s[:j]).decode("latin-1"))
        elif b & 0x80:
            j += -(b & 0x3F) if b & 0x40 else (b & 0x3F)
            j = min(max(j, 0), limit)
        elif j < limit:
            s[j] = b
            j += 1
    return names


def load_names(namelist: Path = DEFAULT_NAMELIST) -> list[str]:
    src = namelist.read_text(encoding="latin-1")
    size = int(re.search(r"packname\[(0x[0-9A-Fa-f]+|\d+)\]", src).group(1), 0)
    body = re.search(r"lzssname\[\]\s*=\s*\{(.*?)\};", src, re.S).group(1)
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    raw = bytes(int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", body))
    if raw[:4] != b"SZDD":
        raise ValueError("lzssname is not an SZDD stream")
    return [n for n in unpack_names(lzss_unpack(raw[SZDD_HEADER:], size)) if n]


def candidates(names: list[str]) -> dict[int, list[str]]:
    out: dict[int, list[str]] = defaultdict(list)
    for n in names:
        if n not in out[herchash(n)]:
            out[herchash(n)].append(n)
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--namelist", type=Path, default=DEFAULT_NAMELIST)
    ap.add_argument("--fs", type=Path, default=DEFAULT_FS)
    ap.add_argument("--out", type=Path, default=ROOT / "manifests" / "name-candidates.json")
    ap.add_argument("--hash", nargs="*", default=[], help="print the id of these names and exit")
    a = ap.parse_args(argv)
    if a.hash:
        for n in a.hash:
            print(f"{herchash(normalize(n)):08X} {normalize(n)}")
        return 0
    names = load_names(a.namelist)
    cand = candidates(names)
    with a.fs.open("rb") as f:
        ids = [e.id for e in read_index(f.read())]  # bounds check needs the full file
    archive_ids = sorted({i for i in ids if i})
    result = {f"{i:08X}": cand.get(i, []) for i in archive_ids}
    resolved = sum(1 for v in result.values() if v)
    multi = {k: v for k, v in result.items() if len(v) > 1}
    unused = sum(1 for h in cand if h not in set(archive_ids))
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=1), encoding="utf-8")
    print(f"names={len(names)} distinct_hashes={len(cand)} archive_ids={len(archive_ids)} "
          f"resolved={resolved} unresolved={len(archive_ids) - resolved} multi_candidate={len(multi)} "
          f"names_not_in_archive={unused} -> {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
