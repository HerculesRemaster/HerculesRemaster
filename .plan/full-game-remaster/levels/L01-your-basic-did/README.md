# L01 Your Basic D.I.D. — index to existing Level 1 work

This folder is an **index**. Level 1 work already exists and stays where it is; nothing here moves, copies or resets it. The remaining full-game gaps below are tracked through the shared plans and the global P3–P5 IDs. They do not open a second L01 plan.

## Identity (VERIFIED-STATIC, `../../reference/levels.json`)

| field | value |
|---|---|
| internal ID / label | 1 / TRAINING GROUND |
| engine / family | ENG1.DLL / `i2.5` |
| archive directory | `M:\GRAFIX\MAPS\TRAINING\` |
| BIN | index 565, ID `5ED398E4`, 1,259,520 B, SHA-256 `4529e134ee5d4c61949f104ae7f01bfa09ca8c0d3f1492de0ac769e682c7761e` |
| CIF | index 564, ID `5ECB98E5`, 336,384 B, SHA-256 `3cef394222de36e89121f86afe5e03a3222acb73edce932f82867f8508e64b82` |
| ENG1 descriptor | 0x5a6a8 (table 0x5a690); map load 0x1f750, stream chunks 0x1f9e0 |
| TITLE | descriptor 0x234bc, module pointer 0x2228c |
| EXE palette | 0x5760 selector 1, path string 0x36744 |
| per-level config | ENG1 0x68af0 + 0x48·1; type table at +0x1c = ENG1 0x67608 (`docs/slice-definition.md`) |
| difficulty | Beginner / Medium / Herculean |

## Where Level 1 work lives

| work | location | state |
|---|---|---|
| lean plan: P0–P6 done, P7 pilot in progress | [`.plan/hercules-remaster/`](../../../hercules-remaster/README.md) | active; edit only there |
| playable slice (hybrid GPU renderer, x 1214 → 10240) | `docs/slice-definition.md`, `docs/gpu-renderer.md` | done |
| hero states, physics, input, Stymphalian bird | `docs/gameplay-spec.md`, `analysis/annotations/p5_hero.json` | done (ENG1 hero is engine-wide) |
| decoded map: 5 layers, 500×16, 451 spawns | `extracted/levels/TRAINING/` (`level.json`, previews, `viewer.html`) | done |
| demo format and replays: DEMO, DEMO1_1/1_2/1_3 | `docs/demo-format.md`, `extracted/demos/` | done |
| runtime trace: title → TRAINING load at tick 1134 | `analysis/traces/runs/demo90`, `docs/resource-consumers.md` | demo only |
| full-game scenarios | [`reference/scenarios/l01-your-basic-did/`](../../reference/scenarios/l01-your-basic-did/) (9 definitions) | planned |

## Remaining full-game L01 gaps (tracked elsewhere)

| gap | tracked by |
|---|---|
| Full Medium traversal beyond the slice: training areas, traversal, combat exercises, secrets, checkpoint/retry, completion → tally → L02 load (ENG1 → ENG3 switch) | S1 runner + scenario JSONs; `P3.ALL.07`/`P3.ALL.08` |
| T28 models, sky and flat quads not drawn; T44/T4c/T50/T40 unknown | [S2](../_shared/S2-eng1-map-completion.md) |
| Checkpoint IDs; Hermes level-marker semantics (LEAD) | [S6](../_shared/S6-progression-difficulty.md) `S6.04` |
| Beginner/Herculean differential traces | `L01.difficulty-*` scenarios; S6 `S6.07` |
| Remaining actors: Phil (banks PHIL_*), TRAINING_* banks, damsel dummies (FANGIRLS? — LEAD) | [S4](../_shared/S4-actors-animation.md) |
| Password LEAD (walkthrough, Medium): Gladiator, Minotaur, Gladiator, Medusa | S6 `S6.03` |
| 4K art beyond the pilot | lean P7 → [S7](../_shared/S7-remaster-integration.md) |

Acceptance for L01 within the full game: every `l01-your-basic-did` scenario row reaches captured/traced/assets/spec in `coverage.md`, and the L01→L02 transition is part of the S6 linked Medium run.

Audio: [docs/full-game/audio.md](../../../../docs/full-game/audio.md) (banks, volumes, CD segment; S5).
