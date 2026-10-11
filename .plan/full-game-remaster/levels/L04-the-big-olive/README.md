# L04 The Big Olive — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L04 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L04 The Big Olive (Windows manual spelling) |
| internal ID / label | 5 / VISIT TO THEBES |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\THEBES\` (descriptor path `O:\THEBES\`) |
| BIN | index 513, ID `496E5AF9`, offset 0x1623c98, 1,214,464 B, SHA-256 `f0ac0524dd1c84b6fcb74e320b6d61eba618d4bc6a4fafd7b49dd63d55da9f93` |
| CIF | index 514, ID `496F5AF1`, offset 0x22aca32, 336,384 B, SHA-256 `79a39f2e11b2411d504a3fac2e86a63663bd614f3128d8673a33a69fbb08a095` |
| ENG1 descriptor | 0x5a708 (table 0x5a690, 0x18 B), header size 0xfc800; map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·5 (type table at +0x1c) |
| TITLE | descriptor 0x234ec, module pointer 0x2229c; dispatch 0x16300 |
| EXE palette | 0x5760 selector 5, path string 0x36714 |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |
| reported second part | **no** separate descriptor or named map. The BIN decodes as one section. The walkthrough lists Part 1 and Part 2 passwords, and ENG1 references two demos, `DEMO5_1` (start index 6) and `DEMO5_2` (start index 2). All of this is LEAD (see [FACTS](FACTS.md)) |

## Scope

The plan covers the following, from first entry to the next-level load:
- the Thebes street route, including the background stairs and depth routes
- regular enemies: thugs, skeletons and the Minotaur (all LEAD)
- harpy and citizen rescue events
- destruction, falling debris and secrets
- the **Thebes substage question**: what the walkthrough's "Part 1 / Part 2" corresponds to in this build
- checkpoint/retry
- completion, tally, and the L05 HYDRA load

The second part is an **L04 substage question**. It never becomes a public level, it never takes internal ID 4 (that is NESSUS, handled in [L03](../L03-the-centaurs-forest/README.md)), and no existing ID is renumbered.

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events (level ID, substage/section, start index) | every `L04.TR.*`, especially `L04.TR.05` |
| [S2](../_shared/S2-eng1-map-completion.md) T28/fields/previews, config export `S2.05` | `L04.RE.01`, `L04.AS.01`, `L04.GP.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode, bank → level index `S4.01`, encounter template | `L04.AS.03`–`04`, `L04.GP.03` |
| [S5](../_shared/S5-audio-music.md) audio binding (THEBES music segment, `q:\HERC_THE\`) | `L04.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, password substage encoding `S6.02`, verified entry `S6.03`, checkpoints `S6.04` | `L04.TR.01`, `L04.TR.05`–`07`, `L04.GP.05` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L04.RM.*` |
| L01 / lean plan | ENG1 hero spec (`docs/gameplay-spec.md`) is reused, not re-derived |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, start-point table, actor handlers, rescue/destruction scripts, substage code path | P2, S2.05 | todo |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, demo replays, complete traversal, substage boundary, difficulty diffs | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | map/palette, Thebian/enemy banks, effects, audio, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | streets/stairs, enemies, rescues, destruction, substage, checkpoints, completion spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | art inventory, renderer coverage, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l04-the-big-olive/`)

[full-progression](../../reference/scenarios/l04-the-big-olive/full-progression.json) ·
[streets](../../reference/scenarios/l04-the-big-olive/streets.json) ·
[background-stairs](../../reference/scenarios/l04-the-big-olive/background-stairs.json) ·
[enemy-encounters](../../reference/scenarios/l04-the-big-olive/enemy-encounters.json) ·
[rescue-events](../../reference/scenarios/l04-the-big-olive/rescue-events.json) ·
[destruction-secrets](../../reference/scenarios/l04-the-big-olive/destruction-secrets.json) ·
[section-transition](../../reference/scenarios/l04-the-big-olive/section-transition.json) ·
[checkpoint-completion](../../reference/scenarios/l04-the-big-olive/checkpoint-completion.json) ·
[difficulty-beginner](../../reference/scenarios/l04-the-big-olive/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l04-the-big-olive/difficulty-herculean.json)

## Level acceptance criteria

1. Every L04 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The Thebes substage question is answered with evidence: whether "Part 2" is a start index in the one THEBES map, a reload of ID 5, a separate load, or not observable. The result is recorded in `section-transition.json` with run path and ticks. Any newly found substage is appended through the discovery workflow, attached to L04. Nothing is renumbered and no eleventh level is created.
3. `docs/full-game/levels/L04.md` specifies the route, stairs/depth topology, actors, rescues, destruction, substage structure, checkpoints and completion, with code and trace sources.
4. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
5. In remaster mode the level plays from entry to the L05 load with approved art. The user's side-by-side check against the original passes.

## Notes
