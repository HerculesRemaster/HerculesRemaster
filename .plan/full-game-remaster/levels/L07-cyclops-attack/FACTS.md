# L07 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 8 CYCLOPS CHASE, ENG3 descriptor 0x385e8, BIN 491/`4482B8EF`, CIF 494/`4582B0EF`, hashes as in README; BIN magic `69312e61` (`i1.a`), CIF magic `00000005` | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| The ID 8 descriptor record holds `0x10038654` (`O:\CYCLOPS\`), `0x125184`, `0x0`, `0x100383a0`, `0x10038388`, `0x10038398` | `reference/binary-analysis/dispatch-tables.json` |
| Field +4 (0x125184 = 1,200,516) equals the CYCLOPS BIN payload size | `dispatch-tables.json` compared with `levels.json` |
| `analysis/exports/ENG3.DLL/strings.json` lists `O:\GAUNTLET\` and `O:\TORMENT\` but not `O:\CYCLOPS\`; the descriptor's path pointer resolves to it | `analysis/exports/ENG3.DLL/strings.json`, `dispatch-tables.json` |
| ENG3 0x1a930 loads the map through the descriptor indexed by the game-state level byte; `MOD_Tick` init passes field +0x14 (ID 8: 0x10038398) to `FUN_1001fa40` | `reference/binary-analysis/ENG3.DLL-1001a930.c`, `ENG3.DLL-1001abb0.c` |
| No stream-chunk routine is recorded for CYCLOPS (`stream_chunk: null`); palette EXE 0x5760 selector 8, path string 0x3666c; module switch EXE 0x4040 | `reference/levels.json` |
| ENG3 strings include `P:\SRC3\ALIEN1\CYCLOPS`, `CYEND` and `PEOPLE`; decoded banks 0480 SRC3_ALIEN1_CYCLOPS, 0493 SRC3_ALIEN1_CYEND, 0547 SRC3_ALIEN1_PEOPLE exist, plus the shared ENG3 hero banks 0561, 0553, 0577 | `analysis/exports/ENG3.DLL/strings.json`, `extracted/sprites/` |
| The archive holds eight demo payloads (DEMO, DEMO1_1–1_3, DEMO2_1, DEMO3_1, DEMO5_1, DEMO5_2). None is named for ID 8 | `extracted/demos/`, `P5-levels-gameplay.md` (`P5.ALL.06`) |
| The CYCLOPS CD-music segment is 2:22:19–2:25:10 on track 2; no CD audio is preserved (Q-01) | `docs/music-transport.md` |
| No decoded map exists: `tools/level/level_decode.py` raises on non-`i2.5` tags | `tools/level/level_decode.py` |

## VERIFIED-RUNTIME

None yet. No CYCLOPS load has been traced, and no attract demo is known for this level.

## LEAD (sources to check; these are not results)

- **Manual** (`reference/sources.md`): Cyclops alternate routes and traffic. The scenario designs built from it (REFERENCED):
  - `alternate-lanes`: stairs on both sides, lane/depth changes, coins, returns to the street
  - `obstacles`: columns, citizens and chariots as obstacles and cross traffic
  - `chase-opening` / `chase-completion`: Cyclops presence at the start, a Cyclops outcome at the end, then the Titan Flight transition
- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): it has no Cyclops Attack walkthrough section; only a password table.
- **Bank-name hypotheses:** 0480 CYCLOPS as the chasing Cyclops; 0493 CYEND as the end sequence; 0547 PEOPLE as the citizens; 0499 ALIEN1 for other street actors.
- **Walkthrough Medium password:** Helmet, Pegasus, Hercules' Head, Archer. The walkthrough also lists Beginner and Herculean entries. Check them in this build (S6.03).
- **L02 runner rules** carry over to L07 only where code shows the same handlers run.

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| What do descriptor fields +0xc, +0x10 and +0x14 point to for ID 8, and how do they differ from ID 2? | `S3.02`, then `L07.RE.01` |
| How are stairs and alternate lanes stored in `i1.a`, and are they the same branch mechanism as the GAUNTLET crossroads? | `L07.RE.03` (after `L02.RE.03`), `L07.TR.04` |
| Is the obstacle/traffic schedule fixed map data, timed, or position-triggered? Which kinds are columns, citizens and chariots? | `L07.RE.02`, `L07.TR.05` |
| What is the Cyclops actor: a scripted pursuer, a hazard source, or a camera-bound sprite? Can it catch the hero? | `L07.RE.04`, `L07.TR.03` |
| What ends the level, and is CYEND a separate scene, a bank swap, or a cutscene? | `L07.RE.05`, `L07.TR.06` |
| Checkpoint IDs and the respawn rule in a chase | `L07.TR.07`, S6.04 |
| Which banks load in L07 (including 0503/0601)? | `L07.TR.10` FS opens, S4.02 |
| What state carries across the L06 → L07 and L07 → L08 module switches? | `L07.TR.09`, S6.05 |
