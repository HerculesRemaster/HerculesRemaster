# L03 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of FOREST gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` (the ENG1 hero, reused); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L03.md`, L03 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L03.GP.01` (P5.ALL.02–03) Layout section of `L03.md`: blocks/sections, layers and parallax, collision/surfaces, camera limits, and the foreground/background path topology with its switch points.
- [ ] `L03.GP.02` (P5.ALL.07) Traversal: background-path switching, tree climbing, barriers and cracks, and secret routes. Specify only what differs from the existing ENG1 hero spec.
- [ ] `L03.GP.03` (P5.ALL.08) For every regular enemy, fill in an S4 encounter entry: spawn kind, handlers, states, attacks, damage, defeat, rewards, and any weapon requirement (the centaur special-sword LEAD needs proof from code or a trace).
- [ ] `L03.GP.04` (P5.ALL.08) Nessus encounter:
  - entry trigger and the result of `L03.TR.04` (stage load or in-map)
  - states, telegraphs, vulnerable windows and the ride mechanic
  - health and damage, phases (only those observed), finish/defeat, exit
  - difficulty differences

  Every row carries an evidence label and RVA/tick references.
- [ ] `L03.GP.05` (P5.ALL.04) Scripts: pickups and letters, Hermes markers and checkpoints, respawn rules, and the completion trigger → tally → L04 load.
- [ ] `L03.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L03.GP.07` (P5.ALL.13) Gameplay-coverage rows for L03. Each scenario links to its spec section and its evidence.

## Done when

- `L03.md` covers every L03 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
