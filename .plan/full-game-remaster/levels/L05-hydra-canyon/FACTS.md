# L05 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 6 HYDRA BATTLE, ENG1 descriptor 0x5a720, BIN 541/`527C4FA9`, CIF 517/`4A7C50A9`, hashes as in README; TITLE 0x234f8 / module pointer 0x222a0; palette selector 6, path string 0x366fc | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| ENG1 per-level table 0x68af0 includes ID 6 | `docs/level-format.md` |
| Descriptor 0x5a720 values: path pointer 0x1005a7e4 (`O:\HYDRA\`), header size 0xf0800, +8 = 0, then 0x10058878 / 0x10058860 / 0x10058870 | `reference/binary-analysis/dispatch-tables.json` |
| Header size 0xf0800 = first block sector 481 × 0x800 | `docs/level-format.md`, `extracted/levels/HYDRA/level.json` |
| Decoded map: 1 section, 2 layers, 80×18 cells, 1 block (sector 481, 6 sectors), **2 spawns**, both kind 0x31 (templates 12 and 11, x 1506 and 2980, flags 129). Kinds index the per-level type table, so 0x31 here need not match 0x31 in FOREST or THEBES | `extracted/levels/HYDRA/level.json` |
| Camera records: 99, of which 96 are type 1 (arc) and 3 type 0 (straight). `n_anim` 0, `n_scroll` 0 | `extracted/levels/HYDRA/level.json`, `docs/level-format.md` (type 1/2 = arc) |
| Decoder warning: "VRAM image runs 3156 bytes past the end of the file (padded with 0)" | `extracted/levels/HYDRA/level.json`; listed in S2 |
| T28 points to 3D models drawn by `GFX_DrawModel` ENG1 0x31540 | `docs/level-format.md` (via S2) |
| ENG1 strings include `O:\HYDRA\` (0x1005a7e4) and `P:\SRC1\BOSS\HYDRA` (0x1005d548) | `analysis/exports/ENG1.DLL/strings.json` |
| The ENG1 string export contains no HYDRA-named `q:\` Sonix directory (it has `q:\HERC_THE\` and `q:\HERC_MED\`) | `analysis/exports/ENG1.DLL/strings.json` |
| No HYDRA demo: the ENG1 demo strings and `extracted/demos/` cover only DEMO, DEMO1_1–1_3, DEMO2_1, DEMO3_1, DEMO5_1, DEMO5_2 | `analysis/exports/ENG1.DLL/strings.json`, `extracted/demos/` |
| Decoded bank 0804 BOSS_HYDRA (`M:\GRAFIX\CHOP\SRC1\BOSS\HYDRA\ANIMPSX.BIN`, 0 decode errors) | `extracted/sprites/0804_SRC1_BOSS_HYDRA/bank.json` |
| CD music table has a HYDRA segment 2:18:33–2:20:56 (between THEBES and MEDUSA) | `docs/music-transport.md` |

## VERIFIED-RUNTIME

None yet. No HYDRA load has been traced, and there is no demo to replay.

## LEAD (sources to check; these are not results)

- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): password table only (§3.4.6), since the walkthrough stops after L03. Medium: Coin, Helmet, Coin, Gladiator. Check it in this build (S6.03).
- **Manual** (`reference/sources.md`): Hydra encounter.
- **Scenario groups** (`reference/scenarios/l05-hydra-canyon/`): canyon entry (with crowd/UI and boss initialization), attack cycles, damage with head/state changes, defeat and exit, death/retry.
- **Head cutting and regrowth**, and any phase structure: general Hydra lore and scenario wording only. The phase count stays null until observed.
- **Hydra drawn with T28 models**: hypothesis from the arena's 2 layers, few spawns and arc camera records. It could also be sprite-only (bank 0804), or both.
- **Kind 0x31 is the Hydra (or its parts)**: hypothesis only, because they are the only spawns.
- **Bank-name attribution:** 0804 BOSS_HYDRA to L05 by name.

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| What do the two kind-0x31 spawns create, and where does the Hydra's own logic live (type handler, level script, or both)? | `L05.RE.02`, `L05.RE.03`, `L05.TR.02` spawn events |
| Is the Hydra drawn with T28 models, sprite frames from 0804, or both? | `L05.RE.04` with S2.01; `L05.TR.09` draw pairs |
| Head count, head-cut/regrowth rules, health, damage values, phases and difficulty scaling | `L05.RE.03`, `L05.TR.03`–`04`, `L05.TR.07` |
| Cause of the 3,156 B VRAM overrun (streamed, partial, or decoder size rule) | S2.03 with a runtime VRAM dump in `L05.TR.02` |
| Retry point after death inside the fight; does the boss reset? | `L05.TR.06`, S6.04 |
| Which Sonix directory and SFX the arena uses | S5.01/S5.03, `L05.AS.05` |
| Whether the L04 → L05 and L05 → L06 changes (all ENG1) reload the module or only the map | `L05.TR.08` |
| Whether any NPC/crowd bank loads (the scenario expects `npc_animation`) | `L05.TR.02` FS opens |

## Notes
