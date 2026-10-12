"""Compare a level's decoded VRAM with runtime VRAM dumps (S2.03 / L*.AS.02).

    vram_compare.py RUN_DIR LEVEL [--dir extracted/full-game/levels]
RUN_DIR holds vram_<tick>.bin (hook HERC_HOOK_VRAM_TICKS: EXE 0x96e6e0, first MB, 8 columns x 256-byte rows per
half, unswizzled here as in runtime/display/gpu_renderer.cpp Unswizzle). Regions:
  img0      VRAM (0,0) 768x512 units = bytes x 0..1535      (map header image, LVL_UploadVram 0x1fc90)
  img1      VRAM (768,256) 256x256 units = bytes x 1536..2047, rows 256..511, split at the map header size
  cells     rows 0..255, x >= 1536: runtime sprite cells (ANIM_UploadCells), not map data
Prints equality per region and writes <RUN_DIR>/vram_compare.json.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]


def unswizzle(raw: bytes) -> np.ndarray:
    v = np.frombuffer(raw, np.uint8)
    lin = np.zeros((512, 2048), np.uint8)
    for half in range(2):
        for c in range(8):
            o = half * 0x80000 + c * 0x10000
            lin[half * 256:(half + 1) * 256, c * 256:(c + 1) * 256] = v[o:o + 0x10000].reshape(256, 256)
    return lin


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("run")
    ap.add_argument("level")
    ap.add_argument("--dir", default=str(ROOT / "extracted" / "full-game" / "levels"))
    a = ap.parse_args(argv)
    d = Path(a.dir) / a.level
    L = json.loads((d / "level.json").read_text(encoding="utf-8"))
    hs, img1 = L["header"]["header_size"], L["header"]["img1"]
    read_rows = max(0, min(256, (hs - img1) // 512))           # rows of img1 inside the loaded header
    f = np.frombuffer((d / "vram.bin").read_bytes(), np.uint8).reshape(512, 2048)
    out = {"level": a.level, "img1_rows_loaded": read_rows, "dumps": {}}
    for p in sorted(Path(a.run).glob("vram_*.bin")):
        lin = unswizzle(p.read_bytes())
        r = {"img0_equal": float((lin[:, :1536] == f[:, :1536]).mean()),
             "img1_loaded_equal": float((lin[256:256 + read_rows, 1536:] == f[256:256 + read_rows, 1536:]).mean()) if read_rows else None,
             "img1_tail_rows": 256 - read_rows,
             "cells_area_equal_to_file": float((lin[:256, 1536:] == f[:256, 1536:]).mean())}
        out["dumps"][p.name] = r
        print(a.level, p.name, json.dumps(r))
    (Path(a.run) / "vram_compare.json").write_text(json.dumps(out, indent=1) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
