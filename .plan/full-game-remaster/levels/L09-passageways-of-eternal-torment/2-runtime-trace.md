# L09 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L09 scenario on Medium from first entry to the L10 load, with the Herculean comparison and the Beginner availability check. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; `S3.06` ENG3 hooks; S6.01–S6.03 (difficulty, verified entry); `S6.06` (Beginner boundary); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l09-passageways-of-eternal-torment/<scenario>/<run-id>/`, `reference/captures/full-game/l09-passageways-of-eternal-torment/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [ ] `L09.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L08. For repeated scenes, use an S6-verified password or a replay checkpoint. No archived demo is named for ID 10. Record entry method, start index and initial state in `run.json`.
- [x] `L09.TR.02` (P3.ALL.03) Identity at load. Confirm the level ID (10), the module (ENG3), the map opens (520/521), the start index and the difficulty, and log what the descriptor fields +0xc/+0x10/+0x14 targets hold at load (from `L09.RE.01`).
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
- `L09.TR.01` / `TR.02` (2026-10-12): run `analysis/traces/full-game/l09-passageways-of-eternal-torment/full-progression/20261012-070726-password-config`. Entry is an S6-verified password (repeated-scene entry). The canonical linked Medium progression is still open, because it needs a played run.
  - **Identity:** module_load ENG3, level 10, start 0, restart 0, difficulty 1, lives 4. Map MAP_PSX.BIN index 520 and CIF index 521 open through eng3 `LVL_LoadMap`.
  - **Descriptor at runtime:** +0x14 → `AUD_LoadLevelSonix` (record 0x38440 vol 70/110, `docs/full-game/audio.md`). The bank FS opens are exactly the +0x10 level list plus the start-0 list: TORMENT, SOULS, HERCULES, XTRA_DB1, LETTERS (482, 569, 561, 553, 577). No other SRC3 bank opens.
  - Runtime VRAM and palette checks are in phase 3 (AS.02).
- `L09.TR.01` / `TR.02` (2026-10-12): run `analysis/traces/full-game/l09-passageways-of-eternal-torment/full-progression/20261012-070726-password-config` (password 21 Medium). Identity and bank opens are recorded in TR.02 above.
- `L09.TR.06` (2026-10-12, progress; same run, no input from load at tick 1611):
  - The hero runs on with no input. Restart index 1 is set at tick 1692 (start record 1 = x 1024 on track 0).
  - **Deaths:** at ticks 2035 and 2405 (370 ticks apart, the same route twice), each with health 88 rather than 0. Lives drop 4→3→2; respawn at start 1 (mode 5 for 50 ticks, then play).
  - Health not reaching 0 points to a fall/kill-plane death (`HERO_Die` path, `chase.md`) and not damage. The fall position, the death animation and the River Styx identification need a run with `-Hero`.
  - **Determinism:** the two deaths repeat with the same period.
- `L09.TR.06` (2026-10-12, uncontrolled fall captured): run `analysis/traces/full-game/l09-passageways-of-eternal-torment/river-falls/20261012-072443-password-config` (password 21, no input, `-Hero`; ENG3 hero logging was added to the hook: x/y/z, state, track, health).
  - **Fall:** the hero runs at speed 12.0 and loses the ground at x ≈ 3840 (column 30, inside the attribute-code-0x63 run, columns 29–38 of track 0). It enters state 3 (fall) and dies on reaching y 179 > 170 (code 0x63/0x77 rule, `chase.md`): state 6, life −1 (4→3, then 3→2 at the same x).
  - **Respawn:** after 50 ticks at start 1 (x 1024, y 4, track 0), health restored to 128.
  - **Hits before the fall:** state 5 at x ≈ 2404/2785/3595 (health 128→113→88).
  - **Determinism:** the third life is hit at different x (2288, 2529), so a random source acts here (LVL_Rand 0x1010 / RT_RandMod_slot, see `eng3-tables.md` object kinds).
  - Still open: the controlled fall (with input) and the game-over path.
