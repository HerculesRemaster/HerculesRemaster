"""Bind sound plays to the objects that request them (S5.01) -> extracted/meta/full-game/sfx-events.json.

    sfx_events.py
Reads every analysis/traces/**/resource-usage.jsonl that has hook "sfx_request" events (detours of ENG1 0x6da0 /
ENG3 AUD_PlaySfx 0xbc47: handler sound id, object, object kind obj+0x2e, caller). Rule, VERIFIED-RUNTIME: the EXE
voice id of the following sfx_play is id & 0x7ff (bits 0x800/0x2000/0x4000/0x8000 are play flags: 0x4000 =
positional, see AUD_PlaySfx). A request without a sfx_play in the same tick was refused (voice limit, bank slot
0xff, or flags). Per level (from level_load) and voice: request ids, object kinds (with the level's
L*-types.json identification when present), callers, plays and refusals.
"""
import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SLUG = {1: "L01", 2: "L02", 3: "L03", 5: "L04", 6: "L05", 7: "L06", 8: "L07", 9: "L08", 10: "L09", 11: "L10"}


def kind_names():
    out = {}
    for f in (ROOT / "extracted/meta/full-game").glob("L*-types.json"):
        d = json.loads(f.read_text(encoding="utf-8"))
        out[f.name[:3]] = {int(k["kind"], 16): (k.get("what") or k.get("name") or "")[:70] for k in d.get("kinds", [])}
    return out


def main():
    names = kind_names()
    lv = defaultdict(lambda: defaultdict(lambda: {"request_ids": set(), "kinds": set(), "callers": set(), "plays": 0,
                                                  "refused": 0, "runs": set()}))
    nruns = 0
    for f in sorted((ROOT / "analysis/traces").rglob("resource-usage.jsonl")):
        level, pend, had = None, None, False
        run = str(f.parent.relative_to(ROOT)).replace("\\", "/")
        for line in f.open(encoding="utf-8", errors="replace"):
            if '"level_load"' in line:
                level = json.loads(line)["level"]
            elif '"module_load"' in line:
                level = None
            elif '"sfx_request"' in line:
                e = json.loads(line)
                if pend is not None:
                    pend[0]["refused"] += 1
                had = True
                v = lv[level if level is not None else 255][e["id"] & 0x7FF]
                v["request_ids"].add(e["id"]); v["kinds"].add(e["kind"]); v["callers"].add(e["caller"]); v["runs"].add(run)
                pend = (v, e["tick"])
            elif '"sfx_play"' in line and pend is not None:
                e = json.loads(line)
                if e["tick"] == pend[1] and (e["sfx"] & 0x7FF) in (lv[level if level is not None else 255].keys()):
                    pend[0]["plays"] += 1
                else:
                    pend[0]["refused"] += 1
                pend = None
        nruns += had
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/trace/sfx_events.py for the rule.", "levels": {}}
    for level, voices in sorted(lv.items()):
        slug = SLUG.get(level, "TITLE")
        doc["levels"][slug] = {str(vid): {"request_ids": [hex(x) for x in sorted(v["request_ids"])],
                                          "kinds": [{"kind": f"0x{k:04x}" if k >= 0 else None, "what": names.get(slug, {}).get(k)}
                                                    for k in sorted(v["kinds"])],
                                          "callers": sorted(v["callers"]), "plays": v["plays"], "refused": v["refused"],
                                          "runs": sorted(v["runs"])}
                               for vid, v in sorted(voices.items())}
    out = ROOT / "extracted/meta/full-game/sfx-events.json"
    out.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    print(f"{nruns} runs with sfx_request; " + ", ".join(f"{k}: {len(v)} voices" for k, v in doc["levels"].items()) + f" -> {out.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
