"""Transformation records (master §5): command line, tool version, input/config/output hashes, UTC timestamp.

Every tool that writes outputs calls ``record_transformation``; one JSON line is appended to
``manifests/provenance/<tool>.jsonl``. Records are append-only and never rewritten.
"""
from __future__ import annotations

import datetime as _dt
import hashlib
import json
import platform
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PROVENANCE_DIR = ROOT / "manifests" / "provenance"


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with Path(path).open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def canonical_json(obj) -> str:
    """Stable serialisation used for configuration hashes (sorted keys, no whitespace)."""
    return json.dumps(obj, sort_keys=True, separators=(",", ":"), ensure_ascii=False, default=str)


def config_hash(config: dict) -> str:
    return sha256_bytes(canonical_json(config).encode("utf-8"))


def display_path(path: Path, root: Path = ROOT) -> str:
    p = Path(path).resolve()
    try:
        return p.relative_to(Path(root).resolve()).as_posix()
    except ValueError:
        return str(p)


def hash_paths(paths, root: Path = ROOT) -> dict[str, str]:
    """{display path: sha256} for existing files, in the given order."""
    return {display_path(p, root): sha256_file(p) for p in paths}


def record_transformation(tool: str, argv: list[str], tool_version: str, inputs: dict[str, str], config: dict,
                          outputs: dict[str, str], provenance_dir: Path = PROVENANCE_DIR, now: _dt.datetime | None = None) -> dict:
    """Append one provenance record for ``tool`` and return it.

    ``inputs`` and ``outputs`` map a path (repository-relative where possible) to its sha256.
    """
    if not tool or any(c in tool for c in "/\\:"):
        raise ValueError(f"tool must be a plain name, got {tool!r}")
    for kind, mapping in (("inputs", inputs), ("outputs", outputs)):
        for path, digest in mapping.items():
            if not (isinstance(digest, str) and len(digest) == 64 and all(c in "0123456789abcdef" for c in digest)):
                raise ValueError(f"{kind}[{path!r}] is not a lowercase sha256: {digest!r}")
    stamp = (now or _dt.datetime.now(_dt.timezone.utc)).astimezone(_dt.timezone.utc).replace(microsecond=0)
    record = {
        "tool": tool,
        "argv": list(argv),
        "tool_version": tool_version,
        "python": platform.python_version(),
        "platform": platform.platform(),
        "inputs": dict(inputs),
        "config": config,
        "config_sha256": config_hash(config),
        "outputs": dict(outputs),
        "timestamp_utc": stamp.isoformat(),
    }
    provenance_dir = Path(provenance_dir)
    provenance_dir.mkdir(parents=True, exist_ok=True)
    with (provenance_dir / f"{tool}.jsonl").open("a", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")
    return record


def read_records(tool: str, provenance_dir: Path = PROVENANCE_DIR) -> list[dict]:
    path = Path(provenance_dir) / f"{tool}.jsonl"
    if not path.exists():
        return []
    return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines() if line.strip()]


if __name__ == "__main__":  # print the records of one tool
    for rec in read_records(sys.argv[1] if len(sys.argv) > 1 else "preserve"):
        print(json.dumps(rec, indent=1))
