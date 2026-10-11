# Demo format (M:\DEMO\DEMO*.DAT)

9 files of 4,096 bytes: `DEMO`, `DEMO1_1`…`1_3`, `DEMO2_1`, `DEMO3_1`, `DEMO5_1`, `DEMO5_2`. Decoder: `tools/state/demo.py` → `extracted/demos/<name>.json`.

## Loading (ENG1 0x5ea0, called from `MOD_Tick` after `LVL_LoadMap`)

The file name comes from ENG1 table 0x4d938, indexed by game state +8 (0 when flag 0x2000000 is set). Up to 0x1800 bytes are read.

| offset | type | goes to |
|---|---|---|
| 0 | u8 | game state +1 and +2 = **start index** in the level start table (`HERO_Init` 0x15090 reads 0x5a69c[level] + 0x18·index). `DEMO1_1`/`1_2`/`1_3` (1, 3, 6) all play TRAINING from different starts. |
| 2 | u16 | OR'ed into the game flags at +0x30 (0 in every file) |
| 4 | u8 | game state +0xe (0 in every file) |
| 6… | u16 per tick | pad words |

The level itself is chosen before the demo is read (the map opens first in every trace).

## Playback (ENG1 0x6005, once per tick)

The word index starts at 3. Each tick: `prev = word[i]`, `i++`, `held = word[i]`, `pressed = (prev ^ held) & held` → game state +0x5c (held) / +0x5e (pressed), +0x60 = 0.

Playback stops at index 0x374 (884 ticks, 29.5 s) and sets game state +3 = 0x13 (back to the title). The same happens when the real pad had input that tick.

## Buttons

These are the EXE pad bits (keyboard map 0x7f30):

| bit | key | hero action (default slots, `gameplay-spec.md`) |
|---|---|---|
| 0x10 / 0x40 / 0x80 / 0x20 | arrows | up / down / left / right |
| 0x8000 | X | sword |
| 0x4000 | Z | jump |
| 0x2000 | C | punch |
| 0x1000 | D | charge |
| 0x800 | S | – |

`DEMO1_1`: holds Right, jumps with Z (7 presses), swings the sword with X (5 presses). The trace shows the same take-offs (vy 14.0) and sword states 12/43.
