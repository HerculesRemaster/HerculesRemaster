"""ENG3 `i1.a` maps (GAUNTLET, CYCLOPS, TORMENT) -> extracted/full-game/levels/<NAME>/ (S3.04).

eng3_decode.py [--out extracted/full-game/levels]

Rules from ENG3.DLL (docs/full-game/eng3-format.md): LVL_ParseHeader 0x1af10, LVL_UploadVram 0x1aa30,
LVL_FindSurface 0x1b2e0, HERO_Init 0x1c720, level descriptor 0x38528. Writes level.json (header counts,
colour, blocks of 12 x N u32 cells with the known cell fields, the collision-profile offsets, start
records, raw offsets of the tables whose meaning is still open), vram.bin/vram.png and the CIF tables
(palette.json, fog/shade/blend.bin) like tools/level/level_decode.py. Unknown structures raise
UnsupportedVariant rather than being guessed.
"""
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from level_decode import ROOT, MANIFEST, u16, u32, vram_image, vram_png, texture  # noqa: E402

LEVELS = {"GAUNTLET": 2, "CYCLOPS": 8, "TORMENT": 10}
ENG3 = ROOT / "backups" / "originals" / "ENG3.DLL"


class UnsupportedVariant(Exception):
    pass


def rel(d, o, mask=~3):
    return o + (u32(d, o) & mask)


def eng3_rva_reader():
    b = ENG3.read_bytes()
    pe = struct.unpack_from("<I", b, 0x3c)[0]
    n = struct.unpack_from("<H", b, pe + 6)[0]
    opt = struct.unpack_from("<H", b, pe + 20)[0]
    secs, o = [], pe + 24 + opt
    for _ in range(n):
        vs, va, raw, ptr = struct.unpack_from("<IIII", b, o + 8)
        secs.append((va, vs, raw, ptr))
        o += 40

    def rd(rva, size):
        for va, vs, raw, ptr in secs:
            if va <= rva < va + max(vs, raw):
                return b[ptr + rva - va:ptr + rva - va + size]
        raise KeyError(hex(rva))
    return rd


def start_records(level_id):
    """Descriptor 0x38528[level] +0xc -> 0x18-byte start records (HERO_Init 0x1c720)."""
    rd = eng3_rva_reader()
    table = struct.unpack("<I", rd(0x38534 + level_id * 0x18, 4))[0] - 0x10000000
    out = []
    for i in range(16):
        r = rd(table + i * 0x18, 0x18)
        x, y, z, section = struct.unpack_from("<HHBB", r, 0)
        ptr = struct.unpack_from("<I", r, 8)[0]
        if not (0x10000000 <= ptr < 0x10100000):          # next level's data or text: end of table
            break
        out.append({"index": i, "x": x, "y": y, "z": z, "section": section, "list": f"0x{ptr - 0x10000000:x}",
                    "u32_10": f"0x{struct.unpack_from('<I', r, 0x10)[0]:08x}"})
    return out


def templates(level_id):
    """Per-level object config 0x3b900 (0x48 bytes, u16 level id, 999 ends; LVL_SelectObjectConfig 0x1d9b0) +0x40:
    10-byte spawn templates {s16 dx, dy, dz, u16 flags, u16 type} (LVL_InitSpawnLists 0x1c010)."""
    rd = eng3_rva_reader()
    o = 0x3B900
    while struct.unpack("<H", rd(o, 2))[0] not in (level_id, 999):
        o += 0x48
    return struct.unpack("<I", rd(o + 0x40, 4))[0] - 0x10000000, rd


def srel(d, o):
    return o + struct.unpack_from("<h", d, o)[0]


def tracks(d, base, n, level_id):
    """Track table hdr+0x30, 0x10 bytes per track (docs/full-game/eng3-tables.md); rel s16 = from the field."""
    t_rva, rd = templates(level_id)
    out = []
    for t in range(n):
        o = base + 0x10 * t
        bl = srel(d, o + 6)
        nb = d[bl]                                             # u8 n, then n x (u8 block, s8 height)
        vis, p = [], srel(d, o + 8)
        while d[p] != 0xFF and len(vis) < 64:
            vis.append(d[p])
            p += 1
        cnt, sp = u16(d, o + 0xC), srel(d, o + 0xE)
        spawns = []
        for i in range(cnt):
            x, y, z, ti = struct.unpack_from("<HHhH", d, sp + 8 * i)
            dx, dy, dz, flags, typ = struct.unpack("<hhhHH", rd(t_rva + 10 * ti, 10))
            spawns.append({"x": x, "y": y, "z": z, "template": ti, "wx": x + dx, "wy": y + dy, "wz": z + dz,
                           "flags": flags, "type": f"0x{typ:04x}"})
        out.append({"index": t, "parent": d[o], "branch_column": d[o + 1], "parent_draw_limit": d[o + 2],
                    "yaw_offset": u16(d, o + 4), "blocks": [list(struct.unpack_from("<Bb", d, bl + 1 + 2 * i)) for i in range(nb)], "visible_tracks": vis, "turn_list_offset": srel(d, o + 0xA),
                    "spawn_list_offset": sp, "spawns": spawns})
    return out


def splines(d, arrays, hdr):
    """n14 track splines (LVL_TrackSpline 0x1b650): array after n8, nE, n10, n12 dwords; rec = entry + (dword & ~3):
    u16 count, u16 (open), count x 0x10 key points {u16 x, u16 z centre (+0x300), u16 y part, u16 half-width,
    u16 y part, s8 camera term (+0xa), s8 (+0xc)} (docs/full-game/eng3-tables.md)."""
    base = arrays + 4 * (hdr["n8_profiles"] + hdr["nE"] + hdr["n10"] + hdr["n12"])
    out = []
    for i in range(hdr["n14"]):
        e = base + 4 * i
        r = rel(d, e)
        n, f2 = u16(d, r), u16(d, r + 2)
        pts = []
        for k in range(n):
            o = r + 4 + 0x10 * k
            x, zc, y1, hw, y2 = struct.unpack_from("<5H", d, o)
            pts.append({"x": x, "z_centre": zc, "y": y1 + y2, "half_width": hw, "cam": struct.unpack_from("<b", d, o + 0xA)[0],
                        "c": struct.unpack_from("<b", d, o + 0xC)[0]})
        out.append({"index": i, "u16_02": f2, "points": pts})
    return out


def models(d, arrays, hdr):
    """nE models (k = obj+0x34, 1-based): entry k at arrays + 4(n8 + k - 1), record = entry + s32 (self-relative).
    GFX_ModelHitbox 0x24870: +2 u8 n hitboxes, +3 u8 cull radius, +4 s32 offset, +8 n x 8-byte box =
    u32 min corner (x s16 bits 8-23, y s8 bits 24-31, z s8 bits 0-7) and u32 extent (x u16 bits 8-23, y u8
    bits 24-31, z u8 bits 0-7). A non-zero offset is added to the corner as x += s8 bits 0-7, y += s16 bits 8-23,
    z += s8 bits 24-31 (literal from the code). Runtime scales by obj+0x3a (or +0x3a/+0x3c/+0x3e)."""
    def s8(v):
        return v - 256 if v > 127 else v
    out = []
    for k in range(1, hdr["nE"] + 1):
        e = arrays + 4 * (hdr["n8_profiles"] + k - 1)
        r = e + struct.unpack_from("<i", d, e)[0]
        if not 0 <= r < len(d) - 8:
            out.append({"model": k, "error": "record outside the file"})
            continue
        n, cull = d[r + 2], d[r + 3]
        off = struct.unpack_from("<i", d, r + 4)[0]
        boxes = []
        for i in range(n if n < 32 else 0):
            c, sz = struct.unpack_from("<II", d, r + 8 + 8 * i)
            x, y, z = struct.unpack("<h", struct.pack("<H", (c >> 8) & 0xFFFF))[0], s8(c >> 24), s8(c & 0xFF)
            if off:
                x += s8(off & 0xFF)
                y += struct.unpack("<h", struct.pack("<H", (off >> 8) & 0xFFFF))[0]
                z += off >> 24
            boxes.append({"min": {"x": x, "y": y, "z": z}, "extent": {"x": (sz >> 8) & 0xFFFF, "y": sz >> 24, "z": sz & 0xFF}})
        out.append({"model": k, "record": r, "cull_radius": cull, "offset": off, "hitboxes": boxes})
    return out


def mesh_textures(d, arrays, hdr):
    """Texture words used by the n8 cell meshes (0x206f0, docs/full-game/eng3-tables.md "n8"): entry k at
    arrays + 4(k - 1), lo s16 = mesh offset in dwords from entry + 2; mesh = u8 nverts, u8 nfaces, nverts u32, then
    from mesh + 4*nverts + 6 nfaces x 8-byte faces {u32 4 x 6-bit vertex index + flags byte (0x80 flat colour,
    0x20 semi-transparent), u32 texture (low 16 = byte offset into the texture-word table hdr+0x2c, 0x40658)}.
    A texture word is 3 u32 as ENG1 (level_decode.texture): uv0 | clut << 16, uv1 | tpage << 16, uv2 | uv3 << 16.
    Returns the distinct (texture offset, flags & 0xa0) pairs with their decoded words."""
    t2c = hdr["t2c"]
    used = {}
    for k in range(1, hdr["n8_profiles"] + 1):
        e = arrays + 4 * (k - 1)
        lo = struct.unpack_from("<h", d, e)[0]
        m = e + 2 + 4 * lo
        if not 0 <= m < len(d) - 2:
            continue
        nv, nf = d[m], d[m + 1]
        f0 = m + 4 * nv + 6
        for i in range(nf):
            o = f0 + 8 * i
            if o + 8 > len(d):
                break
            fw, tw = struct.unpack_from("<II", d, o)
            flags = fw >> 24
            if flags & 0x80:
                continue
            off = tw & 0xFFFF
            if (off, flags & 0x20) not in used and t2c + off + 12 <= len(d):
                used[(off, flags & 0x20)] = texture(d, t2c, off)
    return [{"offset": off, "semi": bool(sm), **w} for (off, sm), w in sorted(used.items())]


def decode(d: bytes, name: str) -> dict:
    if d[:4] != b"i1.a":
        raise UnsupportedVariant(d[:4])
    n8, nblocks, nC, nE, n10, n12, n14 = struct.unpack_from("<7H", d, 8)
    hdr = {"tag": "i1.a", "size": u32(d, 4), "n8_profiles": n8, "blocks": nblocks, "u16_0c": nC, "nE": nE,
           "n10": n10, "n12": n12, "n14": n14, "img0": rel(d, 0x18), "img1": rel(d, 0x1c),
           "t28": rel(d, 0x28), "t2c": rel(d, 0x2c), "t30_tracks": rel(d, 0x30),
           "colour": f"{u32(d, 0x34) & 0xffffff:06x}"}
    blocks = []
    p = 0x38
    for k in range(nblocks):
        b = rel(d, p + 4 * k, ~1)
        rows, cols = u16(d, b), u16(d, b + 2)
        if rows != 12:
            raise UnsupportedVariant(f"block {k}: {rows} rows")
        cells = list(struct.unpack_from(f"<{rows * cols}I", d, b + 4))
        blocks.append({"offset": b, "rows": rows, "columns": cols,
                       "cells": cells,
                       "layout": "column-major: cell (column c, row r) = cells[c * rows + r]", "cell_fields": "bit0 empty; bits1-10 profile; bits11-23 light zone; bit24 mirror; bits24-31 s8 height"})
    q = 0x38 + 4 * nblocks
    hdr["after_blocks"] = {"ptr_40680": rel(d, q), "ptr_4067c_zones": rel(d, q + 4), "ptr_40678_corners": rel(d, q + 8),
                           "ptr_4066c": rel(d, q + 12), "arrays": q + 16}
    arrays = q + 16
    profiles = list(struct.unpack_from(f"<{n8}I", d, arrays)) if n8 else []
    hdr["array_layout_dwords"] = {"profiles(n8)": n8, "nE": nE, "n10": n10, "n12": n12, "n14": n14}
    return {"name": name, "level_id": LEVELS[name], "engine": "ENG3", "header": hdr, "blocks": blocks,
            "profiles_raw": profiles, "starts": start_records(LEVELS[name]),
            "tracks": tracks(d, hdr["t30_tracks"], nC, LEVELS[name]),
            "templates_rva": f"0x{templates(LEVELS[name])[0]:x}",
            "splines": splines(d, arrays, hdr),
            "models": models(d, arrays, hdr),
            "mesh_textures": mesh_textures(d, arrays, hdr)}


def main(argv) -> int:
    out_root = ROOT / "extracted" / "full-game" / "levels"
    if "--out" in argv:
        out_root = Path(argv[argv.index("--out") + 1])
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    ok = 0
    for name in LEVELS:
        e = next(x for x in entries if (x.get("name_candidate") or "").upper().endswith(f"\\{name}\\MAP_PSX.BIN"))
        d = (ROOT / e["file"]).read_bytes()
        lvl = decode(d, name)
        out = out_root / name
        out.mkdir(parents=True, exist_ok=True)
        vram = vram_image(d, lvl["header"]["img0"], lvl["header"]["img1"])
        (out / "vram.bin").write_bytes(vram)
        vram_png(vram).save(out / "vram.png")
        cif = next((x for x in entries if (x.get("name_candidate") or "").upper()
                    == e["name_candidate"].upper()[:-4] + ".CIF"), None)
        if cif:
            c = (ROOT / cif["file"]).read_bytes()
            pal = [list(c[i * 3:i * 3 + 3]) for i in range(256)]
            pal[0], pal[255] = [0, 0, 0], [255, 255, 255]
            (out / "palette.json").write_text(json.dumps(pal), encoding="utf-8")
            (out / "fog.bin").write_bytes(c[768 + 0x40000 + 0x10000:768 + 0x40000 + 0x10000 + 0x1F00])
            (out / "shade.bin").write_bytes(c[768 + 0x40000:768 + 0x40000 + 0x10000])
            (out / "blend.bin").write_bytes(c[768:768 + 0x40000])
            lvl["palette_file"] = {"index": cif["index"], "name": cif["name_candidate"]}
        lvl["archive"] = {"index": e["index"], "name": e["name_candidate"]}
        (out / "level.json").write_text(json.dumps(lvl), encoding="utf-8")
        print(f"{e['index']:04d} {name}: blocks={len(lvl['blocks'])} profiles={len(lvl['profiles_raw'])} "
              f"starts={len(lvl['starts'])} colour={lvl['header']['colour']}")
        ok += 1
    print(f"decoded={ok} -> {out_root}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
