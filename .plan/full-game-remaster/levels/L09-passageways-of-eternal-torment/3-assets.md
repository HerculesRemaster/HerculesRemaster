# L09 Phase 3 — Asset extraction

Goal: export every asset TORMENT uses at runtime in lossless form, with scene links, and reconcile it against the trace.
Needs: `S3.04` `eng3_decode.py` and `S3.05` ENG3 preview; S4 (SRC3 bank decode, encounter template, unsupported banks); S5 (audio binding, ENG3 sound calls); phase 2 runs.
Outputs: `extracted/full-game/levels/TORMENT/` (`level.json`, previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L09.AS.01` (P5.ALL.03, P4.ALL.04) Render the TORMENT previews with S3: sections, lanes/depth routes, river edges, obstacles and actors labelled from `L09-types.json`, pickups, markers and camera path. Align them with the landmarks from `L09.TR.03`. Record any mismatch.
- [ ] `L09.AS.02` (P4.ALL.04) Export the CIF 521 palette and fog, plus any animated or scrolling textures S3 identifies (the river surface is a candidate). Compare the offline VRAM against a runtime VRAM dump of TORMENT.
- [ ] `L09.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every bank that loads in L09, each confirmed by its runtime load: candidates 0482 TORMENT and 0569 SOULS, chariot/stalactite/skeleton banks, the ENG3 hero (0561, 0553) including any fall/death sequence, the letters (0577), effects and pickups.
- [ ] `L09.AS.04` (P4.ALL.06) Export the skeleton, lost-soul and chariot links state → sequence → frame → sfx/effect, using the S4 template and the Medium/Herculean columns. Hand any unsupported bank to S4.02 with its consumer RVA.
- [ ] `L09.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (souls, chariot, stalactites, falls), and the TORMENT music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L09.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L09 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L09.AS.07` (P4.ALL.10) Compare each scenario's expected categories (`enemy_animation`, `depth_routes`, `fx_projectiles`, `checkpoint_state` and the rest) with what was observed and exported. The `beginner-unavailable` row expects only UI/tally/script assets. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used TORMENT asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
