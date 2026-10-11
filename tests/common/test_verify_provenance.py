"""Independent verifier tests for 00a.B.P2.T1 (R04): tools/common/provenance.py.

Expected hashes are computed here with hashlib/json from the task text (master section 5), not with
the helpers under test.
"""

import datetime as dt
import hashlib
import json

import pytest

from tools.common import provenance as pv

ROOT = pv.ROOT
H_IN = hashlib.sha256(b"input bytes").hexdigest()
H_OUT = hashlib.sha256(b"output bytes").hexdigest()


def test_default_location_is_manifests_provenance():
    assert pv.PROVENANCE_DIR == ROOT / "manifests" / "provenance"


def test_record_line_has_command_versions_hashes_and_utc_timestamp(tmp_path):
    config = {"palette": "level1", "scale": 4, "nested": {"b": 2, "a": 1}}
    argv = ["tools/decode/esf.py", "--in", "extracted/raw/0001.bin"]
    pv.record_transformation("esf", argv, "0.3.1", {"extracted/raw/0001.bin": H_IN}, config,
                             {"extracted/audio/0001.wav": H_OUT}, provenance_dir=tmp_path)
    lines = (tmp_path / "esf.jsonl").read_text(encoding="utf-8").splitlines()
    assert len(lines) == 1
    rec = json.loads(lines[0])
    assert rec["tool"] == "esf"
    assert rec["argv"] == argv
    assert rec["tool_version"] == "0.3.1"
    assert rec["inputs"] == {"extracted/raw/0001.bin": H_IN}
    assert rec["outputs"] == {"extracted/audio/0001.wav": H_OUT}
    assert rec["config"] == config
    expected_cfg = hashlib.sha256(
        json.dumps(config, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode("utf-8")).hexdigest()
    assert rec["config_sha256"] == expected_cfg
    stamp = dt.datetime.fromisoformat(rec["timestamp_utc"])
    assert stamp.utcoffset() == dt.timedelta(0), "timestamp must be UTC"
    assert abs((dt.datetime.now(dt.timezone.utc) - stamp).total_seconds()) < 120


def test_non_utc_now_is_converted_to_utc(tmp_path):
    plus2 = dt.timezone(dt.timedelta(hours=2))
    rec = pv.record_transformation("t", [], "1", {}, {}, {}, provenance_dir=tmp_path,
                                   now=dt.datetime(2026, 10, 11, 14, 0, 0, tzinfo=plus2))
    assert rec["timestamp_utc"] == "2026-10-11T12:00:00+00:00"


def test_records_are_appended_never_rewritten(tmp_path):
    for i in range(3):
        pv.record_transformation("tool_a", [f"run{i}"], "1", {}, {"i": i}, {}, provenance_dir=tmp_path)
    pv.record_transformation("tool_b", ["other"], "1", {}, {}, {}, provenance_dir=tmp_path)
    a = (tmp_path / "tool_a.jsonl").read_text(encoding="utf-8").splitlines()
    assert [json.loads(l)["argv"] for l in a] == [["run0"], ["run1"], ["run2"]]
    assert len((tmp_path / "tool_b.jsonl").read_text(encoding="utf-8").splitlines()) == 1
    assert [r["config"]["i"] for r in pv.read_records("tool_a", tmp_path)] == [0, 1, 2]


def test_rejected_record_leaves_existing_log_untouched(tmp_path):
    pv.record_transformation("t", ["ok"], "1", {"a": H_IN}, {}, {"b": H_OUT}, provenance_dir=tmp_path)
    before = (tmp_path / "t.jsonl").read_bytes()
    for bad in ("", "abc", H_IN[:-1], H_IN + "0", H_IN.upper(), None, "g" * 64):
        with pytest.raises(ValueError):
            pv.record_transformation("t", ["bad"], "1", {"a": bad}, {}, {}, provenance_dir=tmp_path)
        with pytest.raises(ValueError):
            pv.record_transformation("t", ["bad"], "1", {}, {}, {"b": bad}, provenance_dir=tmp_path)
    assert (tmp_path / "t.jsonl").read_bytes() == before


@pytest.mark.parametrize("name", ["", "a/b", "a\\b", "c:x", "../up"])
def test_tool_name_cannot_escape_the_provenance_dir(tmp_path, name):
    with pytest.raises(ValueError):
        pv.record_transformation(name, [], "1", {}, {}, {}, provenance_dir=tmp_path)


def test_hash_paths_matches_hashlib(tmp_path):
    f = tmp_path / "d" / "x.bin"
    f.parent.mkdir()
    f.write_bytes(b"input bytes")
    assert pv.hash_paths([f], root=tmp_path) == {"d/x.bin": H_IN}
    assert pv.sha256_file(f) == H_IN


def test_read_records_of_unknown_tool_is_empty(tmp_path):
    assert pv.read_records("never_ran", tmp_path) == []
