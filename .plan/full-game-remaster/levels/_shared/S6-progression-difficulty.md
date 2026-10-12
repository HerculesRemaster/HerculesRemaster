# S6 Progression, entry methods and difficulty

Goal: recover the cross-level game flow. That means difficulty encoding, passwords and level select, checkpoints/lives/continues, tally/loading, and level-to-level transitions. Also verify the Beginner boundary, and run the linked Medium L01→L10→ending capture that closes the full game.
Needs: S1; P2 TITLE/EXE dispatch; per-level checkpoint findings feed back in.
Slices: `P3.ALL.07`, `P3.ALL.09`, `P3.ALL.10`, `P5.ALL.11`, `P5.ALL.12`.
Outputs:
- `docs/full-game/gameplay/{progression,checkpoints,difficulty}.md`
- `extracted/meta/full-game/tuning.json` (difficulty-keyed constants)
- `reference/passwords.json` (only icons verified in this build)

**LEAD:**
- The manual says Beginner ends after L08 and that L09/L10 need Medium or Herculean.
- The walkthrough lists Medium passwords per level, including separate Big Olive Part 1 and Part 2 entries. These are walkthrough values for some edition; they must be entered and accepted in this build before use.

## Tasks

- [x] `S6.01` (P3.ALL.03) Find the difficulty variable and its encoding (TITLE options → game state). Find which code reads it in ENG1 and ENG3.
- [x] `S6.02` (P5.ALL.11) Decompile password entry and validation in TITLE (`CHOP_TITLE_PSWD` bank 0554). Recover the icon alphabet, the encoding of level, difficulty and state, and whether a password can select a substage (needed for the Thebes Part 2 lead).
- [x] `S6.03` (P3.ALL.04) Verify each password LEAD in the running build. Record the accepted ones in `reference/passwords.json` with their initial state. Verify any F3 or level-select path the manual mentions.
- [x] `S6.04` (P5.ALL.11) Specify these for both engines: checkpoint persistence (Hermes level markers are a LEAD, from the walkthrough), lives/continues, game over, restart (ENG1 `HERO_Die` 0x18f60 → start index), and the tally screen (`CHOP_TITLE_TALLYUP` 0639) with its rewards.
- [x] `S6.05` (P5.ALL.11) Specify the transitions: level completion → tally → next-level load, including ENG1↔ENG3 module switches (L01→L02, L02→L03, L06→L07, L07→L08, L08→L09, L09→L10) and the ending and credits.
- [ ] `S6.06` (P3.ALL.10) Beginner boundary. Play Beginner through L08 and record what follows it (ending? return to title?). Check that L09/L10 cannot be entered normally on Beginner, including by password. Only after that, set the L09/L10 Beginner cells to `UNAVAILABLE_BY_DESIGN`.
- [x] `S6.07` (P3.ALL.09, P5.ALL.12) Coordinate the difficulty comparisons. Each level plan runs its own Beginner/Herculean comparison sets. S6 owns the shared encoding and `tuning.json` schema: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `S6.08` (P3.ALL.07) Capture the linked Medium run from a new game through L01→L10→ending. Consecutive runs may be linked at level boundaries if state continuity is recorded. A password-only sample does not establish progression.

## Done when

- The difficulty and password encodings are code-backed.
- The Beginner boundary is observed.
- The linked Medium run through the ending exists, with level-boundary ticks.
- `progression.md`, `checkpoints.md` and `difficulty.md` cover both engines.

## Notes
- `S6.01`/`S6.02` (2026-10-12, VERIFIED-STATIC): difficulty = game state +0xe (0 Beginner, 1 Medium, 2 Herculean). ENG1 `HERO_ApplyDamage` 0x18c50 uses the damage table 0x55348[hit + difficulty·8]; `HERO_EnemyInitHp` 0x24270 sets enemy HP = difficulty·2 + 6. Password table TITLE 0x25a30 (`HERO_PasswordCheck` 0xcd30): 36 entries = 12 per difficulty + 6 cheat steps; icon = slot value / 8. The Beginner group sends L09/L10 to 12 (ending). Thebes has two entries per difficulty, differing only by index gs+0x12 (unread by ENG1), so the substage question goes to S6.03 at runtime. → `docs/full-game/gameplay/difficulty.md`, `analysis/exports/full-game/password_table.json`. ENG3 readers of +0xe: S3.
- `S6.04` progress (2026-10-12, session gamehercules-22, VERIFIED-STATIC): ENG1 checkpoints/lives/game over/module exit → `docs/full-game/gameplay/checkpoints.md` (checkpoint objects 0x400a–0x4011, restart index gs+2, `HERO_Die`/`HERO_KillInstant`, `MOD_ExitCheck` 0x060f9, `MOD_ReleaseLevel` 0x1f220). Open before the tick: ENG3 (S3), tally/continues (TITLE 0x16600, S6.05).
- `S6.05` (2026-10-12, VERIFIED-STATIC) → `docs/full-game/gameplay/progression.md`:
  - `MOD_FrontEndStateMachine` TITLE 0x164b0. Only exit code 0x31 = level complete → tally (0xd2) → level FMV → gs[0] += 1 (0x167ac/0x167ae, checked). Order 1→2→3→5→6→7→8→9→10→11→ending (4 forced to 5).
  - Tally score / extra life (lives < 9, 0x1d127, checked) / continue for all 8 letters; game over → continue prompt; Beginner goes to the ending after 9 (0x167c8…); ending slideshow `GFX_EndingSlideshow`. 29 TITLE functions named.
  - Corrects `checkpoints.md`: 0x22 is not "complete"; TITLE treats only 0x31 so. Runtime level-completion trace still to do (S6.08).
- `S6.06` progress (2026-10-12, VERIFIED-RUNTIME, password path): the Beginner password slots for L09/L10/end (table entries 9–11, result 12) show the LEV12 card, then `endeasy.TGA` (Beginner ending) and credits c1… (`analysis/traces/full-game/_passwords/09`, ticks 1642–2312). No Beginner password loads level 10 or 11. Open before the tick: a Beginner L08 (TITANS) completion run (static: ending, `progression.md`).
- `S6.03` (2026-10-12, VERIFIED-RUNTIME): `tools/trace/password_check.py` entered all 36 password table entries in the running game. The icon cycles per slot were measured (`analysis/traces/runs/pswd2`; the screen takes input from about tick 562).
  - 31 entries load exactly the table level at the table difficulty (TRAINING … VORTEX on Medium/Herculean, up to TITANS on Beginner).
  - The 5 result-12 entries (Beginner 9–11, Medium 23, Herculean 35) go to the ending.
  - Both Thebes entries per difficulty load THEBES at start 0 (hero 256/1228); they differ only in gs+0x12, so the "Part 2" lead is not a separate start in this build.
  - → `reference/passwords.json` (accepted entries, icons = position in each slot's list, observed state), `analysis/exports/full-game/password_check.json`, runs `analysis/traces/full-game/_passwords/NN`. The `password` entry method is in `run_scenario.ps1`. No F3 level select exists; F3 is the save/load screen (`HERO_SaveLoadScreen`).
- `S6.07` (2026-10-12): shared encoding = game state +0x0e. Schema and first entries in `extracted/meta/full-game/tuning.json` via `tools/state/tuning_export.py`; values are read from `backups/originals/` in their original representation, keyed by program + RVA, entity, scenario and difficulty. Entries:
  - ENG1 hero damage 0x55348 and enemy HP formula;
  - ENG3 hero damage 0x380a0;
  - tally extra-life thresholds and the Beginner end rule;
  - HYDRA/VORTEX table pointers and the Nessus attack formula (from gamehercules-22).

  Per-level comparison runs stay in each level plan, and new constants are appended to the exporter.

- `S6.06` input from L09.RE.06: ENG3 reads difficulty (gs+0xe) only in `HERO_ApplyDamage` 0x19410, so there is no engine-side check on level 10. The Beginner restriction is the TITLE password table and progression only.
- `S6.03` follow-up (2026-10-12, VERIFIED-RUNTIME): the cheat passwords 100 → 101 unlock an in-game cheat level select (fifth main-menu item, TITLE `HERO_CheatMenuInput` 0x16f0). It covers all 10 levels and has a no-life-loss toggle. Verified to load GAUNTLET and CYCLOPS. It is a game-provided entry, and is scripted as `run_scenario -Entry cheat_select`. Details in `docs/full-game/gameplay/difficulty.md` "Cheats". For S6.06: whether it allows L09/L10 on Beginner is checked next.
- `S6.06` progress (2026-10-12, VERIFIED-RUNTIME, third route): on Beginner, the in-game cheat level select with TORMENT (gs[0] = 10) does not load the map. TITLE goes straight to `endeasy.TGA` and credits c1–c4, then back to the menu. Run: `analysis/traces/full-game/l09-passageways-of-eternal-torment/beginner-unavailable/20261012-075412-cheat_select-beginner`.
  - All three routes into L09 on Beginner (password, cheat select, and the static rule after level 9) lead to the ending.
  - Still open before the tick: the played Beginner L08 (TITANS) completion run, which needs a human player (record with session 22's `tools/trace/record_play.ps1`).
- Correction (2026-10-12, 08:57): after the cheat menu START the selected level loads without further input. `cheat_pad` used to press UP ×5, CROSS and START ×2 afterwards, and those presses fell inside the level in the cheat_select runs before 08:57 (speed-ups, a jump, a pause of about 150 ticks). Their observations stand, but they are not no-input runs. Fixed in `tools/trace/password_check.py`.
- `S6.04` (2026-10-12, done): `docs/full-game/gameplay/checkpoints.md` now covers both engines.
  - **ENG3 section:** Pegasus checkpoints → gs+2 = (kind + 6) & 15 = start-record index; respawn and section restart 0x21; lives, death causes, game over and exit codes.
  - **Runtime-confirmed:** respawn at start 1 in TORMENT, and the no-life-loss toggle.
  - **Tally, rewards and continues:** in `progression.md` (S6.05).
  - **Still open, needing played runs** (tracked in S6.08 and the level TR tasks): runtime traces of a level completion → tally, and of a game over → continue.
