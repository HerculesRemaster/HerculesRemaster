# L03 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L03 scenario on Medium from first entry through Nessus to the L04 load, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l03-the-centaurs-forest/<scenario>/<run-id>/`, `reference/captures/full-game/l03-the-centaurs-forest/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L03.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L02. For repeated scenes, use an S6-verified password or a replay checkpoint. Record entry method, start index and initial state in `run.json`.
- [ ] `L03.TR.02` (P5.ALL.06) Replay DEMO3_1 under trace. Confirm the level ID (3), the start index (1 per JSON) and the map opens (515/516). It is the first state/timing check for this level.
- [ ] `L03.TR.03` (P3.ALL.08) `forest-main-route` + `background-paths` + `forest-secrets`. Do a complete traversal and record spawn/despawn events, layer/depth switches, secrets and pickups. Refine scene boundaries from state and resource changes.
- [ ] `L03.TR.04` (P3.ALL.03) **Nessus transition** (`nessus-approach`).
  - Log the level-ID global, start index, module, every FS open (watch for `NESSUS` paths, banks 0599–0819, `q:\NESSUS\` / `q:\HERC_NES\`) and every `AUD_StartMusic` call around the fight.
  - Classify the result as one of three cases:
    - (a) a separate ID-4 stage load
    - (b) an in-FOREST section
    - (c) something else, described in the notes
  - Update `levels.json` `extra_stage_descriptors[0].runtime_relationship` with run path and ticks, and refine the Nessus scenario JSONs. No scenario IDs change.
- [ ] `L03.TR.05` (P3.ALL.08) `nessus-attack-damage` + `nessus-defeat`. Capture every observed attack, ride, reject and finish, and the hits taken and dealt per state. Log the actor state and health fields recovered in `L03.RE.04` per tick. Split phases into new scenario files only when the observed state changes.
- [ ] `L03.TR.06` (P3.ALL.08) `checkpoint-retry`. Die before and after markers, including inside the Nessus fight. Record respawn index/position and actor reset. Record the game-over path if one is reached.
- [ ] `L03.TR.07` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes. Compare load resources first, then spawns, damage and Nessus health/timing. Add dedicated diff scenarios wherever they differ.
- [ ] `L03.TR.08` (P3.ALL.07) `full-progression`. Capture the L02 → L03 (ENG3 → ENG1 module switch) → completion/tally → L04 load segment, as a link in the S6 linked run.
- [ ] `L03.TR.09` (P3.ALL.11–12) Collect decoder pairs for FOREST tiles/models/banks reset per scene, then update coverage dimensions separately.

## Done when

- Every L03 scenario has a run, with tick ranges, in its JSON and in coverage.
- The Nessus relationship is classified with evidence.
- The difficulty comparisons are recorded.

## Notes
