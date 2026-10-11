# Gameplay specification (ENG1.DLL)

ENG1 RVAs (base 0x10000000). Units: world units (a cell is 64), positions and velocities in 16.16 fixed point; one tick = 33 ms (EXE `RT_Tick`, fixed 30 Hz; confirmed by the hook tick counter: one tick per present, 30.33/s). Constants in their original form are in `reference/tuning.json`. Runtime check: `tools/state/trace_state.py` → `analysis/traces/hero_state.csv` (per tick x, y, z, vx, vy, flags, state, frame during the attract demo).

## Frame loop (`MOD_Tick` 0x20300, playing branch)

1. demo input (0x6005)
2. world (0x2180)
3. block streaming (`LVL_StreamBlocks`)
4. 0x5600
5. if not paused: 0x2fb70, `LVL_UpdateCamera` 0x2f840, object update 0x258e0 (three pools), 0x34d0, **hero update 0x156a0**, 0x1fd90, 0x25d00
6. draw (0x3470, 0x5d50, 0x3a50, 0x32bd0, 0x2da90)
7. VRAM uploads (`LVL_AnimateTextures`, `ANIM_UploadCells`, 0x58b0)
8. frame counter (game state +0x38)

## Input

The EXE maps keys to pad bits (EXE 0x7f30: arrows 0x10/0x40/0x80/0x20; X 0x8000, Z 0x4000, C 0x2000, D 0x1000, S 0x800). Game state +0x5c holds the held buttons and +0x5e those newly pressed this tick. Action slots (game state +0x6c…+0x7a, defaults from 0x4d958):

| slot | default | use in the hero code |
|---|---|---|
| +0x6c | 0x8000 (X) | sword (0x17290 → swing script 0x541f8; script ops 0x0d/0x14/0x15/0x19 test it) |
| +0x6e | 0x4000 (Z) | jump (variable height while held, 0x171b0) |
| +0x70 | 0x2000 (C) | punch (0x17290 → script 0x540c0; ops 0x11/0x13) |
| +0x72 | 0x1000 (D) | charge / power (op 0x16 builds 0x72194 up to 0xff while held) |
| +0x74…+0x7a | 0x400, 0x800, 0x100, 0x200 | other actions |

## Hero object (pool C slot 0, pointer 0x72860; 0x134 bytes)

| offset | meaning |
|---|---|
| +0x00, +0x04, +0x08 | x, y, z (16.16) |
| +0x22 | sprite bank (1 = HERCULES; scripts can switch it, op 0x27) |
| +0x30 / +0x32 | script timer / script position |
| +0x34 | frame |
| +0x54 | draw flags (0x10 = mirror) |
| +0x64 | animation script pointer |
| +0x94 | facing (0 = right, 0xff = left) |
| +0x95 | vertical direction (0xff = rising) |
| +0x9c…+0xa4 | previous x, y, z |
| +0xa8 | x speed |
| +0xac | target x speed |
| +0xb0 | acceleration |
| +0xb4 | deceleration |
| +0xb8 | vertical speed |
| +0xbc | target / maximum vertical speed |
| +0xc0 | gravity while falling |
| +0xc4 | deceleration while rising |
| +0x108 | air-control target speed |

State id: 0x72108; handler table 0x72040, filled by `HERO_Init` 0x15090. Per tick, 0x15780 handles flashing, invulnerability, input masking and the cheat fly (flag 0x8000 with buttons 0xa000), then calls `table[state]`.

## Animation scripts (`HERO_AnimStep` 0x15d00)

Each script is a list of `{s16 a, s16 b}`. a ≥ 0 shows frame a for **b + 1** ticks (the timer counts down past 0). a < 0 is opcode a + 0x34 with argument b:

- 0x33 loop (back to the script start or the last 0x32), 0x32 loop point (the next entry), 0x31 stop;
- 0x17 SFX b, 0x00 footsteps;
- 0x22/0x23 spawn effect, 0x2e flip, 0x30 turn around;
- button-conditional branches that switch state.

Entering a state sets +0x64 to a script and resets +0x30/+0x32 to −1. All 56 hero scripts are decoded in `extracted/sprites/0560_SRC1_HERO_HERCULES/hero_scripts.json` (`tools/state/hero_anim.py`).

## States

| id | name | entry (setter) | script(s) → frames | physics / rules |
|---|---|---|---|---|
| 10 | init | `HERO_Init` 0x15090 | – | start position from the level start table 0x5a69c[level] + 0x18·start index (game state +1; demo byte 0): x = (u16 − 0x20)·65536, y = (u16 + 0x40)·65536, z = byte << 23 |
| 0 | idle | 0x16790 | 0x53820 → random idle (op 0x2d: 0x538d0 / 0x53930 / 0x53878 / 0x53830, chosen by `RT_RandMod(20)` and a wall test) | frames 1–8 breathing, fidgets 272…, 536…, 563… |
| 1 | run | 0x16a60 | 0x53a20 (2, 3 once; loop 10–15, 4–9, 2 ticks each, footstep SFX at 10 and 4) | target speed 8.0 (0x80000), acceleration 0.5 (0x8000); speed steps toward the target each tick (`HERO_MoveX` 0x19a10) |
| 3 | stop / stance | 0x16c40, 0x16cf0 | 0x53b90 (361–365) | entered when x speed ≥ 4.0 (0x40000) at release, else idle; speeds cleared |
| 6 | turn around | 0x16e70 | 0x53c00 / 0x53c18 / 0x53c40 / 0x53c80 | |
| 4 | jump take-off | 0x17000 | 0x53cc0 (455–464) | `HERO_JumpStart` 0x17130: vy = 14.0 (0xe0000) rising, rise deceleration 0.875 (0xe000), fall gravity 0.4375 (0x7000), max fall 13.0 (0xd0000); air target 4.0 (0x108 = 0x40000), acceleration 0.5 |
| 2 | jump | 0x17060 | 0x53d10 (85–93) | rising: vy −= 0.875 per tick; jump button released → extra −0.75 (0xc000) per tick (variable height); at vy < 0 → falling with gravity 0.375 (0x6000) capped at 13.0, state 7 (`HERO_AirPhysics` 0x19d70) |
| 7 | airborne / fall | 0x17340, 0x19d70 | 0x53df8 (461–464) | walking off a ledge: target 4.0, deceleration 0.5; lands on a surface within max(\|dx\|, 4) + 8 below (0x29da0, `LVL_FindSurface`) |
| 8 | land | 0x176e0 | 0x53da0 (18–24) | x speed and air target cleared |
| 11 | crouch | 0x17a50 | 0x547e8 (120, 121) | Down (0x40) |
| 24 | air down move | 0x19d70 | 0x53e28 (392–395) | Down held at the apex of a take-off: gravity 1.0 (0x10000), max 32.0 (0x200000) |
| 5 | roll | 0x178f0 | 0x548e8 (26–28, 145–139) | air target 4.0 |
| 23 | fell below the section | 0x170c0 | 0x53cc0 | when y passes rows·64 (`HERO_AirPhysics`): y clamped, vy = 8.0 |
| 16 | ledge somersault | 0x18900 | 0x54680 (383–388) | 0x188a0: grab point found ahead/above (0x30010 at x ± 0x24, y − 0x30) |
| 19 | forced walk | 0x174e0 | – | x speed fixed at 3.0 (0x30000) |
| 12, 43, 44, 45 | sword | 0x17ff0 | 0x54120, 0x54260/0x542a0 (211–216), 0x543d0 (339–350) | sword button (+0x6c) |
| 46 | sword recovery | script op 0x0d/0x14/0x15 | 0x541d8 / 0x54470 / 0x541b0 (344…339) | released sword button |
| 41, 42 | punch combo | 0x17ba0 | 0x53ea0 (32–51), 0x53f80 (313–320) | punch button (+0x70) |
| 20 | lift / throw | 0x18b10 | 0x544c0 (bank `HC_THROW`) | target object type 2, 4 or 5 (0x7210c) |
| 17 | strength actions | 0x18c50 | 0x546d8, 0x54718, 0x54750, 0x54798 | |
| 22 | lose a life | `HERO_Die` 0x18f60 | 0x54930 / 0x549f0 / 0x54aa0 (picked by free space left/right) | lives (game state +0xc) − 1, deaths +0xd + 1; no lives → game state +3 = 1 (game over); respawn at start index +1 = +2 |
| 25 | end / death sequence | 0x19160 | 0x54b50, 0x54c30 (`HC_DIE` 1–8, 7–8 loop) | |
| 27 | cheat fly | 0x15780 | – | flag 0x8000 + buttons 0xa000 |

Runtime (TRAINING demo, `hero_state.csv`):
- top |vx| = 8.000 and jump take-off vy = 14.000, matching the constants above;
- full jumps rise 95–105 units in 14–16 ticks (theory without release: 16 ticks, 119 units);
- frames per state match the scripts (state 1: 2, 3 then 10–15 / 4–9; state 2: 85–90; state 8: 18–24).

## One enemy: Stymphalian bird (TRAINING type 0x38, bank `ALIEN2\STYBIRD`)

The type table comes from the level config (0x68af0 record +0x1c; 12 bytes per type: `[spawn, stub, update]`). For type 0x38 the spawn is 0x433d0 and the update 0x43440.

- **Spawn 0x433d0:** depth layer = (z + 0x40) >> 7; flags 0x202; behaviour state (+0x2a) = 0x21; frame setup through 0x292f0/0x29270/0x283e0.
- **Update 0x43440:** a switch on +0x2a (states 2–0x2d).
  - In states 2/0x27 the bird keeps 64–128 units above the hero. If less than 64 above, y −= 6.0 (0x60000); if more than 128 above, y += 6.0. Then 0x43d80 steers horizontally.
  - A cell attribute of 0x1a (`LVL_CellAttribute`) means the hero is out of reach (vertical distance treated as 0xff).
  - Unknown states fall back to state 1.

## Open

- The exact dispatch offset of the object type tables. The `[spawn, stub, update]` reading comes from behaviour.
- Damage from enemies: hit boxes are world AABBs {x0,y0,z0,x1,y1,z1} (frame box record via 0x30e40, placed by 0x2a880, overlap test 0x2a7a0; hero box at hero+0x88). Objects call `HERO_RequestDamage` 0x266a0 (strongest hit per tick; returns invulnerability timer 0x72008). How `HERO_Update` turns the hit into a state is still open.
- States 9, 13, 14, 28, 47 (seen in code, not in the demo).
