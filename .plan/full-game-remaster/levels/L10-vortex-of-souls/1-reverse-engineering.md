# L10 Phase 1 — Reverse engineering

Goal: recover the VORTEX-specific code and data. That means the per-level config and type table, where the level's actors come from (the map has no spawns), how the vortex itself is built and moved through, the final encounter, the rescue objective, and the completion → ending → credits path.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`, plus `TITLE.DLL` for the ending); S2.01 T28 model record; S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L10-types.json`; notes in this file.

Do not assume platformer rules or a boss identity. Name an actor only when a bank load or code proves it.

## Tasks

- [x] `L10.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·11 and list its fields next to TRAINING's and TITANS's. Resolve the type-table pointer (+0x1c) and the start-point records for ID 11 (0x5a69c + 0x18·start index, as `HERO_Init` 0x15090 uses them). Resolve the descriptor's remaining pointer fields (0x1006b178, 0x1006b158, 0x1006b168) from their consumers.
- [x] `L10.RE.02` (P5.ALL.01, P5.ALL.08) Actor source. The decoded map has 0 spawns, so find what creates VORTEX's objects:
  - walk the ID-11 type table and find the callers of each spawn handler other than the map spawn loop
  - check the config record and the descriptor fields for event/script lists
  - look for level-ID comparisons with 11 (0xb) in init, tick and hero code
  - find every reader of `P:\SRC1\BOSS\HADES` (0x1005d534), and of `PAIN`/`PANIC` (0x1005db78 / 0x1005db60)

  Write `L10-types.json` as kind → RVAs → bank → creator RVA. Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [x] `L10.RE.03` (P5.ALL.02, P5.ALL.07) Vortex construction and movement. Using S2.01 (`GFX_DrawModel` ENG1 0x31540), determine whether the vortex is drawn from T28 models, effects, the single 20×20 layer with its 18 tile meshes, or a combination, and what drives the scrolling texture (`n_scroll` 1). Determine the hero's movement mode here: which states/handlers run, input mapping, depth and camera. If it shares code with the L08 flight mode (`L08.RE.03`), cite those RVAs.
- [x] `L10.RE.04` (P5.ALL.08) Decompile the final-encounter actor update(s): states and transitions, health, attacks, vulnerable windows, damage through `HERO_RequestDamage` 0x266a0, phase changes, defeat condition, and difficulty reads (S6.01). Record phases only as the code shows them; unknown counts stay null.
- [x] `L10.RE.05` (P5.ALL.04, P5.ALL.08) Souls and the rescue objective: the handlers for soul actors or hazards, the rescue target and its trigger, success/failure states, and whether the rescue gates completion.
- [x] `L10.RE.06` (P5.ALL.11) Completion → ending. Recover the completion trigger and the module request that follows (TITLE dispatch 0x16300 or another path), then the TITLE code that shows the ending and credits: which TGA screens it loads through `RT_LoadFile` slot 0x68 (candidates `C*`, `SEQ*`, `LEV12`), which `CHOP_TITLE_*` banks and music it uses, and how it returns to title. Also find the Beginner gate that keeps ID 10/11 out of reach (with S6.02/S6.06), and hand the results to S6.05.
- [x] `L10.RE.07` (P5.ALL.01) Re-run the ENG1 string export so it includes the descriptor-referenced `O:\VORTEX\` (0x1005a7c0) and any `q:\VORTEX\` / `q:\HERC_VOR\` paths that the first scan missed. Find who reads `q:\TORMENT\` (0x100518ac) and `q:\HERC_TOR\` (0x100517f4) in ENG1.

## Done when

- The creator of every VORTEX actor is documented down to exact RVAs (or marked unknown, with the next consumer to check).
- The vortex construction, movement mode, final encounter and ending path are documented with RVAs.
- `L10-types.json` exists.

## Notes
- `L10.RE.01` (2026-10-12): `extracted/meta/full-game/L10-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68ce8: actor_pool_count 20 (TRAINING 60); marker_pool_count_minus5 60 (TRAINING 120); pickup_pool_count_minus5 148 (TRAINING 98); actor_kind_funcs 0x6a968 (TRAINING 0x67608); marker_kind_funcs 0x6acb0 (TRAINING 0x67bd0); pickup_kind_funcs 0x6af60 (TRAINING 0x67f80); spawn_templates 0x702a8 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a798 (0x5a690 + 0x18·11):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x6b178, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x6b158, the level bank list (0x3270, S4.01);
    - +0x14 = 0x6b168, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (1; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(144,416,128).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L10.RE.02` (2026-10-12, VERIFIED-STATIC): VORTEX's objects come from `LVL_InitLevelActors` 0x1fd20 (called at section start via 0x25620) → `LVL_InitVortex` 0x44120. It:
  - places the hero at (0x135, 0x220, 0x80);
  - spawns kind 0x28 (type entry +0x1e0: `FUN_100445a0`/`FUN_100447c0`, the final encounter) and 30 × kind 0x2b (0x2000 pool, effects);
  - plays sfx 0x2c0 and starts the boss gauge (5);
  - selects difficulty tables 0x69180 / 0x69220 / 0x692b8 → 0x70198/0x7018c.
  - 0x447c0 spawns kinds 0x29/0x2a and effects 0x40/0x29; 0x445a0 spawns effect 0x29; 0x45a80 / 0x45ef0 spawn effects 0x2a / 0x2c.
  - Background: `LVL_DrawBackgroundVortex` 0x2fa0 (T44 tiles).
- `L10.RE.04` progress (2026-10-12; this note first said RE.03 by mistake): `docs/full-game/gameplay/encounters.md` § Final encounter (partial).
  - Kind 0x28 (`HERO_HadesSpawn` 0x445a0 / `HERO_HadesUpdate` 0x447c0): 200 hit points, punch damage from 0x19b40, projectiles 2, gauge 256/(200/damage).
  - Defeat flag 0x72014 → `FUN_10045780`.
  - Open: projectile damage, the movement scripts, the ending path.
- `L10.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - ENG1 side: `HERO_HadesUpdate` sets 0x72014 when the boss's hit points reach 0 and starts timer 0x7028c = 300 (`FUN_10045780`). While 0x72014 is set the hero cannot die (`HERO_Update`). When the timer has run out, `LVL_TickVortex` 0x44240 calls `HERO_RequestLevelEnd` → `HERO_SetEndSequence` → opcode −9 → gs+3 = 0x31 → default exit path.
  - TITLE side (`progression.md`): tally 0xd2 → post-tally FMV 0xf (Herculean) or 0x1b (Medium) → 0xcd ending → 0xcf. On Beginner, VORTEX (11) can't be reached; its card state goes to 0xcf.
  - The module request that follows the ending is in `progression.md` (gamehercules-40, S6.05).
- `L10.RE.07` done (2026-10-12):
  - `analysis/exports/full-game/eng1_strings_raw.json` holds 1,258 raw ASCII strings from every non-code section of ENG1, 907 of them missing from the Ghidra `list_strings` export (defined strings only).
  - It contains `O:\VORTEX\` 0x5a7c0 (descriptor +0 for ID 11, read by `LVL_LoadMap`/`LVL_LoadBlock`), `q:\VORTEX\` 0x518a0, `q:\HERC_VOR\` 0x517e4, `q:\TORMENT\` 0x518ac and `q:\HERC_TOR\` 0x517f4.
  - The `q:\` strings are reached only through ENG1's copy of the Sonix directory table 0x516a0 (39 `{ptr dir, u32 size}` records, the same id space as the EXE table 0x435178: music 0–12, SFX 13–38). `q:\TORMENT\` is entries 10/23 and `q:\HERC_TOR\` entry 35.
  - ENG1's only reader of that table is `AUD_ForestStreamTick` 0xe670 / `AUD_StreamNessusMusic` 0xebd0, with fixed ids 0x51690–0x51698 = 37, 16, 17 (NESSUS/FOREST). So ENG1 never selects the TORMENT entries; TORMENT audio is loaded by ENG3 through slot 0x6c (`audio.md`).
  - VORTEX's banks come through its descriptor +0x14 record (S5.03).
- `L10.RE.05` done (2026-10-12, VERIFIED-STATIC):
  - **Souls:** the 30 class-0x2000 kind-0x2b objects spawned by `LVL_InitVortex` (VORTEX pickup table +0x2c: init `HERO_SoulInit` 0x46230, tick `HERO_SoulTick` 0x46320; bank 80). Each takes a random position (x 0x40..0x500, y 0xd0..0x380, z near the hero), frame 0x1c7 onward (bank 80 animation table 0x6938c), and fades in to brightness 0x60 and back out. It then moves to a new random spot, unless the boss is beaten (0x72014), in which case it is freed. No collision or hero test: they are ambient.
  - **Hazards:** kind 0x29 (`HERO_HadesEffectUpdate` 0x45a80) falls onto the surface, then spawns kind 0x2a; kind 0x2a (`HERO_HadesUpdate2` 0x45ef0) is a homing projectile steering toward the hero, sfx 0xc2ca, trail 0x2c every other tick.
  - **No rescue objective** exists in ENG1's VORTEX code. Completion is gated only by the boss defeat (0x72014 → `LVL_TickVortex` → end flag; RE.06). The "rescue Meg" story beat (LEAD) is not a gameplay condition here.
- `L10.RE.04` done (2026-10-12): `encounters.md` § Final encounter.
  - States 0 (choose a random action script when the hero is within 384 units), 1 (script run), 2 (flip facing).
  - 200 hit points: punch damage from 0x19b40, projectiles and thrown objects 2. Defeat flag 0x72014.
  - Attacks: kind 0x29 drop → 0x2a homing fireball (strength 2, one time in 8).
  - No phases in the code. Difficulty selects the 0x69180/0x69220/0x692b8 table (layout open).
- `L10.RE.03` done (2026-10-12, VERIFIED-STATIC):
  - **Arena:** VORTEX is a one-screen arena drawn by the normal cell renderer: one layer of 20×20 cells, 18 block types, a floor and two side ledges (`preview_composite.png`).
  - **The vortex look** is a combination:
    - the T44 background tile map (12×24 tiles, green wall of souls) drawn by `LVL_DrawBackgroundVortex` 0x2fa0 at (camera x / 8, camera y / 4 − 0x40) over clear colour (0, 32, 0) (`background.png`);
    - the 30 ambient soul objects (kind 0x2b, bank 80, `HERO_SoulTick`, RE.05);
    - the boss and its projectiles.
  - The single T28 model (1 quad) is not placed by any VORTEX handler.
  - **Scrolling texture:** the one T3c rect is never animated in VORTEX. `LVL_ScrollTexture` 0x2d80 is called only by TRAINING objects (0x3cb90, 0x3cf90, 0x3d440, 0x3d930, 0x3dcd0, `HERO_TrainingUpdate3`) and FOREST's `HERO_WaterUpdate3`.
  - **Hero movement:** normal ENG1 platforming. `HERO_Init` has no level-11 branch (flight is level 9 only; the level-6 special case is in 0x16840). `LVL_InitVortex` places the hero at (0x135, 0x220, 0x80); the camera is the level's 2 records.

