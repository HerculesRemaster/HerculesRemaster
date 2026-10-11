# L05 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L05 scenario on Medium from first entry through the Hydra fight to the L06 load, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l05-hydra-canyon/<scenario>/<run-id>/`, `reference/captures/full-game/l05-hydra-canyon/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L05.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L04. For repeated fights, use an S6-verified password or a replay checkpoint. There is no HYDRA demo, so the first replayable start comes from a recorded input run (S1.05). Record entry method, start index and initial state in `run.json`.
- [ ] `L05.TR.02` (P3.ALL.08) `canyon-entry`. Record the map opens (541/517), every bank/model/sound load, the two kind-0x31 spawn events and what they create, the first controllable frame and the music start. Take a runtime VRAM dump after load for S2.03 (3,156 B overrun).
- [ ] `L05.TR.03` (P3.ALL.08) `hydra-attacks`. Capture every observed attack cycle: telegraph, active window, hit box, damage dealt to the hero. Log the Hydra state fields recovered in `L05.RE.03` per tick.
- [ ] `L05.TR.04` (P3.ALL.08) `hydra-damage-phases`. Apply damage using the observed mechanics. Log per tick the Hydra health, head/part fields and state, and any new resource or attack sequence. Split phases into new scenario files only when the observed state changes. The phase count stays null until it is observed.
- [ ] `L05.TR.05` (P3.ALL.08) `hydra-defeat`. Capture the defeat sequence, the completion trigger, the tally, and the L06 load.
- [ ] `L05.TR.06` (P3.ALL.08) `death-retry`. Die early and late in the fight. Record respawn index/position, whether the Hydra's health and parts reset, and the game-over path if one is reached.
- [ ] `L05.TR.07` (P3.ALL.09) `difficulty-beginner` / `difficulty-herculean`. Use matched routes. Compare load resources first, then Hydra health, damage, attack timing and part counts. Add dedicated diff scenarios (for example `L05.hydra-damage-herculean`) wherever they differ.
- [ ] `L05.TR.08` (P3.ALL.07) `full-progression`. Capture the L04 → L05 (ENG1 → ENG1; record whether the module reloads) → defeat/tally → L06 load segment, as a link in the S6 linked run.
- [ ] `L05.TR.09` (P3.ALL.11–12) Collect decoder pairs for HYDRA tiles, sprite frames and model draws reset per scene, then update coverage dimensions separately.

## Done when

- Every L05 scenario has a run, with tick ranges, in its JSON and in coverage.
- The Hydra's states and damage are logged per tick across a complete fight.
- A runtime VRAM dump of HYDRA exists for S2.03.
- The difficulty comparisons are recorded.

## Notes
