# L06 Phase 3 — Asset extraction

Goal: export every asset MEDUSA uses at runtime, including Medusa, the skeletons, their effects and any models, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (T28 models, VRAM overrun, previews), S4 (bank decode, bank → level index, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/MEDUSA/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L06.AS.01` (P5.ALL.03, P4.ALL.04) Render MEDUSA previews with S2: both layers, the composite, collision, the six spawns labelled from `L06-types.json`, the arc camera path, triggers and any T28 models. Align them with the lair landmarks from `L06.TR.02`. Record any mismatch.
- [x] `L06.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, plus the one animated texture. Compare the offline VRAM against the runtime VRAM dump from `L06.TR.02`, and use the result to settle the 84,096 B overrun with S2.03.
- [ ] `L06.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for 0816 BOSS_MEDUSA, the skeleton bank(s) whose runtime load `L06.TR.03` confirms (0486 and/or 0566), gaze/projectile effects, any reflection prop, and pickups. Export Medusa's model records if `L06.RE.05` shows models are used.
- [ ] `L06.AS.04` (P4.ALL.06) Export the Medusa encounter links state → sequence/model → frame → sfx/effect, using the S4 template and per-difficulty columns. Distinguish observed states from inferred phase names.
- [ ] `L06.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (skeletons, gaze, projectiles, defeat) and the MEDUSA music segment with its start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L06.AS.06` (P4.ALL.07–08) Compare offline frames, tiles and model renders against the L06 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L06.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used MEDUSA asset exports with metadata and a matching runtime sample.
- The skeleton bank attribution is confirmed by runtime loads.
- The VRAM overrun is explained.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L06.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l06-medusas-lair/full-progression/20261012-135959-cheat_select-medium`: img0 and the 67 loaded img1 rows 1.0 equal. Overrun: the other 189 rows of img1 lie past the header (0xcc000); at runtime they are not the file's continuation (60.9 % equal, almost all zero bytes) and stay unchanged between dumps, so they are leftover memory, not map data (S2.03). The one animated record has period 0, and `LVL_AnimateTextures` skips period 0, so it never animates (runtime: frame 0 = file at both dumps). Palette/fog/blend/shade: `extracted/full-game/levels/MEDUSA/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L06.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 6 sprite banks load in MEDUSA at runtime, all decoded with 0 frame errors; 6 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): none; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L06.AS.06` / `AS.07` (2026-10-12): 9 rows for level 7 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
