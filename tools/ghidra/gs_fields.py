"""Game-state field use per function: gs_fields.py <program> <gs pointer global> [--out file]

Decompiles every function that references the game-state pointer global (TITLE 0x1002bb44, ENG1
0x10070f24, ...) and lists the field offsets each one reads and writes (regex over the decompiler
text: `ptr[0x..]`, `*(T *)(ptr + 0x..)`, `ptr[n]`). Writes are lines where the access is followed
by a plain `=`. Output: JSON {function: {"addr", "reads": [...], "writes": [...]}} and a short
per-field summary on stdout.
"""
import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("program")
    ap.add_argument("ptr")
    ap.add_argument("--out", type=Path)
    a = ap.parse_args(argv)
    refs = m.get("get_xrefs_to", a.program, address=a.ptr, limit=1000)["references"]
    funcs = sorted({r.get("from_function") for r in refs if r.get("from_function")})
    name = "DAT_" + a.ptr[2:].lower().lstrip("0").rjust(8, "0")
    acc = re.compile(rf"(?:\*\([^)]*\)\s*\(\s*{name}\s*\+\s*(0x[0-9a-f]+|\d+)\s*\)|{name}\s*\[\s*(0x[0-9a-f]+|\d+)\s*\])(\s*=(?!=))?")
    out, fields = {}, defaultdict(lambda: {"r": set(), "w": set()})
    for f in funcs:
        try:
            r = m.get("force_decompile", a.program, function=f)
        except m.GhidraError:
            continue
        text = r.get("decompiled") if isinstance(r, dict) else r
        if not isinstance(text, str):
            continue
        reads, writes = set(), set()
        for mm in acc.finditer(text):
            off = int(mm.group(1) or mm.group(2), 0)
            (writes if mm.group(3) else reads).add(off)
        if not reads and re.search(rf"\*{name}\b", text):
            reads.add(0)
        for o in reads:
            fields[o]["r"].add(f)
        for o in writes:
            fields[o]["w"].add(f)
        out[f] = {"reads": sorted(reads), "writes": sorted(writes)}
    if a.out:
        a.out.parent.mkdir(parents=True, exist_ok=True)
        a.out.write_text(json.dumps(out, indent=1), encoding="utf-8")
    for o in sorted(fields):
        print(f"+0x{o:x}: written by {sorted(fields[o]['w'])}  read by {len(fields[o]['r'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
