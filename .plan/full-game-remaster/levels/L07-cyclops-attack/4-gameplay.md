# L07 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of CYCLOPS gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `S3.07` runner model (`docs/full-game/gameplay/chase.md`) and `docs/full-game/levels/L02.md` (both reused, not re-derived); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L07.md`, L07 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L07.GP.01` (P5.ALL.02–03) Layout section of `L07.md`: street sections, lanes and stairs with their switch and return points, collision/surfaces, and camera limits.
- [ ] `L07.GP.02` (P5.ALL.09) Runner use in CYCLOPS: speed range, lane/stairs changes, jump, and anything that differs from `chase.md` or from GAUNTLET (constants per level, in original representation).
- [ ] `L07.GP.03` (P5.ALL.08) For every obstacle and traffic kind (columns, citizens, chariots, as proven), fill in an S4 encounter-style entry: kind, handlers, schedule/trigger, collision, damage or death, and lane rules.
- [ ] `L07.GP.04` (P5.ALL.08) Cyclops encounter:
  - entry and presence through the chase
  - states, telegraphs, any attacks or hazards it causes, and whether it can catch the hero
  - the end condition and the CYEND sequence
  - difficulty differences

  Every row carries an evidence label and RVA/tick references.
- [ ] `L07.GP.05` (P5.ALL.04) Scripts: coins and pickups on the lanes, checkpoints, respawn rules, and the completion trigger → tally → L08 load.
- [ ] `L07.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L07.GP.07` (P5.ALL.13) Gameplay-coverage rows for L07. Each scenario links to its spec section and its evidence.

## Done when

- `L07.md` covers every L07 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
