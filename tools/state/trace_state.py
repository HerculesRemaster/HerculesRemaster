"""Per-tick hero state during the attract demo (hook event "hero", ENG1 globals) -> CSV + summary.

    trace_state.py [run_dir]   default: runs tools/trace/run_trace.ps1 -Hero (60 s) into
                               analysis/traces/runs/hero_state/ first
Writes analysis/traces/hero_state.csv (tick, x, y, z in world units, vx, vy, up, facing, state,
frame) and prints the states seen, the jump arcs (take-off y, apex, ticks to apex) and speeds.
"""
from __future__ import annotations

import csv
import json
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main(argv: list[str]) -> int:
    run = Path(argv[1]) if len(argv) > 1 else ROOT / "analysis" / "traces" / "runs" / "hero_state"
    if len(argv) <= 1:
        subprocess.run(["pwsh", "-NoProfile", "-File", str(ROOT / "tools" / "trace" / "run_trace.ps1"),
                        "-Seconds", "60", "-Name", run.name, "-Pairs", "0", "-Hero"], check=True, capture_output=True)
    rows = []
    for line in (run / "resource-usage.jsonl").open(encoding="utf-8"):
        if '"ev":"hero"' in line:
            e = json.loads(line)
            rows.append([e["tick"], e["x"] / 65536, e["y"] / 65536, e["z"] / 65536, e["vx"] / 65536, e["vy"] / 65536,
                         e["up"], e["facing"], e["state"], e["frame"]])
    out = ROOT / "analysis" / "traces" / "hero_state.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["tick", "x", "y", "z", "vx", "vy", "up", "facing", "state", "frame"])
        w.writerows(rows)
    states = Counter(r[8] for r in rows)
    arcs, cur = [], None
    for prev, r in zip(rows, rows[1:]):
        if r[6] and not prev[6]:
            cur = {"tick": r[0], "y0": prev[2], "vy0": r[5], "apex": r[2], "ticks": 0}
        if cur:
            cur["ticks"] += 1
            cur["apex"] = min(cur["apex"], r[2])
            if not r[6]:
                cur["rise"] = round(cur["y0"] - cur["apex"], 1)
                arcs.append(cur)
                cur = None
    print(f"ticks={len(rows)} states={dict(states.most_common())}")
    print(f"max |vx|={max(abs(r[4]) for r in rows):.3f} units/tick, max vy={max(r[5] for r in rows):.3f}")
    for a in arcs[:8]:
        print(f"  jump at tick {a['tick']}: vy0={a['vy0']:.3f} rise={a['rise']} units in {a['ticks']} ticks")
    print(f"-> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
