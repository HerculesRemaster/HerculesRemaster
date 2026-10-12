# ENG3 engine and `i1.a` maps (S3, in progress)

ENG3.DLL RVAs (base 0x10000000). Names: `analysis/annotations/full_game_s3.json` (+ 9 carried from ENG1 by `tools/ghidra/fn_match.py`). Levels: GAUNTLET (2), CYCLOPS (8), TORMENT (10). Everything here is VERIFIED-STATIC unless marked otherwise.

## Skeleton

| RVA | name | what |
|---|---|---|
| 0x1abb0 | `MOD_Tick` | first tick runs the init, the following ticks the update. **Init:** `LVL_AllocPools`, `LVL_LoadMap`, `GFX_LoadPanel2`, `LVL_InitLevel`, `HERO_DemoLoad`, then `LVL_StartSection`. **Per tick:** `HERO_DemoStep`, the update calls, and the draw calls (skipped while gs+4 = pause), then `ANIM_UploadCells`, frame counter gs+0x38 |
| 0x1a330 | `LVL_AllocPools` | three object pools: 0x134-byte objects (same size as ENG1), 0xec- and 0x94-byte objects; counts from DAT 0x40bcc/0x40c32/0x40bd2; per-level table 0x39860 |
| 0x1a930 | `LVL_LoadMap` | descriptor 0x38528[level]: +0 path, +4 = whole map size (no streaming, unlike ENG1). Reads the full MAP_PSX.BIN; tag check vs gs+0x8c (hangs on mismatch, as ENG1) |
| 0x1aa30 | (VRAM upload) | +0x18 image → VRAM (0,0) 768×512 units, +0x1c image → (768,256) 256×256; the map block is then shrunk to the header part |
| 0x1af10 | (header parse) | see below |
| 0x25060 | `GFX_LoadPanel2` | reads `m:\language\srclang\stpanel2.dat` (0x21000 bytes) and uploads it: the ENG3 HUD sheet (ENG1 uses STPANEL1.DAT) |
| 0x1b8f0 | `LVL_InitLevel` | clears the three pools (0x1ce10, 0x1d320, 0x1d860) and the 0x1f360 lists |
| 0x1a560 | `LVL_StartSection` | per (re)start: start record, banks for it (0x8340), level flags, `HERO_Init`, camera, VRAM regions |
| 0x1c720 | `HERO_Init` | hero (pool A slot 0, 0x42540) from the start record |
| 0x8da0 | `ANIM_DrawObjectFrame` | same code as ENG1 0x3cd0 (frame request; S3.06 hook point) |
| 0xae00 / 0xaf65 | `HERO_DemoLoad` / `HERO_DemoStep` | same code as ENG1 (demo format shared) |

## Level descriptor (0x38528, 0x18 bytes per level ID; only 2, 8, 10 filled)

| field | GAUNTLET | CYCLOPS | TORMENT | meaning |
|---|---|---|---|---|
| +0x00 | `O:\GAUNTLET\` | `O:\CYCLOPS\` | `O:\TORMENT\` | map folder |
| +0x04 | 0x129e28 | 0x125184 | 0x10903c | full map size (read at once) |
| +0x0c | 0x382c8 | 0x383a0 | 0x38450 | start records, 0x18 bytes each |
| +0x10 | 0x382b8 | 0x38388 | 0x38430 | level bank list (0xff-terminated bank indices into the ENG3 bank table 0x39548, 0x14 per bank): `[1]` = SRC3\HERO\HERCULES for all three |
| +0x14 | 0x382c0 | 0x38398 | 0x38440 | passed by `MOD_Tick` init through 0x1fa40 to EXE slot 0x06c `AUD_LoadLevelSonix` (ENG3 table copy 0x407a0 + 0x6c); same Sonix record as ENG1 (`tools/decode/level_sonix.py`): [0xff, music vol, sfx vol] [0xfe, x] music bank id, sfx bank ids, 0xff. GAUNTLET = vol 70/110, music `q:\GAUNTLET\`, sfx `q:\GAUNTLET\` + `q:\HERC_GAU\`; table in `docs/full-game/audio.md`. Runtime: the audio event from eng3 0x1ac63 in the password-13 run |
| +0x08 | 0 | 0 | 0 | always zero |

**Start record:**
- u16 x and u16 y, used as-is (ENG1 applies −32/+64);
- u8 z (depth << 23);
- **u8 section at +5**, which goes to hero+0x38 and DAT 0x40638;
- a pointer at +8 to the section bank list (0xff-terminated). `FUN_100089e0` frees every bank in neither list, `FUN_100084b0` loads the listed ones. GAUNTLET: ALIEN1, XTRA_DB1, LETTERS. CYCLOPS: PEOPLE, CYCLOPS, CYEND, XTRA_DB1, LETTERS. TORMENT: TORMENT, XTRA_DB1, LETTERS, SOULS;
- a u32 at +0x10 (0x01c00000 GAUNTLET, 0x00010000 CYCLOPS/TORMENT, meaning open).

| level | start records (x, y, section) |
|---|---|
| GAUNTLET | (32, 448, 0), (512, 448, 0…6), … (at least 8) |
| CYCLOPS | (0, 448, 0), (0, 448, 0…2), (1920, 448, 0) ×2 (6) |
| TORMENT | (0, 0, 0), (1024, 0, 0…4) (6) |

`HERO_Init` sets level-specific camera values (`DAT 0x40620` = 0xe8 for levels ≠ 2, 0xc4 for TORMENT plus 0x410e0 = 600, …).

The per-level table at 0x39860 (0x20 bytes per ID) is used by `LVL_AllocPools` (+0x18 count), `LVL_StartSection` (+0x10 bytes → 0x431d0, +0x14 flags bit 0/1 → map state +0x16/+0x18) and 0x20190 (+0x1c).

## `i1.a` header (`FUN_1001af10`)

| offset | field | GAUNTLET | CYCLOPS | TORMENT |
|---|---|---:|---:|---:|
| 0x00 | tag `i1.a` | | | |
| 0x04 | map size | 0x129e28 | 0x125184 | 0x10903c |
| 0x08 | u16 count n8 (→ 0x40654 array) | 962 | 509 | 391 |
| 0x0a | u16 blocks (rel pointers at +0x38; DAT 0x40787, max 6 kept in 0x40784) | 32 | 16 | 6 |
| 0x0c | u16 → 0x40786 | 11 | 4 | 5 |
| 0x0e | u16 nE (→ 0x40788; ≠ 0 forces 0x39558 = 3 in 0x8340) | 30 | 52 | 16 |
| 0x10 | u16 n10 | 0 | 1 | 2 |
| 0x12 | u16 n12 | 2 | 0 | 1 |
| 0x14 | u16 n14 | 8 | 9 | 5 |
| 0x18 / 0x1c | rel → VRAM images (as i2.5 +0x14/+0x18) | | | |
| 0x28 / 0x2c / 0x30 | rel tables → DAT 0x40650 (model faces: u32 = 4 × 6-bit vertex index + flags byte, `GFX_DrawModel` 0x23ef0) / 0x40658 (texture words) / 0x40674 (track lists, `LVL_FindSurface`) | | | |
| 0x34 | RGB colour → 0x40628 | 808080 | 808080 | 595959 |
| 0x38 | `blocks` rel pointers (bit 0 masked) → 0x40684; each block = u16 rows (12), u16 columns, then columns × rows u32 cells, **column-major** (cell (c, r) at c·12 + r, as in ENG1) | 32 × (12×128) | 16 × (12×256/255) | 6 × (12×240/150/255) |
| after | 4 rel pointers (→ 0x40680, 0x4067c, 0x40678, 0x4066c), then dword arrays of n8, nE, n10, n12, n14 entries (0x40654, 0x40660, 0x40664, 0x40670, 0x40668) | | | |

## Blocks and collision (`LVL_FindSurface` 0x1b2e0, `LVL_ProfileHeight` 0x1b100)

- **Grid:** a block is a column-major grid of 128-unit cells: index = (x >> 7)·12 + ((z + 0x300) >> 7). The 12 rows are depth slices. Rendered with `tools/level/eng3_decode.py` output, the dense blocks show continuous bands along x (track lanes).
- **Track lists:** table hdr+0x30 holds 0x10-byte records per track entry. At +6 a self-relative offset points to a byte list `{count, (block, height offset)×count}`. Only those blocks are tested.
- **Cell u32:**
  - bit 0 set = no surface;
  - bits 1–10 = collision profile index into the n8 array (each entry a self-relative offset to the profile);
  - bit 24 = mirror (sub-x → 63 − sub-x);
  - bits 24–31 = signed height (& ~1).
- **Surface height** = `height offset·32 − profile·2 − cell height·2 + 0x40` relative to y. The function returns the nearest surface above and below.

- **Cell bits 11–23** (13 bits) are a lighting zone. In ENG1 these bits are the camera index.
  - `GFX_ObjectLightColour` 0x23b00 looks the zone up in table 0x4067c (`u16 +2`: bits 0–2 light ramp, bit 3 flat, >> 4 index into 0x40678).
  - 0x40678 holds four corner light bytes, interpolated by the object's position in the cell.
  - The result indexes the ramp table 0x41460 (ramp·64 + level), scaled by the object brightness (+0x4a) and returned as a POLY_FT4 colour (code 0x2c/0x2e).

- **Array at 0x40654** (= first array + n8 − 1 dwords; each entry a self-relative offset): 3D model/keyframe table.
  - `FUN_10024420` takes the object's +0x36 model, blends vertices between two keyframes with factor obj+0x4c (`(a·(256 − t) + b·t) >> 8` per component) and feeds the GTE helpers 0x1fd80/0x1fdc0/0x1fca0.
  - The exact split between the profile array (0x4065c − 2) and this table is still being checked.

## Tracks, spawns and the remaining tables

`docs/full-game/eng3-tables.md` (S3.01/S3.03) covers the rest of the format:
- the `MOD_Tick` callees;
- the track table hdr+0x30 (parent, branch column, block list, visible tracks, turn list, spawn list);
- spawns `{u16 x, u16 y, s16 z, u16 template}` with the 10-byte templates of config 0x3b900 +0x40;
- zones, corner light, cell meshes (n8), models with hitboxes (nE), animated (n10) and scrolling (n12) textures, track splines (n14), and cell attribute / trigger codes.

`tools/level/eng3_decode.py` writes the tracks and spawns into `level.json`; `tools/level/eng3_types.py` writes the per-kind tables (`extracted/meta/full-game/L02/L07/L09-types.json`).

Track structure (block list = {block, height} with heights 0/4/8/12 as stacked layers):

| level | tracks | branching | spawns per track |
|---|---|---|---|
| GAUNTLET | 11 | binary: 0 → 1, 2 at column 117; 1 → 3, 4; 2 → 5, 6 at 117; 3–6 → 7–10 at 127, which share blocks 28–31 | 105/82/67/80/72/60/82/76/76/76/76 |
| CYCLOPS | 4 | chain 0 → 1 → 2 → 3 (branch columns 255, 254, 254) | 196/177/165/0 |
| TORMENT | 5 | 0 → 1, 2 at column 229; 1 → 3, 2 → 4 at column 139 | 108/80/85/149/138 |

Start records: "section" = track index.

## Open

- Pointer 4 (0x4066c, no reader).
- The axis order of the attribute grid.
- The model u16 at +8 + 8n.
- The meaning of the slot 0x8c timer.
- What each object kind does, for every level (in progress: `eng3-tables.md` "Object kinds per level").
