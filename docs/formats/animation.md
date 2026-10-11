# Sprite animation

## How the game animates (ENG1.DLL)

- There is no sequence table in the sprite banks. **Hercules is driven by animation scripts in ENG1 data**: a state-entry routine sets `obj+0x64` to a script and `HERO_AnimStep` (0x15d00) steps it (`{s16 frame, s16 ticks}` entries plus opcodes for loops, SFX, effects and branches; see `docs/gameplay-spec.md`). All 56 hero scripts are decoded by `tools/state/hero_anim.py` → `extracted/sprites/0560_SRC1_HERO_HERCULES/hero_scripts.json`. Simpler entities set the frame number directly (e.g. ENG1 0x1e320 picks frame 0x40, 0x42, 0x44 or 0x45 at random); frame writes appear in about 60 ENG1 functions.
- Per tick, the object list is drawn through 0x3bd0, then 0x3cd0. Object fields used:

  | offset | meaning |
  |---|---|
  | +0x0c/+0x0e | screen x/y |
  | +0x1c | palette (0xff = the frame entry's palette) |
  | +0x22 | bank (ENG1 bank table 0x5cee0) |
  | +0x34 | frame |
  | +0x36 | second frame, drawn when flag 0x8000000 is set (Hercules' overlay layer) |
  | +0x54 | flags; 0x10 = mirror horizontally (dx and part x are negated, 0x4010) |

- Frame data, pivots and boxes come from the bank (`i-family.md`). The pivot is the frame's `(dx, dy)` + part positions relative to the object x/y; the decoder stores it as `origin` per PNG.
- A tick is 33 ms (`RT_Tick`, 30 Hz). Frames last 1–5 ticks in the traces.

## Recovered data

`tools/decode/anim.py` builds `extracted/sprites/<bank>/anim.json` from the hook's `anim` events (runs `demo_anim`, `demo_pal`, `attract7` = TRAINING demos `DEMO1_1`…`1_3`):

- `frames`: ticks shown (median/min/max), pivot, dx/dy, boxes (raw 6 bytes);
- `clips`: per object, the frame timeline with ticks, x/y, flip, palette;
- `transitions`, `loops` (strongly connected groups) and `sequences` (mutually dominant successor chains);
- `labels`: named sequences from `tools/decode/anim_labels.json`. Labels were given by looking at the decoded frames, plus the bank name for death.

`tools/viewer/anim_viewer/build.py` writes `viewer.html` per bank (open from disk). It plays labelled sequences and observed clips at 33 ms per tick, with the pivot marked.

## Hercules (bank 560 `SRC1\HERO\HERCULES`, 593 entries, plus `HC_THROW` 761, `HC_DIE` 765)

| label | frames | ticks per frame (median) |
|---|---|---|
| run | 2, 3 (start), 10–15, 4–9 (cycle) | 1, 1, then 2 |
| idle | 465–468 (loop) | 2 |
| jump | 85–93 | 3, 3, 3, 2 … |
| jump_somersault | 383–391 | 3 … 5 |
| flip | 295–302 | 2 |
| roll | 26–28, 145–139 | 2 |
| land_crouch / crouch | 18–24 / 120–122 | |
| attack (sword) | thrust 211–217, 246–247; lunge 188–191, 344, 343, 340, 339; swing 428–431; short 186–187; raise 252–254, 235–238 | |
| air_down_move (state 24, script 0x53e28) | 392–396, 400–403 | |
| throws (761) | 1–5, 21–37; 72–87; 38–52, 18–20 | |
| death (765) | 1–8 (7–8 loop) | |
| effects | sword swoosh 504–506, 477–480 | |

Durations are runtime medians (`anim.json` → `labels[].ticks`). The authoritative per-frame tick counts are in `hero_scripts.json` (script data), e.g. jump 0x53d10, run start 0x53a20, land 0x53da0. 126 of the hero's frames were seen in the demos. Moves the demos never show (e.g. other power-ups) need gameplay input, which is P5.

## Open

- The meaning of the 6-byte frame boxes (hit/collision); P5 hero behaviour.
- ENG3 and TITLE use their own copies of the draw code; only ENG1 is hooked (`eng1.dll+0x3cd0`).
