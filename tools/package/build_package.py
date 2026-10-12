"""Build the remaster package (reproducible zip) into dist/.

build_package.py --version 0.1.0 [--art working/game/art] [--no-build]

Contents of HerculesRemaster-<version>/ (never any original game file):
  ddraw.dll               runtime/display (MSVC /Brepro, so the same sources give the same bytes)
  hercules_display.ini    runtime/display template with renderer = gpu
  art/<family>/...        packed replacement art (optional, from atlas.py)
  install.ps1, uninstall.ps1, README.txt, NOTICES.txt
  package-manifest.json   version, sha256 of every file and of the build inputs, the replacement
                          manifest it was packed from, and the game edition it installs onto
The zip is deterministic: sorted entries, fixed timestamps and attributes.
"""
import argparse
import hashlib
import json
import shutil
import subprocess
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).parent
EPOCH = (2026, 1, 1, 0, 0, 0)
SOURCES = ["runtime/display/ddraw_proxy.c", "runtime/display/gpu_renderer.cpp", "runtime/display/cd_audio.cpp", "runtime/display/ddraw.def",
           "runtime/display/build.bat", "runtime/display/hercules_display.ini"]
# docs/canonical-edition.md: the edition the proxy's code checks (byte patterns) were taken from
EDITION = {"HERCULES.EXE": "b3ab3ee9e5c973bb245d8eed795e8e7065d7dfc371e412e602b2059af894a551",
           "ENG1.DLL": "01b78ae814892007bb6031b2470f10e09bdd912fdb8d30de9f4cfea7c638f757",
           "ENG3.DLL": "aefa1147deae55c4cb2290282f5142df00baa49a9cd59f671f7a3ce5b8b7fca7",
           "TITLE.DLL": "b17a37fb1bd5002c54cedb6dba1509bc1ca2d8483fa9724d8e4ade04fe79188b",
           "HERCULES.FS": "affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4"}


def sha(p: Path) -> str:
    h = hashlib.sha256()
    with p.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--version", required=True)
    ap.add_argument("--art", type=Path, help="packed art folder to include (e.g. working/game/art)")
    ap.add_argument("--no-build", action="store_true", help="use the existing runtime/display/bin/ddraw.dll")
    ap.add_argument("--portable", type=Path, metavar="GAME_DIR",
                    help="private portable folder instead of the add-on zip: copies the game files from GAME_DIR "
                         "(e.g. backups/originals), sets portable = on, no installer and no zip")
    a = ap.parse_args(argv)
    if not a.no_build:
        subprocess.run(["cmd", "/c", str(ROOT / "runtime" / "display" / "build.bat")], check=True,
                       stdout=subprocess.DEVNULL)
    if a.portable:
        return build_portable(a)
    name = f"HerculesRemaster-{a.version}"
    stage = ROOT / "dist" / name
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)
    shutil.copyfile(ROOT / "runtime" / "display" / "bin" / "ddraw.dll", stage / "ddraw.dll")
    ini = (ROOT / "runtime" / "display" / "hercules_display.ini").read_bytes().decode()
    ini = ini.replace("renderer = gdi ", "renderer = gpu ").replace("log = on ", "log = off")
    (stage / "hercules_display.ini").write_bytes(ini.encode())
    for f in ("install.ps1", "uninstall.ps1", "README.txt", "NOTICES.txt"):
        text = (HERE / f).read_text(encoding="utf-8").replace("{version}", a.version)
        (stage / f).write_text(text, encoding="utf-8", newline="\r\n")
    art_files = 0
    if a.art and a.art.exists():
        for f in sorted(a.art.rglob("*.png")):
            if "dump" in f.relative_to(a.art).parts:
                continue
            dst = stage / "art" / f.relative_to(a.art)
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(f, dst)
            art_files += 1
    rman = ROOT / "remastered" / "manifest.json"
    files = sorted(p for p in stage.rglob("*") if p.is_file())
    manifest = {
        "package": name, "version": a.version,
        "game_edition": EDITION,
        "build_inputs": {s: sha(ROOT / s) for s in SOURCES},
        "replacement_manifest": {"sha256": sha(rman) if rman.exists() else None,
                                 "entries": len(json.loads(rman.read_text())["entries"]) if rman.exists() else 0,
                                 "art_files": art_files},
        "files": {str(p.relative_to(stage)).replace("\\", "/"): {"bytes": p.stat().st_size, "sha256": sha(p)} for p in files},
    }
    (stage / "package-manifest.json").write_text(json.dumps(manifest, indent=1, sort_keys=True), encoding="utf-8")
    files = sorted(p for p in stage.rglob("*") if p.is_file())
    out = ROOT / "dist" / f"{name}.zip"
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for p in files:
            info = zipfile.ZipInfo(f"{name}/{p.relative_to(stage).as_posix()}", EPOCH)
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o644 << 16
            z.writestr(info, p.read_bytes())
    print(f"{out.relative_to(ROOT)}  {len(files)} files, {out.stat().st_size / 1024:.0f} KiB, sha256 {sha(out)}")
    return 0


# The game files a portable folder needs (canonical edition); the installer leftovers (HERC.REG, SETUP.BAT,
# unins000.dat) and the redistributor's ReadMe.txt stay out.
GAME_FILES = ["HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL", "HERCULES.FS", "HERCULES.HLP", "DISNEY.INI"]


def build_portable(a) -> int:
    """dist/HerculesRemaster-<version>-portable/: the game + proxy + art, runs from any folder without the
    registry key (proxy portable = on serves HKLM\\...\\Hercules\\1.00 from hercules_settings.ini).
    Contains Disney's game files: for the user's own machine only, never distributed (authorization-scope.md)."""
    for f, h in EDITION.items():
        if sha(a.portable / f) != h:
            raise SystemExit(f"{f} in {a.portable} is not the canonical edition")
    stage = ROOT / "dist" / f"HerculesRemaster-{a.version}-portable"
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)
    for f in GAME_FILES:
        shutil.copyfile(a.portable / f, stage / f)
    shutil.copyfile(ROOT / "runtime" / "display" / "bin" / "ddraw.dll", stage / "ddraw.dll")
    ini = (ROOT / "runtime" / "display" / "hercules_display.ini").read_bytes().decode()
    ini = ini.replace("renderer = gdi ", "renderer = gpu ").replace("log = on ", "log = off").replace("portable = off", "portable = on ")
    (stage / "hercules_display.ini").write_bytes(ini.encode())
    text = (HERE / "PORTABLE.txt").read_text(encoding="utf-8").replace("{version}", a.version)
    (stage / "README.txt").write_text(text, encoding="utf-8", newline="\r\n")
    art_files = 0
    if a.art and a.art.exists():
        for f in sorted(a.art.rglob("*.png")):
            if "dump" in f.relative_to(a.art).parts:
                continue
            dst = stage / "art" / f.relative_to(a.art)
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(f, dst)
            art_files += 1
    music = a.portable / "music"
    if music.is_dir():
        shutil.copytree(music, stage / "music")
    size = sum(p.stat().st_size for p in stage.rglob("*") if p.is_file())
    print(f"{stage.relative_to(ROOT)}  {len(GAME_FILES)} game files, {art_files} art files, {size / 2**20:.0f} MiB")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
