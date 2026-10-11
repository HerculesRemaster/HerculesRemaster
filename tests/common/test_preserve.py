import hashlib
import os
import stat

import pytest

from tools.common import preserve


def make_tree(tmp_path):
    src = tmp_path / "src"
    src.mkdir()
    (src / "A.BIN").write_bytes(b"\x00\x01payload")
    (src / "notes.txt").write_text("hello\n", encoding="utf-8")
    root = tmp_path / "repo"
    return src, root / "backups" / "originals", root / "manifests", root


def test_copy_then_verify(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    names = ["A.BIN", "notes.txt"]
    recs = preserve.preserve(src, dest, names, man, root)
    assert [r["path"] for r in recs] == ["backups/originals/A.BIN", "backups/originals/notes.txt"]
    assert recs[0]["sha256"] == hashlib.sha256(b"\x00\x01payload").hexdigest()
    assert not os.access(dest / "A.BIN", os.W_OK), "copies must be read-only"
    sums = (man / "SHA256SUMS").read_text(encoding="utf-8")
    assert f"{recs[0]['sha256']}  backups/originals/A.BIN\n" in sums
    ok, lines = preserve.verify(src, dest, names, man, root)
    assert ok, lines


def test_rerun_is_idempotent(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    first = preserve.preserve(src, dest, ["A.BIN"], man, root)
    second = preserve.preserve(src, dest, ["A.BIN"], man, root)
    assert first == second


def test_refuses_to_overwrite_differing_copy(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    dest.mkdir(parents=True)
    (dest / "A.BIN").write_bytes(b"different")
    with pytest.raises(preserve.RefusedOverwrite):
        preserve.preserve(src, dest, ["A.BIN"], man, root)
    assert (dest / "A.BIN").read_bytes() == b"different", "existing copy must be left untouched"


def test_verify_detects_modified_copy(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    preserve.preserve(src, dest, ["A.BIN"], man, root)
    os.chmod(dest / "A.BIN", stat.S_IWRITE | stat.S_IREAD)
    (dest / "A.BIN").write_bytes(b"tampered")
    ok, lines = preserve.verify(src, dest, ["A.BIN"], man, root)
    assert not ok
    assert lines[0].startswith("MISMATCH")


def test_verify_detects_missing_copy(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    preserve.preserve(src, dest, ["A.BIN"], man, root)
    os.chmod(dest / "A.BIN", stat.S_IWRITE | stat.S_IREAD)
    (dest / "A.BIN").unlink()
    ok, lines = preserve.verify(src, dest, ["A.BIN"], man, root)
    assert not ok and lines[0].startswith("MISSING")


def test_missing_source_raises(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    with pytest.raises(FileNotFoundError):
        preserve.preserve(src, dest, ["NOPE.DLL"], man, root)


def test_non_pe_files_have_no_pe_block(tmp_path):
    src, dest, man, root = make_tree(tmp_path)
    (src / "FAKE.EXE").write_bytes(b"MZ" + b"\x00" * 62)  # MZ header without a valid PE
    recs = preserve.preserve(src, dest, ["FAKE.EXE", "notes.txt"], man, root)
    assert "pe" not in recs[0] and recs[0]["kind"] == "data"
    assert recs[1]["kind"] == "text"


def test_copy_run_records_provenance(tmp_path):
    from tools.common import provenance

    src, dest, man, root = make_tree(tmp_path)
    recs = preserve.preserve(src, dest, ["A.BIN"], man, root, provenance_dir=man / "provenance", argv=["preserve.py"])
    rows = provenance.read_records("preserve", man / "provenance")
    assert len(rows) == 1
    row = rows[0]
    assert row["argv"] == ["preserve.py"] and row["tool_version"] == preserve.TOOL_VERSION
    assert row["inputs"] == {str(src / "A.BIN"): recs[0]["sha256"]}
    assert row["outputs"]["backups/originals/A.BIN"] == recs[0]["sha256"]
    assert "manifests/SHA256SUMS" in row["outputs"] and "manifests/file-inventory.json" in row["outputs"]


def test_dest_outside_repository_is_refused_before_copying(tmp_path):
    outside = tmp_path / "elsewhere"
    assert preserve.main(["--dest", str(outside)]) == preserve.EXIT_REFUSED
    assert not outside.exists()
    assert preserve.main(["--manifests", str(tmp_path / "m"), "--verify"]) == preserve.EXIT_REFUSED


def test_inventory_lists_import_descriptors_without_functions():
    exe = preserve.DEFAULT_DEST / "HERCULES.EXE"
    if not exe.exists():
        pytest.skip("backups/originals/HERCULES.EXE not preserved yet")
    info = preserve.pe_info(exe)
    lower = [d.lower() for d in info["import_descriptors"]]
    assert "comdlg32.dll" in lower, info["import_descriptors"]
    assert len(info["imports"]) == len(info["import_descriptors"])
