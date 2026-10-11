"""Preserve the original game files: read-only copies, SHA256SUMS and a file inventory.

Master §6.2-6.4 and plan module 00a (requirement R01).

    .venv/Scripts/python.exe tools/common/preserve.py            copy + write manifests
    .venv/Scripts/python.exe tools/common/preserve.py --verify   re-hash sources and copies

Exit codes: 0 ok, 1 hash mismatch or missing file, 2 refused (a differing copy exists, or --dest/--manifests
point outside the repository). The source folder is only ever read. Each copy run appends a provenance record
to manifests/provenance/preserve.jsonl.
"""
from __future__ import annotations

import argparse
import datetime as _dt
import hashlib
import json
import os
import shutil
import stat
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from tools.common import provenance  # noqa: E402  (after the sys.path bootstrap)

TOOL_VERSION = "1.1"
DEFAULT_SRC = ROOT.parent  # C:\Games\GameHercules
DEFAULT_DEST = ROOT / "backups" / "originals"
MANIFESTS = ROOT / "manifests"

# The game edition recorded in docs/canonical-edition.md.
GAME_FILES = [
    "HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL", "HERCULES.FS", "HERCULES.HLP",
    "DISNEY.INI", "HERC.REG", "SETUP.BAT", "ReadMe.txt", "unins000.dat",
]

EXIT_OK, EXIT_MISMATCH, EXIT_REFUSED = 0, 1, 2


class RefusedOverwrite(Exception):
    pass


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def rel(path: Path, root: Path) -> str:
    return path.resolve().relative_to(root.resolve()).as_posix()


def import_descriptor_dlls(pe) -> list[str]:
    """DLL names of every import descriptor, including descriptors that import no functions.

    pefile drops descriptors without imported functions (HERCULES.EXE has one for comdlg32.dll), so the
    descriptor array is walked directly: 20-byte IMAGE_IMPORT_DESCRIPTORs, Name RVA at offset 12, zero-terminated.
    """
    import pefile

    entry = pe.OPTIONAL_HEADER.DATA_DIRECTORY[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]]
    names, rva = [], entry.VirtualAddress
    while rva and len(names) < 4096:
        raw = pe.get_data(rva, 20)
        if len(raw) < 20 or raw == bytes(20):
            break
        name_rva = struct.unpack_from("<I", raw, 12)[0]
        names.append(pe.get_string_at_rva(name_rva).decode("ascii", "replace"))
        rva += 20
    return names


def pe_info(path: Path) -> dict | None:
    """PE metadata for the inventory, or None when the file is not a PE image."""
    data = path.read_bytes()
    if data[:2] != b"MZ":
        return None
    import pefile  # imported lazily: only PE files need it

    try:
        pe = pefile.PE(data=data, fast_load=False)
    except pefile.PEFormatError:
        return None
    exports = []
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        for e in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            exports.append({"name": e.name.decode("ascii", "replace") if e.name else None,
                            "ordinal": e.ordinal, "rva": f"0x{e.address:x}"})
    descriptors = import_descriptor_dlls(pe)
    imports = {name: [] for name in descriptors}
    for imp in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        imports[imp.dll.decode("ascii", "replace")] = [
            i.name.decode("ascii", "replace") if i.name else f"ord{i.ordinal}" for i in imp.imports]
    ts = pe.FILE_HEADER.TimeDateStamp
    info = {
        "machine": f"0x{pe.FILE_HEADER.Machine:x}",
        "timestamp_utc": _dt.datetime.fromtimestamp(ts, _dt.timezone.utc).isoformat(),
        "image_base": f"0x{pe.OPTIONAL_HEADER.ImageBase:x}",
        "entry_rva": f"0x{pe.OPTIONAL_HEADER.AddressOfEntryPoint:x}",
        "sections": [{"name": s.Name.rstrip(b"\0").decode("ascii", "replace"), "va": f"0x{s.VirtualAddress:x}",
                      "virtual_size": s.Misc_VirtualSize, "raw_size": s.SizeOfRawData} for s in pe.sections],
        "exports": exports,
        "imports": imports,
        "import_descriptors": descriptors,
    }
    pe.close()
    return info


def kind_of(name: str, pe: dict | None) -> str:
    if pe is not None:
        return "pe"
    if name.upper().endswith(".FS"):
        return "archive"
    if name.lower().endswith((".ini", ".reg", ".bat", ".txt")):
        return "text"
    return "data"


def copy_one(src: Path, dest: Path) -> str:
    """Copy src to dest unless an identical copy exists; return the sha256. Never overwrites a differing file."""
    src_hash = sha256_file(src)
    if dest.exists():
        dest_hash = sha256_file(dest)
        if dest_hash != src_hash:
            raise RefusedOverwrite(f"{dest} exists with sha256 {dest_hash}, source {src} has {src_hash}")
        return src_hash
    dest.parent.mkdir(parents=True, exist_ok=True)
    tmp = dest.with_name(dest.name + ".partial")
    shutil.copy2(src, tmp)
    if sha256_file(tmp) != src_hash:
        tmp.unlink()
        raise OSError(f"copy of {src} does not match its source")
    tmp.replace(dest)
    os.chmod(dest, stat.S_IREAD)
    return src_hash


def preserve(src_dir: Path, dest_dir: Path, names: list[str], manifest_dir: Path, root: Path,
             provenance_dir: Path | None = None, argv: list[str] | None = None) -> list[dict]:
    records = []
    for name in names:
        src = src_dir / name
        if not src.is_file():
            raise FileNotFoundError(src)
        dest = dest_dir / name
        digest = copy_one(src, dest)
        pe = pe_info(dest)
        rec = {"path": rel(dest, root), "source_path": str(src), "size": dest.stat().st_size,
               "sha256": digest, "kind": kind_of(name, pe)}
        if pe is not None:
            rec["pe"] = pe
        records.append(rec)
    manifest_dir.mkdir(parents=True, exist_ok=True)
    (manifest_dir / "SHA256SUMS").write_text("".join(f"{r['sha256']}  {r['path']}\n" for r in records),
                                             encoding="utf-8", newline="\n")
    (manifest_dir / "file-inventory.json").write_text(json.dumps(records, indent=1) + "\n", encoding="utf-8")
    if provenance_dir is not None:
        outputs = {r["path"]: r["sha256"] for r in records}
        outputs.update(provenance.hash_paths([manifest_dir / "SHA256SUMS", manifest_dir / "file-inventory.json"], root))
        provenance.record_transformation(
            "preserve", argv if argv is not None else sys.argv, TOOL_VERSION,
            {r["source_path"]: r["sha256"] for r in records},
            {"src": str(src_dir), "dest": rel(dest_dir, root), "files": list(names)}, outputs, provenance_dir)
    return records


def read_sums(sums: Path) -> dict[str, str]:
    out = {}
    for line in sums.read_text(encoding="utf-8").splitlines():
        if line.strip():
            digest, path = line.split(None, 1)
            out[path.strip().lstrip("*")] = digest
    return out


def verify(src_dir: Path, dest_dir: Path, names: list[str], manifest_dir: Path, root: Path) -> tuple[bool, list[str]]:
    """Re-hash copies against SHA256SUMS and against their sources."""
    lines, ok = [], True
    sums_path = manifest_dir / "SHA256SUMS"
    if not sums_path.exists():
        return False, [f"missing {sums_path}"]
    sums = read_sums(sums_path)
    for name in names:
        dest, src = dest_dir / name, src_dir / name
        key = rel(dest, root) if dest.exists() else (dest_dir / name).as_posix()
        expected = sums.get(key)
        if expected is None or not dest.exists() or not src.exists():
            ok = False
            lines.append(f"MISSING {name}: manifest={'yes' if expected else 'no'} copy={dest.exists()} source={src.exists()}")
            continue
        got_dest, got_src = sha256_file(dest), sha256_file(src)
        good = got_dest == expected == got_src
        ok &= good
        lines.append(f"{'OK      ' if good else 'MISMATCH'} {name} {got_dest}" + ("" if good else f" (manifest {expected}, source {got_src})"))
    return ok, lines


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--src", type=Path, default=DEFAULT_SRC)
    ap.add_argument("--dest", type=Path, default=DEFAULT_DEST)
    ap.add_argument("--manifests", type=Path, default=MANIFESTS)
    ap.add_argument("--verify", action="store_true", help="re-hash copies and sources against SHA256SUMS")
    args = ap.parse_args(argv)
    for opt, path in (("--dest", args.dest), ("--manifests", args.manifests)):
        if not path.resolve().is_relative_to(ROOT.resolve()):
            print(f"refused: {opt} {path} is outside the repository {ROOT}", file=sys.stderr)
            return EXIT_REFUSED
    if args.verify:
        ok, lines = verify(args.src, args.dest, GAME_FILES, args.manifests, ROOT)
        print("\n".join(lines))
        print("verify:", "PASS" if ok else "FAIL")
        return EXIT_OK if ok else EXIT_MISMATCH
    try:
        records = preserve(args.src, args.dest, GAME_FILES, args.manifests, ROOT,
                           provenance_dir=args.manifests / "provenance",
                           argv=["tools/common/preserve.py", *(sys.argv[1:] if argv is None else argv)])
    except RefusedOverwrite as e:
        print(f"refused: {e}", file=sys.stderr)
        return EXIT_REFUSED
    for r in records:
        print(f"{r['sha256']}  {r['path']}  ({r['size']} bytes, {r['kind']})")
    return EXIT_OK


if __name__ == "__main__":
    sys.exit(main())
