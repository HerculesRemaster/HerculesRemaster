# Level progression, tally, continues and ending (S6.05, TITLE)

Status: VERIFIED-STATIC (Ghidra, this build), not yet runtime-checked. Addresses are TITLE.DLL RVAs (VA = 0x10000000 + RVA). gs = `DAT_1002bb44` (EXE 0x615c98). Constants are written as they appear in the code.

## Process model

- TITLE runs small cooperative processes. Each has a function, a state `[p+4]` and a wait mode `[p+8]`:
  - 0 = sleep until woken,
  - 1 = run next tick,
  - 2 = used around module switches.
- Helper functions:
  - `FUN_10005db0(fn, msg, 1, 0)` 0x5db0 spawns a child with state 0xffff.
  - `FUN_10005e90(fn, msg)` 0x5e90 wakes the process running `fn`.
  - `FUN_10005f10` 0x5f10 kills a process.
- **Entry:** `MOD_Tick` 0x5fb0 runs on every TITLE start, including each return from an engine. It does the following:
  - sets gs+1 = gs+2 = 0 and gs+0xa = 4 (0x5fdf–0x5ff1);
  - spawns the front-end machine `FUN_100164b0` in state **0xffff** (0x605c).
  
  0xffff is the "back from a module" dispatcher.
- **FMV:** `FUN_10009980(id, mask)` 0x9980 returns −1 (no FMV) if gs+0x30 & 0x10000. It returns immediately when gs+6 == 3. Otherwise it calls EXE slot 0x250 (`e:\data\%d.etv` player), so FMV `id` = `<id>.etv`. It reads only the frame counts from the PSX-style table 0x251e8 (16 B/entry, names `\MV\M1.;1`…). The result is tested as `& 8` (START: skipped) or `& 0x4000` (CROSS).
- **Module switch:** `MOD_StartLevelModule` 0x16300 → `FUN_1000c3d0` = EXE slot 0x64 `MOD_ChangeModule(&DAT_10029128, 0x22288[gs[0]])`. This happens only when gs+0xe4 == 0. Otherwise it calls slot 0x18 (stub).

## State machine `FUN_100164b0` (state in [ESI+4])

| state | action | next |
|---|---|---|
| 0xffff | gs+4 = 0, AUD reset, `DAT_1002ace8` = 0, then dispatch on **gs+3** (0x17732) | see "Back from a module" below |
| 0x33/0x34/0x35/0x36 | fade/hold `FUN_10001de0` (0x35: 0x78 ticks) | 0x26 → 0x27 |
| 0x27 | FMV 0x1a (only if ace8 == 0), FMV 1; START → 0x24 | 0x25 |
| 0x25 | FMV 2; START → 0x24 | 0x24 |
| 0x24 | FMV 3 (mask 0x10000) | 0xe7 |
| 0xe7 | if ace8 and `FUN_10006350(0)` == 1: child `LAB_100109f0` (save/load screen, calls `HERO_SaveLoadScreen`) | 0xe6 |
| 0xe6 | if gs+6 == 3: music, gs[0] 11 → 0, go to 0x29. Else: draw `MENUb.TGA`, child **main menu** `LAB_10007510` (0x17514), ace8/acec = 0 | 0x28 (sleep) |
| 0xf0 | draw `MENUb.TGA`, child main menu | 0x28 |
| 0x28 | woken by the menu: **game start** (0x166be–0x16735), see "Where gs[0], start and lives are set" | 0x29 or 0xdc |
| 0x29 | per-level reset, **gs[0] += 1** (0x167ae), pick the level card (0x167b5) | 100…0x6e, 0xcf, 0xfa, 0xe6 |
| 100…0x6e | (100 only: FMV 0x12 if acec == 0, 0x16956) child level card | 0xa0…0xaa (sleep) |
| 0xa0…0xaa | slot 0x1c(&DAT_10029128, engine path) = `RT_StubZero` | 0x78…0x82 (sleep) |
| 0x78…0x82 | `MOD_StartLevelModule` | 0x8c…0x96 |
| 0x8c…0x96 | defeat FMV per level (0x16e8d) | 200 |
| 200, 0xc9 | continue prompt / game over | 0x29 or 0xe6 |
| 0xd2 | AUD load (0xc,1)/(0x26,2), child **tally** `LAB_1001c500` (0x17086) | 0xd3 (sleep) |
| 0xd3 | per-level post-tally dispatch (0x170a5) | 0xb6…0xbe or 0x29 |
| 0xb6…0xbe | post-level FMV | 0x29, or 0xcd (end) |
| 0xcd | AUD unload, music | 0xcf |
| 0xcf | child **ending slideshow** `LAB_10001a70` (0x171b5) | 0xce (sleep) |
| 0xce | FMV 3 | 0xf0 |
| 0xfa | FMV chain (0x17554–0x176a9) | 0xf0 |
| 0xdc | attract demo (0x17205–0x172de) | 0xf0 |
| 0x104 / 0x10e | child `LAB_100102f0` (save screen) / `LAB_10010670` (password screen) | 0xe6 |
| 0x5a | AUD unload | 0xffff |

## Back from a module (state 0xffff, 0x17732–0x177fb)

| gs+3 | path |
|---|---|
| 0x11 | gs+7 = 0, gs+3 = 0, gs+0x12 = 0xff, ace8 = 1 → 0x33 (boot sequence: logos/FMV 1, 2, 3 → menu) |
| 0x12, 0x13 (quit, demo end) | → 0xe6 (wait 2): main menu, or the next level when gs+6 == 3 |
| **0x31 (level complete)** | → **0xd2 tally** |
| anything else (1, 2 game over; also 0x21/0x22/5 if they ever reach TITLE) | gs[0] 4 → 5, then 0x8c…0x96 by level; gs[0] outside 1..11 → ace8 = 0 and the state is left unchanged |

The engines write 0x31 (ENG1 `HERO_AnimStep` 0x16662, 0x37d29, `FUN_1003adc0` 0x3aedb; ENG3 `HERO_State09` 0x19d2c), 0x22/0x21 and 1/2 (see `checkpoints.md`). **Only 0x31 leads to the tally and the next level.**

## Level order and transitions

`0x29` (0x16742…) first resets the per-level fields:
- gs+6 = 0 unless 3;
- gs+0x11 = 0;
- the words gs+0x14, +0x16, +0x18, +0x1a, +0x1c = 0.

It then calls AUD_StopMusic, increments gs[0] and switches on the result:

| gs[0] | card state → card process | card screen | engine (0x22288) | post-tally FMV (0xd3 → 0xb6…) |
|---|---|---|---|---|
| 1 TRAINING | 100 → 0x14300 | SEQ5.TGA (0x1469a) | ENGINE1 | none, straight to 0x29 |
| 2 GAUNTLET | 0x65 → 0x14970 | SEQ6.TGA (0x14d16) | **ENGINE3** | none |
| 3 FOREST | 0x66 → 0x13ba0 | SEQ4.TGA (0x14021) | ENGINE1 | 0xc |
| 4 → forced to 5 (0x168a9) | – | – | – | 4/5: 0xd |
| 5 THEBES | 0x67 → 0x12280 | SEQ1.TGA (0x12618) | ENGINE1 | 0xd |
| 6 HYDRA | 0x69 → 0x15cb0 | SEQ9.TGA (0x16021) | ENGINE1 | 0xe |
| 7 MEDUSA | 0x6a → 0x12e90 | SEQ2.TGA (0x1320f) | ENGINE1 | 0x11 |
| 8 CYCLOPS | 0x6b → 0x13500 | SEQ3.TGA (0x138d7) | **ENGINE3** | 0x13 |
| 9 TITANS | 0x6c → 0x14ff0 | SEQ7.TGA (0x1537e) | ENGINE1 | Beginner: none → 0xcd **end**; else 0x19 |
| 10 TORMENT | Beginner: 0xcf; else 0x6d → 0x15650 | SEQ8.TGA (0x159e9) | **ENGINE3** | 0x10 |
| 11 VORTEX | Beginner: 0xcf; else 0x6e → 0x128e0 | SEQ10.TGA (0x12bd0) | ENGINE1 | Herculean 0xf, else 0x1b → 0xcd **end** |
| 12 | Beginner: 0xcf; else 0xfa | – | (PLATE.PIC, never started here) | – |
| other (0, 13…) | → 0xe6 menu | | | |

- **Cards:** each card process (e.g. TRAINING 0x14300) loads its `M:\GRAFIX\MOVIE\SCREENS\SEQn.TGA` (table 0x22308…0x22330; SEQ11 is not referenced by a card). It wakes the parent (0x145a1), animates, and then **calls `MOD_StartLevelModule` itself**. The call sites are:
  - TRAINING 0x14569;
  - the other cards: 0x12506, 0x12af8, 0x130e9, 0x137b1, 0x13ef6, 0x14bea, 0x15258, 0x158bd, 0x15f0f.

  After the call the card goes to 0xfffe/0xfffd and wakes the parent again.
- **ENG1↔ENG3:** there is no direct engine-to-engine path. Every level ends in TITLE (exit 0x31 → tally → card), and the card requests `0x22288[gs[0]]`. So GAUNTLET, CYCLOPS and TORMENT switch to ENGINE3 and back.
- **Runtime fact matches:** new game → acec = 0 → FMV 0x12 (18.etv, 0x16956), then the TRAINING card SEQ5.TGA ("YOUR BASIC D.I.D."), then the module at start index 0 (TITLE `MOD_Tick` 0x5fdf).

## Where gs[0], start and lives are set

| where | gs[0] | lives gs+0xc | continues gs+0x10 |
|---|---|---|---|
| main menu START GAME (0x7f4a) → 0x28, bb34 = 0, gs[0] == 0 | 0 (0x16722), then 0x29 → 1 | = gs+0xb option (0x1672c) | 0 (0x16735) |
| password accepted (bb34 = 1) → 0x28 | `DAT_1002bb38` − 1 (0x166f3) | = gs+0xb | 0 |
| gs[0] != 0 at 0x28 (load game `HERO_RestoreSaveRecord` 0x121d0, cheat level select `FUN_10001300` 0x1356) | gs[0] − 1 (0x16705), acec = 0 | kept (from the record) | kept |
| continue accepted (0xc9) | gs[0] − 1 (0x16f72) → same level | = gs+0xb (0x16f6a) | − 1 (0x16f5f) |
| every 0x29 | + 1 (0x167ae) | – | – |
| demo (0xdc) | demo table 0x26068[gs+7] | – | – |

- **Start/restart index:** TITLE always enters a module with gs+1 = gs+2 = 0 (0x5fdf, 0x5fe8). TITLE has no mid-level start.
- **Save record** `FUN_10012070` 0x12070 stores the **next** level (same order 1→2→3→5→…→11→12) with lives, the option, difficulty, continues, flags and action slots. On load, 0x28 then performs −1/+1 to reach that level.

## Tally (`LAB_1001c500`, bank t007.TGA via 0x2237c)

Init (state 0xffff, 0x1cb75–0x1d14c):
- **Score `DAT_1002aff0`** (0x1d041–0x1d095) = `50 + pct/2 − 25·deaths`, clamped to 0..100:
  - `pct = (gs+0x14 + gs+0x18)·100 / (gs+0x16 + gs+0x1a)`, and 0 if the totals are 0;
  - deaths = gs+0x0d.
  - The counters are ENG1 pickups: 0x2019 → +0x14 (collected) / +0x16 (total spawned), 0x2018 → +0x18 / +0x1a (`HERO_CollectPickup` 0x12ef0 in ENG1; `LVL_SpawnObjects`).
- **Percentage `DAT_1002b318`** = pct clamped to 0..100 (0x1d0ce). Both values are drawn by 0x1a480…0x1a5cb.
- **Extra life** (0x1d0fb–0x1d12a): if `DAT_1002b36c` == 0 and score > threshold `DAT_1002b328` and gs+0xc < 9, then gs+0xc += 1 and `DAT_1002b08c` = 1.
  - Thresholds (0x1cec2): 0x28 Beginner, 0x3c Medium, 0x50 Herculean.
  - b36c is set to 1 after the save sub-screen (0x1ca55), so re-entering the tally gives no second life.
- **Continue reward** (0x1cef2): if word gs+0x1c == 0xff (all 8 HERCULES letters, ENG1 pickup 0x200b sets bit `obj+0x2a`), then gs+0x10 += 1. The letters are cleared at every 0x29, so they must all be collected in one level.
- **Password shown** (0x1cd2d–0x1cdcb): the 4 icons are taken from password record `0x25a30[(gs+0xe·12 + gs[0])]`, which is the entry for the next level (e.g. after level 9: Beginner → 12, Medium → 10). If gs+0x11 == 0x0f, all four are set to 4 instead.
  - gs+0x11 bits come from ENG1 pickups 0x201a…0x201d.
  - Flag gs+0x30 & 0x800000 forces gs+0x11 = 0x0f (0x1cd35).

States:
1. Fade in and draw.
2. Calls EXE slot 0x240 `(next level, difficulty)` (0x1c6db; EXE 0x404b10 raises the per-difficulty max-level bytes 0xb9e85c[0..diff], **clamping Beginner [0] to 9**). START in sub-states 1..3 calls EXE slot 0x248 with `next<<16 | diff<<8 | lives` (0x1c9af; EXE 0x404bc0 stores it at 0xb9e860).
3. Child save screen `LAB_100102f0` (0x1ca38).
4. Re-init.
5. 0xfffe: wakes the parent (0x1cb59) → 0xd3.

## Game over / continues

- Any non-0x31 return (lives 0 → gs+3 = 1) plays a defeat FMV (0x16e8d), chosen by level:
  - 4 for levels 1, 2;
  - 8 for 3, 9;
  - 6 for 5, 7;
  - 0x14 for 6;
  - 7 for 8;
  - 10 for 10, 11.
- **200** (0x16ee0): gs+0x30 & 0x10 (cheat) forces gs+0x10 = 1.
  - If gs+0x10 == 0 → acec = 0.
  - Else FMV 0x16 with mask 0x4000, and acec = CROSS pressed (bit 14).
- **0xc9:**
  - acec = 1 → continue: gs+0x10 − 1, lives = gs+0xb, gs[0] − 1, FMV 0x15, music → 0x29. The same level restarts from start index 0 with a new level card.
  - acec = 0 → game over: FMV 9. Unless skipped with START, FMV 3 follows. Then 0xe6 (main menu). gs[0] is left as is.
- Continues are earned only by the HERCULES letters in the tally, or restored from a save record. A new game or a password sets them to 0.

## Beginner boundary

- **After TITANS (9) on Beginner:** tally → 0xd3 → 0xbc → gs+0xe == 0 → 0xcd → **ending slideshow**. The difficulty test at 0x16d5b skips FMV 0x19 (0x16d7c).
- **Guards:** 0x29 sends gs[0] = 10, 11 or 12 straight to 0xcf on Beginner (0x167c8, 0x167f7, 0x16826).
  - The Beginner password slots L09/L10 (result 12) therefore lead to gs[0] = 11 → 12 → 0xcf.
  - The tally after level 9 shows that result-12 password.
- **EXE unlock (slot 0x240)** also caps Beginner progress at 9.

## Ending

- **VORTEX (11) done, Medium/Herculean:** tally → 0xbe → FMV 0xf (Herculean; 15.etv, longest entry, 1846 frames) or 0x1b (Medium; 27.etv, the table entry has no name pointer) → 0xcd → 0xcf.
- **Slideshow `LAB_10001a70`:** EXE slot 0x244 call (−2, 0), screens from table 0x223b0.
  - Start index 0 = `endeasy.TGA` if gs+0xe == 0 (with a longer first hold, counter = −0xf0). Otherwise the start index is 1 = `c1.TGA`.
  - It advances every 0x3c ticks through c1…c31, skipping index 0x1d (c29), and stops at index 0x20.
  - START skips it.
  - Then the parent: FMV 3 → 0xf0 → main menu.
  - No progress is reset; gs[0] stays 11 (or 9).
- **Password result 12 on Medium/Herculean** → 0xfa: plays FMV 0x12, 0xc, 0xd, 0xe, 0xb, 0x11, 0x13, 0x19, 0x10, then 0xf/0x1b in sequence (START aborts) → menu. This is a movie replay, not the slideshow.
- **Attract demo (0xdc):** entered after 0x384 idle menu ticks (`DAT_1002bb30` = 1, 0x78e3).
  - It alternates a demo with FMV 0xb (mask 0x8000).
  - Demos come from 0x26068 {level, demo#}: (1,1), (1,2), (1,3), (2,4), (3,5), (5,6), (5,7); gs+7 cycles 0..6.
- **Cheat gs+6 = 3** (password steps 0x68 → 0x69, `HERO_PasswordCheck` 0xce85) does the following:
  - sets flags gs+0x30 |= 0x80000008;
  - skips all FMVs;
  - makes quitting a level (0x12/0x13) go to the next level (0xe6 → 0x29), wrapping 11 → 1.

## Open

1. **Parent states 0x78…0x82 → 0x8c…0x96:** after a card has already requested the module (e.g. 0x14569), the parent calls `MOD_StartLevelModule` again, then defeat FMV + continue. Whether this ever runs depends on when the EXE performs the switch (slot 0x64). Read EXE `MOD_ChangeModule` 0x404040 and the DLL tick loop, or trace at runtime.
2. **Who writes gs+3 = 0x11** (the boot entry): no writer in TITLE, ENG1 or ENG3. Read the EXE gs init (0x615c9b).
3. **gs+0x11 == 0x0f → password icons = 4:** the meaning is open. Read the tally draw functions 0x1b710 / 0x1bc60 / 0x1bdc0 and their use of `DAT_1002b0f0..fc`.
4. **What the defeat FMVs 4, 6, 7, 8, 10, 0x14 and the post-level FMVs show:** check `e:\data\<id>.etv` assets.
5. **Readers of EXE 0xb9e85c (max level per difficulty) and 0xb9e860 (resume record):** the PC level-select/LSGAME path. Read the EXE xrefs.
6. **Entry into states 0x104, 0x10e, 0x34 and 0xe7's `FUN_10006350`:** read the main menu `LAB_10007510`.
7. **`HERO_SaveLoadScreen` 0x11e16:** sets gs[0] = 0 and gs+0x10 = 0 after a restore (slot 8). Its purpose is open.
8. **Thebes Part 2 (S6.03):** TITLE always starts at index 0, so a Part 2 start would need ENG1-side logic.
