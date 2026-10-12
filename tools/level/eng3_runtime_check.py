"""Offline ENG3 map export vs a traced run (L02/L07/L09 AS.02).

eng3_runtime_check.py <run dir> [NAME]

  vram     every pairs/s*_vram_* upload whose from_index is the level's MAP_PSX.BIN is compared byte for byte
           with the same rectangle of extracted/full-game/levels/<NAME>/vram.bin (1024 x 512 units, 2048-byte rows)
  palette  palette.json hashed like the hook's "palette" event (FNV-1a 32 over the 256 RGBQUAD entries,
           B G R 0, of the DIB colour table at 0x96e3f4 + 0x28) and looked up in the run's palette events
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LV = ROOT / "extracted" / "full-game" / "levels"


def map_index(name):
    for e in json.loads((ROOT / "manifests" / "archive-manifest.json").read_text(encoding="utf-8"))["entries"]:
        if (e.get("name_candidate") or "").upper() == f"M:\\GRAFIX\\MAPS\\{name}\\MAP_PSX.BIN":
            return e["index"]
    raise KeyError(name)


def pal_hash(pal):
    k = 2166136261
    for r, g, b in pal:
        for v in (b, g, r, 0):
            k = ((k ^ v) * 16777619) & 0xFFFFFFFF
    return f"{k:08X}"


def main(argv):
    run = Path(argv[0])
    names = argv[1:] or [n for n in ("GAUNTLET", "CYCLOPS", "TORMENT") if (LV / n / "vram.bin").exists()]
    hashes = {}
    for line in (run / "resource-usage.jsonl").open(encoding="utf-8", errors="replace"):
        if '"palette"' in line:
            e = json.loads(line)
            hashes.setdefault(e["hash"], e["tick"])
    out = {}
    for name in names:
        idx = map_index(name)
        vram = (LV / name / "vram.bin").read_bytes()
        rects = []
        for pj in sorted(run.glob("pairs/s*_vram_*/pair.json")):
            p = json.loads(pj.read_text())
            if p.get("from_index") != idx:
                continue
            r = p["rect"]
            up = (pj.parent / "upload.bin").read_bytes()
            off = b"".join(vram[(r["y"] + y) * 2048 + r["x"] * 2:(r["y"] + y) * 2048 + (r["x"] + r["w16"]) * 2]
                           for y in range(r["h"]))
            rects.append({"pair": pj.parent.name, "rect": r, "bytes": len(up), "tick": p["tick"],
                          "differing_bytes": sum(a != b for a, b in zip(up, off)) + abs(len(up) - len(off))})
        h = pal_hash(json.loads((LV / name / "palette.json").read_text()))
        out[name] = {"map_index": idx, "vram": rects, "palette_hash": h, "palette_first_tick": hashes.get(h)}
        ok = rects and all(x["differing_bytes"] == 0 for x in rects)
        print(f"{name}: {len(rects)} map uploads, {'identical' if ok else 'DIFFER or none'}; "
              f"palette {h} {'seen at tick ' + str(hashes[h]) if h in hashes else 'not seen'}")
    (run / "eng3_runtime_check.json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
