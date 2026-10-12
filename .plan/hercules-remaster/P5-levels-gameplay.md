# P5 Levels + gameplay

Goal: recover the level format and the hero's simulation (clock, demo format, behaviours, constants) from code and traces.
Needs: P2, P3, P4 (images).
Outputs:
- `tools/level/`, `tools/state/`
- `extracted/levels/`
- `docs/level-format.md`, `docs/gameplay-spec.md`, `docs/demo-format.md`
- `reference/tuning.json`

## Levels

- [x] Map the `GRAFIX\MAPS` entries and the large i2.5 payloads to the level-load code path (Ghidra xrefs).
- [x] In Ghidra, recover the level header with `create_struct` (size, tile size, layers, parallax, tileset/palette refs), plus the collision encoding, spawn table and camera limits (`LVL_*`).
- [x] Write `tools/level/level_decode.py` → `extracted/levels/<level>/level.json`. Keep unknown fields with their offsets.
- [x] Write `tools/level/render_preview.py` (PNG per layer, composite, collision overlay) and `tools/viewer/level_viewer/` (HTML with layer toggles).

## Gameplay

- [x] In Ghidra, locate the hero object/state machine (`HC_DIE`, `HC_DUNIT`, `HC_THROW`), the per-frame update reached from `PC_DLLEngineMain`, and the `DEMO*.DAT` reader (`HERO_*`).
- [x] Find the simulation rate from the code (frame limiter, `timeGetTime` use). Confirm it with a tick-counter hook during the demo.
- [x] Decode `DEMO*.DAT` through its reader code into `tools/state/demo.py` and write `docs/demo-format.md`.
- [x] Write `tools/state/trace_state.py`, which logs per-tick hero x, y, velocity, state id and animation frame during the demo (using the P3 tracer).
- [x] Specify the hero's idle, move, jump, land, attack, damage, death and restart, plus one enemy, from the decompiled handlers. Write `docs/gameplay-spec.md` and put the constants, in their original representation, in `reference/tuning.json`.

Done when one level renders with collision and spawns, and the hero's behaviours and constants are specified.

## Notes

- Level path: `LVL_LoadMap` ENG1 0x1f750 (tag check, header, VRAM upload 0x1fc90), section/block streaming 0x1f870/0x1f9e0, header parse 0x20670, section 0x20830, block cells 0x208f0. Collision 0x20b50/0x20990, camera 0x2f840/0x32300, cell meshes 0x2df20, spawns 0x26040 + ENG1 level config 0x68af0. 48 names → `analysis/annotations/p5.json` (applied).
- Levels are grids of 64-unit 3D block types (mesh + collision heightfield) in 128-unit depth layers along a straight/arc camera path. Colours: MAP_PSX.CIF palette + 31 fog remap rows (the PC rasteriser is 8-bit).
- 7/10 maps decode (ENG1 i2.5: TRAINING, FOREST, THEBES, TITANS, HYDRA, MEDUSA, VORTEX); ENG3 i1.a (GAUNTLET, CYCLOPS, TORMENT) still unsupported. TRAINING renders in full (25,049 quads) with collision lines and 451 spawns; `viewer.html` has layer toggles. → `docs/level-format.md`.
- Q-07: ENG1 = level ids 0,1,3,5,6,7,9,11; ENG3 = 2 (GAUNTLET), 8 (CYCLOPS), 10 (TORMENT) (+4 NESSUS, presumably).
- Hero: pool C slot 0 (0x72860), state id 0x72108, handler table 0x72040 (`HERO_Init` 0x15090), animation scripts in ENG1 data (`HERO_AnimStep` 0x15d00; 56 scripts decoded). Physics: run 8.0 / accel 0.5; jump vy 14.0, rise decel 0.875 (+0.75 when the button is released), gravity 0.375 after apex, max fall 13.0. Runtime trace (`hero_state.csv`) matches: max vx 8.000, take-off vy 14.000.
- Simulation rate: fixed 33 ms step (EXE `RT_Tick`), confirmed by the tick hook (1 tick per present, 30.33/s).
- DEMO*.DAT: 6-byte header (byte 0 = start index, not the level), then one u16 pad word per tick from word 3, 884 ticks. → `docs/demo-format.md`, `tools/state/demo.py`.
- Enemy: Stymphalian bird (type 0x38) spawn 0x433d0 / update 0x43440. → `docs/gameplay-spec.md`, `reference/tuning.json`. 35 more names → `analysis/annotations/p5_hero.json` (applied).
- Open: ENG3 maps (i1.a), damage/hit boxes, remaining states 9/13/14/28/47, models/sky in the level preview.

