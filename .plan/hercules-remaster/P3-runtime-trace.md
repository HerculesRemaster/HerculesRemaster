# P3 Run the original + runtime trace

Goal: launch the original from a working copy and observe which resources it loads, which code consumes them, and the decoded buffers.
Needs: P0, P2 (RVAs to hook), P2F (working copy, launcher, fullscreen proxy).
Outputs:
- `working/game/`, `tools/runtime/`, `tools/trace/`
- `analysis/traces/resource-usage.jsonl`, `analysis/traces/pairs/`
- `docs/resource-consumers.md`, `docs/runtime-launch.md`

## Tasks

- [x] (done in P2F) Write `tools/runtime/make_working_copy.ps1`, which copies `backups/originals` to `working/game`.
- [x] (done in P2F) Write `tools/runtime/launch_original.ps1`. It starts `working/game/HERCULES.EXE` from its folder, records window titles, dialogs and the exit code, takes screenshots at 2/5/10/30 s, then closes the game.
- [x] Write `tools/runtime/capture_demo.ps1`. It idles at the title until demo mode starts, records 60 s with ffmpeg gdigrab to `reference/captures/auto/`, and notes the display mode and frame cadence.
- [x] If the game doesn't run on Win11, apply one compatibility change at a time to `working/game` only (the P2F proxy `ddraw.dll` is the first one). Traces run with the proxy present unless it gets in the way; note which in the notes. **[HUMAN]** The user must approve any DirectDraw wrapper download.
- [x] Choose the tracer. Try the x64dbg `headless.exe` script route first: break on `CreateFileA`, log, continue. If it can't run unattended, build an x86 hook DLL with MSVC under `runtime/hooks/`.
- [x] Trace `CreateFileA`/`SetFilePointer`/`ReadFile` on `hercules.fs` with the caller's RVA. Map the read offsets to archive entries with `tools/trace/map_reads.py`.
- [x] Hook the `FS_*` lookup and log the requested ID, the caller RVA and the destination buffer for the title and the demo to `resource-usage.jsonl`.
- [x] Capture input/output pairs for at least 1 image and 1 ESF sound to `analysis/traces/pairs/<index>/`: the payload, plus the decoder's output buffer with its dimensions, stride and format.
- [x] Log one character's consecutive frame requests and one sound-play event during the demo.
- [x] Write one entry end to end in `docs/resource-consumers.md`: entry → lookup RVA → decoder RVA → consumer buffer.
- [ ] Optional **[HUMAN]**:
  - an elevated Procmon startup trace
  - a play-check (visuals, sound, input)
  - a few recorded play sessions with an input log, in `reference/scenarios/`

Token budget: launch and trace scripts write logs to files and print a summary. Read them with `rtk log <file>` (deduplicated), `rtk read --tail-lines N` or `rtk grep`, and run scripts as `rtk summary pwsh -File …`.

Done when one entry is traced from lookup to consumer and the image/sound pairs are captured.

## Notes

- The game runs on Win11 unchanged (Q-09 yes). Its own fullscreen is broken; P2F fixes that with the proxy. Traces so far ran **without** the proxy, under the hook DLL's own windowed/active options.
- The game only ticks while it is the active app (`g_appActive` ≥ 2). While active it requests fullscreen. The tracer handles both (`HERC_HOOK_WINDOWED`, `HERC_HOOK_ACTIVE`, `HERC_HOOK_NOINPUT`).
- Tracer: x64dbg `headless.exe` works unattended (`bplog kernel32:CreateFileA` logged `"hercules.fs"`). For buffers and FS-level hooks, the chosen tool is our injected `runtime/hooks/hercules_hook.dll` (`herc_inject.exe`, `build.ps1`; `tools/trace/run_trace.ps1`).
- 90 s demo trace (`analysis/traces/runs/demo90`): TITLE (menu, `t0xx.TGA` slideshow), then ENG1 at tick 1134 loads TRAINING + `M:\DEMO\DEMO1_1.DAT` (demo), back to TITLE at tick 2016. 278 opens, 182 ESF decodes, 60 `sfx_play`, 5403 VRAM uploads.
- Pairs: ESF (`AUD_DecodeAdpcm` 0x2410 in/out) and TGA screens uploaded raw from file offset 786 (`MENUb.TGA`, 160×256 units = 320×256 8bpp). ESF name format is `pc_%ld.esf`.
- capture_demo: screen grabbing was replaced by an in-process frame dump (`HERC_HOOK_FRAMES`, StretchDIBits source frames) encoded with ffmpeg (`frames_to_video.py`). The capture shows the game's exact 320×240 frames and nothing from the desktop: 30.33 presents/s, 1 tick per present. Traces now run with the P2F proxy installed, forced to `windowed` and started minimized.
- map_reads: 950 OS reads → 253 entries; every FS_Read range is covered (CRT ReadFile EXE+0x13FBC).
- Pairs (`analysis/traces/pairs/`): 4 ESF match an independent decoder bit for bit (ESF format confirmed); 3 images (0584/0545 TGA pixels from offset 786, 0466 raw).
- Frame requests: ENG1 0x3cd0 hook; Hercules bank 1 (593 frames) sequences logged per tick. `ANIMPSX.BIN` = `i1.4` + u16 count + u32 `offset<<8|flags` table (offsets from file+4). → `docs/resource-consumers.md`, `docs/runtime-launch.md`.
- Q-07 (part): TRAINING (demo) runs on ENG1.

- Optional [HUMAN] item skipped (not needed for P4/P5).
