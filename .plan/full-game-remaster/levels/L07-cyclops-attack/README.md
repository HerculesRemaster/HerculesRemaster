# L07 Cyclops Attack — level implementation plan

Plan status: **deferred**, and **blocked by [S3](../_shared/S3-eng3-engine.md)**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L07 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

L07 is the second ENG3 level. ENG3 has no decoded map yet: `tools/level/level_decode.py` raises on every tag other than `i2.5`, and the `i1.a` decoder is an S3 output (`S3.03`, `S3.04`). Engine-wide ENG3 work stays in S3, and the GAUNTLET-specific runner findings come from [L02](../L02-heros-gauntlet/README.md). This plan covers only what is specific to CYCLOPS.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L07 Cyclops Attack (Windows manual spelling) |
| internal ID / label | 8 / CYCLOPS CHASE |
| engine / family | ENG3.DLL / `i1.a` (BIN magic `69312e61`) |
| archive directory | `M:\GRAFIX\MAPS\CYCLOPS\` |
| BIN | index 491, ID `4482B8EF`, offset 0x10d3cec, 1,200,516 B, SHA-256 `523372a1d74701252677cb8afa879f5a126de7cd7b95d455d88a3c17537b2450` |
| CIF | index 494, ID `4582B0EF`, offset 0x2112032, 336,384 B, SHA-256 `5d5af9095f66dce6354eab54af619ea4dffa420b6b2902f4829194e3441d1c17` |
| ENG3 descriptor | 0x385e8 (table 0x38528, 0x18 B); path pointer 0x10038654 → `O:\CYCLOPS\`; field +4 = 0x125184 |
| map load | ENG3 0x1a930; no stream-chunk routine recorded (`stream_chunk: null`) |
| per-level config | UNRESOLVED (`S3.02`) |
| TITLE | descriptor 0x23510, module pointer 0x222a8; dispatch 0x16300 |
| EXE palette | 0x5760 selector 8, path string 0x3666c |
| module switch | EXE 0x4040 (L06 ENG1 → L07 ENG3 → L08 ENG1) |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |

`analysis/exports/ENG3.DLL/strings.json` does not list `O:\CYCLOPS\`; the descriptor references it. Re-running the string export is `S3.02`.

## Scope

The plan covers the following, from first entry to the next-level load:
- the Cyclops chase through the city, as CYCLOPS uses the S3 runner model and the L02 findings
- stairs and alternate lanes, with their returns to the street
- the obstacle and cross-traffic schedule: columns, citizens and chariots (scenario design, LEAD)
- the Cyclops actor during the chase, and the end sequence (CYEND bank name, LEAD)
- coins and pickups along the lanes
- death/retry and checkpoints
- the L06 → L07 (ENG1 → ENG3) entry, completion, tally, and the L07 → L08 (ENG3 → ENG1) TITANS load

## Dependencies

| needs | for |
|---|---|
| [S3](../_shared/S3-eng3-engine.md) `S3.01`–`S3.03` named ENG3 skeleton, descriptor use, i1.a struct | every `L07.RE.*` |
| S3 `S3.04`/`S3.05` `eng3_decode.py` and ENG3 preview | `L07.AS.01`, `L07.GP.01` |
| S3 `S3.06` ENG3 hooks | every `L07.TR.*` |
| S3 `S3.07` runner model (`docs/full-game/gameplay/chase.md`) | `L07.GP.02` |
| [L02](../L02-heros-gauntlet/README.md) `L02.RE.02`–`RE.03`, `L02.GP.02` (handler names, branch data, runner use) | `L07.RE.02`–`RE.03`, `L07.GP.02` |
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L07.TR.*` |
| [S4](../_shared/S4-actors-animation.md) SRC3 bank decode and encounter template | `L07.AS.03`–`04`, `L07.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (CYCLOPS music segment, ENG3 sound calls) | `L07.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods, transitions | `L07.TR.01`, `L07.TR.07`–`TR.09`, `L07.RE.06` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L07.RM.*` |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | descriptor/config for ID 8, obstacle and traffic handlers, lane/stairs data, Cyclops actor, CYEND end sequence, completion | P2, S3.01–S3.03, L02 phase 1 | todo (blocked by S3) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, every lane, Cyclops end, difficulty diffs, L06→L07→L08 links | S1, S3.06, S6.01–03 | todo (blocked by S3) |
| [3 Assets](3-assets.md) | i1.a map/palette, SRC3 banks (Cyclops, CYEND, people), audio, scene coverage | S3.04–05, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | lanes, obstacle schedule, Cyclops encounter, checkpoints, completion spec | 1, 2, 3, S3.07 | todo |
| [5 Remaster](5-remaster.md) | renderer coverage, art inventory, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l07-cyclops-attack/`)

[full-progression](../../reference/scenarios/l07-cyclops-attack/full-progression.json) ·
[chase-opening](../../reference/scenarios/l07-cyclops-attack/chase-opening.json) ·
[chase-middle](../../reference/scenarios/l07-cyclops-attack/chase-middle.json) ·
[alternate-lanes](../../reference/scenarios/l07-cyclops-attack/alternate-lanes.json) ·
[obstacles](../../reference/scenarios/l07-cyclops-attack/obstacles.json) ·
[chase-completion](../../reference/scenarios/l07-cyclops-attack/chase-completion.json) ·
[death-retry](../../reference/scenarios/l07-cyclops-attack/death-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l07-cyclops-attack/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l07-cyclops-attack/difficulty-herculean.json)

## Level acceptance criteria

1. Every L07 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The CYCLOPS map decodes through S3's `eng3_decode.py`, and its lane/stairs data and obstacle schedule are identified, with every lane traced.
3. The Cyclops actor and the end sequence are specified with evidence: what drives the Cyclops during the chase, what ends the level, and which banks load (CYCLOPS/CYEND attribution confirmed or rejected).
4. `docs/full-game/levels/L07.md` specifies the CYCLOPS use of the runner model, lanes, obstacles, the Cyclops, checkpoints and completion, with code and trace sources, and defers shared rules to `chase.md` and L02.
5. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
6. The L06 → L07 (ENG1 → ENG3) and L07 → L08 (ENG3 → ENG1) transitions are captured as links in the S6 linked run.
7. In remaster mode the level plays from entry to the L08 load with approved art. The user's side-by-side check against the original passes.

## Notes
