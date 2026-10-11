# L09 Phase 1 — Reverse engineering

Goal: recover the TORMENT-specific code and data on top of S3, L02 and L07. That means what the ID 10 descriptor and any per-level config point to, the passage section and lane data, the skeleton and lost-soul handlers, the chariot, stalactite and River Styx fall rules, the completion hand-off to L10, and the static side of the Beginner restriction. S3 owns the ENG3 skeleton, the `i1.a` layout and the runner model; L02/L07 own the first naming of shared obstacle and lane code. This phase does not redo them.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG3.DLL`); `S3.01`–`S3.03`; `S3.04` TORMENT `level.json`; `L02.RE.02`–`RE.03` and `L07.RE.02`–`RE.03` names.
Outputs: Ghidra renames/structs (the S3 prefixes; `save_program`); `extracted/meta/full-game/L09-types.json`; notes in this file.

## Tasks

- [ ] `L09.RE.01` (P5.ALL.01) Read descriptor 0x38618 field by field using the S3.02 consumer map: +0 path (0x10038648), +4 0x10903c, +8 zero, and the pointers +0xc 0x10038450, +0x10 0x10038430, +0x14 0x10038440. Dump what each pointer targets for ID 10 and diff it against IDs 2 and 8. If S3.02 finds a per-level config table, record the ID 10 record.
- [ ] `L09.RE.02` (P5.ALL.08) For each obstacle/actor kind in the TORMENT tables, decode its handler entry and list its banks and sounds. Reuse the L02/L07 names where the same handler runs. Write `L09-types.json` as kind → RVAs → bank → count in map. Name skeletons, souls, chariot and stalactites only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L09.RE.03` (P5.ALL.09) Passage sections and depth routes.
  - Find how `i1.a` stores the TORMENT sections, lanes/depth routes and section boundaries, and whether they reuse the L02/L07 branch and lane records.
  - Check whether any section boundary loads new resources, given that no ENG3 stream-chunk routine is known.
- [ ] `L09.RE.04` (P5.ALL.08) Decompile the skeleton and lost-soul updates: spawn, movement, collision with the hero, whether they can be attacked or only avoided, defeat or pass-by, and difficulty reads (S6.01).
- [ ] `L09.RE.05` (P5.ALL.04) Hazard and fall scripts:
  - the chariot: path, trigger and collision
  - falling stalactites: activation trigger and timing relative to the chariot
  - River Styx falls: the fall condition (edge, gap or hazard), the death sequence and the respawn rule

  Note any random source and feed it to `S1.05`.
- [ ] `L09.RE.06` (P5.ALL.11) Recover the completion trigger, the tally hand-off, and the request for L10 (ID 11) that leads to the EXE 0x4040 switch back to ENG1. Record what ENG3 reads from game state on entry from L08. Find any difficulty check on ID 10 in ENG3 and hand it to S6.06; the TITLE/EXE side of the Beginner restriction stays in S6.02/S6.06.

## Done when

- Every TORMENT obstacle/actor kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The section data, the hazard and fall rules, and the completion hand-off are documented down to the exact RVAs.
- `L09-types.json` exists.

## Notes
