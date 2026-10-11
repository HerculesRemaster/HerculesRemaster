# L10 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L10 scenario on Medium from first entry through the final encounter, ending and credits, with the Herculean comparison and the Beginner availability check. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l10-vortex-of-souls/<scenario>/<run-id>/`, `reference/captures/full-game/l10-vortex-of-souls/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L10.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L09 (an ENG3 → ENG1 module switch). For repeated scenes, use an S6-verified password or a replay checkpoint. Record entry method, start index and initial state in `run.json`.
- [ ] `L10.TR.02` (P3.ALL.03) Identity and resources. On the first VORTEX load, log the level-ID global (11), the module, the map opens (518/519), the 0xdf800 header read and the stream-chunk load. Because the map has no spawns, log **every** FS open and bank load after the map load, and every object creation with its caller RVA. Take a runtime VRAM dump for S2.03 (the 13,816 B overrun). No demo exists for this level, so this run is also the first state/timing baseline.
- [ ] `L10.TR.03` (P3.ALL.08) `vortex-entry`. Log `input.jsonl` against hero position x/y/z, velocity, state id 0x72108, active handler and frame per tick. Derive the observed movement mode and input mapping, and check them against `L10.RE.03`. Record camera behaviour.
- [ ] `L10.TR.04` (P3.ALL.08) `souls-traversal`. Explore all reachable routes and soul interactions. Record object creation/removal, collisions, damage and scene boundaries from state and resource changes only.
- [ ] `L10.TR.05` (P3.ALL.08) `final-encounter`. Capture the entry trigger, every observed attack and vulnerable window, hits taken and dealt per state, and the bank/sound/music calls around it. Log the actor state and health fields recovered in `L10.RE.04` per tick. Split phases into new scenario files only when the observed state changes.
- [ ] `L10.TR.06` (P3.ALL.08) `rescue-objective`. Capture the objective as it actually happens: prerequisites, interaction, success and failure.
- [ ] `L10.TR.07` (P3.ALL.08) `completion-ending`. Capture from the completion trigger through every ending screen, the credits and the return to title. Log module requests, TGA/bank loads, `AUD_StartMusic` calls and any request for missing media (recorded as `missing_source_media`, never as extracted).
- [ ] `L10.TR.08` (P3.ALL.08) `death-retry`. Die before and inside the final encounter. Record respawn index/position, encounter reset and objective persistence. Record the game-over path if one is reached.
- [ ] `L10.TR.09` (P3.ALL.09) `difficulty-herculean`. Use matched routes. Compare load resources first, then object creation, damage and final-encounter health/timing. Add dedicated diff scenarios wherever they differ.
- [ ] `L10.TR.10` (P3.ALL.10) `beginner-unavailable`. This is an availability check, not a gameplay capture. Together with `L08.TR.09` and S6.06, record what follows Beginner L08, then attempt normal, password and F3 entry where available. Never force the internal ID. Record the observed rejection or endpoint.
- [ ] `L10.TR.11` (P3.ALL.07) `full-progression`. Capture the L09 → L10 → completion → ending → credits segment as the final link of the S6.08 linked run.
- [ ] `L10.TR.12` (P3.ALL.11–12) Collect decoder pairs for VORTEX tiles/models/banks and the ending screens, then update coverage dimensions separately.

## Done when

- Every L10 scenario has a run, with tick ranges, in its JSON and in coverage.
- The runtime source of every L10 actor is recorded (caller RVA and tick).
- The ending/credits sequence is captured to the return to title; the Beginner availability result and the Herculean comparison are recorded.

## Notes
