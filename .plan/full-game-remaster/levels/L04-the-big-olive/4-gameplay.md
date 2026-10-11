# L04 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of THEBES gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` (the ENG1 hero, reused); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L04.md`, L04 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L04.GP.01` (P5.ALL.02–03) Layout section of `L04.md`: blocks/sections, the 5 layers and parallax, collision/surfaces, camera limits, the street/stairs/depth-route topology with its switch points, and the start records with their positions.
- [ ] `L04.GP.02` (P5.ALL.07) Traversal: background stairs and depth changes, destructible objects, and secret routes. Specify only what differs from the existing ENG1 hero spec.
- [ ] `L04.GP.03` (P5.ALL.08) For every enemy (thugs, skeletons, harpies, the Minotaur), fill in an S4 encounter entry: spawn kind, handlers, states, attacks, damage, defeat and rewards. If `L04.RE.05` shows the Minotaur is a boss, specify it as a full encounter with only the phases observed.
- [ ] `L04.GP.04` (P5.ALL.04) Scripts: harpy and citizen rescues (trigger, success/failure, scoring), destruction and falling debris (trigger, hazard, damage, persistent state), pickups and letters, secrets.
- [ ] `L04.GP.05` (P5.ALL.11) Substage and progression:
  - the result of `L04.TR.05`: how "Part 1 / Part 2" maps onto this build (start index, reload, separate load, or none)
  - Hermes markers and checkpoints, respawn rules on either side of the boundary
  - which passwords resume where, if S6 verifies them
  - the completion trigger → tally → L05 load

  Every row carries an evidence label and RVA/tick references.
- [ ] `L04.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L04.GP.07` (P5.ALL.13) Gameplay-coverage rows for L04. Each scenario links to its spec section and its evidence.

## Done when

- `L04.md` covers every L04 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The substage structure is specified from evidence, not from the walkthrough.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
