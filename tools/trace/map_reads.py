"""Map the OS-level reads of hercules.fs in a hook trace to archive entries.

    map_reads.py analysis/traces/runs/<run>/resource-usage.jsonl [--out read-map.json]

os_read events (ReadFile from HERCULES.EXE, file position + bytes) are split over the index
entries they touch. Each FS-level read (FS_Read: entry index, position, bytes) is checked to
be covered by an OS read at offset + position, which confirms the index/offset mapping from
the game's side. Writes a JSON report next to the trace and prints a summary.
"""
from __future__ import annotations

import argparse
import bisect
import json
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "fs"))
from fs_index import DEFAULT_FS, read_index  # noqa: E402


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("trace", type=Path)
    ap.add_argument("--fs", type=Path, default=DEFAULT_FS)
    ap.add_argument("--out", type=Path)
    a = ap.parse_args(argv)
    with a.fs.open("rb") as f:
        entries = read_index(f.read())
    spans = sorted((e.offset, e.offset + e.size, i) for i, e in enumerate(entries) if e.size)
    starts = [s[0] for s in spans]

    def touched(lo: int, hi: int) -> list[int]:
        out, k = [], max(bisect.bisect_right(starts, lo) - 1, 0)
        while k < len(spans) and spans[k][0] < hi:
            if spans[k][1] > lo:
                out.append(spans[k][2])
            k += 1
        return out

    ev = [json.loads(line) for line in a.trace.open(encoding="utf-8")]
    os_reads = [e for e in ev if e["ev"] == "os_read"]
    fs_reads = [e for e in ev if e["ev"] == "read" and e["bytes"]]
    index_area = 0
    per_entry: Counter = Counter()
    rows = []
    for e in os_reads:
        lo, hi = e["file_pos"], e["file_pos"] + e["got"]
        if hi <= 0x3000:
            index_area += 1
            rows.append({"t": e["t"], "file_pos": lo, "bytes": e["got"], "entries": ["index"]})
            continue
        ids = touched(lo, hi)
        per_entry.update(ids)
        rows.append({"t": e["t"], "file_pos": lo, "bytes": e["got"], "entries": ids, "caller": e["caller"]})
    # coverage check: every FS-level read lies inside OS-read bytes
    cov = sorted((r["file_pos"], r["file_pos"] + r["bytes"]) for r in rows)
    merged = []
    for lo, hi in cov:
        if merged and lo <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], hi)
        else:
            merged.append([lo, hi])
    mstarts = [m[0] for m in merged]
    uncovered = []
    for e in fs_reads:
        lo = entries[e["index"]].offset + e["pos"]
        hi = lo + e["bytes"]
        k = bisect.bisect_right(mstarts, lo) - 1
        if k < 0 or merged[k][1] < hi:
            uncovered.append({"index": e["index"], "pos": e["pos"], "bytes": e["bytes"], "t": e["t"]})
    report = {
        "trace": str(a.trace), "os_reads": len(os_reads), "index_reads": index_area, "fs_reads": len(fs_reads),
        "fs_reads_not_covered": len(uncovered), "entries_touched": len(per_entry),
        "os_read_callers": dict(Counter(r.get("caller", "-") for r in rows)),
        "uncovered": uncovered[:50], "reads": rows,
    }
    out = a.out or a.trace.with_name("read-map.json")
    out.write_text(json.dumps(report, indent=1), encoding="utf-8")
    print(f"os_reads={len(os_reads)} (index area {index_area}) entries_touched={len(per_entry)} "
          f"fs_reads={len(fs_reads)} not_covered={len(uncovered)} callers={report['os_read_callers']} -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
