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
- [ ] `S6.03` (P3.ALL.04) Verify each password LEAD in the running build. Record the accepted ones in `reference/passwords.json` with their initial state. Verify any F3 or level-select path the manual mentions.
- [ ] `S6.04` (P5.ALL.11) Specify these for both engines: checkpoint persistence (Hermes level markers are a LEAD, from the walkthrough), lives/continues, game over, restart (ENG1 `HERO_Die` 0x18f60 → start index), and the tally screen (`CHOP_TITLE_TALLYUP` 0639) with its rewards.
- [ ] `S6.05` (P5.ALL.11) Specify the transitions: level completion → tally → next-level load, including ENG1↔ENG3 module switches (L01→L02, L02→L03, L06→L07, L07→L08, L08→L09, L09→L10) and the ending and credits.
- [ ] `S6.06` (P3.ALL.10) Beginner boundary. Play Beginner through L08 and record what follows it (ending? return to title?). Check that L09/L10 cannot be entered normally on Beginner, including by password. Only after that, set the L09/L10 Beginner cells to `UNAVAILABLE_BY_DESIGN`.
- [ ] `S6.07` (P3.ALL.09, P5.ALL.12) Coordinate the difficulty comparisons. Each level plan runs its own Beginner/Herculean comparison sets. S6 owns the shared encoding and `tuning.json` schema: original representation, keyed by program+RVA, entity/state, difficulty and scenario.
- [ ] `S6.08` (P3.ALL.07) Capture the linked Medium run from a new game through L01→L10→ending. Consecutive runs may be linked at level boundaries if state continuity is recorded. A password-only sample does not establish progression.

## Done when

- The difficulty and password encodings are code-backed.
- The Beginner boundary is observed.
- The linked Medium run through the ending exists, with level-boundary ticks.
- `progression.md`, `checkpoints.md` and `difficulty.md` cover both engines.

## Notes
- `S6.01`/`S6.02` (2026-10-12, VERIFIED-STATIC): difficulty = game state +0xe (0 Beginner, 1 Medium, 2 Herculean). ENG1 `HERO_ApplyDamage` 0x18c50 uses the damage table 0x55348[hit + difficulty·8]; `HERO_EnemyInitHp` 0x24270 sets enemy HP = difficulty·2 + 6. Password table TITLE 0x25a30 (`HERO_PasswordCheck` 0xcd30): 36 entries = 12 per difficulty + 6 cheat steps; icon = slot value / 8. The Beginner group sends L09/L10 to 12 (ending). Thebes has two entries per difficulty, differing only by index gs+0x12 (unread by ENG1), so the substage question goes to S6.03 at runtime. → `docs/full-game/gameplay/difficulty.md`, `analysis/exports/full-game/password_table.json`. ENG3 readers of +0xe: S3.
