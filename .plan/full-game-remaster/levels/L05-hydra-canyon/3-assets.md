# L05 Phase 3 — Asset extraction

Goal: export every asset HYDRA uses at runtime, including the Hydra's models and sprite frames, in lossless form, with scene links, and reconcile it against the trace.
Needs: S2 (T28 models, VRAM overrun, previews), S4 (bank decode, encounter template), S5 (audio binding), phase 2 runs.
Outputs: `extracted/full-game/levels/HYDRA/` (previews, viewer), `extracted/full-game/sprites/0804_*/`, rows in `extracted/meta/full-game/{level-assets,scene-asset-coverage}.json`.

## Tasks

- [ ] `L05.AS.01` (P5.ALL.03, P4.ALL.04) Render HYDRA previews with S2: both layers, the composite, collision, the two spawns labelled from `L05-types.json`, the arc camera path, triggers and the T28 models (S2's done-when requires models to render in at least HYDRA's preview). Align them with the arena landmarks from `L05.TR.02`. Record any mismatch.
- [x] `L05.AS.02` (P4.ALL.04) Export the CIF palette, fog, blend and shade. Compare the offline VRAM against the runtime VRAM dump from `L05.TR.02`, and use the result to settle the 3,156 B overrun with S2.03.
- [ ] `L05.AS.03` (P4.ALL.05) Export animations with code-backed hitboxes for 0804 BOSS_HYDRA and any other bank whose runtime load `L05.TR.02` confirms (effects, projectiles, crowd/NPC if any), plus pickups. Export the Hydra's model records with their animation/transform source if `L05.RE.04` shows models are used.
- [ ] `L05.AS.04` (P4.ALL.06) Export the Hydra encounter links state → sequence/model → frame → sfx/effect, using the S4 template and per-difficulty columns. Distinguish observed states from inferred phase names.
- [ ] `L05.AS.05` (P4.ALL.01–02) Audio: the SFX per fight state and the HYDRA music segment with its start ticks. CD audio stays `missing_source_media` unless Q-01 is answered.
- [x] `L05.AS.06` (P4.ALL.07–08) Compare offline frames, tiles and model renders against the L05 runtime pairs. Write the `level-assets.json` and `scene-asset-coverage.json` rows (archive index/ID/range, decoder version, consumer RVA, scenes, difficulty, ticks).
- [x] `L05.AS.07` (P4.ALL.10) Compare each scenario's expected categories with what was observed and exported. Queue the missing pairs back to phase 2.

## Done when

- Every runtime-used HYDRA asset, including models, exports with metadata and a matching runtime sample.
- The VRAM overrun is explained.
- The scene coverage rows are complete.
- Any unsupported asset has a code-backed reason and a next step.

## Notes
- `L05.AS.02` (2026-10-12): runtime VRAM dumps (`-VramTicks`, `tools/level/vram_compare.py`), run `analysis/traces/full-game/l05-hydra-canyon/canyon-entry/20261012-063658-debug_level-medium (DEBUG level entry)`: img0 and loaded img1 rows 1.0 equal. The 3,156 B overrun is the last 31 rows of img1 past the header: not loaded from the file (runtime bytes differ from the file continuation, see L06), and no texture samples them (S2.03). No animated or scrolling records. Palette/fog/blend/shade: `extracted/full-game/levels/HYDRA/` (palette.json, fog.bin, blend.bin, shade.bin; `tools/level/level_decode.py`). Animated/scrolling textures: `texture-anims.json` (`tools/level/texture_anims.py`, T38/T3c layouts in docs/level-format.md).
- `L05.AS.03` (2026-10-12, progress): `extracted/meta/full-game/level-anim-coverage.json` (`tools/decode/level_anim_coverage.py`). 4 sprite banks load in HYDRA at runtime, all decoded with 0 frame errors; 4 were drawn in runs and have `anim.json` (clips, ticks, loops, code-backed frame hitboxes from `GFX_FrameHitbox` 0x30e40; `tools/decode/anim.py` over all 130 runs). Not drawn yet (frames exported, no clips): none; these and the unseen frames of the drawn banks need the played sessions (docs/full-game/play-sessions.md). Per-actor boxes that handlers test themselves stay with S4.03.
- `L05.AS.06` / `AS.07` (2026-10-12): 7 rows for level 6 in `extracted/meta/full-game/level-assets.json` and the scenario rows in `scene-asset-coverage.json` (`tools/meta/scene_assets.py`; archive index/id, category, consumer DLL+RVA, decoder, scenarios, runs, first tick). Offline/runtime pairs agree: map VRAM byte-identical (AS.02), every bank that loads decodes, drawn frames pixel-matched where art dumps exist (S4.05 strict check; unmatched shown frames are off-screen anim events, not decode errors). The expected-vs-observed categories per scenario are listed there; the missing ones are the phase 2 queue (every scenario without a run, plus the categories missing from runs that exist).
