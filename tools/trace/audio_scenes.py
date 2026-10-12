"""Bind runtime sound events to scenes (S5.01 / S5.02) -> extracted/meta/full-game/audio-scene-coverage.json.

Reads every analysis/traces/**/resource-usage.jsonl. A scene = one module/level stretch between module or level
loads (hook "module_load" / "level_load" events). Per scene it collects:
  sfx      hook "sfx_play" events (sound id, channel, DLL caller): count, first tick, callers
  music    hook "audio" events (gamehercules-40's slot hooks): AUD_LoadLevelSonix / AUD_LoadMusic / AUD_LoadSfx /
           AUD_StartMusic / AUD_StopMusic with their arguments and ticks (runs made before that hook lack them)
Scenes are keyed by run path + start tick; a per-level summary merges all scenes of a level id.

    audio_scenes.py
"""
from __future__ import annotations

import json
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAMES = {255: "TITLE", 1: "TRAINING", 2: "GAUNTLET", 3: "FOREST", 5: "THEBES", 6: "HYDRA", 7: "MEDUSA", 8: "CYCLOPS",
         9: "TITANS", 10: "TORMENT", 11: "VORTEX"}


def main() -> int:
    scenes = []
    for f in sorted((ROOT / "analysis" / "traces").rglob("resource-usage.jsonl")):
        cur = None
        for line in f.open(encoding="utf-8", errors="replace"):
            if not any(k in line for k in ('"module_load"', '"level_load"', '"sfx_play"', '"ev":"audio"')):
                continue
            try:
                e = json.loads(line)
            except ValueError:
                continue
            ev = e["ev"]
            if ev in ("module_load", "level_load"):
                key = (e.get("module") or (cur or {}).get("module"), e.get("level"))
                if cur is None or (cur["module"], cur["level"]) != key:
                    cur = {"run": f.parent.relative_to(ROOT).as_posix(), "module": key[0], "level": key[1],
                           "start_tick": e["tick"], "sfx": Counter(), "first": {}, "callers": defaultdict(set), "music": []}
                    scenes.append(cur)
            elif cur is None:
                continue
            elif ev == "sfx_play":
                s = e.get("sfx")
                cur["sfx"][s] += 1
                cur["first"].setdefault(s, e["tick"])
                cur["callers"][s].add(e.get("dll") or e.get("caller"))
            elif ev == "audio":
                cur["music"].append({k: e.get(k) for k in ("tick", "call", "a", "b", "dll")})
    per_level = defaultdict(lambda: {"scenes": 0, "sfx": Counter(), "music_calls": Counter(), "runs": set()})
    out_scenes = []
    for s in scenes:
        lv = per_level[NAMES.get(s["level"], str(s["level"]))]
        lv["scenes"] += 1
        lv["sfx"].update(s["sfx"])
        lv["runs"].add(s["run"])
        for m in s["music"]:
            lv["music_calls"][f"{m['call']}({m['a']}, {m['b']})"] += 1
        out_scenes.append({"run": s["run"], "module": s["module"], "level": s["level"], "start_tick": s["start_tick"],
                           "sfx": [{"id": k, "count": v, "first_tick": s["first"][k], "callers": sorted(x for x in s["callers"][k] if x)}
                                   for k, v in sorted(s["sfx"].items(), key=lambda x: (x[0] is None, x[0]))],
                           "music": s["music"]})
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/trace/audio_scenes.py. Music is CD audio and missing "
                   "(missing_source_media); music rows are the engine calls only.",
           "levels": {k: {"scenes": v["scenes"], "runs": len(v["runs"]), "distinct_sfx": len(v["sfx"]),
                          "sfx": dict(sorted(((str(a), b) for a, b in v["sfx"].items()))),
                          "music_calls": dict(v["music_calls"])} for k, v in sorted(per_level.items())},
           "scenes": out_scenes}
    p = ROOT / "extracted" / "meta" / "full-game" / "audio-scene-coverage.json"
    p.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    for k, v in doc["levels"].items():
        print(f"{k:10} scenes {v['scenes']:4d} runs {v['runs']:3d} distinct sfx {v['distinct_sfx']:4d} music calls {v['music_calls']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
