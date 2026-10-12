# L09 Phase 4 — Gameplay reconstruction (specification)

Goal: write a code- and trace-backed specification of TORMENT gameplay. Under hybrid route A the original logic still runs, so this specification is for art/timing integration, coverage, and any later native port.
Needs: phases 1–3; `S3.07` runner model (`docs/full-game/gameplay/chase.md`), `docs/full-game/levels/L02.md` and `L07.md` (all reused, not re-derived); S4 `encounters.md` template; S6 checkpoint/progression/difficulty docs.
Outputs: `docs/full-game/levels/L09.md`, L09 rows in `extracted/meta/full-game/{gameplay-coverage,tuning}.json`.

## Tasks

- [x] `L09.GP.01` (P5.ALL.02–03) Layout section of `L09.md`: passage sections and boundaries, lanes/depth routes, river edges and other fall surfaces, collision, and camera limits.
- [x] `L09.GP.02` (P5.ALL.09) Runner use in TORMENT: speed range, lane/depth changes, jump, and anything that differs from `chase.md`, GAUNTLET or CYCLOPS (constants per level, in original representation).
- [x] `L09.GP.03` (P5.ALL.08) For skeletons and lost souls, fill in S4 encounter entries: kind, handlers, states, movement, collision, damage, and defeat or pass-by.
- [x] `L09.GP.04` (P5.ALL.08) Hazards: the chariot, falling stalactites (trigger and timing, including their relation to the chariot), and River Styx falls (condition, death sequence). Every row carries an evidence label and RVA/tick references.
- [x] `L09.GP.05` (P5.ALL.04) Scripts: pickups, checkpoints, respawn after a fall, and the completion trigger → tally → L10 load.
- [x] `L09.GP.06` (P5.ALL.12) Difficulty table in `tuning.json`: original representation, keyed by program+RVA, entity/state, difficulty and scenario. Medium and Herculean only; record the observed Beginner availability result from `L09.TR.09` with a link to `docs/full-game/gameplay/difficulty.md`.
- [x] `L09.GP.07` (P5.ALL.13) Gameplay-coverage rows for L09. Each scenario links to its spec section and its evidence.

## Done when

- `L09.md` covers every L09 Medium scenario, and every required behavior links to a handler RVA and a trace tick range.
- The behaviors that are still unknown are listed, each with the next trace or decompilation that would resolve it.

## Notes
- `L09.GP.01` (2026-10-12): `docs/full-game/levels/L09.md` "Layout" (VERIFIED-STATIC data; identifications marked LEAD where only data supports them).
- `L09.GP.02`–`GP.06` (2026-10-12): `docs/full-game/levels/L09.md`.
  - "Runner parameters" (GP.02): only the per-level values; the shared rules stay in `chase.md`.
  - Generated "Objects" tables (`tools/level/eng3_spec.py` from `L09-types.json`): hazards GP.03, enemies/NPCs GP.04, pickups/checkpoints/completion GP.05.
  - Difficulty (GP.06): `extracted/meta/full-game/tuning.json` entries `eng3.L09.kind*.damage` (hit code → columns of 0x380a0 per difficulty, `tools/state/tuning_export.py`). No ENG3 object reads gs+0xe.
  - Runtime cross-checks: the observed health drops match the table at Medium. TORMENT −15 (T0) and −25 (T2, stalactite), run `river-falls/20261012-072443`; CYCLOPS −30 (T3) and −15 (T0), run `death-retry/20261012-073756`.
  - Behaviour is VERIFIED-STATIC; identifications are graded VISUAL/LEAD per row.
- `L09.GP.07` (2026-10-12): `extracted/meta/full-game/gameplay-coverage.json` (`tools/meta/gameplay_coverage.py`) links each L09 scenario to its spec section and evidence runs. Scenarios without runs stay "spec" until phase 2 captures them.
