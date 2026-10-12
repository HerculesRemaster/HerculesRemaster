# L05 Phase 1 — Reverse engineering

Goal: recover the HYDRA-specific code and data. That means the per-level config and type table, the handlers behind the two kind-0x31 spawns, the Hydra state machine, how the Hydra is drawn (models and/or sprites), and the arena scripts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.01 (T28 model record, `GFX_DrawModel` 0x31540); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_`, `GFX_` prefixes, `save_program`); `extracted/meta/full-game/L05-types.json`; notes in this file.

## Tasks

- [x] `L05.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·6 and list its fields next to TRAINING's and THEBES's. Resolve the type-table pointer (+0x1c) and the ID-6 start-point table (check the hypothesis that descriptor field +0xc, 0x10058878, is what `HERO_Init` 0x15090 reads). Identify descriptor fields +0x10 and +0x14 from their consumers.
- [x] `L05.RE.02` (P5.ALL.08) Decode the type-table entry for kind 0x31 `[spawn, stub 0xc720, update]` (12 B). Name the handlers, and find what templates 11 and 12 select. List every bank, model and sound the handlers load. Write `L05-types.json`. Put actor names only where a bank or code proves them.
- [x] `L05.RE.03` (P5.ALL.08) Decompile the Hydra update(s):
  - find who loads `P:\SRC1\BOSS\HYDRA` (string 0x1005d548), and from which handler
  - states and transitions, attacks and their hit boxes (`HERO_RequestDamage` 0x266a0)
  - the per-head or per-part structure, if any: count, cut condition, regrowth (LEAD), and whether parts are separate actors
  - health, vulnerable windows, phase changes (only those the code supports), defeat
  - difficulty reads (S6.01)
- [x] `L05.RE.04` (P5.ALL.02, with S2.01) Determine how the Hydra is drawn: T28 model calls from the Hydra handlers into `GFX_DrawModel` 0x31540, sprite frame requests for bank 0804, or both. Record which T28 records HYDRA's map uses and which handler animates them. Record how the arc camera records (type 1) are consumed in the arena.
- [x] `L05.RE.05` (P5.ALL.04) Recover the arena scripts: boundaries, camera, any crowd/NPC or hazards, and the trigger that starts the fight. Reuse generic ENG1 handler names from TRAINING/FOREST/THEBES.
- [x] `L05.RE.06` (P5.ALL.11) Recover the defeat → completion trigger and the hand-off to tally and to L06 (ID 7) through the shared S6 path.

## Done when

- Kind 0x31 maps to named handlers, and the Hydra's code path is documented down to the exact RVAs.
- The draw path (models/sprites) is known.
- `L05-types.json` exists.

## Notes
- `L05.RE.01` (2026-10-12): `extracted/meta/full-game/L05-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68c10: actor_pool_count 38 (TRAINING 60); marker_pool_count_minus5 100 (TRAINING 120); pickup_pool_count_minus5 96 (TRAINING 98); actor_kind_funcs 0x57f10 (TRAINING 0x67608); marker_kind_funcs 0x582e0 (TRAINING 0x67bd0); pickup_kind_funcs 0x585c0 (TRAINING 0x67f80); spawn_templates 0x587a0 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a720 (0x5a690 + 0x18·6):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x58878, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x58860, the level bank list (0x3270, S4.01);
    - +0x14 = 0x58870, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (2; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(2240,992,128), 1=(2240,768,128).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L05.RE.02` progress (2026-10-12, VERIFIED-STATIC): the map's two kind-0x31 spawns are the background pillars (`FUN_1001dd00`: model 9 at x 0x394, or model 10 at x 0xdea when x > 0x8b0; update 0x22180 is empty).
  - The fight is created by code: `LVL_InitLevelActors` 0x1fd20 (per-level switch, section start) → `LVL_InitHydra` 0x1ac10.
  - `LVL_InitHydra` sets up the camera, spawns effects 0x202c/0x202a, controllers kind 0x34/0x35 and the first head through `HERO_HydraHeadPhase` 0x1c3f0 / `HERO_HydraSpawnHead` 0x1c4d0 (kind 0x2d).
  - It plays sfx 0x180/0x193, starts the boss gauge (5) and selects a difficulty table 0x56048 / 0x560e0 / 0x56168 (Beginner/Medium/Herculean) → 0x6f3cc.
  - Head phases (0x6f44c): 0 → slot 7; 1 → slots 3, 1, 5; phases 4–7 replace the cut head and add slots 2, 4, 0, 6; phase 8 adds none.
  - HYDRA-specific actor kinds 0x28–0x35 (type table 0x57f10): `extracted/meta/full-game/L05-types.json` covers only map kinds. Heads use T28 models 11–56 (`models.png`).
- `L05.RE.03` progress (2026-10-12): `docs/full-game/gameplay/encounters.md` § Hydra (partial).
  - Phases 0–8 of the head split.
  - Contact damage `HERO_RequestDamage(head, 0x22)` = index 2 → 10/25/30 (corrected 2026-10-12, was quoted as 8/18/22), from a box built from the model's carried vertices.
  - Cut rule: attack box 2 near the neck, slot +0x49 ≤ 0x38, attack bits & 0x18.
  - Drops and screams.
  - Names: `HERO_HydraHeadSpawn` 0x1b2a0, `HERO_HydraHeadUpdate` 0x1b7f0, `HERO_HydraHeadAI` 0x1b370, `HERO_HydraCutHead` 0x1d6c0.
  - Open: controllers 0x34/0x35 (attack patterns), the table 0x56048 layout, the win condition.
- `L05.RE.03` done (2026-10-12): `encounters.md` § Hydra.
  - Structure: 8 slots of neck kind 0x29 + head kind 0x2d (cut head 0x30). Neck states 0/1/2/3/4.
  - Damage: strike and contact n = 0x22 → index 2 → 10/25/30 (corrected 2026-10-12).
  - Cut window: +0x49 0x26..0x38, attack box 2 near the neck.
  - Win: 8 cuts (counter 0x6efb8, gauge 32 per cut) → 0x72014 → `LVL_TickHydra` explosions → end flag → 0x31 → tally (also covers the ENG1 side of RE.06).
  - Difficulty only changes the drop stream 0x56048/0x560e0/0x56168.
  - The BOSS\HYDRA bank comes from the level list.
- `L05.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - ENG1 side: 8th cut (`HERO_HydraCutHead`) → 0x72014 → `LVL_TickHydra` 0x1ae00 explosions (sequence 0x6f3e0) → end flag 0x72170 → `HERO_SetEndSequence` → opcode −9 → gs+3 = 0x31 → default exit path.
  - TITLE side (gamehercules-40, `docs/full-game/gameplay/progression.md`): exit 0x31 → tally state 0xd2 → 0xd3 post-tally FMV 0xe → 0x29 (gs[0] += 1 at 0x167ae) → card SEQ2 (MEDUSA) → `MOD_StartLevelModule` → ENGINE1 MEDUSA (ID 7).
  - A traced level end is still to come (phase 2).
- `L05.RE.02` done (2026-10-12): `extracted/meta/full-game/L05-types.json` covers every map kind plus the level's code-spawned kinds (14 rows). Per kind: class (kind & 0x7000), count, positions, type-table RVAs and current Ghidra names, the sprite bank (anim-table bank id → obj+0x22; 5 kinds have one) and constant sound ids (`tools/level/handler_sounds.py`).
  - Handlers are named from their bank when every user shares it (`tools/level/name_handlers.py` → `analysis/annotations/full_game_types.json`, 203 handlers across levels). 7 kinds now have named handlers; 0 are stubs (0xc720).
  - Unnamed, with the next consumer = the handler code listed in the file: 0x0031, 0x002a, 0x002b, 0x002c, 0x002e, 0x002f, 0x0033.
  - Not spawned by `LVL_SpawnVisible` (0x1000-class): none.
  - Kind 0x31 = pillars (`FUN_1001dd00`). Template 11 → model 9 at x 0x394, template 12 → model 10 at x 0xdea (x > 0x8b0 rule); update 0x22180 is empty. The HYDRA-specific kinds 0x28–0x35 are listed as code-spawned.
- `L05.RE.04` done (2026-10-12, VERIFIED-STATIC):
  - **Both.** The necks (kind 0x29) and heads (0x2d) have anim-table bank 0 and are drawn as multi-part T28 model objects (`GFX_DrawModelList`/`GFX_DrawModelChildren` 0x31330/0x31470 → `GFX_DrawModel` 0x31540).
  - Their parts are animated by the model animator `ANIM_StepModelParts` 0x28aa0. It works on the child list obj+0x128 with 6-byte `{frame/opcode, …}` records; the head spawn sets part table 0x56240 (count 9, then `{part, model}` byte pairs).
  - The poses are T28 models 11–56. Bank 79 `BOSS\HYDRA` sprites are only effects and the cut-head kind 0x30 (anim-table bank 79).
  - The map itself uses T28 models 9/10 (pillars, kind 0x31 templates 11/12).
  - **Camera:** 96 of HYDRA's 99 camera records are arcs (type 1, radius 0x28c; `LVL_UpdateCamera` 0x2f840 / `LVL_PlaceOnPath` 0x32300, `docs/level-format.md`). `LVL_InitHydra` sets the camera mode 0x70d10 = 0x1f and offsets 0x70ce0/0x70d04/0x70d02.
- `L05.RE.05` done (2026-10-12, VERIFIED-STATIC):
  - **Start:** there is no start trigger. The fight is created at section start by `LVL_InitLevelActors` → `LVL_InitHydra` and runs per tick in `LVL_TickHydra`.
  - **Boundaries:** cell attribute 1 marks the two arena wall columns (flags & 7 block movement; `preview_collision.png`). The neck strike aim is clamped to x 0x480..0xcf0.
  - **Camera:** set by `LVL_InitHydra` (mode 0x70d10 = 0x1f, 0x70ce0 = 0xa0, 0x70d04 = 0xffa0, 0x70d02 = 0x20). The arena is 96 arc camera records.
  - **Hazards and props (HYDRA type table):**
    - 0x2f tumbling debris (`HERO_HydraDebrisUpdate` 0x1e3f0, bounces at y 0x3d8, sfx 0x188–0x18a);
    - 0x33 falling boulder (`HERO_HydraBoulderUpdate` 0x1e600). Hitting the hero calls `HERO_RequestDamage(rock, 0)` = index 0 → 5/12/15, the lightest entry, and no recoil (stored hit 1 < 2 sets the light-hit flag; corrected 2026-10-12, was quoted as 22/50/60); otherwise it lands at y 0x3d8.
    - 0x2a pillar variant (model 9/10, flags 0x12000000, update empty).
    - 0x2c arena stage controller (`HERO_HydraStageTick` 0x1ed30: stages 0x6f4bc 0..5, table 0x589f8, sfx 0x192).
    - 0x2b state-table dispatcher (0x1c200 → table 0x562d0).
    - 0x2e neck segment follower (0x1c2a0, interpolates between part tables 0x56338).
    - 0x28 ALIEN1 bank actor (`HERO_Alien1Spawn`/`Update` 0x1dbc0/0x1dc00).
    - Intro effects 0x34/0x35.
  - **Crowd/NPC:** the fleeing children are frames 78–104 of bank 79. The kind that shows them is the ALIEN1-bank 0x28 or a bank-79 kind (0x30/0x32); that is decided at runtime (S4.05 anim events).

