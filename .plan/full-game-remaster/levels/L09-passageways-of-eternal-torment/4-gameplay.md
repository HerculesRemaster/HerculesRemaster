# L09 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of TORMENT gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `S3.07` runner model (`docs/full-game/gameplay/chase.md`), `docs/full-game/levels/L02.md` and `L07.md` (all reused, not re-derived); S4 `encounters.md` template; S6 checkpoint/progression/difficulty docs.
Outputs: `docs/full-game/levels/L09.md`, L09 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L09.GP.01` (P5.ALL.02–03) Layout section of `L09.md`: passage sections and boundaries, lanes/depth routes, river edges and other fall surfaces, collision, and camera limits.
- [ ] `L09.GP.02` (P5.ALL.09) Runner use in TORMENT: speed range, lane/depth changes, jump, and anything that differs from `chase.md`, GAUNTLET or CYCLOPS (constants per level, in original representation).
- [ ] `L09.GP.03` (P5.ALL.08) For skeletons and lost souls, fill in S4 encounter entries: kind, handlers, states, movement, collision, damage, and defeat or pass-by.
- [ ] `L09.GP.04` (P5.ALL.08) Hazards: the chariot, falling stalactites (trigger and timing, including their relation to the chariot), and River Styx falls (condition, death sequence). Every row carries an evidence label and RVA/tick references.
- [ ] `L09.GP.05` (P5.ALL.04) Scripts: pickups, checkpoints, respawn after a fall, and the completion trigger → tally → L10 load.
- [ ] `L09.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario. Medium and Herculean only; record the observed Beginner availability result from `L09.TR.09` with a link to `docs/full-game/gameplay/difficulty.md`.
- [ ] `L09.GP.07` (P5.ALL.13) Gameplay-coverage rows for L09. Each scenario links to its spec section and its evidence.

## Done when

- `L09.md` covers every L09 Medium scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
