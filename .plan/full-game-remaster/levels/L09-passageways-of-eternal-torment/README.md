# L09 Passageways of Eternal Torment — level implementation plan

Plan status: **deferred**, and **blocked by [S3](../_shared/S3-eng3-engine.md)**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L09 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

L09 is the third and last ENG3 level. ENG3 has no decoded map yet: `tools/level/level_decode.py` raises on every tag other than `i2.5`, and the `i1.a` decoder is an S3 output (`S3.03`, `S3.04`). Engine-wide ENG3 work stays in S3, and shared runner, obstacle and lane findings come from [L02](../L02-heros-gauntlet/README.md) and [L07](../L07-cyclops-attack/README.md). This plan covers only what is specific to TORMENT.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L09 Passageways of Eternal Torment (Windows manual spelling) |
| internal ID / label | 10 / PASSAGEWAYS TORMENT |
| engine / family | ENG3.DLL / `i1.a` (BIN magic `69312e61`) |
| archive directory | `M:\GRAFIX\MAPS\TORMENT\` |
| BIN | index 520, ID `4B83C8E3`, offset 0x1878498, 1,085,500 B, SHA-256 `b5eafa1a5204289ff6e08454a9e972c4654be9baada61ecdec16b7f0bff0c162` |
| CIF | index 521, ID `4C83C0E3`, offset 0x253da32, 336,384 B, SHA-256 `47cd479c216c25353d62d0e4cbb5538c8ac102df9d745fb0b6cabd03242b663a` |
| ENG3 descriptor | 0x38618 (table 0x38528, 0x18 B); path pointer 0x10038648 → `O:\TORMENT\`; field +4 = 0x10903c |
| map load | ENG3 0x1a930; no stream-chunk routine recorded (`stream_chunk: null`) |
| per-level config | UNRESOLVED (`S3.02`) |
| TITLE | descriptor 0x23528, module pointer 0x222b0; dispatch 0x16300 |
| EXE palette | 0x5760 selector 10, path string 0x3669c |
| module switch | EXE 0x4040 (L08 ENG1 → L09 ENG3 → L10 ENG1) |
| difficulty | Medium / Herculean listed as playable; Medium is the baseline. Beginner is unavailable per the manual only (LEAD until `S6.06` observes it) |

## Scope

The plan covers the following, from first entry to the next-level load:
- the passage run through successive sections, as TORMENT uses the S3 runner model and the L02/L07 findings
- skeletons and lost souls (scenario design, LEAD)
- the chariot and falling stalactites (scenario design, LEAD)
- falls into the River Styx, with death and retry
- pickups, checkpoints and retry
- the L08 → L09 (ENG1 → ENG3) entry, completion, tally, and the L09 → L10 (ENG3 → ENG1) VORTEX load
- the Beginner availability check. This is an availability check through `S6.06`, not a Beginner gameplay capture.

## Dependencies

| needs | for |
|---|---|
| [S3](../_shared/S3-eng3-engine.md) `S3.01`–`S3.03` named ENG3 skeleton, descriptor use, i1.a struct | every `L09.RE.*` |
| S3 `S3.04`/`S3.05` `eng3_decode.py` and ENG3 preview | `L09.AS.01`, `L09.GP.01` |
| S3 `S3.06` ENG3 hooks | every `L09.TR.*` |
| S3 `S3.07` runner model (`docs/full-game/gameplay/chase.md`) | `L09.GP.02` |
| [L02](../L02-heros-gauntlet/README.md) / [L07](../L07-cyclops-attack/README.md) phase 1 names, lane/branch data and runner use | `L09.RE.02`–`RE.03`, `L09.GP.02` |
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L09.TR.*` |
| [S4](../_shared/S4-actors-animation.md) SRC3 bank decode and encounter template | `L09.AS.03`–`04`, `L09.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (TORMENT music segment, ENG3 sound calls) | `L09.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods, transitions, `S6.06` Beginner boundary | `L09.TR.01`, `L09.TR.08`–`TR.10`, `L09.RE.06` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L09.RM.*` |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | descriptor/config for ID 10, section/lane data, skeleton and soul handlers, chariot, stalactites, river falls, completion, Beginner gate | P2, S3.01–S3.03, L02/L07 phase 1 | todo (blocked by S3) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, Herculean diff, Beginner availability, L08→L09→L10 links | S1, S3.06, S6.01–03, S6.06 | todo (blocked by S3) |
| [3 Assets](3-assets.md) | i1.a map/palette, SRC3 banks (torment, souls), audio, scene coverage | S3.04–05, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | sections, actors, hazards, falls, checkpoints, completion spec | 1, 2, 3, S3.07 | todo |
| [5 Remaster](5-remaster.md) | renderer coverage, art inventory, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l09-passageways-of-eternal-torment/`)

[full-progression](../../reference/scenarios/l09-passageways-of-eternal-torment/full-progression.json) ·
[passage-opening](../../reference/scenarios/l09-passageways-of-eternal-torment/passage-opening.json) ·
[passage-sections](../../reference/scenarios/l09-passageways-of-eternal-torment/passage-sections.json) ·
[skeletons-lost-souls](../../reference/scenarios/l09-passageways-of-eternal-torment/skeletons-lost-souls.json) ·
[chariot-stalactites](../../reference/scenarios/l09-passageways-of-eternal-torment/chariot-stalactites.json) ·
[river-falls](../../reference/scenarios/l09-passageways-of-eternal-torment/river-falls.json) ·
[passage-completion](../../reference/scenarios/l09-passageways-of-eternal-torment/passage-completion.json) ·
[beginner-unavailable](../../reference/scenarios/l09-passageways-of-eternal-torment/beginner-unavailable.json) ·
[difficulty-herculean](../../reference/scenarios/l09-passageways-of-eternal-torment/difficulty-herculean.json)

## Level acceptance criteria

1. Every L09 Medium scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The TORMENT map decodes through S3's `eng3_decode.py`, and its passage sections and lanes are identified, with every section traced.
3. `docs/full-game/levels/L09.md` specifies the TORMENT use of the runner model, sections, skeletons and souls, chariot, stalactites, river falls, checkpoints and completion, with code and trace sources, and defers shared rules to `chase.md`, L02 and L07.
4. Herculean differences are recorded, or equality is recorded for the routes compared.
5. The Beginner cell holds the observed `S6.06` result: `UNAVAILABLE_BY_DESIGN` only once the endpoint and the rejected entries are observed in this build. It is never counted as a missing Beginner playthrough.
6. The L08 → L09 (ENG1 → ENG3) and L09 → L10 (ENG3 → ENG1) transitions are captured as links in the S6 linked run.
7. In remaster mode the level plays from entry to the L10 load with approved art. The user's side-by-side check against the original passes.

## Notes
