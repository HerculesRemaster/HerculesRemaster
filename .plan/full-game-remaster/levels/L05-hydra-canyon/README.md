# L05 Hydra Canyon — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L05 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L05 Hydra Canyon (Windows manual spelling) |
| internal ID / label | 6 / HYDRA BATTLE |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\HYDRA\` (descriptor path `O:\HYDRA\`) |
| BIN | index 541, ID `527C4FA9`, offset 0x1461498, 997,376 B, SHA-256 `7d4793ac02408d2b35d286f7bc05779517c5aab72ef09226fd86171f266eb5db` |
| CIF | index 517, ID `4A7C50A9`, offset 0x2208632, 336,384 B, SHA-256 `0d31de1d60577fa117a4b41298c65becd827410d57beb69ea8dbfa362cab2a61` |
| ENG1 descriptor | 0x5a720 (table 0x5a690, 0x18 B), header size 0xf0800; map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·6 (type table at +0x1c) |
| TITLE | descriptor 0x234f8, module pointer 0x222a0; dispatch 0x16300 |
| EXE palette | 0x5760 selector 6, path string 0x366fc |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |
| map shape | a small arena: 2 layers, 80×18 cells, 1 block, only 2 spawns (kind 0x31); decoder warns of a 3,156 B VRAM overrun (S2.03) |

## Scope

The plan covers the following, from first entry to the next-level load:
- the canyon entry and arena setup
- the **Hydra** boss encounter: attacks, damage, head/state changes and any phases (all LEAD until observed)
- death/retry inside the fight
- defeat, completion, tally, and the L06 MEDUSA load

The two map spawns cannot by themselves describe a multi-headed boss. The working hypothesis is that the Hydra logic lives in a type handler or level script, and that it is drawn at least partly with T28 models (S2.01). Both are hypotheses until `L05.RE.02`–`04` confirm them. The phase count stays null until it is observed.

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L05.TR.*` |
| [S2](../_shared/S2-eng1-map-completion.md) T28 models via `GFX_DrawModel` 0x31540 (`S2.01`), VRAM overrun (`S2.03`), previews (`S2.04`), config export (`S2.05`) | `L05.RE.04`, `L05.AS.01`–`02`, `L05.GP.01`, `L05.RM.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode and encounter template | `L05.AS.03`–`04`, `L05.GP.03` |
| [S5](../_shared/S5-audio-music.md) audio binding (HYDRA music segment) | `L05.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods, checkpoints/tally | `L05.TR.01`, `L05.TR.06`–`07`, `L05.GP.05` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L05.RM.*` |
| L01 / lean plan | ENG1 hero spec (`docs/gameplay-spec.md`) is reused, not re-derived |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, kind 0x31 handlers, Hydra state machine, model use, arena scripts | P2, S2.01, S2.05 | in progress (RE.01 done) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, Hydra attacks/damage/defeat, retry, difficulty diffs | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | arena map/models/palette, Hydra bank, effects, audio, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | arena layout, Hydra encounter, retry, completion spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | art inventory, renderer coverage (models), 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l05-hydra-canyon/`)

[full-progression](../../reference/scenarios/l05-hydra-canyon/full-progression.json) ·
[canyon-entry](../../reference/scenarios/l05-hydra-canyon/canyon-entry.json) ·
[hydra-attacks](../../reference/scenarios/l05-hydra-canyon/hydra-attacks.json) ·
[hydra-damage-phases](../../reference/scenarios/l05-hydra-canyon/hydra-damage-phases.json) ·
[hydra-defeat](../../reference/scenarios/l05-hydra-canyon/hydra-defeat.json) ·
[death-retry](../../reference/scenarios/l05-hydra-canyon/death-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l05-hydra-canyon/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l05-hydra-canyon/difficulty-herculean.json)

## Level acceptance criteria

1. Every L05 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The Hydra encounter is specified from code and trace: what spawns it, how it is drawn (models, sprites, or both), its states, attacks, damage rules and defeat. Phases are split into scenario files only where observed state changes.
3. The HYDRA VRAM overrun has a code-backed explanation (with S2.03).
4. `docs/full-game/levels/L05.md` specifies the arena, the Hydra encounter, retry and completion, with code and trace sources.
5. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
6. In remaster mode the level plays from entry to the L06 load with approved art. The user's side-by-side check against the original passes.

## Notes

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
