# L08 Phase 3 — Asset extraction

Goal: export every asset TITANS uses at runtime, including the Pegasus, Titan and god banks, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (models, unknown fields, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/TITANS/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L08.AS.01` (P5.ALL.03, P4.ALL.04) Render TITANS's previews with S2: all 5 layers, the composite, collision, spawns labelled from `L08-types.json`, triggers, cameras and T28 models (S2.01/S2.04). Align them with the section boundaries from `L08.TR.04`. Record any mismatch.
- [x] `L08.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade. The header lists no animated or scrolling textures; record any palette or texture animation that is observed anyway. Compare the offline VRAM against a runtime VRAM dump of TITANS.
- [ ] `L08.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L08, each confirmed by its runtime load: PEGASUS 0492 and any hero flight frames; the Titan candidates 0786/0563/0568/0483; the god candidates 0000/0764/0495/0498/0502/0476; WATER 0571 and GRIFFON 0478 / GRIF 0769 only if they load here; effects; pickups.
- [ ] `L08.AS.04` (P4.ALL.06) Export the Titan encounter and god rescue links state → sequence → frame → sfx/effect, using the S4 template and per-difficulty columns.
- [ ] `L08.AS.05` (P4.ALL.01–02) Audio: the SFX per section (Sonix `TITANS` and `HERC_TIT`), and the TITANS music segment with its start tick. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L08.AS.06` (P4.ALL.04) Transition screens: the loading screen shown on L08 entry, the tally, and on Beginner the end screen (candidates `0743_LEV9`, `0562_ENDEASY`). Link each to the run and tick where it was shown.
- [x] `L08.AS.07` (P4.ALL.07–08) Compare offline frames and tiles against the L08 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [ ] `L08.AS.08` (P4.ALL.10) Compare each scenario's expected categories (including `flight_animation`, `npc_animation`, `boss_animation`, `depth_routes`) with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used TITANS, Pegasus, Titan and god asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L08.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l08-titan-flight/full-progression/20261012-140311-cheat_select-medium`: img0 and img1 1.0 equal at both dumps; no T38/T3c records and no texture change between dumps. Palette/fog/blend/shade: `extracted/full-game/levels/TITANS/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L08.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 6 sprite banks load in TITANS at runtime, all decoded with 0 frame errors; 3 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): 525 RtRock, 568 RockT, 807 Spy; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L08.AS.06` / `AS.07` (2026-10-12): 9 rows for level 9 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
