# L03 The Centaur's Forest — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L03 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L03 The Centaur's Forest (Windows manual spelling) |
| internal ID / label | 3 / FOREST OF CENTAURS |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\FOREST\` |
| BIN | index 515, ID `49716801`, offset 0x11f8e70, 1,304,576 B, SHA-256 `78bc5a60ecce1c99e752fb30a16bc5ace4ee80f686365e1da8136a0decbd69cb` |
| CIF | index 516, ID `497267F9`, offset 0x2164232, 336,384 B, SHA-256 `db53596305532dde182b68879960e6e1f8f3440e5d9236237834b202c9326277` |
| ENG1 descriptor | 0x5a6d8 (table 0x5a690, 0x18 B); map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·3 (type table at +0x1c) |
| TITLE | descriptor 0x234d4, module pointer 0x22294; dispatch 0x16300 |
| EXE palette | 0x5760 selector 3, path string 0x3672c |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |
| related stage descriptor | ID 4 NESSUS BATTLE: ENG1 0x5a6f0, TITLE 0x234e0, `O:\NESSUS\`, no named map entries, runtime relationship UNKNOWN (`levels.json` → `extra_stage_descriptors`) |

## Scope

The plan covers the following, from first entry to the next-level load:
- the forest platforming route, including foreground and background paths, tree climbing, stone barriers (Power Punch) and cracks (ground slam)
- forest secrets and pickups
- regular enemies: thugs, vultures, centaurs and winged panthers (all LEAD)
- the **Nessus** boss encounter
- checkpoint/retry
- completion, tally, and the L04 THEBES load

The Nessus fight is an **L03 substage question**. ID 4 never becomes a public level.

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L03.TR.*` |
| [S2](../_shared/S2-eng1-map-completion.md) T28/fields/previews | `L03.AS.01`, `L03.GP.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode and encounter template | `L03.AS.03`, `L03.GP.03`–`05` |
| [S5](../_shared/S5-audio-music.md) audio binding (FOREST and NESSUS music segments) | `L03.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods | `L03.TR.01`, `L03.TR.07` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L03.RM.*` |
| L01 / lean plan | ENG1 hero spec (`docs/gameplay-spec.md`) is reused, not re-derived |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, actor handlers, Nessus code path and ID 4 use | P2, S2.05 | in progress (RE.01, RE.03, RE.04 done) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, complete traversal, Nessus transition, difficulty diffs | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | map/model/palette, actor and Nessus banks, audio, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | traversal, enemies, Nessus encounter, checkpoints, completion spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | art inventory, renderer coverage, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l03-the-centaurs-forest/`)

[full-progression](../../reference/scenarios/l03-the-centaurs-forest/full-progression.json) ·
[forest-main-route](../../reference/scenarios/l03-the-centaurs-forest/forest-main-route.json) ·
[background-paths](../../reference/scenarios/l03-the-centaurs-forest/background-paths.json) ·
[forest-secrets](../../reference/scenarios/l03-the-centaurs-forest/forest-secrets.json) ·
[nessus-approach](../../reference/scenarios/l03-the-centaurs-forest/nessus-approach.json) ·
[nessus-attack-damage](../../reference/scenarios/l03-the-centaurs-forest/nessus-attack-damage.json) ·
[nessus-defeat](../../reference/scenarios/l03-the-centaurs-forest/nessus-defeat.json) ·
[checkpoint-retry](../../reference/scenarios/l03-the-centaurs-forest/checkpoint-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l03-the-centaurs-forest/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l03-the-centaurs-forest/difficulty-herculean.json)

## Level acceptance criteria

1. Every L03 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The Nessus relationship is answered with evidence: whether ID 4 is entered, which map and banks load, and where the music switches. `levels.json` → `extra_stage_descriptors[0].runtime_relationship` is updated, and nothing is renumbered.
3. `docs/full-game/levels/L03.md` specifies the route, actors, Nessus phases, checkpoints and completion, with code and trace sources.
4. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
5. In remaster mode the level plays from entry to the L04 load with approved art. The user's side-by-side check against the original passes.

## Notes

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
