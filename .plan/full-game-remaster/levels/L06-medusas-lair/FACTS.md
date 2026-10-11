# L06 facts and evidence status

## VERIFIED-STATIC

| fact | source |
|---|---|
| ID 7 MEDUSA BATTLE, ENG1 descriptor 0x5a738, BIN 530/`508159E4`, CIF 531/`508259DC`, hashes as in README; TITLE 0x23504 / module pointer 0x222a4; palette selector 7, path string 0x366e4 | `reference/levels.json`, `reference/binary-analysis/map-archive-entries.json` |
| ENG1 per-level table 0x68af0 includes ID 7 | `docs/level-format.md` |
| Descriptor 0x5a738 values: path pointer 0x1005a7d8 (`O:\MEDUSA\`), header size 0xcc000, +8 = 0, then 0x1005a030 / 0x1005a018 / 0x1005a028 | `reference/binary-analysis/dispatch-tables.json` |
| Header size 0xcc000 = first block sector 408 × 0x800 | `docs/level-format.md`, `extracted/levels/MEDUSA/level.json` |
| Decoded map: 1 section, 2 layers, 80×18 cells, 1 block (sector 408, 6 sectors), **6 spawns**, all decoded with kind 0x0 and flags 0; templates 1 (×5) and 6 (×1); x 232–6016; three at z 0 and three at z 128 | `extracted/levels/MEDUSA/level.json` |
| Camera records: 80, of which 77 are type 1 (arc) and 3 type 0 (straight). `n_anim` 1 (one animated texture), `n_scroll` 0 | `extracted/levels/MEDUSA/level.json`, `docs/level-format.md` |
| Decoder warning: "VRAM image runs 84096 bytes past the end of the file (padded with 0)". S2 lists the three overruns: HYDRA 3,156 B, MEDUSA 84,096 B, VORTEX 13,816 B | `extracted/levels/MEDUSA/level.json`, `_shared/S2-eng1-map-completion.md` |
| T28 points to 3D models drawn by `GFX_DrawModel` ENG1 0x31540 | `docs/level-format.md` (via S2) |
| ENG1 strings include `O:\MEDUSA\` (0x1005a7d8), `q:\HERC_MED\` (0x10051824), `P:\SRC1\BOSS\MEDUSA` (0x1005d574), `P:\SRC1\BOSS\SKELWAR` (0x1005d55c), `P:\SRC1\ALIEN1\SKELETON` (0x1005d938) | `analysis/exports/ENG1.DLL/strings.json` |
| No MEDUSA demo: the ENG1 demo strings and `extracted/demos/` cover only DEMO, DEMO1_1–1_3, DEMO2_1, DEMO3_1, DEMO5_1, DEMO5_2 | `analysis/exports/ENG1.DLL/strings.json`, `extracted/demos/` |
| Decoded banks exist for 0816 BOSS_MEDUSA, 0486 SKELETON, 0566 BOSS_SKELWAR (presence only, not level use) | `extracted/sprites/` |
| CD music table has a MEDUSA segment 2:20:56–2:22:19 (after HYDRA) | `docs/music-transport.md` |
| L07 CYCLOPS is ID 8 on ENG3 (descriptor ENG3 0x385e8, loader 0x1a930, BIN 491 / CIF 494), so L06 → L07 changes modules; S6.05 lists this transition | `reference/level-map.md`, `_shared/S6-progression-difficulty.md` |

## VERIFIED-RUNTIME

None yet. No MEDUSA load has been traced, and there is no demo to replay.

## LEAD (sources to check; these are not results)

- **Walkthrough** (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`): password table only (§3.4.7), since the walkthrough stops after L03. Medium: Archer, Pegasus, Archer, Nessus. Check it in this build (S6.03).
- **Manual** (`reference/sources.md`): Medusa encounter.
- **Scenario groups** (`reference/scenarios/l06-medusas-lair/`): lair traversal and entry, skeleton encounters, "Medusa gaze and venom", a mirror/reflection interaction leading to defeat, retry.
- **Reflection-defeat mechanic**: from the scenario wording and the manual's encounter group only. What is reflected, by what object, and what it does to Medusa are unknown.
- **Gaze effect on the hero** (status effect, damage, or instant loss): unknown; no source in this repo states it.
- **Bank-name attribution:** 0816 BOSS_MEDUSA to L06 by name; 0486 SKELETON is also a candidate for L04; 0566 BOSS_SKELWAR could belong to L06, L04 or another ENG1 level.
- **Kind 0x0 spawns are skeletons or lair props**: hypothesis only.

## UNRESOLVED (with next step)

| question | next step |
|---|---|
| Is kind 0x0 (with flags 0) a real type-table index, or does the decoder read these six spawns differently from THEBES/HYDRA? What do templates 1 and 6 create? | `L06.RE.02` against `tools/level/level_decode.py` and the ENG1 spawn reader; `L06.TR.02` spawn events |
| Where does Medusa come from (map spawn, type handler, or level script), and is she drawn with T28 models, sprites (0816), or both? | `L06.RE.03`, `L06.RE.05` with S2.01 |
| Gaze and projectile rules: telegraph, hit test, effect on the hero, avoidance | `L06.RE.03`, `L06.TR.04` |
| Defeat mechanic: is it a reflection; which object or hero action; health and phases; difficulty scaling | `L06.RE.03`, `L06.TR.05`, `L06.TR.07` |
| Which skeleton banks load here, and is SKELWAR a regular enemy or a boss-like encounter? | `L06.RE.04`, `L06.TR.03` FS opens; S4.01 |
| Cause of the 84,096 B VRAM overrun (streamed, partial, or decoder size rule) | S2.03 with a runtime VRAM dump in `L06.TR.02` |
| Retry point; whether Medusa and the skeletons reset | `L06.TR.06`, S6.04 |
| Exact ENG1 → ENG3 hand-off: module unload/load order, tally, and the first CYCLOPS map open | `L06.TR.08`, S6.05 |
| Whether `q:\HERC_MED\` is the L06 Sonix directory | S5.03, `L06.AS.05` |

## Notes
