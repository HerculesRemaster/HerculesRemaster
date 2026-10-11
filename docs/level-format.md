# Level format (MAP_PSX.BIN / MAP_PSX.CIF)

Levels are PSX-style 2.5D scenes. Each section is a grid of reusable textured 3D blocks (64 × 64 world units per cell, 16 rows) along a camera path that can be straight or arced. Collision is a heightfield per block type, and objects come from a spawn list. Decoder: `tools/level/level_decode.py` → `extracted/levels/<LEVEL>/{level.json, vram.bin, vram.png, palette.json, fog.bin, blend.bin, shade.bin}`; preview `tools/level/render_preview.py`; viewer `tools/viewer/level_viewer/build.py` → `viewer.html`. ENG1.DLL RVAs; names in `analysis/annotations/p5.json`.

## Which engine

ENG1's per-level table (0x68af0, 0x48 bytes per level id, 999 ends) lists level ids 0, 1 TRAINING, 3 FOREST, 5 THEBES, 6 HYDRA, 7 MEDUSA, 9 TITANS and 11 VORTEX. These maps have tag `i2.5`. The `i1.a` maps (GAUNTLET, CYCLOPS, TORMENT) belong to ENG3 (Q-07), and `i1.3`/`i1.9` (unnamed) are still open. `LVL_LoadMap` (0x1f750) refuses a map whose tag differs from the engine's (game state +0x8c): it hangs in an endless loop.

## File layout (`i2.5`)

`rel` fields are self-relative: target = field offset + (u32 & ~3).

| offset | field | code |
|---|---|---|
| 0x00 | tag `i2.5` | 0x1f750 |
| 0x04 | header size = first block's sector × 0x800 (TRAINING 0x103000) | 0x1f750 reads 8 bytes, then this many |
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

## Open

- T28 models / background (sky) and flat-colour quads are not drawn yet; arcs are drawn unrolled.
- Meaning of T44, T4c, T50 (shade), the attribute table T40 and section fields 1 and 3.
- ENG3 `i1.a` and the `i1.3`/`i1.9` maps.
