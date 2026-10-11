# L08 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 9 BATTLE OF TITANS, ENG1 descriptor 0x5a768, BIN 538/`526D69FA`, CIF 539/`526E69F2`, offsets and hashes as in README; TITLE 0x2351c, module pointer 0x222ac; palette selector 9, path 0x366cc | `reference/levels.json`, `reference/level-map.md` |
| ENG1 per-level table 0x68af0 (0x48 B per ID, 999 ends) lists IDs 0, 1, 3, 5, 6, 7, **9** and 11 | `docs/level-format.md` |
| Descriptor 0x5a768 values: path `O:\TITANS\` (0x1005a7cc), header size 0xf7000, 0, then 0x10063108 / 0x100630e4 / 0x100630f8 (meaning not yet recovered) | `reference/binary-analysis/dispatch-tables.json` |
| The header size 0xf7000 equals the decoded BIN `header_size` 1,011,712 = first block sector 494 × 0x800, the rule in `docs/level-format.md` (offset 0x04) | `extracted/levels/TITANS/level.json`, `docs/level-format.md` |
| TITLE rows: ID 8 CYCLOPS CHASE → `\SRC3\ENGINE\` ENGINE3; ID 9 → `\SRC1\ENGINE\` ENGINE1; ID 10 PASSAGEWAYS TORMENT → `\SRC3\ENGINE\` ENGINE3. The ENG1 descriptor slot for ID 10 (0x5a780) is all zero. So both L07→L08 and L08→L09 cross an engine boundary | `reference/binary-analysis/dispatch-tables.json` |
| Decoded map: 1 section, 5 layers, 512×18 cells, 4 blocks (sectors 494, 518, 545, 574), 93 spawns from 43 templates, spawn x 197–29059, y 313–1107, **every spawn at z = 128**; 546 camera records; header `n_anim` 0, `n_scroll` 0, no decode warnings | `extracted/levels/TITANS/level.json` |
| Most frequent spawn kinds: 0x61 ×11, 0x200b ×8, 0x2015 ×7, 0x51 ×6, 0x43 ×5, 0x42 ×5, 0x2016 ×5. Kinds index the per-level type table, so their meaning is per level | `extracted/levels/TITANS/level.json` |
| ENG1 strings: `O:\TITANS\` 0x1005a7cc, `q:\TITANS\` 0x100518b8, `q:\HERC_TIT\` 0x10051804; SRC1 bank paths `ALIEN2\PEGASUS` 0x1005d830, `IceT` 0x1005d804, `RockT` 0x1005d818, `LavaT` 0x1005d7ec, `TornadoT` 0x1005d7d4, `Zeus` 0x1005d704, `Ares` 0x1005d790, `Apollo` 0x1005d7a4, `Athena` 0x1005d778, `Aphrod` 0x1005d7bc, `Bacchus` 0x1005d760, `Water` 0x1005d748, `Griffon` 0x1005d718, `Grif` 0x1005d69c; debug format `" GoodbyeRockT %d, %d, %d\n"` 0x100655d0 | `analysis/exports/ENG1.DLL/strings.json` |
| Decoded banks (bank.json count / exported PNGs): 0492 PEGASUS 133/128, 0786 ICET 51/51, 0563 LAVAT 84/84, 0568 ROCKT 66/66, 0483 TORNADOT 40/40, 0000 ZEUS 14/14, 0764 ARES 14/14, 0495 APOLLO 15/15, 0498 ATHENA 15/15, 0502 APHROD 16/16, 0476 BACCHUS 17/17, 0571 WATER 18/18, 0478 GRIFFON 141/110, 0769 GRIF 498/491; all `SRC1\ALIEN2`, 0 errors | `extracted/sprites/<bank>/bank.json` |
| Sonix entries: `SONIX\TITANS\MUSIC.MUS` (592), `SONIX\TITANS\SFX.SFX` (608), `SONIX\HERC_TIT\SFX.SFX` (596) | `extracted/meta/sonix.json` |
| CD music segment TITANS 2:25:10–2:28:33, between CYCLOPS (2:22:19–2:25:10) and TORMENT (2:28:33–2:31:08) | `docs/music-transport.md` |
| The ENG1 hero has a state 27 "cheat fly" (per-tick handler 0x15780, flag 0x8000 + buttons 0xa000); the hero handler table is 0x72040, filled by `HERO_Init` 0x15090, start table 0x5a69c | `docs/gameplay-spec.md` |
| Decoded screen `0562_ENDEASY` reads "Congratulations! But to be a true hero play the game on a higher difficulty setting." `0742_LEV8` is 16 rows short (`ok_truncated`) | `extracted/images/tga/0562_ENDEASY.png`, `docs/formats/tga.md` |
| The archived demos are DEMO, DEMO1_1–1_3, DEMO2_1, DEMO3_1, DEMO5_1–5_2; none has a 9 in its name (the filename → level mapping itself is a LEAD) | `extracted/demos/` |

## VERIFIED-RUNTIME

None yet. No TITANS load has been traced, and there is no demo payload to replay for this level.

## LEAD (sources to check; these are not results)

- **Manual** (`reference/sources.md`): Pegasus flight, elemental Titans and god rescues; Beginner ends after L08, and L09/L10 need Medium or Herculean.
- **Scenario set** (`reference/scenarios/l08-titan-flight/`): the scene groups "Ice, Wind, Lava and Rock Titans" and "Final rescue/flight completion" (`zeus-completion`) are REFERENCED/INFERRED discovery targets, not observations.
- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): its route text stops after L03, so it has no L08 route, enemy or boss information. It lists only passwords for "Titan Flight": Beginner Pegasus, Pegasus, Helmet, Coin; Medium Gladiator, Coin, Coin, Lightning; Herculean Hydra, Pegasus, Pegasus, Helmet. Check them in this build (S6.03).
- **Bank-name hypotheses:**
  - PEGASUS 0492 as the mount/flight form
  - ICET 0786, LAVAT 0563, ROCKT 0568, TORNADOT 0483 as the Ice, Lava, Rock and Wind Titans ("T" read as Titan)
  - ZEUS 0000, ARES 0764, APOLLO 0495, ATHENA 0498, APHROD 0502, BACCHUS 0476 as the rescued gods
  - WATER 0571: ambiguous (a Titan hazard, a god effect, or another level such as L05)
  - GRIFFON 0478 / GRIF 0769: ambiguous; the L03 plan also lists them as winged-panther candidates
- The `GoodbyeRockT` debug string suggests a Rock Titan exit/removal routine. Its caller is a starting point, not a result.
- **TGA screens:** `0743_LEV9` shows an ice figure and a lava figure, which fits a TITANS loading screen if `LEVn` follows internal ID n. `0742_LEV8` would then belong to ID 8 CYCLOPS. `0562_ENDEASY` is the likely Beginner endpoint screen. All three are unconfirmed until a runtime load shows them.

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| Does flight use separate hero states/handlers, the Pegasus actor driving the hero, or a mode flag over the existing table 0x72040? Is state 27 (cheat fly) related? | `L08.RE.03`; `L08.TR.03` logs hero state id 0x72108, handler and position per tick |
| How do the pad bits map to flight axes (horizontal, vertical, depth), and which attacks remain? All spawns sit at z = 128; whether the hero leaves that plane is unknown | `L08.RE.03` input decode; `L08.TR.03` `input.jsonl` vs position |
| Is the camera player-driven or a forced scroll? How do the 546 camera records and the 4 streamed blocks drive it? | `L08.RE.03`, `L08.TR.04` (stream-chunk 0x1f9e0 events, camera state) |
| Which spawn kinds are the Titans, gods, Pegasus and other hazards? | `L08.RE.02` type table + `L08.TR.04`–`06` spawn events |
| Titan count, order, attacks, damage, defeat/avoid conditions and difficulty scaling | `L08.RE.04`, `L08.TR.05`, `L08.TR.09` |
| What a god rescue requires, what it rewards, and whether it is required for completion | `L08.RE.05`, `L08.TR.06` |
| What ends the level (final rescue, exit trigger, Zeus or something else)? | `L08.RE.06`, `L08.TR.07` |
| What follows L08 on Beginner (ENDEASY? title? credits?) | `L08.RE.06` difficulty branch; `L08.TR.09` → S6.06 |
| Meaning of descriptor fields 0x10063108 / 0x100630e4 / 0x100630f8 and the config-record fields for ID 9 | `L08.RE.01`, S2.05 |
| Checkpoint IDs and respawn positions during flight | `L08.TR.08`, S6.04 |

## Notes
