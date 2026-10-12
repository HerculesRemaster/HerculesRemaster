"""Per ENG1/ENG3 level: sprite banks loaded at runtime vs exported animation data (L*.AS.03).

    level_anim_coverage.py   -> extracted/meta/full-game/level-anim-coverage.json

Loaded banks: rows of extracted/meta/full-game/level-assets.json (tools/meta/scene_assets.py) whose name ends in
ANIMPSX.BIN. Exported: extracted/sprites/<bank>/bank.json (frames, decoded by tools/decode/ifam.py) and
anim.json (tools/decode/anim.py: clips, durations, loops, code-backed frame hitboxes from GFX_FrameHitbox
ENG1 0x30e40 / ENG3 0x23860). A bank that loaded but was never drawn in any run has frames but no clips.
"""
from __future__ import annotations

import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SPRITES = ROOT / "extracted" / "sprites"
META = ROOT / "extracted" / "meta" / "full-game"
LEVEL_NAMES = {1: "TRAINING", 2: "GAUNTLET", 3: "FOREST", 5: "THEBES", 6: "HYDRA", 7: "MEDUSA", 8: "CYCLOPS",
               9: "TITANS", 10: "TORMENT", 11: "VORTEX"}


def main() -> int:
    rows = json.loads((META / "level-assets.json").read_text(encoding="utf-8"))["rows"]
    out = defaultdict(list)
    for r in rows:
        if not r["name"].upper().endswith("ANIMPSX.BIN") or r["level"] not in LEVEL_NAMES:
            continue
        idx = r["archive_index"]
        d = next(iter(SPRITES.glob(f"{idx:04d}_*")), None)
        bank = json.loads((d / "bank.json").read_text(encoding="utf-8")) if d and (d / "bank.json").is_file() else None
        anim = json.loads((d / "anim.json").read_text(encoding="utf-8")) if d and (d / "anim.json").is_file() else None
        frames = anim.get("frames", {}) if anim else {}
        hb = sum(1 for f in frames.values() if isinstance(f, dict) and f.get("hitboxes"))
        out[LEVEL_NAMES[r["level"]]].append({
            "archive_index": idx, "name": r["name"], "consumer": r["consumer"], "first_tick": r["first_tick"],
            "frames_decoded": len(bank["frames"]) if bank else None,
            "frame_errors": sum(1 for f in bank["frames"] if f.get("error")) if bank else None,
            "clips": len(anim.get("clips", [])) if anim else 0, "frames_seen": len(frames),
            "frames_with_hitboxes": hb, "drawn_in_runs": bool(anim and anim.get("clips")),
            "export": str(d.relative_to(ROOT)).replace("\\", "/") if d else None})
    res = {"_doc": __doc__.split("\n\n")[1].strip(), "levels": {k: sorted(v, key=lambda x: x["archive_index"]) for k, v in out.items()}}
    (META / "level-anim-coverage.json").write_text(json.dumps(res, indent=1) + "\n", encoding="utf-8")
    for lv, bs in res["levels"].items():
        drawn = [b for b in bs if b["drawn_in_runs"]]
        print(f"{lv:9s} banks {len(bs):3d} drawn {len(drawn):3d} hitbox frames {sum(b['frames_with_hitboxes'] for b in bs):5d}",
              "not drawn:", [f"{b['archive_index']} {b['name'].split(chr(92))[-2]}" for b in bs if not b["drawn_in_runs"]][:12])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
