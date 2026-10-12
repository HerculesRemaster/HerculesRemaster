# P5 All-level layouts and full gameplay

Plan status: **deferred for later implementation**. Registry/scenario documents live in this plan's `reference/`; new run/export/specification outputs use the isolated paths in [START-HERE.md](START-HERE.md). Existing lean tools/outputs below are inherited inputs.

Goal: recover layouts and **full gameplay for all ten Windows levels**: traversal, chase/flight modes, encounters, scripts, checkpoints, scoring, progression and the ending, from code plus runtime traces.
Needs: P2, P3 scene evidence, P4 assets; P2F remains prerequisite to runtime observation. Work can advance by level/scene; one rendered level or one enemy does not close P5.
Outputs:
- `tools/level/`, `tools/state/`
- `extracted/levels/`
- `docs/level-format.md`, `docs/gameplay-spec.md`, `docs/demo-format.md`
- `reference/tuning.json`
- Future `extracted/full-game/levels/<level>/{level.json,scenes/}`, `docs/full-game/levels/<level>.md`
- Future `docs/full-game/gameplay/{progression,combat,encounters,chase,flight,checkpoints,difficulty}.md`
- Future `extracted/meta/full-game/gameplay-coverage.json` (scenario-linked specification coverage)

Existing tasks/checks remain intact. [Task identities](TASK-IDENTITIES.md) preserve equivalent archived IDs; additions use `P5.ALL.*`.

## Levels

- [ ] Map the `GRAFIX\MAPS` entries and the large i2.5 payloads to the level-load code path (Ghidra xrefs).
- [ ] In Ghidra, recover the level header with `create_struct` (size, tile size, layers, parallax, tileset/palette refs), plus the collision encoding, spawn table and camera limits (`LVL_*`).
- [ ] Write `tools/level/level_decode.py` → `extracted/levels/<level>/level.json`. Keep unknown fields with their offsets.
- [ ] Write `tools/level/render_preview.py` (PNG per layer, composite, collision overlay) and `tools/viewer/level_viewer/` (HTML with layer toggles).

## Gameplay

- [ ] In Ghidra, locate the hero object/state machine (`HC_DIE`, `HC_DUNIT`, `HC_THROW`), the per-frame update reached from `PC_DLLEngineMain`, and the `DEMO*.DAT` reader (`HERO_*`).
- [ ] Find the simulation rate from the code (frame limiter, `timeGetTime` use). Confirm it with a tick-counter hook during the demo.
- [ ] Decode `DEMO*.DAT` through its reader code into `tools/state/demo.py` and write `docs/demo-format.md`.
- [ ] Write `tools/state/trace_state.py`, which logs per-tick hero x, y, velocity, state id and animation frame during the demo (using the P3 tracer).
- [ ] Specify the hero's idle, move, jump, land, attack, damage, death and restart, plus one enemy, from the decompiled handlers. Write `docs/gameplay-spec.md` and put the constants, in their original representation, in `reference/tuning.json`.

## Full-game recovery tasks

- [ ] `P5.ALL.01` Complete the ten-level BIN/CIF → header/chunk/palette/consumer map from `reference/levels.json`. Cover ENG1 0x1f750 / ENG3 0x1a930, `i2.5` and `i1.a`; resolve actual Nessus and Thebes substage transitions. The verified static map is a starting point, not decoded level data.
- [ ] `P5.ALL.02` Recover engine/variant-specific structures for layers/order/parallax, collision/surface/depth, spawn/event tables, camera limits and streaming. Export **L01–L10** layouts/scenes with source IDs/ranges, unknown byte offsets and original coordinate representation. Avoid forcing chase/flight data into an unverified platformer schema.
- [ ] `P5.ALL.03` Render every level's layers/composite, collision, spawns, triggers and camera/depth paths. Align P3 scene/checkpoint landmarks, including background stairs, alternate chase lanes and flight depth; record camera/scroll state and unexplained mismatches.
- [ ] `P5.ALL.04` Recover training gates, rescues, breakables/secrets, moving props/platforms, hazards, boss transitions, checkpoint/respawn and completion/tally scripts. Specify trigger conditions, ordering, persistence and difficulty guards.
- [ ] `P5.ALL.05` Extend per-tick state tracing to every scenario: hero position/depth/velocity/state/frame, camera, level/substage/difficulty, HP/lives/power-ups, checkpoint and encounter/event state. Recover field offsets before interpreting values. Preserve P2's fixed 33 ms clock finding; measure pause/catch-up/render-skip behavior in platformer/chase/flight/boss modes.
- [ ] `P5.ALL.06` Decode the eight archived demo payloads through their readers and map each to its actual level/route. Use the demo replay as the first state/timing check; demos alone cannot provide complete progression. Compare repeated scenario traces with recorded initial RNG/state where recovered.
- [ ] `P5.ALL.07` Specify all observed traversal forms: ledges, poles/swinging, carrying/throwing, power punch, ground slam, foreground/background paths/stairs, secrets and returns. Extend combat/pickup rules to combos/uppercuts, power-up selection/energy, projectiles/reflection, damage/invulnerability/knockback, rescues, inventory, coins/letters/vases and tally/password awards.
- [ ] `P5.ALL.08` Recover state machines for every observed enemy/NPC/hazard and all major encounters: Nessus (L03), Hydra (L05), Medusa (L06), Cyclops events (L07), Titans/god rescues (L08), final-level actors/events (L10). Record entry, telegraphs/attacks, vulnerable windows, damage/phase changes, defeat/exit and asset/audio bindings. Tentative phases remain discovery targets until observed.
- [ ] `P5.ALL.09` Specify L02/L07/L09 rush/chase behavior: automatic motion/speed control, lane/depth changes, jumping/obstacle timing, cross traffic/falling hazards, collision/fall/death and completion, including difficulty-specific schedules.
- [ ] `P5.ALL.10` Specify L08 Pegasus flight: axes/depth, changed input/attacks, motion/camera, Titan hazards, rescues, damage/retry and completion. Recover actual code/trace rules instead of importing side-scroller assumptions.
- [ ] `P5.ALL.11` Recover all-level progression/unlocks/password entry, Thebes substages, checkpoint persistence/reset, lives/continues/game-over/restart, tally/loading, inter-level transitions and final rescue/encounter/ending/credits. Verify Beginner ends after L08 and excludes L09/L10; use Medium for complete progression.
- [ ] `P5.ALL.12` Record Beginner/Medium/Herculean resource, spawn, health/damage, timing, phase, route and progression differences from matched P3 traces. Unknown or unobserved differences remain open. Store measured constants in `extracted/meta/full-game/tuning.json` in their original integer/fixed-point/tick representation, keyed by source program+RVA, entity/state, difficulty and scenario; measure tolerances before setting numeric acceptance bounds.
- [ ] `P5.ALL.13` Complete `docs/full-game/levels/<level>.md` and `extracted/meta/full-game/gameplay-coverage.json` for all ten levels: scenes/layout, collision/spawns/scripts, camera/audio, checkpoints/scoring, encounters/difficulty and completion. Link every required behavior to source offsets, handlers and P3 scenario/tick evidence. Hand the complete reference set to P6; append new stages/scenes using the discovery workflow.

Done when **all ten levels** render with verified layers/collision/spawns/events/cameras, every required scene and observed difficulty difference has a code/trace-supported specification, and linked Medium progression through the ending is accounted for. The original one-level/hero/one-enemy targets remain intermediate milestones. Unknown fields/behaviors affecting required gameplay block completion and name the next trace/decompilation needed.

## Notes
