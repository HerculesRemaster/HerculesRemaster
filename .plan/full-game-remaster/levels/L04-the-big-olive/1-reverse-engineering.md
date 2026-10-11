# L04 Phase 1 — Reverse engineering

Goal: recover the THEBES-specific code and data. That means the per-level config and type table, the ID-5 start-point table, the spawn/update handlers for every THEBES actor, the rescue/destruction scripts, and any code path that could split THEBES into parts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, start table, type-table layout); L03 phase 1 names for shared ENG1 handlers.
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L04-types.json`; notes in this file.

## Tasks

- [ ] `L04.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·5 and list its fields next to TRAINING's and FOREST's. Resolve the type-table pointer (+0x1c).
  - Resolve the start-point table that `HERO_Init` 0x15090 reads (0x5a69c[level] + 0x18·index). Check the hypothesis that this is descriptor field +0xc, which for ID 5 holds 0x10061780.
  - Count the ID-5 start records and decode each one (x, y, z per `docs/gameplay-spec.md`). Mark which records indices 2 and 6 (the `DEMO5_*` starts) select.
  - Identify descriptor fields +0x10 (0x10061738) and +0x14 (0x10061778) from their consumers.
- [ ] `L04.RE.02` (P5.ALL.08) For each of the 87 kinds used in the THEBES spawns, decode the type-table entry `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each one loads. Start with the most frequent kinds (0x2018, 0x2019, 0x25, 0x2013, 0x202b). Write `L04-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L04.RE.03` (P3.ALL.03, P5.ALL.11) **Thebes substage, static side.**
  - Find every writer of the start-index fields (game state +1/+2) and of the level-ID global besides the title/demo path.
  - Find any handler or script that requests a map reload, a level change back to 5, or a mid-level bank/music switch.
  - Find who reads the 8 THEBES blocks' columns as boundaries, if anyone does.
  - Cross-check with S6.02: whether the password decoder produces a start index.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [ ] `L04.RE.04` (P5.ALL.04) Recover the THEBES script triggers: background stairs and depth/route switching, harpy and citizen rescue triggers and their scoring, destruction and falling debris, and secret routes. Check whether these are generic ENG1 handlers already named for TRAINING or FOREST (`L03.RE.05`). Reuse those names; don't re-derive them.
- [ ] `L04.RE.05` (P5.ALL.08) Decompile the enemy updates: thugs, skeletons, harpies and the Minotaur.
  - states, attacks and damage (`HERO_RequestDamage` 0x266a0)
  - whether the Minotaur has a health bar, phases or a scripted entry, which would make it a boss encounter
  - difficulty reads (S6.01)
- [ ] `L04.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to L05 (ID 6) through the shared S6 path.

## Done when

- Every THEBES spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The ID-5 start table is decoded, and every code path that can change the start index or reload THEBES is documented down to the exact RVAs.
- `L04-types.json` exists.

## Notes
