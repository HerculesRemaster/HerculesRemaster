# Difficulty, level identity and passwords (S1.01, S6.01, S6.02)

All rows are VERIFIED-STATIC (Ghidra decompilation of this build) unless marked otherwise. Game state = the shared struct at EXE 0x615c98 (interface slot 0x004), seen as `DAT_1002bb44` in TITLE and `DAT_10070f24` in ENG1.

## Identity fields

| field | meaning | evidence |
|---|---|---|
| +0x00 u8 | level ID (1, 2, 3, 5–11; 12 = ending) | TITLE `FUN_10016300` picks the engine as `0x22288[gs[0]]`: ENGINE3 for 2, 8, 10, ENGINE1 otherwise, `PLATE.PIC` movie for 12. ENG1 `LVL_SelectLevelConfig` 0x280f0 matches it against the level table 0x68af0 |
| +0x01 u8 | start index used by `HERO_Init` (0x5a69c[level] + 0x18·index) | ENG1 `HERO_Init` 0x15090, `HERO_DemoLoad`, `HERO_Die` |
| +0x02 u8 | restart index (copied to +1 on a life loss) | ENG1 `HERO_Die` 0x18f60 |
| +0x0c u8 | lives | ENG1 `HERO_Die`; TITLE copies +0x0b → +0x0c at a level start (0x16729) |
| +0x0e u8 | **difficulty: 0 Beginner, 1 Medium, 2 Herculean** | see below |
| +0x12 u8 | index of the accepted password entry (TITLE only; ENG1 never reads it) | TITLE `FUN_1000cd30` |
| +0x24 s16 | hero health (reduced by the per-difficulty damage table) | ENG1 `FUN_10018c50` |
| +0x30 u32 | flags (cheats set 0x80000000 / 0x8) | TITLE `FUN_1000cd30` |
| +0x8c | engine map tag (`LVL_LoadMap` refuses a map whose tag differs) | ENG1 0x1f750 |

There is no separate section/substage field: every ENG1 map has one section (`level.json`), so the position inside a level is the start index.

## Difficulty (+0x0e)

- **Set by:** TITLE `FUN_1000cd30` (from the password record), TITLE `FUN_100121d0` (restoring a saved game record, slot field +0x10), ENG1 `HERO_DemoLoad` (demo byte 4 = 0) and ENG1 `FUN_1001f220`.
- **Read by 12 ENG1 functions.** Two of them show the meaning:
  - `FUN_10018c50` (`HERO_ApplyDamage`): `health −= 0x55348[((stored − 1) & 7) + (difficulty & 3)·8]`, where `HERO_RequestDamage` 0x266a0 stores n + 1 (bit 0x10 cleared). So the table index is **n & 7** for a request n < 0x10 (0x11 → 1, 0x22 → 2). Runtime check by gamehercules-22: in the THEBES Beginner demo the Minotaur (n = 3) costs 12.
  - `FUN_10024270`: an enemy's hit points (+0xee) = `difficulty·2 + 6`.

| difficulty | hero damage per table index 0–7 = request n & 7 (ENG1 0x55348) | enemy HP (`FUN_10024270`) |
|---|---|---:|
| 0 Beginner | 5, 8, 10, 12, 15, 16, 20, 22 | 6 |
| 1 Medium | 12, 18, 25, 30, 35, 40, 45, 50 | 8 |
| 2 Herculean | 15, 22, 30, 38, 45, 50, 55, 60 | 10 |

ENG3 readers: open (S3).

## Passwords (TITLE)

**Screen:** `FUN_1000cf00` creates four icon objects (0x2a278, 0x2a27c, 0x2a280, 0x2a274).

**Check:** `FUN_1000cd30` compares `frame − base` of the four objects (bases 0x1e3, 0x121, 0x182, 0xc0) with the table at TITLE 0x25a30. Records are 0x18 bytes: 4 slot values, result, difficulty. Slot values step by 8 (icon = value / 8; every slot starts at 24).

**On accept** (result < 100), `FUN_1000d4b0` → 0xe8be does the following:
- `DAT_1002bb38` = result, `DAT_1002bb34` = 1;
- `gs[0] = result − 1` (0x166e6), and the next-level step then loads the result;
- gs+0x0e = difficulty, gs+0x12 = index.

**Entries:** 36 normal entries, 12 per difficulty, plus 6 cheat steps. The full table is in `analysis/exports/full-game/password_table.json`.

| difficulty | results, in table order |
|---|---|
| 0 Beginner | 1, 2, 3, 5, 5, 6, 7, 8, 9, **12, 12, 12** |
| 1 Medium | 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12 |
| 2 Herculean | 1, 2, 3, 5, 5, 6, 7, 8, 9, 10, 11, 12 |

**Findings:**
- **Beginner:** in the Beginner group, the slots of L09 (10) and L10 (11) lead to 12 (the ending). This is code-backed support for the manual's lead that Beginner ends after L08. The runtime check (S6.06) is still open.
- **Thebes:** two entries per difficulty lead to level 5 (Thebes) and differ only by their index (gs+0x12). ENG1 never reads gs+0x12, so whether the second one starts Part 2 has to be seen at runtime (S6.03).
- **Cheats** are two-step sequences: entry 100 then 101, 102 then 103, 104 then 105 (`HERO_PasswordCheck` 0xcd30). A result ≥ 100 plays SFX 0x302 and stays on the password screen with the cursor unchanged.
  - **100 → 101:** gs+0x30 |= 0x80000000, gs+0x2e = 0. VERIFIED-RUNTIME: run `analysis/traces/full-game/_cheats/cheat_100_101`, result 100 at tick 1424, then 101 with gs+0x30 = 0x80000000 at tick 2366.
    - The main menu (`MOD_MainMenu` 0x7510) then has a fifth item (index 4, max item 3 + the flag), which opens the cheat menu `HERO_CheatMenuInput` 0x16f0 (`M:\GRAFIX\MOVIE\SCREENS\CHEAT.TGA`, drawn by `HERO_CheatLevelSelect` 0x1300).
    - Cheat menu input:
      - RIGHT = next level, list TITLE 0x22030 (u16 frame, u8 level): 1, 2, 3, 5, 6, 7, 8, 9, 10, 11;
      - UP toggles gs+0x30 bit 8 (`HERO_Die` skips the life decrement, both engines);
      - LEFT toggles 0x10 (forces gs+0x10 = 1, `progression.md`);
      - DOWN toggles 0x800000 (consumer open);
      - START leaves with gs[0] = the level, which then loads by itself (about 150 ticks later).
    - VERIFIED-RUNTIME: GAUNTLET (`…/l02-heros-gauntlet/full-progression/20261012-073417-password-config`) and CYCLOPS (`…/l07-cyclops-attack/death-retry/20261012-073756-cheat_select-medium`).
    - Scripted as `run_scenario.ps1 -Entry cheat_select -CheatLevel <index> [-CheatLives]` (`tools/trace/password_check.py` `cheat_pad`). This is an in-game feature, not a memory write; with the life toggle on, the life-decrement path is not representative.
  - **102 → 103:** the same flag, gs+0x2e = 1. The cheat menu then uses list 0x22080 (same levels, other frames) and has no toggles; UP/DOWN step the level.
  - **104 → 105:** gs+0x30 |= 0x80000008 (no life loss) and gs+6 = 3.
- **Saved games:** TITLE `FUN_10010fe0` (F3 load game) restores 0x3c-byte slots from TITLE 0x2aa50 through `FUN_100121d0`. The record fields go to the game state as: +4 → level, +8 → lives (+0x0c), +0xc → +0x0b, +0x10 → difficulty (+0x0e), +0x14 → +0x10, +0x30 → flags, +0x18/+0x1c → +0x1b0c/+0x1b10, +0x20…+0x2c → the action slots +0x6c…+0x78, +0x34 → +0x1b24. Where the slots persist is open.
