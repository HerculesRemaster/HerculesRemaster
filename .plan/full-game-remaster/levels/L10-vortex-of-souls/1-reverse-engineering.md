# L10 Phase 1 — Reverse engineering

Goal: recover the VORTEX-specific code and data. That means the per-level config and type table, where the level's actors come from (the map has no spawns), how the vortex itself is built and moved through, the final encounter, the rescue objective, and the completion → ending → credits path.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`, plus `TITLE.DLL` for the ending); S2.01 T28 model record; S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L10-types.json`; notes in this file.

Do not assume platformer rules or a boss identity. Name an actor only when a bank load or code proves it.

## Tasks

- [ ] `L10.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·11 and list its fields next to TRAINING's and TITANS's. Resolve the type-table pointer (+0x1c) and the start-point records for ID 11 (0x5a69c + 0x18·start index, as `HERO_Init` 0x15090 uses them). Resolve the descriptor's remaining pointer fields (0x1006b178, 0x1006b158, 0x1006b168) from their consumers.
- [ ] `L10.RE.02` (P5.ALL.01, P5.ALL.08) Actor source. The decoded map has 0 spawns, so find what creates VORTEX's objects:
  - walk the ID-11 type table and find the callers of each spawn handler other than the map spawn loop
  - check the config record and the descriptor fields for event/script lists
  - look for level-ID comparisons with 11 (0xb) in init, tick and hero code
  - find every reader of `P:\SRC1\BOSS\HADES` (0x1005d534), and of `PAIN`/`PANIC` (0x1005db78 / 0x1005db60)

  Write `L10-types.json` as kind → RVAs → bank → creator RVA. Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [ ] `L10.RE.03` (P5.ALL.02, P5.ALL.07) Vortex construction and movement. Using S2.01 (`GFX_DrawModel` ENG1 0x31540), determine whether the vortex is drawn from T28 models, effects, the single 20×20 layer with its 18 tile meshes, or a combination, and what drives the scrolling texture (`n_scroll` 1). Determine the hero's movement mode here: which states/handlers run, input mapping, depth and camera. If it shares code with the L08 flight mode (`L08.RE.03`), cite those RVAs.
- [ ] `L10.RE.04` (P5.ALL.08) Decompile the final-encounter actor update(s): states and transitions, health, attacks, vulnerable windows, damage through `HERO_RequestDamage` 0x266a0, phase changes, defeat condition, and difficulty reads (S6.01). Record phases only as the code shows them; unknown counts stay null.
- [ ] `L10.RE.05` (P5.ALL.04, P5.ALL.08) Souls and the rescue objective: the handlers for soul actors or hazards, the rescue target and its trigger, success/failure states, and whether the rescue gates completion.
- [ ] `L10.RE.06` (P5.ALL.11) Completion → ending. Recover the completion trigger and the module request that follows (TITLE dispatch 0x16300 or another path), then the TITLE code that shows the ending and credits: which TGA screens it loads through `RT_LoadFile` slot 0x68 (candidates `C*`, `SEQ*`, `LEV12`), which `CHOP_TITLE_*` banks and music it uses, and how it returns to title. Also find the Beginner gate that keeps ID 10/11 out of reach (with S6.02/S6.06), and hand the results to S6.05.
- [ ] `L10.RE.07` (P5.ALL.01) Re-run the ENG1 string export so it includes the descriptor-referenced `O:\VORTEX\` (0x1005a7c0) and any `q:\VORTEX\` / `q:\HERC_VOR\` paths that the first scan missed. Find who reads `q:\TORMENT\` (0x100518ac) and `q:\HERC_TOR\` (0x100517f4) in ENG1.

## Done when

- The creator of every VORTEX actor is documented down to exact RVAs (or marked unknown, with the next consumer to check).
- The vortex construction, movement mode, final encounter and ending path are documented with RVAs.
- `L10-types.json` exists.

## Notes
