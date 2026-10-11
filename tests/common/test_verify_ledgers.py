"""Independent verifier tests for 00a.B.P2.T2 (R03, R06): docs/evidence-ledger.md and docs/task-ledger.md.

Checks are made with code written here (table parsing, hashing, FS index counts), not with
tools/common/check_findings.py, except for the task's verify command itself.
"""

import re
import struct
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
LEDGER = ROOT / "docs" / "evidence-ledger.md"
TASK_LEDGER = ROOT / "docs" / "task-ledger.md"
SUMS = ROOT / "manifests" / "SHA256SUMS"
FS = ROOT / "backups" / "originals" / "HERCULES.FS"
LABELS = ("REPORTED", "VERIFIED", "INFERRED", "UNKNOWN", "DECIDED")
FINDING_HEADER = ["id", "finding", "label", "sha256", "module", "rva", "evidence", "next"]

needs_docs = pytest.mark.skipif(not (LEDGER.is_file() and TASK_LEDGER.is_file()), reason="ledgers not written")


def finding_rows():
    lines = LEDGER.read_text(encoding="utf-8").splitlines()
    rows = []
    for i, line in enumerate(lines):
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if cells == FINDING_HEADER:
            for row in lines[i + 2:]:
                if not row.startswith("|"):
                    break
                rows.append(dict(zip(FINDING_HEADER, [c.strip().strip("`") for c in row.strip().strip("|").split("|")])))
    return rows


@needs_docs
def test_task_verify_command_exits_0():
    out = subprocess.run([sys.executable, str(ROOT / "tools" / "common" / "check_findings.py"),
                          str(LEDGER), "--min-rows", "10"], cwd=ROOT, capture_output=True, text=True)
    assert out.returncode == 0, out.stdout + out.stderr


@needs_docs
def test_ledger_defines_all_labels_and_the_table_format():
    text = LEDGER.read_text(encoding="utf-8")
    for label in LABELS:
        assert f"`{label}`" in text, f"label {label} not defined"
    assert "| " + " | ".join(FINDING_HEADER) + " |" in text, "finding table with the ARCHITECTURE 4.1 header missing"


@needs_docs
def test_at_least_ten_seeded_findings_with_unique_ids_and_valid_labels():
    rows = finding_rows()
    assert len(rows) >= 10
    ids = [r["id"] for r in rows]
    assert len(ids) == len(set(ids))
    for r in rows:
        assert re.match(r"^(%s)\b" % "|".join(LABELS), r["label"]), r
        assert r["next"], f"{r['id']} has no next verification step"


@needs_docs
def test_verified_rows_link_existing_files_and_known_hashes():
    known = {line.split()[0] for line in SUMS.read_text(encoding="utf-8").splitlines() if line.strip()}
    for r in finding_rows():
        if not r["label"].startswith("VERIFIED"):
            continue
        assert re.fullmatch(r"[0-9a-f]{64}", r["sha256"]), r["id"]
        assert r["sha256"] in known, f"{r['id']}: hash is not a preserved original"
        paths = [p for p in re.split(r"[,;\s]+", r["evidence"]) if p and p != "-"]
        assert any((ROOT / p).is_file() for p in paths), f"{r['id']}: no existing evidence file in {r['evidence']!r}"


@needs_docs
@pytest.mark.skipif(not FS.is_file(), reason="preserved HERCULES.FS not present")
def test_fs_index_numbers_in_ledger_match_measurement():
    data = FS.read_bytes()
    recs = [struct.unpack_from("<III", data, 12 * i) for i in range(1024)]
    nonempty = sum(1 for r in recs if r[2])
    zero_size = sum(1 for r in recs if not r[2])
    zero_id = [i for i, r in enumerate(recs) if not r[0]]
    text = LEDGER.read_text(encoding="utf-8")
    assert (nonempty, zero_size, len(zero_id), zero_id[0]) == (820, 204, 203, 821)
    assert f"{nonempty} non-empty, {zero_size} zero-size, {len(zero_id)} zero-id slots from index {zero_id[0]}" in text
    esf = sum(1 for r in recs if r[2] and data[r[1]:r[1] + 4] == b"ESF\x03")
    tga = sum(1 for r in recs if r[2] and data[r[1]:r[1] + 4] == b"\x00\x01\x01\x00")
    assert f"{esf} payloads start with ESF" in text and f"{tga} with 00 01 01 00" in text
    assert f"{len(data):,} bytes" in text


@needs_docs
def test_task_ledger_names_all_four_master_states():
    text = TASK_LEDGER.read_text(encoding="utf-8")
    for state in ("ready", "in_progress", "blocked", "verified"):
        assert re.search(r"`%s`" % state, text), f"state {state} missing"


@needs_docs
def test_task_ledger_points_at_existing_plan_files():
    text = TASK_LEDGER.read_text(encoding="utf-8")
    plan = ROOT / ".plan" / "hercules-remaster"
    for name in re.findall(r"`([A-Z_]+\.md)`", text):
        assert (plan / name).is_file() or list(plan.glob(f"*/{name}")), f"{name} named in task ledger does not exist"
