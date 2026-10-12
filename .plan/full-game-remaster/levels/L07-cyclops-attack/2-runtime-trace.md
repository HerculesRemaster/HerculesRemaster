# L07 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L07 scenario on Medium from first entry to the L08 load, covering every lane and the Cyclops end sequence, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; `S3.06` ENG3 hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l07-cyclops-attack/<scenario>/<run-id>/`, `reference/captures/full-game/l07-cyclops-attack/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L07.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L06. For repeated scenes, use an S6-verified password or a replay checkpoint. No archived demo is named for ID 8, so a recorded input replay must come from this project's own runs. Record entry method, start index and initial state in `run.json`.
- [x] `L07.TR.02` (P3.ALL.03) Identity at load. Confirm the level ID (8), the module (ENG3), the map opens (491/494) and the start index, and log what the descriptor fields +0xc/+0x10/+0x14 targets hold at load (from `L07.RE.01`).
- [ ] `L07.TR.03` (P3.ALL.08) `chase-opening` + `chase-middle`. Do a complete traversal and record per tick: speed, lane, jumps, Cyclops position/state, obstacle activations, spawns/despawns, pickups and section boundaries.
- [ ] `L07.TR.04` (P3.ALL.08) `alternate-lanes`. Take the stairs on both sides wherever they exist, and record the lane-change tick, input and the return to the street. If a lane holds content the existing scenarios don't cover, append a scenario through the discovery workflow. No existing scenario ID changes.
- [ ] `L07.TR.05` (P3.ALL.08) `obstacles`. Capture each column, citizen and chariot event with one avoidance and one collision. Log activation ticks and run the same route twice to check whether the schedule repeats.
- [ ] `L07.TR.06` (P3.ALL.08) `chase-completion`. Record the final section, the Cyclops outcome, every bank load around it (watch for CYEND 0493), the score/tally and the music change.
- [ ] `L07.TR.07` (P3.ALL.08) `death-retry`. Die in different lanes and sections; record the respawn position/lane, speed, obstacle and Cyclops reset, and the life decrement. Record the game-over path if one is reached.
- [ ] `L07.TR.08` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes and lanes. Compare load resources first, then traffic schedule, speed, Cyclops behavior and damage. Add dedicated diff scenarios wherever they differ.
- [ ] `L07.TR.09` (P3.ALL.07) `full-progression`. Capture the L06 → L07 (ENG1 → ENG3 module switch through EXE 0x4040) → completion/tally → L08 (ENG3 → ENG1) segment, as a link in the S6 linked run. Record the state carried across both switches.
- [ ] `L07.TR.10` (P3.ALL.11–12) Collect decoder pairs for CYCLOPS map data, CIF palette and SRC3 banks reset per scene, log every bank FS open, then update coverage dimensions separately.

## Done when

- Every L07 scenario has a run, with tick ranges, in its JSON and in coverage.
- Every lane and the Cyclops end sequence are traced.
- The difficulty comparisons are recorded.

## Notes
- `L07.TR.01` / `TR.02` (2026-10-12): run `analysis/traces/full-game/l07-cyclops-attack/full-progression/20261012-070546-password-config`. Entry is an S6-verified password (repeated-scene entry). The canonical linked Medium progression is still open, because it needs a played run.
  - **Identity:** module_load ENG3, level 8, start 0, restart 0, difficulty 1, lives 4. Map MAP_PSX.BIN index 491 and CIF index 494 open through eng3 `LVL_LoadMap`.
  - **Descriptor at runtime:** +0x14 → `AUD_LoadLevelSonix` (record 0x38398 vol 75/110, `docs/full-game/audio.md`). The bank FS opens are exactly the +0x10 level list plus the start-0 list: CYCLOPS, PEOPLE, CYEND, HERCULES, XTRA_DB1, LETTERS (480, 547, 493, 561, 553, 577). No other SRC3 bank opens.
  - Runtime VRAM and palette checks are in phase 3 (AS.02).
- `L07.TR.07` (2026-10-12, progress): run `analysis/traces/full-game/l07-cyclops-attack/death-retry/20261012-073756-cheat_select-medium`. Entry is the in-game cheat level select (S6), Medium, with the no-life-loss toggle on, `-Hero`. The entry pad of these runs (before the 08:57 fix) ended with UP ×5, CROSS and START ×2, which fell inside the level: brief speed-ups, one jump, and a pause of about 150 ticks. After that there is no input.
  - **Hits:** at x 2649 … 7396 (health 128 → 98 → 83 → … → 23). The steps are −30 (code 2: rock or column, T3) and −15 (T0: citizens).
  - **Death:** at x 9997, health −7 (state 6). With the toggle on, lives stay 4. Respawn at restart 1 after about 106 ticks; the hit sequence then repeats at about the same x.
  - Still open: the life decrement (a run without the toggle) and deaths in other lanes and sections.
