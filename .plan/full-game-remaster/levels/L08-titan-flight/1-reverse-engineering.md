# L08 Phase 1 — Reverse engineering

Goal: recover the TITANS-specific code and data. That means the per-level config and type table, the flight mode (hero/Pegasus states, input mapping, depth and camera), the handlers for every Titan, god and hazard actor, and the completion path including the Beginner branch.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L08-types.json`; notes in this file.

Do not import side-scroller assumptions. Every flight rule must come from a decompiled handler; where a path is shared with the ENG1 hero spec, cite the shared RVA instead of re-deriving it.

## Tasks

- [x] `L08.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·9 and list its fields next to TRAINING's. Resolve the type-table pointer (+0x1c) and the start-point records for ID 9 (0x5a69c + 0x18·start index, as `HERO_Init` 0x15090 uses them). Resolve the descriptor's remaining pointer fields (0x10063108, 0x100630e4, 0x100630f8) from their consumers.
- [x] `L08.RE.02` (P5.ALL.08) For each kind used in the TITANS spawns, decode the type-table entry `[spawn, stub, update]` (12 B), name the handlers and list the banks and sounds each one loads. Write `L08-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank load or code proves them; otherwise mark them LEAD.
- [x] `L08.RE.03` (P5.ALL.10) Flight mode.
  - Find every reader of the `ALIEN2\PEGASUS` path (0x1005d830) and what it attaches to the hero.
  - Determine whether ID 9 installs different hero handlers in table 0x72040, switches the state id (0x72108) into a flight-only range, or drives the hero from a Pegasus actor. Check whether the per-tick handler 0x15780 and state 27 (cheat fly) share code with it.
  - Decode the input mapping from the polled pad bits (`RT_PollJoystick` EXE 0x7c50, keyboard 0x7f30) to flight motion: horizontal, vertical and any depth axis; speeds and limits; which attack buttons remain and what they spawn.
  - Recover the camera/scroll rule: player-driven or forced, how the 546 camera records and `LVL_StreamBlocks` 0x1f870 advance through the 4 blocks, and any screen bounds.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [x] `L08.RE.04` (P5.ALL.08) Titan hazards. Find the readers of the `IceT`, `LavaT`, `RockT` and `TornadoT` paths and the caller of the `GoodbyeRockT` debug string (0x100655d0). Decompile each update: states, attacks/projectiles, damage through `HERO_RequestDamage` 0x266a0, defeat or avoid conditions, exit, and difficulty reads (S6.01). Bank names are only a starting point.
- [ ] `L08.RE.05` (P5.ALL.04, P5.ALL.08) God rescues. Find the readers of the `Zeus`, `Ares`, `Apollo`, `Athena`, `Aphrod` and `Bacchus` paths; recover the rescue trigger, interaction, scoring and persistence. Find the readers of `Water`, `Griffon` and `Grif` to settle whether those banks belong to TITANS at all.
- [x] `L08.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to the next level through the shared S6 path: the request for ID 10 (ENG3) through TITLE dispatch 0x16300 on Medium/Herculean, and the difficulty branch that ends the game on Beginner. Record which code requests the end screen (for example the `ENDEASY` TGA through TITLE `RT_LoadFile` slot 0x68), and hand the result to S6.05/S6.06.

## Done when

- Every TITANS spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The flight mode is documented down to exact RVAs: state/handler selection, input mapping, motion, depth and camera.
- The completion and Beginner-branch code paths are documented with RVAs.
- `L08-types.json` exists.

## Notes
- `L08.RE.01` (2026-10-12): `extracted/meta/full-game/L08-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68ca0: actor_pool_count 60 (TRAINING 60); marker_pool_count_minus5 40 (TRAINING 120); pickup_pool_count_minus5 41 (TRAINING 98); actor_kind_funcs 0x61f80 (TRAINING 0x67608); marker_kind_funcs 0x62790 (TRAINING 0x67bd0); pickup_kind_funcs 0x62a60 (TRAINING 0x67f80); spawn_templates 0x62bf8 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a768 (0x5a690 + 0x18·9):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x63108, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x630e4, the level bank list (0x3270, S4.01);
    - +0x14 = 0x630f8, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (12; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(192,832,128), 1=(9024,768,128), 2=(21376,768,128), 3=(192,832,128), 4=(192,832,128), 5=(192,832,128), 6=(192,832,128), 7=(192,832,128), 8=(192,832,128), 9=(192,832,128), 10=(192,832,128), 11=(192,832,128).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L08.RE.03` progress (2026-10-12, VERIFIED-STATIC): flight mode is set up per level, not per object.
  - `HERO_Init` 0x15090 clears gauge gs+0x2a in level 9 and returns through `HERO_InitFlight` 0x374c0. That sets hero state 0x28 with the Pegasus scripts 0x633e8, attaches child kind 0xa007 (`FUN_10027d40`), picks a palette script by restart index (2 → 0x63ef8, 1 → 0x636f0, else 0x63a78) and sets up the helper objects 0x707dc/0x707fc.
  - Flight death is `HERO_KillInstant` 0x37080 (health 0, state 0x23, lives − 1).
  - Bank 48 PEGASUS comes from the TITANS level list (S4.01; no HERCULES bank in that list).
  - `LVL_InitTitans` 0x3c170 picks 0x6fff4 = table 0x65580[restart index] and spawns kind 0x36 at the hero.
  - Background `LVL_DrawBackgroundTitans` 0x3c600.
  - Open: the flight input/physics (states from 0x28), the Titans and gods handlers (kinds 0x28–0x68).
- `L08.RE.05` progress (2026-10-12, VERIFIED-STATIC):
  - The gods are map kinds with their bank ids from the anim table (`L08-types.json`): 0x2c Aphrodite (bank 53, x 5921), 0x2d Apollo (54, x 15458), 0x2e Ares (55, x 20387), 0x2f Athena (56, x 20963), 0x30 Bacchus (57, x 25250). Zeus is 0x45 (61, x 29059).
  - Shared spawn `HERO_GodSpawn` 0x3abd0 (z + 12; Bacchus and Zeus face left) → state 0x35.
  - Update `HERO_GodUpdate` 0x3ac80: in 0x35, hero contact (body box) → sfx 0x627b, anim 1, state 0x2d → state 2. The god flies off at +6.0 x / −6.0 y per tick with sparkles (0x3c490) and is freed 400 units above the hero (0x3ac10).
  - No code makes the rescues gate completion, and no score write was seen in these handlers. The scoring and persistence readers are open (tally `progression.md`).
- `L08.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - Zeus (kind 0x45, `HERO_ZeusUpdate` 0x3adc0): hero contact in state 0x35 → sfx 0x627b, state 0x2d, timer 120. At 16 ticks left, fade-out (0x1b30 = 4 then 8). At 0, **gs+3 = 0x31 written directly** (no `HERO_SetEndSequence`).
  - TITLE (`progression.md`): tally → Beginner: 0xcd ending (the Beginner boundary); Medium/Herculean: FMV 0x19 → gs[0] = 10 → card SEQ8 → **ENGINE3** TORMENT (ID 10).
- `L08.RE.02` done (2026-10-12): `extracted/meta/full-game/L08-types.json` covers every map kind plus the level's code-spawned kinds (74 rows). Per kind: class (kind & 0x7000), count, positions, type-table RVAs and current Ghidra names, the sprite bank (anim-table bank id → obj+0x22; 63 kinds have one) and constant sound ids (`tools/level/handler_sounds.py`).
  - Handlers are named from their bank when every user shares it (`tools/level/name_handlers.py` → `analysis/annotations/full_game_types.json`, 203 handlers across levels). 60 kinds now have named handlers; 1 are stubs (0xc720).
  - Unnamed, with the next consumer = the handler code listed in the file: 0x0044, 0x003f, 0x0041, 0x0052, 0x0053, 0x0055, 0x0057, 0x0059, 0x005b, 0x005e, 0x005f, 0x0060, 0x0068.
  - Not spawned by `LVL_SpawnVisible` (0x1000-class): none.
- `L08.RE.04` progress (2026-10-12):
  - Titan kinds by bank: RockT 0x28, IceT 0x29, LavaT 0x2a (+ code-spawned LavaT parts 0x31–0x34, 0x3b–0x3d, 0x56, 0x5a: projectiles/lava pieces; sfx 0x6a49–0x6a4d), TornadoT 0x2b (sfx 0x256).
  - Harpies (Harp 0x61 ×11, 0x5c–0x67), Spy 0x5d (hit points 4), Pig/Grif/Merv 0x5e–0x60 (shared handler 0x3c050/0x3c070). Digest: `actor-digest.md` § L08.
  - The bank requests 0x37fa0 (LavaT, 50) and 0x38490 (TornadoT, 51) come from the TITANS code path.
  - Open: per-Titan attack states and damage (helpers), the `GoodbyeRockT` debug-string caller.
- `L08.RE.03` done (2026-10-12, VERIFIED-STATIC): flight mode.
  - Tick `HERO_TickFlightMode` 0x379e0 (function created in Ghidra). Steering `HERO_SteerPegasusByPad` 0x377f0 moves the Pegasus object 0x7217c from the pad word gs+0x5c (0x10 up, 0x20 right, 0x40 down, 0x80 left): velocity +0xa8 (x) / +0xb8 (y) changes by 2.0 per tick while held, capped at ±7.5, and decays by 1.0 per tick when released; x speed is centred on the scroll speed 0x707e8 instead of 0. Position += velocity.
  - Attacks from state 2 when gs+0x6c & gs+0x5c (attack button) by the weapon index 0x6efb4: 0 → state 0x48 (anim 2); 1 → 0x49 (anim 3) → 0x4a held (0x1a3e0/0x371a0 each tick) → 0x4b on release; 2 → 0x4c (anim 6) → 0x4d (anim 7) spawns effect 0x4005 at hero + (32, 20). Per-weapon repeat timers 0x72140[weapon] reload from 0x55320.
  - Damage `HERO_ApplyDamageInFlight` 0x37e30(level): health gs+0x24 −= table 0x55348[difficulty gs+0xe & 3][level] (the same int[3][8] table as on foot; level ≥ 8 → 1). Skipped while 0x72110 ≠ 0, gs+0x30 & 2, or the grace counter 0x6fff0 > 0 (set to 15 when recoil state 0x24 ends). Hit: sfx 0x6278 or 0x6279 (random), state 0x24, Pegasus helper scripts 0x63350/0x63378. Health ≤ 0 and not gs+0x30 & 1 → `HERO_KillInstant` 0x37080.
  - Death state 0x23: the hero drifts x −2, y +3 per tick; at the end gs+3 = 0x22 if lives gs+0xc = 0 (game over), else 0x21 (restart).
  - Debug only: with gs+0x30 & 0x4000, pad-2 word gs+0x64 bits 0x400/0x100/0x200 kill the hero; with gs+0x30 & 0x2000, bit 0x800 ends the level (gs+3 = 0x31). gs+0x64 is a pad word written by the EXE (ENG1 only clears it, 0x1f361), consistent with the ENG3 debug bits in chase.md.
- `L08.RE.04` done (2026-10-12, VERIFIED-STATIC): the four Titans are background figures in a fixed order, each loading the next one's bank when it leaves.
  - Position: x = (anchor 0x707f4 − camera x)·65536/6 + 400 (TornadoT /12), so they scroll at 1/6 (1/12) of the camera. Entrance state 0x37 blends the level palette toward the Titan's palette (step 4/255 per tick; LavaT 2) and then sets it as current: RockT 0x636f0, IceT 0x63a78, LavaT 0x63ef8, TornadoT 0x64270. These are the same palettes `HERO_InitFlight` picks by restart index.
  - Poses: `HERO_SelectRocktPose` 0x37fe0 / `HERO_SelectIcetPose` 0x384c0 pick at random (0x2c460(0x8000)): RockT 0x2e (37.5 %), 0x1a (37.5 %), throw 0xd → 0xe (25 %); in 0xe at anim-script step 8 (obj+0x32) it spawns the rock kind 0x39 at (x + 16, y − 96). IceT 0x2e / 0x2f / 0x30. LavaT state 0x31 at script step 13 spawns four lava columns kind 0x33 (y −46/−50/−52/−54); its entrance spawns lava pieces kind 0x56 every 8 blend steps below 0x90 with sfx 0x6a4c/0x6a4d. TornadoT rises (0x2b, 80 ticks, scale +0x2c) and sinks (0x2c); its entrance spawns kind 0x57 every 32 steps, and its exit state spawns kind 0x5b every 16 ticks for 160 ticks.
  - Exit: when x < 0 (RockT/IceT) the state becomes 0x39; after 200 ticks `HERO_RemoveRocktLoadIcet` 0x37fa0 frees banks 49 RockT and 59 RcRock and requests 50 IceT, `HERO_RemoveIcetLoadLavat` 0x38490 frees 50 and requests 51 LavaT (bank ids from the ENG1 table 0x5cee0; the earlier note "0x37fa0 LavaT 50" was off by one Titan). LavaT and TornadoT free themselves below x −40 (banks 51 / 52).
  - Damage: no Titan touches the hero itself. The hazards they spawn call `HERO_ApplyDamageInFlight` with level 3 (LavaT part 0x32) or 2 (0x34, 0x5a, 0x52/0x53, 0x3f), and through `HERO_CheckPegasusContact` 0x3c4d0 (Pegasus box: kinds 0x3d, harpy 0x5c, Spy 0x5d) or `HERO_CheckFlightHeroContact` 0x3c510 (hero box: lava pieces 0x56), both level 2. Difficulty enters only through the damage table (Beginner/Medium/Herculean level 2: 10/25/30, level 3: 12/30/38).
  - `GoodbyeRockT`: printed by kind 0x58 (RtRock debris, update 0x383d0) through the empty `DBG_PrintfStub` 0x1000; the debris falls with gravity 0.5 and is freed 300 units right of or 200 below the hero.
- `L08.RE.05` progress 2 (2026-10-12, VERIFIED-STATIC):
  - Bank ownership settled from the anim tables: **Water** (bank 58) is drawn by FOREST kinds 0x31, 0x4b (water actor that scrolls the T3c texture), 0x48 and 0x49; **Griffon** (60) by FOREST kinds 0x46 and 0x47. Neither belongs to TITANS. **Pig / Grif / Merv** (65 / 66 / 67) are TITANS kinds 0x5e / 0x5f / 0x60 (shared handlers 0x3c050 / 0x3c070; they never damage the hero: they fall with gravity 0.5 and are knocked off when the hero's attack box 0x71440 overlaps them while 0x6fec4 is set).
  - Load path: TITANS loads only banks 48 PEGASUS and 2 XTRA_DB1 (level list) plus 63 Harp, 49 RockT, 62 RtRock and 64 Spy (start lists). It has no 0x1000-class bank triggers, and no ENG1 code requests banks 53–57 (gods), 61 (Zeus) or 65–67: every `ANIM_RequestBank` 0x3520 call site is either a constant for another bank or the letter/start-list helpers (0x12790, 0x128a0, 0x3270). `LVL_SpawnActor` 0x270c0 and `HERO_GodSpawn` 0x3abd0 do not request banks either. How (or whether) the god sprites get their banks needs the TITANS play session (session 18): watch the FS opens at each god's x.
  - Still open: scoring and persistence of the rescues (TITLE tally side).
