# L05 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of HYDRA gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` (the ENG1 hero, reused); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L05.md`, L05 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L05.GP.01` (P5.ALL.02–03) Layout section of `L05.md`: the single block, both layers, collision/surfaces, the arc camera path and its limits, arena boundaries and the start position.
- [ ] `L05.GP.02` (P5.ALL.07) Hero movement in the arena. Specify only what differs from the existing ENG1 hero spec (for example movement along the arc camera path, if `L05.RE.04` shows it changes control).
- [ ] `L05.GP.03` (P5.ALL.08) Hydra encounter:
  - entry trigger and spawn (kind 0x31, templates 11 and 12)
  - drawing: models, sprites, or both
  - states, telegraphs, attacks and vulnerable windows
  - head/part structure, cut and regrowth rules, health and damage
  - phases (only those observed), defeat and exit
  - difficulty differences

  Every row carries an evidence label and RVA/tick references.
- [ ] `L05.GP.04` (P5.ALL.04) Scripts: arena triggers, any hazards or pickups, and the defeat → completion trigger.
- [ ] `L05.GP.05` (P5.ALL.11) Death and retry inside the fight (respawn point, boss reset), and completion → tally → L06 load.
- [ ] `L05.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L05.GP.07` (P5.ALL.13) Gameplay-coverage rows for L05. Each scenario links to its spec section and its evidence.

## Done when

- `L05.md` covers every L05 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
