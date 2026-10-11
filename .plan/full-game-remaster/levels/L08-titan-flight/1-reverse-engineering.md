# L08 Phase 1 — Reverse engineering

Goal: recover the TITANS-specific code and data. That means the per-level config and type table, the flight mode (hero/Pegasus states, input mapping, depth and camera), the handlers for every Titan, god and hazard actor, and the completion path including the Beginner branch.
Needs: P2 Ghidra project (ghidra-mcp GUI, `program=ENG1.DLL`); S2.05 config export; `docs/gameplay-spec.md` (hero, damage, type-table layout).
Outputs: Ghidra renames/structs (`LVL_`, `HERO_` prefixes, `save_program`); `extracted/meta/full-game/L08-types.json`; notes in this file.

Do not import side-scroller assumptions. Every flight rule must come from a decompiled handler; where a path is shared with the ENG1 hero spec, cite the shared RVA instead of re-deriving it.

## Tasks

- [ ] `L08.RE.01` (P5.ALL.01) Read config record 0x68af0 + 0x48·9 and list its fields next to TRAINING's. Resolve the type-table pointer (+0x1c) and the start-point records for ID 9 (0x5a69c + 0x18·start index, as `HERO_Init` 0x15090 uses them). Resolve the descriptor's remaining pointer fields (0x10063108, 0x100630e4, 0x100630f8) from their consumers.
- [ ] `L08.RE.02` (P5.ALL.08) For each kind used in the TITANS spawns, decode the type-table entry `[spawn, stub, update]` (12 B), name the handlers and list the banks and sounds each one loads. Write `L08-types.json` as kind → RVAs → bank → count in map. Put actor names only where a bank load or code proves them; otherwise mark them LEAD.
- [ ] `L08.RE.03` (P5.ALL.10) Flight mode.
  - Find every reader of the `ALIEN2\PEGASUS` path (0x1005d830) and what it attaches to the hero.
  - Determine whether ID 9 installs different hero handlers in table 0x72040, switches the state id (0x72108) into a flight-only range, or drives the hero from a Pegasus actor. Check whether the per-tick handler 0x15780 and state 27 (cheat fly) share code with it.
  - Decode the input mapping from the polled pad bits (`RT_PollJoystick` EXE 0x7c50, keyboard 0x7f30) to flight motion: horizontal, vertical and any depth axis; speeds and limits; which attack buttons remain and what they spawn.
  - Recover the camera/scroll rule: player-driven or forced, how the 546 camera records and `LVL_StreamBlocks` 0x1f870 advance through the 4 blocks, and any screen bounds.
  - Record the call chain as program+RVA. Do not conclude anything about runtime use from static reachability alone.
- [ ] `L08.RE.04` (P5.ALL.08) Titan hazards. Find the readers of the `IceT`, `LavaT`, `RockT` and `TornadoT` paths and the caller of the `GoodbyeRockT` debug string (0x100655d0). Decompile each update: states, attacks/projectiles, damage through `HERO_RequestDamage` 0x266a0, defeat or avoid conditions, exit, and difficulty reads (S6.01). Bank names are only a starting point.
- [ ] `L08.RE.05` (P5.ALL.04, P5.ALL.08) God rescues. Find the readers of the `Zeus`, `Ares`, `Apollo`, `Athena`, `Aphrod` and `Bacchus` paths; recover the rescue trigger, interaction, scoring and persistence. Find the readers of `Water`, `Griffon` and `Grif` to settle whether those banks belong to TITANS at all.
- [ ] `L08.RE.06` (P5.ALL.11) Recover the completion trigger and the hand-off to tally and to the next level through the shared S6 path: the request for ID 10 (ENG3) through TITLE dispatch 0x16300 on Medium/Herculean, and the difficulty branch that ends the game on Beginner. Record which code requests the end screen (for example the `ENDEASY` TGA through TITLE `RT_LoadFile` slot 0x68), and hand the result to S6.05/S6.06.

## Done when

- Every TITANS spawn kind maps to named handlers (or is marked unknown, with the next consumer to check).
- The flight mode is documented down to exact RVAs: state/handler selection, input mapping, motion, depth and camera.
- The completion and Beginner-branch code paths are documented with RVAs.
- `L08-types.json` exists.

## Notes
