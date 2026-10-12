# L07 Phase 3 — Asset extraction

Goal: export every asset CYCLOPS uses at runtime, including the Cyclops and the end sequence, in lossless form, with scene links, and reconcile it against the trace.
Needs: `S3.04` `eng3_decode.py` and `S3.05` ENG3 preview; S4 (SRC3 bank decode, encounter template, unsupported banks); S5 (audio binding, ENG3 sound calls); phase 2 runs.
Outputs: `extracted/full-game/levels/CYCLOPS/` (`level.json`, previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L07.AS.01` (P5.ALL.03, P4.ALL.04) Render the CYCLOPS previews with S3: street and lanes, stairs, obstacles and traffic labelled from `L07-types.json`, pickups, markers and camera path. Align them with the landmarks from `L07.TR.03`/`TR.04`. Record any mismatch.
- [ ] `L07.AS.02` (P4.ALL.04) Export the CIF 494 palette and fog, plus any animated or scrolling textures S3 identifies. Compare the offline VRAM against a runtime VRAM dump of CYCLOPS.
- [x] `L07.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every bank that loads in L07, each confirmed by its runtime load: Cyclops candidate 0480, citizen candidate 0547 PEOPLE, chariot/column candidates, the ENG3 hero (0561, 0553), the letters (0577), effects and pickups.
- [x] `L07.AS.04` (P4.ALL.06) Export the Cyclops encounter links state → sequence → frame → sfx/effect, and the end sequence (CYEND 0493 candidate), using the S4 template and per-difficulty columns. Hand any unsupported bank to S4.02 with its consumer RVA.
- [x] `L07.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (traffic, collisions, Cyclops), and the CYCLOPS music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L07.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L07 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L07.AS.07` (P4.ALL.10) Compare each scenario's expected categories (`boss_animation`, `depth_routes`, `npc_animation`, `chase_hazards` and the rest) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used CYCLOPS asset, including the Cyclops and end-sequence banks, exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- AS.02 (2026-10-12, progress): offline vs runtime, run `analysis/traces/full-game/l07-cyclops-attack/full-progression/20261012-070546-password-config`, checked with `tools/level/eng3_runtime_check.py`.
  - **Palette:** the CIF palette hashes to 04424BF7, the runtime palette.
  - **VRAM:** the map image upload 0,0 768x512 is byte-identical. The upload 768,256 256x256 is identical for its first 114688 bytes. Its last 16384 bytes (rows 224–255) lie past the end of MAP_PSX.BIN (img1 + 0x20000 > file size). At runtime they are leftover memory after the read buffer, not file data: the offline export keeps them 0, and replacement art should treat them as undefined.
- AS.03 (2026-10-12, progress): runtime bank loads are confirmed in `animation-coverage.json` (`tools/decode/bank_runtime.py`). Per-kind banks and runtime draws are in `extracted/meta/full-game/L07-types.json`.
- `L07.AS.04` (2026-10-12): no bank that loads in L07 is an unsupported variant. Every SRC3 bank (HERCULES 561, XTRA_DB1 553, LETTERS 577, ALIEN1 499 / CYCLOPS 480 / PEOPLE 547 / CYEND 493 / TORMENT 482 / SOULS 569) decodes as i1.4 with 0 frame errors, and every frame drawn in the traces is decoded (`tools/decode/bank_runtime.py`).
  - One decoder fix along the way: frames are 1..count inclusive. The last frame is drawn at runtime (CYCLOPS 167, CYEND 33, ALIEN1 828, SOULS 20), and `tools/decode/ifam.py` / `docs/formats/i-family.md` are updated.
  - The frames without a PNG are empty entries (offset 0).
- `L07.AS.03` (2026-10-12): animations with code-backed hitboxes for every bank that loads in L07.
  - **Sprites:** `extracted/sprites/<bank>/anim.json` for the SRC3 banks (`tools/decode/anim.py --src3` over the full-game ENG3 runs). Each frame has its observed timing and `hitboxes`, the frame record boxes as `GFX_FrameHitbox` 0x23860 reads them for `LVL_ObjOverlap` 0x1eff0.
  - **Models:** `models[].hitboxes` in `extracted/full-game/levels/<NAME>/level.json` (`GFX_ModelHitbox` 0x24870: min corner + unsigned extent).
  - **Per-kind hit tests:** the box sizes used by each handler are in `L07-types.json` "damage".
  - Every bank load is runtime-confirmed (`animation-coverage.json`).
- `L07.AS.01` (2026-10-12, progress): `extracted/full-game/levels/CYCLOPS/preview.png` (`tools/viewer/eng3_preview.py`) has one band per track with its blocks, and the spawns coloured by category from `L07-types.json` (hazard red, enemy orange, boss magenta, NPC blue, pickup yellow, checkpoint green, FX grey). It also marks the start records and branch columns. Still open: alignment with the traversal landmarks of TR.03/TR.04, which needs a played run.
- `L07.AS.05` (2026-10-12):
  - **Banks (VERIFIED-STATIC):** the Sonix banks of the level are in `docs/full-game/audio.md` (descriptor +0x14 record via slot 0x6c).
  - **SFX per scene:** `extracted/meta/full-game/audio-scene-coverage.json` (`tools/trace/audio_scenes.py`) has the sfx ids, counts and first ticks of every traced CYCLOPS scene.
  - **Music:** `AUD_StartMusic(0, 0)` at the level-load tick (eng3 0x1acd1). CD audio stays `missing_source_media` (Q-01).
  - **Per-event sounds:** the ids each handler requests (e.g. SFX 0x603b potion, 0x603c letter, 0x602e/0x602d hero hit) are in `L07-types.json` / `eng3_kind_handlers.json`.
  - Still open: the runtime mapping from handler id (`AUD_PlaySfx` 0xbc47, table gs+0x2f0, 3 bytes per id) to the EXE voice id in `sfx_play`, which needs a hook on 0xbc47.
- `L07.AS.06` / `AS.07` (2026-10-12): rows are in `extracted/meta/full-game/level-assets.json` and `scene-asset-coverage.json` (`tools/meta/scene_assets.py`). Each row gives the archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs and first tick. Offline/runtime pairs agree: VRAM byte-identical (AS.02), palettes, every drawn frame decoded (AS.04). The expected-vs-observed categories per scenario are listed there, and the missing ones are the phase 2 queue: every scenario without a run, plus `loading_tally`/`script_triggers` for the runs that exist.
