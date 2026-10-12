# L04 Phase 3 — Asset extraction

Goal: export every asset THEBES uses at runtime in lossless form, with scene links, and reconcile it against the trace. THEBES is the richest ENG1 map (587 spawns, 87 kinds), so actor coverage is the bulk of this phase.
Needs: S2 (models, unknown fields, previews), S4 (bank decode, bank → level index, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/THEBES/` (previews, viewer), `extracted/full-game/sprites/<bank>/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L04.AS.01` (P5.ALL.03, P4.ALL.04) Render THEBES previews with S2: all 5 layers, the composite, collision, the 8 block boundaries, spawns labelled from `L04-types.json`, triggers (stairs, rescues, destruction), cameras and any T28 models. Mark the start records from `L04.RE.01` and any substage boundary from `L04.TR.05`. Align them with the traversal landmarks from `L04.TR.03`. Record any mismatch.
- [x] `L04.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade. The header has no animated or scrolling textures (`n_anim` 0, `n_scroll` 0); confirm at runtime that none appear. Compare the offline VRAM against a runtime VRAM dump of THEBES.
- [ ] `L04.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for every actor bank that loads in L04, each confirmed by its runtime load:
  - Thebian banks 0329, 0484, 0763, 0766, 0767, 0768, 0800, 0808, 0815, 0817, 0820
  - enemies 0486 SKELETON, 0496 MINOTAUR, 0479 HARPIES / 0774 HARP
  - 0532 PANIC and any NPC/story banks (0785, 0793, 0807, 0799, 0477, 0812)
  - debris/effects (0524/0525 only if loaded here), the HERCULES letters, pickups
- [ ] `L04.AS.04` (P4.ALL.06) Export encounter links state → sequence → frame → sfx/effect with the S4 template for the Minotaur and any other encounter `L04.RE.05` classifies as major, with per-difficulty columns.
- [ ] `L04.AS.05` (P4.ALL.01–02) Audio: the SFX per scene (rescues, destruction, enemies) and the THEBES music segment with its start ticks. Record any music restart or change at the substage boundary as evidence for `L04.TR.05`. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L04.AS.06` (P4.ALL.07–08) Compare offline frames and tiles against the L04 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L04.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used THEBES asset exports with metadata and a matching runtime sample.
- Every bank attributed to L04 is confirmed by a runtime load; the rest are released back to S4.01 as unattributed.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L04.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l04-the-big-olive/full-progression/20261012-135647-cheat_select-medium`: img0 and img1 1.0 equal at ticks 4200 and 5000; n_anim 0 and n_scroll 0 confirmed (no T38/T3c records, map images unchanged between dumps). Palette/fog/blend/shade: `extracted/full-game/levels/THEBES/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L04.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 19 sprite banks load in THEBES at runtime, all decoded with 0 frame errors; 15 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): 496 MINOTAUR, 766 THSNOW, 768 THFLASH, 815 THEWGUY; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L04.AS.06` / `AS.07` (2026-10-12): 24 rows for level 5 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
