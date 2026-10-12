# Full-game runtime launch (S1 harness)

Everything runs `working/game/HERCULES.EXE` with the tracer `runtime/hooks/bin/hercules_hook.dll` injected (`herc_inject.exe`), one game process at a time. Rebuild the hook with `pwsh -File runtime/hooks/build.ps1` and the display proxy with `runtime\display\build.bat` (copy `runtime/display/bin/ddraw.dll` to `working/game/`).

## Run a scenario

```
pwsh -File tools/trace/run_scenario.ps1 -Scenario L01.traversal -Entry new_game -Difficulty Medium -Seconds 120 [-Pad "600-900:0020"] [-Frames]
pwsh -File tools/trace/run_scenario.ps1 -Scenario L01.title-demo-baseline -Entry attract -Difficulty Config -Seconds 75
pwsh -File tools/trace/run_scenario.ps1 -Scenario L01.traversal -Entry replay -Replay <run>\input.jsonl -Difficulty Medium
.venv/Scripts/python.exe tools/trace/update_coverage.py            # attach all runs to scenarios/coverage
.venv/Scripts/python.exe tools/trace/collect_pairs.py <run> --out <run>/pairs_verified
```

Output: `analysis/traces/full-game/<level-slug>/<scenario-file>/<run-id>/` with `resource-usage.jsonl`, `input.jsonl`, `pairs/`, `run.json` and optionally `frames.bin`. `run-id` is `<yyyyMMdd-HHmmss>-<entry>-<difficulty>`.

## Entry methods

| method | how | status |
|---|---|---|
| `attract` | title idle → attract demos (TRAINING ×3, then GAUNTLET at about tick 10,536) | verified: L01 load at tick 1133/1134 |
| `new_game` | pad CROSS (0x4000) on START GAME at tick 300, START (0x0008) past the level card at 420 → TRAINING, start index 0 | verified: load at tick 460/461 |
| `replay` | the pad bits of a recorded `input.jsonl`, through the scripted pad | verified deterministic (identical anim/state/level events) |
| password | icons from `analysis/exports/full-game/password_table.json` after S6.03 | pending S6.03 |
| `debug_level` | rewrites the next ENGINE1/ENGINE3 request to `-DebugLevel` (game state +0 = level, +1/+2 = 0). **Not an entry method**: debugging only, labelled `debug_only` in run.json, never upgrades coverage | available |

Pad bits are PSX bits (EXE keyboard map 0x7f30): UP 0x10, RIGHT 0x20, DOWN 0x40, LEFT 0x80, START 0x08, CROSS 0x4000 (Z, jump/confirm), CIRCLE 0x2000 (C), SQUARE 0x8000 (X, sword), TRIANGLE 0x1000 (D), R1 0x800 (S).

## Difficulty

`-Difficulty Beginner|Medium|Herculean` sets the "Game Play" option byte (EXE config 0xb9e864) just before the EXE applies it to game state +0xe (EXE 0x4ac0). This is the same variable the F2 dialog edits.

`Config` keeps the player's own saved option. On this machine that is the per-user VirtualStore copy of `HKLM\SOFTWARE\WOW6432Node\Disney Interactive\Hercules\1.00\Config`: byte 0x0c = difficulty, 0x18 = lives.

Traced runs never save options: EXE 0x4da0 → `ret`; set `HERC_HOOK_KEEPCONFIG=1` to allow saving. Demos force difficulty 0 while they play (`HERO_DemoLoad`, demo byte 4) and restore it from +0xf afterwards (ENG1 `FUN_1001f220`).

## Tracer environment (set by the scripts)

| variable | meaning |
|---|---|
| `HERC_HOOK_DIR` | output folder |
| `HERC_HOOK_WINDOWED`, `HERC_HOOK_ACTIVE`, `HERC_HOOK_NOINPUT` | windowed, keep running unfocused, ignore the real keyboard |
| `HERC_HOOK_PAD` | scripted pad `from-to:hexbits,...` (replaces `RT_ReadPadBits` 0x8040) |
| `HERC_HOOK_DIFFICULTY`, `HERC_HOOK_KEEPCONFIG` | options preset; allow saving options |
| `HERC_HOOK_LEVEL` | debug level rewrite |
| `HERC_HOOK_PAIRS` | VRAM upload pairs per scene (ESF: 4 per scene); caps reset at every module/level load |
| `HERC_HOOK_FRAMES` | frame dump with tick per frame (frame↔tick index; `-Frames`) |
| `HERC_HOOK_HERO`, `HERC_HOOK_VRAM_TICKS` | per-tick hero log; VRAM dumps |
| `HERC_RENDERER=gpu` | GPU renderer; the window is then shown (it needs a real client area) |

## Events in `resource-usage.jsonl`

These are in addition to the inherited file, VRAM, ESF and sound events:

| event | when | fields |
|---|---|---|
| `module_request` / `module_load` | engine switch (EXE 0x5cd0 / 0x5c20) | name, module, level, difficulty |
| `level_load` / `level_palette` | FS_Open of MAP_PSX.BIN/.CIF | engine, loader caller, normalized path (`O:\` → `M:\GRAFIX\MAPS\`), archive index + ID, level, start, restart, lives, difficulty, password |
| `state` | any identity byte changes (per tick) | level, start, restart, mode (+3), lives, difficulty, saved difficulty (+0xf), password, health, config bytes |
| `spawn` / `despawn` | ENG1 object create 0x27db0 / free 0x27260 | object, kind, position |
| `anim` | ENG1 sprite frame change (0x3cd0) | object, bank, frame, position |
| `palette` | palette hash or frame brightness changed | hash, brightness |
| `sfx_play`, `esf_decode` | sound start, ESF decode | |

`input.jsonl` holds the pad bits after `RT_PollInput` (~u16 at `*(0xa8e78c)+2`), one line per change.

`run.json` holds:
- binary/archive and tracer hashes;
- entry method and difficulty (requested and observed);
- identity events, initial state and tick range;
- the frame↔tick index (with `-Frames`), the audio origin and the CD-audio note (music is CD track 2 and absent).

ENG3 spawn/frame hooks come with S3.
