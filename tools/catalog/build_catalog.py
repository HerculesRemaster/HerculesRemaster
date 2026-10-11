"""Build the resource catalog: archive manifest + families + decoder coverage.

Inputs: manifests/archive-manifest.json, manifests/archive-families.json and every
extracted/meta/*-coverage.json. A coverage file is written by a decoder (P4) as
    {"decoder": "<name>", "entries": [{"index": int, "status": "decoded|partial|failed", "outputs": [...]}]}
Entries no decoder covers get status "raw". Output: manifests/catalog.json, one row per entry:
index, id, name, family, size, status, decoder, outputs.
"""
from __future__ import annotations

import argparse
import json
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--manifest", type=Path, default=ROOT / "manifests" / "archive-manifest.json")
    ap.add_argument("--families", type=Path, default=ROOT / "manifests" / "archive-families.json")
    ap.add_argument("--meta", type=Path, default=ROOT / "extracted" / "meta")
    ap.add_argument("--out", type=Path, default=ROOT / "manifests" / "catalog.json")
    a = ap.parse_args(argv)

    entries = json.loads(a.manifest.read_text(encoding="utf-8"))["entries"]
    families = json.loads(a.families.read_text(encoding="utf-8"))
    coverage, problems = {}, []
    for path in sorted(a.meta.glob("*-coverage.json")) if a.meta.is_dir() else []:
        cov = json.loads(path.read_text(encoding="utf-8"))
        for c in cov.get("entries", []):
            if c["index"] in coverage:
                problems.append(f"index {c['index']} covered by {coverage[c['index']]['decoder']} and {cov['decoder']}")
            coverage[c["index"]] = {"decoder": cov["decoder"], "status": c["status"], "outputs": c.get("outputs", [])}

    rows = []
    for e in entries:
        cov = coverage.get(e["index"], {})
        rows.append({
            "index": e["index"],
            "id": e["id"],
            "name": e["name_candidate"],
            "family": families[str(e["index"])],
            "size": e["size"],
            "status": cov.get("status", "raw" if e["size"] else "empty"),
            "decoder": cov.get("decoder"),
            "outputs": cov.get("outputs", []),
        })
    known = {e["index"] for e in entries}
    problems += [f"coverage for unknown index {i}" for i in coverage if i not in known]
    a.out.write_text(json.dumps(rows, indent=1), encoding="utf-8")
    status = Counter(r["status"] for r in rows)
    print(f"rows={len(rows)} coverage_files={len({c['decoder'] for c in coverage.values()})} "
          + " ".join(f"{k}={v}" for k, v in status.most_common()) + f" problems={len(problems)} -> {a.out}")
    for p in problems[:20]:
        print(f"problem: {p}", file=sys.stderr)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
