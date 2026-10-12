# L03 Phase 1 — Reverse engineering

Goal: recover the FOREST-specific code and data. That means the per-level config and type table, the spawn/update handlers for every FOREST actor, and the code path that handles Nessus and ID 4.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L03-types.json`; notes in this file.

## Tasks

- [x] `L03.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·3 and list its fields next to TRAINING's. Resolve the type-table pointer (+0x1c) and the start-point table (0x5a69c-style records for ID 3, as `HERO_Init` 0x15090 uses them).
- [x] `L03.RE.02` (P5.ALL.08) For each kind used in the FOREST spawns, decode the type-table entry `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each one loads. Write `L03-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank or code proves them; otherwise mark them LEAD.
- [x] `L03.RE.03` (P3.ALL.03, P5.ALL.01) Nessus and ID 4.
  - Find every reader of descriptor 0x5a6f0, TITLE 0x234e0, and the `O:\NESSUS\` / `q:\NESSUS\` / `q:\HERC_NES\` strings.
  - Find who loads the `NESSUS\*` bank paths.
  - Determine whether FOREST's script or a type handler requests a level change to 4, or a mid-level bank/sound switch.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [x] `L03.RE.04` (P5.ALL.08) Decompile the Nessus actor update(s):
  - states, and the transitions punch → kick → look → ride → reject/finish/defeat (bank names are only a starting point)
  - health, and the damage rules, including "weapons don't hurt" (LEAD)
  - ride/mount handling with the hero
  - the Power Punch finish condition
  - difficulty reads (S6.01)
- [x] `L03.RE.05` (P5.ALL.04) Recover the FOREST script triggers: background-path switching (layer/depth change), climbable trees, breakable stone barriers and cracks, and secret routes. Check whether these are generic ENG1 handlers already named for TRAINING. Reuse those names; don't re-derive them.
- [x] `L03.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to L04 (ID 5) through the shared S6 path.

## Done when

- Every FOREST spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The Nessus code path and the use of the ID-4 descriptor are documented down to the exact RVAs.
- `L03-types.json` exists.

## Notes
- `L03.RE.01` (2026-10-12): `extracted/meta/full-game/L03-types.json` (`tools/level/level_types.py`; field meanings in `extracted/meta/full-game/eng1-level-config.json`, S2.05).
  - Config record 0x68b80: actor_pool_count 60 (TRAINING 60); marker_pool_count_minus5 80 (TRAINING 120); pickup_pool_count_minus5 76 (TRAINING 98); actor_kind_funcs 0x4e268 (TRAINING 0x67608); marker_kind_funcs 0x4e8c8 (TRAINING 0x67bd0); pickup_kind_funcs 0x4ec50 (TRAINING 0x67f80); spawn_templates 0x4ede8 (TRAINING 0x68170).
  - Descriptor ENG1 0x5a6d8 (0x5a690 + 0x18·3):
    - +0 map dir (read by `LVL_LoadMap`/`LVL_LoadBlock`);
    - +4 map header size and +8 = 0, no ENG1 reader;
    - +0xc = 0x4f260, the start table read by `HERO_Init` (confirmed);
    - +0x10 = 0x4f220, the level bank list (0x3270, S4.01);
    - +0x14 = 0x4f258, passed to interface slot 0x6c `AUD_LoadLevelSonix` in `MOD_Tick` (the level's Sonix descriptor; S5).
  - Starts (10; 0x18-byte records: u16 x, u16 y, u8 z·128, +8 bank list): 0=(990,588,0), 1=(5312,464,384), 2=(7648,608,384), 3=(10624,656,512), 4=(14528,608,384), 5=(18496,672,512), 6=(960,672,0), 7=(960,672,0), 8=(960,672,0), 9=(960,672,0).
  - Start k lines up with checkpoint kind 0x400a+k where the map has one (`docs/full-game/gameplay/checkpoints.md`).
- `L03.RE.03` (2026-10-12, VERIFIED-STATIC; runtime use not concluded): the Nessus fight is code inside FOREST (ID 3). ID 4 is never selected by ENG1 code.
  - **ID-4 descriptor** ENG1 0x5a6f0 (`O:\NESSUS\`, header size 0x131000, starts 0x4f360, banks 0x4f350, audio 0x4f358) has no direct reader. It is reached only by indexing 0x5a690 with gs[0] = 4 (`LVL_LoadMap`, `LVL_LoadBlock`, `HERO_Init`, `MOD_Tick`).
  - Neither `M:\GRAFIX\MAPS\NESSUS\MAP_PSX.BIN` nor `.CIF` exists in HERCULES.FS (hash check), so a load of ID 4 has no map.
  - TITLE: 0x234e0 is the "NESSUS BATTLE" record of the level-name table 0x234d4 (12-byte records, data only). The engine table 0x22288[4] = ENGINE1. No password result is 4 (S6.02). Whether the TITLE next-level step can produce 4 is left to S6.05.
  - `q:\HERC_NES\` (ENG1 0x51854, EXE 0x35adc) has no code reference.
  - **Trigger:** FOREST kind 0x53 at x 18520, next to checkpoint/start 5 (18490/18496). Handlers `LVL_NessusTriggerSpawn` 0xc780 and `LVL_NessusTriggerTick` 0xc7e0.
    - When the hero passes it, it starts the FOREST-only audio stream `AUD_ForestStreamTick` 0xe670, which `MOD_Tick` calls only while level = 3. That stream loads `q:\NESSUS\` SFX.SFX, then MUSIC.MUS (`AUD_StreamNessusMusic` 0xebd0).
    - Once loaded, it sets audio state 6, calls `LVL_SpawnNessus` 0xc730 (actor kind 0x2f at hero x + 320) and sets **restart index gs+2 = 5**, so a death restarts at start 5 just before the arena.
    - If the hero goes more than 512 units back, the trigger frees itself and unloads.
  - **Nessus actor:** FOREST type entry for kind 0x2f is `HERO_NessusSpawn` 0xbb10 / stub 0xc720 / `HERO_NessusUpdate` 0xbc50.
  - **NESSUS banks:**
    - Basic (68) and Water (58) come with start 5's bank list (S4.01).
    - The move banks Punch (69), Kick (70), Look (71), … are requested and freed by the Nessus code itself (`ANIM_RequestBank` 0x3520 / `ANIM_FreeBank` 0x3870, ids 0x45..), so they are a mid-level bank switch, not a level change.
  - `HERO_NessusUpdate` reads difficulty (gs+0xe) in its hit counter (RE.04).
- `L03.RE.04` (2026-10-12, VERIFIED-STATIC): `docs/full-game/gameplay/encounters.md` § Nessus.
  - Punch/kick cycles: count = rnd(3) + 1 + difficulty; contact damage strength 4 = 15/35/45.
  - Back-off opens a ride window (0x72120 = 1 → 2 when the airborne hero lands in his box, FOREST-only check in 0x17400).
  - Riding drains the boss gauge 0x70832 by 1 every other tick. Then he throws the hero off (`HERO_NessusThrowOff` 0xbb90).
  - At gauge 0 he is dazed and only a charged Power Punch (0x72012 & 2, charge ≥ 101) finishes him.
  - No hit-source check, so other attacks do nothing.
  - Names: `HERO_BossGaugeTick` 0x12040, `HERO_NessusThrowOff` 0xbb90.
  - Open: rides needed (Ride anim length), kind 0x54 after the defeat.
- `L03.RE.06` progress (ENG1 side, VERIFIED-STATIC): the defeated Nessus (state 0x23 at the end of its anim) calls 0x19240 → end flag 0x72170 = 1.
  - Hero code (0x16840 and others) then runs `HERO_SetEndSequence` 0x19160. End script 0x54b50 ends with opcode −9, which sets gs+3 = 0x31.
  - `MOD_ExitCheck` takes the default path (`MOD_ReleaseLevel`, TITLE message 0x6ef00), and TITLE sends 0x31 to the tally (state 0xd2, gamehercules-40's `progression.md`).
  - Corrected the same day: an earlier note said gs+3 = 2; that is opcode −51 at record 4, which exits only when no stream is active (`checkpoints.md`).
  - The defeat also spawns decor kind 0x54 (0xc8c0/0xc910).
  - Still open: the TITLE side (tally, then ID 5 THEBES). That is S6.05; this task stays open until then.
- `L03.RE.06` done (2026-10-12, VERIFIED-STATIC):
  - ENG1 side: Nessus defeat (`HERO_NessusUpdate` state 0x23 end) → 0x19240 end flag 0x72170 → `HERO_SetEndSequence` → end script 0x54b50 opcode −9 → gs+3 = 0x31 → `MOD_ExitCheck` default path (`MOD_ReleaseLevel`, TITLE message 0x6ef00).
  - TITLE side (gamehercules-40, `docs/full-game/gameplay/progression.md`): exit 0x31 → tally state 0xd2 → 0xd3 post-tally FMV 0xc → 0x29 (gs[0] += 1 at 0x167ae; 4 is forced to 5 at 0x168a9) → card SEQ1 (THEBES) → `MOD_StartLevelModule` → ENGINE1 THEBES (ID 5).
  - A traced level end is still to come (phase 2).
- `L03.RE.02` done (2026-10-12): `extracted/meta/full-game/L03-types.json` covers every map kind plus the level's code-spawned kinds (70 rows). Per kind: class (kind & 0x7000), count, positions, type-table RVAs and current Ghidra names, the sprite bank (anim-table bank id → obj+0x22; 46 kinds have one) and constant sound ids (`tools/level/handler_sounds.py`).
  - Handlers are named from their bank when every user shares it (`tools/level/name_handlers.py` → `analysis/annotations/full_game_types.json`, 203 handlers across levels). 47 kinds now have named handlers; 0 are stubs (0xc720).
  - Unnamed, with the next consumer = the handler code listed in the file: 0x0009, 0x004c, 0x004e, 0x004f.
  - Not spawned by `LVL_SpawnVisible` (0x1000-class): 0x102b, 0x103c, 0x1044, 0x1402, 0x1418, 0x1420, 0x1440, 0x181b, 0x1826, 0x182a, 0x182b, 0x182d, 0x182e, 0x183c, 0x1844, 0x1c02, 0x1c18, 0x1c20, 0x1c40.
- `L03.RE.05` done (2026-10-12, VERIFIED-STATIC; FOREST kinds from `L03-types.json`):
  - **Stone barriers:**
    - 0x4c ×6 (`LVL_BarrierUpdate` 0xa010): hit points +0xee; each hero punch accepted by the attack bits (~0x72168 & 0x5f) removes 1; a charged Power Punch (bit 2 with charge 0x7082c > 200) or a projectile breaks it at once. Break: sfx 0xcf, debris `FUN_10009db0`, flag 0x6ef90.
    - 0x4e ×4 (`LVL_LayerBarrierUpdate` 0xa490): the same, but only on the hero's depth layer, and its hit points reset to 50 whenever the hero is more than 160 units away (so it must be broken in one approach).
  - **Cracks:** 0x4f ×2 (`LVL_CrackUpdate` 0xfa00). When the hero stands on it (rider list +0xe8) within 64 units and presses the 0x20 attack bit (ground slam), dust 0x2027; states 1/2 swap the collision box tables 0x52050/0x520a8.
  - **Nessus arena:** 0x09 at x 18456 (`LVL_NessusArenaTick` 0x14e10). When the hero passes it, it arms the Nessus spawn (0x6ef14 = 1), focuses the camera (0x734f4) and locks the left camera limit (0x70ce8). While a Nessus splash is pending (0x72178) it hurts the hero with `HERO_RequestDamage(Nessus, 0x22)` (strength 1), except in the hero's protected states.
  - **After Nessus:** Meg 0x55 at x 19110 (`HERO_MegSpawn`, bank 76) and the decor kind 0x54.
  - **Climbable trees / background paths:** no FOREST-specific object. Depth changes are generic hero code (layer index (z + 0x40) >> 7, blocked by cell attribute flag 2; `LVL_CellAttribute`). Climbing uses the cell collision profiles (S2).
  - **Streaming:** the class-0x1000 trigger kinds load and free banks along the route (`animation-coverage.json` `eng1_map_triggers`).
  - **Secrets:** the item-bit pickups 0x201a..0x201d (one each) and the letters 0x200b (9).

