"""Attach full-game runs to their scenario definitions and coverage (S1.08).

update_coverage.py [run dir ...]    (default: every analysis/traces/full-game/*/*/*/run.json)

For each run.json (tools/trace/run_meta.py):
  scenario JSON  evidence.runs gets {run_id, path, entry, difficulty_observed, debug_only, ticks};
                 evidence.trace_intervals gets {run_id, level_load_tick, last_tick}; evidence.captures
                 gets the frame index when the run has frames.bin
  coverage       the trace dimension becomes PARTIAL (a run of the scenario's level exists, scene
                 boundaries not yet checked) and capture PARTIAL with frames; asset_extraction and
                 gameplay_specification are never touched here. A debug_level run never upgrades
                 anything (it is not an entry method). Existing better states (TRACED, VERIFIED, ...)
                 are kept.
Both coverage.json and the matching row of coverage.md are rewritten; summary counts are not.
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SC = ROOT / ".plan" / "full-game-remaster" / "reference" / "scenarios"
ORDER = ["PENDING", "PLANNED", "PARTIAL", "CAPTURED", "TRACED", "VERIFIED"]


def better(cur, new):
    return new if ORDER.index(new) > ORDER.index(cur if cur in ORDER else "PENDING") else cur


def scenario_file(sid):
    for p in SC.rglob("*.json"):
        if p.name in ("coverage.json", "schema.json"):
            continue
        try:
            if json.loads(p.read_text(encoding="utf-8")).get("scenario_id") == sid:
                return p
        except ValueError:
            pass
    return None


def apply(run_json: Path, cov: dict):
    r = json.loads(run_json.read_text(encoding="utf-8"))
    sp = scenario_file(r["scenario_id"])
    if not sp:
        print(f"skip {run_json}: scenario {r['scenario_id']} not found")
        return
    sc = json.loads(sp.read_text(encoding="utf-8"))
    rel = run_json.parent.relative_to(ROOT).as_posix()
    debug = r["entry_method"]["debug_only"]
    loads = [e for e in r["identity_events"] if e["ev"] == "level_load" and e.get("level") == sc["internal_level_id"]]
    ev = sc["evidence"]
    ev["runs"] = [x for x in ev["runs"] if not (isinstance(x, dict) and x.get("run_id") == r["run_id"])] + [{
        "run_id": r["run_id"], "path": rel, "entry": r["entry_method"]["method"], "debug_only": debug,
        "difficulty_observed": r["difficulty"]["observed_at_level_load"], "ticks": r["ticks"]}]
    if loads:
        ev["trace_intervals"] = [x for x in ev["trace_intervals"] if not (isinstance(x, dict) and x.get("run_id") == r["run_id"])] + [{
            "run_id": r["run_id"], "level_load_tick": loads[0]["tick"], "last_tick": r["ticks"]["last"],
            "boundaries": "level load to end of run; scene boundaries not yet checked"}]
    if isinstance(r["frame_tick_index"], dict):
        ev["captures"] = [x for x in ev["captures"] if not (isinstance(x, dict) and x.get("run_id") == r["run_id"])] + [{
            "run_id": r["run_id"], "path": rel, **r["frame_tick_index"]}]
    if loads and not debug:
        sc["coverage"]["trace"] = better(sc["coverage"]["trace"], "PARTIAL")
        if isinstance(r["frame_tick_index"], dict):
            sc["coverage"]["capture"] = better(sc["coverage"]["capture"], "PARTIAL")
        if ev["status"] in ("PLANNED", "REFERENCED"):
            ev["status"] = "PARTIAL"
    sp.write_text(json.dumps(sc, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    for row in cov["rows"]:
        if row["scenario_id"] == r["scenario_id"]:
            row["capture"], row["trace"], row["evidence_status"] = sc["coverage"]["capture"], sc["coverage"]["trace"], ev["status"]
    print(f"{r['scenario_id']}: run {r['run_id']} -> trace {sc['coverage']['trace']}, capture {sc['coverage']['capture']}")


def rewrite_md(cov):
    md = SC / "coverage.md"
    text = md.read_text(encoding="utf-8")
    for row in cov["rows"]:
        pat = re.compile(r"^(\| \[" + re.escape(row["scenario_id"]) + r"\]\([^)]*\) \|[^|]*\|[^|]*\|[^|]*\|)\s*[^|]*\|\s*[^|]*\|", re.M)
        text = pat.sub(lambda m: f"{m.group(1)} {row['capture']} | {row['trace']} |", text)
    md.write_text(text, encoding="utf-8")


def main(argv):
    runs = [Path(a).resolve() / "run.json" for a in argv] or sorted((ROOT / "analysis" / "traces" / "full-game").glob("*/*/*/run.json"))
    covp = SC / "coverage.json"
    cov = json.loads(covp.read_text(encoding="utf-8"))
    for r in runs:
        apply(r, cov)
    covp.write_text(json.dumps(cov, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    rewrite_md(cov)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
