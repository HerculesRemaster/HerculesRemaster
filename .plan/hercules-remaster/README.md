# Hercules reverse engineering and 4K remaster (lean plan)

Goal: reverse-engineer Disney's Hercules (1997, Windows) with **ghidra-mcp** and decode its assets, then rebuild a playable runtime with original assets and swap in 4K art.

Master document: `C:\Games\GameHercules\Hercules-Reverse-Engineering-Master-Document.md`.
Plan version 2026-10-12.1 (lean, adds P2F fullscreen). RE uses the bethington/ghidra-mcp GUI plugin in Ghidra over the project `C:\Games\Ghidra\HerculesRevertEngineer`, not the headless server. Command output goes through **rtk** to save tokens (CONTINUE rule 11). This version keeps implementation and RE work only. It has no test suites, evidence files, verifiers or plan tool. The old full plan is archived in `../_archive/hercules-remaster-full-2026-10-11.7z`.

## Files

| file | content |
|---|---|
| [CONTINUE.md](CONTINUE.md) | prompt for starting or resuming work |
| [FACTS.md](FACTS.md) | known measurements (hashes, PE, archive, tools) and open questions |
| `P0` … `P8`, `P2F` | one file per phase: goal, inputs, outputs, task checklist, notes |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [P0](P0-setup.md) | workspace, preserved originals, Ghidra project + ghidra-mcp (GUI plugin) | - | done |
| [P1](P1-archive.md) | HERCULES.FS extraction + resource catalog | P0 | done |
| [P2](P2-binary-map.md) | Ghidra: archive reader, ID hash, DLL contract, engine loop, DirectDraw | P0, P1 | done |
| [P2F](P2F-fullscreen.md) | **run the original fullscreen on 4K Win11 first**: own proxy `ddraw.dll`, borderless, 9× integer scale (2880×2160, pillarbox), `hercules_display.ini` | P2 | done |
| [P3](P3-runtime-trace.md) | run the original game, trace resource use at runtime | P0, P2, P2F | done |
| [P4](P4-assets.md) | decode audio, images/palettes, animations | P1, P2 (anim: P3) | done |
| [P5](P5-levels-gameplay.md) | level format, timing, demo format, hero behaviour | P2, P3, P4 | done |
| [P6](P6-runtime-slice.md) | choose runtime architecture, build a playable slice with original assets | P4, P5 | done |
| [P7](P7-hires.md) | 4K asset pipeline + approved pilot | P4, P5, P6 | done (hero_run_2 regeneration open) |
| [P8](P8-expand-package.md) | all levels, 4K performance, packaging | P7 | in progress: [HUMAN] clean-VM install of 0.2.0 |
| W0–W7 | browser/WebAssembly port: [`W0 scope and decisions`](W0-web-scope.md), technical plan in [`docs/wasm/`](../../docs/wasm/README.md) | P6, P8 | W0 in progress: SDK done; local/LAN, music and saves awaiting confirmation |

P2F (fullscreen on 4K) comes before any other runtime or remaster work: the original must be playable fullscreen on the 3840×2160 monitor first. P1 and the Ghidra part of P0 can run in parallel. P4 audio and images can run in parallel once P1 and P2 are done.

## Progress tracking

- Tick `- [x]` in the phase file when a task is implemented and works.
- Add 1–3 lines to the phase's `## Notes` section: key findings (module + RVA), surprises, the next step.
- Update the `state` column above (todo / in progress / done / blocked: reason).
