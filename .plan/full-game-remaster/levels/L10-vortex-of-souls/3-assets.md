# L10 Phase 3 — Asset extraction

Goal: export every asset VORTEX uses at runtime, including the final encounter and the ending/credits media, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (models, VRAM overrun, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/VORTEX/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, `extracted/full-game/images/` (ending screens, if re-exported), rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L10.AS.01` (P5.ALL.03, P4.ALL.04) Render VORTEX's previews with S2: the single layer, the T28 models (S2.01/S2.04), object positions from `L10.TR.02`, and the 2 camera records. Re-export the VRAM image once S2.03 explains the 13,816 B overrun, and validate it against the runtime VRAM dump.
- [ ] `L10.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, and the scrolling texture with its observed rate. Add palette animation where it is observed.
- [ ] `L10.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L10, starting from the runtime opens in `L10.TR.02`. HADES 0803, MEG 0785, PAIN 0812, PANIC 0532 and SOULS 0569 are included only if they load here.
- [ ] `L10.AS.04` (P4.ALL.06) Export the final-encounter links state → sequence → frame → sfx/effect, using the S4 template and per-difficulty columns.
- [ ] `L10.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (Sonix `VORTEX`, `HERC_VOR`, and `TORMENT`/`HERC_TOR` only if opened here), the VORTEX music segment with its start tick, and whatever music plays during the ending and credits. CD audio stays `missing_source_media` unless Q-01 is answered.
- [ ] `L10.AS.06` (P4.ALL.04) Ending media: every screen, bank and effect shown from completion to the return to title (candidates `C1`–`C31`, `SEQ1`–`SEQ10`, `LEV11`, `LEV12`, `CHOP_TITLE_SEQ*`), each linked to its run and tick. Any requested external movie is recorded as `missing_source_media`.
- [ ] `L10.AS.07` (P4.ALL.07–08) Compare offline frames, tiles and screens against the L10 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L10.AS.08` (P4.ALL.10) Compare each scenario's expected categories (including `ending_media`, `boss_animation`, `npc_animation`, `depth_routes`) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used VORTEX asset, final-encounter bank and ending/credits screen exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported or missing asset has a code-backed reason and a next step.

## Notes
