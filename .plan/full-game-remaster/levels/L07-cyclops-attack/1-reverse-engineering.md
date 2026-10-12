# L07 Phase 1 — Reverse engineering

Goal: recover the CYCLOPS-specific code and data on top of S3 and L02. That means what the ID 8 descriptor and any per-level config point to, the handlers for every CYCLOPS obstacle and traffic kind, the stairs/lane data, the Cyclops actor, the CYEND end sequence, and the completion hand-off to L08. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; L02 owns the first naming of shared obstacle and branch code. This phase does not redo either.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01`–`S3.03`; `S3.04` CYCLOPS `level.json`; `L02.RE.02`–`RE.03` names.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L07-types.json`; notes in this file.

## Tasks

- [x] `L07.RE.01` (P5.ALL.01) Read descriptor 0x385e8 field by field using the S3.02 consumer map: +0 path (0x10038654), +4 0x125184, +8 zero, and the pointers +0xc 0x100383a0, +0x10 0x10038388, +0x14 0x10038398. Dump what each pointer targets for ID 8 and diff it against the ID 2 results from `L02.RE.01`. If S3.02 finds a per-level config table, record the ID 8 record.
- [x] `L07.RE.02` (P5.ALL.08) For each obstacle/actor kind in the CYCLOPS tables, decode its handler entry and list its banks and sounds. Reuse the L02 names where the same handler runs. Write `L07-types.json` as kind → RVAs → bank → count in map. Name columns, citizens and chariots only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L07.RE.03` (P5.ALL.09) Stairs and alternate lanes.
  - Find how `i1.a` stores the CYCLOPS lanes and stairs, and whether they use the same branch records as the GAUNTLET crossroads (`L02.RE.03`).
  - Find the input and position rules for taking stairs and for returning to the street.
  - Record the lane count and positions from data.
- [x] `L07.RE.04` (P5.ALL.08) Decompile the Cyclops actor:
  - spawn, and how its position follows the hero or the track
  - states and the attacks or hazards it produces (if any), with telegraphs
  - whether it can catch or damage the hero, and the rule for that
  - difficulty reads (S6.01)
- [x] `L07.RE.05` (P5.ALL.04) Recover the cross-traffic schedule (columns, citizens, chariots): fixed map data, timer, or position trigger, and any random source (feed it to `S1.05`). Recover the end trigger and what CYEND does: a separate scene, a bank swap or a scripted sequence.
- [x] `L07.RE.06` (P5.ALL.11) Recover the completion → tally hand-off and the request for L08 (ID 9) that leads to the EXE 0x4040 switch back to ENG1. Record what ENG3 reads from game state on entry from L06 through the shared S6.05 path.

## Done when

- Every CYCLOPS obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The lane/stairs data, the traffic schedule, the Cyclops actor and the end sequence are documented down to the exact RVAs.
- `L07-types.json` exists.

## Notes
- `L07.RE.01` (2026-10-12, VERIFIED-STATIC, `docs/full-game/eng3-format.md`): descriptor for ID 8:
  - +0xc start records 0x383a0: 6 starts (0/448 sections 0-2; 1920/448); record = u16 x, u16 y, u8 z, u8 section, +8 section bank list;
  - +0x10 level bank list 0x38388 = [1] SRC3\HERO\HERCULES; section banks PEOPLE, CYCLOPS, CYEND, XTRA_DB1, LETTERS;
  - +0x14 0x38398 → EXE slot 0x06c `AUD_LoadLevelSonix` (byte list, format open, S5);
  - per-level table 0x39860+8*0x20: +0x14 flags 2, +0x18 0x800, +0x1c 0x51d.
- `L07.RE.06` (2026-10-12, VERIFIED-STATIC):
  - **Completion:** the end-of-level object (`LVL_EndObjectUpdate` ENG3 0x16320) puts the hero in state 10 (carried). At the level's end x, state 9 / `HERO_LevelComplete` 0x19c20 sets gs+3 = 0x31 (`docs/full-game/gameplay/chase.md`).
  - **Hand-off:** TITLE `MOD_FrontEndStateMachine` 0x164b0 sends only 0x31 to the tally (0xd2), then gs[0] += 1 → ID 9 TITANS → `MOD_StartLevelModule` → ENGINE1 (`progression.md`).
  - **Entry state ENG3 reads:** gs+0/+1/+2, lives +0xc (clamped 1..9 in `HERO_InitStates`), health +0x24 (set 0x80), pickup/letter counters +0x14/+0x18/+0x1c (`FUN_10015680`).
  - Runtime confirmation of a real completion is pending a played run.
- `L07.RE.03` (2026-10-12, partial, VERIFIED-STATIC):
  - **Topology:** 4 tracks in a chain 0 → 1 → 2 → 3, branch columns 255/254/254. The forced switches are code 1 (track 0, column 255, rows 4–7) and code 2 (track 1, column 254, rows 3–6). Track 3 has no spawns.
  - There are no lane-choice crossroads. The "alternative block" per section in `HERO_Tick` (0→(0,1), 1→(3,4), 2→(6,7), `chase.md`) is the open lead for stairs/alternate lanes: its use (spline/lane clamp at 0x41c68) still has to be tied to the stairs geometry.
  - **Mechanism:** the hero's cell attribute code each tick (`HERO_Tick` 0x174e0: `LVL_TriggerAt` 0x1ecf0 → `LVL_TriggerCode` 0x22b30) indexes the u16 table at object config 0x3b900 +0x44. Bit 0x8000 calls the function table at config +0x38 with the low byte (all entries `RT_StubVoid`/0 in the three levels). Bit 0x100 calls `LVL_SetObjectTrack` 0x22620(obj, n): obj+0x38 = n, x −= branch column·128; for the camera target it also sets the current track 0x40638 and shifts the camera x. For codes 1–0x20 the table holds 0x100|code, so attribute code n = "go to track n". Cells come from `extracted/full-game/levels/<NAME>/level.json` (zone → attr record → constant code). The structure is in `docs/full-game/eng3-format.md` "Tracks, spawns".
- `L07.RE.03` (2026-10-12, update): the CYCLOPS splines (n14 = 9) come in triples per section, and the `HERO_Tick` (primary, alternative) pairs are (0, 1), (3, 4), (6, 7).
  - The alternatives 1/4/7 are narrow (half-width 0–248), shifted in z (454–958) and use camera term 120. The clamp to them applies only while y is in 320–448 (flag 0x3fb2c).
  - This is the stairs/raised-lane candidate (`docs/full-game/levels/L07.md`). Data is VERIFIED-STATIC; the "stairs" meaning is a LEAD until L07.TR.04.
  - Splines 2/5/8 are read by 0x5760/0x5b40 (open; possibly the Cyclops path, L07.RE.04).
- `L07.RE.02` / `RE.04` / `RE.05` (2026-10-12; `extracted/meta/full-game/L07-types.json`, `docs/full-game/levels/L07.md` "Cyclops (L07)", `analysis/exports/full-game/eng3_cyclops.json`, `eng3_kind_handlers.json`; 15 + 158 names applied):
  - **Kinds:** boulders 0x2a (55, code 0x200); columns 0x2b/0x2e; chariots 0x2d (crossing) and 0x2f (oncoming, code 0x400); citizens 0x35/0x3f/0x42 (contact code 4); sinking street 0x39; swooper 0x3d; rock-dropping flyer 0x3e; dash-breakable crate 0x48; cloth 0x47; Pegasus checkpoints 0x200b–0x200d.
  - **Cyclops (RE.04):** kind 0x29 is created by code at every section start or restart (`LVL_StartSection` → 0x1870 → 0x3290), not from the map. It is kept 1664 units ahead of the hero in the background and has no hitbox, so it never hits the hero.
    - Its states are driven by trigger codes in the map: 0x57 stomp, 0x5b throw, 0x53 idle. The stomp shakes the camera for 80 ticks and arms the columns. The throw throws a rock (kind 0x30, hit code 2 → 25/30/40).
    - No difficulty read and no random in its controller.
  - **Traffic (RE.05):** all traffic is fixed map spawns released by `LVL_SpawnStep` as columns are drawn. Each object starts on hero distance; columns start only after a stomp. No timer, no random schedule.
  - **CYEND:** a second Cyclops bank (recovery/death pose, club smash), not an end sequence. Completion is the shared end object at x 30592 → `HERO_LevelComplete` → gs+3 = 0x31.
  - Splines 2/5/8 are the citizens' running paths (0x5760/0x5b40).
