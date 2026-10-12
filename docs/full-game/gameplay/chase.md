# ENG3 hero "runner" model (S3.07): GAUNTLET (2), CYCLOPS (8), TORMENT (10)

Status: everything is **VERIFIED-STATIC** (Ghidra decompilation/disassembly of ENG3.DLL, base 0x10000000) unless it is listed under Open. RVAs are given without the base. The values are 16.16 fixed point unless marked otherwise. One tick = 1/30 s (as in ENG1). Hero = pool A slot 0 (`DAT 0x42540` = `DAT 0x41d54`, 0x134 bytes). gs = game state (`DAT 0x407a4`).

## Hero fields used (same layout as ENG1 where the code matches)

| off | meaning | off | meaning |
|---|---|---|---|
| +0x00/+0x04/+0x08 | x (forward, along the track) / y (down is +) / z (lane/depth) | +0xa8 | forward speed |
| +0x34 | frame | +0xac | target forward speed |
| +0x38 u16 | section (start record +5) | +0xb0 / +0xb4 | forward accel / decel |
| +0x64 | anim script ptr (+0x30 timer, +0x32 pos) | +0xb8 / +0xbc | vy / max vy |
| +0x94 | facing (0 = +x, 0xff = −x) | +0xc0 / +0xc4 | gravity (falling) / decel (rising) |
| +0x95 | 0xff = rising | +0xc8 / +0xcc | lane speed / target |
| +0x96 | lane sign (0: z −= lane speed) | +0xd0 / +0xd4 | lane accel / decel |
| +0x116 u16 | hit code (set by obstacles/objects) | +0xec u8 | invulnerability timer |
| +0x110 | hitter object ptr | +0xee s16 | dash timer (900 at init) |
| +0xf0 | extra x push added every step | +0x126 u16 | alternative death (crush) flag |

## Per-tick order (`HERO_Tick` 0x174e0)

1. Section history: if hero+0x38 > max (0x3fb04), it is appended to the list 0x41cc0 (used for the completion tally).
2. Camera distance 0x40640 moves toward the target 0x3803c by step 0x38040 = 4. Targets are 0x140 (320) normally, 0x168 on UP and 0xf0 on DOWN (State01). `HERO_Init` sets 0x40640 to 0x140.
3. Active blocks from the section (0x40638): primary 0x41c64, alternative 0x41c68 (999 = none).
   - CYCLOPS: section 0→(0,1), 1→(3,4), 2→(6,7).
   - GAUNTLET: section n→(n,999) for n = 0–7; sections 8–10→(7,999).
   - TORMENT: 0→4, 1→0, 2→1, 3→2, 4→3 (all with alternative 999).
4. Debug actions (gs+0x65 bits, open): &4, &1 (+ gs+0x10 = 1) and &2 (+ gs+0x10 = 0) call `HERO_Die` 0x19a30 when gs+0x30 & 0x4000. &8 calls `HERO_LevelComplete` 0x19c20 when gs+0x30 & 0x2000.
5. Invulnerability: +0xec − 1 per tick. At 0, +0xed is cleared.
6. Trigger at the hero (0x1ecf0 → 0x22b30).
7. 0x180e0 (pre-step, below).
8. 0x19130: flashing while +0xec is set (brightness table 0x38148).
9. `state[0x41cb4 & 0x7fff]()`.
10. CYCLOPS lane clamp, then the HUD (0x17210) and the 8-object effect (0x16f30). Neither of these moves the hero.

**0x180e0 pre-step:**
- `0x1a290`: newly pressed 0x400 / 0x800 cycles `DAT 0x431e4` through 0–3 (meaning open).
- Saves the previous x/y/z (+0x9c..+0xa4).
- Cheat fly entry (see state 7).
- Surface reactions (0x17ae0).
- Current block 0x40639 = 0x3fb9c.
- Track spline 0x1b650 gives the camera height: 0x40620 = 0xe8 − spline[4] (0x18350).
- Dash timer +0xee − 1.
- **Side clamps** while x < 0x3000000 (768.0):
  - GAUNTLET: sections 2/4/6 keep z ≥ 0x200000 (+32); sections 1/3/5 keep z ≤ −0x200000 (−32).
  - TORMENT: section 2 keeps z ≥ +32; section 1 keeps z ≤ −32.

## States (table 0x41c80, filled by `HERO_InitStates` 0x178a0, initial state 0)

| id | handler | meaning | entered by (setter RVA) | script |
|---|---|---|---|---|
| 0 | 0x18600 | stand / wait at the start | init; 0x185c0 (from 7, 9) | 0x37df0 |
| 1 | 0x18950 | **run (forced forward)** | 0x18690: from 0 (UP/DOWN), landing (3), end of hit (5), end of dash (8) | 0x37c38 |
| 2 | 0x18eb0 | jump (rising) | 0x18de0 via 0x18dc0 (jump newly pressed) from 1 and 8 | 0x37e08 (0x37fe8 while dashing) |
| 3 | 0x19030 | fall / airborne | 0x18f70 (no ground in 0/1/8/10, exit of 7); apex in 0x1a180 | 0x37e48 |
| 4 | 0x02390 | empty stub (also called at the end of every forward step 0x19f30) | – | – |
| 5 | 0x195d0 | hit / knock-back | 0x19200 (obstacle probe 0x17ae0 and ≈ 48 object callers) | 0x37f50, or 0x37f88 if airborne (0x3fb24) |
| 6 | 0x19b70 | dying | `HERO_Die` 0x19a30 | 0x37e58, or 0x37e98 if +0x126 |
| 7 | 0x19d40 | cheat fly | 0x180e0 (gs+0x30 & 0x8000 and held 0x0c00) | – |
| 8 | 0x18880 | dash (power-up) | 0x18790 from 1 when 0x3fb20 ≠ 0 | 0x37fe8 |
| 9 | 0x19cd0 | level complete | `HERO_LevelComplete` 0x19c20 | 0x37ea0 |
| 10 | 0x18c90 | ride / carried (input off) | 0x18bc0 from 0/1 when 0x3faf0 ≠ 0; also 0x16240 (object touch) | 0x37ca0 |

The animation step 0x18370 is ENG1's `HERO_AnimStep` format with opcodes:
- −3 (returns 1 = script ended);
- −9 / −5 (branch unless UP 0x10 / DOWN 0x40 is held);
- −8 (gs+3 = 0x21);
- −7 (fade);
- −4 (turn, snaps the x sub-cell);
- −0x28 / −0x27 (SFX).

### State details

- **0 stand (0x18600):**
  - No ground (`0x1e8e0` = 0) → state 3.
  - Otherwise `0x18630`:
    - 0x3faf0 → state 10;
    - UP (0x10) → state 1 with facing 0;
    - DOWN (0x40) → state 1 with facing 0xff.
  - No forward motion in this state.
- **1 run (0x18950):**
  - 0x3faf0 → state 10.
  - Clamp y ≤ 0x2000000 (512).
  - Health gs+0x24 < 1 and not (gs+0x30 & 1 and & 2) → `HERO_Die`.
  - Jump: `0x18dc0` = (gs+0x6e & newly pressed gs+0x5e) → state 2. This check is skipped (no jump) in these end zones:
    - GAUNTLET: x ≥ 0x39800000 (14720) in sections 7–10;
    - CYCLOPS: x ≥ 0x74800000 (29824) in section 2;
    - TORMENT: x ≥ 0x71000000 (28928) in sections 3–4.
  - If 0x40614 < 0x4e9, the script pointer is reset to 0x37c38 every tick. 0x40614 is 0 at `HERO_Init`; no writer was found.
  - Target speed:
    - default `0x38038` = 0x000a0000 (**10.0**);
    - UP held: 0x100000 (16.0), script 0x37cd8, camera target 0x168;
    - DOWN held: −0x40000 (−4.0), script 0x37ca0, camera target 0xf0; once camera 0x40640 < 0xf1, the target is 0x50000 (5.0) instead;
    - GAUNTLET with y > 0x1e10000 (481 = water level 0x410e0): target >> 1;
    - 0x3fb2c set: target = 10.0 >> 1 = 5.0.
  - Steps: forward step 0x19f30, lane step 0x1a070. No ground → state 3. Animation.
  - 0x3fb20 (dash) → state 8.
- **2 jump (0x18eb0):**
  - Target speed −0x100 per tick.
  - DOWN: target 0xfffa0000 (−6.0), or 5.0 once the camera is ≤ 240.
  - Lane input, forward step and lane step.
  - Jump button not held: vy −= 0x10000 (1.0)/tick, floor 0 (variable height).
  - Vertical step 0x1a180, then the surface snap 0x1ea60 (no state change).
- **3 fall (0x19030):**
  - DOWN gives the same −6.0/5.0 targets.
  - Lane input, forward, lane and vertical steps.
  - Landing test 0x1eaf0. On landing:
    - SFX 0x6037 unless this is the first landing (0x3fb68);
    - GAUNTLET with y > 0x410e0·65536 (481): SFX 0x6036;
    - then state 1.
- **5 hit (0x195d0):** the reaction comes from the hit code +0x116. "Away" means the z side away from the hitter's z (hitter = +0x110 → +8).

  | +0x116 | reaction |
  |---|---|
  | 4, 0x23, 0x2e, 0x33, 0x40, 0x24, 0x30, 0x32, 0x28, 0x2f, 0x34 | target 0x80000 (8.0) and the forward step (keeps running) |
  | 2, 0x25, 0x36, 0x31, 0x100, 0x200 | x −= 1.0, z ∓ 0x30000 (3.0) away |
  | 8 | z ∓ 3.0 away (x unchanged) |
  | 0x29, 0x2b | x −= 1.0, z ∓ 0x10000 (1.0) away |
  | 0x35 | z + 0x38000 (3.5) or − 3.0, from the relative position (0x1420) |
  | 0x10 / 0x20 | x −= 1.0, z −/+ 0x20000 (2.0) |
  | 0x80 | as 0x20 if z ≥ 0x3fb78·65536 (cell base + 64, from 0x19200), else as 0x10 |
  | 0x21, 0x22, 0x26, 0x27, 0x2a, 0x2c, 0x2d, 0x400, 0x800 | x −= 1.0 |

  After the reaction:
  - Clamp y ≤ 512.
  - No ground: vy 0x40000 (4.0), max 0x80000 (8.0), gravity 0x28000 (2.5), falling.
  - Script end → state 1.
- **6 dying (0x19b70):**
  - gs+5 = 1.
  - Vertical step (falls).
  - Script position > 10 → fade (gs+0x1b30 = 4).
  - At the script end: gs+3 = 0x22 if lives gs+0xc = 0 (game over), else 0x21 (restart).
  - Snaps to the surface (0x1eeb0) when +0xe0 and +0xdc = 0.
- **7 cheat fly (0x19d40):**
  - Entry sets target and max vy to 0x400000 (64.0), or 0x100000 (16.0) if 0x400 is held; accel 0x10000 (0x180e0).
  - Held 0x1000 → rise; held 0x4000 → descend (`HERO_VerticalStep`).
  - UP / DOWN: forward / backward step.
  - LEFT: z −= 0x40000 (4.0); RIGHT: z += 4.0.
  - Exit: 0x0c00 released after ≥ 6 ticks → state 3 if there is a surface, else state 0.
- **8 dash (0x18880):**
  - Entry 0x18790: speed 0xc0000, decel 0x6000, +0xd8 = 8.
  - Every tick: target **0x180000 (24.0)**, accel 0x60000 (6.0).
  - The jump is allowed. Lane input; forward, lane and ground steps.
  - Frame 0x4a: loop the script while +0xee > 0, else jump to position 0xd.
  - Frame 0x4b or timer ≤ 0 → 0x3fb20 = 0, HUD event 0x3fae8 = 2, state 1.
- **9 level complete (0x19cd0):**
  - Script position > 0x1e → fade.
  - At the script end: state 0, gs+3 = **0x31**.
- **10 ride (0x18c90):**
  - Target speed: GAUNTLET 0x10000 (1.0), CYCLOPS 0x50000 (5.0), TORMENT 0x10000.
  - Entry 0x18bc0: speed 1.0, target 1.0.
  - Forward step and ground check (→ state 3).
  - No lane input, no jump.

## Forced-motion and speed model

- **Forward step 0x19f30** (all levels):
  - speed (+0xa8) moves toward the target (+0xac): + accel (+0xb0) when below it, − decel (+0xb4) when above it, clamped at the target;
  - x += (facing 0 ? speed + push (+0xf0) : push − speed).
- **Run entry 0x18690:**
  - speed **0xc0000 (12.0)**, target `0x38038` (10.0);
  - accel 0x60000 (6.0, so UP reaches 16.0 within a tick);
  - decel 0x6000 (0.375 per tick).
  - Lane speeds and vy are cleared.
- The hero is always pushed forward in states 1/2/3/8/10. There is no "stop" except state 0 (start) and DOWN, which only lowers the target to −4.0 and then 5.0.
- **Lane (depth z) step 0x1a070:**
  - lane speed (+0xc8) moves toward +0xcc (accel +0xd0 / decel +0xd4);
  - z −= lane speed (+0x96 = 0) or z += it.
- **Lane input 0x18d00:**
  - LEFT (0x80) sets the lane speed and its target to 0x40000 (z −4.0/tick); RIGHT (0x20) sets them to 0xfffc0000 (z +4.0/tick);
  - neither held → 0;
  - wall flags 0x3fb40 (LEFT) / 0x3fb44 (RIGHT) zero the lane speed and are cleared.
  - Movement is immediate, with no ramp.
- **Vertical 0x1a180:**
  - Falling (+0x95 = 0): vy += gravity, capped at max vy; y += vy.
  - If y > rows·0x800000 (block rows u16 at 0x40744[0x40638], written by `LVL_ParseHeader` 0x1b031; 12 rows → 1536.0): y is clamped and vy = 0x80000.
  - Rising: vy −= +0xc4; y −= vy. When vy < 0: falling, max vy 0x100000, gravity 0x18000 (1.5), state 3 (unless state 6).
  - `0x1a110` clamps y afterwards (below).
- **Jump 0x18de0:**
  - vy **0xba000 (11.625)** rising;
  - rise decel 0xa000 (0.625);
  - gravity 0x6000, which becomes 0x18000 at the apex;
  - max vy 0x100000 (16.0).
- **Walk-off fall 0x18f70:** vy 0x40000 (4.0), max 0x100000; gravity 0x28000 is written and then overwritten with **0x6000 (0.375)**; decel 0xa000.
- **Floor clamps 0x1a110:**
  - GAUNTLET: y ≤ 0x2000000 (512) always;
  - CYCLOPS: y ≤ 0x1c00000 (448) only when gs+0x30 & 7;
  - TORMENT: y ≤ 0xc80000 (200) only when gs+0x30 & 7.
  - State 1/5 also clamp y ≤ 512 in every level.
- **Ground:** `0x1e8e0` (stand test, tolerance max(|dx|, 4) + 2…4 + |dz|), `0x1eaf0` (landing, tolerance uses 0x431e0), `0x1ea60` (snap) via `LVL_FindSurface` and the platform list (0x1e830/0x1e540).
  - 0x431e0 = 8 GAUNTLET, 0x10 CYCLOPS, 2 TORMENT (`HERO_Init` 0x1c720).

## Controls (gs+0x5c held, +0x5e newly pressed)

| input | effect |
|---|---|
| UP 0x10 | state 0 → run. In run: target 16.0 and camera 0x168 |
| DOWN 0x40 | state 0 → run facing 0xff. Run: −4.0 then 5.0. Jump/fall: −6.0 then 5.0. Camera 0xf0 |
| LEFT 0x80 / RIGHT 0x20 | lane z −4.0 / +4.0 per tick (states 1, 2, 3, 8) |
| jump slot gs+0x6e (0x4000) | newly pressed → jump (states 1, 8); released while rising → −1.0 vy/tick |
| sword +0x6c, punch +0x70, slot +0x72 | **not read** by any hero state or helper listed here |

## Obstacles, damage, death

- **Look-ahead probe (0x17ae0, every tick):** cell attribute via `0x1ecf0` (trigger objects 0x41200, then cell attribute 0x1b4a0) at:
  - CYCLOPS/TORMENT: x + 0x800000 (128), z ± 0x100000 (16, on the side of z's sign);
  - GAUNTLET: x + 0x400000 (64), z ± 0x40000 (4).

  | code | effect |
  |---|---|
  | 0x52 | block RIGHT + hit 0x20 |
  | 0x56 | block RIGHT + hit 0x10 |
  | 0x55, 0x5a | block RIGHT if z > 0, else block LEFT |
  | 0x5d | block RIGHT |
  | 0x5e | block LEFT |
  | 0x7a, 0x7b | block both |

  - A second probe at x + 0x600000 (96): code 0x73 with y > 448 → hit 0x80.
  - CYCLOPS with z < 0: probe (x + 128, z + 16). Code 0x56 → block RIGHT, plus hit 0x10 if (x & 0x7f0000) > 0x200000. Code 0x5d → block RIGHT.
- **Triggers at the hero position (same function):**

  | code | effect |
  |---|---|
  | 0x50 | 0x3fb9c = alternative block |
  | 0x51 | 0x3fb9c = primary block |
  | 0x64 | 0x3fb2c = 1 (half speed and **no death**) |
  | 0x65 | clear 0x3fb2c |
  | 0x76 | speed and target 0x70000 (7.0); spawns type 7 once (+0xdc) |
  | 0x75 | spawns type 0x41 once if y ≥ 0x1f40000 |
  | 0x63, 0x77 | when y ≥ 0x7c0000: spawn a pool A object at y − 0x500000 once, then 0x284f0. If y > 0xa9ffff (170): x snapped to 128, +0x126 = 1, 24 debris objects, `HERO_Die` |
  | 0x53, 0x54, 0x57, 0x5b | set the state of object 0x3fb0c (0x42/0x44/0x41/0x43) |

- **Hit entry 0x19200:**
  - While dashing, codes 0x22/0x26/0x32/0x36 are ignored. Any other hit ends the dash.
  - Ignored in states 6/7, and for code 1 with (x & 0x7f0000) < 0x400000.
  - If +0xec = 0 and gs+0x30 & 7 = 0: damage (0x19410). Health < 1 (no cheat 1 & 2) → `HERO_Die`. Else invulnerability +0xec = 0x40 (64 ticks).
  - While invulnerable, codes 4/0x30 are ignored. Every other code still enters state 5.
- **Damage 0x19410:** health gs+0x24 (starts 0x80 = 128, 0x178a0) −= table 0x380a0[difficulty & 3]. The table is u32 entries, 0x20 per difficulty; columns T0–T4 at +0/+4/+8/+0xc/+0x10.
  - SFX 0x602e, or 0x602d if airborne; none for codes 0x10/0x20.

  | diff | T0 | T1 | T2 | T3 | T4 |
  |---|---|---|---|---|---|
  | 0 Beginner | 10 | 15 | 20 | 25 | 30 |
  | 1 Medium | 15 | 20 | 25 | 30 | 35 |
  | 2 Herculean | 25 | 30 | 35 | 40 | 45 |

  | codes | damage (jump table 0x1957c) |
  |---|---|
  | 4, 0x22–0x25, 0x27, 0x34, 0x35 | T0 |
  | 0x21, 0x26, 0x29, 0x2b–0x2d, 0x80 | T1 |
  | 0x28, 0x2e–0x33, 0x36 | T2 |
  | 2, 0x200, 0x400, 0x800 | T3 |
  | 0x100 | T4 |
  | 0x40 | T1 + T0 |
  | 0x2a | T1 + T1 + T0 |

  Codes 1, 8, 0x10 and 0x20 deal no damage.
- **`HERO_Die` 0x19a30:**
  - Does nothing while 0x3fb2c ≠ 0.
  - gs+5 = 1; dash and invulnerability are cleared.
  - Lives gs+0xc − 1 and deaths gs+0xd + 1 (skipped if gs+0x30 & 8). Lives 0 → gs+3 = 1.
  - Speeds are zeroed; gs+1 = gs+2 (restart); state 6.
- **Pit object** (type table entry 0x3406c → 0x59d0): moves the hero to the object and calls `HERO_Die`.

## Completion

The end object update 0x16320 is in the type tables at 0x33db4, 0x3633c and 0x3a654, one per level. Its touch handler 0x16240 starts the ride: state 10, 0x3faf0 = 1, and the object copies the hero speed.

- **The object:**
  - Slows by 0x2000 (GAUNTLET/CYCLOPS) or 0x4000 (TORMENT) per tick.
  - Moves its z toward 0 (GAUNTLET/TORMENT) or −0x400000 (CYCLOPS) by 2.0 per tick; the hero z follows it while the run, fast or slow scripts play.
- **Completion:** `HERO_LevelComplete` (once, by flag) when the hero x ≥:
  - GAUNTLET 0x3c000000 (15360.0);
  - CYCLOPS 0x77800000 (30592.0);
  - TORMENT 0x74000000 (29696.0).
- GAUNTLET/TORMENT also set gs+3 = 2 once (meaning open).
- **0x19c20:**
  - State 9, speeds 0, gs+1 = gs+2.
  - For each visited section (list 0x41cc0), it adds the u16 values 0x40c40[s·2] → gs+0x16 and 0x40c44[s·2] → gs+0x1a (bonus tally).

## Difficulty

In the hero path, gs+0x0e is read only by the damage table above (0x19410). Speeds, jump and lane values do not depend on difficulty.

## Per-level differences (gs+0 checks)

| item | GAUNTLET (2) | CYCLOPS (8) | TORMENT (10) |
|---|---|---|---|
| start y forced to 0x1c00000 (448) (0x178a0) | yes | yes; also +0x1c = 10 | no |
| start z (`HERO_Init`) | record | 0x400000 (64) | record |
| section → block map | n→n | pairs (0,1), (3,4), (6,7) | 4, 0, 1, 2, 3 |
| probe distance | x + 64, z ± 4 | x + 128, z ± 16 (+ the extra z<0 probe) | x + 128, z ± 16 |
| side clamps (x < 768) | sections 1–6 | – | sections 1, 2 |
| lane clamp to the track spline (0x1b650, block, 0x19), when block = alternative | – | yes (`HERO_Tick`, after the state call); 0x3fb2c = 1 while 0x1400001 ≤ y ≤ 0x1c00000 | – |
| water 0x410e0 (speed /2, SFX) | 481 | 600 (unused here) | 600 |
| landing tolerance 0x431e0 | 8 | 16 | 2 |
| ride target speed | 1.0 | 5.0 | 1.0 |
| no-jump zone | x ≥ 14720, sections 7–10 | x ≥ 29824, section 2 | x ≥ 28928, sections 3–4 |
| completion x | 15360 | 30592 | 29696 |
| cheat floor (gs+0x30 & 7) | 512 (always) | 448 | 200 |

## Open

- gs+0x64/0x65 debug bits and gs+0x30 & 0x2000/0x4000 (HERO_Tick call sites 0x1770b, 0x17721, 0x17748, 0x1776a): which keys set them.
- gs+3 = 2 (0x16320, flags 0x37b8c / 0x37b9c) and the codes 0x21/0x22/0x31 consumers (EXE/TITLE side).
- 0x40614 (State01 0x18a86): only read here. If no writer exists, the run script pointer is reset to 0x37c38 every tick.
- Hero +0x96 is never written by the code above. That LEFT means z− depends on it being 0 (check `LVL_AllocPools`/0x1ce50).
- 0x431e4 (0..3 cycled by 0x400/0x800 in 0x1a290): who reads it (probably the camera).
- Which objects set +0x116 and +0x110 (callers of 0x19200, e.g. 0x2440, 0x2ac0, 0x10590). The surface code list in 0x22b30 (table 0x40c38) is only partly mapped.
- Power-ups: 0x15680 (object type 3, dash timer +0xee = 0x56) and 0x4a40 (timer 10, health + 2). The full pickup list is still open.
- What the 0x3fb2c "half speed / no death" zone is in the game (0x64 trigger; CYCLOPS y band).
