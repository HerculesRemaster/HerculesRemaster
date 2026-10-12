# L02 Phase 1 — Reverse engineering

Goal: recover the GAUNTLET-specific code and data on top of S3. That means what the ID 2 descriptor and any per-level config point to, the handlers for every GAUNTLET obstacle and actor kind, the crossroads branch data, the pickup/rescue/power-up scripts, and the completion hand-off to L03. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; this phase does not redo them.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01` (named skeleton), `S3.02` (descriptor use), `S3.03` (`i1.a` structs); `S3.04` GAUNTLET `level.json` for the kinds actually present.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L02-types.json`; notes in this file.

## Tasks

- [x] `L02.RE.01` (P5.ALL.01) Read descriptor 0x38558 field by field using the S3.02 consumer map: +0 path (0x10038660), +4 0x129e28, +8 zero, and the pointers +0xc 0x100382c8, +0x10 0x100382b8, +0x14 0x100382c0 (the last is passed to `FUN_1001fa40` in `MOD_Tick` init). Dump what each pointer targets for ID 2 and lay it next to IDs 8 and 10. If S3.02 finds a per-level config table, record the ID 2 record and its type-table pointer.
- [x] `L02.RE.02` (P5.ALL.08) For each obstacle/actor kind in the GAUNTLET spawn or obstacle tables (from `S3.03`/`S3.04`), decode its handler entry, name the spawn/update/collision handlers and list the banks and sounds each one loads. Write `L02-types.json` as kind → RVAs → bank → count in map. Put hazard names (blade, hammer, stone hand, boulder, rolling stone, pillar, pond platform) only where a bank or code proves them; otherwise mark them LEAD.
- [x] `L02.RE.03` (P5.ALL.09) Crossroads branch data.
  - Find how `i1.a` stores the GAUNTLET track topology: branch records, lane/track tables, and where branches rejoin.
  - Find the code that reads the player's input at a crossroad and selects a branch, and any default when no input is given.
  - Record the branch count and positions from data, not from the walkthrough's "two crossroads".
- [x] `L02.RE.04` (P5.ALL.04) GAUNTLET scripts and power-ups:
  - pickups: coins, HERCULES letters, Herculade bottles, action figures, password vases (names from the walkthrough are LEAD)
  - Hermes Sandals: the speed change and the boulder-smash rule
  - Helmet of Invincibility: activation and its effect on collision/damage
  - damsel-dummy rescue: trigger, interaction and score/state effect
  - Hermes level markers: what they store for retry

  Check whether each handler is shared ENG3 code already named in S3. Reuse those names; don't re-derive them.
- [x] `L02.RE.05` (P5.ALL.08) Decompile the oncoming-enemy and dummy-enemy updates: approach path, lane choice, collision with the hero, defeat or pass-by, and difficulty reads (S6.01).
- [x] `L02.RE.06` (P5.ALL.11) Recover the completion trigger, the tally hand-off, and the request for L03 (ID 3) that leads to the EXE 0x4040 module switch back to ENG1. Also record what ENG3 reads from game state on entry from L01 (lives, score, power-ups) through the shared S6.05 path.

## Done when

- Every GAUNTLET obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The crossroads branch data and the branch-selection code are documented down to the exact RVAs.
- `L02-types.json` exists.

## Notes
- `L02.RE.01` (2026-10-12, VERIFIED-STATIC, `docs/full-game/eng3-format.md`): descriptor for ID 2:
  - +0xc start records 0x382c8: 8 starts (32/448 section 0; 512/448 sections 0-6); record = u16 x, u16 y, u8 z, u8 section, +8 section bank list;
  - +0x10 level bank list 0x382b8 = [1] SRC3\HERO\HERCULES; section banks ALIEN1, XTRA_DB1, LETTERS;
  - +0x14 0x382c0 → EXE slot 0x06c `AUD_LoadLevelSonix` (byte list, format open, S5);
  - per-level table 0x39860+2*0x20: +0x10 bytes 42 7b 79, +0x14 flags 2, +0x18 0x800, +0x1c 0x51d.
- `L02.RE.06` (2026-10-12, VERIFIED-STATIC):
  - **Completion:** the end-of-level object (`LVL_EndObjectUpdate` ENG3 0x16320) puts the hero in state 10 (carried). At the level's end x, state 9 / `HERO_LevelComplete` 0x19c20 sets gs+3 = 0x31 (`docs/full-game/gameplay/chase.md`).
  - **Hand-off:** TITLE `MOD_FrontEndStateMachine` 0x164b0 sends only 0x31 to the tally (0xd2), then gs[0] += 1 → ID 3 FOREST → `MOD_StartLevelModule` → ENGINE1 (`progression.md`).
  - **Entry state ENG3 reads:** gs+0/+1/+2, lives +0xc (clamped 1..9 in `HERO_InitStates`), health +0x24 (set 0x80), pickup/letter counters +0x14/+0x18/+0x1c (`FUN_10015680`).
  - Runtime confirmation of a real completion is pending a played run.
- `L02.RE.03` (2026-10-12, VERIFIED-STATIC):
  - **Topology** (track table hdr+0x30): 11 tracks in a binary tree. 0 → 1, 2 (branch column 117); 1 → 3, 4; 2 → 5, 6 (117); 3, 4, 5, 6 → 7, 8, 9, 10 (column 127). Tracks 7–10 share blocks 28–31, so the routes rejoin the same final geometry; each has its own spawn list (76 spawns).
  - **Crossroads (from data):** codes 1/2 on track 0, 3/4 on track 1 and 5/6 on track 2, all in columns 118–127 of the last block. Lane rows 0–5 take the odd child, rows 6–11 the even child. Tracks 3–6 have a single code (7–10) in column 127 rows 5–6: the forced continuation.
  - **Choice:** each route has two lane-based choices (end of track 0, end of track 1 or 2). No button is read: the branch is the lane the hero occupies, so the default is whichever side of rows 5/6 the hero is on.
  - **Mechanism:** the hero's cell attribute code each tick (`HERO_Tick` 0x174e0: `LVL_TriggerAt` 0x1ecf0 → `LVL_TriggerCode` 0x22b30) indexes the u16 table at object config 0x3b900 +0x44. Bit 0x8000 calls the function table at config +0x38 with the low byte (all entries `RT_StubVoid`/0 in the three levels). Bit 0x100 calls `LVL_SetObjectTrack` 0x22620(obj, n): obj+0x38 = n, x −= branch column·128; for the camera target it also sets the current track 0x40638 and shifts the camera x. For codes 1–0x20 the table holds 0x100|code, so attribute code n = "go to track n". Cells come from `extracted/full-game/levels/<NAME>/level.json` (zone → attr record → constant code). The structure is in `docs/full-game/eng3-format.md` "Tracks, spawns".
  - Pending: runtime confirmation (a run that takes each side).
- `L02.RE.02` / `RE.04` / `RE.05` (2026-10-12):
  - **Types:** `extracted/meta/full-game/L02-types.json` (`tools/level/eng3_types.py`) holds every placed or runtime kind: handlers (158 names applied, `analysis/annotations/full_game_eng3_kinds.json`), bank, map count and tracks, runtime draws, identification and category. Details: `docs/full-game/eng3-tables.md` "Object kinds per level" and `analysis/exports/full-game/eng3_kind_handlers.json`.
  - **Hazards (code-backed behaviour; names VISUAL/LEAD):** punching bags 0x40 (43); sliding walls 0x2f/0x30 (closing gate); pendulum 0x37; hanging weights 0x3e; toppling stacks 0x2a/0x35; statue 0x29; rotating 0x2e; sweeper 0x48; rolling groups 0x2b (breakable while dashing); shark 0x32; fire ring 0x3c (rim code 0x40; jumping through releases a potion); end post 0x49.
    - Hit codes and damage per difficulty are listed per kind. No handler reads difficulty.
    - Open: weights 0x3e with byte 1 use model 22, but the hit test checks model 21, so they may never hit (runtime check).
  - **Power-ups and scripts (RE.04):** kind 6 bonus item (subtype = spawn byte):
    - helmet: 200 ticks of invulnerability;
    - potion: health +0x20;
    - coins: gs+0x18 / +0x14;
    - winged sandals: dash, hero+0xee = 0x56, 0x3fb20 = 1;
    - urn: secret counter 0x3faec, gs+0x11 bits;
    - amphora: extra life (a potion if lives > 8);
    - mini-Hercules: max health +0x20;
    - letters H-E-R-C-U-L-E-S: gs+0x1c bits.

    The Pegasus checkpoints 0x200b–0x2011 set the restart section gs+2 (one per track 0–6), so they are the "Hermes markers" of the walkthrough. Phil 0x3a gives cues depending on health (EXE slot 0x850, open). Camera markers 0x2019/0x201a.
  - **Enemies and dummies (RE.05):** sword dummies 0x2c (slide across lanes), totem squads 0x2d (4 walkers), homing dummy 0x44 (follows the hero's lane). All break on contact (damage per hit code), with no attack path (ENG3 never reads the attack input). Goats 0x43/0x45/0x47 are decoration that flees.
  - **Random:** LVL_Rand 0x1010 and RT_RandMod_slot 0x1fab0 (for S1.05).
