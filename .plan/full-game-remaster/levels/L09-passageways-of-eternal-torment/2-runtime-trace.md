# L09 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L09 scenario on Medium from first entry to the L10 load, with the Herculean comparison and the Beginner availability check. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; `S3.06` ENG3 hooks; S6.01–S6.03 (difficulty, verified entry); `S6.06` (Beginner boundary); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l09-passageways-of-eternal-torment/<scenario>/<run-id>/`, `reference/captures/full-game/l09-passageways-of-eternal-torment/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L09.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L08. For repeated scenes, use an S6-verified password or a replay checkpoint. No archived demo is named for ID 10. Record entry method, start index and initial state in `run.json`.
- [ ] `L09.TR.02` (P3.ALL.03) Identity at load. Confirm the level ID (10), the module (ENG3), the map opens (520/521), the start index and the difficulty, and log what the descriptor fields +0xc/+0x10/+0x14 targets hold at load (from `L09.RE.01`).
- [ ] `L09.TR.03` (P3.ALL.08) `passage-opening` + `passage-sections`. Do a complete traversal and record per tick: speed, lane/depth, jumps, obstacle activations, spawns/despawns, pickups, section boundaries and any FS opens at those boundaries.
- [ ] `L09.TR.04` (P3.ALL.08) `skeletons-lost-souls`. Capture each skeleton and lost-soul encounter with avoidance, and with combat if the code allows it; record collisions, damage and effects.
- [ ] `L09.TR.05` (P3.ALL.08) `chariot-stalactites`. Record chariot passes and stalactite activations with their timing relative to each other, plus one successful and one failed route.
- [ ] `L09.TR.06` (P3.ALL.08) `river-falls`. Record a controlled fall into the River Styx: animation, audio, life decrement, respawn position/section and reset. Record the game-over path if one is reached.
- [ ] `L09.TR.07` (P3.ALL.08) `passage-completion`. Record the final hazard/event, the tally, and every load and music change up to the L10 VORTEX map open.
- [ ] `L09.TR.08` (P3.ALL.09) `difficulty-herculean`. Use matched routes. Compare load resources first, then spawns, hazard timing, damage and falls. Add dedicated diff scenarios wherever they differ.
- [ ] `L09.TR.09` (P3.ALL.10) `beginner-unavailable`. Run through `S6.06`: complete Beginner L08 and record what follows; try normal entry and Beginner passwords/F3 where available, and confirm that no TORMENT gameplay load happens. Do not force the internal ID. This is an availability check, not a gameplay capture. Set the cell to `UNAVAILABLE_BY_DESIGN` only after the observation.
- [ ] `L09.TR.10` (P3.ALL.07) `full-progression`. Capture the L08 → L09 (ENG1 → ENG3 module switch through EXE 0x4040) → completion/tally → L10 (ENG3 → ENG1) segment, as a link in the S6 linked run. Record the state carried across both switches.
- [ ] `L09.TR.11` (P3.ALL.11–12) Collect decoder pairs for TORMENT map data, CIF palette and SRC3 banks reset per scene, log every bank FS open, then update coverage dimensions separately.

## Done when

- Every L09 scenario has a run, with tick ranges, in its JSON and in coverage.
- The Herculean comparison is recorded.
- The Beginner availability result is observed and recorded.

## Notes
