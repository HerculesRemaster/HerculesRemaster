# L03 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 3 FOREST OF CENTAURS, ENG1 descriptor 0x5a6d8, BIN 515/`49716801`, CIF 516/`497267F9`, hashes as in README | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| ENG1 per-level table 0x68af0 lists IDs 0, 1, 3, 5, 6, 7, 9, 11, with **no ID 4** | `docs/level-format.md` |
| ID 4 descriptor: ENG1 0x5a6f0, TITLE 0x234e0, path `O:\NESSUS\`; no named NESSUS BIN/CIF in the archive | `reference/levels.json` `extra_stage_descriptors`, `reference/level-map.md` |
| The ID 4 descriptor's header-load field is 0x131000. It differs from every gameplay descriptor, including FOREST's 0x106800 (field +4 = header size, `docs/level-format.md`) | `reference/binary-analysis/dispatch-tables.json` |
| ENG1 strings include `O:\NESSUS\`, `q:\NESSUS\`, and `P:\SRC1\ALIEN2\NESSUS\{Basic,Punch,Kick,Look,Ride,Reject,Finish,Defeat}` | `analysis/exports/ENG1.DLL/strings.json` |
| The CD music table has a separate NESSUS segment (2:12:53–2:15:29), between FOREST (2:08:55–2:12:53) and THEBES | `docs/music-transport.md` |
| Decoded map: 1 section, 11 layers, 310×14 cells, 6 blocks, 283 spawns from 67 templates, spawn x 798–19118 | `extracted/levels/FOREST/level.json` |
| Most frequent spawn kinds: 0x2b ×41, 0x2e ×36, 0x2019 ×33, 0x2013 ×23, 0x31 ×11, 0x51 ×10. Kinds index the per-level type table, so their meaning is per level | `extracted/levels/FOREST/level.json` |
| Decoded Nessus banks: 0599 BASIC, 0602 KICK, 0603 RIDE, 0609 LOOK, 0614 PUNCH, 0797 FINISH, 0818 DEFEAT, 0819 REJECT | `extracted/sprites/` |

## VERIFIED-RUNTIME

None yet. No FOREST load has been traced. A DEMO3_1 payload exists, with start index 1 (`extracted/demos/DEMO3_1.json`), but it has not been replayed under trace.

## LEAD (sources to check; these are not results)

- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`):
  - background paths reached with Up; Power Punch on stone barriers; ground slam on cracks; tree climbing
  - enemies: thugs, vultures, centaurs (special swords needed) and winged panthers
  - the boss is Nessus, inside this level. Weapons don't hurt him. Jump his punch and kick, then ride him while he "looks" confused; repeat until his health bar empties, then finish with a Power Punch.
- **Bank-name hypotheses:**
  - ARCHERC 0474, WRESTLEC 0535 and ROCKC 0542 as centaur variants (reading "C" as centaur)
  - GRIFFON 0478 / GRIF 0769 as the winged panther
  - STYBIRD 0485 as the vulture
- **Manual** (`reference/sources.md`): forest background paths and Nessus.
- **Walkthrough Medium password:** Nessus, Hercules' Head, Minotaur, Archer. Check it in this build (S6.03).

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| Is the Nessus fight a separate ID-4 stage load, an in-map section of FOREST, or an ENG1 mode with no map change? | `L03.RE.03` code path from the ID-4 descriptor; `L03.TR.04` traces the level-ID global and map/bank opens across the fight |
| Which payload, if any, would satisfy the ID-4 header size 0x131000? No named map exists; check the unnamed payloads in `P4.ALL.09` and the FS opens in `L03.TR.04` | `L03.RE.03` |
| Which spawn kinds are the centaurs, panthers, vultures and thugs? | `L03.RE.02` type table + `L03.TR.03` spawn events |
| Nessus phase count, health, damage values, and difficulty scaling | `L03.RE.04`, `L03.TR.05`, `L03.TR.07` |
| Checkpoint IDs and Hermes-marker positions | `L03.TR.06`, S6.04 |
| Whether DEMO3_1 starts at start index 1 in FOREST | `L03.TR.02` |
