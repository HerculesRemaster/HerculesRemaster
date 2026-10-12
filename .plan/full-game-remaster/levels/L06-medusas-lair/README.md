# L06 Medusa's Lair — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L06 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L06 Medusa's Lair (Windows manual spelling) |
| internal ID / label | 7 / MEDUSA BATTLE |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\MEDUSA\` (descriptor path `O:\MEDUSA\`) |
| BIN | index 530, ID `508159E4`, offset 0x1554c98, 847,872 B, SHA-256 `39bd41f031f8e1e69bc4bbe57b3568dfc2c904c0f7b01e14adb8a775225975cf` |
| CIF | index 531, ID `508259DC`, offset 0x225a832, 336,384 B, SHA-256 `4d6cde643d0285ea68a12bc83cdfe2bb7716c87d4c61ff86be7590889a6a84c0` |
| ENG1 descriptor | 0x5a738 (table 0x5a690, 0x18 B), header size 0xcc000; map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·7 (type table at +0x1c) |
| TITLE | descriptor 0x23504, module pointer 0x222a4; dispatch 0x16300 |
| EXE palette | 0x5760 selector 7, path string 0x366e4 |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |
| map shape | 2 layers, 80×18 cells, 1 block, 6 spawns (all decoded as kind 0x0); decoder warns of an 84,096 B VRAM overrun, the largest of the ENG1 maps (S2.03) |
| next level | L07 CYCLOPS is ENG3 (ENG3 0x385e8, loader 0x1a930), so L06 → L07 is an ENG1 → ENG3 module switch |

## Scope

The plan covers the following, from first entry to the next-level load:
- lair traversal and entry
- skeleton encounters (LEAD)
- the **Medusa** boss encounter: gaze and projectile/venom attacks, and the reflection mechanic used to defeat her (all LEAD)
- death/retry
- defeat, completion, tally, and the ENG1 → ENG3 switch to the L07 CYCLOPS load

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events (both engines, for the L07 hand-off) | every `L06.TR.*` |
| [S2](../_shared/S2-eng1-map-completion.md) T28 models via `GFX_DrawModel` 0x31540 (`S2.01`), VRAM overrun (`S2.03`), previews (`S2.04`), config export (`S2.05`) | `L06.RE.05`, `L06.AS.01`–`02`, `L06.GP.01`, `L06.RM.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode, bank → level index `S4.01`, encounter template | `L06.RE.04`, `L06.AS.03`–`04`, `L06.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (MEDUSA music segment, `q:\HERC_MED\`) | `L06.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding, entry methods, checkpoints, ENG1 → ENG3 transition `S6.05` | `L06.TR.01`, `L06.TR.06`–`08`, `L06.GP.05` |
| [S3](../_shared/S3-eng3-engine.md) ENG3 hooks (`S3.06`), only for the receiving side of the L07 load | `L06.TR.08` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L06.RM.*` |
| L01 / lean plan | ENG1 hero spec (`docs/gameplay-spec.md`) is reused, not re-derived |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, kind 0x0 handlers, skeletons, Medusa state machine, reflection mechanic | P2, S2.01, S2.05 | in progress (RE.01 done) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, gaze/projectiles, reflection defeat, retry, difficulty diffs, L07 hand-off | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | lair map/models/palette, Medusa and skeleton banks, effects, audio, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | lair layout, skeletons, Medusa encounter, retry, completion spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | art inventory, renderer coverage, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l06-medusas-lair/`)

[full-progression](../../reference/scenarios/l06-medusas-lair/full-progression.json) ·
[lair-traversal](../../reference/scenarios/l06-medusas-lair/lair-traversal.json) ·
[skeleton-encounters](../../reference/scenarios/l06-medusas-lair/skeleton-encounters.json) ·
[gaze-projectiles](../../reference/scenarios/l06-medusas-lair/gaze-projectiles.json) ·
[reflection-defeat](../../reference/scenarios/l06-medusas-lair/reflection-defeat.json) ·
[death-retry](../../reference/scenarios/l06-medusas-lair/death-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l06-medusas-lair/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l06-medusas-lair/difficulty-herculean.json)

## Level acceptance criteria

1. Every L06 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The Medusa encounter is specified from code and trace: spawn, gaze and projectile attacks with their effect on the hero, the defeat mechanic (whether it is a reflection), and the exit. Phases are split only where observed state changes.
3. The bank attributions are settled by runtime loads: which skeleton banks (0486, 0566 SKELWAR) load in L06.
4. The MEDUSA VRAM overrun has a code-backed explanation (with S2.03).
5. `docs/full-game/levels/L06.md` specifies the lair route, skeletons, the Medusa encounter, retry and completion, with code and trace sources.
6. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
7. In remaster mode the level plays from entry to the L07 load with approved art. The user's side-by-side check against the original passes.

## Notes

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
