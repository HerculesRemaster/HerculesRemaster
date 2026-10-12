# Level format (MAP_PSX.BIN / MAP_PSX.CIF)

Levels are PSX-style 2.5D scenes. Each section is a grid of reusable textured 3D blocks (64 × 64 world units per cell, 16 rows) along a camera path that can be straight or arced. Collision is a heightfield per block type, and objects come from a spawn list. Decoder: `tools/level/level_decode.py` → `extracted/levels/<LEVEL>/{level.json, vram.bin, vram.png, palette.json, fog.bin, blend.bin, shade.bin}`; preview `tools/level/render_preview.py`; viewer `tools/viewer/level_viewer/build.py` → `viewer.html`. ENG1.DLL RVAs; names in `analysis/annotations/p5.json`.

## Which engine

ENG1's per-level table (0x68af0, 0x48 bytes per level id, 999 ends) lists level ids 0, 1 TRAINING, 3 FOREST, 5 THEBES, 6 HYDRA, 7 MEDUSA, 9 TITANS and 11 VORTEX. These maps have tag `i2.5`. The `i1.a` maps (GAUNTLET, CYCLOPS, TORMENT) belong to ENG3 (Q-07), and `i1.3`/`i1.9` (unnamed) are still open. `LVL_LoadMap` (0x1f750) refuses a map whose tag differs from the engine's (game state +0x8c): it hangs in an endless loop.

## File layout (`i2.5`)

`rel` fields are self-relative: target = field offset + (u32 & ~3).

| offset | field | code |
|---|---|---|
| 0x00 | tag `i2.5` | 0x1f750 |
| 0x04 | header size = first block's sector × 0x800 (TRAINING 0x103000) | 0x1f750 reads 8 bytes, then this many. The VRAM uploads (0x1fc90) still copy full rects, so in FOREST, VORTEX, MEDUSA and HYDRA the tail of the 256×256 image comes from memory past the header (runtime-checked on HYDRA; no texture samples it) |
| 0x08 | u16 sections | `LVL_ParseHeader` 0x20670 |
| 0x0a | u16 n_e (table T40 follows T4c by n_e entries) | |
| 0x0c, 0x0e | u16 counts of T34 entries (block types + extra records) | |
| 0x10 | u16 animated VRAM rects (T38, `LVL_AnimateTextures` 0x2cb0) | |
| 0x12 | u16 scrolling VRAM rects (T3c, `LVL_ScrollTexture` 0x2d80) | |
| 0x14 | rel → VRAM image 768 × 512 units, uploaded to (0,0) | `LVL_UploadVram` 0x1fc90 |
| 0x18 | rel → VRAM image 256 × 256 units, uploaded to (768,256) | |
| 0x1c | s32 relative: end of the kept header (the block is shrunk with `RT_ReMalloc` after the upload) | 0x1fc90, 0x1f750 |
| 0x24 | rel → T28 3D models (`GFX_DrawModel` 0x31540) | |
| 0x28 | rel → T30 texture words (3 × u32 per quad: `uv0 \| clut<<16`, `uv1 \| tpage<<16`, `uv2 \| uv3<<16`) | |
| 0x2c | 3 bytes (stored at 0x70cf0) | |
| 0x30 | rel × sections → section descriptors | |
| then | rel × 4 → T58 camera records (16 bytes), T54 camera index (u32), T50 shade table, T44 | |
| then | T34 u32 table (block types); T38, T3c, T4c, T40 follow by the counts above | |
| from header size | geometry blocks of 0x800-byte sectors, streamed by camera column (`LVL_StreamBlocks` 0x1f870) | |

### Section descriptor (`LVL_SelectSection` 0x20830)

`u8 layers, u8, u8 blocks, u8, u16 columns, u16 rows (16), u16 spawns, s16 spawn list (relative to +10), u32 × layers, then blocks × 12 bytes {u16 sector, s16 sectors, s16 first column, s16 width, s16, s16}`. TRAINING has 5 layers, 500 columns, 16 rows and 7 blocks; the last block ends exactly at the file length.

### Block (`LVL_SetBlockLayers` 0x208f0)

`s16 first column, s16 width × layers` (padded to an even count), then per layer `width × rows` u32 cells, column-major (cell (c, r) at index c·rows + r). A layer is a 128-unit depth slice: the layer index is `(z + 0x40) >> 7`.

### Cell u32

| bits | meaning | code |
|---|---|---|
| 0 | mirror horizontally | 0x20b50, 0x2ed10 |
| 1–10 | block type → T34 (mesh + collision) | 0x2df20, 0x20b50 |
| 11–23 | camera index → T54 → camera record in T58 | 0x2f840, 0x32300 |
| 24–30 | attribute → T40 | 0x20d30 |

### Block type t (T34 entry at (t−1)·4)

- `s16 mesh` (word offset): mesh entry k = t + mesh at T34 + (k−1)·4 holds `s16 kind, u8 vertices, u8 quads`, then:
  - vertices u32 = `x s8 | y s16 << 8 | z s8 << 24` (mirror: x' = 64 − x);
  - one u32 skipped;
  - quads × 8 bytes: `u32 = v0 | v1<<6 | v2<<12 | v3<<18 | flags<<24`, `u32 = T30 offset | depth<<22`.
  - Flags: 0x80 = flat colour (low byte of the 2nd word), 0x20 = semi-transparent.
  - `LVL_DrawCellMesh` 0x2df20 ORs the CLUT field `0x7830 + level·0x40` into the texture words, where level = depth fog step.
- `s16 collision` (byte offset from the field) → profile (`LVL_ProfileHeight` 0x20990):
  - u8 class: x classes in the low nibble, z classes in the high nibble (minus 1); class c splits the cell into 2^c segments (table 0x5e0d0).
  - Then samples: 0x80 = no surface; 0x40|v = flat height 2v; otherwise linear interpolation to the next sample.

### Positions

- A vertex in cell (c, r) is at world `(c·64 + x, r·64 + y)` along the unrolled path (`LVL_CellSetupStraight` 0x2e3b0).
- A collision surface is at `y = r·64 + 32 − h` (`LVL_FindSurface` 0x20b50). Check: TRAINING row-13 ground meshes have y = −32 (world 800), and their profiles give h = 64 (world 800).

### Camera record (T58, 16 bytes; `LVL_UpdateCamera` 0x2f840, `LVL_PlaceOnPath` 0x32300)

`s32 x, s32 z, u32 (angle & 0xfff, bits 24+ second angle), u16 (index into T58), u16 (type << 14 | start column)`. Type 0 = straight, 1/2 = arc (radius 0x28c). Cells carry their camera index, so the path is defined per column.

### Spawns (`LVL_SpawnObjects` 0x26040)

- Section spawn list: `{u16 x, u16 y, u16 z, u16 template}` (8 bytes).
- Template table from ENG1's level config (`LVL_SelectLevelConfig` 0x280f0: record +0x40): `{s16 dx, dy, dz, u16 flags, s16 kind}`. Object = (x+dx, y+dy, z+dz), kind.
- Flag 0x200 = re-placed on reset. Kinds 0x2018/0x2019 are counted into game state +0x1a/+0x16.
- **Class = kind & 0x7000** (`LVL_SpawnVisible` 0x262a0 / 0x26480): 0 actors, 0x4000 markers (checkpoints), 0x2000 pickups.
  - 0x1000-class kinds are not objects. They are **bank-streaming triggers** fired 120–180 units before the camera reaches them (0x28290): 0x10XX request bank XX, 0x18XX free it, 0x14XX/0x1CXX request/free the HERCULES letter banks of bitmask XX.
  - Lists per map: `animation-coverage.json` `eng1_map_triggers`.
- TRAINING: 451 spawns; most common kinds 0x2019 (154), 0x2018 (60), 0x38, 0x2013.

## Palette file MAP_PSX.CIF (`GFX_LoadCif` EXE 0x68c0; `GFX_LoadLevelPalette` 0x5760)

| offset | size | content |
|---|---|---|
| 0 | 768 | 256 × RGB (`GFX_SetPaletteEntry` 0x6780 forces entries 0 and 255 to black and white) |
| 768 | 0x40000 | four 256 × 256 blend tables (semi-transparency) |
| 0x40300 | 0x10000 | shade table `shade[brightness][index]` (256 × 256; used for polygons whose brightness byte ≠ 0x80, EXE 0x40dd70; table at 0x91c488) |
| 0x50300 | 0x1f00 | 31 × 256 fog/depth remap rows |

The PC rasteriser draws into a 320×240 8-bit frame. A level texel t at fog level L shows `palette[fog[L][t]]`; a texel of index 0 is transparent. Runtime VRAM dumps (`HERC_HOOK_VRAM_TICKS`, unswizzled with the 0xa710 layout) equal the file's VRAM images.

## Preview

`render_preview.py TRAINING --cols 0:500 --collision` renders 25,049 textured quads in 5 layers: trees, training rigs and ground match the game. `preview_collision.png` adds collision lines at mid-depth per layer and spawn markers. `viewer.html` toggles layers and the overlay.

## Models (T28; S2.01)

`GFX_DrawModel` 0x31540 draws an object whose frame field (+0x34) is a model number k:
- `GFX_DrawObjectModel` 0x313e0 handles k = 1..n_extra (header +0x0e).
- Model k is T34 entry `n_types − 1 + k`, a self-relative s32 pointing to the model.
- Decoded by `level_decode.decode_models` into `level.json` `models`.
- Contact sheets: `tools/level/render_models.py` → `extracted/full-game/levels/<LEVEL>/models.png`.

| part | layout | code |
|---|---|---|
| header `LVL_ModelHeader` | `s16 T28 word, u8 n, u8 radius, u32 offset` | radius → visibility test 0x32050; offset ≠ 0 → `GFX_SetModelOffset` 0x31d30 (packed like a vertex, × object scale >> 7) |
| extra | `n × 8` bytes | not read by the draw (open) |
| per face group | `s16 (the T28 word again), s16 count, u32 vertices[count]` | the draw reads the first count at +10 + 8n; the next count follows 4 + 4·count later |
| vertex | packed like block vertices (x s8, y s16, z s8) | `GFX_TransformModelVerts` 0x318d0 (table 0x5cec0 by object +0x23) puts vertex i in scratch slot i; slots above count keep the previous group's vertices |
| T28 group `LVL_ModelGroup` (at T28 + word·4) | `u16 quads, u8 more groups, u8 carried`, then quads, then `carried` slot bytes (copied to the attach list 0x70840 for child objects), padded to 4 | 0x31540 |
| quad `LVL_ModelQuad` | `u32 = i0 \| i1<<6 \| i2<<12 \| i3<<18 \| flags<<24`; `u32` = T30 byte offset (high byte 0) or flat colour (low byte) | packet xy0..3 = i0, i1, i3, i2 |

- **Flags:**
  - 0x08: back-face culled (NCLIP of i0, i1, i3), unless bit 0x01 is set.
  - 0x04: skipped while 0x6ffbc = 0.
  - 0x40: also test the triangle (i1, i2, i3).
- **Depth:** OT index = (z0 + z2)/8. When object +0x54 & 0x40, it is the mean of all four z values / 16. Clamped to 0x4bf.
- **CLUT:** `0x7830 + fog·0x40`, where fog = clamp(object +0x10 >> 5, 1, 29).

**Counts:**

| level | models | quads |
|---|---:|---:|
| HYDRA | 78 | 7,472 |
| TRAINING | 89 | 1,918 |
| TITANS | 18 | 823 |
| FOREST | 23 | 478 |
| THEBES | 27 | 399 |
| MEDUSA | 16 | 375 |
| VORTEX | 1 | 1 |

- **HYDRA's** models are mostly Hydra head and neck poses, one model per animation frame, plus boulders and platforms.
- **TRAINING** has 8 records (k 42–46, 49, 50, 52) that do not repeat their T28 word before the count. They are not in this mesh layout; their consumer is open.

## Tables T50, T44, T4c, T40 and section bytes (S2.02)

| table | consumer | layout / meaning | status |
|---|---|---|---|
| T50 | `LVL_TransformCellStraight`, 0x2f340 (arc cells), 0x311a0 (object colour) | u32 per entry = four corner light levels `{u8 x0y0, x1y0, x0y1, x1y1}`. Bilinear over the cell (x, y & 0x3f), then colour = 0x6f4e8[(flags & 7)·64 + level] (64 levels × 8 light palettes). Index = T54 camera flags >> 4 when flags & 8 = 0; with flags & 8 the level is fixed at (flags >> 6) & 0xff. Objects use their own +0x2c the same way, bilinear when +0x54 & 0x600 = 0x600 | VERIFIED-STATIC; entry count = max index + 1 in all seven maps; exported as `level.json` `lights` |
| T44 | `LVL_DrawBackgroundTiles` 0x2ff0, called by the per-level background hooks | parallax background tile map `{s16 width, s16 height, s16 tile[width·height]}`. Each 16×16 tile is drawn as a sprite (packet code 0x64): u = tile & 0xf8, v = (tile >> 8) & 0xf8, mode = tile & 3 (flip variants), bit 8 selects one of two OT slots (0x88/0x8c). Horizontally adjacent tiles with consecutive u are merged (up to 21). The scroll is the camera position divided per level (see below). texture pages 0x9c/0x9e = VRAM (768,256)/(896,256) by tile bit 8 (draw modes set in 0x1f320); tile & 3 = 2× horizontal (bit 0) / vertical (bit 1) magnification (FT4 branch); CLUT word 0x73480 (no static writer; colours = palette[texel]) | VERIFIED-STATIC; rendered by `tools/level/render_background.py` |
| T4c | 0x20ea0, called by the camera update 0x2fb70 when camera mode 0x70d10 & 0x80, and by 0x37770, 0x3af20, 0x3c1e0, 0x3c330 | n_e (header +0x0a) self-relative lists: `u16 count`, then 16-byte keys `{u16 key, …}` interpolated by a column/position key into 7 shorts (camera/scroll limits). n_e = 0 in every ENG1 map except TITANS (2) | layout VERIFIED-STATIC; field meanings open |
| T40 | `LVL_CellAttribute` 0x20d30 | attribute a (cell bits 24–30) → T40 entry a−1, self-relative → `u16 classes` (x low nibble, y bits 4–7, z bits 8+; segment table 0x5e0d0, as for collision), then an s8 grid sampled at (x & 0x3f, y + 0x20 & 0x3f, depth). The value goes to the per-level `attribute_flags` (+0x44) / `attribute_funcs` (+0x38) tables (`extracted/meta/full-game/eng1-level-config.json`, 0x2ff50) | VERIFIED-STATIC |
| section byte 1, byte 3 | none | `LVL_SelectSection` and every other reader of the section table (0x19c30, 0x19d30, `HERO_AirPhysics`, `LVL_SpawnObjects`) read only +0, +2, +4, +6, +8, +10 | unread by ENG1 |

### Background hooks (S2.04)

ENG1 0x5df00 + 0x1c·level holds a per-level byte/flag record. Its +0x14 field is a background function (stored to 0x70d24 by 0x1f320, called at the end of the cell draw 0x2da90):

| level | hook | what it draws |
|---|---|---|
| TRAINING | `LVL_DrawBackgroundTraining` 0x42320 | T44 at (camera x / 4, (camera y − 0x250) / 3); clear colour (0x3f,0x75,0xbc) below y 32, else (0x7f,0x8b,0x47) (0x5a30) |
| FOREST | `LVL_DrawBackgroundForest` 0xe400 | T44 at (x / 4, y / 4 − 0x30); colour (0xa3,0xa7,0x57) above 0x50, else (0x57,0xab,0xf3) |
| THEBES | `LVL_DrawBackgroundThebes` 0x36c40 | colour (0xa1,0xbc,0xbb) below 0x50, else (0x11,0x1e,0x39); T44 at (x / 4, (y − 0x250) / 5) |
| HYDRA | `LVL_DrawBackgroundHydra` 0x1efa0 | two path-placed sprites at path x 0xb80 and 0x580 (0x1efe0, 0x50 × 0xcc) |
| TITANS | `LVL_DrawBackgroundTitans` 0x3c600 | its own scrolling strips (0x3c980) |
| VORTEX | `LVL_DrawBackgroundVortex` 0x2fa0 | T44 at (x / 8, y / 4, −0x40 in level 11) |
| MEDUSA | none | |

## Open

- Boss models are posed by code each tick and are not placed in the static previews (S2.04). Arcs are drawn unrolled.
- Model extra bytes (n × 8) and TRAINING's 8 non-mesh records.
- T4c key fields (TITANS only); the per-level attribute value meanings (+0x44 flags per value).
- ENG3 `i1.a` maps (S3). The `i1.3`/`i1.9` entries (631–638, 409) are unreferenced by this build (`docs/formats/i-family.md`).
- ENG1 level ids 0 (`O:\PLAYROOM\`) and 4 (`O:\NESSUS\`) have config, start and bank lists (ENG1 0x5a690) but no `MAP_PSX.BIN` in HERCULES.FS (neither `M:\GRAFIX\MAPS\…` path hashes to an archive id), so they cannot load.
