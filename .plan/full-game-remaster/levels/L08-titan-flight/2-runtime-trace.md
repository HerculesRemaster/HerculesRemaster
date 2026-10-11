# L08 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L08 scenario on Medium from first entry through the flight to the L09 load, with Beginner/Herculean comparisons and the Beginner endpoint. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l08-titan-flight/<scenario>/<run-id>/`, `reference/captures/full-game/l08-titan-flight/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L08.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L07 (an ENG3 → ENG1 module switch). For repeated scenes, use an S6-verified password or a replay checkpoint. Record entry method, start index and initial state in `run.json`.
- [ ] `L08.TR.02` (P3.ALL.03) Identity. On the first TITANS load, log the level-ID global (9), the module, the map opens (538/539), the 0xf7000 header read and each stream-chunk load (0x1f9e0) per block. No demo exists for this level, so this run is also the first state/timing baseline.
- [ ] `L08.TR.03` (P3.ALL.08) `flight-opening`. Log `input.jsonl` against hero (and Pegasus, if separate) position x/y/z, velocity, state id 0x72108, active handler and frame per tick. Derive the observed axis mapping and attack changes, and check them against `L08.RE.03`. Record camera position and whether it moves without input.
- [ ] `L08.TR.04` (P3.ALL.08) `flight-sections`. Do a complete traversal. Record spawn/despawn events, stream-chunk loads, camera changes, depth changes and pickups. Refine section boundaries from state and resource changes only.
- [ ] `L08.TR.05` (P3.ALL.08) `titan-encounters`. Capture each Titan as encountered: order, telegraph, attacks, hits taken and dealt, how the encounter resolves, and the bank/sound loads around it. Log the actor state fields recovered in `L08.RE.04` per tick. Split into per-Titan scenario files only when the observation shows separate encounters.
- [ ] `L08.TR.06` (P3.ALL.08) `god-rescues`. Capture every rescue: prerequisite, interaction, score/HUD change, and whether it persists across death.
- [ ] `L08.TR.07` (P3.ALL.08) `zeus-completion`. Capture the final rescue/exit as it actually happens, the tally, and the L09 TORMENT load (ENG1 → ENG3 module switch; S6.05).
- [ ] `L08.TR.08` (P3.ALL.08) `death-retry`. Die in each section type, including during a Titan encounter. Record respawn index/position, flight state on restore, actor reset and rescue persistence. Record the game-over path if one is reached.
- [ ] `L08.TR.09` (P3.ALL.09, P3.ALL.10) `difficulty-beginner` / `difficulty-herculean`. Use matched routes. Compare load resources first, then spawns, damage and Titan timing. Add dedicated diff scenarios wherever they differ. On Beginner, also capture everything that follows L08 completion (end screen, credits, return to title, or a further load) and hand it to S6.06.
- [ ] `L08.TR.10` (P3.ALL.07) `full-progression`. Capture the L07 → L08 → completion/tally → L09 load segment, as a link in the S6.08 linked run.
- [ ] `L08.TR.11` (P3.ALL.11–12) Collect decoder pairs for TITANS tiles/models/banks reset per section, then update coverage dimensions separately.

## Done when

- Every L08 scenario has a run, with tick ranges, in its JSON and in coverage.
- The flight input/motion/camera behaviour is observed per tick and matched against `L08.RE.03`.
- The Beginner endpoint is observed and handed to S6.06; the difficulty comparisons are recorded.

## Notes
