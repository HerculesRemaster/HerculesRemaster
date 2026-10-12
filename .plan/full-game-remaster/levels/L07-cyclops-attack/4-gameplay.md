# L07 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of CYCLOPS gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `S3.07` runner model (`docs/full-game/gameplay/chase.md`) and `docs/full-game/levels/L02.md` (both reused, not re-derived); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L07.md`, L07 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [x] `L07.GP.01` (P5.ALL.02–03) Layout section of `L07.md`: street sections, lanes and stairs with their switch and return points, collision/surfaces, and camera limits.
- [x] `L07.GP.02` (P5.ALL.09) Runner use in CYCLOPS: speed range, lane/stairs changes, jump, and anything that differs from `chase.md` or from GAUNTLET (constants per level, in original representation).
- [x] `L07.GP.03` (P5.ALL.08) For every obstacle and traffic kind (columns, citizens, chariots, as proven), fill in an S4 encounter-style entry: kind, handlers, schedule/trigger, collision, damage or death, and lane rules.
- [x] `L07.GP.04` (P5.ALL.08) Cyclops encounter:
  - entry and presence through the chase
  - states, telegraphs, any attacks or hazards it causes, and whether it can catch the hero
  - the end condition and the CYEND sequence
  - difficulty differences

  Every row carries an evidence label and RVA/tick references.
- [x] `L07.GP.05` (P5.ALL.04) Scripts: coins and pickups on the lanes, checkpoints, respawn rules, and the completion trigger → tally → L08 load.
- [x] `L07.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [x] `L07.GP.07` (P5.ALL.13) Gameplay-coverage rows for L07. Each scenario links to its spec section and its evidence.

## Done when

- `L07.md` covers every L07 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
- `L07.GP.01` (2026-10-12): `docs/full-game/levels/L07.md` "Layout" (VERIFIED-STATIC data; identifications marked LEAD where only data supports them).
- `L07.GP.02`–`GP.06` (2026-10-12): `docs/full-game/levels/L07.md`.
  - "Runner parameters" (GP.02): only the per-level values; the shared rules stay in `chase.md`.
  - Generated "Objects" tables (`tools/level/eng3_spec.py` from `L07-types.json`): hazards GP.03, enemies/NPCs GP.04, pickups/checkpoints/completion GP.05.
  - Difficulty (GP.06): `extracted/meta/full-game/tuning.json` entries `eng3.L07.kind*.damage` (hit code → columns of 0x380a0 per difficulty, `tools/state/tuning_export.py`). No ENG3 object reads gs+0xe.
  - Runtime cross-checks: the observed health drops match the table at Medium. TORMENT −15 (T0) and −25 (T2, stalactite), run `river-falls/20261012-072443`; CYCLOPS −30 (T3) and −15 (T0), run `death-retry/20261012-073756`.
  - Behaviour is VERIFIED-STATIC; identifications are graded VISUAL/LEAD per row.
- `L07.GP.07` (2026-10-12): `extracted/meta/full-game/gameplay-coverage.json` (`tools/meta/gameplay_coverage.py`) links each L07 scenario to its spec section and evidence runs. Scenarios without runs stay "spec" until phase 2 captures them.
