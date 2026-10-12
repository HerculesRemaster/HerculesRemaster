# L06 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L06 scenario on Medium from first entry through the Medusa fight to the L07 load, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks (both engines for the hand-off); S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l06-medusas-lair/<scenario>/<run-id>/`, `reference/captures/full-game/l06-medusas-lair/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [x] `L06.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L05. For repeated scenes, use an S6-verified password or a replay checkpoint. There is no MEDUSA demo, so the first replayable start comes from a recorded input run (S1.05). Record entry method, start index and initial state in `run.json`.
- [ ] `L06.TR.02` (P3.ALL.08) `lair-traversal`. Record the map opens (530/531), every bank/model/sound load, the six spawn events and what they create, depth/route switches and pickups. Take a runtime VRAM dump after load for S2.03 (84,096 B overrun).
- [ ] `L06.TR.03` (P3.ALL.08) `skeleton-encounters`. Record each skeleton's spawn, bank (0486 or 0566), states, attacks, damage and defeat. Log the FS bank opens, so the SKELWAR attribution is settled by load.
- [ ] `L06.TR.04` (P3.ALL.08) `gaze-projectiles`. Capture every gaze and projectile/venom attack: telegraph, active window, hit test, effect on the hero and recovery. Log the Medusa state fields recovered in `L06.RE.03` and the hero state per tick.
- [ ] `L06.TR.05` (P3.ALL.08) `reflection-defeat`. Capture the defeat mechanic as observed: the object or action involved, success and failure, damage to Medusa per hit, and the defeat sequence. Split phases into new scenario files only when the observed state changes. If the mechanic is not a reflection, record what it is; the scenario ID stays.
- [ ] `L06.TR.06` (P3.ALL.08) `death-retry`. Die in the lair and inside the fight. Record respawn index/position, whether Medusa's health and the skeletons reset, and the game-over path if one is reached.
- [ ] `L06.TR.07` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes. Compare load resources first, then skeleton spawns, damage, gaze/projectile timing and Medusa health. Add dedicated diff scenarios wherever they differ.
- [ ] `L06.TR.08` (P3.ALL.07) `full-progression`. Capture the L05 → L06 (ENG1 → ENG1; record whether the module reloads) → defeat/tally → **ENG1 → ENG3** module switch → L07 CYCLOPS load segment (first ENG3 map open of 491/494 via 0x1a930), as a link in the S6 linked run.
- [ ] `L06.TR.09` (P3.ALL.11–12) Collect decoder pairs for MEDUSA tiles, sprite frames and model draws reset per scene, then update coverage dimensions separately.

## Done when

- Every L06 scenario has a run, with tick ranges, in its JSON and in coverage.
- The defeat mechanic is classified with evidence.
- A runtime VRAM dump of MEDUSA exists for S2.03.
- The ENG1 → ENG3 hand-off to L07 is captured.
- The difficulty comparisons are recorded.

## Notes
- `L06.TR.01` done (2026-10-12): run `analysis/traces/full-game/l06-medusas-lair/lair-traversal/20261012-071743-password-medium`, S6-verified password table index 18 (MEDUSA Medium; `tools/trace/level_entry_sweep.ps1`, hero log, art_dump on → `art_index.csv` with 6903 new block keys).
  - Map 530 opens at tick 1391. Initial state: level 7, start 0, lives 4, health 128, difficulty 1, mode 5 → 0 after 15 ticks.
  - Hero first at (952, 736, 76), last at (952, 736, 76) (idle).
  - Audio: none logged in this run.
  - Idle deaths: health 0 at 2215 (lives → 3).
  - The canonical linked-progression entry is S6.08 (gamehercules-40).

