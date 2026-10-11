"""Independent verifier tests for 00a.B.P1.T1 (R05): Git boundaries and the uv-managed environment.

Expected values come from the task text and ARCHITECTURE.md section 6, not from the files under test.
"""

import shutil
import subprocess
import sys
import tomllib
from importlib import metadata
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
VENV_PY = ROOT / ".venv" / "Scripts" / "python.exe"
GIT = shutil.which("git")

needs_git = pytest.mark.skipif(
    GIT is None or not (ROOT / ".git").exists(), reason="git repository not available"
)

# Paths the task text says must stay out of Git (one or more per required pattern).
MUST_IGNORE = [
    ".venv/Scripts/python.exe",
    "backups/originals/HERCULES.FS",
    "backups/originals/HERCULES.EXE",
    "working/game/HERCULES.EXE",
    "extracted/raw/0000_00000000.bin",
    "extracted/audio/0000_x.wav",
    "extracted/images/0000_x.png",
    "extracted/sprites/0000_x.png",
    "remastered/images_4k/a.png",
    "remastered/sprites_4k/hero/run/0001.png",
    "remastered/top.png",
    "analysis/ghidra/Hercules_RE.gpr",
    "analysis/ghidra/Hercules_RE.rep/idx/x",
    "analysis/traces/run1/buffers/0001.bin",
    "analysis/traces/x.bin",
    "reference/captures/level1.mp4",
    "reference/a.mp4",
]

# Code and text specifications that must remain trackable (master section 5, ARCHITECTURE.md section 6).
MUST_TRACK = [
    "pyproject.toml",
    "uv.lock",
    ".python-version",
    ".gitignore",
    ".gitattributes",
    "README.md",
    "tools/common/preserve.py",
    "tests/common/test_preserve.py",
    "tests/conftest.py",
    "manifests/SHA256SUMS",
    "manifests/file-inventory.json",
    "manifests/provenance/preserve.jsonl",
    "docs/canonical-edition.md",
    "extracted/meta/audio-coverage.json",
    "analysis/exports/functions.txt",
    "analysis/annotations/00a.json",
    "analysis/imhex/hfs.hexpat",
    "analysis/traces/run1/trace.jsonl",
    "reference/scenarios/s1.json",
    "remastered/manifest/manifest.json",
    ".plan/hercules-remaster/tools/plan-state.json",
    ".plan/hercules-remaster/evidence/00a/WORKLOG.md",
]

REQUIRED_DEPS = ["pillow", "numpy", "pefile", "pytest"]


def _check_ignore(path):
    return subprocess.run(
        [GIT, "check-ignore", "-q", path], cwd=ROOT, capture_output=True, text=True
    ).returncode


@needs_git
@pytest.mark.parametrize("path", MUST_IGNORE)
def test_required_paths_are_ignored(path):
    assert _check_ignore(path) == 0, f"{path} must be gitignored"


@needs_git
@pytest.mark.parametrize("path", MUST_TRACK)
def test_specifications_are_not_ignored(path):
    assert _check_ignore(path) == 1, f"{path} must stay trackable"


@needs_git
def test_verify_check_backups_originals_hercules_fs():
    # The task's verify check, verbatim.
    assert _check_ignore("backups/originals/HERCULES.FS") == 0


def test_venv_exists_and_is_uv_managed():
    assert VENV_PY.is_file(), f"missing {VENV_PY}"
    cfg = (ROOT / ".venv" / "pyvenv.cfg").read_text(encoding="utf-8")
    assert any(line.startswith("uv = ") for line in cfg.splitlines()), "venv was not created by uv"
    assert "include-system-site-packages = false" in cfg


def test_venv_imports_required_packages():
    out = subprocess.run(
        [str(VENV_PY), "-c", "import PIL, numpy, pefile, pytest; print('ok')"],
        cwd=ROOT, capture_output=True, text=True,
    )
    assert out.returncode == 0, out.stderr
    assert out.stdout.strip() == "ok"


def test_pyproject_declares_required_dependencies():
    data = tomllib.loads((ROOT / "pyproject.toml").read_text(encoding="utf-8"))
    deps = data["project"]["dependencies"]
    names = {d.split("=")[0].split(">")[0].split("<")[0].split("~")[0].strip().lower() for d in deps}
    for dep in REQUIRED_DEPS:
        assert dep in names, f"pyproject.toml does not declare {dep}"


def _lock_versions():
    lock = tomllib.loads((ROOT / "uv.lock").read_text(encoding="utf-8"))
    return {p["name"].lower(): p.get("version") for p in lock["package"]}


def test_uv_lock_pins_exact_versions():
    versions = _lock_versions()
    for dep in REQUIRED_DEPS:
        assert versions.get(dep), f"uv.lock has no exact version for {dep}"


def test_installed_versions_match_uv_lock():
    versions = _lock_versions()
    script = (
        "from importlib import metadata;"
        f"print(';'.join(n + '=' + metadata.version(n) for n in {REQUIRED_DEPS!r}))"
    )
    out = subprocess.run([str(VENV_PY), "-c", script], cwd=ROOT, capture_output=True, text=True)
    assert out.returncode == 0, out.stderr
    installed = dict(item.split("=", 1) for item in out.stdout.strip().split(";"))
    for dep in REQUIRED_DEPS:
        assert installed[dep] == versions[dep], f"{dep}: installed {installed[dep]} != locked {versions[dep]}"


def test_python_version_file_matches_venv():
    wanted = (ROOT / ".python-version").read_text(encoding="utf-8").strip()
    cfg = (ROOT / ".venv" / "pyvenv.cfg").read_text(encoding="utf-8")
    line = next(l for l in cfg.splitlines() if l.startswith("version_info"))
    actual = line.split("=", 1)[1].strip()
    assert actual == wanted or actual.startswith(wanted + "."), f".python-version {wanted} vs venv {actual}"


@needs_git
@pytest.mark.parametrize("path", ["x.png", "a/b.wav", "c.mp4", "d.bin", "HERCULES.FS"])
def test_gitattributes_marks_media_binary(path):
    out = subprocess.run(
        [GIT, "check-attr", "binary", "--", path], cwd=ROOT, capture_output=True, text=True
    )
    assert out.returncode == 0, out.stderr
    assert out.stdout.strip().endswith("binary: set"), out.stdout


def test_suite_runs_in_project_venv():
    # The module's commands run pytest through .venv; flag a run from another interpreter.
    if Path(sys.prefix).resolve() != (ROOT / ".venv").resolve():
        pytest.skip(f"pytest not run from the project venv ({sys.prefix})")
    for dep in REQUIRED_DEPS:
        assert metadata.version(dep)
