# L04 Phase 3 — Asset extraction

Goal: export every asset THEBES uses at runtime in lossless form, with scene links, and reconcile it against the trace. THEBES is the richest ENG1 map (587 spawns, 87 kinds), so actor coverage is the bulk of this phase.
Needs: S2 (models, unknown fields, previews), S4 (bank decode, bank → level index, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/THEBES/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L04.AS.01` (P5.ALL.03, P4.ALL.04) Render THEBES previews with S2: all 5 layers, the composite, collision, the 8 block boundaries, spawns labelled from `L04-types.json`, triggers (stairs, rescues, destruction), cameras and any T28 models. Mark the start records from `L04.RE.01` and any substage boundary from `L04.TR.05`. Align them with the traversal landmarks from `L04.TR.03`. Record any mismatch.
- [ ] `L04.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade. The header has no animated or scrolling textures (`n_anim` 0, `n_scroll` 0); confirm at runtime that none appear. Compare the offline VRAM against a runtime VRAM dump of THEBES.
- [ ] `L04.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L04, each confirmed by its runtime load:
  - Thebian banks 0329, 0484, 0763, 0766, 0767, 0768, 0800, 0808, 0815, 0817, 0820
  - enemies 0486 SKELETON, 0496 MINOTAUR, 0479 HARPIES / 0774 HARP
  - 0532 PANIC and any NPC/story banks (0785, 0793, 0807, 0799, 0477, 0812)
  - debris/effects (0524/0525 only if loaded here), the HERCULES letters, pickups
- [ ] `L04.AS.04` (P4.ALL.06) Export encounter links state → sequence → frame → sfx/effect with the S4 template for the Minotaur and any other encounter `L04.RE.05` classifies as major, with per-difficulty columns.
- [ ] `L04.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (rescues, destruction, enemies) and the THEBES music segment with its start ticks. Record any music restart or change at the substage boundary as evidence for `L04.TR.05`. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L04.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L04 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L04.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used THEBES asset exports with metadata and a matching runtime sample.
- Every bank attributed to L04 is confirmed by a runtime load; the rest are released back to S4.01 as unattributed.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
