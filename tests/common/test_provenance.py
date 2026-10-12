import datetime as dt
import hashlib
import json

import pytest

from tools.common import provenance as pv

H = hashlib.sha256(b"x").hexdigest()


def test_record_has_required_fields_and_is_appended(tmp_path):
    now = dt.datetime(2026, 10, 11, 12, 0, 0, 123456, tzinfo=dt.timezone.utc)
    rec = pv.record_transformation("fs_extract", ["fs_extract.py", "a.fs"], "1.0", {"a.fs": H},
                                   {"out": "raw", "level": 1}, {"raw/0000.bin": H}, provenance_dir=tmp_path, now=now)
    for key in ("tool", "argv", "tool_version", "python", "inputs", "config", "config_sha256", "outputs", "timestamp_utc"):
        assert key in rec
    assert rec["timestamp_utc"] == "2026-10-11T12:00:00+00:00"
    pv.record_transformation("fs_extract", ["again"], "1.0", {}, {}, {}, provenance_dir=tmp_path, now=now)
    lines = (tmp_path / "fs_extract.jsonl").read_text(encoding="utf-8").splitlines()
    assert len(lines) == 2 and json.loads(lines[0])["argv"] == ["fs_extract.py", "a.fs"]
    assert pv.read_records("fs_extract", tmp_path)[1]["argv"] == ["again"]


def test_config_hash_is_order_independent():
    assert pv.config_hash({"a": 1, "b": [1, 2]}) == pv.config_hash({"b": [1, 2], "a": 1})
    assert pv.config_hash({"a": 1}) != pv.config_hash({"a": 2})


def test_rejects_bad_hashes_and_tool_names(tmp_path):
    with pytest.raises(ValueError):
        pv.record_transformation("t", [], "1", {"a": "nothex"}, {}, {}, provenance_dir=tmp_path)
    with pytest.raises(ValueError):
        pv.record_transformation("t", [], "1", {}, {}, {"o": H.upper()}, provenance_dir=tmp_path)
    with pytest.raises(ValueError):
        pv.record_transformation("../escape", [], "1", {}, {}, {}, provenance_dir=tmp_path)


def test_hash_paths_uses_repo_relative_posix_paths(tmp_path):
    f = tmp_path / "sub" / "f.bin"
    f.parent.mkdir()
    f.write_bytes(b"x")
    assert pv.hash_paths([f], root=tmp_path) == {"sub/f.bin": H}
