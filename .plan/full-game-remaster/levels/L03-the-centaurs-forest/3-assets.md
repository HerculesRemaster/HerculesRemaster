# L03 Phase 3 — Asset extraction

Goal: export every asset FOREST uses at runtime, including Nessus, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (models, unknown fields, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/FOREST/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L03.AS.01` (P5.ALL.03, P4.ALL.04) Render FOREST's previews with S2: all 11 layers, the composite, collision, spawns labelled from `L03-types.json`, triggers, cameras and models. Align them with the traversal landmarks from `L03.TR.03`. Record any mismatch.
- [ ] `L03.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, plus the two animated textures and one scrolling texture. Add palette animation where it is observed. Compare the offline VRAM against a runtime VRAM dump of FOREST.
- [ ] `L03.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L03: Nessus 0599/0602/0603/0609/0614/0797/0818/0819; centaur, panther and vulture candidates (each confirmed by its runtime load); effects; the HERCULES letters; pickups.
- [ ] `L03.AS.04` (P4.ALL.06) Export the Nessus encounter links state → sequence → frame → sfx/effect, using the S4 template and per-difficulty columns.
- [ ] `L03.AS.05` (P4.ALL.01–02) Audio: the SFX per scene, and the FOREST and NESSUS music segments with their start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L03.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L03 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L03.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used FOREST and Nessus asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
