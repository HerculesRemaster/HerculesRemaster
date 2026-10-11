# L06 Phase 3 — Asset extraction

Goal: export every asset MEDUSA uses at runtime, including Medusa, the skeletons, their effects and any models, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (T28 models, VRAM overrun, previews), S4 (bank decode, bank → level index, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/MEDUSA/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L06.AS.01` (P5.ALL.03, P4.ALL.04) Render MEDUSA previews with S2: both layers, the composite, collision, the six spawns labelled from `L06-types.json`, the arc camera path, triggers and any T28 models. Align them with the lair landmarks from `L06.TR.02`. Record any mismatch.
- [ ] `L06.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, plus the one animated texture. Compare the offline VRAM against the runtime VRAM dump from `L06.TR.02`, and use the result to settle the 84,096 B overrun with S2.03.
- [ ] `L06.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for 0816 BOSS_MEDUSA, the skeleton bank(s) whose runtime load `L06.TR.03` confirms (0486 and/or 0566), gaze/projectile effects, any reflection prop, and pickups. Export Medusa's model records if `L06.RE.05` shows models are used.
- [ ] `L06.AS.04` (P4.ALL.06) Export the Medusa encounter links state → sequence/model → frame → sfx/effect, using the S4 template and per-difficulty columns. Distinguish observed states from inferred phase names.
- [ ] `L06.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (skeletons, gaze, projectiles, defeat) and the MEDUSA music segment with its start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L06.AS.06` (P4.ALL.07–08) Compare offline frames, tiles and model renders against the L06 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L06.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used MEDUSA asset exports with metadata and a matching runtime sample.
- The skeleton bank attribution is confirmed by runtime loads.
- The VRAM overrun is explained.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
