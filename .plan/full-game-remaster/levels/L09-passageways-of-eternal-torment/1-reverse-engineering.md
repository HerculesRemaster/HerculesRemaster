# L09 Phase 1 — Reverse engineering

Goal: recover the TORMENT-specific code and data on top of S3, L02 and L07. That means what the ID 10 descriptor and any per-level config point to, the passage section and lane data, the skeleton and lost-soul handlers, the chariot, stalactite and River Styx fall rules, the completion hand-off to L10, and the static side of the Beginner restriction. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; L02/L07 own the first naming of shared obstacle and lane code. This phase does not redo them.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01`–`S3.03`; `S3.04` TORMENT `level.json`; `L02.RE.02`–`RE.03` and `L07.RE.02`–`RE.03` names.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L09-types.json`; notes in this file.

## Tasks

- [x] `L09.RE.01` (P5.ALL.01) Read descriptor 0x38618 field by field using the S3.02 consumer map: +0 path (0x10038648), +4 0x10903c, +8 zero, and the pointers +0xc 0x10038450, +0x10 0x10038430, +0x14 0x10038440. Dump what each pointer targets for ID 10 and diff it against IDs 2 and 8. If S3.02 finds a per-level config table, record the ID 10 record.
- [x] `L09.RE.02` (P5.ALL.08) For each obstacle/actor kind in the TORMENT tables, decode its handler entry and list its banks and sounds. Reuse the L02/L07 names where the same handler runs. Write `L09-types.json` as kind → RVAs → bank → count in map. Name skeletons, souls, chariot and stalactites only where a bank or code proves them; otherwise mark them LEAD.
- [x] `L09.RE.03` (P5.ALL.09) Passage sections and depth routes.
  - Find how `i1.a` stores the TORMENT sections, lanes/depth routes and section boundaries, and whether they reuse the L02/L07 branch and lane records.
  - Check whether any section boundary loads new resources, given that no ENG3 stream-chunk routine is known.
- [x] `L09.RE.04` (P5.ALL.08) Decompile the skeleton and lost-soul updates: spawn, movement, collision with the hero, whether they can be attacked or only avoided, defeat or pass-by, and difficulty reads (S6.01).
- [x] `L09.RE.05` (P5.ALL.04) Hazard and fall scripts:
  - the chariot: path, trigger and collision
  - falling stalactites: activation trigger and timing relative to the chariot
  - River Styx falls: the fall condition (edge, gap or hazard), the death sequence and the respawn rule

  Note any random source and feed it to `S1.05`.
- [x] `L09.RE.06` (P5.ALL.11) Recover the completion trigger, the tally hand-off, and the request for L10 (ID 11) that leads to the EXE 0x4040 switch back to ENG1. Record what ENG3 reads from game state on entry from L08. Find any difficulty check on ID 10 in ENG3 and hand it to S6.06; the TITLE/EXE side of the Beginner restriction stays in S6.02/S6.06.

## Done when

- Every TORMENT obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The section data, the hazard and fall rules, and the completion hand-off are documented down to the exact RVAs.
- `L09-types.json` exists.

## Notes
- `L09.RE.01` (2026-10-12, VERIFIED-STATIC, `docs/full-game/eng3-format.md`): descriptor for ID 10:
  - +0xc start records 0x38450: 6 starts (0/0 section 0; 1024/0 sections 0-4); record = u16 x, u16 y, u8 z, u8 section, +8 section bank list;
  - +0x10 level bank list 0x38430 = [1] SRC3\HERO\HERCULES; section banks TORMENT, XTRA_DB1, LETTERS, SOULS;
  - +0x14 0x38440 → EXE slot 0x06c `AUD_LoadLevelSonix` (byte list, format open, S5);
  - per-level table 0x39860+10*0x20: +0x14 flags 2, +0x18 0x800, +0x1c 0x61e.
- `L09.RE.06` (2026-10-12, VERIFIED-STATIC):
  - **Completion:** the end-of-level object (`LVL_EndObjectUpdate` ENG3 0x16320) puts the hero in state 10 (carried). At the level's end x, state 9 / `HERO_LevelComplete` 0x19c20 sets gs+3 = 0x31 (`docs/full-game/gameplay/chase.md`).
  - **Hand-off:** TITLE `MOD_FrontEndStateMachine` 0x164b0 sends only 0x31 to the tally (0xd2), then gs[0] += 1 → ID 11 VORTEX → `MOD_StartLevelModule` → ENGINE1 (`progression.md`).
  - **Entry state ENG3 reads:** gs+0/+1/+2, lives +0xc (clamped 1..9 in `HERO_InitStates`), health +0x24 (set 0x80), pickup/letter counters +0x14/+0x18/+0x1c (`FUN_10015680`). Difficulty: the only ENG3 reader of gs+0xe is `HERO_ApplyDamage` 0x19410 (`analysis/exports/full-game/eng3_gs_fields.json`), so ENG3 has no level-10 difficulty check; the Beginner restriction is entirely TITLE/EXE (handed to S6.06).
  - Runtime confirmation of a real completion is pending a played run.
- `L09.RE.03` (2026-10-12, VERIFIED-STATIC):
  - **Topology:** 5 tracks. 0 → 1, 2 at column 229; 1 → 3 and 2 → 4 at column 139.
  - **Depth route choice:** codes 1/2 in track 0, columns 230–239. Rows 2–5 → track 1, rows 6–9 → track 2. Tracks 1 and 2 switch on (codes 3/4, columns 137–149) to 3 and 4.
  - Track 0 uses blocks 0 (height 0) and 5 (height 4); the gaps of block 0 are covered by block 5 (`preview.png`).
  - **Resources:** no section boundary loads anything. The whole map is read at once (descriptor +4 = full size), and every start record has the same bank list [4, 2, 8, 9] (`animation-coverage.json` `eng3_levels`).
  - **Mechanism:** the hero's cell attribute code each tick (`HERO_Tick` 0x174e0: `LVL_TriggerAt` 0x1ecf0 → `LVL_TriggerCode` 0x22b30) indexes the u16 table at object config 0x3b900 +0x44. Bit 0x8000 calls the function table at config +0x38 with the low byte (all entries `RT_StubVoid`/0 in the three levels). Bit 0x100 calls `LVL_SetObjectTrack` 0x22620(obj, n): obj+0x38 = n, x −= branch column·128; for the camera target it also sets the current track 0x40638 and shifts the camera x. For codes 1–0x20 the table holds 0x100|code, so attribute code n = "go to track n". Cells come from `extracted/full-game/levels/<NAME>/level.json` (zone → attr record → constant code). The structure is in `docs/full-game/eng3-format.md` "Tracks, spawns".
- `L09.RE.02` / `RE.04` / `RE.05` (2026-10-12):
  - **Types:** `extracted/meta/full-game/L09-types.json` (`tools/level/eng3_types.py`) maps every placed or runtime kind to its handlers, bank, map count and identification. 158 handler names were applied (`analysis/annotations/full_game_eng3_kinds.json`). Details: `docs/full-game/eng3-tables.md` "Object kinds per level" and `analysis/exports/full-game/eng3_kind_handlers.json`.
  - **Kinds:** skeletons 0x31 (walking) and 0x33 (dropping); lost souls 0x2f (no damage); spirits 0x2e (no damage); stalactites 0x30 (120 placed, hit code 0x31 → T2); flying chariot 0x29 (drops fireballs); fire beast 0x3d; bone wall 0x35; bone pile 0x40; rock fall 0x46; raft 0x34; Pegasus checkpoints 0x200b–0x200f.
  - **No defeat by attack:** skeletons cannot be beaten (ENG3 never reads attack input), only avoided. No handler reads difficulty; difficulty acts only through the damage table 0x380a0.
  - **River Styx falls:** attribute codes 0x63/0x77 (`LVL_CellAttribute`). Once the hero falls below y 170 → `HERO_Die` with 24 debris objects. TORMENT columns: track 0 0x63 at 29–38 and 141–150, 0x77 at 99–109, 162–182 and 224–229; the other tracks are listed in the run note. Confirmed at runtime (L09.TR.06).
  - **Random:** LVL_Rand 0x1010 (stalactite drips) and RT_RandMod_slot 0x1fab0 (debris). Fed to S1.05 via `eng3-tables.md`.
  - **Identifications:** VISUAL where the sprite frames show the object; LEAD for level models (stalactites and rocks are models).
