# L07 Phase 3 — Asset extraction

Goal: export every asset CYCLOPS uses at runtime, including the Cyclops and the end sequence, in lossless form, with scene links, and reconcile it against the trace.
Needs: `S3.04` `eng3_decode.py` and `S3.05` ENG3 preview; S4 (SRC3 bank decode, encounter template, unsupported banks); S5 (audio binding, ENG3 sound calls); phase 2 runs.
Outputs: `extracted/full-game/levels/CYCLOPS/` (`level.json`, previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L07.AS.01` (P5.ALL.03, P4.ALL.04) Render the CYCLOPS previews with S3: street and lanes, stairs, obstacles and traffic labelled from `L07-types.json`, pickups, markers and camera path. Align them with the landmarks from `L07.TR.03`/`TR.04`. Record any mismatch.
- [ ] `L07.AS.02` (P4.ALL.04) Export the CIF 494 palette and fog, plus any animated or scrolling textures S3 identifies. Compare the offline VRAM against a runtime VRAM dump of CYCLOPS.
- [ ] `L07.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every bank that loads in L07, each confirmed by its runtime load: Cyclops candidate 0480, citizen candidate 0547 PEOPLE, chariot/column candidates, the ENG3 hero (0561, 0553), the letters (0577), effects and pickups.
- [ ] `L07.AS.04` (P4.ALL.06) Export the Cyclops encounter links state → sequence → frame → sfx/effect, and the end sequence (CYEND 0493 candidate), using the S4 template and per-difficulty columns. Hand any unsupported bank to S4.02 with its consumer RVA.
- [ ] `L07.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (traffic, collisions, Cyclops), and the CYCLOPS music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L07.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L07 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L07.AS.07` (P4.ALL.10) Compare each scenario's expected categories (`boss_animation`, `depth_routes`, `npc_animation`, `chase_hazards` and the rest) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used CYCLOPS asset, including the Cyclops and end-sequence banks, exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
