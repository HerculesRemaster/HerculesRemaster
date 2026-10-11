# L06 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of MEDUSA gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` (the ENG1 hero, reused); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L06.md`, L06 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L06.GP.01` (P5.ALL.02–03) Layout section of `L06.md`: the single block, both layers, collision/surfaces, the arc camera path and its limits, depth routes, the start position and the fight area.
- [ ] `L06.GP.02` (P5.ALL.07) Lair traversal. Specify only what differs from the existing ENG1 hero spec, including any hero state the gaze imposes.
- [ ] `L06.GP.03` (P5.ALL.08) For every skeleton type, fill in an S4 encounter entry: spawn kind, handlers, states, attacks, damage, defeat and rewards.
- [ ] `L06.GP.04` (P5.ALL.08) Medusa encounter:
  - entry trigger and spawn
  - drawing: models, sprites, or both
  - gaze and projectile attacks: telegraphs, hit tests, effect on the hero, avoidance
  - the defeat mechanic as observed (reflection or otherwise), vulnerable windows, health
  - phases (only those observed), defeat and exit
  - difficulty differences

  Every row carries an evidence label and RVA/tick references.
- [ ] `L06.GP.05` (P5.ALL.04, P5.ALL.11) Scripts and progression: pickups and props, checkpoints and respawn rules, and the defeat → tally → ENG1 → ENG3 switch → L07 load.
- [ ] `L06.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L06.GP.07` (P5.ALL.13) Gameplay-coverage rows for L06. Each scenario links to its spec section and its evidence.

## Done when

- `L06.md` covers every L06 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
