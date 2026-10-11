# Start here

This is a complete **deferred full-game plan**, separate from the active [lean plan](../hercules-remaster/README.md). It is ready for later implementation; this documentation task launches no gameplay sessions and starts no remaster work.

Read [README.md](README.md), [CONTINUE.md](CONTINUE.md), [FACTS.md](FACTS.md), [P3](P3-runtime-trace.md), [P4](P4-assets.md), [P5](P5-levels-gameplay.md), the [level map](reference/level-map.md), [sources](reference/sources.md) and [coverage matrix](reference/scenarios/coverage.md). P0/P1/P2/P2F and completed title/demo, audio, image and hero-animation work are inherited prerequisites. Check their actual outputs/hash identities when starting; do not reset or repeat successful work merely because this plan has a new folder.

| location                                                    | purpose                                                                                         |
| ----------------------------------------------------------- | ----------------------------------------------------------------------------------------------- |
| `.plan/hercules-remaster/`                                  | active lean plan; keep its documents and progress unchanged                                     |
| `.plan/full-game-remaster/`                                 | full-game phase documents, inherited milestones, new stable task IDs                            |
| `.plan/full-game-remaster/reference/`                       | plan-owned level registry, source references, binary excerpts and scenario definitions/coverage |
| `analysis/traces/full-game/<level>/<scenario>/<run-id>/`    | future trace events, run metadata, inputs, checkpoints and decoder pairs                        |
| `reference/captures/full-game/<level>/<scenario>/<run-id>/` | future video/audio/frame index                                                                  |
| `extracted/meta/full-game/`                                 | future full-game scene/level/decoder coverage and asset associations                            |
| `extracted/full-game/{images,sprites,levels}/`              | future full-game exports; reuse already decoded shared audio by identity                        |
| `docs/full-game/`                                           | future level/gameplay/consumer/gap specifications                                               |

Paths above are relative to `C:\Games\GameHercules\Remaster`. Phase descriptions may name existing shared tools and inherited output files; these are inputs. New full-game data/specifications use the dedicated paths above, with explicit CLI output parameters. Any shared-tool extension must preserve its lean defaults and existing outputs.

Before the first new run, add ignore patterns for large full-game trace/media/export outputs while keeping this plan's definitions and coverage tracked. Preserve existing ignore rules and lean tool defaults.

Start later with `P3.ALL.03`: verify runtime level/substage/difficulty identities, then build the scenario runner. Use **Medium** for linked L01–L10 progression. Beginner comparisons cover L01–L08 plus negative availability checks for L09/L10; Herculean comparisons cover all ten levels. Keep the fullscreen-4K P2F prerequisite (3840×2160 borderless, 9× 2880×2160 image, 480 px side bars).

**Per-level execution.** Work is picked from [levels/README.md](levels/README.md).
1. Start with shared S1 (runtime harness) and S6.01–S6.03 (difficulty and entry methods).
2. Then follow the ENG1 track (S2 + S4 → L03 → L04 → L05 → L06 → L08 → L10) or the ENG3 track (S3 → L02 → L07 → L09). The two tracks can run in parallel.
3. Within a level, take the first unticked task in its phase files. Level outputs go to the isolated paths below, using `<level-slug>` from the scenario folder names. Level specifications go to `docs/full-game/levels/<Lxx>.md`.

L01 work stays in the lean plan, and [levels/L01-your-basic-did](levels/L01-your-basic-did/README.md) only indexes it.

Document updates keep existing task/scenario IDs stable. Search/refine/add scenes with the workflow in [reference/scenarios/README.md](reference/scenarios/README.md). A definition is planning evidence; a captured/traced/decoded/specification-complete state requires actual artifacts. Runtime capture, extraction and gameplay implementation remain pending until this plan is started.
