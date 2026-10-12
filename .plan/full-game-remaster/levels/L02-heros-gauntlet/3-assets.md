# L02 Phase 3 — Asset extraction

Goal: export every asset GAUNTLET uses at runtime in lossless form, with scene links, and reconcile it against the trace.
Needs: `S3.04` `eng3_decode.py` and `S3.05` ENG3 preview (no `i1.a` export exists before them); S4 (SRC3 bank decode, encounter template, unsupported banks); S5 (audio binding, ENG3 sound calls); phase 2 runs.
Outputs: `extracted/full-game/levels/GAUNTLET/` (`level.json`, previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L02.AS.01` (P5.ALL.03, P4.ALL.04) Render the GAUNTLET previews with S3: track/lanes, crossroads and branches, obstacles labelled from `L02-types.json`, pickups, rescue points, markers and camera path. Align them with the traversal landmarks from `L02.TR.03`/`TR.04`. Record any mismatch.
- [ ] `L02.AS.02` (P4.ALL.04) Export the CIF 556 palette and fog through `tools/decode/palette.py`, plus any animated or scrolling textures S3 identifies. Compare the offline VRAM against a runtime VRAM dump of GAUNTLET.
- [x] `L02.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every bank that loads in L02, each confirmed by its runtime load: the ENG3 hero (0561 HERCULES, 0553 XTRA_DB1), the HERCULES letters (0577), enemy/hazard candidates (0499 ALIEN1), damsel candidates (0547 PEOPLE), effects and pickups.
- [x] `L02.AS.04` (P4.ALL.03) If any bank that loads in L02 is an unsupported variant (0503/0601 or another), hand it to S4.02 with its consumer RVA. Keep `UnsupportedVariant` until the rule comes from code.
- [x] `L02.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (hazards, jumps, pickups, rescues), and the GAUNTLET music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L02.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L02 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L02.AS.07` (P4.ALL.10) Compare each scenario's expected categories (`chase_hazards`, `npc_animation`, `props_pickups`, `script_triggers` and the rest) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used GAUNTLET asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L02.AS.02` (2026-10-12, progress): offline vs runtime, run `analysis/traces/full-game/l02-heros-gauntlet/full-progression/20261012-063901-password-config` (password 13):
  - VRAM: both map uploads (MAP_PSX.BIN index 557: rect 0,0 768x512 and 768,256 256x256, tick 1787) are byte-identical to `extracted/full-game/levels/GAUNTLET/vram.bin`.
  - Palette: `palette.json` (CIF 556) hashes to 124999A6, the runtime palette from tick 1788 (brightness fade-in 88→128 on the same colours). The tool is `tools/level/eng3_runtime_check.py`, which writes `eng3_runtime_check.json` in the run.
  - Still open: fog/shade/blend tables have no runtime dump yet, and animated textures wait on S3.03.
  - The other uploads in that scene come from TITLE SEQ6 (509, letters/HUD) and STPANEL2.DAT (575, y 512), not from the map.
- `L02.AS.03` (2026-10-12, progress): runtime bank loads are in `extracted/meta/full-game/animation-coverage.json`. All ENG3 banks are now VERIFIED-STATIC per level from the descriptor lists (`tools/decode/bank_index.py`): GAUNTLET = level [1 HERCULES] + start [3 ALIEN1, 2 XTRA_DB1, 8 LETTERS]. Shown in the run: HERCULES 103 frames, XTRA_DB1 22, ALIEN1 173 (object types 0x2c 0x38 0x3a 0x3c 0x40 0x41 0x43 0x44 0x45 0x47 0x4a 0x2016 0x2017 0x4001), LETTERS 53. No PEOPLE bank in GAUNTLET (CYCLOPS only). Hitboxes wait on L02.RE.02.
- `L02.AS.04` (2026-10-12): no bank that loads in L02 is an unsupported variant. Every SRC3 bank (HERCULES 561, XTRA_DB1 553, LETTERS 577, ALIEN1 499 / CYCLOPS 480 / PEOPLE 547 / CYEND 493 / TORMENT 482 / SOULS 569) decodes as i1.4 with 0 frame errors, and every frame drawn in the traces is decoded (`tools/decode/bank_runtime.py`).
  - One decoder fix along the way: frames are 1..count inclusive. The last frame is drawn at runtime (CYCLOPS 167, CYEND 33, ALIEN1 828, SOULS 20), and `tools/decode/ifam.py` / `docs/formats/i-family.md` are updated.
  - The frames without a PNG are empty entries (offset 0).
- `L02.AS.03` (2026-10-12): animations with code-backed hitboxes for every bank that loads in L02.
  - **Sprites:** `extracted/sprites/<bank>/anim.json` for the SRC3 banks (`tools/decode/anim.py --src3` over the full-game ENG3 runs). Each frame has its observed timing and `hitboxes`, the frame record boxes as `GFX_FrameHitbox` 0x23860 reads them for `LVL_ObjOverlap` 0x1eff0.
  - **Models:** `models[].hitboxes` in `extracted/full-game/levels/<NAME>/level.json` (`GFX_ModelHitbox` 0x24870: min corner + unsigned extent).
  - **Per-kind hit tests:** the box sizes used by each handler are in `L02-types.json` "damage".
  - Every bank load is runtime-confirmed (`animation-coverage.json`).
- `L02.AS.01` (2026-10-12, progress): `extracted/full-game/levels/GAUNTLET/preview.png` (`tools/viewer/eng3_preview.py`) has one band per track with its blocks, and the spawns coloured by category from `L02-types.json` (hazard red, enemy orange, boss magenta, NPC blue, pickup yellow, checkpoint green, FX grey). It also marks the start records and branch columns. Still open: alignment with the traversal landmarks of TR.03/TR.04, which needs a played run.
- `L02.AS.05` (2026-10-12):
  - **Banks (VERIFIED-STATIC):** the Sonix banks of the level are in `docs/full-game/audio.md` (descriptor +0x14 record via slot 0x6c).
  - **SFX per scene:** `extracted/meta/full-game/audio-scene-coverage.json` (`tools/trace/audio_scenes.py`) has the sfx ids, counts and first ticks of every traced GAUNTLET scene.
  - **Music:** `AUD_StartMusic(0, 0)` at the level-load tick (eng3 0x1acd1). CD audio stays `missing_source_media` (Q-01).
  - **Per-event sounds:** the ids each handler requests (e.g. SFX 0x603b potion, 0x603c letter, 0x602e/0x602d hero hit) are in `L02-types.json` / `eng3_kind_handlers.json`.
  - Still open: the runtime mapping from handler id (`AUD_PlaySfx` 0xbc47, table gs+0x2f0, 3 bytes per id) to the EXE voice id in `sfx_play`, which needs a hook on 0xbc47.
- `L02.AS.06` / `AS.07` (2026-10-12): rows are in `extracted/meta/full-game/level-assets.json` and `scene-asset-coverage.json` (`tools/meta/scene_assets.py`). Each row gives the archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs and first tick. Offline/runtime pairs agree: VRAM byte-identical (AS.02), palettes, every drawn frame decoded (AS.04). The expected-vs-observed categories per scenario are listed there, and the missing ones are the phase 2 queue: every scenario without a run, plus `loading_tally`/`script_triggers` for the runs that exist.
