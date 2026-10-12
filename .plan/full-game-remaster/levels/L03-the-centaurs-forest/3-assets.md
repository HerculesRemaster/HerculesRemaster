# L03 Phase 3 — Asset extraction

Goal: export every asset FOREST uses at runtime, including Nessus, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (models, unknown fields, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/FOREST/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L03.AS.01` (P5.ALL.03, P4.ALL.04) Render FOREST's previews with S2: all 11 layers, the composite, collision, spawns labelled from `L03-types.json`, triggers, cameras and models. Align them with the traversal landmarks from `L03.TR.03`. Record any mismatch.
- [x] `L03.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade, plus the two animated textures and one scrolling texture. Add palette animation where it is observed. Compare the offline VRAM against a runtime VRAM dump of FOREST.
- [ ] `L03.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L03: Nessus 0599/0602/0603/0609/0614/0797/0818/0819; centaur, panther and vulture candidates (each confirmed by its runtime load); effects; the HERCULES letters; pickups.
- [ ] `L03.AS.04` (P4.ALL.06) Export the Nessus encounter links state → sequence → frame → sfx/effect, using the S4 template and per-difficulty columns.
- [x] `L03.AS.05` (P4.ALL.01–02) Audio: the SFX per scene, and the FOREST and NESSUS music segments with their start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L03.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L03 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L03.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used FOREST and Nessus asset exports with metadata and a matching runtime sample.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L03.AS.05` done (2026-10-12): `extracted/meta/full-game/audio-scene-coverage.json` FOREST (9 scenes, 7 runs, 51 distinct sfx ids with counts and first ticks) and `docs/full-game/audio.md`.
  - **FOREST music:** `AUD_LoadLevelSonix`(record 0x4f258) then `AUD_StartMusic(0, 0)` at level load (tick 1567 in the password runs).
  - **NESSUS music:** at the Nessus trigger, `q:\NESSUS\SFX.SFX` (605) and `MUSIC.MUS` (589) are streamed (ticks +1/+2 after the trigger), then `AUD_StopMusic` + `AUD_StartMusic(0, 0)` at +4 (run `nessus-approach/20261012-070937`, ticks 1568/1569/1571).
  - CD segments: FOREST 2:08:55–2:12:53 and NESSUS 2:12:53–2:15:29 (`music-transport.md`). The audio stays `missing_source_media` (no disc, Q-01).
- `L03.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l03-the-centaurs-forest/full-progression/20261012-135336-cheat_select-medium`: img0 0.9955 equal (the difference is the two animated rects, which hold table frames 7 and 3 at runtime), img1 loaded rows 1.0. Animated: 8 frames every 2 frames at (128,384) 64×64 units; 4 frames every 4 frames at (128,320). Scroll: one rect, driven only by the water actor kind 0x4b (`HERO_WaterUpdate3` 0xd460: dy −16 rows once per game frame, mod 128); not moved at the start position (offset 0 at both dumps). No palette animation (the EXE never fades through the palette, P7 notes). Palette/fog/blend/shade: `extracted/full-game/levels/FOREST/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L03.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 21 sprite banks load in FOREST at runtime, all decoded with 0 frame errors; 18 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): 524 RcRock, 535 WRESTLEC, 603 Ride; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L03.AS.06` / `AS.07` (2026-10-12): 52 rows for level 3 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
