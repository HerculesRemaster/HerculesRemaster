# L10 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of VORTEX gameplay and the game's ending. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` and `docs/full-game/gameplay/flight.md` (cited only where code shows a shared path); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L10.md`, L10 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L10.GP.01` (P5.ALL.02–03) Layout section of `L10.md`: the single layer, the model/effect geometry, collision/surfaces, camera records and limits, and the scene boundaries.
- [ ] `L10.GP.02` (P5.ALL.07) Movement mode in the vortex: states/handlers, input mapping, depth and camera. Specify only what differs from the existing ENG1 hero or flight specs, and link those for shared rules.
- [ ] `L10.GP.03` (P5.ALL.08) For every soul, enemy and hazard, fill in an S4 encounter entry: creator RVA (since the map has no spawns), handlers, states, attacks, damage, defeat and rewards.
- [ ] `L10.GP.04` (P5.ALL.08) Final encounter:
  - entry trigger and actor creation
  - states, telegraphs and vulnerable windows
  - health and damage, phases (only those observed; unknown counts stay null), defeat, exit
  - Herculean differences

  Every row carries an evidence label and RVA/tick references. The actor is named from code or runtime bank loads only.
- [ ] `L10.GP.05` (P5.ALL.04, P5.ALL.11) Scripts: the rescue objective, checkpoints, respawn and game-over rules, and the completion trigger → ending → credits → title. Link S6.05's `progression.md` for the shared transition rules.
- [ ] `L10.GP.06` (P5.ALL.12) Difficulty table in `tuning.json` (Medium/Herculean): original representation, keyed by program+RVA, entity/state, difficulty and scenario. The Beginner cell is `UNAVAILABLE_BY_DESIGN` only after S6.06 has observed it.
- [ ] `L10.GP.07` (P5.ALL.13) Gameplay-coverage rows for L10. Each scenario links to its spec section and its evidence.

## Done when

- `L10.md` covers every L10 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The ending path is specified from completion to the return to title.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
