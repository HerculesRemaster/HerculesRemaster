# L06 Phase 1 — Reverse engineering

Goal: recover the MEDUSA-specific code and data. That means the per-level config and type table, the handlers behind the six kind-0x0 spawns, the skeleton enemies, the Medusa state machine with its gaze/projectile attacks and defeat mechanic, and the lair scripts.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.01 (T28 model record, `GFX_DrawModel` 0x31540); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_`, `GFX_` prefixes, `save_program`); `extracted/meta/full-game/L06-types.json`; notes in this file.

## Tasks

- [ ] `L06.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·7 and list its fields next to TRAINING's and HYDRA's. Resolve the type-table pointer (+0x1c) and the ID-7 start-point table (check the hypothesis that descriptor field +0xc, 0x1005a030, is what `HERO_Init` 0x15090 reads). Identify descriptor fields +0x10 and +0x14 from their consumers.
- [ ] `L06.RE.02` (P5.ALL.08) Check how the ENG1 spawn reader interprets the six MEDUSA spawns (all kind 0x0, flags 0), and compare with `tools/level/level_decode.py`. Then decode the type-table entry for the actual kind(s) `[spawn, stub 0xc720, update]` (12 B), name the handlers and list the banks and sounds each loads. Write `L06-types.json`. Put actor names only where a bank or code proves them.
- [ ] `L06.RE.03` (P5.ALL.08) Decompile the Medusa update(s):
  - find who loads `P:\SRC1\BOSS\MEDUSA` (string 0x1005d574), and from which handler
  - states and transitions; gaze and projectile/venom attacks (LEAD): telegraph, hit test, and the effect on the hero (state change or `HERO_RequestDamage` 0x266a0)
  - the defeat mechanic: whether a reflection is involved, which object or hero action triggers it, and how it damages Medusa
  - health, vulnerable windows, phase changes (only those the code supports), defeat
  - difficulty reads (S6.01)
- [ ] `L06.RE.04` (P5.ALL.08) Skeleton enemies: find who loads `P:\SRC1\ALIEN1\SKELETON` (0x1005d938) and `P:\SRC1\BOSS\SKELWAR` (0x1005d55c), and which level's type table references those handlers. This settles whether SKELWAR belongs to L06. Decompile their states, attacks and defeat (including any revive behavior, if the code has one).
- [ ] `L06.RE.05` (P5.ALL.02, P5.ALL.04, with S2.01) Lair scripts and drawing: T28 model use in the lair and by Medusa, the one animated texture, the arc camera records, depth routes, props/pickups (including any mirror/shield prop the reflection mechanic needs), and the trigger that starts the Medusa fight.
- [ ] `L06.RE.06` (P5.ALL.11) Recover the defeat → completion trigger and the hand-off to tally and to L07 (ID 8, ENG3) through the shared S6 path, including the module change request (TITLE 0x16300 → EXE 0x4040).

## Done when

- Every MEDUSA spawn maps to named handlers (or is marked unknown, with the next consumer to check).
- The Medusa and skeleton code paths are documented down to the exact RVAs, including the defeat mechanic.
- `L06-types.json` exists.

## Notes
