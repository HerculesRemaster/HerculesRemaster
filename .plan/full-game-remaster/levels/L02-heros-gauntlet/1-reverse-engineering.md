# L02 Phase 1 — Reverse engineering

Goal: recover the GAUNTLET-specific code and data on top of S3. That means what the ID 2 descriptor and any per-level config point to, the handlers for every GAUNTLET obstacle and actor kind, the crossroads branch data, the pickup/rescue/power-up scripts, and the completion hand-off to L03. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; this phase does not redo them.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01` (named skeleton), `S3.02` (descriptor use), `S3.03` (`i1.a` structs); `S3.04` GAUNTLET `level.json` for the kinds actually present.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L02-types.json`; notes in this file.

## Tasks

- [ ] `L02.RE.01` (P5.ALL.01) Read descriptor 0x38558 field by field using the S3.02 consumer map: +0 path (0x10038660), +4 0x129e28, +8 zero, and the pointers +0xc 0x100382c8, +0x10 0x100382b8, +0x14 0x100382c0 (the last is passed to `FUN_1001fa40` in `MOD_Tick` init). Dump what each pointer targets for ID 2 and lay it next to IDs 8 and 10. If S3.02 finds a per-level config table, record the ID 2 record and its type-table pointer.
- [ ] `L02.RE.02` (P5.ALL.08) For each obstacle/actor kind in the GAUNTLET spawn or obstacle tables (from `S3.03`/`S3.04`), decode its handler entry, name the spawn/update/collision handlers and list the banks and sounds each one loads. Write `L02-types.json` as kind → RVAs → bank → count in map. Put hazard names (blade, hammer, stone hand, boulder, rolling stone, pillar, pond platform) only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L02.RE.03` (P5.ALL.09) Crossroads branch data.
  - Find how `i1.a` stores the GAUNTLET track topology: branch records, lane/track tables, and where branches rejoin.
  - Find the code that reads the player's input at a crossroad and selects a branch, and any default when no input is given.
  - Record the branch count and positions from data, not from the walkthrough's "two crossroads".
- [ ] `L02.RE.04` (P5.ALL.04) GAUNTLET scripts and power-ups:
  - pickups: coins, HERCULES letters, Herculade bottles, action figures, password vases (names from the walkthrough are LEAD)
  - Hermes Sandals: the speed change and the boulder-smash rule
  - Helmet of Invincibility: activation and its effect on collision/damage
  - damsel-dummy rescue: trigger, interaction and score/state effect
  - Hermes level markers: what they store for retry

  Check whether each handler is shared ENG3 code already named in S3. Reuse those names; don't re-derive them.
- [ ] `L02.RE.05` (P5.ALL.08) Decompile the oncoming-enemy and dummy-enemy updates: approach path, lane choice, collision with the hero, defeat or pass-by, and difficulty reads (S6.01).
- [ ] `L02.RE.06` (P5.ALL.11) Recover the completion trigger, the tally hand-off, and the request for L03 (ID 3) that leads to the EXE 0x4040 module switch back to ENG1. Also record what ENG3 reads from game state on entry from L01 (lives, score, power-ups) through the shared S6.05 path.

## Done when

- Every GAUNTLET obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The crossroads branch data and the branch-selection code are documented down to the exact RVAs.
- `L02-types.json` exists.

## Notes
