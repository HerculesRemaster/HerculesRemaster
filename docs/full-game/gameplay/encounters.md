# Encounter specifications (S4.04)

One entry per enemy, boss or hazard. Every level plan fills an entry for each of its actors and links it from the per-level index below. Every row carries an evidence label:
- **VERIFIED-STATIC**: decompiled code of this build.
- **VERIFIED-RUNTIME**: trace or capture.
- **LEAD**: walkthrough, manual, names.
- **OPEN**: not yet found.

Addresses are RVAs in the named DLL. Names are in `analysis/annotations/`.

## Template

```
### <actor> (<engine>, level <id/name>, kind 0x....)
| field | value | evidence |
|---|---|---|
| bank / sequences | bank id + path (extracted/meta/full-game/animation-coverage.json), anim sets used | |
| spawn | kind, class (A actor / B 0x2000-0x4000 / C 0x8000), template flags, type-table entry (level config +0x1c/+0x24/+0x2c): spawn, stub, update RVAs | |
| initial state | behaviour state +0x2a, anim set, position rules | |
| states | one row per state: entry condition, anim set (0x292f0 arg), movement (speeds in 16.16 units/tick), exit | |
| telegraphs | frames or states that announce an attack | |
| attack windows | states/frames whose box can hit the hero; hit strength passed to HERO_RequestDamage 0x266a0 | |
| vulnerable windows | when HERO_FindHitSource 0x26de0 (hero punch 0x80, held/thrown objects, projectiles) is checked | |
| damage taken | hit points (+0xee; HERO_EnemyInitHp 0x24270 = difficulty*2 + 6 where used) or one-hit | |
| damage dealt | hero damage = ENG1 0x55348[(hit-1)&7 + difficulty*8] (HERO_ApplyDamage 0x18c50) | |
| phase changes | thresholds and transitions | |
| defeat / exit | defeat state, drops, counters, despawn rule | |
| difficulty guards | every read of game state +0xe in the actor's code | |
| sounds / effects | sfx ids (0x6da0 calls), spawned effect kinds | |
| open items | each with the function to check next | |
```

## Worked example: Stymphalian bird (ENG1, TRAINING level 1, kind 0x38)

| field | value | evidence |
|---|---|---|
| bank / sequences | bank 46 `P:\SRC1\ALIEN2\STYBIRD`, archive 485, extracted `0485_SRC1_ALIEN2_STYBIRD`; in TRAINING start lists 1, 3, 4 and 6. Anim sets 1 (swoop in), 2 (hover), 6 (defeat) | VERIFIED-STATIC (`bank_index.py`; 0x43440) |
| spawn | class A kind 0x38; TRAINING type table 0x67608 + 0x38·12: spawn 0x433d0, update 0x43440 (`HERO_BirdUpdate`). Spawn: depth layer (z + 0x40) >> 7, flags 0x202, state 0x21 | VERIFIED-STATIC |
| initial state | 0x21 (swoop in) from the spawn; state 0x26 waits until the hero is within 72 + rnd(48) units horizontally, then goes to 0x21 with anim set 1 | VERIFIED-STATIC |
| state 0x21 swoop in | speed +0xa8 by frame: frames 6, 7, 8+ set x speed 2.0/2.67/4.0 and y speed −1.5/−3.0/−6.0; frames ≥ 10 x speed 8.0. Horizontal move is blocked when the attribute ahead has flag bits & 7. At the end of the anim: state 2, anim set 2, x speed 8.0 | VERIFIED-STATIC |
| states 2 / 0x27 hover | keeps 64–128 units above the hero (y ∓ 6.0 per tick), steers with `HERO_BirdSteer` 0x43d80. If the cell attribute is 0x1a, the hero counts as out of reach. At the end of the anim: `HERO_BirdChooseAction` 0x43b50 | VERIFIED-STATIC |
| states 0x2b / 0x2c layer change | when the hero is on another depth layer (and random > 0x4000): z ∓ 8 units per tick over frames 8–15, then z snaps to the layer. Blocked when the target cell's attribute flags & 2 | VERIFIED-STATIC |
| states 8 / 9, 0x22 dive / attack | speed ramps (0x8000 per tick up to 8.0). 0x22 accelerates on frames 10–15 and decelerates on 16–20, and ends when +0x114 ≠ 5 | VERIFIED-STATIC; telegraph frames OPEN |
| attack window | every state except 0x23: when the bird's box overlaps the hero box (+0x88, 0x2a710), `HERO_RequestDamage(bird, 0x11, 0)` → strength (0x11−1) & 7 = 0 → hero damage 5 / 12 / 15 (Beginner/Medium/Herculean) | VERIFIED-STATIC |
| vulnerable window | every state except 0x23: `HERO_FindHitSource` 0x26de0. A hero punch (0x80) counts only when 0x19b40 accepts the current attack bits (~0x72168 & 0x5f). A projectile or held object counts always | VERIFIED-STATIC |
| damage taken | one hit (no +0xee hit points) | VERIFIED-STATIC |
| defeat | `HERO_BirdDefeat` 0x43e20: state 0x23, anim set 6, x speed clamped ±3.0, effect kind 0x2027, drop via 0x43ff0, defeated counter 0x70f0e + 1; freed (0x27410) at the end of the anim | VERIFIED-STATIC; drop table OPEN |
| despawn | more than 500 × 0x140 units from the camera (0x2aa30) → freed; state 0x2d falls with gravity +0xc0 until 100 × 0x3c away | VERIFIED-STATIC |
| difficulty guards | none in 0x43440 / 0x43b50 / 0x43e20; difficulty only scales the hero damage | VERIFIED-STATIC |
| sounds / effects | effect 0x2027 at defeat; sfx in the anim scripts (0x283e0 opcodes) OPEN | |
| open items | anim-script sound/event opcodes (level config +0x34 table, 0x283e0); drop table 0x43ff0; telegraph frames of 8/9/0x22 | |

## Nessus (ENG1, FOREST level 3, kind 0x2f) — L03.RE.04

| field | value | evidence |
|---|---|---|
| bank / sequences | banks 68 Basic and 58 Water come with FOREST start 5. The fight requests and frees 69 Punch, 70 Kick, 71 Look, 72 Ride, 73 Reject, 74 Finish and 75 Defeat itself (`ANIM_RequestBank` 0x3520 / `ANIM_FreeBank` 0x3870). Anim sets: 0 idle, 1 turn, 2 walk, 4 defeat, 5 kick, 6 ride, 7 reject, 8 dazed, 9 back-off end, 10 punch, 11 back-off, 12 down | VERIFIED-STATIC (0xbc50) |
| spawn | not on the map. FOREST trigger kind 0x53 (x 18520; `LVL_NessusTriggerSpawn` 0xc780 / `LVL_NessusTriggerTick` 0xc7e0) waits for the Nessus SFX/music stream (`AUD_ForestStreamTick` 0xe670), then `LVL_SpawnNessus` 0xc730 spawns kind 0x2f at hero x + 320 and sets restart index 5. Type entry: `HERO_NessusSpawn` 0xbb10 / stub 0xc720 / `HERO_NessusUpdate` 0xbc50 | VERIFIED-STATIC |
| initial state | 0x37 (entrance, +0x108 = 2 ticks), facing −1, flags 0x202, effect 0x2024 attached, bank 69 requested, boss gauge state 0x72188 = 5 (gauge slides in and fills to 255: `HERO_BossGaugeTick` 0x12040) | VERIFIED-STATIC |
| state 1 decide | hero health ≤ 0 → 0x44. Hero behind → turn (9). More than 80 units away → walk (10, 6.0/tick). Otherwise attack: Punch (0x16, needs bank 69 loaded) or Kick (0x46, needs bank 70), alternating with 0x721b4. Attack count +0x108 = rnd(3) + 1 + **difficulty** | VERIFIED-STATIC |
| attacks 0x16 / 0x46 | at each anim end, if the cell ahead is not a wall (attribute flag 2), the count drops by 1. At count < 1 → back-off 0x44: frees Punch/Kick, requests Look + Ride | VERIFIED-STATIC |
| attack window | every state except 0x23/0x24: when Nessus's attack box (0x2a880 mode 2) overlaps the hero box (+0x88), `HERO_RequestDamage(nessus, 5)` → strength 4 → hero damage 15 / 35 / 45 (Beginner/Medium/Herculean). An attack box that misses the hero but hits a wall cell makes a splash (effect 0x2027, sfx 0x24) | VERIFIED-STATIC |
| back-off 0x44 → 0x45 (ride window) | moves away at 3.0/tick (blocked by walls), then anim 9 with ride flag 0x72120 = 1. If the airborne hero's position is inside Nessus's box (mode 3) during this window (FOREST only; 0x17400), the flag becomes 2 and the hero state 0xd (riding). Otherwise Nessus goes back to state 1 and requests the other attack bank | VERIFIED-STATIC |
| damage taken | Nessus has no hit-source check: punches and thrown objects do nothing ("weapons don't hurt" LEAD confirmed statically). Health is the boss gauge 0x70832 (0..255). While riding (0x41, 2.0/tick forward), the gauge drops by 1 every other tick | VERIFIED-STATIC |
| ride end | 0x41 → reject 0x42 (anim 7; frame 7 throws the hero off: `HERO_NessusThrowOff` 0xbb90, hero ±8.0 x, −3.0 y, falling) → 0x47. Gauge > 0 → state 1. Gauge 0 → dazed 0x43 (anim 8, bank 75, sfx 0x610f) | VERIFIED-STATIC |
| finish | while dazed (sfx 0x110 every 30–59 ticks), only a hero punch with the Power Punch bit (0x72012 & 2) and charge ≥ 101 (0x7082c) whose punch box 0x71440 overlaps Nessus → defeat 0x23 (anim 4, frees Finish, sfx 0x10f via 0x7f8e, spawns kind 0x54: 0xc8c0/0xc910) → end flag 0x72170 (0x19240) → 0x25 down | VERIFIED-STATIC |
| hero defeat | hero health ≤ 0 → Nessus goes to 0x44 (back-off/laugh path) | VERIFIED-STATIC |
| difficulty guards | attack count + difficulty (more attacks per cycle on harder settings); hero damage table | VERIFIED-STATIC |
| open items | ride duration (Ride anim length → gauge loss per ride, i.e. rides needed), kind 0x54 (what follows the defeat), the audio stream table 0x51690 (S5) | |

## Hydra (ENG1, HYDRA level 6) — L05.RE.03

| field | value | evidence |
|---|---|---|
| bank / models | bank 79 `BOSS\HYDRA` is loaded by `ANIM_LoadLevelBanks` from HYDRA's level list (no handler loads it). It holds effects only (acid, rocks, explosions, children). Heads and necks are T28 models 11–56 (HYDRA `models.png`) | VERIFIED-STATIC |
| structure | separate actors per slot (8 slots; record 0x6f188 + 0x48·slot; placement table 0x574d8, 0x54 bytes per slot):<br>- neck kind 0x29 (spawn 0x1cf50, update 0x1d0b0);<br>- head kind 0x2d (`HERO_HydraHeadSpawn` 0x1b2a0 / `HERO_HydraHeadUpdate` 0x1b7f0);<br>- a cut head becomes kind 0x30.<br>Both are created by `HERO_HydraSpawnHead` 0x1c4d0 | VERIFIED-STATIC |
| start | `LVL_InitLevelActors` 0x1fd20 → `LVL_InitHydra` 0x1ac10: camera, effects 0x202c/0x202a, intro effects kind 0x34/0x35 (looping frames 0x50–0x5d / 0x60–0x67 moving toward the camera, then freed), cut counter 0x6efb8 = 8, boss gauge (5), sfx 0x180/0x193, first head (phase 0 → slot 7). Per tick: `LVL_TickLevelActors` 0x1fd90 → `LVL_TickHydra` 0x1ae00 | VERIFIED-STATIC |
| neck states (+0x2a) | 0 grow (64 ticks, scale up); 3 sway (timer 50–99); 4 strike; 1 stump after a cut (waits, then 2); 2 regrow (`HERO_HydraHeadPhase` while cuts remain) | VERIFIED-STATIC |
| strike (4) | when the sway timer runs out, the hero is in reach (0x1cf10) and no other neck within ±62 units of the target is striking. Aim = hero x ± 16 plus the slot offset, clamped 0x480..0xcf0. Growl sfx 0x598c–0x598f at tick 0x1c and 0x5196–0x5199 at tick 0x28. A second timer makes the head spit (slot state 2, anim 1, sfx 0x5984) | VERIFIED-STATIC |
| attack window | in neck states 3/4 with extension +0x49 ≥ 0x26: if the head box (built from the model's carried vertices → attach list 0x70840) overlaps the hero's body box (frame box 1), `HERO_RequestDamage(head, 0x22)` → strength 1 → 8 / 18 / 22 (Beginner/Medium/Herculean) | VERIFIED-STATIC |
| vulnerable window | neck state 3 or 4, +0x49 between 0x26 and 0x38; the hero's attack box (frame box 2) within ±12 units of the neck point; 0x19b40 accepts the attack bits (~0x72168 & 0x18) → `HERO_HydraCutHead` 0x1d6c0 | VERIFIED-STATIC |
| cuts / regrowth / health | each cut: neck state 1, the head becomes kind 0x30 and flies (bounces at y 0x3d8, sfx 0x188), **cut counter − 1**, boss gauge − 0x20 (8 × 32 = 256), screams 0x5984–0x5987. Phases (0x6f44c): cutting the first head grows three (slots 3, 1, 5); later cuts regrow the cut slot and add 2, 4, 0, 6 until phase 8 | VERIFIED-STATIC |
| defeat | the 8th cut sets 0x72014 = 1: the hero can no longer die (health held ≥ 10, `HERO_Update`). `LVL_TickHydra` sequence 0x6f3e0: five explosions (effect 0x202e at fixed spots, 30 ticks apart), then 60 ticks, then end flag 0x72170 (0x19240) → `HERO_SetEndSequence` → script opcode −9 → gs+3 = 0x31 → tally | VERIFIED-STATIC |
| difficulty | the hero damage table; and the drop stream 0x56048 / 0x560e0 / 0x56168 → 0x6f3cc, read by 0x1b760 for drops on cuts. It is a u32 stream: 0 = set chance (num, den), < 0 = loop, > 0 = item id (1, 2, 3, 10). Neck and head behaviour has no difficulty read | VERIFIED-STATIC |
| debug | in `LVL_TickHydra`, SQUARE+TRIANGLE held (pad 0x9000) with gs+0x30 & 0x400000 cuts a head (development shortcut; not an entry method) | VERIFIED-STATIC |

## Medusa (ENG1, MEDUSA level 7, kind 0x2e) — L06.RE.03

| field | value | evidence |
|---|---|---|
| bank / models | banks 77 `BOSS\MEDUSA` and 78 `BOSS\SKELWAR` (level list); Medusa itself is spawned with frame 0x3d (T28 model 61 of MEDUSA) | VERIFIED-STATIC |
| spawn | not on the map (the six map spawns have kind 0). `LVL_InitLevelActors` → `LVL_InitMedusa` 0x21050:<br>- hero at (0x3b8, −, 0x4c);<br>- Medusa kind 0x2e at (0, 0x2f0, 0x280), spawn 0x21310 (scale 0x200);<br>- 0x22190 (spawns kind 0x2f), 0x23040 (kind 0x2d + effect 0x2029), effect 0x202d, 0x218f0;<br>- sfx 0x1c6, 0x6fe20 = 8, timer 0x6fde4 = 500;<br>- difficulty tables (Beginner 0x58b08/0x58ce0) → 0x6fcf8/0x6fe0c | VERIFIED-STATIC |
| update | `HERO_MedusaUpdate` 0x21320: the body follows the hero's x every tick. State 1 → phase 1, state 2 → phase 2. Camera shake 0x70cbc = ±0x40000 (±0xa0000 while 0x6fde0 counts down), sign flipping every other tick | VERIFIED-STATIC |
| phase 1 | `HERO_MedusaPhase1` 0x22280: two attack slots at hero x + 0x21 and + 0x8c (y 0x26f, z 0x220, frame 6), each run by `HERO_MedusaAttackSlot` 0x22440 (ids 2 and 5). When a slot ends, the counter 0x6fd14[slot·7] goes up; when both slots are spent → `HERO_MedusaPhaseEnd` 0x220c0 (spawns kind 0x31) | VERIFIED-STATIC; slot attacks OPEN |
| phase 2 (collapse) | `HERO_MedusaCollapse` 0x21aa0: every 4th tick a falling rock kind 0x34 (1/8: random position in the room; otherwise near the hero, ±32 units standing still, 120–152 ahead when moving) plus debris effect 0x202a every tick. Ends when timer 0x6fe2c < 0 (0x218f0) | VERIFIED-STATIC |
| attack slot | `HERO_MedusaAttackSlot` 0x22440. Each slot is a projectile that travels toward the camera (z − 0x20 per tick) and checks the actors in its path (actor pool):<br>- kind 0x2d (`HERO_MedusaSpawn5`/`Update3`, bank 77) within ±32 x whose counter 0x6fd14[part·7] = 6 → **reflected** (slot state 2), 0x6fe2c = 30, sfx 0x51ce, one hero voice 0x26;<br>- kind 0x30 within ±44 with counter 0 → slot state 1 (rises 4.0 per tick, debris 0x202a);<br>- a skeleton warrior (0x32) in its path dies (hit points 0);<br>- the hero (y > 0x2d0) within ±16 → `HERO_RequestDamage(medusa, 3)` → strength 2 → 10 / 25 / 30, sfx 0x1c5 one time in 4; the slot ends.<br>A slot that passes the hero's z − 32 ends. When a slot ends next to a part, that part's counter 0x6fd14 goes up | VERIFIED-STATIC; which object the player uses to reflect (the 0x2d parts at counter 6) is a LEAD for runtime |
| skeleton warriors | kind 0x32, bank 78: see L06.RE.04 notes (4 hit points, contact strength 0, spawned by `LVL_TickMedusa` every 60–179 ticks while 0x6fe20 > 0 and fewer than 3 are alive) | VERIFIED-STATIC |
| defeat | both slots spent → `HERO_MedusaPhaseEnd` → kind 0x31 `HERO_MedusaDefeatTick` (debris, then `HERO_RequestLevelEnd`) | VERIFIED-STATIC |
| health / defeat mechanic | 8 units (0x6fe20). A cycle (both slots spent, `HERO_MedusaPhaseEnd`) costs one unit and 0x20 of gauge **only** if a slot grabbed a settled column piece: kind 0x2d with counter 6, made by a slot lifting and breaking a column kind 0x30 (pieces fall and settle). No mirror object exists; the reflection LEAD maps to this. At 0 → kind 0x31 defeat | VERIFIED-STATIC |
| open items | how the player steers this in play terms (runtime), the difficulty table layout 0x58b08/0x58ce0 | |

## Final encounter (ENG1, VORTEX level 11, kind 0x28) — L10.RE.04

| field | value | evidence |
|---|---|---|
| bank | VORTEX's level list loads bank 80 `BOSS\HADES` (S4.01). That this actor is Hades is a LEAD from the bank name | VERIFIED-STATIC / LEAD |
| spawn | `LVL_InitLevelActors` → `LVL_InitVortex` 0x44120 → kind 0x28: `HERO_HadesSpawn` 0x445a0. It sits at hero x + 64, faces a random side, has hit points **+0xee = 200** and frame 0x86, gets an attached child 41000 (0xa028, frame 0x91) and an effect kind 0x29, then starts movement script 0x693e8 (`FUN_100446d0`). The boss gauge starts (5); difficulty tables 0x69180/0x69220/0x692b8 | VERIFIED-STATIC |
| behaviour | `HERO_HadesUpdate` 0x447c0 runs its own script interpreter (`{s32 frame, s32 ticks}` with opcodes 0 = return to the stored state, −1 = sfx, −2 = attached frame, −3 = spawn kind 0x2a (entry +0x1f8), −4 = load movement script 0x693e8 + n·0x60).<br>- State 0: when the hero is within 384 units and 0x72014 = 0, pick a random action script (0x69be8, 0x6a198, …).<br>- State 2 flips the facing.<br>- It throws kind 0x29 at the hero's x (clamped to the ranges 0x69358/0x69370).<br>- Every 240–479 ticks it drops a pickup (`FUN_100131c0` item 0x2b–0x2e \| 0x20/0x40 at x 0x56 or 0x4a6) | VERIFIED-STATIC |
| damage taken | `HERO_FindHitSource`. A hero punch (0x80) deals what 0x19b40 returns for the current attack bits (~0x72168 & 0x5f); projectiles and thrown objects deal 2. The gauge loses 256 / (200 / damage). Hit reaction scripts 0x69988/0x69ab8; 8 invulnerable ticks (+0xec); a drop from 0x44f80 | VERIFIED-STATIC |
| defeat | hit points < 1 → held at 1, defeat flag **0x72014 = 1**, timer 0x7028c = 300, `FUN_10045780` (defeat / ending sequence). It then sprays effect kind 0x2d every tick and 0x29 every 8 ticks | VERIFIED-STATIC; what `FUN_10045780` leads to (ending, credits) is S6.05 |
| attacks | kind 0x29 (`HERO_HadesEffectUpdate` 0x45a80) falls to the surface, then spawns kind 0x2a. Kind 0x2a (`HERO_HadesUpdate2` 0x45ef0) homes on the hero (turn rate ≤ its speed +0xb8, which grows to 32; speed +0xc8 slows to 10 over time), leaves a trail 0x2c every other tick, and dies on a wall/surface or when its timer runs out. On contact with the hero box: `FUN_10045bf0`(fireball, 3, 0) → one time in 8, `HERO_RequestDamage` strength (3−1)&7 = 2 → 10 / 25 / 30, plus a hero voice line 0x70/0x71/0x73/0x75 or sfx 0x4acb | VERIFIED-STATIC |
| phases | none in code: one 200-hit-point pool, random action scripts (0x69be8, 0x6a198, …) while the hero is within 384 units | VERIFIED-STATIC |
| difficulty | `LVL_InitVortex` picks the table 0x69180 / 0x69220 / 0x692b8 → 0x70198/0x7018c (layout open); no other difficulty read in 0x447c0 / 0x45ef0 | VERIFIED-STATIC |
| open items | the movement script tables 0x693e8, the difficulty table layout, `FUN_10045780` (defeat pose) | |

## Per-level index

Every actor kind of every ENG1 level, with its bank, damage, hit points and sounds: [actor-digest.md](actor-digest.md) (static digest).


| level | entries (filled by the level plans) |
|---|---|
| L01 TRAINING | Stymphalian bird (above) |
| L03 FOREST | Nessus (above) |
| L05 HYDRA | Hydra (above) |
| L06 MEDUSA | Medusa (above) |
| L10 VORTEX | Final encounter (above) |
| L02–L10 | `.plan/full-game-remaster/levels/<Lxx>/` link their entries here |
