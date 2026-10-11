"""Pack approved replacement images into runtime key files and report texture memory.

atlas.py [--art-dir working/game/art] [--allow-partial]   (normally called by jobs.py pack)

Each manifest block is cut from the approved image at rect x scale and written as
<art_dir>/<family>/<key>_<w>x<h>.png (families: sprite, level, ui, screen; the ini's art_families picks
which the game loads) (indexed images stay indexed, others RGBA). Every block is its own texture
and the renderer reads it with integer loads (no filtering, no neighbours), so no padding or
extrusion is needed. A sprite bank is packed only when all its frames are approved (frame-count
check), unless --allow-partial. A key that appears in several entries is written once (first wins).
"""
import argparse
import json
from collections import defaultdict

from PIL import Image

import common


def pack(approved_ids: list[str], art_dir=common.ART_DIR, allow_partial=False) -> dict:
    ents = common.entries_by_id()
    k = common.manifest()["scale"]
    banks = defaultdict(set)
    for e in ents.values():
        if e["family"] == "sprite":
            banks[e["source"]["index"]].add(e["id"])
    ok_ids, skipped = [], defaultdict(int)
    for i in approved_ids:
        e = ents[i]
        if e["family"] == "sprite" and not allow_partial and not banks[e["source"]["index"]] <= set(approved_ids):
            skipped[f"bank {e['source']['index']:04d} incomplete"] += 1
            continue
        ok_ids.append(i)
    art_dir.mkdir(parents=True, exist_ok=True)
    written, conflicts, mem = set(), 0, defaultdict(int)
    for i in ok_ids:
        e = ents[i]
        img = Image.open(common.output_path(e))
        rgba = img.mode != "P"
        if rgba:
            img = img.convert("RGBA")
        for b in e["geometry"]["blocks"]:
            if not b["key"]:
                continue
            x, y, w, h = b["rect"]
            name = f"{b['key']}_{w}x{h}.png"
            if name in written:
                conflicts += 1
                continue
            (art_dir / e["family"]).mkdir(parents=True, exist_ok=True)
            img.crop((x * k, y * k, (x + w) * k, (y + h) * k)).save(art_dir / e["family"] / name)
            written.add(name)
            mem[e["family"]] += w * h * k * k * (4 if rgba else 1)
    report = {"entries": len(ok_ids), "skipped": dict(skipped), "files": len(written), "duplicate_keys": conflicts,
              "texture_mb": {f: round(v / 2 ** 20, 1) for f, v in mem.items()},
              "texture_mb_total": round(sum(mem.values()) / 2 ** 20, 1)}
    (common.REMASTERED / "pack-report.json").write_text(json.dumps(report, indent=1), encoding="utf-8")
    return {"packed": ok_ids, "report": report}


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--art-dir", default=str(common.ART_DIR))
    ap.add_argument("--allow-partial", action="store_true")
    a = ap.parse_args()
    jobs = json.loads(common.JOBS.read_text(encoding="utf-8"))["jobs"]
    ids = [j["entry"] for j in jobs.values() if j["state"] in ("approved", "packed")]
    from pathlib import Path
    print(json.dumps(pack(ids, Path(a.art_dir), a.allow_partial)["report"], indent=1))
