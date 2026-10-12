"""Bank -> engine -> level index (S4.01) -> extracted/meta/full-game/animation-coverage.json.

Static rules (ENG1.DLL / ENG3.DLL from backups/originals, read with pefile):
  bank table   ENG1 0x5cee0, ENG3 0x39548: 0x14-byte records {ptr path "P:\\SRCn\\...", u32 frame count,
               ptr data, u32, u16 state (3 = loaded), u16}; bank id = record index. Record 0 is empty: its path
               is the empty directory-prefix string (ENG1 0x6ffc0, ENG3 0x3fc80) passed to the file reader; ENG1 0x3910 / 0x3270 walk ids 0..80
               (state words 0x5cef0..0x5d543)
  level lists  ENG1 0x5a690 + 0x18 * level id: {ptr map dir, u32, u32, ptr starts, ptr bank list, ...};
               bank list = u8 ids ending 0xff; each start (0x18 bytes) has its own list at +8
               (FUN_10003270(level list, start list): banks in neither are freed, 0x3910)
  archive      path P:\\SRCn\\X\\Y -> manifest entry M:\\GRAFIX\\CHOP\\SRCn\\X\\Y\\ANIMPSX.BIN
Evidence labels: engine VERIFIED-STATIC (string in that engine's bank table); ENG1 level VERIFIED-STATIC
when the id is in that level's list or a start list; other banks (loaded by actor code on demand) are LEAD
until a runtime load confirms them.
  ENG3 lists   descriptor ENG3 0x38528 + 0x18 * level id (2 GAUNTLET, 8 CYCLOPS, 10 TORMENT; HERO_Init 0x1c720):
               +0xc start records (0x18 bytes, section bank list at +8), +0x10 level bank list (same u8 format)

    bank_index.py
"""
from __future__ import annotations

import json
import struct
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
NAMES = {0: "(id 0)", 1: "TRAINING", 3: "FOREST", 4: "(id 4)", 5: "THEBES", 6: "HYDRA", 7: "MEDUSA", 9: "TITANS",
         11: "VORTEX"}
TABLES = {"ENG1.DLL": (0x1005CEE0, 81), "ENG3.DLL": (0x10039548, None)}


def image(dll: str):
    pe = pefile.PE(str(ROOT / "backups" / "originals" / dll), fast_load=True)
    img = pe.get_memory_mapped_image()
    return lambda va, n: img[va - 0x10000000:va - 0x10000000 + n]


def cstr(rd, va: int) -> str:
    return rd(va, 96).split(b"\0")[0].decode("latin-1")


def banks(dll: str) -> list[dict]:
    rd = image(dll)
    base, count = TABLES[dll]
    out = []
    for k in range(count or 64):
        rec = rd(base + 0x14 * k, 0x14)
        path_va, frames = struct.unpack_from("<II", rec, 0)
        path = cstr(rd, path_va) if 0x10000000 < path_va < 0x10100000 else ""
        if not path.upper().startswith("P:\\SRC"):
            if k == 0:                       # no bank: the path is the empty directory prefix string
                out.append({"id": 0, "record_rva": f"0x{base - 0x10000000:x}",
                            "path": f"(none: empty directory string 0x{path_va - 0x10000000:x})", "frames": frames})
                continue
            break
        out.append({"id": k, "record_rva": f"0x{base + 0x14 * k - 0x10000000:x}", "path": path, "frames": frames})
    return out


def id_list(rd, va: int) -> list[int] | None:
    ids = []
    while va and rd(va, 1)[0] != 0xFF:
        ids.append(rd(va, 1)[0])
        va += 1
        if len(ids) > 24 or ids[-1] > 80:
            return None                       # not a bank list: past the last start entry
    return ids


def eng1_levels() -> dict:
    rd = image("ENG1.DLL")
    out = {}
    for lv in range(12):
        rec = 0x1005A690 + 0x18 * lv
        _, _, _, starts, blist, _ = struct.unpack("<6I", rd(rec, 24))
        if not blist:
            continue
        start_lists = []
        if starts:
            for i in range(16):
                ent = starts + 0x18 * i
                p = struct.unpack("<I", rd(ent + 8, 4))[0]
                if not 0x10000000 < p < 0x10100000:
                    break
                ids = id_list(rd, p)
                if ids is None:
                    break
                start_lists.append(ids)
                if ent + 0x18 >= blist and blist > starts:
                    break
        out[lv] = {"record_rva": f"0x{rec - 0x10000000:x}", "level_list": id_list(rd, blist), "start_lists": start_lists}
    return out


ENG3_NAMES = {2: "GAUNTLET", 8: "CYCLOPS", 10: "TORMENT"}


def eng3_levels() -> dict:
    rd = image("ENG3.DLL")
    out = {}
    for lv in ENG3_NAMES:
        rec = 0x10038528 + 0x18 * lv
        starts, blist = struct.unpack("<II", rd(rec + 0xC, 8))
        start_lists = []
        for i in range(16):
            p = struct.unpack("<I", rd(starts + 0x18 * i + 8, 4))[0]
            if not 0x10000000 < p < 0x10100000:
                break
            ids = id_list(rd, p)
            if ids is None:
                break
            start_lists.append(ids)
        out[lv] = {"record_rva": f"0x{rec - 0x10000000:x}", "level_list": id_list(rd, blist), "start_lists": start_lists}
    return out


LETTER_BANKS = (6, 7, 8, 9, 10, 11, 7, 12)        # ENG1 0x53800: banks of the 8 HERCULES letters


def eng1_map_triggers() -> dict:
    """Map spawns of class 0x1000 are streaming triggers (ENG1 0x26480 -> 0x28290) fired when the camera is
    120..180 units before them: 0x10XX ANIM_RequestBank(XX), 0x18XX ANIM_FreeBank(XX), 0x14XX letter banks
    (bitmask XX over LETTER_BANKS, 0x128a0: requested unless the letter is collected), 0x1CXX frees them (0x12910)."""
    out = {}
    for d in sorted((ROOT / "extracted" / "full-game" / "levels").iterdir()):
        f = d / "level.json"
        if not f.exists():
            continue
        L = json.loads(f.read_text(encoding="utf-8"))
        if "sections" not in L:                      # ENG3 (i1.a) level files: not ENG1 triggers
            continue
        rows = []
        for sp in (s for sec in L["sections"] for s in sec["spawns"] if "kind" in s and s["kind"] & 0x7000 == 0x1000):
            k = sp["kind"]
            op = {0x1000: "request", 0x1800: "free", 0x1400: "letters_request", 0x1C00: "letters_free"}[k & 0x1C00]
            ids = [LETTER_BANKS[i] for i in range(8) if k & (1 << i)] if op.startswith("letters") else [k & 0xFF]
            rows.append({"kind": f"0x{k:04x}", "x": sp["wx"], "op": op, "banks": ids})
        out[d.name] = sorted(rows, key=lambda r: r["x"])
    return out


def type_table_banks() -> dict:
    """Bank ids drawn by each level's type-table kinds (extracted/meta/full-game/L*-types.json; anim-table bank)."""
    out = {}
    for f in sorted((ROOT / "extracted" / "meta" / "full-game").glob("L*-types.json")):
        t = json.loads(f.read_text(encoding="utf-8"))
        if t.get("engine", "ENG1") != "ENG1" or "dir" not in t.get("level", {}):
            continue
        out[t["level"]["dir"]] = sorted({k["bank"]["id"] for k in t["kinds"] if k.get("bank") and k["bank"].get("id")})
    return out


def code_requests() -> dict:
    """bank id -> levels, from constant ANIM_RequestBank ids (extracted/meta/full-game/bank-requests.json) whose caller
    is a type-table handler of that level (L*-types.json)."""
    req = ROOT / "extracted" / "meta" / "full-game" / "bank-requests.json"
    if not req.exists():
        return {}
    handler_levels = {}
    for f in sorted((ROOT / "extracted" / "meta" / "full-game").glob("L*-types.json")):
        t = json.loads(f.read_text(encoding="utf-8"))
        for k in t.get("kinds", []):
            for e in k.get("entry", {}).values():
                if isinstance(e, dict) and e.get("rva"):
                    handler_levels.setdefault(e["rva"], set()).add(t["level"].get("dir"))
    out = {}
    for r in json.loads(req.read_text(encoding="utf-8")):
        for lv in handler_levels.get(r.get("entry_rva"), set()):
            for i in r.get("bank_ids", []):
                out.setdefault(i, set()).add(lv)
    return out


def main() -> int:
    entries = json.loads((ROOT / "manifests" / "archive-manifest.json").read_text(encoding="utf-8"))["entries"]
    by_name = {(e.get("name_candidate") or "").upper(): e for e in entries}
    sprite_dirs = {int(p.name[:4]): p.name for p in (ROOT / "extracted" / "sprites").iterdir() if p.name[:4].isdigit()}
    levels = eng1_levels()
    levels3 = eng3_levels()
    doc = {"_doc": __doc__.split("\n\n")[0] + " See tools/decode/bank_index.py for the rules and labels.",
           "eng1_levels": {str(lv): {"name": NAMES.get(lv, "?"), **v} for lv, v in levels.items() if v["level_list"] is not None},
           "eng3_levels": {str(lv): {"name": ENG3_NAMES[lv], **v} for lv, v in levels3.items()},
           "eng1_map_triggers": eng1_map_triggers(),
           "banks": []}
    triggers, ttab, creq = doc["eng1_map_triggers"], type_table_banks(), code_requests()
    for dll in TABLES:
        for b in banks(dll):
            rel = b["path"][3:].upper()                                  # SRCn\X\Y
            e = None if b["id"] == 0 else by_name.get(f"M:\\GRAFIX\\CHOP\\{rel}\\ANIMPSX.BIN")
            row = {"engine": dll[:4], **b, "archive_index": e["index"] if e else None,
                   "extracted": sprite_dirs.get(e["index"]) if e else None}
            if dll == "ENG1.DLL":
                lv_static = sorted({NAMES.get(lv, str(lv)) for lv, v in levels.items()
                                    if b["id"] in (v["level_list"] or []) or any(b["id"] in s for s in v["start_lists"])})
                trig = sorted({lv for lv, rows in triggers.items() for r in rows
                               if r["op"].endswith("request") and b["id"] in r["banks"]})
                drawn = sorted({lv for lv, ids in ttab.items() if b["id"] in ids})
                code = sorted(creq.get(b["id"], set()))
                row["levels"] = sorted(set(lv_static) | set(trig) | set(code))
                row["map_triggers"] = trig
                row["code_requests"] = code
                row["type_table_levels"] = drawn
                row["level_evidence"] = ("VERIFIED-STATIC (level/start list" + (", map trigger" if trig else "") + ")" if lv_static
                                         else "VERIFIED-STATIC (map trigger)" if trig
                                         else "VERIFIED-STATIC (ANIM_RequestBank in a level handler)" if code
                                         else "LEAD (drawn by a type-table kind; load path open)" if drawn
                                         else "LEAD (no static loader found)")
            else:
                lv_static = sorted({ENG3_NAMES[lv] for lv, v in levels3.items()
                                    if b["id"] in (v["level_list"] or []) or any(b["id"] in s for s in v["start_lists"])})
                row["levels"] = lv_static
                row["level_evidence"] = "VERIFIED-STATIC (ENG3 level/start list)" if lv_static else "LEAD (not in an ENG3 level list)"
            doc["banks"].append(row)
    out = ROOT / "extracted" / "meta" / "full-game" / "animation-coverage.json"
    out.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")
    n = len(doc["banks"])
    print(f"{n} banks ({sum(1 for b in doc['banks'] if b['archive_index'] is not None)} in the archive, "
          f"{sum(1 for b in doc['banks'] if b['extracted'])} extracted, "
          f"{sum(1 for b in doc['banks'] if b['levels'])} with a static level) -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
