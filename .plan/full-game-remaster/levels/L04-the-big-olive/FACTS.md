# L04 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 5 VISIT TO THEBES, ENG1 descriptor 0x5a708, BIN 513/`496E5AF9`, CIF 514/`496F5AF1`, hashes as in README; TITLE 0x234ec / module pointer 0x2229c; palette selector 5, path string 0x36714 | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| ENG1 per-level table 0x68af0 lists IDs 0, 1, 3, 5, 6, 7, 9, 11. ID 5 is THEBES; no other ID in the table names THEBES | `docs/level-format.md` |
| Descriptor 0x5a708 values: path pointer 0x1005a7f0 (`O:\THEBES\`), header size 0xfc800, +8 = 0, then 0x10061780 / 0x10061738 / 0x10061778. ENG1 descriptor slots 2, 8 and 10 are zero | `reference/binary-analysis/dispatch-tables.json` |
| Header size 0xfc800 = first block sector 505 × 0x800, matching the rule "header size = first block's sector × 0x800" | `docs/level-format.md`, `extracted/levels/THEBES/level.json` |
| Decoded map: **1 section**, 5 layers, 300×21 cells, 8 blocks (first columns 0, 21, 58, 99, 122, 169, 198, 235), 587 spawns from 104 templates, spawn x 130–19083 | `extracted/levels/THEBES/level.json` |
| Spawn kinds: 87 distinct. Most frequent: 0x2018 ×110, 0x2019 ×110, 0x25 ×39, 0x2013 ×24, 0x202b ×21, 0x30/0x31/0x3e ×18, 0x32 ×13. Kinds index the per-level type table, so their meaning is per level | `extracted/levels/THEBES/level.json` |
| All decoded camera records are type 0 (straight, per the camera record layout). `n_anim` 0, `n_scroll` 0 | `extracted/levels/THEBES/level.json`, `docs/level-format.md` |
| `HERO_Init` 0x15090 takes the start position from the level start table 0x5a69c[level] + 0x18·start index; the demo's byte 0 is the start index | `docs/gameplay-spec.md`, `docs/demo-format.md` |
| ENG1 strings include `M:\DEMO\DEMO5_1.DAT` (0x1004da94), `M:\DEMO\DEMO5_2.DAT` (0x1004daa8), `q:\HERC_THE\` (0x10051844), and SRC1 bank paths `ALIEN1\THEBIANS\{THHAFOOD,THCHAR,THEWGUY,THUGS,THBURNT,THFLASH,THOLD,THFAT,THEQ,THFLOOD,THSNOW}`, `ALIEN1\{HARPIES,MINOTAUR,SKELETON,PANIC,PAIN}`, `ALIEN2\{Harp,RcRock,RtRock,Merv,ALEXFISH}` | `analysis/exports/ENG1.DLL/strings.json` |
| Decoded demos: `DEMO5_1` (archive index 470) start index 6, `DEMO5_2` (index 472) start index 2, 881 ticks each | `extracted/demos/DEMO5_1.json`, `extracted/demos/DEMO5_2.json` |
| CD music table has a THEBES segment 2:15:29–2:18:33 (between NESSUS and HYDRA) | `docs/music-transport.md` |
| Decoded banks exist for 0329 THEQ, 0484 THHAFOOD, 0763 THCHAR, 0766 THSNOW, 0767 THFLOOD, 0768 THFLASH, 0800 THFAT, 0808 THBURNT, 0815 THEWGUY, 0817 THUGS, 0820 THOLD, 0479 HARPIES, 0774 HARP, 0496 MINOTAUR, 0486 SKELETON, 0532 PANIC, 0524 RCROCK, 0525 RTROCK, 0785 MEG, 0793 PIG, 0807 SPY, 0799 MERV, 0477 ALEXFISH, 0812 PAIN (presence only, not level use) | `extracted/sprites/` |

## VERIFIED-RUNTIME

None yet. No THEBES load has been traced. The two `DEMO5_*` payloads have not been replayed under trace.

## LEAD (sources to check; these are not results)

- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): it is incomplete (v0.5) and only walks L01–L03. For L04 it has password tables only, split into §3.4.4 "The Big Olive — Part 1" and §3.4.5 "Part 2".
  - Part 1 Medium: Nessus, Coin, Hydra, Hercules' Head
  - Part 2 Medium: Gladiator, Hydra, Archer, Gladiator
  - Beginner and Herculean values for both parts are in the same tables.

  Check them in this build (S6.03). Two passwords suggest a resumable point inside L04; they do not prove a second map or ID.
- **Community password guide** (`reference/sources.md`): it also reports Big Olive parts. Search lead only.
- **Manual** (`reference/sources.md`): Thebes stairs, enemies, harpy rescues and debris.
- **Scenario groups** (`reference/scenarios/l04-the-big-olive/`): streets, background stairs, thugs/skeletons/Minotaur, harpy and citizen rescues, destruction/debris/secrets.
- **Demo filenames:** `DEMO5_1`/`DEMO5_2` are read as ID 5 by name only. Their different start indices (6 and 2) are a hint for the substage question, not a result.
- **Bank-name hypotheses:**
  - THEBIANS_* as citizens (rescue targets, PANIC as fleeing crowd)
  - THUGS as thugs
  - HARPIES / HARP as harpies
  - MINOTAUR, SKELETON as enemies
  - RCROCK/RTROCK as debris or falling rock (ambiguous; may belong to another level)
  - MEG, PIG, SPY, MERV, ALEXFISH, PAIN as story/NPC actors. MEG, PIG and SPY are not found in any `analysis/exports/*/strings.json`.
- **Walkthrough "Hermes Level Marker"** (HLM) as the checkpoint object (S6.04).

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| What is "Part 2"? A start index in the one THEBES map, a reload of ID 5 with another start index, a separate load, or only a password resume point? | `L04.RE.01` (start table for ID 5), `L04.RE.03` (static code path), `L04.TR.02` (demo starts), `L04.TR.05` (boundary trace) |
| How many start records does ID 5 have, and where do indices 2 and 6 put the hero? Is descriptor field +0xc (0x10061780) the start table that `HERO_Init` reads? | `L04.RE.01` |
| Can a password encode a start index or substage? | S6.02; `L04.TR.05` with an accepted Part 2 password |
| Which spawn kinds are the thugs, skeletons, Minotaur, harpies and citizens? | `L04.RE.02` type table + `L04.TR.04` spawn events |
| Is the Minotaur a boss encounter or a regular enemy? | `L04.RE.05`, `L04.TR.04` |
| Rescue, destruction and debris trigger logic; secret locations | `L04.RE.04`, `L04.TR.04` |
| Which of the ambiguous banks (RCROCK/RTROCK, MEG, PIG, SPY, MERV, ALEXFISH, PAIN, SKELWAR) load in L04 | `L04.TR.03`–`04` FS opens; S4.01 |
| Checkpoint IDs and Hermes-marker positions | `L04.TR.06`, S6.04 |
| Whether the L03 → L04 change (both ENG1) reloads the module or only the map | `L04.TR.08` |
| Whether `q:\HERC_THE\` is the L04 Sonix directory | S5.03, `L04.AS.05` |

## Notes
