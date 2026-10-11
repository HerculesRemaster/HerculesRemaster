# L10 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 11 VORTEX OF SOULS, ENG1 descriptor 0x5a798, BIN 518/`4B806805`, CIF 519/`4B8167FD`, offsets and hashes as in README; TITLE 0x23534, module pointer 0x222b4; palette selector 11, path 0x366b4 | `reference/levels.json`, `reference/level-map.md` |
| `levels.json` lists Medium and Herculean as playable and records `beginner_unavailable` [L09, L10], sourced from the Windows manual with the build-runtime check pending | `reference/levels.json` |
| ENG1 per-level table 0x68af0 (0x48 B per ID, 999 ends) lists IDs 0, 1, 3, 5, 6, 7, 9 and **11** | `docs/level-format.md` |
| Descriptor 0x5a798 values: path `O:\VORTEX\` (0x1005a7c0), header size 0xdf800, 0, then 0x1006b178 / 0x1006b158 / 0x1006b168 (meaning not yet recovered) | `reference/binary-analysis/dispatch-tables.json` |
| The header size 0xdf800 equals the decoded BIN `header_size` 915,456 = block sector 447 × 0x800 (`docs/level-format.md`, offset 0x04) | `extracted/levels/VORTEX/level.json`, `docs/level-format.md` |
| TITLE rows: ID 10 PASSAGEWAYS TORMENT → `\SRC3\ENGINE\` ENGINE3; ID 11 → `\SRC1\ENGINE\` ENGINE1. L09 → L10 crosses an engine boundary | `reference/binary-analysis/dispatch-tables.json` |
| Decoded map: 1 section, **1 layer**, 20×20 cells, 1 block (sector 447, 1 sector), **0 spawns**, 2 camera records, 18 tile-mesh `types` entries, 49 texture entries; header `n_anim` 0, `n_scroll` 1, `t28` 13776 | `extracted/levels/VORTEX/level.json` |
| Decoder warning: "VRAM image runs 13816 bytes past the end of the file (padded with 0)". S2 lists it with HYDRA (3,156 B) and MEDUSA (84,096 B) | `extracted/levels/VORTEX/level.json`, `_shared/S2-eng1-map-completion.md` |
| ENG1 strings include `P:\SRC1\BOSS\HADES` (0x1005d534) and `P:\SRC1\ALIEN1\PANIC` / `PAIN` (0x1005db60 / 0x1005db78), plus `q:\TORMENT\` (0x100518ac) and `q:\HERC_TOR\` (0x100517f4). The export has **no** `O:\VORTEX\`, `q:\VORTEX\` or `q:\HERC_VOR\` even though the descriptor points at 0x1005a7c0, so the string export is incomplete for this level | `analysis/exports/ENG1.DLL/strings.json`, `reference/binary-analysis/dispatch-tables.json` |
| Bank 0803 `SRC1\BOSS\HADES`: bank.json count 698, 516 PNG frames exported, 8 palettes, 0 errors. Bank 0569 `SRC3\ALIEN1\SOULS` (an ENG3/SRC3 path): count 20 | `extracted/sprites/0803_SRC1_BOSS_HADES/bank.json`, `extracted/sprites/0569_SRC3_ALIEN1_SOULS/bank.json` |
| Sonix entries: `SONIX\VORTEX\MUSIC.MUS` (604), `SONIX\VORTEX\SFX.SFX` (615), `SONIX\HERC_VOR\SFX.SFX` (598) | `extracted/meta/sonix.json` |
| CD music segment VORTEX 2:31:08–2:34:20, the last level segment in the table. No separate ending or credits segment is listed (OPTIONS has seq 1–4) | `docs/music-transport.md` |
| Decoded screens: `0622_C1` shows the title "CREDITS"; there are 31 `C*` screens (C1–C31) and 10 `SEQ*` screens; `0562_ENDEASY` reads "Congratulations! But to be a true hero play the game on a higher difficulty setting."; `LEV1`–`LEV12` exist. TITLE banks include `CHOP_TITLE_SEQ1`–`SEQ10`, `TALLYUP` 0639, `PSWD` 0554 | `extracted/images/tga/`, `docs/formats/tga.md`, `extracted/sprites/` |

## VERIFIED-RUNTIME

None yet. No VORTEX load has been traced, and no archived demo has an 11 in its name (`extracted/demos/`).

## LEAD (sources to check; these are not results)

- **Manual** (`reference/sources.md`): the final two levels require Medium or Herculean. The manual does not describe the final-level objectives in detail.
- **Scenario set** (`reference/scenarios/l10-vortex-of-souls/`): "souls", "rescue objective" and "final encounter" are INFERRED/REFERENCED discovery targets.
- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): its route text stops after L03. For "Vortex of Souls" it lists only Medium Gladiator, Lightning, Gladiator, Nessus and Herculean Coin, Hydra, Minotaur, Gladiator (no Beginner entry, which fits the manual). It also lists a "View FMV Sequences" password: Medium Pegasus, Gladiator, Nessus, Gladiator; Herculean Nessus, Medusa, Helmet, Hydra. Check them in this build (S6.03); the FMV password may expose the ending media without playing L10.
- **Final-encounter hypothesis:** the SRC1 `BOSS` banks are SKELWAR 0566, HADES 0803, HYDRA 0804 and MEDUSA 0816. HYDRA and MEDUSA match the L05/L06 names; HADES matches no earlier level name. That makes 0803 a candidate for L10, nothing more.
- Other SRC1 character banks without a level attribution, such as MEG 0785, PAIN 0812 and PANIC 0532, may appear here or elsewhere; only a runtime load in VORTEX attributes them.
- SOULS 0569 is an SRC3 (ENG3) bank, so it most likely belongs to L09 TORMENT, not to this ENG1 level, despite the name.
- **TGA screens:** `0748_LEV11` shows green swirling figures, which fits a VORTEX loading screen if `LEVn` follows internal ID n. `0749_LEV12` (a row of figures holding X cards), the `SEQ*` screens and the `C*` credits are ending/credits candidates. `0562_ENDEASY` is a Beginner-endpoint candidate, which belongs to L08/S6.06.
- The ENG1 references to `q:\TORMENT\` / `q:\HERC_TOR\` may mean ENG1 code loads TORMENT audio (for example in VORTEX). Unconfirmed.

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| With 0 map spawns, where are the final-encounter actor(s), souls and hazards created: the level config, the type table's init path, a level-ID switch, or a script? | `L10.RE.02`; `L10.TR.02`/`05` log spawn events with caller RVA |
| Is the vortex drawn from T28 models, effects or the single 20×20 layer? What do the 18 tile meshes and the scrolling texture show? | `L10.RE.03` with S2.01/S2.04 |
| What movement mode does the hero use in the vortex (standard ENG1 states, flight, falling, something else)? | `L10.RE.03`; `L10.TR.03` |
| Final encounter: actor identity, health, attacks, vulnerable windows, phase count (null until observed), difficulty scaling | `L10.RE.04`, `L10.TR.05`, `L10.TR.09` |
| What is the rescue objective, and is it a precondition for completion? | `L10.RE.05`, `L10.TR.06` |
| Which module plays the ending and credits, which screens/banks and music it uses, and whether any external movie media is requested | `L10.RE.06`, `L10.TR.07` |
| Is the 13,816 B VRAM overrun a decoder size-rule error or a streamed/partial image? (The overrun equals the header's `img0` value 13,816 here, but HYDRA's does not match its `img0`, so this may be coincidence.) | S2.03 with a runtime VRAM dump from `L10.TR.02` |
| Which Sonix directories VORTEX actually opens (`VORTEX`, `HERC_VOR`, `TORMENT`, `HERC_TOR`) | `L10.RE.07`; `L10.TR.02` FS opens |
| Is L10 unreachable on Beginner by normal play, password and F3? | `L10.TR.10` with S6.06 |
| Meaning of descriptor fields 0x1006b178 / 0x1006b158 / 0x1006b168 and the config-record fields for ID 11 | `L10.RE.01`, S2.05 |

## Notes
