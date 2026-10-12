# L10 Phase 3 — Asset extraction

Goal: export every asset VORTEX uses at runtime, including the final encounter and the ending/credits media, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (models, VRAM overrun, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/VORTEX/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, `extracted/full-game/images/` (ending screens, if re-exported), rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L10.AS.01` (P5.ALL.03, P4.ALL.04) Render VORTEX's previews with S2: the single layer, the T28 models (S2.01/S2.04), object positions from `L10.TR.02`, and the 2 camera records. Re-export the VRAM image once S2.03 explains the 13,816 B overrun, and validate it against the runtime VRAM dump.
- [x] `L10.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, and the scrolling texture with its observed rate. Add palette animation where it is observed.
- [ ] `L10.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L10, starting from the runtime opens in `L10.TR.02`. HADES 0803, MEG 0785, PAIN 0812, PANIC 0532 and SOULS 0569 are included only if they load here.
- [ ] `L10.AS.04` (P4.ALL.06) Export the final-encounter links state → sequence → frame → sfx/effect, using the S4 template and per-difficulty columns.
- [ ] `L10.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (Sonix `VORTEX`, `HERC_VOR`, and `TORMENT`/`HERC_TOR` only if opened here), the VORTEX music segment with its start tick, and whatever music plays during the ending and credits. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L10.AS.06` (P4.ALL.04) Ending media: every screen, bank and effect shown from completion to the return to title (candidates `C1`–`C31`, `SEQ1`–`SEQ10`, `LEV11`, `LEV12`, `CHOP_TITLE_SEQ*`), each linked to its run and tick. Any requested external movie is recorded as `missing_source_media`.
- [x] `L10.AS.07` (P4.ALL.07–08) Compare offline frames, tiles and screens against the L10 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L10.AS.08` (P4.ALL.10) Compare each scenario's expected categories (including `ending_media`, `boss_animation`, `npc_animation`, `depth_routes`) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used VORTEX asset, final-encounter bank and ending/credits screen exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported or missing asset has a code-backed reason and a next step.

## Notes
- `L10.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l10-vortex-of-souls/full-progression/20261012-140622-cheat_select-medium`: img0 and loaded img1 rows 1.0 equal (31 tail rows past the header, leftover memory as in MEDUSA). The scrolling record (dst (0,256), 128×256 units) has no caller in ENG1 (only FOREST water and TRAINING devices call `LVL_ScrollTexture` 0x2d80; `LVL_DrawBackgroundVortex` 0x2fa0 does not scroll), so its observed rate is 0: offset 0 at both dumps. Palette/fog/blend/shade: `extracted/full-game/levels/VORTEX/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L10.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 4 sprite banks load in VORTEX at runtime, all decoded with 0 frame errors; 4 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): none; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L10.AS.06` / `AS.07` (2026-10-12): 7 rows for level 11 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
