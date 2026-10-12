# S1 Runtime harness (scenario runner, identity, sync)

Goal: one reusable harness that enters any level/scenario reproducibly, records identity and synchronized frame/tick/input/resource data, and stores runs under the isolated full-game paths. Every level's `2-runtime-trace.md` depends on it.
Needs: P2F (proxy `ddraw.dll`, fullscreen route; traces may force `windowed`), the inherited tracer (`runtime/hooks/hercules_hook.c`, `tools/trace/run_trace.ps1`), `tools/state/trace_state.py`.
Slices: `P3.ALL.03`–`P3.ALL.06`, `P3.ALL.11`–`P3.ALL.12`.
Outputs:
- `tools/trace/run_scenario.ps1` (or an extension of `run_trace.ps1`, keeping the existing defaults)
- `analysis/traces/full-game/<level-slug>/<scenario>/<run-id>/{run.json,resource-usage.jsonl,input.jsonl,checkpoints.jsonl,pairs/}`
- `reference/captures/full-game/<level-slug>/<scenario>/<run-id>/` (lossless 320×240 frames → video, audio, frame↔tick index)
- `docs/full-game/runtime-launch.md`

## Tasks

- [x] `S1.01` (P3.ALL.03) Find the live global(s) holding the current level ID, substage/section, start index and difficulty. Start from the dispatch: TITLE 0x16300 → 0xc3d0 → EXE slot 0x64 → 0x4040/0x5cd0/0x5c20, ENG1 `LVL_LoadMap` 0x1f750 and ENG3 0x1a930 (game state +0x8c engine tag). Log them at every module/map load. Recover field offsets in Ghidra before interpreting any value.
- [x] `S1.02` (P3.ALL.03) Hook module load/unload (EXE 0x4040) and map load in **both** engines (ENG1 0x1f750/0x1f9e0, ENG3 0x1a930). Emit `level_load` events with program+RVA, normalized `O:\`→`M:\GRAFIX\MAPS\` path, BIN/CIF index and ID, and start index.
- [x] `S1.03` (P3.ALL.04) Scenario runner. It takes a scenario JSON from `reference/scenarios/<level>/` and produces a `<run-id>`.
  - Supported entry methods:
    - new game (Medium canonical)
    - password, using the S6-verified icons only
    - F3/level-select, if S6 verifies it
    - recorded input replay
  - An arbitrary level-ID memory write is **not** an entry method; it may only be used for debugging and must be labeled as such.
- [x] `S1.04` (P3.ALL.05) Record in `run.json`:
  - binary/archive SHA-256
  - proxy/ini hashes
  - entry method
  - difficulty
  - initial lives/health/power-ups
  - tracer env (`HERC_HOOK_*`)
  - frame↔tick origin
  - audio timestamp origin
  - whether CD audio is absent
- [x] `S1.05` (P3.ALL.05) Record polled pad bits per tick (`RT_PollJoystick` EXE 0x7c50, keyboard 0x7f30) to `input.jsonl`, and make replay deterministic for the same start state. Note any RNG source found (an S6 dependency).
- [x] `S1.06` (P3.ALL.06) Extend the hooks with checkpoint, spawn/despawn, sequence/frame and palette change events, plus draw/play consumers. The ENG1 hooks extend the existing 0x3cd0 frame hook. The ENG3 equivalents come from S3.
- [x] `S1.07` (P3.ALL.11) Reset `collect_pairs.py` caps at scene boundaries so later-level evidence isn't suppressed by the title-load caps. Pairs must record dimensions/stride/format/palette.
- [x] `S1.08` (P3.ALL.12) Write a coverage updater, `tools/trace/update_coverage.py`. It writes run paths and tick ranges into scenario JSON and `coverage.{md,json}`, and each dimension (capture/trace/assets/spec) stays separate.
- [x] `S1.09` Write `docs/full-game/runtime-launch.md`: how to run a scenario, entry methods, env flags, and where outputs land.

## Done when

- The harness runs `L01.title-demo-baseline` and one other L01 scenario end to end, reproducing the inherited demo90 load at tick ~1134. Run that check once; there is no test suite.
- A level load in each engine emits identity events: L01 (ENG1) and the L02 attract demo (ENG3).
- The coverage updater writes a row.

## Notes
- `S1.01` (2026-10-12): identity = game state (EXE 0x615c98): +0 level ID, +1 start index, +2 restart index, +0xc lives, +0xe difficulty, +0x12 password index, +0x8c map tag. No separate substage field (one section per ENG1 map; position = start index). → `docs/full-game/gameplay/difficulty.md`. Logging at load is S1.02.
- `S1.02` (2026-10-12): `hercules_hook.c` emits `module_load` (after EXE `MOD_LoadModule` 0x5c20) and `level_load`/`level_palette` (FS_Open of MAP_PSX.BIN/.CIF) with engine, loader caller, normalized path, archive index + ID and game-state identity. Runs: `analysis/traces/runs/s102` (L01 demo: ENG1 load tick 1133/1134, BIN 565 `5ED398E4`, CIF 564 `5ECB98E5`, difficulty 0) and `analysis/traces/runs/s102_attract` (L02 attract demo: ENG3 load tick 10536/10537, BIN 557 `57E5A3D6`). The loader field is the first DLL return address (the file helper, e.g. eng1.dll+0x2c3aa), not `LVL_LoadMap` itself.
- `S1.03`–`S1.05` (2026-10-12): `tools/trace/run_scenario.ps1` + `tools/trace/run_meta.py`.
  - Entry methods: `attract`, `new_game` (TITLE: CROSS 0x4000 on START GAME at tick 300, START 0x0008 past the level card at 420; real new game, start index 0), `replay` (input.jsonl → `HERC_HOOK_PAD`), `debug_level` (labelled DEBUG ONLY in run.json). Passwords wait on S6.03.
  - Difficulty: `HERC_HOOK_DIFFICULTY` sets config byte 0xb9e864 before the EXE applies it (0x4ac0 → `FUN_00401fd0` → gs+0xe), the variable the F2 "Game Play" dialog edits. The player's own options live in the per-user VirtualStore copy of HKLM `...\Hercules.00\Config` (this machine: difficulty 0, lives 4).
  - Incident: one preset run let the game save its options (EXE 0x4da0 copies gs+0xe back and calls RegSetValueEx) and changed the saved difficulty 0 → 1. The original value was restored and traced runs now never save (0x4da0 → ret unless `HERC_HOOK_KEEPCONFIG=1`).
  - `input.jsonl` = pad bits after `RT_PollInput` (~u16 at *(0xa8e78c)+2) per change. RNG: gs+0x48/+0x4c are reset to fixed seeds by `FUN_00401ff0`. A replay of `traversal/20261012-052007-new_game-medium` gave identical anim/state/level events (1,260 events) in `traversal/20261012-052101-replay-medium`.
  - Baseline: `title-demo-baseline/20261012-052154-attract-config` reproduces the demo90 load at tick 1133/1134.
- `S1.06`–`S1.09` (2026-10-12):
  - Events: `state` (identity bytes, health, config), `spawn`/`despawn` (ENG1 0x27db0/0x27260; the hero spawns at start 0, 1214/780), `palette` (palette hash + brightness), plus the existing `anim`/`sfx_play`. ENG3 equivalents: S3.
  - Pair caps reset at every module/level load; folders `s<scene>_esf_*`/`s<scene>_vram_*` with tick/scene/level/palette hash; `collect_pairs.py --out` verified 12/12 in `title-demo-baseline/20261012-053058-attract-config`.
  - `tools/trace/update_coverage.py` writes runs/trace intervals into the scenario JSON and coverage.{json,md} (trace PARTIAL for L01.traversal).
  - Doc `docs/full-game/runtime-launch.md`.
- Done-when check: baseline + `L01.traversal` (new_game and replay) end to end; identity events in ENG1 (L01) and ENG3 (L02 attract demo, `analysis/traces/runs/s102_attract`); coverage updater wrote rows. S1 done.

