# ENG3 per-tick calls and `i1.a` tables (S3.01, S3.03)

ENG3.DLL, base 0x10000000. RVAs are given without the base. Everything here is **VERIFIED-STATIC** (Ghidra decompilation, plus a byte check of the three MAP_PSX.BIN files) unless it is marked *inferred* or listed under Open. gs = game state (`DAT 0x407a4`). "slot 0xNN" = EXE function table copy at 0x407a0 + 0xNN.

## Task A: callees of `MOD_Tick` 0x1abb0

The order follows the loop. P = also runs while paused (gs+4 ≠ 0).

| RVA | what it does (evidence) | suggested name |
|---|---|---|
| 0x1fa70 | thunk to slot 0x84 (EXE 0x4041a0): reads the pad into gs+0x5c (held) and gs+0x5e (newly pressed) | `MOD_ReadPad` |
| 0x1f920 | thunk to slot 0x2c (EXE 0x403f00): async CD read step. Reads up to 5 sectors per call (gs+0x9c/0x9e/0xa4/0xb8) and runs the done callback at gs+0xb0. Also called in the exit loop of 0xb059 | `MOD_StreamReadStep` |
| 0x1fa90 | thunk to slot 0x8c (EXE 0x401d80): countdown 0x4352b0 while gs+0x1b00 = 2, then EXE 0x40a150(9) (a no-op for 9) and gs+0x1b00 = 0. Meaning open | `MOD_ExeTimerTick` |
| 0xacb0 | screen fade. Brightness 0x3fa44 moves by the step 0x4254c (set to 8 at section start) toward 0x35168 (0x80) or 0x3fa40 (0). Calls 0xad20 (fade colour 0x43168 = level·0x10101 \| 0x2c000000) and 0x1fe70 | `GFX_FadeStep` |
| 0xa700 | frame start. Clears the ordering table (0x1fc10(buffer+0x88, 0x800)) and resets the primitive pointer 0x43164 to 0x405c4 (+ per-level offset 0x39878 for buffer 1) | `GFX_BeginFrame` |
| 0x8540 | bank streaming. If no read is busy (gs+0x9c = 0, gs+0xdf = 0), takes the next bank in the state table 0x39558 (10 shorts, banks 1–9) whose state is 1 and starts its ANIMPSX.BIN load (0x8610) | `LVL_BankLoadStep` |
| 0x222c0 | camera follow: camera x/y/z 0x405e0/e4/e8 follow the target object 0x431d4. Mode bits 0x4063a, step tables 0x399f0/0x39a18/0x39a40, z clamped by `LVL_TrackSpline`(0x40639), y by `LVL_FindSurface`. Writes the cell coordinates 0x405ec/f0/f4 | `LVL_CameraFollow` |
| 0x22070 | view setup: converts camera (track x, z) to world 0x405a0/a4/a8 and yaw 0x405b0 through the per-track path 0x40540[0x40638] (see track +0xa below) and the sin table 0x407b4. Adds shake 0x405fc, then sets the GTE (0x1fff0, 0x20010) | `GFX_SetupView` |
| 0x20190 P | draws the level geometry. Copies the light matrix 0x39490, then for the current track 0x40638 and every track in its visibility list (track +8) and parent (track +0) calls 0x204c0. That function draws the cells (0x206f0, using 0x4065c meshes, 0x40658 texture words, 0x4067c/0x40678 light) and writes the drawn column range to 0x4078c | `GFX_DrawLevel` |
| 0x8b20 P | object draw. Depth-sorts the three pools (0x94-byte C, 0xec-byte B, 0x134-byte A) into lists through 0xa470, then: models list 0x43170 → 0x23ce0 (`GFX_DrawModel`); list 0x431d8 → 0x7b90; sprites list 0x431c4 → `ANIM_DrawObjectFrame`, 0x24a80/0x8ca0, or a per-type draw 0x35010[obj+0x23] | `GFX_DrawObjects` |
| 0x1b9f0 | object update pass 1: for each live object in pools C, B, A (+0x2e type ≠ 0) whose flags +0x50 have bit 0, calls the type handler (tables 0x40bb4 / 0x40b98 / 0x40b8c). Builds the per-frame lists 0x41200 (trigger/platform objects, used by `LVL_TriggerAt`), 0x40d20, 0x40de0, 0x40f40, 0x40fa0, 0x40e40, 0x411c0. First runs the track spawner 0x1c430 | `LVL_UpdateObjects` |
| 0x85a0 P | bank release: banks in state 4 → 0x89a0 (free the buffer, state 0); state 7 → free, then state 6 | `LVL_BankReleaseStep` |
| 0x1880 | CYCLOPS only (level 8): calls 0x3610, the controller of the Cyclops object 0x3fb0c (its animation states 0x41–0x45 and camera shake 0x405fc) | `LVL_CyclopsTick` |
| 0x1bd40 | object update pass 2: the handlers with flags bit 1 (+8 slot for pool A), the list 0x413a0, the hero-ride object 0x41c60. Then 0x1c5c0 (smoothing of 0x40648/0x40610), then the x wrap (& 0x1fffffff) of all pools when the level flag 0x39873 is set | `LVL_PostUpdateObjects` |
| 0x250f0 P | HUD: timer digits when gs+6 = 3; health bar lerp gs+0x20→0x1e and gs+0x22→0x24; panel sprites through 0x25620; digit cells copied in VRAM through 0x8000 | `GFX_DrawHud` |
| 0x8050 | animated textures (n10 table, below): every `period` frames, uploads the next frame (0x1fe30 = VRAM LoadImage) | `GFX_AnimateTextures` |
| 0xaa70 | deferred VRAM upload: if 0x43178 is set, LoadImage(rect 0x35120, 0x4316c), then clears the flag | `GFX_FlushPendingUpload` |
| 0xb512 | positional SFX: for the 24 voices (0x41ec8, stride 0x40), computes left/right volume from the owner object's screen x (+0xc − 0xa0) and depth (+0x10), and stops or keeps voices (EXE sound thunks 0x200e0/0x20050/0x20150) | `MOD_UpdateSfxVoices` |
| 0xb059 | exit check on the gs+3 command. 0/1/2/5 → continue. 0x12/0x13 → `MOD_ReleaseLevel`, back to TITLE. Default → release, then TITLE or 0x1f8b0. 0x21 → reset, return 0x21, which makes `MOD_Tick` rerun `LVL_StartSection` (restart) | `MOD_CheckExit` |

Other functions named while tracing:
- 0x22790 `LVL_BuildTrackPaths` (from `LVL_StartSection`);
- 0x1c010 `LVL_InitSpawnLists` (via 0x1b9c0, from `LVL_StartSection`);
- 0x1c430 `LVL_SpawnStep`;
- 0x1d9b0 `LVL_SelectObjectConfig` (from `LVL_AllocPools`);
- 0x8120 `GFX_ScrollTexture`;
- 0x1b4a0 `LVL_CellAttribute`;
- 0x10024870 `GFX_ModelHitbox`.

## Task B: tables after the blocks (`LVL_ParseHeader` 0x1af10)

Let q = 0x38 + 4·blocks (the first dword after the block pointers). The four pointers are `q + (dword & ~3)`. The arrays start at A = q + 0x10 (`DAT 0x4065c`). They are contiguous dword arrays in the order **n8, nE, n10, n12, n14, attr**:

| global | value | indexing (consumer) |
|---|---|---|
| 0x4065c | A | n8 cell entries, 1-based: entry k at A + 4(k − 1) |
| 0x40654 | A + 4(n8 − 1) | nE models, 1-based: `0x40654[k]`, k = 1..nE = A + 4(n8 + k − 1) |
| 0x40660 | A + 4(n8 + nE) | n10 animated textures, 0-based |
| 0x40664 | + 4·n10 | n12 scrolling textures, 0-based |
| 0x40670 | + 4·n12 | n14 track splines, 0-based |
| 0x40668 | + 4·n14 | cell attribute records, 1-based: `[0x40668 − 4 + 4k]` |

Every array entry is a self-relative offset from the entry's own address. The exceptions are noted per table.

Counts kept in bytes:
- 0x40786 = +0x0c = track count nC (11/4/5);
- 0x40788 = nE;
- 0x40789 = n10;
- 0x4078a = n12;
- 0x40785 = n14.

### Pointer 1 → 0x40680: unused
Written only (no other xref). In all three maps it points to the same address as pointer 2 (the zone table).

### Pointer 2 → 0x4067c: zones, 4 bytes per zone (index = cell bits 11–23)
- `+0 u16` attribute index into 0x40668 (0 = none), read by 0x1b4a0.
- `+2 u16` light word (bits 0–2 ramp, bit 3 flat, >> 4 corner index), read by 0x206f0 and `GFX_ObjectLightColour` 0x23b00.

Zones: GAUNTLET 20, CYCLOPS 807, TORMENT 1742.

### Pointer 3 → 0x40678: corner light, u32 = 4 bytes per entry
Read by 0x20ed0, 0x21a10 and 0x23b00 (as already documented).

### Pointer 4 → 0x4066c: no consumer
Written only. It points right after the track table (hdr+0x30 + 0x10·nC) and before the hdr+0x28 face table. Sizes: 8 bytes (GAUNTLET `01 00 02 00 00 00 00 01`), 0x3c bytes (CYCLOPS `02 00 0e 00 00 00 80 00 00 10 80 10 …`), 0x20 bytes (TORMENT). Open.

### n8 (0x4065c): cell geometry
Cell bits 1–10 = k. Entry dword: `lo s16` = mesh offset in dwords, `hi s16` = collision profile offset in bytes. Both are relative to entry+2.
- `LVL_FindSurface` 0x1b3b4 reads `hi`: profile = (entry + 2) + hi.
- 0x206f0 reads `lo`: mesh = (entry + 2) + 4·lo. Mesh layout:
  - u8 nverts, u8 nfaces;
  - nverts × u32 packed vertex (transformed by 0x20ed0/0x20c50/0x21390/0x21a10);
  - from mesh + 4·nverts + 6, nfaces × 8-byte face: u32 = 4 × 6-bit vertex index (bits 0/6/12/18), plus flags in bits 24–31 (0x80 flat colour, 0x20 semi-transparent); then u32 texture: low 16 bits = byte offset into 0x40658 (3 dwords: tpage/clut/uv), bits 24–31 = OT depth bias (`>> 22 & 0x3fc`).

So the "collision profile" array is also the render mesh table.

### nE (0x40654): 3D models (k = obj+0x34, also obj+0x36 in `GFX_ModelVertices`)
Model record r = entry + dword:
- `+0 s16` face list index into 0x40650 (dwords);
- `+2 u8` n hitboxes;
- `+3 u8` cull radius (0x24a00);
- `+4 s32` packed offset (≠ 0 → 0x246e0);
- `+8` n × 8-byte hitbox (0x24870, 1-based; 0 = first). The first u32 is the centre: x s16 bits 8–23, y s8 bits 24–31, z s8 bits 0–7. The second u32 is the size, in the same packing. Both are scaled by obj+0x3a (or +0x3a/+0x3c/+0x3e when +0x54 & 0x80);
- `+8 + 8n` u16 (equal to +0 in the models checked);
- `+0xa + 8n s16` vertex count, then vertices as u32: x = s16 bits 8–23, y = s8 bits 24–31, z = s8 bits 0–7 (0x24280). Keyframe blending uses data from +0xc + 8n (`GFX_ModelVertices`).

*Correction (2026-10-12, `GFX_ModelHitbox` re-read):* the first u32 is the **min corner**, not the centre (x s16 bits 8–23, y s8 bits 24–31, z s8 bits 0–7, plus the +4 offset when ≠ 0). The second u32 is an **unsigned extent** (x u16, y u8, z u8). Exported as `models[].hitboxes` in `level.json`. Sprite objects use the frame record boxes instead (`GFX_FrameHitbox` 0x23860: 6 s8 = z0, y0, x0·2, z1, y1, x1·2), exported as `hitboxes` in the SRC3 `anim.json`.

Drawn by 0x23d90 (single model) and 0x23e20 (parent + child list obj+0x120).

### n10 (0x40660): animated VRAM textures (0x8050, every frame unless paused)
Record:
- `u16 nframes`;
- `u16 period` (ticks, 0 = off);
- `RECT {u16 x, y, w, h}`;
- then nframes × (w·h u16) pixel blocks.

Frame = (gs+0x38 / period) % nframes, uploaded to the RECT. CYCLOPS: 4 frames / 3 ticks at (480, 256) 21×32. TORMENT: 2 entries, 6 frames / 5 ticks.

### n12 (0x40664): scrolling VRAM textures (`GFX_ScrollTexture` 0x8120(idx, dx, dy))
- Record +6/+8: destination x, y. +0xa/+0xc: w, h. +0xe/+0x10: source x, y.
- Scroll state per entry: 0x40584 (u16 ox, oy; allocated and cleared by 0x82d0).
- The offset wraps modulo w/h, and the four sub-rectangles are copied with 0x8000 (VRAM→VRAM copy, *inferred* MoveImage).
- Driven by objects (callers 0x13860, 0x29ea0), not per tick.
- GAUNTLET has 2 (8×64 and 7×64 strips), TORMENT 1.

### n14 (0x40670): track splines (`LVL_TrackSpline` 0x1b650(out, idx, flags, x))
- idx = path index 0x40639 (< n14).
- Entry offset is in dwords: rec = entry + 4·(dword >> 2).
- rec: `u16 count` (+2 u16), then count × 0x10-byte key points from rec+4:

| off | type | meaning |
|---|---|---|
| +0 | u16 | x key |
| +2 | u16 | z centre (+0x300) |
| +4, +8 | u16 | y parts (summed) |
| +6 | u16 | half-width |
| +0xa | s8 | camera height term (out[4]) |
| +0xc | s8 | out[5]/out[6] |

- The point pair around x is lerped (16.16): out = {zmin, y, zmax, y, cam, c, c}.
- Users:
  - `HERO_PreStep` (camera height 0x40620 = 0xe8 − out[4]);
  - `HERO_Tick` (lane clamp);
  - 0x222c0 (camera z clamp, 0x4063c = out[5]);
  - 0x5760, 0x5b40.

### attr (0x40668): cell attribute / trigger codes (0x1b4a0 = `LVL_CellAttribute`(x, y, z, track))
For each block in the track's block list:
1. cell → zone (bits 11–23) → zone `+0 u16` k.
2. If k ≠ 0: record r = (0x40668 − 4 + 4k) + dword.
3. `u16 hdr`, then bytes. hdr = 0 → constant code = s8 at r+2.
4. Otherwise hdr picks a byte grid. Bits 4–7 = x-step, bits 8–15 = y-step, bits 0–3 = z-step entries of table 0x39a68 (0x10 stride). The grid is indexed by the sub-cell x, y (relative to the surface height) and z.

`LVL_TriggerAt` 0x1ecf0 first tests the object list 0x41200 (returns obj+0xd8), then this table. The codes are the chase.md obstacle and trigger codes. GAUNTLET records 1–21 hold the constant codes 1, 2, 0x5e, 0x5d, 3–7, 0x76, 0x52, 0x73, 0x7b, 0x7a, 0x56, 0x52, 0x51, 7–0xa. CYCLOPS and TORMENT are similar (0x50–0x5e, 0x63–0x77, 0x86). The count is not stored: the maximum zone index is 21 / 23 / 13.

### Track table (hdr+0x30 → 0x40674), 0x10 bytes × nC: where object spawns come from

| off | type | meaning |
|---|---|---|
| +0 | u8 | parent track (0x7f = root) |
| +1 | u8 | branch column on the parent |
| +2 | u8 | column limit for drawing the parent (0x20190) |
| +4 | u16 | yaw offset (0x22790) |
| +6 | rel s16 | block list `{n, (block, height)×n}` |
| +8 | rel s16 | visible tracks list (0xff-terminated) |
| +0xa | rel s16 | s8 per column: turn direction (0 straight, ±1). `LVL_BuildTrackPaths` 0x22790 turns it into 0x40540[track] = 0x10 bytes per column (s32 world x, s32 world z, s16 yaw +8, +0xe flag, +0xf). This feeds `GFX_SetupView`, 0x204c0, 0x21200 and 0x24bb0 |
| +0xc | u16 | spawn count |
| +0xe | rel s16 | **spawn list** |

**Spawns** (the ENG1 section list counterpart) are 8 bytes each: `{u16 x, u16 y, s16 z, u16 template}`.
- `LVL_InitSpawnLists` 0x1c010 copies them per track into 0x3fbd0[track] (malloc (n+1)·0x14; runtime entry from +0x14: s32 x, y, z (16.16) = spawn + template offset; +0xc flags; +0xe state, bit 0x80 at +0xd = spawned; +0x10 type).
- Templates: 0x40c34 = per-level object config 0x3b900 (0x48 bytes per level, u16 level id, 999-terminated, picked by 0x1d9b0) +0x40. Each template is 10 bytes `{s16 dx, dy, dz, u16 flags, u16 type}`. On restart, entries whose template flags & 2 are reset.
- `LVL_SpawnStep` 0x1c430 (every tick in `LVL_UpdateObjects`): for each track drawn this frame (0x4078c[t] = {frame, first col, end col}, written by 0x204c0), entries with x in (end·128, end·128 + 256) and flags & 0xc0 = 0 are spawned by type & 0x7000:
  - 0 → `LVL_SpawnActor` 0x1c9f0 (pool A, type handler 0x40b8c);
  - 0x4000 → 0x1d050;
  - 0x2000 → 0x1d570;
  - 0x1000 → 0x1db50.
- Counts: GAUNTLET 105/82/67/80/72/60/82/76 (tracks 7–10 share one list); CYCLOPS 196/177/165/0; TORMENT 108/80/85/149/138.

## Open
- Pointer 4 (0x4066c) data: no ENG3 reader, so it may be dead data or read only by the EXE.
- The attr grid form (hdr ≠ 0): the exact axis order. The size per record is not stored.
- The model u16 at +8 + 8n, and GAUNTLET model 1 (n = 0, odd data after +8).
- 0x1fa90 (slot 0x8c) meaning. The spline field +2 and the point field +0xc meaning.
- Spawn types 0x1000/0x2000/0x4000 (pools B/C?): to confirm in 0x1d050/0x1d570/0x1db50.

## Object kinds per level

Static RE of the type handlers (S3 RE.02 follow-up). Data: `analysis/exports/full-game/eng3_kind_handlers.json` (hit code, damage per difficulty, gives, random, notes). Damage = HERO_Hit 0x19200 hit code -> table 0x380a0 column (Beginner/Medium/Herculean). No handler reads gs+0xe directly. Evidence: VERIFIED-STATIC = role from code; VISUAL = role from code, identity from sprite frames; LEAD = identity from the walkthrough (behaviour still from code). Random: `RT_RandMod_slot` 0x1fab0 (EXE slot 0x834) and `LVL_Rand` 0x1010 (local LCG). Names are proposals (not applied in Ghidra).

### GAUNTLET (L02, id 2)

| kind | map | what | hit (dmg) | ev | handlers |
|---|---|---|---|---|---|
| 0x0001 | 0 | Hercules (hero, pool A slot 0); type entry is the stub, driven by HERO_Tick 0x174e0 | - | VERIFIED-STATIC |  |
| 0x0006 | 326 | bonus item by spawn byte: 0 helmet (invulnerable 200), 1 potion (+0x20 health), 2/4 coin (gs+0x18/gs+0x14), 3 sandals (dash), 5 urn (secret, gs+0x11), 6 amphora (life), 7 mini-Hercules (+0x20 max health), 8-15 letters HERCULES (gs+0x1c) | - | VISUAL | `LVL_BonusItemSpawn` 0x15440, `LVL_BonusItemTick` 0x15680 |
| 0x0029 | 7 | toppling statue/column (level model 2 + two 0x4029 parts) that falls toward the hero | 0x21 (15/20/30) | LEAD | `LVL_GntToppleStatueSpawn` 0xd280, `LVL_GntToppleStatueTick` 0xd410 |
| 0x002a | 11 | swaying stack of 4 blocks (model 3, 0x402a) that collapses forward when the hero is near | 0x2 (25/30/40) | LEAD | `LVL_GntStackSwayFwdSpawn` 0xd7d0, `LVL_GntStackSwayFwdTick` 0xdc20 |
| 0x002b | 10 | group of rolling model-13 objects (spawn byte = count: 1 large x2 scale, or 6 small) bouncing between lane walls | 0x26 (15/20/30), 0x27 (10/15/25) | LEAD | `LVL_GntRollerGroupSpawn` 0x10370, `LVL_GntRollerGroupTick` 0x10590 |
| 0x002c | 16 | training dummy with swords (ALIEN1 403-439) sliding across lanes; breaks on contact | 0x22 (10/15/25) | VISUAL | `LVL_GntSwordDummySpawn` 0xee40, `LVL_GntSwordDummyTick` 0xeeb0 |
| 0x002d | 2 | squad of 4 walking totem dummies (ALIEN1 477-523) wandering; each breaks on contact | 0x22 (10/15/25) | VISUAL | `LVL_GntTotemSquadSpawn` 0x10b10, `LVL_GntTotemSquadTick` 0x10df0 |
| 0x002e | 23 | rotating obstacle of 4 model-23 pieces (0x402d) in a 2x2 block | 0x2b (15/20/30) | LEAD | `LVL_GntSpinnerSpawn` 0xf460, `LVL_GntSpinnerTick` 0xf7b0 |
| 0x002f | 18 | sliding wall (model 9 + 2 parts 0x402f, 2 wheels 0x3f) that slides +z across the lane and back | 0x28 (20/25/35) | LEAD | `LVL_GntSlideWallRSpawn` 0xf980, `LVL_GntSlideWallRTick` 0xfab0 |
| 0x0030 | 18 | sliding wall, mirror of 0x2f (slides -z); with 0x2f forms a closing gate | 0x28 (20/25/35) | LEAD | `LVL_GntSlideWallLSpawn` 0xfec0, `LVL_GntSlideWallLTick` 0xffd0 |
| 0x0032 | 10 | shark leaping out of the water (ALIEN1 0,16-26,53-58,167-175) | 0x2a (40/55/85) | VISUAL | `LVL_GntSharkSpawn` 0x113f0, `LVL_GntSharkTick` 0x11470 |
| 0x0035 | 15 | as 0x2a, falling the other way (angle decreasing) | 0x2 (25/30/40) | LEAD | `LVL_GntStackSwayBackSpawn` 0xe020, `LVL_GntStackSwayBackTick` 0xe2f0 |
| 0x0037 | 23 | hanging pendulum of 4 segments (models 12,12,12 + 11 weight) | 0x2c (15/20/30) | LEAD | `LVL_GntPendulumSpawn` 0x11760, `LVL_GntPendulumTick` 0x11a80 |
| 0x0038 | 0 | bonus figure above the fire ring (ALIEN1 85-108); poofs (138-156) when the hero jumps through the ring | - | VISUAL | `LVL_GntRingTargetSpawn` 0x11d30, `LVL_GntRingTargetTick` 0x11d80 |
| 0x003a | 11 | Phil (satyr trainer, ALIEN1 646-820) commenting as the hero passes | - | VISUAL | `LVL_GntPhilSpawn` 0x11ef0, `LVL_GntPhilTick` 0x11f90 |
| 0x003c | 8 | flaming ring (ALIEN1 273-280) bobbing on a rope (0x41) with a bonus figure (0x38) | 0x40 (25/35/55) | VISUAL | `LVL_GntFireRingSpawn` 0x12200, `LVL_GntFireRingTick` 0x12250 |
| 0x003e | 30 | hanging weight on 4 segments (model 12 x3 + model 21/22), half swing (byte 0) or full swing (byte 1) | 0x25 (10/15/25) | LEAD | `LVL_GntHangingWeightSpawn` 0x12500, `LVL_GntHangingWeightTick` 0x12820 |
| 0x003f | 5 | rotating decoration (model 16, or 20 for byte 1); also used as the wheels of 0x2f/0x30 | - | LEAD | `LVL_GntWheelDecoSpawn` 0x12e40, `LVL_GntWheelDecoTick` 0x12e80 |
| 0x0040 | 43 | swinging punching bag on a rope (ALIEN1 264-272) | 0x23 (10/15/25) | VISUAL | `LVL_GntPunchBagSpawn` 0xebe0, `LVL_GntPunchBagTick` 0xec60 |
| 0x0041 | 0 | rope sprite (frame 637) above the fire ring | - | VISUAL | `LVL_GntRopeInit` 0xee00 |
| 0x0043 | 66 | wandering goats (ALIEN1 329-389) that flee when the hero approaches | - | VISUAL | `LVL_GntGoatWalkerSpawn` 0x13010, `LVL_GntGoatWalkerTick` 0x130e0 |
| 0x0044 | 13 | training dummy that homes on the hero lane (ALIEN1 403-439); breaks on contact | 0x22 (10/15/25) | VISUAL | `LVL_GntChaserDummySpawn` 0x13330, `LVL_GntChaserDummyTick` 0x133b0 |
| 0x0045 | 0 | goat in a herd crossing diagonally (first one ALIEN1 821-828) | - | VISUAL | `LVL_GntHerdGoatSpawn` 0x13a40, `LVL_GntHerdGoatTick` 0x13a90 |
| 0x0046 | 1 | goat-herd source near the start; scrolls VRAM texture 1 every tick | - | VISUAL | `LVL_GntHerdSourceInit` 0x13840, `LVL_GntHerdSourceTick` 0x13860 |
| 0x0047 | 145 | idle goats (ALIEN1 354-365) | - | VISUAL | `LVL_GntGoatIdleSpawn` 0x13b30, `LVL_GntGoatIdleTick` 0x13c10 |
| 0x0048 | 15 | rotating two-arm sweeper (model 24) | 0x2d (15/20/30) | LEAD | `LVL_GntSweeperSpawn` 0x12be0, `LVL_GntSweeperTick` 0x12c40 |
| 0x0049 | 3 | end post (model 25) near the finish (x 15101-15121) | 0x35 (10/15/25) | LEAD | `LVL_GntEndPostSpawn` 0x13cb0, `LVL_EndPostTick` 0x2a090 |
| 0x004a | 0 | dust trail under the dummies (ALIEN1 596-603) | - | VISUAL | `LVL_GntDustTrailInit` 0x13d00, `LVL_GntDustTrailTick` 0x13d30 |
| 0x004b | 6 | finish scenery (models 28/29/30 by spawn byte, z = 0) | - | LEAD | `LVL_GntEndSceneryInit` 0x13d70, `LVL_StaticTick` 0x13e00 |
| 0x2002 | 0 | ground shadow under model objects (bank 1 frames 205-207) | - | VISUAL | `LVL_ModelShadowInit` 0x14270, `LVL_ModelShadowTick` 0x142b0 |
| 0x200b | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 1 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200c | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 2 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200d | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 3 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200e | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 4 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200f | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 5 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x2010 | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 6 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x2011 | 1 | Pegasus checkpoint (bank 1 230-249) for restart section 7 | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x2016 | 0 | explosion/dust puff (GAUNTLET bank 3 frames 604-616; CYCLOPS bank 6 209-221; TORMENT bank 4 180-192) | - | VISUAL | `LVL_DustPuffInit` 0x14b30, `LVL_DustPuffTick` 0x14bb0 |
| 0x2017 | 0 | sprite drop shadow (parent frame flattened on the ground) | - | VERIFIED-STATIC | `LVL_SpriteShadowInit` 0x145a0, `LVL_SpriteShadowTick` 0x145d0 |
| 0x2018 | 0 | sprite shadow that also lies on the water level 0x410e0 | - | VERIFIED-STATIC | `LVL_WaterShadowInit` 0x14820, `LVL_WaterShadowTick` 0x14850 |
| 0x2019 | 10 | camera marker: max of obj+0xe while on screen -> 0x3fb88, 0x3fb8c = 1 | - | VERIFIED-STATIC | `LVL_CamMarkerMaxTick` 0x1b890 |
| 0x201a | 10 | camera marker: min of obj+0xe while on screen -> 0x38520, 0x3fb8c = 1 | - | VERIFIED-STATIC | `LVL_CamMarkerMinTick` 0x1b8c0 |
| 0x201b | 0 | checkpoint sparkle | - | VERIFIED-STATIC | `LVL_SparkleInit` 0x168b0, `LVL_SparkleTick` 0x168f0 |
| 0x4001 | 0 | debris piece (thrown frame) with bounce | - | VERIFIED-STATIC | `LVL_DebrisInit` 0x14c30, `LVL_DebrisTick` 0x14d40 |

### CYCLOPS (L07, id 8)

| kind | map | what | hit (dmg) | ev | handlers |
|---|---|---|---|---|---|
| 0x0006 | 278 | bonus item, same handler as L02 0x0006 | - | VISUAL | `LVL_BonusItemSpawn` 0x15440, `LVL_BonusItemTick` 0x15680 |
| 0x002a | 55 | falling boulder / rubble (random model 2,1,11,33,36 from 0x34fe0) that drops when the hero is near | 0x200 (25/30/40) | LEAD | `LVL_CycBoulderSpawn` 0x4580, `LVL_CycBoulderTick` 0x4650 |
| 0x002b | 14 | toppling column (models 1/11/2 + capital 3 or 0x2a) | 0x2 (25/30/40) | LEAD | `LVL_CycColumnFwdSpawn` 0x1aa0, `LVL_CycColumnFwdTick` 0x1fe0 |
| 0x002d | 7 | chariot crossing the street (PEOPLE side view 85-99) | 0x400 (25/30/40) | VISUAL | `LVL_CycCrossChariotSpawn` 0x23a0, `LVL_CycCrossChariotTick` 0x2440 |
| 0x002e | 26 | toppling column falling the other way (capital 0x28 or 0x33) | 0x2 (25/30/40) | LEAD | `LVL_CycColumnBackSpawn` 0x2580, `LVL_CycColumnBackTick` 0x2ac0 |
| 0x002f | 7 | chariot driving down the street toward the camera (PEOPLE front view 101-124) with dust clouds (126-133) | 0x400 (25/30/40) | VISUAL | `LVL_CycChariotSpawn` 0x2eb0, `LVL_CycChariotTick` 0x3020 |
| 0x0035 | 7 | group of 3 fleeing citizens (PEOPLE scripts 10-14: goat 265-275, old man 136-145, lady 162-181) | - | VISUAL | `LVL_CycFleeGroupSpawn` 0x3bf0, `LVL_CycFleeGroupTick` 0x3dc0 |
| 0x0039 | 2 | collapsing street section: 18 tiles (3x6) that sink with shaking | - | LEAD | `LVL_CycSinkFloorSpawn` 0x3ef0, `LVL_CycSinkFloorTick` 0x4100 |
| 0x003d | 3 | flying swooper that circles in from 2304 ahead (PEOPLE 243-253, frames not decoded) | 0x800 (25/30/40) | VERIFIED-STATIC | `LVL_CycSwooperSpawn` 0x4c10, `LVL_CycSwooperTick` 0x4ca0 |
| 0x003e | 10 | flyer carrying a rock (kind 0x43, model 2) that it drops before the hero, then dives | 0x4 (10/15/25), 0x200 (25/30/40) | VERIFIED-STATIC | `LVL_CycFlyerDropSpawn` 0x4dc0, `LVL_CycFlyerDropTick` 0x5070 |
| 0x003f | 8 | crowd source: spawns fleeing citizens (0x4033) ahead of the hero | 0x4 (10/15/25) | VISUAL | `LVL_CycCrowdSourceInit` 0x53a0, `LVL_CycCrowdSourceTick` 0x53f0, `LVL_CycFleeingCitizenInit` 0x56a0, `LVL_CycFleeingCitizenTick` 0x5760 |
| 0x0042 | 15 | bystander (byte 0 lady / 1 man / 2 old man) who starts running with the hero | 0x4 (10/15/25) | VISUAL | `LVL_CycBystanderSpawn` 0x5a20, `LVL_CycBystanderTick` 0x5b40 |
| 0x0045 | 4 | opening columns that collapse at the start (x 1597-1613, track 0) | - | LEAD | `LVL_CycOpeningColumnsSpawn` 0x62f0, `LVL_CycOpeningColumnsTick` 0x67d0 |
| 0x0047 | 90 | fluttering cloth/leaves (PEOPLE 301-308, 312-319) | - | VISUAL | `LVL_CycFlutterSpawn` 0x6af0, `LVL_CycFlutterTick` 0x6b90 |
| 0x0048 | 9 | breakable obstacle (model 0x34) | 0x36 (20/25/35) | LEAD | `LVL_CycCrateSpawn` 0x6c20, `LVL_CycCrateTick` 0x6ca0 |
| 0x200b | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200c | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200d | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |

### TORMENT (L09, id 10)

| kind | map | what | hit (dmg) | ev | handlers |
|---|---|---|---|---|---|
| 0x0006 | 207 | bonus item, same handler as L02 0x0006 | - | VISUAL | `LVL_BonusItemSpawn` 0x15440, `LVL_BonusItemTick` 0x15680 |
| 0x0029 | 3 | flying chariot pulled by a winged beast (TORMENT 352-358) that overtakes the hero and drops fireballs | 0x30 (20/25/35) | VISUAL | `LVL_TrmChariotSpawn` 0x25700, `LVL_TrmChariotTick` 0x25770 |
| 0x002e | 5 | flock of 4 flying spirits (TORMENT 71-82, frames not decoded) weaving toward the hero | - | LEAD | `LVL_TrmGhostFlockSpawn` 0x25ec0, `LVL_TrmGhostFlockTick` 0x26340 |
| 0x002f | 46 | lost soul rising and dissolving (SOULS 13-20, then 1-12) | - | VISUAL | `LVL_TrmLostSoulSpawn` 0x26550, `LVL_TrmLostSoulTick` 0x26600 |
| 0x0030 | 120 | falling stalactite (model 2, starts 1024 above) | 0x31 (20/25/35) | LEAD | `LVL_TrmStalactiteSpawn` 0x266a0, `LVL_TrmStalactiteTick` 0x26770 |
| 0x0031 | 32 | walking skeleton (TORMENT 121-148) | 0x4 (10/15/25) | VISUAL | `LVL_TrmSkeletonWalkSpawn` 0x26b80, `LVL_TrmSkeletonWalkTick` 0x26c60 |
| 0x0033 | 47 | skeleton dropping from above (TORMENT 155-174) | 0x2f (20/25/35) | VISUAL | `LVL_TrmSkeletonDropSpawn` 0x26f00, `LVL_TrmSkeletonDropTick` 0x26fe0 |
| 0x0034 | 10 | tilting raft/platform (model 1) that sinks under the hero | - | LEAD | `LVL_TrmTiltRaftSpawn` 0x27130, `LVL_TrmTiltRaftTick` 0x27180 |
| 0x0035 | 10 | bone wall / rib-cage (TORMENT frame 175) that topples and bursts | 0x33 (20/25/35) | VISUAL | `LVL_TrmBoneWallSpawn` 0x272e0, `LVL_TrmBoneWallTick` 0x27370 |
| 0x003d | 16 | winged fire beast (TORMENT 223-236) spitting fire (kind 0x3e) | 0x2 (25/30/40) | VISUAL | `LVL_TrmFireBeastSpawn` 0x287b0, `LVL_TrmFireBeastTick` 0x28840 |
| 0x0040 | 31 | bone pile (frame 6) that shatters when touched | 0x32 (20/25/35) | VISUAL | `LVL_TrmBonePileSpawn` 0x28a90, `LVL_TrmBonePileTick` 0x28ae0 |
| 0x0045 | 13 | group of 4 rolling model-7 objects (TORMENT version of GAUNTLET 0x2b) | 0x26 (15/20/30) | LEAD | `LVL_TrmRollerGroupSpawn` 0x29140, `LVL_TrmRollerGroupTick` 0x292f0 |
| 0x0046 | 6 | rock fall: 20 model-7 rocks of graded size dropping in a line | 0x34 (10/15/25) | LEAD | `LVL_TrmRockfallSpawn` 0x297d0, `LVL_TrmRockfallTick` 0x29a10 |
| 0x0047 | 1 | start gate (model 10) rising out of the way | - | LEAD | `LVL_TrmStartGateSpawn` 0x29e50, `LVL_TrmStartGateTick` 0x29ea0 |
| 0x0048 | 1 | end post (model 15) at x 29445 | 0x35 (10/15/25) | LEAD | `LVL_TrmEndPostSpawn` 0x2a040, `LVL_EndPostTick` 0x2a090 |
| 0x0049 | 7 | end scenery (models 13/14/16 by spawn byte) | - | LEAD | `LVL_TrmEndSceneryInit` 0x2a180, `LVL_StaticTick` 0x13e00 |
| 0x200b | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200c | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200d | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200e | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |
| 0x200f | 1 | Pegasus checkpoint, same handler as L02 0x200b | - | VISUAL | `LVL_PegasusCheckpointInit` 0x165f0, `LVL_PegasusCheckpointTick` 0x16670 |

### Open

- EXE slot 0x850 (`0x1faf0`, ids 0x16/0x19/0x1a/0x1b/0x1c from GAUNTLET Phil 0x3a): what it plays.
- Readers of 0x3fb88 / 0x38520 / 0x3fb8c (GAUNTLET camera markers 0x2019/0x201a); hero+0xd8 ring counter (0x3c) consumer.
- GAUNTLET 0x3e with spawn byte 1 (model 22, 15 placed): the hit test only checks model 0x15, so these weights never damage. Check in game whether that is intended.
- Identities of level models (GAUNTLET 2/3/9/11-13/16/20-25/28-30, CYCLOPS 0x34 and the 0x34fe0 rubble, TORMENT 1/2/7/10/13-16) are LEAD; render the nE models to confirm.
- Frames not present in the decoded banks: PEOPLE 149-169 and 224-253 (CYCLOPS 0x3d/0x3e flyers), TORMENT 71-82 (0x2e spirits) and 6 (0x40 bone pile).
- Child kinds not traced: TORMENT 0x3e (fire of 0x3d), 0x37/0x3b (debris), GAUNTLET 0xd3d0 (called on 0x29 landing) and 0x16550 (0x48).
- No object is "defeatable" by attack: hero attack inputs are not read in ENG3 (chase.md). Dummies (0x2c/0x2d/0x44) break on contact; rollers 0x2b/0x45 and CYCLOPS 0x48 break only while dashing.
