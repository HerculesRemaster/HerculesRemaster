# Running the original

Everything runs from `working/game/` (a copy of `backups/originals`, made by `tools/runtime/make_working_copy.ps1`). Run one game process at a time.

| script | what it does | output |
|---|---|---|
| `tools/runtime/make_working_copy.ps1 [-Force]` | copies the originals (without `ReadMe.txt`) to `working/game` | |
| `tools/runtime/launch_fullscreen.ps1` | installs the P2F proxy (`ddraw.dll` + `hercules_display.ini`), runs, captures, checks borderless 4K | `analysis/runtime/fullscreen/<stamp>/` |
| `tools/runtime/launch_original.ps1 [-ShotsAt 2,5,10,30]` | starts the game, logs window titles/dialogs and the exit code, captures the game window, closes it | `analysis/runtime/launch/<stamp>/` |
| `tools/trace/run_trace.ps1 -Seconds N [-Frames] [-Visible] [-Fullscreen] [-WithInput]` | injects `runtime/hooks/hercules_hook.dll`. The game runs windowed (proxy forced to `windowed`), minimized, kept active, with no keyboard input | `analysis/traces/runs/<name>/resource-usage.jsonl`, `pairs/`, `frames.bin` |
| `tools/runtime/capture_demo.ps1` | trace with `-Frames`, waits for `M:\DEMO\DEMO*.DAT`, encodes 60 s of the exact presented frames | `reference/captures/auto/demo-<stamp>.mkv/.json` |
| `tools/trace/map_reads.py <trace>` | OS reads of `hercules.fs` → archive entries, coverage check | `read-map.json` next to the trace |
| `tools/trace/collect_pairs.py <run>` | verified input/output pairs | `analysis/traces/pairs/<index>/` |

Screenshots use `PrintWindow` on the game window only, never the desktop.

## Behaviour that matters for automation

- The game starts on Win11 without changes. Without the proxy, its own fullscreen (`SetDisplayMode(320,240,8)`) gives a black screen; see `fullscreen.md`.
- It ticks only while it is the active app (`g_appActive` 0xba0238 ≥ 2); otherwise the title is "… - Waiting". The hook option `HERC_HOOK_ACTIVE=1` keeps it running in the background.
- Input comes from `GetAsyncKeyState`, which is global, so a background run would see your typing. `HERC_HOOK_NOINPUT=1` makes `RT_ReadPadBits` (0x8040) return 0.
- Attract mode: the title idles for about 40 s (tick ~1134), then ENG1 plays `DEMO1_1.DAT` on TRAINING for about 27 s, then the title starts again.
- Frame cadence (frame dump): 30.33 presents/s, one game tick per present (`ticks_per_present` 1: 1814 of 1819).
- A minimized start has once stalled right after the index read; `run_trace.ps1` retries once.

## Hook DLL options (environment)

`HERC_HOOK_DIR` (output, required), `HERC_HOOK_WINDOWED`, `HERC_HOOK_ACTIVE`, `HERC_HOOK_NOINPUT`, `HERC_HOOK_PAIRS` (image uploads to dump), `HERC_HOOK_FRAMES` (frame dump file). The injector honours `HERC_INJECT_SHOW=minimized`; the proxy honours `HERC_DISPLAY_MODE`. Events: `open`, `read`, `seek`, `close`, `os_open`, `os_seek`, `os_read`, `esf_decode`, `vram_upload`, `sfx_play`, `anim`, `hook_eng1`. Each carries `t` (ms) and `tick`.
