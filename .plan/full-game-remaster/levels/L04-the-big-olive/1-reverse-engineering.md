# L04 Phase 1 — Reverse engineering

Goal: recover the THEBES-specific code and data. That means the per-level config and type table, the ID-5 start-point table, the spawn/update handlers for every THEBES actor, the rescue/destruction scripts, and any code path that could split THEBES into parts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, start table, type-table layout); L03 phase 1 names for shared ENG1 handlers.
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L04-types.json`; notes in this file.

## Tasks

- [x] `L04.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·5 and list its fields next to TRAINING's and FOREST's. Resolve the type-table pointer (+0x1c).
  - Resolve the start-point table that `HERO_Init` 0x15090 reads (0x5a69c[level] + 0x18·index). Check the hypothesis that this is descriptor field +0xc, which for ID 5 holds 0x10061780.
  - Count the ID-5 start records and decode each one (x, y, z per `docs/gameplay-spec.md`). Mark which records indices 2 and 6 (the `DEMO5_*` starts) select.
  - Identify descriptor fields +0x10 (0x10061738) and +0x14 (0x10061778) from their consumers.
- [x] `L04.RE.02` (P5.ALL.08) For each of the 87 kinds used in the THEBES spawns, decode the type-table entry `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each one loads. Start with the most frequent kinds (0x2018, 0x2019, 0x25, 0x2013, 0x202b). Write `L04-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank or code proves them; otherwise mark them LEAD.
- [x] `L04.RE.03` (P3.ALL.03, P5.ALL.11) **Thebes substage, static side.**
  - Find every writer of the start-index fields (game state +1/+2) and of the level-ID global besides the title/demo path.
  - Find any handler or script that requests a map reload, a level change back to 5, or a mid-level bank/music switch.
  - Find who reads the 8 THEBES blocks' columns as boundaries, if anyone does.
  - Cross-check with S6.02: whether the password decoder produces a start index.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [x] `L04.RE.04` (P5.ALL.04) Recover the THEBES script triggers: background stairs and depth/route switching, harpy and citizen rescue triggers and their scoring, destruction and falling debris, and secret routes. Check whether these are generic ENG1 handlers already named for TRAINING or FOREST (`L03.RE.05`). Reuse those names; don't re-derive them.
- [x] `L04.RE.05` (P5.ALL.08) Decompile the enemy updates: thugs, skeletons, harpies and the Minotaur.
  - states, attacks and damage (`HERO_RequestDamage` 0x266a0)
  - whether the Minotaur has a health bar, phases or a scripted entry, which would make it a boss encounter
  - difficulty reads (S6.01)
- [x] `L04.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to L05 (ID 6) through the shared S6 path.

## Done when

- Every THEBES spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The ID-5 start table is decoded, and every code path that can change the start index or reload THEBES is documented down to the exact RVAs.
- `L04-types.json` exists.

## Notes
- `L04.RE.01` (2026-10-12): `extracted/meta/full-game/L04-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68bc8: actor_pool_count 50 (TRAINING 60); marker_pool_count_minus5 80 (TRAINING 120); pickup_pool_count_minus5 110 (TRAINING 98); actor_kind_funcs 0x60778 (TRAINING 0x67608); marker_kind_funcs 0x60c70 (TRAINING 0x67bd0); pickup_kind_funcs 0x60f58 (TRAINING 0x67f80); spawn_templates 0x61130 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a708 (0x5a690 + 0x18·5):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x61780, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x61738, the level bank list (0x3270, S4.01);
    - +0x14 = 0x61778, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (8; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(288,1164,0), 1=(4848,1164,0), 2=(6304,1036,128), 3=(8816,844,0), 4=(12464,908,128), 5=(15744,1164,0), 6=(14656,1036,128), 7=(18464,908,0).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L04.RE.03` (2026-10-12, VERIFIED-STATIC; byte-store scan of every ENG1 reference to the game state, including code outside defined functions):
  - **Start index gs+1** is written only by `MOD_Tick` (0 at module start), `HERO_DemoLoad`, `HERO_Die`, `HERO_KillInstant` and `HERO_SetEndSequence` (each copies gs+2).
  - **Restart index gs+2** is written by `MOD_Tick` (0), `HERO_DemoLoad`, `LVL_CheckpointTick` 0x139f0, `LVL_RestartTriggerTick` 0x42bd0 (TRAINING) and `LVL_NessusTriggerTick` 0xc7e0 (FOREST).
  - **Level id gs[0]** is written in ENG1 only by `MOD_ExitCheck` (0xff on quit).
  - **Map/bank/music switches:** no map reload outside module start (`LVL_LoadMap` only in `MOD_Tick` init). The only mid-level bank requests are the Nessus code, TITANS 0x37fa0/0x38490 and `HERO_SetEndSequence` (`bank-requests.json`). The only mid-level music switch is FOREST's Nessus stream.
  - **Blocks:** columns are used only by block streaming (`LVL_LoadBlock` 0x1f9e0, `LVL_StreamBlocks`), not as boundaries.
  - **Passwords:** `HERO_PasswordCheck` sets gs[0] (via 0x166e6), +0xe and +0x12, never a start index. Runtime (gamehercules-40, `reference/passwords.json`): both Thebes passwords start THEBES at start 0, and only gs+0x12 differs.
  - **Conclusion:** this build has no Thebes substage mechanism. "Part 2" is a password-index difference only. The THEBES tick 0x33720 starts boss gauge state 2 when the hero is in x 0x1510–0x1780, y > 0x440 and 0x6ffda < 8 (a gauge event, RE.04).
- `L04.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - ENG1 side: the level exit is pickup kind 0x200a at x 18912 (`LVL_LevelExitTick` 0x13880): when the hero box touches it, sfx 0x4012, end flag 0x72170 (0x19240) and sparkles → `HERO_SetEndSequence` → opcode −9 → gs+3 = 0x31 → default exit path.
  - TITLE side (gamehercules-40, `docs/full-game/gameplay/progression.md`): exit 0x31 → tally state 0xd2 → 0xd3 post-tally FMV 0xd → 0x29 (gs[0] += 1 at 0x167ae) → card SEQ9 (HYDRA) → `MOD_StartLevelModule` → ENGINE1 HYDRA (ID 6).
  - A traced level end is still to come (phase 2).
- `L04.RE.02` done (2026-10-12): `extracted/meta/full-game/L04-types.json` covers every map kind plus the level's code-spawned kinds (97 rows). Per kind: class (kind & 0x7000), count, positions, type-table RVAs and current Ghidra names, the sprite bank (anim-table bank id → obj+0x22; 48 kinds have one) and constant sound ids (`tools/level/handler_sounds.py`).
  - Handlers are named from their bank when every user shares it (`tools/level/name_handlers.py` → `analysis/annotations/full_game_types.json`, 203 handlers across levels). 48 kinds now have named handlers; 0 are stubs (0xc720).
  - Unnamed, with the next consumer = the handler code listed in the file: 0x0004, 0x0024, 0x003c, 0x003d, 0x0046, 0x0048, 0x0049, 0x202b, 0x202d, 0x0035, 0x0041, 0x004a.
  - Not spawned by `LVL_SpawnVisible` (0x1000-class): 0x1003, 0x1005, 0x1015, 0x1016, 0x1018, 0x1019, 0x101b, 0x101c, 0x101d, 0x101e, 0x1024, 0x1025, 0x1028, 0x1029, 0x140c, 0x1440, 0x1480, 0x1803, 0x1805, 0x1814, 0x1815, 0x1816, 0x1817, 0x1818, 0x1819, 0x181a, 0x181b, 0x181c, 0x181d, 0x181e, 0x1824, 0x1825, 0x1828, 0x1829, 0x1c0c, 0x1c40, 0x1c80.
- `L04.RE.05` progress (2026-10-12): static digest in `docs/full-game/gameplay/actor-digest.md` § L04 (`L04-enemies.json`).
  - Thugs 0x26/0x27 (bank THUGS; hit points 9 / 5; 0x27 contact `HERO_RequestDamage` n = 3 → 12/30/38, corrected 2026-10-12; hittable).
  - Skeletons 0x3e ×18 (SKELETON; hit points 3; strength 2; sfx 0x51dd; one difficulty read).
  - Harpies 0x38 ×8 (hit points 3, hittable, one difficulty read; 15 behaviour states) and 0x47 (strength 2).
  - Minotaur 0x37 (spawn = `HERO_EnemyInitHp` 0x24270: hit points = difficulty·2 + 6; strength 2; 3 difficulty reads) and 0x43 (code-spawned).
  - Open: the per-state behaviour of each (helper calls the digest doesn't follow).
- `L04.RE.04` done (2026-10-12; VERIFIED-STATIC unless marked LEAD; kinds from `L04-types.json`, banks from the anim table):
  - **Citizen rescues:** citizens in a disaster, one kind per disaster bank:
    - snow 0x2a ×4 (THSNOW), earthquake 0x2b ×3 (THEQ), flood 0x2c ×3 (THFLOOD), flash-fire 0x28 ×3 (THFLASH);
    - charred 0x2d/0x3f/0x40 (THCHAR), plus THEWGUY 0x34, THOLD 0x33, THFAT 0x44 and the food vendors 0x39–0x3b (THHAFOOD, code-spawned).
    - Rule (snow citizen `HERO_ThsnowUpdate` 0x33990): calls for help (sfx 0x5140 every 90–149 ticks while the hero is within 32 z). The rescue needs no button: hero within 128 x and on the same depth (±8 z), not yet rescued (+0x97).
    - On rescue: spawns kind 0x4a (a countdown sparkle cycling frames 0x14..5, 0x33e10/0x33e80), attaches a sparkle 0xa001, sfx 0x4012, anim set 7. The citizen then leaves (sfx 0x5941/0x5942).
    - The other disaster kinds use the same bank-per-kind pattern. Scoring/tally counts are TITLE-side (`progression.md`).
  - **Destruction and debris:**
    - cracks 0x46 ×3 (`LVL_CrackUpdate`, as in FOREST);
    - 0x49 ×8 (0x36150/0x361a0): multi-part objects whose parts (child list +0x120) rise to a rest height (state 0) and then act; hit points 50 per the digest. LEAD: collapsing structure / falling debris.
  - **Carried prop:** 0x04 at x 17608 (0x14790/0x14850) follows the hero and changes with the hero's carry states 0x2f/0x30 (0x6ef98), sfx 0x4151. LEAD: an object the hero carries.
  - **Gauge event:** 0x48 at x 6145 (0x10380/0x103c0) only acts in level 5 when counter 0x6ffda = 9. The THEBES tick 0x33720 shows the gauge for x 0x1510–0x1780, y > 0x440 while 0x6ffda < 8: a counted objective with an on-screen gauge (LEAD: the rescue tally).
  - **Background stairs / depth routes:** no THEBES-specific object. Depth changes are generic hero code (cell attribute flag 2 blocks).
  - **Streaming:** class-0x1000 triggers request and free the Thebian banks along the route (`eng1_map_triggers`, e.g. 36/21 at x 576–640, freeing 27/23/36 at 2112).
- `L04.RE.05` done (2026-10-12, VERIFIED-STATIC): enemy updates. Shared pattern: `HERO_FindHitSource` returns 0x80 for a hero punch (damage = 0x19b40(~0x72168 & 0x5f), the attack's strength) or the thrown object (damage 100, spark 0x30290); `ANIM_StepObjectScript` drives the poses; the hero hit box is the object 0x7217c box +0x88; `HERO_RequestDamage(obj, n, 0)` stores n + 1 and `HERO_ApplyDamage` 0x18c50 indexes table 0x55348 with ((stored & ~0x10) − 1) & 7, so n = 3 → index 3 → 12/30/38 (Beginner/Medium/Herculean); runtime: the THEBES attract demo (Beginner) shows a 12-point drop when the Minotaur appears (tick 19940).
  - **Thugs** 0x26 `HERO_ThugsUpdate` 0x10f0 (0x27 `HERO_ThugsUpdate2` 0x13c0 is the same pattern): freed when 380 units from the view (0x2a9e0); while carried (+0x25 & 1) only 0x2a290 runs. Hit: hit points −= damage, anim 0xf (anim 0x13 if already knocked, state ≥ 8), 5 ticks grace (+0xec). Hit points < 1 → anim 0x12 and not hittable (+0x50 &= ~0x200). Walks until the cell 48 units ahead has attribute bits 0xc2 or the floor test fails, then turns (anim 1). In state 2 touching the hero → anim 6 (swing); the swing box (part 2) on the hero → `HERO_RequestDamage` strength 3, or 0x23 on script frame 0xc (index 3 too, plus flag 0x20).
  - **Skeletons** 0x3e `HERO_SkeletonUpdate` 0x352f0: freed at 128 units from the view; hit → anim 0x12 with 40 ticks grace; hit points < 1 → anim 0x15 plus bone effect 0x2027 at y − 32. Turns at cells with attribute bit 2. In state 3 touching the hero → anim 6; boxes 3 and 2 on the hero → strength 3 and sfx 0x51dd at most every 20 ticks.
  - **Harpies** 0x38 `HERO_HarpiesUpdate` 0x105a0: one helper per state (1 0x10800, 2 0x108e0, 3/6 0x109e0, 4 0x10b10, 5 0x10bd0, 7 0x10dc0, 8 0x10e70, 9 0x11040, 10 0x112d0, 11 0x111a0, 12/13 0x113c0; 14/15 only step the script, 15 pins y to 384); z clamped to −128..767. Hit → anim 0x1b (state 1) or 0x1e, 20 ticks grace. Hit points < 1 → anim 0x24, state 0xe, drops what it carries (+0xe8 link cleared) and sets the gauge step 0x72184 = 256 / (difficulty·6 + 7) + 1.
  - **Minotaur** 0x37 `HERO_MinotaurUpdate` 0x24320 (hit points from `HERO_EnemyInitHp` 0x24270 = difficulty·2 + 6): state 8 moves z −4 per tick. While the hero attacks (0x72188) and state < 7, a hit sets the boss-gauge step 0x72184 = 256 / (difficulty·2 + 6) + 1 (300 on the last hit point) and plays the companion object 0x71e28 anim 0x1a, or anim 0x1d when the gauge 0x70832 would run out (then attribute table entries +0x16/+0x18 = 0x10/0x20). Attack bit 0x20 (0x72012) → 0x24c60. Touching the hero box → strength 3. A thrown object (0x71a18 +0x60) that overlaps it is destroyed (0x20000, freed).
