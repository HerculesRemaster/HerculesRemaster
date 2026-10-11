# L04 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L04 scenario on Medium from first entry to the L05 load, with Beginner/Herculean comparisons, and settle the Thebes substage question with evidence. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry, password encoding); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l04-the-big-olive/<scenario>/<run-id>/`, `reference/captures/full-game/l04-the-big-olive/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L04.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L03. For repeated scenes, use an S6-verified password or a replay checkpoint. The Part 1 and Part 2 passwords count as entry methods only after S6.03 accepts them. Record entry method, start index and initial state in `run.json`.
- [ ] `L04.TR.02` (P5.ALL.06) Replay `DEMO5_1` and `DEMO5_2` under trace. For each, confirm the level ID (5), the start index (6 and 2 per JSON), the map opens (513/514) and the hero start position. Compare the two start positions with the start records from `L04.RE.01`. This is the first state/timing check for this level and the first substage evidence.
- [ ] `L04.TR.03` (P3.ALL.08) `streets` + `background-stairs`. Do a complete traversal. Record block streaming (8 blocks), layer/depth switches on the stairs, spawn/despawn events and pickups. Refine scene boundaries from state and resource changes.
- [ ] `L04.TR.04` (P3.ALL.08) `enemy-encounters` + `rescue-events` + `destruction-secrets`. Record the actor kind and bank of each enemy (thugs, skeletons, Minotaur, harpies), every rescue attempt with success/failure and score, every destruction and debris event with the environment before and after, and the secrets found. Log every FS bank open, so ambiguous banks (RCROCK/RTROCK, MEG, PIG, SPY, MERV, ALEXFISH, PAIN, SKELWAR) are attributed by load.
- [ ] `L04.TR.05` (P3.ALL.03) **Thebes substage boundary** (`section-transition`).
  - Across the full Medium route, log the level-ID global, start index (game state +1/+2), any section/substage global from S1.01, the module, every FS open (watch for a reopen of 513/514 or any other map), every `AUD_StartMusic` call, and any tally or loading screen.
  - Repeat from an accepted Part 2 password (if S6.03 accepts it) and record its level ID and start index.
  - Classify the result as one of four cases:
    - (a) one THEBES load; "Part 2" is a start index or checkpoint region within it
    - (b) THEBES (ID 5) reloads with a different start index
    - (c) a separate load (another descriptor or map); append it through the discovery workflow as a stage descriptor attached to L04, never as ID 4 and never as a public level
    - (d) no boundary observed; the lead is documented as unconfirmed
  - Record the result, run path and ticks in `section-transition.json`. No scenario IDs change.
- [ ] `L04.TR.06` (P3.ALL.08) `checkpoint-completion`. Die before and after Hermes markers and on both sides of any boundary found in `L04.TR.05`. Record respawn index/position and actor/rescue/destruction reset. Record the game-over path if one is reached.
- [ ] `L04.TR.07` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes. Compare load resources first, then spawns, damage, rescue counts and the substage boundary. Add dedicated diff scenarios wherever they differ.
- [ ] `L04.TR.08` (P3.ALL.07) `full-progression`. Capture the L03 → L04 (ENG1 → ENG1; record whether the module reloads) → completion/tally → L05 load segment, as a link in the S6 linked run.
- [ ] `L04.TR.09` (P3.ALL.11–12) Collect decoder pairs for THEBES tiles/banks reset per scene, then update coverage dimensions separately.

## Done when

- Every L04 scenario has a run, with tick ranges, in its JSON and in coverage.
- Both demos are replayed and their starts are mapped to start records.
- The Thebes substage question is classified with evidence.
- The difficulty comparisons are recorded.

## Notes
