# L02 Phase 3 — Asset extraction

Goal: export every asset GAUNTLET uses at runtime in lossless form, with scene links, and reconcile it against the trace.
Needs: `S3.04` `eng3_decode.py` and `S3.05` ENG3 preview (no `i1.a` export exists before them); S4 (SRC3 bank decode, encounter template, unsupported banks); S5 (audio binding, ENG3 sound calls); phase 2 runs.
Outputs: `extracted/full-game/levels/GAUNTLET/` (`level.json`, previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L02.AS.01` (P5.ALL.03, P4.ALL.04) Render the GAUNTLET previews with S3: track/lanes, crossroads and branches, obstacles labelled from `L02-types.json`, pickups, rescue points, markers and camera path. Align them with the traversal landmarks from `L02.TR.03`/`TR.04`. Record any mismatch.
- [ ] `L02.AS.02` (P4.ALL.04) Export the CIF 556 palette and fog through `tools/decode/palette.py`, plus any animated or scrolling textures S3 identifies. Compare the offline VRAM against a runtime VRAM dump of GAUNTLET.
- [ ] `L02.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every bank that loads in L02, each confirmed by its runtime load: the ENG3 hero (0561 HERCULES, 0553 XTRA_DB1), the HERCULES letters (0577), enemy/hazard candidates (0499 ALIEN1), damsel candidates (0547 PEOPLE), effects and pickups.
- [ ] `L02.AS.04` (P4.ALL.03) If any bank that loads in L02 is an unsupported variant (0503/0601 or another), hand it to S4.02 with its consumer RVA. Keep `UnsupportedVariant` until the rule comes from code.
- [ ] `L02.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (hazards, jumps, pickups, rescues), and the GAUNTLET music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L02.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L02 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L02.AS.07` (P4.ALL.10) Compare each scenario's expected categories (`chase_hazards`, `npc_animation`, `props_pickups`, `script_triggers` and the rest) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used GAUNTLET asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
