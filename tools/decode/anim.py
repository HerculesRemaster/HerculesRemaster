"""Animation data per sprite bank from runtime traces + decoded banks -> extracted/sprites/<bank>/anim.json.

Hercules' animations are code-driven: entity routines in ENG1 write the frame number
(obj+0x34) directly (e.g. 0x1e320 picks 0x40/0x42/0x44/0x45 at random), so there is no
sequence table in the bank. This tool collects what the game actually showed, from the hook's
"anim" events (ENG1 0x3cd0, logged when an object's frame changes; one tick = 33 ms):
  clips        per object: [(frame, ticks shown, x, y, flip, palette), ...], split at gaps > 15 ticks
  transitions  frame -> next frame counts (all objects of the bank)
  loops        strongly connected groups of the transition graph (cycles such as run loops)
  sequences    linear chains (mutually dominant successor/predecessor), labels in anim_labels.json
  frames       per frame: median/min/max ticks shown, pivot (= bank.json origin), boxes, dx/dy
    anim.py [runs ...]   (default: every analysis/traces/runs/*/resource-usage.jsonl)
"""
from __future__ import annotations

import json
import statistics
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SPRITES = ROOT / "extracted" / "sprites"
MANIFEST = ROOT / "manifests" / "archive-manifest.json"
GAP = 15


def bank_index_by_name() -> dict[str, int]:
    out = {}
    for e in json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]:
        n = (e.get("name_candidate") or "").upper()
        if n.endswith("\\ANIMPSX.BIN") and n.startswith("M:\\GRAFIX\\CHOP\\"):
            out["P:\\" + n[len("M:\\GRAFIX\\CHOP\\"):-len("\\ANIMPSX.BIN")]] = e["index"]
    return out


def sccs(graph: dict[int, set[int]]) -> list[list[int]]:
    index, low, stack, on, out, k = {}, {}, [], set(), [], [0]

    def visit(v):
        index[v] = low[v] = k[0]
        k[0] += 1
        stack.append(v)
        on.add(v)
        for w in graph.get(v, ()):
            if w not in index:
                visit(w)
                low[v] = min(low[v], low[w])
            elif w in on:
                low[v] = min(low[v], index[w])
        if low[v] == index[v]:
            comp = []
            while True:
                w = stack.pop()
                on.discard(w)
                comp.append(w)
                if w == v:
                    break
            if len(comp) > 1 or v in graph.get(v, ()):
                out.append(sorted(comp))

    sys.setrecursionlimit(10000)
    for v in list(graph):
        if v not in index:
            visit(v)
    return out


def chains(trans: Counter) -> list[list[int]]:
    """Linear sequences: a->b kept when b is a's most frequent successor and a is b's most
    frequent predecessor; chains are followed until that stops (cycles are cut once)."""
    succ, pred = defaultdict(Counter), defaultdict(Counter)
    for (a, b), n in trans.items():
        succ[a][b] += n
        pred[b][a] += n
    nxt = {a: c.most_common(1)[0][0] for a, c in succ.items()}
    prv = {b: c.most_common(1)[0][0] for b, c in pred.items()}
    link = {a: b for a, b in nxt.items() if prv.get(b) == a and a != b}
    has_in = set(link.values())
    out, used = [], set()
    starts = [a for a in link if a not in has_in] + [a for a in link if a in has_in]
    for a in starts:
        if a in used:
            continue
        ch = [a]
        used.add(a)
        while ch[-1] in link and link[ch[-1]] not in used:
            ch.append(link[ch[-1]])
            used.add(ch[-1])
        if len(ch) > 1:
            out.append(ch)
    return out


def main(argv: list[str]) -> int:
    runs = [Path(p) for p in argv[1:]] or sorted((ROOT / "analysis" / "traces" / "runs").glob("*/resource-usage.jsonl"))
    names = bank_index_by_name()
    events = defaultdict(list)          # (run, obj) -> events
    for r in runs:
        for line in r.open(encoding="utf-8"):
            if '"ev":"anim"' not in line:
                continue
            e = json.loads(line)
            events[(r.parent.name, e["obj"])].append(e)
    per_bank = defaultdict(lambda: {"clips": [], "durations": defaultdict(list), "transitions": Counter()})
    for (run, obj), evs in events.items():
        evs.sort(key=lambda e: e["t"])
        clip = []
        for a, b in zip(evs, evs[1:] + [None]):
            idx = names.get(a["bank_name"].upper())
            if idx is None:
                continue
            dur = (b["tick"] - a["tick"]) if b is not None else None
            gap = dur is None or dur > GAP or b["bank_name"] != a["bank_name"]
            bank = per_bank[idx]
            clip.append([a["frame"], None if gap else dur, a["x"], a["y"], a.get("flip", 0), a.get("pal", 255)])
            if not gap:
                bank["durations"][a["frame"]].append(dur)
                bank["transitions"][(a["frame"], b["frame"])] += 1
            if gap:
                if len(clip) > 1:
                    bank["clips"].append({"run": run, "obj": obj, "start_tick": a["tick"] - sum(c[1] or 0 for c in clip[:-1]),
                                          "frames": clip})
                clip = []
    labels = json.loads((Path(__file__).with_name("anim_labels.json")).read_text(encoding="utf-8"))
    written = 0
    for idx, b in per_bank.items():
        dirs = list(SPRITES.glob(f"{idx:04d}_*")) or list(SPRITES.glob(f"{idx:04d}"))
        if not dirs:
            continue
        bank_json = json.loads((dirs[0] / "bank.json").read_text(encoding="utf-8"))
        meta = {f["frame"]: f for f in bank_json["frames"]}
        graph = defaultdict(set)
        for (x, y) in b["transitions"]:
            graph[x].add(y)
        frames = {}
        for f, ds in sorted(b["durations"].items()):
            m = meta.get(f, {})
            frames[f] = {"ticks_median": statistics.median(ds), "ticks_min": min(ds), "ticks_max": max(ds), "seen": len(ds),
                         "pivot": m.get("origin"), "dx": m.get("dx"), "dy": m.get("dy"), "boxes": m.get("boxes"),
                         "png": m.get("png")}
        out = {"bank": bank_json["name"], "index": idx, "tick_ms": 33,
               "note": "observed at runtime (ENG1 0x3cd0 frame changes); sequences are code-driven",
               "loops": sccs(graph), "sequences": chains(b["transitions"]),
               "labels": [dict(l, ticks=[frames.get(f, {}).get("ticks_median") for f in l["frames"]])
                          for l in labels.get(str(idx), [])], "transitions": [[a, c, n] for (a, c), n in b["transitions"].most_common()],
               "frames": frames, "clips": b["clips"]}
        (dirs[0] / "anim.json").write_text(json.dumps(out), encoding="utf-8")
        written += 1
        print(f"{idx:04d} {bank_json['name']}: clips={len(b['clips'])} frames_seen={len(frames)} loops={len(out['loops'])}")
    print(f"anim.json written for {written} banks from {len(runs)} runs")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
