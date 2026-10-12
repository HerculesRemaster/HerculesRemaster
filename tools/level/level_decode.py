"""Decode MAP_PSX.BIN level files (i2.5) -> extracted/levels/<LEVEL>/level.json + vram.png.

Rules from ENG1.DLL (docs/level-format.md). All "rel" fields are self-relative: target =
field_offset + (u32 & ~3).
  header (LVL_LoadMap 0x1f750 reads 8 bytes, then u32[1] bytes; tag must equal the engine's)
    +0x00 tag ("i2.5"), +0x04 header size (= first block's sector * 0x800)
    +0x08 u16 sections, +0x0a u16 n_e, +0x0c u16 n_types, +0x0e u16 n_extra,
    +0x10 u16 n_anim (animated VRAM rects, 0x2cb0), +0x12 u16 n_scroll (scrolling rects, 0x2d80)
    +0x14 rel VRAM image 768x512 units -> VRAM (0,0); +0x18 rel 256x256 -> (768,256) (0x1fc90)
    +0x1c s32 relative (returned by 0x1fc90), +0x20 u32
    +0x24 rel -> T28 (model meshes, 0x31540), +0x28 rel -> T30 (texture words, 3 x u32)
    +0x2c 3 bytes (0x70cf0), +0x30 rel[sections] -> section descriptors,
    then rel x4 -> camera records T58 (16 bytes), camera index T54 (u32), shade table T50, T44,
    then the u32 table T34 (block types; T38 anim, T3c scroll, T4c, T40 attributes follow).
  section (0x20830): u8 layers, u8, u8 blocks, u8, u16 columns, u16 rows (16), u16, s16 rel,
    u32 cumulative[layers], then blocks x 12 bytes: u16 sector, s16 sectors, s16 first column,
    s16 width, s16, s16 (block = sectors * 0x800 bytes from sector * 0x800)
  block (0x208f0): s16 first column, s16 width[layers] (padded to even count), then per layer
    width * rows u32 cells, column-major (cell (c, r) at c*rows + r)
  cell u32 (0x20b50, 0x20d30, 0x2f840, 0x2df20): bit0 mirror, bits1-10 block type,
    bits11-23 camera index (T54), bits24-30 attribute (T40)
  block type t (T34 entry at (t-1)*4): s16 mesh word offset, s16 collision byte offset (from +2)
    mesh (entry k = t + s16): s16 kind, u8 vertices, u8 quads, vertices u32 (x s8, y s16, z s8),
    u32 (skipped), quads 8 bytes: u32 = 4 x 6-bit vertex index | flags<<24, u32 = T30 offset |
    depth<<22. flags 0x80 = flat colour (low byte of q1), 0x20 = semi-transparent.
  collision profile (0x20990): u8 class (x low nibble, z high nibble; segments 2^c), samples:
    0x80 none, 0x40|v flat height 2v, else interpolate (v & 0x3f)*2 .. next sample.

  palette file MAP_PSX.CIF (EXE 0x68c0, loaded by GFX_LoadLevelPalette 0x5760 as map_psx + "cif"):
    256 x RGB (entries 0 and 255 forced to black/white by GFX_SetPaletteEntry 0x6780),
    0x40000 four 256x256 blend tables, 0x10000 shade table shade[brightness][index] (EXE 0x40dd70),
    0x1f00 = 31 x 256 fog/depth remap rows. The PC rasteriser draws 8-bit: a level texel t at
    depth level L (CLUT field 0x7830 + L*0x40) shows palette[fog[L][t]].

  spawns (ENG1 0x26040): section +8 = count, section +10 (s16, relative to +10) -> count x
    {u16 x, u16 y, u16 z, u16 template}; templates come from ENG1's per-level table 0x68af0
    (0x48 bytes per level id, 999 ends; +0x40 -> template table, 10 bytes: s16 dx, dy, dz,
    u16 flags (bit 0x200 = re-placed on reset), s16 kind). Object position = (x+dx, y+dy, z+dz).

  models (T34 entries after the block types, GFX_DrawModel 0x31540): see decode_models.

    level_decode.py [--out DIR] [index ...]   (default: every i2.5 map -> extracted/levels)
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

import pefile
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "manifests" / "archive-manifest.json"
OUT = ROOT / "extracted" / "levels"


def u16(d, o): return struct.unpack_from("<H", d, o)[0]
def s16(d, o): return struct.unpack_from("<h", d, o)[0]
def u32(d, o): return struct.unpack_from("<I", d, o)[0]
def rel(d, o): return o + (u32(d, o) & ~3)


def vram_image(d: bytes, img0: int, img1: int) -> Image.Image:
    """1024x512 16-bit VRAM as RGB (PSX BGR555), from the two uploads in 0x1fc90. LVL_LoadMap reads only
    header-size bytes (+4), so image bytes past it are not file data at runtime (S2.03): they stay 0 here."""
    d = d[:u32(d, 4)]
    vram = bytearray(1024 * 512 * 2)
    for y in range(512):
        row = d[img0 + y * 1536:img0 + (y + 1) * 1536]
        vram[y * 2048:y * 2048 + len(row)] = row
    for y in range(256):
        o = (256 + y) * 2048 + 768 * 2
        row = d[img1 + y * 512:img1 + (y + 1) * 512]
        vram[o:o + len(row)] = row
    return bytes(vram)


def vram_png(vram: bytes) -> Image.Image:
    px = struct.unpack(f"<{1024 * 512}H", vram)
    rgb = bytearray(1024 * 512 * 3)
    for i, c in enumerate(px):
        r, g, b = c & 31, (c >> 5) & 31, (c >> 10) & 31
        rgb[i * 3:i * 3 + 3] = bytes(((r << 3) | (r >> 2), (g << 3) | (g >> 2), (b << 3) | (b >> 2)))
    return Image.frombytes("RGB", (1024, 512), bytes(rgb))


def decode_mesh(d: bytes, t34: int, t: int) -> dict | None:
    e = t34 + (t - 1) * 4
    off = s16(d, e)
    if off == 0:
        return None
    k = t + off
    m = t34 + (k - 1) * 4
    kind, nv, nq = s16(d, m), d[m + 2], d[m + 3]
    verts = []
    for i in range(nv):
        v = u32(d, m + 4 + 4 * i)
        x = struct.unpack("<b", bytes((v & 0xFF,)))[0]
        y = struct.unpack("<h", struct.pack("<H", (v >> 8) & 0xFFFF))[0]
        z = struct.unpack("<b", bytes((v >> 24,)))[0]
        verts.append([x, y, z])
    quads = []
    q = m + 4 * nv + 8
    for i in range(nq):
        q0, q1 = u32(d, q + 8 * i), u32(d, q + 8 * i + 4)
        quads.append({"v": [q0 & 63, (q0 >> 6) & 63, (q0 >> 12) & 63, (q0 >> 18) & 63], "flags": q0 >> 24,
                      "tex": q1 & 0xFFFF, "depth": q1 >> 22, "colour": q1 & 0xFF})
    return {"entry": k, "kind": kind, "verts": verts, "quads": quads}


CLASS = [(4, 63, 4, 1), (8, 31, 8, 2), (16, 15, 16, 4), (32, 7, 32, 8), (64, 3, 64, 16)]  # ENG1 0x5e0d0


def surface(raw: bytes, x: int, z: int) -> int:
    """Port of ENG1 0x20990: surface height (0..126 below the cell top) at x, z in 0..63; -1 = none.
    raw = profile bytes starting at the class byte."""
    def val(b0, b1, frac_mask, frac_mul, pos):
        if b0 & 0x80:
            return -1
        if b0 & 0x40:
            return (b0 & 0x3F) << 1
        return ((((b1 & 0x3F) * 2 - (b0 & 0x7FFF) * 2) * ((pos & frac_mask) * frac_mul)) >> 8) + (b0 & 0x7FFF) * 2
    c = raw[0]
    p = raw[1:]
    if c == 0:
        b0 = p[0]
        if b0 & 0x80:
            return -1
        if b0 & 0x40:
            return (b0 & 0x3F) << 1
        return ((((p[1] & 0x3F) * 2 - b0 * 2) * ((x & CLASS[0][1]) * CLASS[0][2])) >> 8) + b0 * 2
    cx = c & 15
    if c >> 4:
        cz = (c >> 4) - 1
        mx, mskx, fx, sx = CLASS[cx]
        mz, mskz, fz, _ = CLASS[cz]
        base = ((mz * z) >> 8) * (sx + 1) + ((mx * x) >> 8)
        b0 = p[base]
        if b0 & 0x80:
            return -1
        if b0 & 0x40:
            return (b0 & 0x3F) << 1
        h0 = val(b0, p[base + 1], mskx, fx, x)
        n0 = p[base + sx + 1]
        h1 = ((((p[base + sx + 2] & 0x3F) * 2 - (n0 & 0x3F) * 2) * ((x & mskx) * fx)) >> 8) + (n0 & 0x3F) * 2
        return ((((z & mskz) * fz) * (h1 - h0)) >> 8) + h0
    mx, mskx, fx, _ = CLASS[cx]
    i = (mx * x) >> 8
    return val(p[i], p[i + 1], mskx, fx, x)


def decode_collision(d: bytes, t34: int, t: int) -> dict | None:
    e = t34 + (t - 1) * 4 + 2
    off = s16(d, e)
    if off == 0:
        return None
    p = e + off
    cls = d[p]
    cx, cz = cls & 15, cls >> 4
    raw = d[p:p + 1 + 17 * 17 + 2]
    heights = [[surface(raw, x, z) for x in range(0, 64, 8)] + [surface(raw, 63, z)] for z in (0, 32, 63)]
    if cls == 0:
        return {"class": 0, "samples": [d[p + 1], d[p + 2]], "heights_z0_32_63": heights}
    nx = (1 << cx) + 1 if cx < 5 else 2
    nz = (1 << (cz - 1)) + 1 if cz else 1
    return {"class": cls, "x_segments": 1 << cx if cx < 5 else None, "z_class": cz,
            "samples": list(d[p + 1:p + 1 + nx * nz]), "nx": nx, "nz": nz, "heights_z0_32_63": heights}


def unpack_vertex(v: int) -> list[int]:
    return [struct.unpack("<b", bytes((v & 0xFF,)))[0], struct.unpack("<h", struct.pack("<H", (v >> 8) & 0xFFFF))[0],
            struct.unpack("<b", bytes((v >> 24,)))[0]]


def decode_models(d: bytes, h: dict) -> list[dict]:
    """T34 entries n_types..n_types+n_extra-1: self-relative s32 -> model, drawn by GFX_DrawModel 0x31540
    (object frame field +0x34 = model k, 1-based; 0x313e0 / 0x31470).
    model: s16 T28 word offset, u8 n, u8 radius (visibility test 0x32050), u32 offset vector (0 = object
    matrix, else packed like a vertex and scaled by the object, 0x31d30), n x 8 bytes (not read by the draw),
    then per face group {s16 (the T28 word again), s16 count, u32 vertices[count]}: the draw reads the first
    count at +10 + 8n, the vertices from count + 2, and the next count 4 + 4*count later transformed into slots 0..count-1
    (slots above count keep the previous chunk's vertices; transform table 0x5cec0).
    T28 group: u16 quads, u8 more groups, u8 carried vertices; quads x 8 bytes {u32 v0|v1<<6|v2<<12|v3<<18|flags<<24,
    u32 T30 byte offset (high byte 0) or flat colour (low byte)}, carried slot bytes, padded to 4.
    Flags 0x08: back-face culled unless 0x01 (0x04: skipped while 0x6ffbc == 0); 0x40: test the 2nd triangle."""
    t34, t28 = h["t34"], h["t28"]
    models = []
    for k in range(1, h["n_extra"] + 1):
        ent = t34 + (h["n_types"] - 1 + k) * 4
        p = ent + struct.unpack_from("<i", d, ent)[0]
        if not 0 < p < len(d) - 10:
            models.append({"k": k, "error": f"pointer 0x{p:x} out of file"})
            continue
        idx, n, radius, off = struct.unpack_from("<hBBI", d, p)
        if s16(d, p + 8 + 8 * n) != idx:   # every mesh repeats its T28 word before the first count
            models.append({"k": k, "t28_word": idx, "n": n, "radius": radius, "raw": d[p:p + 24].hex(),
                           "error": "no T28 word before the vertex count: not a GFX_DrawModel mesh (consumer open)"})
            continue
        q, g = p + 10 + 8 * n, t28 + idx * 4
        slots: list[list[int] | None] = [None] * 64
        groups = []
        for _ in range(64):
            cnt = s16(d, q)
            if not 0 <= cnt <= 64:
                groups.append({"error": f"vertex count {cnt} at 0x{q:x}"})
                break
            for i in range(cnt):
                slots[i] = unpack_vertex(u32(d, q + 2 + 4 * i))
            q += 4 + 4 * cnt
            nq, more, carry = struct.unpack_from("<HBB", d, g)
            if nq > 256:
                groups.append({"error": f"T28 group at 0x{g:x} has {nq} quads"})
                break
            quads = []
            for i in range(nq):
                q0, q1 = u32(d, g + 4 + 8 * i), u32(d, g + 8 + 8 * i)
                vi = [q0 & 63, (q0 >> 6) & 63, (q0 >> 12) & 63, (q0 >> 18) & 63]
                quad = {"v": vi, "flags": q0 >> 24, "p": [slots[j] for j in vi]}
                if q1 >> 24:
                    quad["colour"] = q1 & 0xFF
                else:
                    quad["tex"] = q1
                quads.append(quad)
            groups.append({"vertices": cnt, "quads": quads,
                           "carry": list(d[g + 4 + 8 * nq:g + 4 + 8 * nq + carry])})
            g = (g + 4 + 8 * nq + carry + 3) & ~3
            if more == 0:
                break
        models.append({"k": k, "t28_word": idx, "n": n, "radius": radius,
                       "offset": unpack_vertex(off) if off else None,
                       "extra": d[p + 8:p + 8 + 8 * n].hex(), "groups": groups})
    return models


def texture(d: bytes, t30: int, off: int) -> dict:
    a, b, c = struct.unpack_from("<3I", d, t30 + off)
    return {"uv": [[a & 0xFF, (a >> 8) & 0xFF], [b & 0xFF, (b >> 8) & 0xFF], [c & 0xFF, (c >> 8) & 0xFF],
                   [(c >> 16) & 0xFF, (c >> 24) & 0xFF]], "clut": a >> 16, "tpage": b >> 16}


LEVEL_IDS = {"TRAINING": 1, "GAUNTLET": 2, "FOREST": 3, "THEBES": 5, "HYDRA": 6, "MEDUSA": 7, "CYCLOPS": 8,
             "TITANS": 9, "TORMENT": 10, "VORTEX": 11}  # GFX_LoadLevelPalette 0x5760 switch


def eng1_templates(level_id: int):
    pe = pefile.PE(str(ROOT / "backups" / "originals" / "ENG1.DLL"), fast_load=True)
    img = pe.get_memory_mapped_image()
    rd = lambda va, n: img[va - 0x10000000:va - 0x10000000 + n]
    va = 0x10068AF0
    while True:
        lid = struct.unpack("<h", rd(va, 2))[0]
        if lid == 999:
            return None
        if lid == level_id:
            t = struct.unpack("<I", rd(va + 0x40, 4))[0]
            return lambda i: struct.unpack("<hhhHh", rd(t + 10 * i, 10))
        va += 0x48


def decode(d: bytes, name: str) -> dict:
    tag = d[:4].decode("latin-1")
    if tag != "i2.5":
        raise ValueError(f"unsupported map tag {tag}")
    h = {"tag": tag, "header_size": u32(d, 4), "sections": u16(d, 8), "n_e": u16(d, 0xA), "n_types": u16(d, 0xC),
         "n_extra": u16(d, 0xE), "n_anim": u16(d, 0x10), "n_scroll": u16(d, 0x12),
         "img0": rel(d, 0x14), "img1": rel(d, 0x18), "x1c": 0x1C + struct.unpack_from("<i", d, 0x1C)[0],
         "x20": u32(d, 0x20), "t28": rel(d, 0x24), "t30": rel(d, 0x28), "colour_2c": d[0x2C:0x2F].hex()}
    ns = h["sections"]
    secs = [rel(d, 0x30 + 4 * i) for i in range(ns)]
    p = 0x30 + 4 * ns
    h.update({"t58": rel(d, p), "t54": rel(d, p + 4), "t50": rel(d, p + 8), "t44": rel(d, p + 12), "t34": p + 16})
    t34 = h["t34"]
    sections = []
    for s in secs:
        layers, nblk = d[s], d[s + 2]
        cols, rows = u16(d, s + 4), u16(d, s + 6)
        cum = [u32(d, s + 12 + 4 * i) for i in range(layers)]
        bt = s + 12 + 4 * layers
        blocks = []
        grid = [[[0] * rows for _ in range(cols)] for _ in range(layers)]
        for i in range(nblk):
            sector, nsec, first, width, b4, b5 = struct.unpack_from("<Hhhhhh", d, bt + 12 * i)
            bo = sector * 0x800
            blk = d[bo:bo + nsec * 0x800]
            bfirst = s16(blk, 0)
            widths = [s16(blk, 2 + 2 * l) for l in range(layers)]
            pos = ((layers + 2) & ~1) * 2
            for l in range(layers):
                for c in range(widths[l]):
                    for r in range(rows):
                        col = bfirst + c
                        if 0 <= col < cols:
                            grid[l][col][r] = u32(blk, pos + (c * rows + r) * 4)
                pos += widths[l] * rows * 4
            blocks.append({"sector": sector, "sectors": nsec, "first_column": first, "width": width,
                           "f8": b4, "f10": b5, "block_first": bfirst, "layer_widths": widths})
        nsp, sp = u16(d, s + 8), s + 10 + s16(d, s + 10)
        tmpl = eng1_templates(LEVEL_IDS.get(name, -1))
        spawns = []
        for i in range(nsp):
            x, y, z, ti = struct.unpack_from("<HHHH", d, sp + 8 * i)
            ent = {"x": x, "y": y, "z": z, "template": ti}
            if tmpl:
                dx, dy, dz, fl, kind = tmpl(ti)
                ent.update({"wx": x + dx, "wy": y + dy, "wz": z + dz, "flags": fl, "kind": kind})
            spawns.append(ent)
        sections.append({"offset": s, "layers": layers, "spawns": spawns, "byte1": d[s + 1], "blocks_n": nblk, "byte3": d[s + 3],
                         "columns": cols, "rows": rows, "u8": u16(d, s + 8), "rel10": s16(d, s + 10),
                         "cumulative": cum, "blocks": blocks, "grid": grid})
    used = sorted({(c >> 1) & 0x3FF for sec in sections for lay in sec["grid"] for col in lay for c in col} - {0})
    types = {}
    for t in used:
        types[t] = {"mesh": decode_mesh(d, t34, t), "collision": decode_collision(d, t34, t)}
    models = decode_models(d, h)
    tex_offs = sorted({q["tex"] for ty in types.values() if ty["mesh"] for q in ty["mesh"]["quads"] if not q["flags"] & 0x80}
                      | {q["tex"] for mo in models for gr in mo.get("groups", []) for q in gr.get("quads", [])
                         if "tex" in q and h["t30"] + q["tex"] + 12 <= len(d)})
    textures = {o: texture(d, h["t30"], o) for o in tex_offs}
    cams_used = sorted({(c >> 11) & 0x1FFF for sec in sections for lay in sec["grid"] for col in lay for c in col})
    cameras = {}
    for ci in cams_used:
        rec = u16(d, h["t54"] + ci * 4)
        r = h["t58"] + rec * 16
        x, z, w2 = struct.unpack_from("<iiI", d, r)
        cameras[ci] = {"record": rec, "flags": u16(d, h["t54"] + ci * 4 + 2), "x": x, "z": z,
                       "angle": w2 & 0xFFF, "w2_hi": w2 >> 24, "anchor": u16(d, r + 12),
                       "type": u16(d, r + 14) >> 14, "start_column": u16(d, r + 14) & 0x3FFF}
    # T50 (S2.02): corner light levels {u8 x0y0, x1y0, x0y1, x1y1}, index = T54 flags >> 4 when flags & 8 == 0
    # (else fixed level (flags >> 6) & 0xff); level -> colour 0x6f4e8[(flags & 7) * 64 + level] (0x2f340, 0x311a0)
    lights = [list(d[h["t50"] + 4 * i:h["t50"] + 4 * i + 4]) for i in range((h["t44"] - h["t50"]) // 4)]         if h["t44"] > h["t50"] else []
    return {"name": name, "header": h, "sections": sections, "types": types, "textures": textures, "cameras": cameras,
            "models": models, "lights": lights}


def main(argv: list[str]) -> int:
    entries = json.loads(MANIFEST.read_text(encoding="utf-8"))["entries"]
    maps = [e for e in entries if (e.get("name_candidate") or "").upper().endswith("MAP_PSX.BIN")]
    out_root = OUT
    if "--out" in argv:                                        # full-game: extracted/full-game/levels
        i = argv.index("--out")
        out_root = Path(argv[i + 1])
        argv = argv[:i] + argv[i + 2:]
    if len(argv) > 1:
        maps = [e for e in maps if e["index"] in {int(x) for x in argv[1:]}]
    ok = bad = 0
    for e in maps:
        name = e["name_candidate"].replace("\\", "/").split("/")[-2]
        d = (ROOT / e["file"]).read_bytes()
        try:
            lvl = decode(d, name)
        except Exception as ex:  # noqa: BLE001
            print(f"{e['index']:04d} {name}: {ex}")
            bad += 1
            continue
        h = lvl["header"]
        hs = h["header_size"]
        short = max(0, h["img0"] + 512 * 1536 - hs) + max(0, h["img1"] + 256 * 512 - hs)
        if short:   # S2.03: runtime VRAM there is whatever followed the header buffer; no texture samples it
            lvl["notes"] = [f"VRAM image runs {short} bytes past the map header (0x{hs:x}); LVL_LoadMap does not load "
                            f"them (runtime contents undefined, unused by textures; 0 here)"]
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
        (out / "level.json").write_text(json.dumps(lvl), encoding="utf-8")
        s = lvl["sections"][0]
        print(f"{e['index']:04d} {name}: sections={len(lvl['sections'])} layers={s['layers']} cols={s['columns']} "
              f"rows={s['rows']} blocks={s['blocks_n']} types={len(lvl['types'])} textures={len(lvl['textures'])} "
              f"cameras={len(lvl['cameras'])}")
        ok += 1
    print(f"decoded={ok} failed={bad} -> {out_root}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
