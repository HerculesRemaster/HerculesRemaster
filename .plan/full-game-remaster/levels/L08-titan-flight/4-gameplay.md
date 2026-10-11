# L08 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of TITANS gameplay, centred on Pegasus flight. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `docs/gameplay-spec.md` (the ENG1 hero, cited only where code shows a shared path); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/gameplay/flight.md`, `docs/full-game/levels/L08.md`, L08 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L08.GP.01` (P5.ALL.02–03) Layout section of `L08.md`: blocks/sections, the 5 layers and parallax, collision/surfaces, camera records and limits, depth usage, and the section boundaries.
- [ ] `L08.GP.02` (P5.ALL.10) Write `flight.md`:
  - how flight is entered and which hero/Pegasus states and handlers run
  - axes and depth, input mapping, speeds and limits
  - attacks available in flight and what they spawn
  - motion and camera (forced or player-driven scroll), screen bounds
  - damage, knock-back, death and retry while flying

  Keep constants in their original representation. Each rule carries an evidence label and RVA/tick references. `L08.md` links to it and adds only TITANS-specific values.
- [ ] `L08.GP.03` (P5.ALL.08) For every Titan and other hazard, fill in an S4 encounter entry: spawn kind, handlers, states, telegraphs, attacks, damage, resolution/exit, and difficulty guards. Phase counts come only from observation; unknown values stay null.
- [ ] `L08.GP.04` (P5.ALL.08, P5.ALL.04) God rescues: trigger, interaction, reward, persistence, and whether the rescues gate completion.
- [ ] `L08.GP.05` (P5.ALL.04, P5.ALL.11) Scripts: pickups, checkpoints, respawn rules, and the completion trigger → tally → L09 load. Reference S6.05 for the module switch and S6.06 for the Beginner endpoint rather than re-specifying them.
- [ ] `L08.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L08.GP.07` (P5.ALL.13) Gameplay-coverage rows for L08. Each scenario links to its spec section and its evidence.

## Done when

- `flight.md` and `L08.md` cover every L08 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
