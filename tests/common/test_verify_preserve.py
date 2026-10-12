"""Independent verifier tests for 00a.B.P1.T2 (R01): tools/common/preserve.py.

Expected hashes come from hashlib on the bytes (and from the task text); PE facts come from a
struct-based header parser written here, not from pefile or preserve.py.
"""

import datetime as dt
import hashlib
import json
import os
import stat
import struct
import subprocess
import sys
from pathlib import Path

import pytest

from tools.common import preserve

ROOT = Path(__file__).resolve().parents[2]
ORIGINALS = ROOT / "backups" / "originals"
SUMS = ROOT / "manifests" / "SHA256SUMS"
INVENTORY = ROOT / "manifests" / "file-inventory.json"
SCRIPT = ROOT / "tools" / "common" / "preserve.py"

GAME_FILES = [
    "HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL", "HERCULES.FS", "HERCULES.HLP",
    "DISNEY.INI", "HERC.REG", "SETUP.BAT", "ReadMe.txt", "unins000.dat",
]
TASK_HASHES = {
    "HERCULES.FS": "affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4",
    "HERCULES.EXE": "b3ab3ee9e5c973bb245d8eed795e8e7065d7dfc371e412e602b2059af894a551",
}
FILE_ATTRIBUTE_READONLY = 0x1

have_originals = pytest.mark.skipif(
    not (ORIGINALS.is_dir() and SUMS.is_file() and INVENTORY.is_file()),
    reason="preserved originals or manifests not present",
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def is_read_only(path: Path) -> bool:
    st = path.stat()
    attrs = getattr(st, "st_file_attributes", None)
    if attrs is not None:
        return bool(attrs & FILE_ATTRIBUTE_READONLY)
    return not (st.st_mode & stat.S_IWUSR)


def make_writable(path: Path):
    os.chmod(path, stat.S_IREAD | stat.S_IWRITE)


# ---------------------------------------------------------------- independent PE parser


def parse_pe(data: bytes) -> dict:
    lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    assert data[lfanew:lfanew + 4] == b"PE\0\0"
    machine, nsec, ts, _, _, opt_size, _ = struct.unpack_from("<HHIIIHH", data, lfanew + 4)
    opt = lfanew + 24
    magic = struct.unpack_from("<H", data, opt)[0]
    assert magic == 0x10B, "PE32 expected"
    entry = struct.unpack_from("<I", data, opt + 16)[0]
    image_base = struct.unpack_from("<I", data, opt + 28)[0]
    dirs = opt + 96
    exp_rva, _ = struct.unpack_from("<II", data, dirs)
    imp_rva, _ = struct.unpack_from("<II", data, dirs + 8)
    sections = []
    for i in range(nsec):
        off = opt + opt_size + 40 * i
        name = data[off:off + 8].rstrip(b"\0").decode("ascii", "replace")
        vsize, va, raw_size, raw_ptr = struct.unpack_from("<IIII", data, off + 8)
        sections.append({"name": name, "va": va, "virtual_size": vsize, "raw_size": raw_size, "raw_ptr": raw_ptr})

    def off_of(rva):
        for s in sections:
            if s["va"] <= rva < s["va"] + max(s["virtual_size"], s["raw_size"]):
                return rva - s["va"] + s["raw_ptr"]
        raise ValueError(hex(rva))

    def cstr(rva):
        o = off_of(rva)
        return data[o:data.index(b"\0", o)].decode("ascii", "replace")

    exports = set()
    if exp_rva:
        e = off_of(exp_rva)
        (_, _, _, _, _, base, nfunc, nnames, a_funcs, a_names, a_ords) = struct.unpack_from("<IIHHIIIIIII", data, e)
        for i in range(nnames):
            name_rva = struct.unpack_from("<I", data, off_of(a_names) + 4 * i)[0]
            idx = struct.unpack_from("<H", data, off_of(a_ords) + 2 * i)[0]
            func_rva = struct.unpack_from("<I", data, off_of(a_funcs) + 4 * idx)[0]
            exports.add((cstr(name_rva), base + idx, func_rva))
    dlls = {}  # dll name -> number of thunks (0 = descriptor with an empty ILT/IAT)
    descriptors = []  # dll names in file order, one per IMAGE_IMPORT_DESCRIPTOR
    if imp_rva:
        o = off_of(imp_rva)
        while True:
            desc = struct.unpack_from("<IIIII", data, o)
            if desc == (0, 0, 0, 0, 0):
                break
            thunks = desc[0] or desc[4]
            n = 0
            while struct.unpack_from("<I", data, off_of(thunks) + 4 * n)[0]:
                n += 1
            dlls[cstr(desc[3])] = dlls.get(cstr(desc[3]), 0) + n
            descriptors.append(cstr(desc[3]))
            o += 20
    return {"machine": machine, "timestamp": ts, "entry": entry, "image_base": image_base,
            "sections": sections, "exports": exports, "dlls": dlls, "descriptors": descriptors}


# ---------------------------------------------------------------- integration on the real copies


@have_originals
def test_task_hashes_of_the_real_copies():
    for name, digest in TASK_HASHES.items():
        assert sha((ORIGINALS / name).read_bytes()) == digest


@have_originals
def test_sha256sums_lists_every_game_file_with_correct_hash():
    lines = SUMS.read_text(encoding="utf-8").splitlines()
    assert len(lines) == len(GAME_FILES)
    seen = {}
    for line in lines:
        digest, path = line.split("  ", 1)  # GNU text-mode separator: two spaces
        assert len(digest) == 64 and all(c in "0123456789abcdef" for c in digest)
        assert path.startswith("backups/originals/"), "paths must be repository-relative"
        seen[path.rsplit("/", 1)[1]] = digest
        assert sha((ROOT / path).read_bytes()) == digest, path
    assert sorted(seen) == sorted(GAME_FILES)


@have_originals
def test_copies_equal_sources_byte_for_byte_and_are_read_only():
    src_dir = ROOT.parent
    for name in GAME_FILES:
        copy = ORIGINALS / name
        assert is_read_only(copy), f"{copy} is not read-only"
        if (src_dir / name).is_file():
            assert copy.read_bytes() == (src_dir / name).read_bytes(), name
    assert not list(ORIGINALS.glob("*.partial")), "leftover partial copies"


@have_originals
def test_inventory_matches_bytes_and_sums():
    inv = json.loads(INVENTORY.read_text(encoding="utf-8"))
    sums = {p: d for d, p in (l.split("  ", 1) for l in SUMS.read_text(encoding="utf-8").splitlines())}
    assert [r["path"] for r in inv] == [f"backups/originals/{n}" for n in GAME_FILES]
    for r in inv:
        data = (ROOT / r["path"]).read_bytes()
        assert r["size"] == len(data), r["path"]
        assert r["sha256"] == sha(data) == sums[r["path"]], r["path"]
        assert Path(r["source_path"]).name == Path(r["path"]).name
        is_pe = data[:2] == b"MZ" and r["path"].upper().endswith((".EXE", ".DLL"))
        assert ("pe" in r) == is_pe, r["path"]
        assert (r["kind"] == "pe") == is_pe


@have_originals
def test_inventory_pe_fields_match_independent_header_parse():
    inv = json.loads(INVENTORY.read_text(encoding="utf-8"))
    pe_rows = [r for r in inv if "pe" in r]
    assert sorted(Path(r["path"]).name for r in pe_rows) == ["ENG1.DLL", "ENG3.DLL", "HERCULES.EXE", "TITLE.DLL"]
    for r in pe_rows:
        got = r["pe"]
        want = parse_pe((ROOT / r["path"]).read_bytes())
        assert int(got["machine"], 16) == want["machine"] == 0x14C
        assert got["timestamp_utc"] == dt.datetime.fromtimestamp(want["timestamp"], dt.timezone.utc).isoformat()
        assert int(got["image_base"], 16) == want["image_base"]
        expected_base = 0x400000 if r["path"].endswith(".EXE") else 0x10000000
        assert want["image_base"] == expected_base
        assert int(got["entry_rva"], 16) == want["entry"]
        assert [(s["name"], int(s["va"], 16), s["virtual_size"], s["raw_size"]) for s in got["sections"]] == [
            (s["name"], s["va"], s["virtual_size"], s["raw_size"]) for s in want["sections"]]
        got_exports = {(e["name"], e["ordinal"], int(e["rva"], 16)) for e in got["exports"] if e["name"]}
        assert got_exports == want["exports"], r["path"]
        # Every import descriptor in the file is recorded, including ones with an empty thunk list
        # (pefile alone drops those with a "Damaged Import Table" warning).
        assert got["import_descriptors"] == want["descriptors"], r["path"]
        assert {d.lower(): len(f) for d, f in got["imports"].items()} == {
            d.lower(): n for d, n in want["dlls"].items()}, r["path"]


@have_originals
def test_hercules_exe_empty_comdlg32_descriptor_is_measured():
    # VERIFIED by the struct parser (verifier-01, 2026-10-11): HERCULES.EXE carries an import descriptor
    # for comdlg32.dll whose ILT and IAT are empty; the inventory records it with no functions.
    want = parse_pe((ORIGINALS / "HERCULES.EXE").read_bytes())
    assert want["dlls"].get("comdlg32.dll") == 0
    assert len(want["descriptors"]) == 11
    inv = json.loads(INVENTORY.read_text(encoding="utf-8"))
    exe = next(r for r in inv if r["path"] == "backups/originals/HERCULES.EXE")["pe"]
    assert exe["imports"].get("comdlg32.dll") == []
    assert "comdlg32.dll" in exe["import_descriptors"]


@have_originals
def test_cli_verify_exits_0_and_reports_task_hashes():
    out = subprocess.run([sys.executable, str(SCRIPT), "--verify"], cwd=ROOT, capture_output=True, text=True)
    assert out.returncode == 0, out.stdout + out.stderr
    for name, digest in TASK_HASHES.items():
        assert f"{name} {digest}" in out.stdout


# ---------------------------------------------------------------- unit cases on temporary trees


def fake_game(tmp_path, names=("A.EXE", "B.FS", "c.txt")):
    src = tmp_path / "src"
    src.mkdir()
    for i, n in enumerate(names):
        (src / n).write_bytes(f"payload-{i}-{n}".encode() * (i + 1))
    root = tmp_path / "repo"
    dest = root / "backups" / "originals"
    man = root / "manifests"
    return src, dest, man, root, list(names)


@pytest.fixture
def cli(monkeypatch, tmp_path):
    src, dest, man, root, names = fake_game(tmp_path)
    monkeypatch.setattr(preserve, "ROOT", root)
    monkeypatch.setattr(preserve, "GAME_FILES", names)
    args = ["--src", str(src), "--dest", str(dest), "--manifests", str(man)]
    return src, dest, man, root, names, args


def test_main_copy_then_verify_exit_0_with_independent_hashes(cli):
    src, dest, man, root, names, args = cli
    assert preserve.main(args) == 0
    sums = (man / "SHA256SUMS").read_text(encoding="utf-8")
    for n in names:
        digest = sha((src / n).read_bytes())
        assert f"{digest}  backups/originals/{n}\n" in sums
        assert sha((dest / n).read_bytes()) == digest
        assert is_read_only(dest / n)
    assert preserve.main(args + ["--verify"]) == 0


def test_main_refuses_differing_copy_exit_2_and_overwrites_nothing(cli):
    src, dest, man, root, names, args = cli
    dest.mkdir(parents=True)
    (dest / names[1]).write_bytes(b"someone else's bytes")
    assert preserve.main(args) == 2
    assert (dest / names[1]).read_bytes() == b"someone else's bytes"
    assert not (man / "SHA256SUMS").exists(), "manifests must not be written after a refusal"


def test_rerun_keeps_copies_untouched_and_read_only(cli):
    src, dest, man, root, names, args = cli
    assert preserve.main(args) == 0
    before = {n: (dest / n).stat().st_mtime_ns for n in names}
    sums_before = (man / "SHA256SUMS").read_bytes()
    assert preserve.main(args) == 0
    assert {n: (dest / n).stat().st_mtime_ns for n in names} == before
    assert (man / "SHA256SUMS").read_bytes() == sums_before
    assert all(is_read_only(dest / n) for n in names)


def test_sources_are_never_modified(cli):
    src, dest, man, root, names, args = cli
    snap = {n: ((src / n).read_bytes(), (src / n).stat().st_mtime_ns) for n in names}
    assert preserve.main(args) == 0
    assert preserve.main(args + ["--verify"]) == 0
    assert {n: ((src / n).read_bytes(), (src / n).stat().st_mtime_ns) for n in names} == snap


def test_verify_exit_1_when_source_changes(cli, capsys):
    src, dest, man, root, names, args = cli
    assert preserve.main(args) == 0
    (src / names[0]).write_bytes(b"changed after preservation")
    assert preserve.main(args + ["--verify"]) == 1
    out = capsys.readouterr().out
    assert f"MISMATCH {names[0]}" in out and "verify: FAIL" in out


def test_verify_exit_1_when_manifest_is_tampered(cli):
    src, dest, man, root, names, args = cli
    assert preserve.main(args) == 0
    sums = man / "SHA256SUMS"
    text = sums.read_text(encoding="utf-8")
    first = text.split("  ", 1)[0]
    sums.write_text(text.replace(first, "0" * 64, 1), encoding="utf-8", newline="\n")
    assert preserve.main(args + ["--verify"]) == 1


def test_verify_exit_1_without_manifest(cli):
    src, dest, man, root, names, args = cli
    assert preserve.main(args + ["--verify"]) == 1


def test_verify_exit_1_when_copy_tampered(cli):
    src, dest, man, root, names, args = cli
    assert preserve.main(args) == 0
    make_writable(dest / names[2])
    (dest / names[2]).write_bytes(b"tampered")
    assert preserve.main(args + ["--verify"]) == 1


def test_cli_subprocess_refusal_exit_2(tmp_path):
    # Real script, real GAME_FILES order: a differing first copy must be refused with exit 2.
    src = tmp_path / "src"
    src.mkdir()
    for n in GAME_FILES:
        (src / n).write_bytes(n.encode())
    dest = tmp_path / "dest"
    dest.mkdir()
    (dest / GAME_FILES[0]).write_bytes(b"differs")
    man = tmp_path / "man"
    out = subprocess.run(
        [sys.executable, str(SCRIPT), "--src", str(src), "--dest", str(dest), "--manifests", str(man)],
        cwd=ROOT, capture_output=True, text=True,
    )
    assert out.returncode == 2, out.stdout + out.stderr
    assert "refused" in out.stderr
    assert (dest / GAME_FILES[0]).read_bytes() == b"differs"
    assert not man.exists() or not any(man.iterdir())
