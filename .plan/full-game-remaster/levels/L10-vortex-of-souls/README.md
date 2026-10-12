# L10 Vortex of Souls — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L10 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L10 Vortex of Souls (Windows manual spelling) |
| internal ID / label | 11 / VORTEX OF SOULS |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\VORTEX\` (descriptor path `O:\VORTEX\`) |
| BIN | index 518, ID `4B806805`, offset 0x1ab4cd4, 917,504 B, SHA-256 `605d5300687d2310a64a4b29e2f7bbd0cda2a5a2ea5f05d41e13153686579d32` |
| CIF | index 519, ID `4B8167FD`, offset 0x25e1e32, 336,384 B, SHA-256 `1193f7f475a2b14a588d5001ba1f854a6c22cbd273be6779f09f4caca384c39a` |
| ENG1 descriptor | 0x5a798 (table 0x5a690, 0x18 B), header size field 0xdf800; map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·11 (type table at +0x1c) |
| TITLE | descriptor 0x23534, module pointer 0x222b4; dispatch 0x16300 |
| EXE palette | 0x5760 selector 11, path string 0x366b4 |
| difficulty | Medium / Herculean; Medium is the baseline. Beginner unavailable per the manual (LEAD until S6.06) |
| neighbours | previous L09 TORMENT (ID 10, ENG3); next: none. This is the final level; the ending and credits follow it |

## Scope

The plan covers the following, from first entry to the end of the credits and the return to title:
- vortex entry and the movement mode inside it. The decoded map is nearly empty (1 layer, 20×20 cells, 0 spawns), so the content is probably model-, effect- or script-driven. That is a hypothesis for `L10.RE.02`–`03`, not a result.
- souls and any other enemies or hazards
- the rescue objective (what is rescued, and how, comes from code or runtime)
- the **final encounter**: its actor(s), spawn source, states and phases. The actor's identity is not assumed; BOSS_HADES 0803 is only a hypothesis.
- death/retry and game over
- completion → ending media → credits → title (S6.05)
- the Beginner availability check: an entry/endpoint observation, not a gameplay capture

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L10.TR.*` |
| [S2](../_shared/S2-eng1-map-completion.md) T28 models (`S2.01`, `GFX_DrawModel` ENG1 0x31540), fields (`S2.02`), VORTEX VRAM overrun (`S2.03`), previews (`S2.04`), config export (`S2.05`) | `L10.RE.01`–`03`, `L10.AS.01`–`02`, `L10.GP.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode and encounter template | `L10.AS.03`–`04`, `L10.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (VORTEX music segment, VORTEX/HERC_VOR Sonix, ending music) | `L10.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding and entry methods (`S6.01`–`03`), L09→L10 switch and ending/credits (`S6.05`), Beginner boundary (`S6.06`), linked run end point (`S6.08`) | `L10.TR.01`, `L10.TR.07`, `L10.TR.10`–`11`, `L10.RE.06`, `L10.GP.05`–`06` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L10.RM.*` |
| L01 / lean plan; [L08](../L08-titan-flight/README.md) `flight.md` | the ENG1 hero spec (`docs/gameplay-spec.md`) and the flight spec are cited only where code shows L10 uses the same path |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, where the actors come from, model/effect content, final encounter, ending path | P2, S2.01, S2.05 | in progress (RE.01 done) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, bank opens, final encounter, ending/credits, Beginner availability, Herculean diff | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | map/models/palette, runtime-loaded banks, audio, ending media, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | movement mode, souls, objective, final encounter, completion/ending spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | renderer coverage incl. GPU vs software diff, art inventory, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l10-vortex-of-souls/`)

[full-progression](../../reference/scenarios/l10-vortex-of-souls/full-progression.json) ·
[vortex-entry](../../reference/scenarios/l10-vortex-of-souls/vortex-entry.json) ·
[souls-traversal](../../reference/scenarios/l10-vortex-of-souls/souls-traversal.json) ·
[final-encounter](../../reference/scenarios/l10-vortex-of-souls/final-encounter.json) ·
[rescue-objective](../../reference/scenarios/l10-vortex-of-souls/rescue-objective.json) ·
[completion-ending](../../reference/scenarios/l10-vortex-of-souls/completion-ending.json) ·
[death-retry](../../reference/scenarios/l10-vortex-of-souls/death-retry.json) ·
[beginner-unavailable](../../reference/scenarios/l10-vortex-of-souls/beginner-unavailable.json) ·
[difficulty-herculean](../../reference/scenarios/l10-vortex-of-souls/difficulty-herculean.json)

## Level acceptance criteria

1. Every L10 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges. The Beginner row is an availability result, not a gameplay capture.
2. The source of every L10 actor is answered with evidence: which code spawns the final-encounter actor(s) and souls when the map's spawn list is empty, and which banks load at runtime. The final actor is named only from code or runtime bank loads.
3. `docs/full-game/levels/L10.md` specifies the movement mode, souls, rescue objective, final-encounter states and observed phases (unknown values stay null), checkpoints, and completion → ending → credits, with code and trace sources.
4. The L09 → L10 transition (ENG3 → ENG1) and the ending/credits are captured as the final links of the S6.08 linked run; the Beginner cell is set to `UNAVAILABLE_BY_DESIGN` only after S6.06 observes it.
5. Herculean differences are recorded, or equality is recorded for the routes compared.
6. In remaster mode the level plays from entry through the credits with approved art, GPU frames match software frames, and the user's side-by-side check against the original passes.

## Notes

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
