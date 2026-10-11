"""Parse and check the HERCULES.FS index (1,024 x 12-byte records: id, offset, size, LE u32).

Reports overlaps, gaps, trailing bytes, zero-size entries, duplicate ids and groups of
identical payloads. Prints a summary; the full report goes to manifests/fs-index-report.json.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_FS = ROOT / "backups" / "originals" / "HERCULES.FS"
RECORDS = 1024
RECORD_SIZE = 12
INDEX_SIZE = RECORDS * RECORD_SIZE  # 0x3000


@dataclass(frozen=True)
class Entry:
    index: int
    id: int
    offset: int
    size: int


def read_index(data: bytes) -> list[Entry]:
    if len(data) < INDEX_SIZE:
        raise ValueError(f"archive shorter than its index: {len(data)} < {INDEX_SIZE}")
    entries = []
    for i in range(RECORDS):
        rid, off, size = struct.unpack_from("<III", data, i * RECORD_SIZE)
        if size and not (INDEX_SIZE <= off and off + size <= len(data)):
            raise ValueError(f"record {i}: id={rid:08X} [{off:#x}+{size:#x}] outside payload area 0x3000..{len(data):#x}")
        entries.append(Entry(i, rid, off, size))
    return entries


def check(entries: list[Entry], data: bytes) -> dict:
    nonempty = sorted((e for e in entries if e.size), key=lambda e: e.offset)
    overlaps, gaps = [], []
    pos = INDEX_SIZE
    for e in nonempty:
        if e.offset > pos:
            gaps.append({"start": pos, "end": e.offset, "before_index": e.index})
        elif e.offset < pos:
            overlaps.append({"index": e.index, "offset": e.offset, "previous_end": pos})
        pos = max(pos, e.offset + e.size)
    by_id = defaultdict(list)
    for e in entries:
        if e.id:
            by_id[e.id].append(e.index)
    by_hash = defaultdict(list)
    for e in nonempty:
        by_hash[hashlib.sha256(data[e.offset:e.offset + e.size]).hexdigest()].append(e.index)
    zero_size = [e for e in entries if not e.size]
    zero_id = [e.index for e in entries if not e.id]
    return {
        "archive_size": len(data),
        "records": len(entries),
        "nonempty": len(nonempty),
        "zero_size": [{"index": e.index, "id": f"{e.id:08X}", "offset": e.offset} for e in zero_size],
        "zero_id_indices": zero_id,
        "overlaps": overlaps,
        "gaps": gaps,
        "trailing_bytes": max(0, len(data) - pos),
        "payload_end": pos,
        "duplicate_ids": {f"{k:08X}": v for k, v in by_id.items() if len(v) > 1},
        "identical_payload_groups": [v for v in by_hash.values() if len(v) > 1],
    }


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--fs", type=Path, default=DEFAULT_FS)
    ap.add_argument("--out", type=Path, default=ROOT / "manifests" / "fs-index-report.json")
    a = ap.parse_args(argv)
    data = a.fs.read_bytes()
    entries = read_index(data)
    r = check(entries, data)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(r, indent=1), encoding="utf-8")
    zi = r["zero_id_indices"]
    print(f"records={r['records']} nonempty={r['nonempty']} zero_size={len(r['zero_size'])} "
          f"zero_id={len(zi)} (first {zi[0] if zi else '-'})")
    print(f"overlaps={len(r['overlaps'])} gaps={len(r['gaps'])} trailing={r['trailing_bytes']} "
          f"payload_end={r['payload_end']:#x}/{r['archive_size']:#x}")
    print(f"duplicate_ids={len(r['duplicate_ids'])} identical_payload_groups={len(r['identical_payload_groups'])} "
          f"(entries in groups: {sum(len(g) for g in r['identical_payload_groups'])}) -> {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
