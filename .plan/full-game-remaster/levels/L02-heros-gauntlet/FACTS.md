# L02 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 2 TRAINING GAUNTLET, ENG3 descriptor 0x38558, BIN 557/`57E5A3D6`, CIF 556/`57DDA3D7`, hashes as in README; BIN magic `69312e61` (`i1.a`), CIF magic `00000005` | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| ENG3 descriptor table 0x38528 has non-zero records only for IDs 2, 8 and 10. The ID 2 record holds `0x10038660` (`O:\GAUNTLET\`), `0x129e28`, `0x0`, `0x100382c8`, `0x100382b8`, `0x100382c0` | `reference/binary-analysis/dispatch-tables.json` |
| Field +4 (0x129e28 = 1,220,136) equals the GAUNTLET BIN payload size | `dispatch-tables.json` compared with `levels.json` |
| ENG3 0x1a930 indexes the descriptor table by the game-state level byte (`*DAT_100407a4 * 0x18`), opens `MAP_PSX.BIN` under the record's path, and hangs if the BIN tag (+0x90) differs from the engine tag (+0x8c) | `reference/binary-analysis/ENG3.DLL-1001a930.c` |
| On its init pass, `MOD_Tick` 0x1abb0 passes descriptor field +0x14 (ID 2: 0x100382c0) to `FUN_1001fa40`, then calls 0x1a930 | `reference/binary-analysis/ENG3.DLL-1001abb0.c` |
| No stream-chunk routine is recorded for GAUNTLET (`stream_chunk: null`); map load ENG3 0x1a930; palette EXE 0x5760 selector 2, path string 0x36684; module switch EXE 0x4040 | `reference/levels.json` |
| ENG3 strings include `O:\GAUNTLET\`, the SRC3 bank paths `P:\SRC3\ALIEN1\{ALIEN1,CYCLOPS,CYEND,PEOPLE,SOULS,TORMENT}` and `P:\SRC3\HERO\{HERCULES,LETTERS,XTRA_DB1}`, `M:\DEMO\DEMO2_1.DAT` and `stpanel2.dat` | `analysis/exports/ENG3.DLL/strings.json` |
| Decoded SRC3 banks exist: 0480 CYCLOPS, 0482 TORMENT, 0493 CYEND, 0499 ALIEN1, 0547 PEOPLE, 0553 XTRA_DB1, 0561 HERCULES, 0569 SOULS, 0577 LETTERS. Banks 0503 and 0601 hold only `bank.json` + `palettes.png` (unsupported variants) | `extracted/sprites/` |
| `DEMO2_1` is archived and decoded: `start_index` 0, 881 ticks | `extracted/demos/DEMO2_1.json` |
| The GAUNTLET CD-music segment is 2:06:55–2:08:55 on track 2; no CD audio is preserved (Q-01) | `docs/music-transport.md` |
| `tools/level/level_decode.py` raises `ValueError` on any tag other than `i2.5`, so GAUNTLET has no decoded map | `tools/level/level_decode.py` |

## VERIFIED-RUNTIME

- The GPU-renderer dev overlay `analysis/runtime/levels/gpu_10500.txt` shows `module ENG3 map o:\gauntlet\map_psx.cif` at frame 10500 of an unattended GPU-renderer run. That was presumably the attract cycle; the file itself records no demo ID. This is one overlay dump of a load and a drawn frame. It has no tick range and no gameplay state, and it does not identify the demo being played.

No GAUNTLET gameplay has been traced.

## LEAD (sources to check; these are not results)

- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md` § 3.2, written for "Normal" difficulty):
  - the camera follows Hercules from behind; Up speeds him up, Down slows him, and he cannot stop; only the jump button is available besides the arrows
  - two crossroads, each a left/right choice; the author describes the right-hand path only
  - hazards: rotating blades, hanging/swinging blades, a moving hammer, stone punches, stone hands (including falling ones), hand hammers, rotating stone and wooden bladed rotators, rolling stones, boulders, falling pillars (jump them, or slow down and pass after they fall), small ponds crossed by jumping on two platforms
  - after the Hermes Sandals the hero runs fast and boulders can be smashed (one smashed boulder gives a password vase)
  - enemies run toward the hero; dummy enemies and damsel dummies appear along the route
  - pickups: coins, the letters H-E-R-C-U-L-E-S, Herculade bottles, action figures, password vases, two Hermes level markers on the right-hand branches, and the Helmet of Invincibility, which activates automatically
  - the level ends with the score screen (hero rating and coin percentage)
- **Demo filename:** `DEMO2_1` belongs to ID 2 by the `DEMO<id>_n` naming convention only.
- **Bank-name hypotheses:** 0499 ALIEN1 as the GAUNTLET enemy/hazard bank; 0547 PEOPLE as the damsel dummies; 0561 HERCULES, 0553 XTRA_DB1 and 0577 LETTERS as shared ENG3 hero/HUD banks.
- **Manual** (`reference/sources.md`): training, traversal and hidden routes; the scenario designs for opening, hazards and rescues are REFERENCED from it.
- **Walkthrough Medium password:** Hydra, Medusa, Coin, Medusa. The walkthrough also lists Beginner and Herculean entries. Check them in this build (S6.03).

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| What do descriptor fields +0xc, +0x10 and +0x14 point to for ID 2, and does ENG3 have a per-level config table? | `S3.02`, then `L02.RE.01` |
| How are the crossroads stored in `i1.a` (branch records, lane tables, rejoin points), and which input selects a branch? | `S3.03`, `L02.RE.03`, `L02.TR.04` |
| Which obstacle/actor kinds does GAUNTLET use, and which handlers and banks implement each hazard? | `L02.RE.02` + `L02.TR.05` spawn/collision events |
| How do the Hermes Sandals change speed and allow boulder smashing, and how does the Helmet of Invincibility change collision? | `L02.RE.04`, `L02.TR.05` |
| Checkpoint IDs, Hermes-marker positions, and the respawn rule | `L02.TR.07`, S6.04 |
| Does DEMO2_1 start at start index 0 in GAUNTLET, and which branch does it take? | `S3.08`, `L02.TR.02` |
| Which banks load in L02 (including whether 0503/0601 are ENG3 banks at all)? | `L02.TR.10` FS opens, S4.02 |
| What state carries across the L01 → L02 module switch (lives, score, power-ups)? | `L02.TR.09`, S6.05 |
