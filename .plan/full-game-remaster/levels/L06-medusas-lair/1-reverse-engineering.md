# L06 Phase 1 — Reverse engineering

Goal: recover the MEDUSA-specific code and data. That means the per-level config and type table, the handlers behind the six kind-0x0 spawns, the skeleton enemies, the Medusa state machine with its gaze/projectile attacks and defeat mechanic, and the lair scripts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.01 (T28 model record, `GFX_DrawModel` 0x31540); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_`, `GFX_` prefixes, `save_program`); `extracted/meta/full-game/L06-types.json`; notes in this file.

## Tasks

- [x] `L06.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·7 and list its fields next to TRAINING's and HYDRA's. Resolve the type-table pointer (+0x1c) and the ID-7 start-point table (check the hypothesis that descriptor field +0xc, 0x1005a030, is what `HERO_Init` 0x15090 reads). Identify descriptor fields +0x10 and +0x14 from their consumers.
- [x] `L06.RE.02` (P5.ALL.08) Check how the ENG1 spawn reader interprets the six MEDUSA spawns (all kind 0x0, flags 0), and compare with `tools/level/level_decode.py`. Then decode the type-table entry for the actual kind(s) `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each loads. Write `L06-types.json`. Put actor names only where a bank or code proves them.
- [x] `L06.RE.03` (P5.ALL.08) Decompile the Medusa update(s):
  - find who loads `P:\SRC1\BOSS\MEDUSA` (string 0x1005d574), and from which handler
  - states and transitions; gaze and projectile/venom attacks (LEAD): telegraph, hit test, and the effect on the hero (state change or `HERO_RequestDamage` 0x266a0)
  - the defeat mechanic: whether a reflection is involved, which object or hero action triggers it, and how it damages Medusa
  - health, vulnerable windows, phase changes (only those the code supports), defeat
  - difficulty reads (S6.01)
- [x] `L06.RE.04` (P5.ALL.08) Skeleton enemies: find who loads `P:\SRC1\ALIEN1\SKELETON` (0x1005d938) and `P:\SRC1\BOSS\SKELWAR` (0x1005d55c), and which level's type table references those handlers. This settles whether SKELWAR belongs to L06. Decompile their states, attacks and defeat (including any revive behavior, if the code has one).
- [ ] `L06.RE.05` (P5.ALL.02, P5.ALL.04, with S2.01) Lair scripts and drawing: T28 model use in the lair and by Medusa, the one animated texture, the arc camera records, depth routes, props/pickups (including any mirror/shield prop the reflection mechanic needs), and the trigger that starts the Medusa fight.
- [x] `L06.RE.06` (P5.ALL.11) Recover the defeat → completion trigger and the hand-off to tally and to L07 (ID 8, ENG3) through the shared S6 path, including the module change request (TITLE 0x16300 → EXE 0x4040).

## Done when

- Every MEDUSA spawn maps to named handlers (or is marked unknown, with the next consumer to check).
- The Medusa and skeleton code paths are documented down to the exact RVAs, including the defeat mechanic.
- `L06-types.json` exists.

## Notes
- `L06.RE.01` (2026-10-12): `extracted/meta/full-game/L06-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68c58: actor_pool_count 80 (TRAINING 60); marker_pool_count_minus5 40 (TRAINING 120); pickup_pool_count_minus5 148 (TRAINING 98); actor_kind_funcs 0x59720 (TRAINING 0x67608); marker_kind_funcs 0x59af8 (TRAINING 0x67bd0); pickup_kind_funcs 0x59de8 (TRAINING 0x67f80); spawn_templates 0x6fe60 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a738 (0x5a690 + 0x18·7):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x5a030, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x5a018, the level bank list (0x3270, S4.01);
    - +0x14 = 0x5a028, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (2; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(4928,640,128), 1=(768,128,0).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L06.RE.02` progress (2026-10-12, VERIFIED-STATIC): the six map spawns have kind 0 (template kind 0). The fight is created by `LVL_InitLevelActors` 0x1fd20 → `LVL_InitMedusa` 0x21050, which:
  - places the hero at (0x3b8, −, 0x4c);
  - spawns Medusa kind 0x2e at (0, 0x2f0, 0x280) with frame 0x3d (type entry +0x228), plus effect 0x202d;
  - plays sfx 0x1c6 and sets 0x6fe28 = 3;
  - selects difficulty tables (Beginner 0x58b08/0x58ce0, …) → 0x6fcf8/0x6fe0c.
  - MEDUSA-specific kinds are 0x2d–0x35 (type table 0x59720); spawners in the code: 0x22190 (kind 0x2f), 0x23040 (0x2d), 0x22fe0 (0x30), 0x21210 (0x32), 0x21650 (0x35), 0x21aa0 (0x34), 0x23390 (0x33).
  - MEDUSA has no background hook.
- `L06.RE.03` progress (2026-10-12): `docs/full-game/gameplay/encounters.md` § Medusa (partial).
  - `HERO_MedusaUpdate` 0x21320: phase 1 (`HERO_MedusaPhase1` 0x22280, two attack slots via `HERO_MedusaAttackSlot` 0x22440) and phase 2 (`HERO_MedusaCollapse` 0x21aa0, falling rocks kind 0x34 and debris).
  - Open: the slot attacks and how they end (reflection LEAD), the skeleton warriors, the win/exit.
- `L06.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - ENG1 side: when both phase-1 attack slots are spent, `HERO_MedusaPhaseEnd` 0x220c0 spawns kind 0x31, `HERO_MedusaDefeatTick` 0x21400 (MEDUSA type table).
    - State 2: when timer 0x6fe2c runs out, debris kind 0x35 ×4 (0x21650) plus 100 particles (0x21590); flags cleared on Medusa; 0x7f8e(0x1c6); sfx 0x1ea; timer 90.
    - State 3: when the timer runs out, `HERO_RequestLevelEnd` 0x19240 → end flag → `HERO_SetEndSequence` → opcode −9 → gs+3 = 0x31 → default exit path.
  - TITLE side (`progression.md`): tally 0xd2 → 0xd3 post-tally FMV 0x11 → 0x29 gs[0] = 8 → card SEQ3 → `MOD_StartLevelModule` → **ENGINE3** CYCLOPS (ID 8).
  - A traced level end is still to come (phase 2).
- `L06.RE.02` done (2026-10-12): `extracted/meta/full-game/L06-types.json` covers every map kind plus the level's code-spawned kinds (10 rows). Per kind: class (kind & 0x7000), count, positions, type-table RVAs and current Ghidra names, the sprite bank (anim-table bank id → obj+0x22; 7 kinds have one) and constant sound ids (`tools/level/handler_sounds.py`).
  - Handlers are named from their bank when every user shares it (`tools/level/name_handlers.py` → `analysis/annotations/full_game_types.json`, 203 handlers across levels). 7 kinds now have named handlers; 1 are stubs (0xc720).
  - Unnamed, with the next consumer = the handler code listed in the file: 0x002f, 0x0030.
  - Not spawned by `LVL_SpawnVisible` (0x1000-class): none.
  - The six MEDUSA map spawns (templates 1 and 6) have kind 0. The MEDUSA type-table entry for kind 0 is the stub 0xc720 for spawn and update, so they do nothing; the decoder reads them the same way. The fight comes from `LVL_InitMedusa` (RE.03).
- `L06.RE.04` done (2026-10-12, VERIFIED-STATIC):
  - **Banks:** SKELWAR (bank 78) is drawn by MEDUSA kind 0x32 (anim-table bank id); the MEDUSA level list loads it (S4.01). So SKELWAR belongs to L06. Spawned by code: `LVL_TickMedusa` 0x21210 → kind 0x32. SKELETON (bank 37) is THEBES kind 0x3e (18 map spawns, `HERO_SkeletonSpawn` 0x35200 / `HERO_SkeletonUpdate` 0x352f0), so it is L04 material.
  - **Skeleton warrior:** `HERO_SkelwarSpawn` 0x23a50 faces the hero, takes the hero's z, has hit points +0xee = 4 and 50 invulnerable ticks (+0xec), gets attached child 0xa02e (frame 0x9f), alive counter 0x6fde8 + 1.
  - **`HERO_SkelwarUpdate` 0x23b30:**
    - State 0: rises (sfx 0x51d5). State 1: script steps (sfx 0x51d9 walk, …).
    - It moves toward the hero by 0x40000 per tick on the walk script 0x58f18.
    - Contact (attack box mode 2 vs hero box, 0x201b0) → `HERO_RequestDamage(skel, 1)` → strength 0 → 5/12/15, sfx 0xd1dd.
    - A hero punch (0x80) removes what 0x19b40 returns for ~0x72168 & 0x7f, with hit script 0x590d0 and sfx 0xd1de. A thrown object or projectile removes 0x80 (instant). 5 invulnerable ticks after a hit.
  - **Death:** hit points < 1, or Medusa's counter 0x6fe20 ≤ 0 (all skeletons die). sfx 0xd1df; a drop (0x24090 → item | 0x280) unless the boss is beaten (0x72014); alive counter − 1; 1-in-8 chance of a hero voice line 0x27/0x28/0x29 (0x2c4c0).
  - No revive code.
- `L06.RE.05` progress (2026-10-12, VERIFIED-STATIC):
  - **Start:** there is no trigger; `LVL_InitMedusa` runs at section start.
  - **Skeletons:** `LVL_TickMedusa` 0x21210 spawns a skeleton warrior (kind 0x32) every 60–179 ticks (first after 500), while 0x6fe20 > 0 (8 at init) and fewer than 3 are alive (0x6fde8). Position: hero x ± random 0x60 standing, or 496–559 units ahead when moving; y 0x2e0.
  - **Drawing:** Medusa is T28 model 61 (frame 0x3d) following the hero's x. The lair has 1 animated VRAM rect (T38) and 77 of 80 camera records are arcs. No T44 background, no background hook.
  - **Props:** the map has no prop or pickup spawns (6 inert kind-0 spawns), so any mirror/shield prop for the reflection mechanic would have to come from code. Open: `HERO_MedusaAttackSlot` 0x22440 (reflection LEAD).
- `L06.RE.03` done (2026-10-12, VERIFIED-STATIC; `encounters.md` § Medusa):
  - **Bank:** BOSS\MEDUSA (77) comes from MEDUSA's level list (`ANIM_LoadLevelBanks`). Kinds 0x2d/0x2e/0x31/0x33/0x34/0x35 draw from it.
  - **Medusa (kind 0x2e)** follows the hero's x. `HERO_MedusaPhase1` runs two attack slots (`HERO_MedusaAttackSlot` 0x22440). They behave like reaching arms (z − 0x20 per tick), not a gaze beam:
    - Slot state 0 tests the actors in its path. Hero (y > 0x2d0, ±16) → `HERO_RequestDamage(medusa, 3)` (strength 2 = 10/25/30), slot ends. A skeleton warrior is killed.
    - Stone column kind 0x30 (counter 0x6fd14 = 0) → state 1: lifts it (y − 4.0 per tick), breaks it into four falling pieces (0x22fe0 parts 7–10) above y 0x240, drop item 0x289 on even slots, sfx 0xd1c0. The pieces (kind 0x2d, `HERO_MedusaUpdate3` 0x22dd0) fall and bounce on y 0x2d0, raising their counter to 6 when they settle.
    - A settled piece (kind 0x2d, counter 6) within ±32 x → state 2: the slot grabs it and pulls it back (30 ticks), then Medusa's reaction anim 1, sfx 0x1c3/0x1c4 and **0x6fe10 = 1**. This is what the reflection LEAD maps to in code: no mirror or shield object exists.
  - **Health:** at each cycle end (both slots spent) `HERO_MedusaPhaseEnd` 0x220c0 takes one of 8 health units (0x6fe20) and 0x20 of the boss gauge, but only if a grab happened (0x6fe10). At 0 → kind 0x31 `HERO_MedusaDefeatTick` (RE.06).
  - **Collapse:** `HERO_MedusaCollapse` (state 2) drops rocks between cycles.
  - **Difficulty:** only the tables 0x58b08/0x58ce0… chosen in `LVL_InitMedusa`; no other read in these handlers.

