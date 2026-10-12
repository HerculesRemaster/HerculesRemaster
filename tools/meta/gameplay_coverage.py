"""Gameplay-coverage rows per scenario (P5.ALL.13, L*.GP.07) -> extracted/meta/full-game/gameplay-coverage.json.

    gameplay_coverage.py
For every scenario JSON in .plan/full-game-remaster/reference/scenarios/: the level spec
docs/full-game/levels/<Lxx>.md (if written) and the spec sections it has (## headings), the section that best
matches the scenario's scene_group (keyword rule SECTION_KEYS), and the evidence runs from
extracted/meta/full-game/scene-asset-coverage.json (tools/meta/scene_assets.py; debug-only runs listed apart).
status: "spec+trace" (a section and a non-debug run), "spec" (section only), "trace" (runs only), "open".
"""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SC = ROOT / ".plan" / "full-game-remaster" / "reference" / "scenarios"
SECTION_KEYS = [("Hazards", ("hazard", "obstacle", "stalactite", "chariot", "river", "fall", "column", "traffic")),
                ("Enemies", ("enemy", "skeleton", "soul", "cyclops", "boss", "dummy", "rescue")),
                ("Pickups", ("pickup", "checkpoint", "retry", "death", "completion", "power", "letter")),
                ("Runner parameters", ("difficulty", "speed", "beginner", "herculean")),
                ("Layout", ("route", "crossroad", "lane", "stairs", "section", "opening", "middle", "final", "traversal",
                            "progression", "passage", "chase", "entry"))]


def main():
    cov = json.loads((ROOT / "extracted/meta/full-game/scene-asset-coverage.json").read_text(encoding="utf-8"))
    runs = {c["scenario_id"]: c for c in cov["scenarios"]}
    rows = []
    for f in sorted(SC.glob("*/*.json")):
        d = json.loads(f.read_text(encoding="utf-8"))
        sid, lvl = d["scenario_id"], d["level_id"]
        spec = ROOT / "docs/full-game/levels" / f"{lvl}.md"
        heads = re.findall(r"^##+ (.+)$", spec.read_text(encoding="utf-8"), re.M) if spec.exists() else []
        text = (sid + " " + str(d.get("scene_group", ""))).lower()
        sect = None
        for name, keys in SECTION_KEYS:
            if any(k in text for k in keys):
                sect = next((h for h in heads if h.startswith(name) or name in h), None)
                if sect:
                    break
        if not sect and heads:
            sect = next((h for h in heads if h.startswith("Layout")), None)
        r = runs.get(sid, {})
        good, dbg = r.get("runs", []), r.get("debug_only_runs", [])
        status = "spec+trace" if sect and good else "spec" if sect else "trace" if good else "open"
        rows.append({"scenario_id": sid, "level": lvl, "scene_group": d.get("scene_group"),
                     "spec": f"docs/full-game/levels/{lvl}.md" if spec.exists() else None, "spec_section": sect,
                     "runs": good, "debug_only_runs": dbg, "status": status})
    out = ROOT / "extracted/meta/full-game/gameplay-coverage.json"
    out.write_text(json.dumps({"_doc": __doc__.split("\n\n")[0] + " See tools/meta/gameplay_coverage.py.", "rows": rows},
                              indent=1) + "\n", encoding="utf-8")
    from collections import Counter
    print(Counter(r["status"] for r in rows), "->", out.relative_to(ROOT))
    for r in rows:
        if r["level"] in ("L02", "L07", "L09"):
            print(f"  {r['scenario_id']:32} {r['status']:10} {r['spec_section']}")


if __name__ == "__main__":
    main()
