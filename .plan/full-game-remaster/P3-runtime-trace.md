# P3 All-level runtime tracing and scene coverage

Plan status: **deferred for later implementation**. Registry/scenario documents live in this plan's `reference/`; new run/export/specification outputs use the isolated paths in [START-HERE.md](START-HERE.md). Existing lean tools/outputs below are inherited inputs.

Goal: trace the complete Windows game on **Medium**, through all ten levels and the ending. Associate scenes, resource loads, decoder buffers, animation, audio and gameplay transitions with reproducible scenarios. The completed title/demo work remains the first milestone.
Needs: P0, P1 (catalog), P2 (RVAs), **P2F first** (original playable fullscreen on the 3840×2160 monitor).
Outputs:
- `working/game/`, `tools/runtime/`, `tools/trace/`
- `analysis/traces/resource-usage.jsonl`, `analysis/traces/pairs/`
- `docs/resource-consumers.md`, `docs/runtime-launch.md`
- Plan-owned `reference/levels.json`, `reference/level-map.md`, `reference/sources.md`
- Plan-owned `reference/scenarios/<level>/<scenario>.json`, `reference/scenarios/coverage.{md,json}`
- Future `analysis/traces/full-game/<level>/<scenario>/<run-id>/{run.json,resource-usage.jsonl,input.jsonl,checkpoints.jsonl,pairs/}`
- Future `reference/captures/full-game/<level>/<scenario>/<run-id>/` (video/audio and frame-to-tick index)

## Existing tasks (title/demo milestone retained)

All existing checkboxes and notes retain their completion state. [Task identities](TASK-IDENTITIES.md) retain the equivalent archived IDs; additions below use `P3.ALL.*`. Existing files at legacy output paths remain valid evidence and are linked, not moved or overwritten.

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

## Verified level inventory first

The [level map](reference/level-map.md) and [registry](reference/levels.json) list all ten public titles, internal IDs, archive BIN/CIF resource IDs and engine/load routines. Public spellings are corroborated by the Windows manual; the canonical build's tables and archive support the internal mappings. Static verification and runtime verification are separate: the existing demo proves a TRAINING load, not a completed level.

| public level | internal ID | `M:\GRAFIX\MAPS\` directory | engine / map-loader RVA |
|---|---:|---|---|
| L01 Your Basic D.I.D. | 1 | `TRAINING` | ENG1 / 0x1f750 |
| L02 Hero's Gauntlet | 2 | `GAUNTLET` | ENG3 / 0x1a930 |
| L03 The Centaur's Forest | 3 | `FOREST` | ENG1 / 0x1f750 |
| L04 The Big Olive | 5 | `THEBES` | ENG1 / 0x1f750 |
| L05 Hydra Canyon | 6 | `HYDRA` | ENG1 / 0x1f750 |
| L06 Medusa's Lair | 7 | `MEDUSA` | ENG1 / 0x1f750 |
| L07 Cyclops Attack | 8 | `CYCLOPS` | ENG3 / 0x1a930 |
| L08 Titan Flight | 9 | `TITANS` | ENG1 / 0x1f750 |
| L09 Passageways of Eternal Torment | 10 | `TORMENT` | ENG3 / 0x1a930 |
| L10 Vortex of Souls | 11 | `VORTEX` | ENG1 / 0x1f750 |

Internal ID 4 is labeled `NESSUS BATTLE` and has an `O:\NESSUS\` descriptor, but this archive has no separately named Nessus map. Trace the forest/boss transition before claiming that ID is used or aliases another map. The Big Olive's reported second part is a substage discovery target under L04, not evidence that public level 4 uses internal ID 4.

## All-level tasks

- [x] `P3.ALL.01` Seed the ten-level registry from the Windows manual, EXE palette selector, TITLE engine dispatch and ENG1/ENG3 descriptors. Verify the archive hash, all twenty gameplay BIN/CIF names, their resource hashes and payload hashes. Preserve unknown stage relationships explicitly.
- [x] `P3.ALL.02` Create per-scene JSON definitions and a coverage matrix for all ten levels, including difficulty comparisons and unsupported Beginner entry checks. This completes **scenario design only**; new scene captures remain planned.
- [ ] `P3.ALL.03` Confirm actual level/substage/room/chunk IDs, difficulty encoding, module and load arguments at runtime. Resolve ID 4/Nessus, Thebes sections and checkpoint IDs; update evidence without changing existing identifiers.
- [ ] `P3.ALL.04` Extend `run_trace.ps1` with a scenario runner and run metadata. Medium new-game progression is canonical; verify password/F3 selection and checkpoint/input replay before using shortcuts for repeated scenes. Store accepted password icons/unlock prerequisites and initial state. An arbitrary level-ID memory write is not a verified entry method.
- [ ] `P3.ALL.05` Synchronize frame/audio capture, polled pad bits, simulation ticks and events. Record binary/archive hashes, proxy/config hashes, controls, difficulty, initial health/lives/power-ups, RNG state if recovered, entry method and tracer overrides in `run.json`. Preserve a frame-to-tick index and audio timestamp origin.
- [ ] `P3.ALL.06` Extend existing FS/OS/ESF/VRAM/sound/ENG1 animation hooks to module load/unload, level/substage transitions, checkpoints, spawn/despawn, sequences/frames in **both engines**, palette changes and draw/play consumers. Log normalized names, IDs/indexes, source ranges, buffers/ownership and program+RVA. Existing stack-scanned `dll` attribution is heuristic until checked against direct calls.
- [ ] `P3.ALL.07` Capture linked **Medium L01 → L10 → ending** progression. Retain load/start, completion/tally, next-level, death/retry, game-over and ending boundaries. Link consecutive runs to establish continuity; password-only samples do not establish progression.
- [ ] `P3.ALL.08` Capture every Medium scene in the matrix: background/optional paths, secrets, combat, rescues/destruction, boss phases, chase lanes, flight sections and final events. Refine provisional scene boundaries using video, state transitions and resource activity.
- [ ] `P3.ALL.09` Compare matched Beginner/Herculean runs for L01–L08 and Herculean runs for L09–L10. Compare load resources and representative mechanics first; add dedicated per-scene differential traces wherever resources, spawns, damage, timing, routes or phases differ. Equality is scoped to the route/state compared, never assumed from matching filenames.
- [ ] `P3.ALL.10` Verify Beginner's endpoint after L08 and normal-entry rejection/unavailability of L09/L10. Those matrix cells are planned availability checks until observed, then `unavailable_by_design`; they are never required Beginner gameplay captures.
- [ ] `P3.ALL.11` Build scenario → resource → decoder → consumer associations, including inherited/resident assets, cache hits, streamed chunks, dynamic names and unnamed entries. Reset/sample pair collection at scene boundaries so global title-load caps cannot suppress later-level evidence. Require complete input/output, palette/state, dimensions, stride and format for every consumed family/variant.
- [ ] `P3.ALL.12` Reconcile coverage from actual run outputs. Distinguish planned/referenced/captured/traced/verified, failed entry, partial capture and unavailable. Verified claims require existing source files and tick/event ranges; dropped events or incomplete boundaries remain gaps.
- [ ] `P3.ALL.13` Write `docs/full-game/resource-consumers.md`, extending the completed demo chains to every runtime-used family/variant and both engines. Write full-game launch instructions in `docs/full-game/runtime-launch.md`; update this plan's sources and coverage. Use the [discovery workflow](reference/scenarios/README.md) to append stages/scenes and reopen only affected cells.

## Scene groups and capture contract

Detailed actions and evidence status live in [scenario definitions](reference/scenarios/README.md) and the [coverage matrix](reference/scenarios/coverage.md).

| level | required capture/discovery groups |
|---|---|
| L01 | training areas; traversal/ledges/poles/objects; combat exercises; secrets; checkpoint/retry; completion |
| L02 | successive opening/middle/final gauntlet sections; hazards; damsel rescues; retry/completion |
| L03 | forest routes/background paths; secrets; Nessus approach, attack/damage and defeat; checkpoint/retry |
| L04 | streets/background stairs; enemies/Minotaur; harpy rescues; destruction/secrets; second-part discovery; retry/completion |
| L05 | canyon entry; Hydra attacks, damage/phase changes and defeat; death/retry |
| L06 | lair traversal; skeletons; gaze/projectiles; reflection/defeat; death/retry |
| L07 | chase sections; alternate lanes/stairs; columns/citizens/chariots; completion; death/retry |
| L08 | Pegasus flight sections; Ice/Wind/Lava/Rock Titan encounters; god rescues; Zeus/exit discovery; death/retry |
| L09 | passage sections; skeletons/lost souls; chariot/stalactites; River Styx falls; completion; death/retry |
| L10 | vortex entry/traversal; souls; final encounter/phase discovery; rescue/ending discovery; death/retry |

Each JSON specifies entry method/prerequisites, difficulty, route/actions, semantic checkpoints, expected asset categories, capture boundaries, evidence status and questions. These are capture instructions, not claims that unplayed mechanics, coordinates or phases were verified. Source-supported groups and inferred details are labeled separately.

Trace before entry/load; retain preceding resident-resource state; stop after the final event and transition/unload. Use scene-local intervals inside the complete run. Prefer lossless 320×240 pixels/palette with separate 4K presentation metadata. Distinguish missing CD audio from unobserved SFX. Keep the fullscreen proxy and document any windowed capture override; P2F remains completed and mandatory. The existing optional HUMAN sessions do not make all-level coverage optional; verified replay/automation can supply it.

Done when all ten levels have linked Medium completion/ending traces, every required scene has capture/trace evidence, differing difficulties have matched traces, Beginner exclusions are confirmed, and every runtime-used asset family/variant has reproducible consumer chains and complete decoder pairs. Open required scene/resource gaps block P3 completion. The original one-entry criterion remains a completed milestone.

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
