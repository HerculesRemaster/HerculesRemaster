"""Create the functions Ghidra's auto-analysis missed because they are only reached through pointer
tables (behaviour/state tables in the game DLLs).

create_table_funcs.py <program> <dll file>

Scans the non-code sections of the DLL file for runs of >= 4 consecutive dwords that point into
.text, and creates a function at every target that is not a function start yet; then saves.
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402


def sections(b):
    pe = struct.unpack_from("<I", b, 0x3c)[0]
    n = struct.unpack_from("<H", b, pe + 6)[0]
    opt = struct.unpack_from("<H", b, pe + 20)[0]
    base = struct.unpack_from("<I", b, pe + 24 + 28)[0]
    out, o = [], pe + 24 + opt
    for _ in range(n):
        vs, va, raw, ptr = struct.unpack_from("<IIII", b, o + 8)
        out.append((va, vs, raw, ptr))
        o += 40
    return base, out


def main(prog, path):
    b = Path(path).read_bytes()
    base, secs = sections(b)
    tva, tvs = secs[0][0], secs[0][1]
    lo, hi = base + tva, base + tva + tvs
    funcs = {int(f["address"], 16) for f in m.get_all("find_functions", prog, "functions")}
    targets = set()
    for va, vs, raw, ptr in secs[1:]:
        data = b[ptr:ptr + min(vs, raw)]
        run = []
        for o in range(0, len(data) - 3, 4):
            v = struct.unpack_from("<I", data, o)[0]
            if lo <= v < hi:
                run.append(v)
            else:
                if len(run) >= 4:
                    targets.update(run)
                run = []
    todo = sorted(t for t in targets if t not in funcs)
    ok = 0
    for a in todo:
        for _ in range(2):
            try:
                if m.post("create_function", prog, {"address": hex(a)}).get("success"):
                    ok += 1
                    break
            except m.GhidraError:
                pass
    m.get("save_program", prog)
    print(f"{prog}: {len(targets)} table targets, {len(todo)} missing, {ok} created")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
