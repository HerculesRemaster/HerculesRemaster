# L05 Phase 3 — Asset extraction

Goal: export every asset HYDRA uses at runtime, including the Hydra's models and sprite frames, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (T28 models, VRAM overrun, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/HYDRA/` (previews, viewer), `extracted/full-game/sprites/0804_*/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L05.AS.01` (P5.ALL.03, P4.ALL.04) Render HYDRA previews with S2: both layers, the composite, collision, the two spawns labelled from `L05-types.json`, the arc camera path, triggers and the T28 models (S2's done-when requires models to render in at least HYDRA's preview). Align them with the arena landmarks from `L05.TR.02`. Record any mismatch.
- [ ] `L05.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade. Compare the offline VRAM against the runtime VRAM dump from `L05.TR.02`, and use the result to settle the 3,156 B overrun with S2.03.
- [ ] `L05.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for 0804 BOSS_HYDRA and any other bank whose runtime load `L05.TR.02` confirms (effects, projectiles, crowd/NPC if any), plus pickups. Export the Hydra's model records with their animation/transform source if `L05.RE.04` shows models are used.
- [ ] `L05.AS.04` (P4.ALL.06) Export the Hydra encounter links state → sequence/model → frame → sfx/effect, using the S4 template and per-difficulty columns. Distinguish observed states from inferred phase names.
- [ ] `L05.AS.05` (P4.ALL.01–02) Audio: the SFX per fight state and the HYDRA music segment with its start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L05.AS.06` (P4.ALL.07–08) Compare offline frames, tiles and model renders against the L05 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L05.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used HYDRA asset, including models, exports with metadata and a matching runtime sample.
- The VRAM overrun is explained.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
