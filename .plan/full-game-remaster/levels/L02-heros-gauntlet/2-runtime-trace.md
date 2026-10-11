# L02 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L02 scenario on Medium from first entry to the L03 load, including both branches of every crossroad and the DEMO2_1 replay, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; `S3.06` ENG3 hooks (map load, spawn/despawn, hero state/position, frame requests, sound calls); `S3.08` DEMO2_1 decode; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l02-heros-gauntlet/<scenario>/<run-id>/`, `reference/captures/full-game/l02-heros-gauntlet/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L02.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L01. For repeated scenes, use an S6-verified password or a replay checkpoint. Record entry method, start index and initial state in `run.json`.
- [ ] `L02.TR.02` (P5.ALL.06) Replay DEMO2_1 under trace. Confirm the level ID (2), the start index (0 per JSON), the module (ENG3) and the map opens (557/556). Compare the load with the `gpu_10500` overlay observation. It is the first ENG3 state/timing check and the first runtime test of the S3 runner hooks.
- [ ] `L02.TR.03` (P3.ALL.08) `gauntlet-opening` + `gauntlet-middle` + `gauntlet-final`. Do a complete traversal and record per tick: speed, lane/position, jumps, obstacle activations, spawns/despawns, pickups and section boundaries. Refine scene boundaries from state and resource changes.
- [ ] `L02.TR.04` (P3.ALL.08) Crossroads. Take the left and the right branch at every crossroad found by `L02.RE.03`, and record the branch-selection tick, input and the rejoin point. If a branch holds content the existing scenarios don't cover, append a scenario through the discovery workflow. No existing scenario ID changes.
- [ ] `L02.TR.05` (P3.ALL.08) `hazards`. For each hazard type, capture one successful avoidance and one controlled hit, and log activation and collision ticks against the handler RVAs from `L02.RE.02`. Repeat the boulder and speed cases with and without the Hermes Sandals, and record one Helmet of Invincibility interval.
- [ ] `L02.TR.06` (P3.ALL.08) `rescue-events`. Rescue the damsel dummies encountered; record approach, interaction, score/UI effect and whether the state persists to the tally.
- [ ] `L02.TR.07` (P3.ALL.08) `checkpoint-retry`. Die before and after each Hermes level marker. Record the respawn position/index, speed and obstacle reset, and the life decrement. Record the game-over path if one is reached.
- [ ] `L02.TR.08` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes and matched branches. Compare load resources first, then obstacle schedule, speed, damage and enemy counts. Add dedicated diff scenarios wherever they differ.
- [ ] `L02.TR.09` (P3.ALL.07) `full-progression`. Capture the L01 → L02 (ENG1 → ENG3 module switch through EXE 0x4040) → completion/tally → L03 (ENG3 → ENG1) segment, as a link in the S6 linked run. Record the state carried across both switches.
- [ ] `L02.TR.10` (P3.ALL.11–12) Collect decoder pairs for GAUNTLET map data, CIF palette and SRC3 banks reset per scene, log every bank FS open, then update coverage dimensions separately.

## Done when

- Every L02 scenario has a run, with tick ranges, in its JSON and in coverage.
- Both branches of every crossroad and the DEMO2_1 replay are traced.
- The difficulty comparisons are recorded.

## Notes
