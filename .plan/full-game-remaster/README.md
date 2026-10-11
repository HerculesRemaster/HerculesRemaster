# Hercules reverse engineering and 4K remaster (full-game plan)

Goal: reverse-engineer Disney's Hercules (1997, Windows) with **ghidra-mcp**, trace full Medium progression and scenes across all ten levels, extract their assets and recover full gameplay, then rebuild a playable runtime and swap in 4K art.

Master document: `C:\Games\GameHercules\Hercules-Reverse-Engineering-Master-Document.md`.
Plan version **2026-10-12.2**. **Deferred for later implementation.** This separate plan lives at `C:\Games\GameHercules\Remaster\.plan\full-game-remaster`; the active lean plan remains unchanged at `../hercules-remaster/`. Preparing these documents does not start full-game runtime capture, decoding or implementation.

The P0–P8/P2F structure mirrors the lean plan and inherits its completed groundwork without resetting tasks. RE uses the existing bethington/ghidra-mcp GUI plugin/project `C:\Games\Ghidra\HerculesRevertEngineer`. Keep evidence in scenario metadata and trace/format outputs; no orchestration plan tool is required. Archived identities remain preserved in `../_archive/hercules-remaster-full-2026-10-11.7z` and [TASK-IDENTITIES.md](TASK-IDENTITIES.md).

Paths in phase task descriptions are relative to `Remaster/` unless prefixed with `plan:`. Plan-owned definitions, maps, sources, coverage and binary-analysis excerpts live under **this folder's `reference/`**. When implementation is explicitly started, keep full-game run outputs under `analysis/traces/full-game/`, media under `reference/captures/full-game/` and new coverage under `extracted/meta/full-game/`; link inherited outputs at their current paths. Do not overwrite lean captures/coverage or edit the lean plan to track this work.

## Files

| file | content |
|---|---|
| [CONTINUE.md](CONTINUE.md) | prompt for starting or resuming work |
| [FACTS.md](FACTS.md) | known measurements (hashes, PE, archive, tools) and open questions |
| `P0` … `P8`, `P2F` | one file per phase: goal, inputs, outputs, task checklist, notes |
| [TASK-IDENTITIES.md](TASK-IDENTITIES.md) | unchanged archived IDs, current-task aliases and completion preservation |
| [Level map](reference/level-map.md) / [registry](reference/levels.json) | all ten names, internal IDs, archive entries, engine/load routines and verification status |
| [Scenarios](reference/scenarios/README.md) / [coverage](reference/scenarios/coverage.md) | per-level/per-scene definitions, evidence status and stage discovery workflow |
| [Sources](reference/sources.md) | local binary/archive/runtime references and researched Windows manual |
| [START-HERE.md](START-HERE.md) | deferred implementation entry point and isolated output paths |
| [levels/](levels/README.md) | **ten per-level implementation plans** plus [shared dependencies](levels/_shared/README.md) |

## Per-level plans

The all-level work in P3–P5, together with its P6–P8 integration, is decomposed into ten level plans that can each be executed on their own. Engine-wide work sits in shared plans S1–S7. The phase files below remain the cross-level completion gates. Every level task names the `P3.ALL.*`, `P4.ALL.*` or `P5.ALL.*` task it slices, so existing IDs and completed milestones are unchanged. Order, dependencies and evidence labels are in [levels/README.md](levels/README.md).

| level | plan | engine | key open question |
|---|---|---|---|
| L01 Your Basic D.I.D. | [index](levels/L01-your-basic-did/README.md) (work lives in the lean plan) | ENG1 | full traversal beyond the slice |
| L02 Hero's Gauntlet | [plan](levels/L02-heros-gauntlet/README.md) | ENG3 | i1.a format, runner model (S3) |
| L03 The Centaur's Forest | [plan](levels/L03-the-centaurs-forest/README.md) | ENG1 | Nessus and the ID-4 relationship |
| L04 The Big Olive | [plan](levels/L04-the-big-olive/README.md) | ENG1 | Thebes Part 2 substage |
| L05 Hydra Canyon | [plan](levels/L05-hydra-canyon/README.md) | ENG1 | T28-model boss, VRAM overrun |
| L06 Medusa's Lair | [plan](levels/L06-medusas-lair/README.md) | ENG1 | reflection mechanic, VRAM overrun |
| L07 Cyclops Attack | [plan](levels/L07-cyclops-attack/README.md) | ENG3 | chase lanes and obstacles (S3) |
| L08 Titan Flight | [plan](levels/L08-titan-flight/README.md) | ENG1 | Pegasus flight mode, Beginner endpoint |
| L09 Passageways of Eternal Torment | [plan](levels/L09-passageways-of-eternal-torment/README.md) | ENG3 | passage hazards, Beginner lockout (S3) |
| L10 Vortex of Souls | [plan](levels/L10-vortex-of-souls/README.md) | ENG1 | final encounter (empty spawn list), ending |

Shared: [S1 runtime harness](levels/_shared/S1-runtime-harness.md) · [S2 ENG1 maps](levels/_shared/S2-eng1-map-completion.md) · [S3 ENG3 engine](levels/_shared/S3-eng3-engine.md) · [S4 actors/animation](levels/_shared/S4-actors-animation.md) · [S5 audio/music](levels/_shared/S5-audio-music.md) · [S6 progression/difficulty](levels/_shared/S6-progression-difficulty.md) · [S7 remaster integration](levels/_shared/S7-remaster-integration.md)

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [P0](P0-setup.md) | workspace, preserved originals, Ghidra project + ghidra-mcp (GUI plugin) | - | done |
| [P1](P1-archive.md) | HERCULES.FS extraction + resource catalog | P0 | done |
| [P2](P2-binary-map.md) | Ghidra: archive reader, ID hash, DLL contract, engine loop, DirectDraw | P0, P1 | done |
| [P2F](P2F-fullscreen.md) | **run the original fullscreen on 4K Win11 first**: own proxy `ddraw.dll`, borderless, 9× integer scale (2880×2160, pillarbox), `hercules_display.ini` | P2 | done |
| [P3](P3-runtime-trace.md) | all ten levels/scenes, linked Medium progression, difficulty comparisons, consumer/buffer traces | P0, P1, P2, P2F | planned/deferred; inherited title/demo milestone done |
| [P4](P4-assets.md) | all-level assets/animations/audio/effects/UI; scene associations and archive reconciliation | P1, P2, incremental P3 | planned/deferred; inherited audio/image/hero-animation milestone done |
| [P5](P5-levels-gameplay.md) | all-level layouts/collision/scripts; enemies/bosses/chase/flight/progression | P2, incremental P3/P4 | todo |
| [P6](P6-runtime-slice.md) | choose runtime architecture, build a playable slice with original assets | P4, P5 | todo |
| [P7](P7-hires.md) | 4K asset pipeline + approved pilot | P4, P5, P6 | todo |
| [P8](P8-expand-package.md) | all levels, 4K performance, packaging | P7 | todo |

P2F (fullscreen on 4K) comes before any other runtime or remaster work: the original must be playable fullscreen on the 3840×2160 monitor first. P1 and the Ghidra part of P0 can run in parallel. P4 audio and images can run in parallel once P1 and P2 are done.

Use **Medium** for full progression. Beginner stops after L08; L09/L10 require Medium or Herculean. Trace Beginner/Herculean differences where assets or behavior change, including the Beginner availability boundary. Internal IDs are **1, 2, 3, 5–11**; ID 4 is a separate Nessus descriptor whose actual runtime role still needs confirmation. Static mappings are verified against this archive/build; unplayed scenes remain planned.

P3–P5 now close on full-game coverage, rather than title/demo or one level. P6's slice and P7's art pilot remain implementation milestones using this complete reference set. P8 implements all ten recovered levels; discovery/extraction is no longer deferred to P8. Existing checked tasks stay checked when the broader phase gains work. Inherited P4 checklists and notes include the latest completed audio, image and hero-animation work at separation; later lean progress can be reused without resetting full-game task IDs.

## Progress tracking

- Tick `- [x]` in the phase file when a task is implemented and works.
- Add 1–3 lines to the phase's `## Notes` section: key findings (module + RVA), surprises, the next step.
- Update the `state` column above (todo / in progress / done / blocked: reason).
- Keep new task IDs append-only (`P3.ALL.*`, `P4.ALL.*`, `P5.ALL.*`); keep stable scenario IDs when refining routes or adding substages. Captured, traced, decoded and gameplay-specified are separate coverage states.
