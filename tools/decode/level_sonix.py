"""Per-level Sonix banks (S5.03) -> extracted/meta/full-game/audio-banks.json.

Static rules:
  ENG1 descriptor 0x5a690 + 0x18*level, +0x14 -> Sonix record, passed by MOD_Tick to interface slot 0x6c
  AUD_LoadLevelSonix (EXE 0x16b0):
    [0xff, music volume -> gs+0x1b04, sfx volume -> gs+0x1b08]   (else both 0x80)
    [0xfe, x]                                                    (RT_StubVoid(x, x), then 0x1f40)
    music bank id  -> AUD_LoadMusic(id, slot 1)   (EXE 0x... ; q:\\<dir>\\MUSIC.MUS, VAB)
    sfx bank ids   -> AUD_LoadSfx(id, slot 2..)   (q:\\<dir>\\SFX.SFX) until 0xff; 'c' (0x63) is skipped
  bank id -> directory: EXE table 0x435178, 8-byte records {ptr dir, ...}
  ENG3 descriptor 0x38528 + 0x18*level (2 GAUNTLET, 8 CYCLOPS, 10 TORMENT), +0x14 -> same record format,
  passed to the same slot 0x6c by ENG3 0x1ac63 (runtime "audio" event, GAUNTLET)
  FOREST also streams q:\\NESSUS\\ SFX/MUSIC mid-level (ENG1 AUD_ForestStreamTick 0xe670, table 0x51690).
q:\\ is M:\\LANGUAGE\\SONIX\\ in the archive (manifest names).

    level_sonix.py
"""
from __future__ import annotations

import json
import struct
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
NAMES = {0: "(id 0) PLAYROOM", 1: "TRAINING", 3: "FOREST", 4: "(id 4) NESSUS", 5: "THEBES", 6: "HYDRA", 7: "MEDUSA",
         9: "TITANS", 11: "VORTEX"}


def img(name: str):
    pe = pefile.PE(str(ROOT / "backups" / "originals" / name), fast_load=True)
    return pe.get_memory_mapped_image(), pe.OPTIONAL_HEADER.ImageBase


def cstr(m, base, va):
    return m[va - base:va - base + 64].split(b"\0")[0].decode("latin-1")


def main() -> int:
    e1, b1 = img("ENG1.DLL")
    e3, b3 = img("ENG3.DLL")
    ex, bx = img("HERCULES.EXE")
    entries = json.loads((ROOT / "manifests" / "archive-manifest.json").read_text(encoding="utf-8"))["entries"]
    names = {(e.get("name_candidate") or "").upper(): e["index"] for e in entries}

    def bank_dir(i):
        va = struct.unpack_from("<I", ex, 0x435178 - bx + 8 * i)[0]
        d = cstr(ex, bx, va)
        arch = "M:\\LANGUAGE\\SONIX\\" + d[3:] if d.lower().startswith("q:\\") else d
        return {"id": i, "dir": d, "music_mus": names.get((arch + "MUSIC.MUS").upper()),
                "sfx_sfx": names.get((arch + "SFX.SFX").upper())}

    levels = {}
    plan = [(e1, b1, 0x1005A690, lv, nm, "ENG1") for lv, nm in NAMES.items()]
    plan += [(e3, b3, 0x10038528, lv, nm, "ENG3") for lv, nm in ((2, "GAUNTLET"), (8, "CYCLOPS"), (10, "TORMENT"))]
    for e1, b1, table, lv, nm, eng in plan:
        rec = table + 0x18 * lv
        p = struct.unpack_from("<I", e1, rec - b1 + 0x14)[0]
        if not p:
            continue
        o = p - b1
        out = {"engine": eng, "level_id": lv, "descriptor_field": f"0x{rec - b1 + 0x14:x}", "record": f"0x{o:x}",
               "music_volume": 0x80, "sfx_volume": 0x80}
        if e1[o] == 0xFF:
            out["music_volume"], out["sfx_volume"] = e1[o + 1], e1[o + 2]
            o += 3
        if e1[o] == 0xFE:
            out["fe_arg"] = e1[o + 1]
            o += 2
        out["music"] = bank_dir(e1[o])
        sfx = []
        o += 1
        while e1[o] != 0xFF:
            if e1[o] != 0x63:
                sfx.append(bank_dir(e1[o]))
            o += 1
        out["sfx"] = sfx
        levels[nm] = out
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/decode/level_sonix.py (VERIFIED-STATIC).", "levels": levels}
    path = ROOT / "extracted" / "meta" / "full-game" / "audio-banks.json"
    path.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    for nm, v in levels.items():
        print(f"{nm:16} music {v['music']['dir']:14} sfx {[s['dir'] for s in v['sfx']]} vol {v['music_volume']}/{v['sfx_volume']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
