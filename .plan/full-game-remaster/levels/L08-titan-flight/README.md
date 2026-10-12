# L08 Titan Flight — level implementation plan

Plan status: **deferred**. This plan can be executed on its own once its dependencies exist. It slices the global P3/P4/P5 tasks for L08 and does not replace them. Paths are relative to `Remaster/`. Addresses are program+RVA.

## Identity (VERIFIED-STATIC, `reference/levels.json`)

| field | value |
|---|---|
| public level | L08 Titan Flight (Windows manual spelling) |
| internal ID / label | 9 / BATTLE OF TITANS |
| engine / family | ENG1.DLL / `i2.5` (BIN magic `69322e35`) |
| archive directory | `M:\GRAFIX\MAPS\TITANS\` (descriptor path `O:\TITANS\`) |
| BIN | index 538, ID `526D69FA`, offset 0x174c498, 1,228,800 B, SHA-256 `b1869b638dd8f109c276718534b3b4d4da0781f5b12b3f41fe5d4c6f8b7bf57d` |
| CIF | index 539, ID `526E69F2`, offset 0x22fec32, 336,384 B, SHA-256 `b2a12c9b58ddd54d49923df59c5d8408d0889c6a578622c86ca0ce4c400a5571` |
| ENG1 descriptor | 0x5a768 (table 0x5a690, 0x18 B), header size field 0xf7000; map load 0x1f750, stream chunks 0x1f9e0 |
| per-level config | ENG1 0x68af0 + 0x48·9 (type table at +0x1c) |
| TITLE | descriptor 0x2351c, module pointer 0x222ac; dispatch 0x16300 |
| EXE palette | 0x5760 selector 9, path string 0x366cc |
| difficulty | Beginner / Medium / Herculean; Medium is the baseline |
| neighbours | previous L07 CYCLOPS (ID 8, ENG3); next L09 TORMENT (ID 10, ENG3) on Medium/Herculean. On Beginner the manual says the game ends after L08 (LEAD until S6.06) |

## Scope

The plan covers the following, from first entry to the next-level load (or the Beginner endpoint):
- **Pegasus flight**: the hero's movement, input mapping, attacks, depth and camera while flying. This is not assumed to be the ENG1 side-scroller model; it is specified from code and traces.
- the successive flight sections and their streaming/camera changes
- the elemental **Titan** hazards (Ice, Wind/tornado, Lava, Rock — names are LEAD, from banks and the scenario set)
- the **god rescues** (names are LEAD, from banks)
- the final rescue/exit (the `zeus-completion` scenario; what actually ends the level is a discovery target)
- death/retry
- completion, tally, the ENG1 → ENG3 switch to the L09 TORMENT load, and the Beginner endpoint after L08

The flight model is shared engine knowledge only for this level; it is written once into `docs/full-game/gameplay/flight.md` (`P5.ALL.10`) and summarized in `L08.md`.

## Dependencies

| needs | for |
|---|---|
| [S1](../_shared/S1-runtime-harness.md) runner and identity events | every `L08.TR.*` |
| [S2](../_shared/S2-eng1-map-completion.md) T28 models (`S2.01`, `GFX_DrawModel` ENG1 0x31540), fields (`S2.02`), previews (`S2.04`), config export (`S2.05`) | `L08.RE.01`, `L08.AS.01`, `L08.GP.01` |
| [S4](../_shared/S4-actors-animation.md) bank decode and encounter template | `L08.AS.03`–`04`, `L08.GP.03`–`04` |
| [S5](../_shared/S5-audio-music.md) audio binding (TITANS music segment, TITANS/HERC_TIT Sonix) | `L08.AS.05` |
| [S6](../_shared/S6-progression-difficulty.md) difficulty encoding and entry methods (`S6.01`–`03`), transitions L07→L08 and L08→L09 (`S6.05`), Beginner boundary (`S6.06`), linked run (`S6.08`) | `L08.TR.01`, `L08.TR.09`–`10`, `L08.RE.06`, `L08.GP.05` |
| [S7](../_shared/S7-remaster-integration.md) + an approved P7 pilot | `L08.RM.*` |
| L01 / lean plan | the ENG1 hero spec (`docs/gameplay-spec.md`) is a reference for shared hero code only; flight rules are re-derived where code shows a different path |

## Phases

| phase | content | needs | state |
|---|---|---|---|
| [1 Reverse engineering](1-reverse-engineering.md) | config/type table, flight mode and input, Titan and god handlers, completion and Beginner branch | P2, S2.05 | in progress (RE.01 done) |
| [2 Runtime trace](2-runtime-trace.md) | all scenarios, complete flight traversal, transitions, Beginner endpoint, difficulty diffs | S1, S6.01–03 | todo |
| [3 Assets](3-assets.md) | map/models/palette, Pegasus/Titan/god banks, audio, transition screens, scene coverage | S2, S4, S5, trace | todo |
| [4 Gameplay](4-gameplay.md) | flight spec, Titans, rescues, checkpoints, completion spec | 1, 2, 3 | todo |
| [5 Remaster](5-remaster.md) | renderer coverage incl. GPU vs software diff, art inventory, 4K art, play check | 3, 4, S7, P7 approval | todo |

Phases 1 and 2 run in parallel. Each phase file has its own "Done when" list.

## Scenarios (`reference/scenarios/l08-titan-flight/`)

[full-progression](../../reference/scenarios/l08-titan-flight/full-progression.json) ·
[flight-opening](../../reference/scenarios/l08-titan-flight/flight-opening.json) ·
[flight-sections](../../reference/scenarios/l08-titan-flight/flight-sections.json) ·
[titan-encounters](../../reference/scenarios/l08-titan-flight/titan-encounters.json) ·
[god-rescues](../../reference/scenarios/l08-titan-flight/god-rescues.json) ·
[zeus-completion](../../reference/scenarios/l08-titan-flight/zeus-completion.json) ·
[death-retry](../../reference/scenarios/l08-titan-flight/death-retry.json) ·
[difficulty-beginner](../../reference/scenarios/l08-titan-flight/difficulty-beginner.json) ·
[difficulty-herculean](../../reference/scenarios/l08-titan-flight/difficulty-herculean.json)

## Level acceptance criteria

1. Every L08 scenario row in `coverage.md` is captured, traced, has its assets exported, and has a gameplay spec, each with run paths and tick ranges.
2. The flight mode is specified from code and trace in `docs/full-game/gameplay/flight.md`: axes/depth, input mapping, attacks, motion and camera, damage/retry. Every rule cites a handler RVA and a tick range; nothing is carried over from the side-scroller spec without evidence.
3. `docs/full-game/levels/L08.md` specifies the sections, Titan hazards, god rescues, checkpoints and completion, with code and trace sources. Titan and god identities come from code or runtime bank loads, not from bank names alone.
4. The L07 → L08 and L08 → L09 transitions (ENG3 ↔ ENG1 module switches) are captured as links in the S6 linked run, and what follows L08 on Beginner is observed and handed to S6.06.
5. Beginner and Herculean differences are recorded, or equality is recorded for the routes compared.
6. In remaster mode the level plays from entry to the L09 load with approved art, GPU frames match software frames, and the user's side-by-side check against the original passes.

## Notes

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
