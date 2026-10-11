# L05 Phase 1 — Reverse engineering

Goal: recover the HYDRA-specific code and data. That means the per-level config and type table, the handlers behind the two kind-0x31 spawns, the Hydra state machine, how the Hydra is drawn (models and/or sprites), and the arena scripts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.01 (T28 model record, `GFX_DrawModel` 0x31540); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_`, `GFX_` prefixes, `save_program`); `extracted/meta/full-game/L05-types.json`; notes in this file.

## Tasks

- [ ] `L05.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·6 and list its fields next to TRAINING's and THEBES's. Resolve the type-table pointer (+0x1c) and the ID-6 start-point table (check the hypothesis that descriptor field +0xc, 0x10058878, is what `HERO_Init` 0x15090 reads). Identify descriptor fields +0x10 and +0x14 from their consumers.
- [ ] `L05.RE.02` (P5.ALL.08) Decode the type-table entry for kind 0x31 `[spawn, stub 0xc720, update]` (12 B). Name the handlers, and find what templates 11 and 12 select. List every bank, model and sound the handlers load. Write `L05-types.json`. Put actor names only where a bank or code proves them.
- [ ] `L05.RE.03` (P5.ALL.08) Decompile the Hydra update(s):
  - find who loads `P:\SRC1\BOSS\HYDRA` (string 0x1005d548), and from which handler
  - states and transitions, attacks and their hit boxes (`HERO_RequestDamage` 0x266a0)
  - the per-head or per-part structure, if any: count, cut condition, regrowth (LEAD), and whether parts are separate actors
  - health, vulnerable windows, phase changes (only those the code supports), defeat
  - difficulty reads (S6.01)
- [ ] `L05.RE.04` (P5.ALL.02, with S2.01) Determine how the Hydra is drawn: T28 model calls from the Hydra handlers into `GFX_DrawModel` 0x31540, sprite frame requests for bank 0804, or both. Record which T28 records HYDRA's map uses and which handler animates them. Record how the arc camera records (type 1) are consumed in the arena.
- [ ] `L05.RE.05` (P5.ALL.04) Recover the arena scripts: boundaries, camera, any crowd/NPC or hazards, and the trigger that starts the fight. Reuse generic ENG1 handler names from TRAINING/FOREST/THEBES.
- [ ] `L05.RE.06` (P5.ALL.11) Recover the defeat → completion trigger and the hand-off to tally and to L06 (ID 7) through the shared S6 path.

## Done when

- Kind 0x31 maps to named handlers, and the Hydra's code path is documented down to the exact RVAs.
- The draw path (models/sprites) is known.
- `L05-types.json` exists.

## Notes
