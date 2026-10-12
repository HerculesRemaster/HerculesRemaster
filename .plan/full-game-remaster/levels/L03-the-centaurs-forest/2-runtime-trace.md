# L03 Phase 2 — Runtime trace and complete traversal

Goal: capture and trace every L03 scenario on Medium from first entry through Nessus to the L04 load, with Beginner/Herculean comparisons. Scenario JSONs are refined using only the [discovery workflow](../../reference/scenarios/README.md).
Needs: S1 runner/identity hooks; S6.01–S6.03 (difficulty, verified entry); P2F proxy present (windowed override recorded).
Outputs: `analysis/traces/full-game/l03-the-centaurs-forest/<scenario>/<run-id>/`, `reference/captures/full-game/l03-the-centaurs-forest/<scenario>/<run-id>/`, updated scenario JSONs and `coverage.{md,json}` rows.

## Tasks

- [x] `L03.TR.01` (P3.ALL.04) Entry. The canonical entry is the linked Medium progression from L02. For repeated scenes, use an S6-verified password or a replay checkpoint. Record entry method, start index and initial state in `run.json`.
- [x] `L03.TR.02` (P5.ALL.06) Replay DEMO3_1 under trace. Confirm the level ID (3), the start index (1 per JSON) and the map opens (515/516). It is the first state/timing check for this level.
- [ ] `L03.TR.03` (P3.ALL.08) `forest-main-route` + `background-paths` + `forest-secrets`. Do a complete traversal and record spawn/despawn events, layer/depth switches, secrets and pickups. Refine scene boundaries from state and resource changes.
- [x] `L03.TR.04` (P3.ALL.03) **Nessus transition** (`nessus-approach`).
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
- `L03.TR.01` (2026-10-12): run `analysis/traces/full-game/l03-the-centaurs-forest/forest-main-route/20261012-064207-password-medium`, entry = S6-verified password table index 14 (FOREST Medium).
  - TITLE gs[0] goes 2 → 3 at tick 1428/1429. `MOD_RequestModule` `\SRC1\ENGINE\ENGINE1` and the ENG1 load at 1566; map 515 opens at 1567.
  - Initial state (run.json): start 0, restart 0, difficulty 1, lives 4, health 128, mode 5 → 0 at 1583; hero at x 958, y 672.
  - Audio: `AUD_LoadLevelSonix` with FOREST's record ENG1 0x4f258 (= S5.03), then `AUD_StartMusic(0, 0)` at 1567.
- `L03.TR.03` partial: the same run walked right with periodic jumps from tick 1700 and stopped at x ≈ 2100 by tick 1855 (first obstacle). An open-loop pad script can't traverse; a full traversal needs played input (replay of a recorded play) or a closed-loop controller.
- `L03.TR.02` done (2026-10-12): `analysis/traces/full-game/l03-the-centaurs-forest/forest-main-route/20261012-064511-attract-config` (attract, 36,309 ticks, hero log).
  - The attract cycle repeats every 21,938 ticks: DEMO1_1/1_2/1_3 (TRAINING starts 1/3/6), DEMO2_1 (GAUNTLET), DEMO3_1 (FOREST), DEMO5_1, DEMO5_2 (THEBES). Each demo runs 881 ticks; DEMO3_1 runs at ticks 13672–14553 (and again from 35613).
  - DEMO3_1: `M:\DEMO\DEMO3_1.DAT` (archive 469) opens at 13672 after map 515 and CIF 516. Level 3, start/restart 1 (= the JSON), lives 3, health 128, difficulty 0 (demo byte 4), mode 5 → 0 at 13688.
  - The hero spawns at (5280, 544, z 384) against start record 1 (5312, 464, z 384), i.e. x − 32 and ground-snapped y. The same offset shows in the THEBES demos (L04.TR.02).
- `L03.TR.04` done (2026-10-12), classified **(b) an in-FOREST section**: run `analysis/traces/full-game/l03-the-centaurs-forest/nessus-approach/20261012-070937-password-medium` (password 14 + DEBUG start index 5 via `HERC_HOOK_START`; run.json `debug_only`, so coverage is not upgraded).
  - Module ENG1 and gs[0] = 3 stay unchanged throughout; there is no module request and no other map. The trigger fires at once from start 5:
    - `q:\NESSUS\SFX.SFX` (605) at 1568 and `MUSIC.MUS` (589) at 1569;
    - `AUD_StopMusic` + `AUD_StartMusic(0, 0)` at 1571 switch to the Nessus music segment;
    - the NESSUS\Basic anims start at 1570 (bank 68, type 0x2f).
  - Banks cycle as decompiled (RE.04): Punch 1582, Look 1667, Ride 1685, Kick 1784, repeating.
  - `levels.json` `extra_stage_descriptors[0].runtime_relationship` = IN_FOREST_SECTION (b), with run and ticks.
- `L03.TR.05` / `L03.TR.06` partial (same run, hero idle):
  - Nessus hit the idle hero to 0 health four times: ticks 2254→2705, 2881→3324, 3500→3951; about 450 ticks per life at Medium damage 35.
  - Every death respawned at start/restart 5 (mode 5 → 0, health 128).
  - Lives 4 → 0, then gs+3 = 0x22 at 4112 (opcode −11, no lives) → TITLE at 4111/4115 (title music 4): the game-over path.
  - The ride/reject/finish phases need hero input (a played or recorded run).

