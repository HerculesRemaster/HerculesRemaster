"""Extract every HERCULES.FS entry byte-exactly (no TGA patch) and write the archive manifest.

Payloads: extracted/raw/<index:04d>_<id:08X>.bin. Manifest: manifests/archive-manifest.json,
one row per record with a non-zero id or size (index, id, offset, size, sha256, magic,
name_candidate, extension, file). Every written file is re-read and compared with its
archive slice.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from fs_index import DEFAULT_FS, read_index  # noqa: E402
from resource_hash import DEFAULT_NAMELIST, candidates, load_names  # noqa: E402
from tools.common.provenance import display_path, record_transformation, sha256_file  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
VERSION = "1"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--fs", type=Path, default=DEFAULT_FS)
    ap.add_argument("--namelist", type=Path, default=DEFAULT_NAMELIST)
    ap.add_argument("--out", type=Path, default=ROOT / "extracted" / "raw")
    ap.add_argument("--manifest", type=Path, default=ROOT / "manifests" / "archive-manifest.json")
    a = ap.parse_args(argv)

    data = a.fs.read_bytes()
    cand = candidates(load_names(a.namelist))
    a.out.mkdir(parents=True, exist_ok=True)
    rows, mismatches, written = [], [], 0
    for e in read_index(data):
        if not e.id and not e.size:
            continue
        payload = data[e.offset:e.offset + e.size]
        names = cand.get(e.id, [])
        name = names[0] if len(names) == 1 else None
        row = {
            "index": e.index,
            "id": f"{e.id:08X}",
            "offset": e.offset,
            "size": e.size,
            "sha256": hashlib.sha256(payload).hexdigest() if e.size else None,
            "magic": payload[:4].hex() if e.size else None,
            "name_candidate": name,
            "extension": name.rsplit(".", 1)[-1] if name and "." in name else None,
            "file": None,
        }
        if e.size:
            path = a.out / f"{e.index:04d}_{e.id:08X}.bin"
            path.write_bytes(payload)
            written += 1
            if path.read_bytes() != data[e.offset:e.offset + e.size]:
                mismatches.append(e.index)
            row["file"] = display_path(path)
        rows.append(row)

    manifest = {"archive": display_path(a.fs), "archive_sha256": hashlib.sha256(data).hexdigest(),
                "archive_size": len(data), "entries": rows}
    a.manifest.parent.mkdir(parents=True, exist_ok=True)
    a.manifest.write_text(json.dumps(manifest, indent=1), encoding="utf-8")
    record_transformation("fs_extract", sys.argv, VERSION,
                          inputs={display_path(a.fs): manifest["archive_sha256"],
                                  display_path(a.namelist): sha256_file(a.namelist)},
                          config={"tga_patch": False, "layout": "<index:04d>_<id:08X>.bin"},
                          outputs={display_path(a.manifest): sha256_file(a.manifest)})
    named = sum(1 for r in rows if r["name_candidate"])
    print(f"rows={len(rows)} written={written} named={named} reread_mismatch={len(mismatches)} "
          f"-> {display_path(a.out)}, {display_path(a.manifest)}")
    if mismatches:
        print(f"mismatch indices: {mismatches[:20]}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
