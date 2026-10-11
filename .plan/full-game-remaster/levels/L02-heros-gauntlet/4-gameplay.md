# L02 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of GAUNTLET gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `S3.07` runner model (`docs/full-game/gameplay/chase.md`, reused, not re-derived); S4 `encounters.md` template; S6 checkpoint/progression docs.
Outputs: `docs/full-game/levels/L02.md`, L02 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [ ] `L02.GP.01` (P5.ALL.02–03) Layout section of `L02.md`: track sections, lanes, crossroads and branch topology with their rejoin points, collision/surfaces (ponds and platforms included), and the behind-the-hero camera limits.
- [ ] `L02.GP.02` (P5.ALL.09) Runner use in GAUNTLET: speed range and Up/Down control, forced motion, jump, branch selection, and the Hermes Sandals speed state. Specify only what differs from `chase.md` or what GAUNTLET parameterizes (constants per level, in original representation).
- [ ] `L02.GP.03` (P5.ALL.08) For every hazard, fill in an S4 encounter-style entry: kind, handlers, activation trigger and timing, collision shape (from code), damage or death, and any rule that changes it (boulder smash with sandals, invincibility with the helmet, passing fallen pillars at low speed). Each rule needs proof from code or a trace.
- [ ] `L02.GP.04` (P5.ALL.08) Enemies and dummy enemies: spawn kind, approach, lane behavior, collision, defeat or pass-by.
- [ ] `L02.GP.05` (P5.ALL.04) Scripts: pickups and letters, damsel rescues, power-ups, Hermes markers and checkpoints, respawn rules, and the completion trigger → tally → L03 load.
- [ ] `L02.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `L02.GP.07` (P5.ALL.13) Gameplay-coverage rows for L02. Each scenario links to its spec section and its evidence.

## Done when

- `L02.md` covers every L02 scenario, and every required behavior links to a handler RVA and a trace tick range.
- The runner rules L02 establishes are fed back into `chase.md` (through S3) for L07 and L09 to reuse.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
