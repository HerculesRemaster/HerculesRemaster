# Checkpoints, lives, game over and module exit (S6.04, ENG1 and ENG3)

Status: VERIFIED-STATIC (Ghidra, this build) unless marked otherwise. Game state (gs) = EXE 0x615c98, `DAT_10070f24` in ENG1. Names are in `analysis/annotations/full_game_s1_s6.json`. ENG3 equivalents are S3.

## Restart index and checkpoints

- **Restart index:** gs+0x02. On every life loss (`HERO_Die` 0x18f60, `HERO_KillInstant` 0x37080, `HERO_SetEndSequence`), the start index gs+0x01 is set to it. `HERO_Init` places the hero at start `0x5a69c[level] + 0x18·gs[1]`.
- **Level start:** `MOD_Tick` sets gs+0x01 and gs+0x02 to 0 when the module starts. A demo (`HERO_DemoLoad`) sets both from the demo header.
- **Checkpoint object:**
  - `LVL_CheckpointInit` 0x13990 / `LVL_CheckpointTick` 0x139f0, entries 10..17 of the level-config +0x24 behaviour table.
  - It is spawned with kind `0x400a`..`0x4011`. Its restart index is `(kind + 6) & 0xf`, so 0x400a = 0, …, 0x4011 = 7.
  - Drawn with frame set 0x53370 when it is the current restart index, 0x533c8 otherwise.
  - When the hero's box (`FUN_1002a4d0`) overlaps an inactive one, the game plays sfx 0x28, makes it active and sets **gs+2 = its index**. The previously active one switches back to inactive on its next tick.
  - The active one emits sparkles (kind 0x2010).
  - It snaps to the ground (`LVL_FindSurface`) everywhere except level 9 (TITANS), where it floats.
  - These are the Hermes level markers in the walkthrough LEAD.
- **Other writers of gs+2** (all byte stores found by scanning every code reference to `DAT_10070f24`, including code outside defined functions):
  - `LVL_RestartTriggerTick` 0x42bd0 (TRAINING behaviour table): a destroyed target sets gs+2 = 2.
  - ENG1 0xc87b, in an undefined function (called from 0xc7c0/0xc86d via 0xc730): sets gs+2 = 5. Its level is open.
- **Restart indices are not saved across levels:** they reset at the next module start.

Checkpoint spawns in the extracted maps (`extracted/levels/*/level.json`, kind → index, world x):

| level | checkpoints (index @ world x) |
|---|---|
| TRAINING (1) | 0 @ 1222, 1 @ 8228, 2 @ 15553, 3 @ 19777, 4 @ 25537 |
| FOREST (3) | 0 @ 951, 1 @ 5361, 2 @ 7540, 3 @ 10657, 4 @ 14410, 5 @ 18490 |
| THEBES (5) | 0 @ 273, 1 @ 4830, 2 @ 6272, 3 @ 8799, 4 @ 12481, 5 @ 15703 |
| TITANS (9) | 0 @ 197, 1 @ 9093, 2 @ 21436 |
| HYDRA (6), MEDUSA (7), VORTEX (11) | spawn lists not yet extracted (S2) |

**Not checkpoints:** kinds `0x200a`/`0x200b` (pickup class 0x2000; 0x200b = the HERCULES letters in gs+0x1c) and `0x10xx`/`0x14xx`/`0x18xx`/`0x1cxx` (other classes) share the low byte but are not checkpoints.

## Lives

| item | rule | code |
|---|---|---|
| level start | lives clamped to 1..9; deaths counter gs+0x0d = 0; max health gs+0x1e = 0x80 | `HERO_ResetLevelStats` 0x15600 |
| extra life | pickup kind 0x2012: lives + 1 (max 9), sfx 0xc03d | `HERO_CollectPickup` 0x12ef0 |
| life loss | gs+0x0d + 1; lives − 1 if > 0; flag gs+0x30 & 8 (cheat) gives it back; start = restart index | `HERO_Die`, `HERO_KillInstant` |
| game over | lives reach 0 → gs+0x03 = 1 | same |
| new game / password | TITLE copies gs+0x0b (the "Lives" option) to gs+0x0c, gs+0x10 = 0 | TITLE 0x16729 |

## Module exit (gs+0x03, `MOD_ExitCheck` 0x060f9, every tick)

| gs+3 | set by | `MOD_ExitCheck` path |
|---|---|---|
| 0 | `FUN_1001f320` (section start) | play |
| 5 | `MOD_Tick` start | fade-in; cleared when brightness 0x1006ee84 > 0x7f |
| 1 | `HERO_Die` / `HERO_KillInstant` when lives reach 0 | case 1/2: request TITLE (message 0x1006eef8), gs+5 = 1, gs+0xdf = 1 |
| 2 | anim-script opcode −51 (`HERO_AnimStep` case 1, jump table 0x166b4 → 0x16668). It is used in six scripts, including record 4 of end script 0x54b50 | same as 1, **but only when gs+0xe0 = 0 and gs+0xe4 = 0** (no stream active; reading of the flags is a LEAD). Otherwise gs+3 is cleared and play continues |
| 0x21 / 0x22 | anim-script opcode −11 (case 0x29): 0x22 when lives = 0, else 0x21 | 0x21: returns 0x21 (the engine loop ends without a TITLE request; what follows is open). 0x22: default path below |
| 0x31 | anim-script opcode −9 (case 0x2b): its only script use is record 51 of the level-end script 0x54b50 (`HERO_SetEndSequence` once bank 5 HC_DUNIT is loaded; 0x54c30 is the wait loop until then). Also TITANS code 0x37d0c, 0x3aed5 | default path. TITLE's exit dispatch (0x17732) sends only gs+3 = 0x31 to the tally (state 0xd2) |
| 0x22 (also via 0x37ab7) and other values | | default: wait for streams, `MOD_ReleaseLevel` 0x1f220, request TITLE (message 0x1006ef00) |
| 0x12 / 0x13 | `MOD_PauseQuitInput` 0x02180: SELECT+START held 61 ticks (0x13 in a demo, gs+6 = 3); demo timer end | quit: gs[0] = 0xff, request TITLE (message 0x1006eefc) |

Scripts are `{s16 frame or opcode, s16 ticks or argument}` pairs. A negative value is an opcode, dispatched as `switch (op + 0x34)`. Other opcodes seen: −10 (case 0x2a) starts the fade-out (gs+0x1b30 = 4); −29 is a sound request.

Which TITLE screen each message leads to (tally, continue, game over) is S6.05: the meanings of 0x6eef8/0x6ef00/0x6eefc on the TITLE side are open. **Level completion** is 0x31: end flag 0x72170 → `HERO_SetEndSequence` → end script 0x54b50 → opcode −9. **Game over** is 1: lives reach 0 in `HERO_Die`. TITLE sees gs+3 = 0 after case 1/2 and sends it to the per-level states 0x8c + (level − 1) (gamehercules-40, `progression.md`). gs+0xdf is a bank-streaming guard in TITLE (0x4aa0, 0x4b70), not a completion flag.

`MOD_ReleaseLevel` also copies the action slots gs+0x7c..+0x88 to +0x6c..+0x78. It restores difficulty from gs+0x0f to gs+0x0e, which undoes the demo override.

## ENG3 (runner levels L02 GAUNTLET, L07 CYCLOPS, L09 TORMENT)

VERIFIED-STATIC unless marked. Names: `analysis/annotations/full_game_eng3_kinds.json`, `full_game_s3_tables.json`.

### Restart index and checkpoints

- **Checkpoints:** Pegasus objects, pickup kinds 0x200b and up (`LVL_PegasusCheckpointInit` 0x165f0 / `LVL_PegasusCheckpointTick` 0x16670).
  - Each is inactive until the hero is on the same track and less than 700 units ahead.
  - Then it plays SFX 0x6028 and sets **gs+2 = (kind + 6) & 15**: 0x200b → 1, 0x200c → 2, …
  - The value is a start-record index: start records 1–7 lie at x 512 on tracks 0–6 in GAUNTLET; 1–5 at x 1024 on tracks 0–4 in TORMENT; CYCLOPS starts 2–3 are on tracks 1–2.
  - Per map: GAUNTLET has one Pegasus per track 0–6 (0x200b–0x2011), CYCLOPS 0x200b–0x200d, TORMENT 0x200b–0x200f.
- **Respawn:** `HERO_Die` 0x19a30 sets gs+1 = gs+2, and the next section start places the hero at that start record.
  - **Runtime (TORMENT, password 21):** restart index 1 was set at tick 1692. After each death the hero reappeared at x 1024 (start 1) with health 128, 50 ticks later (`analysis/traces/full-game/l09-passageways-of-eternal-torment/river-falls/20261012-072443-password-config`).
- **Section restart:** gs+3 = 0x21 → `MOD_CheckExit` 0xb059 returns 0x21 and `MOD_Tick` reruns `LVL_StartSection` (the level stays loaded). The spawn lists are reset for entries whose template flags & 2 (`LVL_InitSpawnLists` 0x1c010). The Cyclops is recreated at every section start (L07).

### Lives

| item | rule | code |
|---|---|---|
| level start | lives clamped to 1..9; health gs+0x24 = 0x80 | `HERO_InitStates` |
| extra life | bonus item kind 6, subtype 6 (amphora): lives + 1 (it becomes a potion at spawn if lives > 8) | `LVL_BonusItemTick` 0x15680 |
| life loss | `HERO_Die`: lives gs+0xc − 1, deaths gs+0xd + 1, skipped if gs+0x30 & 8 (cheat menu toggle, runtime-verified: lives stayed 4 in the CYCLOPS cheat run). Does nothing while 0x3fb2c ≠ 0 (trigger code 0x64 zone). | 0x19a30 |
| death causes | health < 1 after `HERO_ApplyDamage` (hit codes → table 0x380a0); falling below y 170 on trigger codes 0x63/0x77 (River Styx, runtime-verified in TORMENT); pit objects (0x59d0) | `chase.md` |
| game over | lives reach 0 → gs+3 = 1 | `HERO_Die` |

### Module exit (`MOD_CheckExit` 0xb059, every tick)

| gs+3 | meaning |
|---|---|
| 0, 1, 2, 5 | continue the tick loop. 1 = game over is handled by TITLE after the level is released |
| 0x12 / 0x13 | quit / demo end → `MOD_ReleaseLevel`, TITLE |
| 0x21 | section restart (above) |
| 0x31 | level complete: the end object `LVL_EndObjectUpdate` 0x16320 starts the ride (state 10); at the level's completion x (15360 / 30592 / 29696) `HERO_LevelComplete` 0x19c20 → state 9 → gs+3 = 0x31 → TITLE tally |
| other | release, then TITLE (or 0x1f8b0) |

The tally, continues and the next-level load are shared TITLE code (`progression.md`, S6.05). A runtime completion of an ENG3 level has not been traced yet (it needs a played run).

## Open (needs runtime or TITLE work)

- **Tally:** the tally screen (`CHOP_TITLE_TALLYUP` 0639) and its rewards are drawn by the TITLE state machine around 0x16600. That code is outside defined functions; screen states are in [ESI+4], e.g. 0x24, 0x25, 0x29, 0xdc, 0xe7. → S6.05 with a traced level completion.
- **Continues:** after game over (message 0x1006eef8), whether continues exist and what they keep.
- **Thebes Part 2:** whether the second Thebes password starts at a restart index > 0 (S6.03).
