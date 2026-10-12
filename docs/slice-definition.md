# P6 slice (hybrid route)

The slice is the original game (HERCULES.EXE + ENG1.DLL) played through the GPU renderer (`docs/gpu-renderer.md`) at 3840×2160. All logic, timing, sound and input are the original's. The renderer redraws each tick at 9× and can swap in replacement art. Names: `analysis/annotations/p5_hero.json`, `p6.json`.

## Range

TRAINING (level id 1, ENG1), section 0, from start point 0 to just past the first bird group: world x 1214 → 10240 (columns 18–159 of 500).

Start points come from the ENG1 table `0x5a69c[level]`, 0x18 bytes each. The hero is placed at `x = e.x − 32`, `y = e.y + 64`, layer `e.z` (`HERO_Init` 0x15090).

| start | x | y | note |
|---:|---:|---:|---|
| 0 | 1214 | 780 | new game |
| 1 | 8224 | 780 | restart point; the demo (`DEMO*.DAT` byte 0 = 1) starts here: `hero_state.csv` tick 1135 has x 8224 |

Restart: `HERO_Die` 0x18f60 → respawn at a start index (`docs/gameplay-spec.md`, state 22).

## Content (spawn list, `extracted/levels/TRAINING/level.json`)

There are 140 spawns in range.

| role | kind | count | where (x) | code |
|---|---|---:|---|---|
| enemy | 0x38 Stymphalian bird (`ALIEN2\STYBIRD`) | 5 | 9209–9918, y 217–321 | spawn 0x433d0, update 0x43440 (`docs/gameplay-spec.md`) |
| hazard | 0x36 contact hazard | 5 | 4135–4611, y 445–515 | `HERO_Hazard36Spawn` 0x3e6d0, `HERO_Hazard36Update` 0x3e770 |
| pickup | 0x2019 | 60 | whole range | counted into game state +0x16 (`LVL_SpawnObjects` 0x26040) |
| pickup | 0x2018 | 22 | whole range | counted into game state +0x1a |

The type table is at level config +0x1c (TRAINING: ENG1 0x67608). Each entry is 12 bytes, `[spawn, stub 0xc720, update]`, indexed by kind.

### Hazard 0x36 (`HERO_Hazard36Update` 0x3e770)

- **State 0 (wait):** when the hero is within 32 units on both axes, it starts animation 0x17.
- **State 1:** the object stands on the floor (0x2a880 surface fetch). If its box overlaps the hero's box (`0x2a7a0`, hero box at hero+0x88), it calls `HERO_RequestDamage(obj, 1, 0)` (0x266a0) and counter 0x70ef0 is incremented.
- **Leaving:** when its animation ends (0x2bc50 bit 0) or the hero hits it (0x26de0), it plays sfx 0x4040, switches to animation 0xb, then state 9 → 0x13. In 0x13 it rises at 2.0 units/tick and is freed once off-screen.

`HERO_RequestDamage` 0x266a0 keeps the strongest hit of the tick (0x6feb4 amount + 1, 0x6feb8 direction, 0x6febc) and returns the invulnerability timer 0x72008. `HERO_Update` applies the hit.

## Input

- **Keyboard:** EXE 0x7f30 (arrows, X/Z/C/D/S; `docs/gameplay-spec.md`).
- **Gamepad:** WinMM joystick (`RT_PollJoystick` EXE 0x7c50). Buttons 0–27 map to bits 0–27. X/Y beyond ±33 % of the calibrated range set 0x10000000 left, 0x20000000 right, 0x40000000 up and 0x80000000 down. XInput pads show up through WinMM's joystick layer.

## Running it

In `working/game/hercules_display.ini`:
- set `renderer = gpu`;
- optionally set `dev = on` for the overlay: frame, module, hero state/frame/position/velocity, last archive file, pivot cross and hero collision box.

Start `working/game/HERCULES.EXE` (borderless 3840×2160, 2880×2160 view). The new game starts at start point 0. The slice ends after the bird group at x ≈ 10240.
