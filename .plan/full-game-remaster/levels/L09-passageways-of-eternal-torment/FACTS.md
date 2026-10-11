# L09 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 10 PASSAGEWAYS TORMENT, ENG3 descriptor 0x38618, BIN 520/`4B83C8E3`, CIF 521/`4C83C0E3`, hashes as in README; BIN magic `69312e61` (`i1.a`), CIF magic `00000005` | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| The ID 10 descriptor record holds `0x10038648` (`O:\TORMENT\`), `0x10903c`, `0x0`, `0x10038450`, `0x10038430`, `0x10038440` | `reference/binary-analysis/dispatch-tables.json` |
| Field +4 (0x10903c = 1,085,500) equals the TORMENT BIN payload size | `dispatch-tables.json` compared with `levels.json` |
| ENG3 0x1a930 loads the map through the descriptor indexed by the game-state level byte; `MOD_Tick` init passes field +0x14 (ID 10: 0x10038440) to `FUN_1001fa40` | `reference/binary-analysis/ENG3.DLL-1001a930.c`, `ENG3.DLL-1001abb0.c` |
| No stream-chunk routine is recorded for TORMENT (`stream_chunk: null`); palette EXE 0x5760 selector 10, path string 0x3669c; module switch EXE 0x4040 | `reference/levels.json` |
| ENG3 strings include `O:\TORMENT\`, `P:\SRC3\ALIEN1\TORMENT` and `P:\SRC3\ALIEN1\SOULS`; decoded banks 0482 SRC3_ALIEN1_TORMENT and 0569 SRC3_ALIEN1_SOULS exist, plus the shared ENG3 hero banks 0561, 0553, 0577 | `analysis/exports/ENG3.DLL/strings.json`, `extracted/sprites/` |
| The archive holds eight demo payloads; none is named for ID 10 | `extracted/demos/`, `P5-levels-gameplay.md` (`P5.ALL.06`) |
| The TORMENT CD-music segment is 2:28:33–2:31:08 on track 2; no CD audio is preserved (Q-01) | `docs/music-transport.md` |
| No decoded map exists: `tools/level/level_decode.py` raises on non-`i2.5` tags | `tools/level/level_decode.py` |
| The scenario set has a Herculean comparison and a Beginner availability check, and no Beginner comparison; coverage marks the Beginner row as an availability check | `reference/scenarios/coverage.md` |

## VERIFIED-RUNTIME

None yet. No TORMENT load has been traced, and no attract demo is known for this level.

## LEAD (sources to check; these are not results)

- **Manual** (`reference/sources.md`): torment passage hazards; the final two levels require Medium or Herculean. `levels.json` lists only Medium/Herculean as playable for L09 and records the source as "Windows manual; build-runtime check pending".
- **Scenario designs** (REFERENCED from the manual): passage opening and successive sections, skeletons and lost souls, a chariot with falling stalactites, falls into the River Styx, and a final passage that leads to the Vortex.
- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): it has no walkthrough section for this level. Its password table lists only Medium and Herculean entries for it, which matches the manual's Beginner restriction.
- **Bank-name hypotheses:** 0482 TORMENT for the passage actors and hazards; 0569 SOULS for the lost souls; 0499 ALIEN1 as a possible shared bank for the other actors.
- **Walkthrough Medium password:** Medusa, Gladiator, Nessus, Pegasus. Check it in this build (S6.03).

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| What do descriptor fields +0xc, +0x10 and +0x14 point to for ID 10, and how do they differ from IDs 2 and 8? | `S3.02`, then `L09.RE.01` |
| How are passage sections and depth routes stored in `i1.a`, and are there mid-level resource loads with no stream-chunk routine? | `L09.RE.03`, `L09.TR.03` |
| Which kinds are skeletons, lost souls, the chariot and stalactites, and do skeletons/souls allow combat or only avoidance? | `L09.RE.02`, `L09.RE.04`, `L09.TR.04` |
| What triggers a River Styx fall, and where does the hero respawn? | `L09.RE.05`, `L09.TR.06`, S6.04 |
| Is the Beginner restriction enforced in TITLE/EXE progression, in password validation, or in ENG3? What does the game do after Beginner L08? | `L09.RE.06`, `L09.TR.09`, S6.02, S6.06 |
| What does the "Vortex transition" consist of: a tally then the L10 load, or a scripted sequence first? | `L09.TR.07` |
| Which banks load in L09 (including 0503/0601)? | `L09.TR.11` FS opens, S4.02 |
| What state carries across the L08 → L09 and L09 → L10 module switches? | `L09.TR.10`, S6.05 |
