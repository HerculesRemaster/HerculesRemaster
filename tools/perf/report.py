"""Frame-time report from the renderer's HERC_PERF CSV (runtime/display/gpu_renderer.cpp).

report.py <perf.csv> [...] [--budget 16.67] [--stall 50] [--from-frame 1700]

Per file: renderer cost per frame (CPU until Present + GPU time, p50/p95/max) against the budget
(Q-05: 16.67 ms p95 at 3840x2160), present interval p50/p95 (the game ticks at 30 Hz, so ~33 ms),
stalls (intervals above --stall ms), load gaps (game frame counter jumps or intervals > 250 ms) and
art texture memory. --from-frame skips the intro/title (the level harness is in the level from
about frame 1600; tools/trace/level_sweep.ps1 shoots at 1700+). Prints one block per file; exit code 1 if a budget is missed.
"""
import argparse
import csv
import statistics
import sys


def pct(v, p):
    v = sorted(v)
    return v[min(len(v) - 1, int(round(p / 100 * (len(v) - 1))))] if v else float("nan")


def report(path, budget, stall, first=0):
    rows = [r for r in list(csv.DictReader(open(path, encoding="utf-8")))[2:] if int(r["frame"]) >= first]   # skip warm-up
    if not rows:
        print(f"{path}: no rows")
        return True
    f = lambda k: [float(r[k]) for r in rows]
    cpu, gpu, iv = f("cpu_ms"), [g for g in f("gpu_ms") if g >= 0], f("interval_ms")
    cost = [c + max(g, 0) for c, g in zip(cpu, f("gpu_ms"))]
    loads = [i for i in iv if i > 250]
    stalls = [i for i in iv if stall < i <= 250]
    ok = pct(cost, 95) <= budget
    print(f"{path}")
    print(f"  frames {len(rows)}  verts p50 {statistics.median(f('verts')):.0f}  art {float(rows[-1]['art_mb']):.1f} MB")
    print(f"  cost  p50 {pct(cost, 50):.2f}  p95 {pct(cost, 95):.2f}  max {max(cost):.2f} ms  (cpu p95 {pct(cpu, 95):.2f}, "
          f"gpu p95 {pct(gpu, 95):.2f})  budget {budget} -> {'ok' if ok else 'MISSED'}")
    print(f"  interval p50 {pct(iv, 50):.1f}  p95 {pct(iv, 95):.1f} ms  stalls >{stall} ms: {len(stalls)}  "
          f"load gaps: {len(loads)} (max {max(loads) if loads else 0:.0f} ms)")
    return ok


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("csv", nargs="+")
    ap.add_argument("--budget", type=float, default=16.67)
    ap.add_argument("--stall", type=float, default=50)
    ap.add_argument("--from-frame", type=int, default=1700)
    a = ap.parse_args()
    sys.exit(0 if all([report(p, a.budget, a.stall, a.from_frame) for p in a.csv]) else 1)
