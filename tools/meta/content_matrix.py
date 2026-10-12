"""Release content matrix, one row per level (full-game S7.05) -> docs/release/content-matrix.md.

    content_matrix.py
Every cell is read from a project file (no hand-entered status):
  map        extracted/full-game/levels/<DIR>/level.json exists (ENG1 level_decode / ENG3 eng3_decode)
  banks      animation-coverage.json: banks of the level (static lists) and how many were drawn at runtime
  audio      audio-banks.json Sonix banks; music is Red Book CD audio = missing_source_media (Q-01)
  spec       docs/full-game/levels/<Lxx>.md headings
  art keys   extracted/meta/full-game/art-inventory/<DIR>.json (keys, seen at runtime)
  art packed inventory keys of the level with a packed replacement file in working/game/art/<family>/
             (tools/hires/jobs.py pack; sprite, level, ui, screen families)
  perf       analysis/runtime/perf/eng?_<DIR>_region.csv p95 at 9x over the in-level frames = every present after
             the first reset of the game tick column (game state +0x38 restarts at 0 when the level module
             loads; the runs enter one level through cheat_select from the title)
  4K art     share of the level's art keys with a 4K file in working/game/art: redrawn (sprite, level, ui, screen
             folders) and the Scale3x² fallback (smooth/, tools/hires/smooth.py)
  4K screen  analysis/runtime/perf/<id>.csv p95 render cost (tools/perf/run_perf.ps1 -Fullscreen: borderless 3840x2160,
             report.py --from-frame 1700; VORTEX from its password run, from frame 2000)
  music      CD track 2 emulated by the proxy (runtime/display/cd_audio.cpp) when working/game/music/Track02.* exists
  gameplay   layout, collision, enemies, scripts, camera, sound, checkpoints, scoring and completion run in the
             original ENG1/ENG3 code (hybrid runtime, Q-03); nothing of it is reimplemented
  runtime    analysis/runtime/sweep/<id>/gpu_*.png + gpu_*.txt naming the level's map: loaded and drawn by the GPU
             renderer with the art (level_sweep.ps1, or the password run for VORTEX)
  play check the level's 5-remaster.md RM.05 [HUMAN] box
"""
import csv
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
META = ROOT / "extracted/meta/full-game"
LEVELS = [("L01", "TRAINING", 1, "ENG1"), ("L02", "GAUNTLET", 2, "ENG3"), ("L03", "FOREST", 3, "ENG1"), ("L04", "THEBES", 5, "ENG1"),
          ("L05", "HYDRA", 6, "ENG1"), ("L06", "MEDUSA", 7, "ENG1"), ("L07", "CYCLOPS", 8, "ENG3"), ("L08", "TITANS", 9, "ENG1"),
          ("L09", "TORMENT", 10, "ENG3"), ("L10", "VORTEX", 11, "ENG1")]


def pct(v, p):
    v = sorted(v)
    return v[min(len(v) - 1, int(round(p / 100 * (len(v) - 1))))] if v else None


def main():
    cov = json.loads((META / "animation-coverage.json").read_text(encoding="utf-8"))
    audio = json.loads((META / "audio-banks.json").read_text(encoding="utf-8"))["levels"]
    plan = ROOT / ".plan/full-game-remaster/levels"
    art = ROOT / "working/game/art"
    redrawn = {f.name.split("_")[0] for d in ("sprite", "level", "ui", "screen") for f in (art / d).glob("*.png")} if art.exists() else set()
    smooth = {f.name.split("_")[0] for f in (art / "smooth").glob("*.png")} if (art / "smooth").exists() else set()
    track = next((f for f in (ROOT / "working/game/music").glob("Track02.*")), None) if (ROOT / "working/game/music").exists() else None
    music = "CD music: emulated from " + track.name if track else "CD music: needs music/Track02.* (disc rip, Q-01)"
    rows = []
    for slug, name, lid, eng in LEVELS:
        lv = ROOT / "extracted/full-game/levels" / name / "level.json"
        banks = [b for b in cov["banks"] if name in b.get("levels", [])]
        drawn = sum(1 for b in banks if (b.get("runtime") or {}).get("frames_shown"))
        a = audio.get(name, {})
        snd = f"{a['music']['dir']} + {len(a.get('sfx', []))} sfx" if a else "-"
        spec = ROOT / "docs/full-game/levels" / f"{slug}.md"
        heads = len(re.findall(r"^## ", spec.read_text(encoding="utf-8"), re.M)) if spec.exists() else 0
        inv = META / "art-inventory" / f"{name}.json"
        keys = json.loads(inv.read_text(encoding="utf-8")) if inv.exists() else None
        art = ROOT / "working/game/art"
        have = {f.name for fam in ("sprite", "level", "ui", "screen") if (art / fam).exists() for f in (art / fam).iterdir()}
        packed = sum(1 for it in keys["items"] if it["file"] in have) if keys else 0
        perf = None
        for f in (ROOT / "analysis/runtime/perf").glob(f"eng?_{name}_region.csv"):
            allr = list(csv.DictReader(f.open()))
            gap = next((i - 1 for i in range(1, len(allr)) if int(allr[i]["tick"]) < int(allr[i - 1]["tick"])), len(allr))
            r = allr[gap + 1:]
            perf = pct([float(x["cpu_ms"]) + max(float(x["gpu_ms"]), 0) for x in r], 95)
        k4 = "-"
        if keys:
            ks = [i["key"] for i in keys["items"]]
            k4 = f"{sum(k in redrawn for k in ks)} redrawn + {sum(k in smooth and k not in redrawn for k in ks)} smooth / {len(ks)}"
        fs = ROOT / "analysis/runtime/perf" / f"{lid}.csv"
        p4 = "not measured"
        if fs.exists():
            r = [x for x in csv.DictReader(fs.open()) if int(x["frame"]) >= (2000 if lid == 11 else 1700)]
            if r:
                p4 = f"{pct([float(x['cpu_ms']) + max(float(x['gpu_ms']), 0) for x in r], 95):.1f} ms"
        sw = ROOT / "analysis/runtime/sweep" / str(lid)
        seen = [t for t in sw.glob("gpu_*.txt") if not t.name.endswith("_ot.txt") and name.lower() in t.read_text(errors="ignore").lower()] if sw.exists() else []
        rt = f"loaded + drawn ({len(seen)} shots)" if seen else "not run"
        rm = next((d for d in plan.iterdir() if d.name.startswith(slug)), None)
        play = "pending"
        if rm and (rm / "5-remaster.md").exists():
            m = re.search(rf"- \[(x| )\] `{slug}\.RM\.05`", (rm / "5-remaster.md").read_text(encoding="utf-8"))
            play = "passed" if m and m.group(1) == "x" else "pending [HUMAN]"
        rows.append([f"{slug} {name} ({lid})", eng, "decoded" if lv.exists() else "open",
                     f"{len(banks)} ({drawn} drawn)", snd + "; " + music,
                     f"{heads} sections" if heads else "not written",
                     f"{keys['keys']} ({keys['seen_at_runtime']} seen)" if keys else "-", f"{packed} ({100 * packed // max(keys['keys'], 1)} %)" if keys else "0",
                     f"{perf:.1f} ms" if perf is not None else "not measured", k4, p4, "original code", rt, play])
    hdr = ["level", "engine", "map", "sprite banks", "audio", "spec (docs/full-game/levels)", "art keys", "art packed",
           "perf p95 @9x", "4K art", "4K screen p95", "gameplay", "runtime", "user play check"]
    out = ["# Content matrix", "", "Generated by `tools/meta/content_matrix.py` from the project files (see its docstring for the "
           "source of each cell). The Q-05 budget is 16.67 ms p95.", "",
           "| " + " | ".join(hdr) + " |", "|" + "---|" * len(hdr)]
    out += ["| " + " | ".join(r) + " |" for r in rows]
    dst = ROOT / "docs/release/content-matrix.md"
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text("\n".join(out) + "\n", encoding="utf-8")
    print("\n".join(out[4:]))


if __name__ == "__main__":
    main()
