"""Offline bank frames vs runtime (S4.05) -> "runtime" block per bank in animation-coverage.json.

Two runtime sources:
  pixels  the runtime texture catalog extracted/art_blocks (keys = FNV-1a 64 of (w, h, texels) of every block
          the GPU renderer drew, art_dump): a frame matches when every keyed part of its offline decode
          (tools/hires/contract.py sprite_entries, ANIM_BuildSpriteQuads 0x4010 parts) is in the catalog
  usage   hook "anim" events (ENG1 0x3cd0 frame changes: bank id, frame, object type) in every trace under
          analysis/traces/**/resource-usage.jsonl: frames shown, object types, and the levels they were seen in
A bank's offline decode is confirmed at pixel level for the matched frames; unmatched frames that were shown at
runtime are listed (decoder mismatch candidates).

    bank_runtime.py
"""
from __future__ import annotations

import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "hires"))
import contract as C  # noqa: E402


def main() -> int:
    cov_p = ROOT / "extracted/meta/full-game/animation-coverage.json"
    cov = json.loads(cov_p.read_text(encoding="utf-8"))
    entries = json.loads(C.MANIFEST_IN.read_text(encoding="utf-8"))["entries"]
    keyer = C.Keyer()
    ents = C.sprite_entries(entries, keyer, 9)
    for key, block in keyer.run():
        block["key"] = f"{key:016x}"
    catalog = set(C.load_catalog())
    frame_keys = defaultdict(dict)                       # archive index -> frame -> [keys]
    for e in ents:
        frame_keys[e["source"]["index"]][e["source"]["frame"]] = [int(b["key"], 16) for b in e["geometry"]["blocks"] if b["key"]]
    shown = defaultdict(lambda: defaultdict(set))       # archive index -> frame -> {level}
    types = defaultdict(set)
    for f in (ROOT / "analysis" / "traces").rglob("resource-usage.jsonl"):
        level = None
        for line in f.open(encoding="utf-8", errors="replace"):
            if '"ev":"state"' in line or '"ev":"anim"' in line:
                try:
                    ev = json.loads(line)
                except ValueError:
                    continue
                if ev["ev"] == "state":
                    level = ev.get("level")
                elif ev.get("bank_index") is not None and ev.get("bank_name"):
                    idx = next((b["archive_index"] for b in cov["banks"] if b["path"] == ev["bank_name"]), None)
                    if idx is not None:
                        shown[idx][ev["frame"]].add(level)
                        types[idx].add(ev.get("type"))
    total_ok = total_frames = 0
    for b in cov["banks"]:
        idx = b["archive_index"]
        if idx is None:
            continue
        fk = frame_keys.get(idx, {})
        matched = sorted(fr for fr, ks in fk.items() if ks and all(k in catalog for k in ks))
        seen = shown.get(idx, {})
        shown_unmatched = sorted(fr for fr in seen if fr in fk and fr not in matched)
        b["runtime"] = {
            "frames_decoded": len(fk), "frames_pixel_matched": len(matched),
            "frames_shown": len(seen), "shown_levels": sorted({lv for s in seen.values() for lv in s if lv is not None}),
            "object_types": sorted(t for t in types.get(idx, set()) if t is not None),
            "shown_but_not_in_catalog": shown_unmatched[:40],
        }
        total_ok += len(matched)
        total_frames += len(fk)
    cov["runtime_doc"] = (__doc__.split("\n\n")[0] + " See tools/decode/bank_runtime.py. Catalog keys: "
                          f"{len(catalog)}.")
    cov_p.write_text(json.dumps(cov, indent=1) + "\n", encoding="utf-8")
    seen_banks = sum(1 for b in cov["banks"] if b.get("runtime", {}).get("frames_shown"))
    print(f"{total_ok}/{total_frames} frames pixel-matched; {seen_banks} banks shown at runtime")
    for b in cov["banks"]:
        r = b.get("runtime")
        if r and (r["frames_shown"] or r["frames_pixel_matched"]):
            print(f"{b['engine']} {b['id']:3d} {b['path'][3:]:40s} decoded {r['frames_decoded']:4d} matched {r['frames_pixel_matched']:4d} "
                  f"shown {r['frames_shown']:4d} levels {r['shown_levels']} unmatched-shown {len(r['shown_but_not_in_catalog'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
