"""Full-game run metadata (S1.04) and replay pad scripts (S1.05).

run_meta.py meta <run dir> --scenario ID --entry E --difficulty D [--replay file] [--debug-level N]
    writes <run dir>/run.json: hashes of the game binaries/archive and of the proxy/ini/hook, entry
    method, requested and observed difficulty, observed identity (module/level loads, first state),
    initial lives/health, tracer env, frame<->tick origin, audio timestamp origin, CD audio note
run_meta.py pad <input.jsonl>
    prints the HERC_HOOK_PAD script ("from-to:hexbits,...") that replays the recorded pad bits
"""
import argparse
import hashlib
import json
import os
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GAME = ROOT / "working" / "game"


def sha(p: Path):
    if not p.exists():
        return None
    h = hashlib.sha256()
    with p.open("rb") as f:
        for c in iter(lambda: f.read(1 << 20), b""):
            h.update(c)
    return h.hexdigest()


def events(path: Path):
    for line in path.open(encoding="utf-8", errors="replace"):
        try:
            yield json.loads(line)
        except ValueError:
            continue


def pad_script(input_jsonl: Path) -> str:
    rows = [json.loads(l) for l in input_jsonl.open(encoding="utf-8") if l.strip()]
    parts = []
    for a, b in zip(rows, rows[1:] + [None]):
        if a["bits"]:
            end = (b["tick"] - 1) if b else a["tick"]
            parts.append(f"{a['tick']}-{end}:{a['bits']:04x}")
    return ",".join(parts)


def frame_index(frames_bin: Path):
    """First and last (tick, ms) of the frame dump {"FRM0", u32 tick, double ms, RGBQUAD[256], u8[76800]}."""
    size = 4 + 4 + 8 + 1024 + 320 * 240
    n = frames_bin.stat().st_size // size
    with frames_bin.open("rb") as f:
        first = struct.unpack("<4sId", f.read(16))
        f.seek((n - 1) * size)
        last = struct.unpack("<4sId", f.read(16))
    return {"file": "frames.bin", "frames": n, "first": {"tick": first[1], "ms": first[2]},
            "last": {"tick": last[1], "ms": last[2]}}


def meta(a):
    run = Path(a.run)
    ev = list(events(run / "resource-usage.jsonl"))
    loads = [e for e in ev if e["ev"] in ("module_load", "level_load")]
    states = [e for e in ev if e["ev"] == "state"]
    first_level = next((e for e in ev if e["ev"] == "level_load"), None)
    first_play = next((s for s in states if s.get("module") in ("ENG1", "ENG3")), None)
    sounds = [e for e in ev if e["ev"] in ("sfx_play", "esf_decode")]
    proxy_ini = GAME / "hercules_display.ini"
    doc = {
        "schema_version": 1,
        "scenario_id": a.scenario,
        "run_id": run.name,
        "entry_method": {"method": a.entry,
                         "debug_only": a.entry == "debug_level" or bool(os.environ.get("HERC_HOOK_START")),
                         "debug_start": int(os.environ["HERC_HOOK_START"]) if os.environ.get("HERC_HOOK_START") else None,
                         "debug_level": a.debug_level if a.entry == "debug_level" else None,
                         "replay_of": a.replay or None,
                         "pad_script": os.environ.get("HERC_HOOK_PAD", "")},
        "difficulty": {"requested": a.difficulty,
                       "mechanism": "F2 Game Play option preset in memory (HERC_HOOK_DIFFICULTY -> EXE 0xb9e864 before 0x4ac0)"
                                    if a.difficulty != "Config" else "player's saved Config (VirtualStore registry)",
                       "observed_at_level_load": first_level["difficulty"] if first_level else None},
        "hashes": {n: sha(GAME / n) for n in ("HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL", "HERCULES.FS")},
        "tracer": {"hercules_hook.dll": sha(ROOT / "runtime" / "hooks" / "bin" / "hercules_hook.dll"),
                   "ddraw.dll (proxy)": sha(GAME / "ddraw.dll"), "hercules_display.ini": sha(proxy_ini),
                   "env": {k: v for k, v in os.environ.items() if k.startswith(("HERC_HOOK_", "HERC_RENDERER", "HERC_DISPLAY"))}},
        "initial_state": {k: first_play.get(k) for k in ("level", "start", "lives", "health", "difficulty")} if first_play else None,
        "identity_events": [{k: e.get(k) for k in ("tick", "ev", "module", "engine", "path", "index", "id", "level", "start", "difficulty")}
                            for e in loads],
        "ticks": {"first": ev[0]["tick"] if ev else None, "last": max((e["tick"] for e in ev), default=None)},
        "frame_tick_index": frame_index(run / "frames.bin") if (run / "frames.bin").exists() else
                            "none (run without -Frames; trace ticks are hook ticks = RT_PollInput calls, one per game tick)",
        "audio_origin": {"first_sound_event": {k: sounds[0].get(k) for k in ("t", "tick", "ev")} if sounds else None,
                         "clock": "t = ms since hook start (QueryPerformanceCounter)"},
        "cd_audio": "absent: music is CD track 2 (docs/music-transport.md); no CD in this setup, so no music plays",
        "files": sorted(p.name for p in run.iterdir() if p.is_file()),
    }
    (run / "run.json").write_text(json.dumps(doc, indent=1), encoding="utf-8")
    print(f"{run.relative_to(ROOT)}: ticks {doc['ticks']}, loads {len(loads)}, difficulty observed "
          f"{doc['difficulty']['observed_at_level_load']}")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("meta"); p.add_argument("run"); p.add_argument("--scenario", required=True)
    p.add_argument("--entry", required=True); p.add_argument("--difficulty", required=True)
    p.add_argument("--replay", default=""); p.add_argument("--debug-level", type=int, default=-1)
    p = sub.add_parser("pad"); p.add_argument("input")
    a = ap.parse_args(argv)
    if a.cmd == "pad":
        print(pad_script(Path(a.input)))
    else:
        meta(a)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
