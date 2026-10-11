# L07 Phase 1 — Reverse engineering

Goal: recover the CYCLOPS-specific code and data on top of S3 and L02. That means what the ID 8 descriptor and any per-level config point to, the handlers for every CYCLOPS obstacle and traffic kind, the stairs/lane data, the Cyclops actor, the CYEND end sequence, and the completion hand-off to L08. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; L02 owns the first naming of shared obstacle and branch code. This phase does not redo either.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01`–`S3.03`; `S3.04` CYCLOPS `level.json`; `L02.RE.02`–`RE.03` names.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L07-types.json`; notes in this file.

## Tasks

- [ ] `L07.RE.01` (P5.ALL.01) Read descriptor 0x385e8 field by field using the S3.02 consumer map: +0 path (0x10038654), +4 0x125184, +8 zero, and the pointers +0xc 0x100383a0, +0x10 0x10038388, +0x14 0x10038398. Dump what each pointer targets for ID 8 and diff it against the ID 2 results from `L02.RE.01`. If S3.02 finds a per-level config table, record the ID 8 record.
- [ ] `L07.RE.02` (P5.ALL.08) For each obstacle/actor kind in the CYCLOPS tables, decode its handler entry and list its banks and sounds. Reuse the L02 names where the same handler runs. Write `L07-types.json` as kind → RVAs → bank → count in map. Name columns, citizens and chariots only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L07.RE.03` (P5.ALL.09) Stairs and alternate lanes.
  - Find how `i1.a` stores the CYCLOPS lanes and stairs, and whether they use the same branch records as the GAUNTLET crossroads (`L02.RE.03`).
  - Find the input and position rules for taking stairs and for returning to the street.
  - Record the lane count and positions from data.
- [ ] `L07.RE.04` (P5.ALL.08) Decompile the Cyclops actor:
  - spawn, and how its position follows the hero or the track
  - states and the attacks or hazards it produces (if any), with telegraphs
  - whether it can catch or damage the hero, and the rule for that
  - difficulty reads (S6.01)
- [ ] `L07.RE.05` (P5.ALL.04) Recover the cross-traffic schedule (columns, citizens, chariots): fixed map data, timer, or position trigger, and any random source (feed it to `S1.05`). Recover the end trigger and what CYEND does: a separate scene, a bank swap or a scripted sequence.
- [ ] `L07.RE.06` (P5.ALL.11) Recover the completion → tally hand-off and the request for L08 (ID 9) that leads to the EXE 0x4040 switch back to ENG1. Record what ENG3 reads from game state on entry from L06 through the shared S6.05 path.

## Done when

- Every CYCLOPS obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The lane/stairs data, the traffic schedule, the Cyclops actor and the end sequence are documented down to the exact RVAs.
- `L07-types.json` exists.

## Notes
