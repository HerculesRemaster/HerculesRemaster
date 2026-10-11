"""Compare the decoders with the runtime pairs in analysis/traces/pairs/<index>/.

- ESF pairs: esf.decode(payload) vs pcm_out.s16le (the game's AUD_DecodeAdpcm output).
- TGA pairs: tga.decode(payload) pixel rows vs upload.bin (the bytes the game sent to VRAM).
Prints one line per pair and a total.
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import esf  # noqa: E402
import tga  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
PAIRS = ROOT / "analysis" / "traces" / "pairs"


def main() -> int:
    idx_tab, step_tab = esf.exe_tables()
    ok = bad = 0
    for d in sorted(PAIRS.iterdir()):
        meta = json.loads((d / "pair.json").read_text(encoding="utf-8"))
        payload = (d / "payload.bin").read_bytes()
        if (d / "pcm_out.s16le").exists():
            pcm = (d / "pcm_out.s16le").read_bytes()
            n = struct.unpack_from("<I", payload, 4)[0]
            mine = esf.decode(payload[16:], n, idx_tab, step_tab)
            cut = len(pcm) - 2 if (n // 2) % 2 else len(pcm)
            same = mine[:cut] == pcm[:cut]
            what = f"esf {len(pcm) // 2} samples"
        elif payload[:3] == b"\x00\x01\x01":
            im, m = tga.decode(payload)
            up = (d / "upload.bin").read_bytes()
            same = im.tobytes()[: len(up)] == up
            what = f"tga {m['width']}x{m['height']} vs upload {meta['rect']}"
        else:
            print(f"{d.name}: no decoder comparison (raw upload)")
            continue
        ok += same
        bad += not same
        print(f"{d.name}: {what}: {'MATCH' if same else 'DIFF'}")
    print(f"match={ok} diff={bad}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
