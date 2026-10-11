# L02 Hero's Gauntlet — level implementation plan

Plan status: **deferred**, and **blocked by [S3](../_shared/S3-eng3-engine.md)**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L02 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

L02 is the first ENG3 level. ENG3 has no decoded map yet: `tools/level/level_decode.py` raises on every tag other than `i2.5`, and the `i1.a` decoder is an S3 output (`S3.03`, `S3.04`). Engine-wide ENG3 work (tick, loader, entity/hero/camera code, the runner model, hooks) stays in S3. This plan covers only what is specific to GAUNTLET.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L02 Hero's Gauntlet (Windows manual spelling) |
| internal ID / label | 2 / TRAINING GAUNTLET |
| engine / family | ENG3.DLL / `i1.a` (BIN magic `69312e61`) |
| archive directory | `M:\GRAFIX\MAPS\GAUNTLET\` |
| BIN | index 557, ID `57E5A3D6`, offset 0x1337670, 1,220,136 B, SHA-256 `95e4ca0052a92b174748489a6dcfcd7a76e351c54a3f9af1535e9454550eaa5c` |
| CIF | index 556, ID `57DDA3D7`, offset 0x21b6432, 336,384 B, SHA-256 `60c1440fd666ad5a9cedf3425842643ed6810d2c825abbd22c4a7b05a6455e5f` |
| ENG3 descriptor | 0x38558 (table 0x38528, 0x18 B); path pointer 0x10038660 → `O:\GAUNTLET\`; field +4 = 0x129e28 |
| map load | ENG3 0x1a930; no stream-chunk routine recorded (`stream_chunk: null`) |
| per-level config | UNRESOLVED (no ENG3 counterpart of ENG1 0x68af0 is known yet; `S3.02`) |
| TITLE | descriptor 0x234c8, module pointer 0x22290; dispatch 0x16300 |
| EXE palette | 0x5760 selector 2, path string 0x36684 |
| module switch | EXE 0x4040 (L01 ENG1 → L02 ENG3 → L03 ENG1) |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |

## Scope

The plan covers the following, from first entry to the next-level load:
- the behind-the-hero gauntlet run: forced forward motion, speed control and jumping, as GAUNTLET uses the shared S3 runner model
- the crossroads and the branch data behind them (two crossroads per the walkthrough, a LEAD)
- the hazard set: rotating and swinging blades, hammers, stone punches/hands, boulders, rolling stones, falling pillars, ponds with jump platforms (all LEAD)
- oncoming enemies and dummy enemies (LEAD)
- damsel rescue events
- pickups, HERCULES letters, power-ups (Hermes Sandals, Helmet of Invincibility; LEAD), Hermes level markers and checkpoint/retry
- the DEMO2_1 replay check
- the first ENG1 → ENG3 module switch (from L01), completion, tally, and the ENG3 → ENG1 switch to the L03 FOREST load

L02 is where the level-specific use of the S3 runner model is first established. L07 and L09 build on its findings.

## Dependencies

| needs | for |
|---|---|
| [S3](../_shared/S3-eng3-engine.md) `S3.01`–`S3.03` named ENG3 skeleton, descriptor use, i1.a struct | every `L02.RE.*` |
| S3 `S3.04`/`S3.05` `eng3_decode.py` and ENG3 preview | `L02.AS.01`, `L02.GP.01` |
| S3 `S3.06` ENG3 hooks | every `L02.TR.*` |
| S3 `S3.07` runner model (`docs/full-game/gameplay/chase.md`) | `L02.GP.02` |
| S3 `S3.08` DEMO2_1 decode | `L02.TR.02` |
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L02.TR.*` |
| [S4](../_shared/S4-actors-animation.md) SRC3 bank decode and encounter template | `L02.AS.03`, `L02.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (GAUNTLET music segment, ENG3 sound calls) | `L02.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods, transitions | `L02.TR.01`, `L02.TR.08`, `L02.TR.09`, `L02.RE.06` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L02.RM.*` |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | descriptor/config for ID 2, obstacle and actor handlers, crossroads branch data, pickups/rescues, completion | P2, S3.01–S3.03 | todo (blocked by S3) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, DEMO2_1 replay, both branches, difficulty diffs, L01→L02→L03 links | S1, S3.06, S3.08, S6.01–03 | todo (blocked by S3) |
| [3 Assets](3-assets.md) | i1.a map/palette, SRC3 banks, audio, scene coverage | S3.04–05, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | runner use, branches, hazards, enemies, rescues, checkpoints, completion spec | 1, 2, 3, S3.07 | todo |
| [5 Remaster](5-remaster.md) | renderer coverage, art inventory, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l02-heros-gauntlet/`)

[full-progression](../../reference/scenarios/l02-heros-gauntlet/full-progression.json) ·
[gauntlet-opening](../../reference/scenarios/l02-heros-gauntlet/gauntlet-opening.json) ·
[gauntlet-middle](../../reference/scenarios/l02-heros-gauntlet/gauntlet-middle.json) ·
[gauntlet-final](../../reference/scenarios/l02-heros-gauntlet/gauntlet-final.json) ·
[hazards](../../reference/scenarios/l02-heros-gauntlet/hazards.json) ·
[rescue-events](../../reference/scenarios/l02-heros-gauntlet/rescue-events.json) ·
[checkpoint-retry](../../reference/scenarios/l02-heros-gauntlet/checkpoint-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l02-heros-gauntlet/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l02-heros-gauntlet/difficulty-herculean.json)

## Level acceptance criteria

1. Every L02 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The GAUNTLET map decodes through S3's `eng3_decode.py`, with explained or explicitly unknown fields, and the crossroads branch data is identified in it, with both branches of every crossroad traced.
3. DEMO2_1 is replayed under trace, and its level ID, start index and map opens are recorded (or the mismatch is recorded).
4. `docs/full-game/levels/L02.md` specifies the GAUNTLET use of the runner model, branches, hazards, enemies, rescues, checkpoints and completion, with code and trace sources, and defers shared runner rules to `chase.md`.
5. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
6. The L01 → L02 (ENG1 → ENG3) and L02 → L03 (ENG3 → ENG1) transitions are captured as links in the S6 linked run.
7. In remaster mode the level plays from entry to the L03 load with approved art. The user's side-by-side check against the original passes.

## Notes
