# L03 Phase 1 — Reverse engineering

Goal: recover the FOREST-specific code and data. That means the per-level config and type table, the spawn/update handlers for every FOREST actor, and the code path that handles Nessus and ID 4.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L03-types.json`; notes in this file.

## Tasks

- [ ] `L03.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·3 and list its fields next to TRAINING's. Resolve the type-table pointer (+0x1c) and the start-point table (0x5a69c-style records for ID 3, as `HERO_Init` 0x15090 uses them).
- [ ] `L03.RE.02` (P5.ALL.08) For each kind used in the FOREST spawns, decode the type-table entry `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each one loads. Write `L03-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank or code proves them; otherwise mark them LEAD.
- [ ] `L03.RE.03` (P3.ALL.03, P5.ALL.01) Nessus and ID 4.
  - Find every reader of descriptor 0x5a6f0, TITLE 0x234e0, and the `O:\NESSUS\` / `q:\NESSUS\` / `q:\HERC_NES\` strings.
  - Find who loads the `NESSUS\*` bank paths.
  - Determine whether FOREST's script or a type handler requests a level change to 4, or a mid-level bank/sound switch.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [ ] `L03.RE.04` (P5.ALL.08) Decompile the Nessus actor update(s):
  - states, and the transitions punch → kick → look → ride → reject/finish/defeat (bank names are only a starting point)
  - health, and the damage rules, including "weapons don't hurt" (LEAD)
  - ride/mount handling with the hero
  - the Power Punch finish condition
  - difficulty reads (S6.01)
- [ ] `L03.RE.05` (P5.ALL.04) Recover the FOREST script triggers: background-path switching (layer/depth change), climbable trees, breakable stone barriers and cracks, and secret routes. Check whether these are generic ENG1 handlers already named for TRAINING. Reuse those names; don't re-derive them.
- [ ] `L03.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to L04 (ID 5) through the shared S6 path.

## Done when

- Every FOREST spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The Nessus code path and the use of the ID-4 descriptor are documented down to the exact RVAs.
- `L03-types.json` exists.

## Notes
