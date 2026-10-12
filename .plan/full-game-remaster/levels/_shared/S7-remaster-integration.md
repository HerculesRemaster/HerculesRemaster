# S7 Remaster integration (renderer and art pipeline per level)

Goal: give every level the same path from original draws to approved 4K replacement art through the hybrid renderer, without changing logic, logical size, pivot or timing.
Needs:
- `runtime/display/gpu_renderer.cpp` (texel-hash override `<key16hex>_<w>x<h>.png`, dev overlay; `docs/gpu-renderer.md`)
- the P7 pipeline (`tools/hires/`, `docs/hires-pipeline.md`, `docs/art-guide.md`)
- `docs/authorization-scope.md` (Q-02)
- **P7 pilot approved**, because rule 8 forbids bulk 4K art before approval. P7 is currently in progress in the lean plan, waiting on the user's pilot images.

Slices: P7 (pipeline reuse), P8 (all-level implementation, performance, packaging).
Outputs:
- `extracted/meta/full-game/art-inventory/<DIR>.json`
- `remastered/<DIR>/`
- `docs/release/content-matrix.md` (P8)

## Tasks

- [x] `S7.01` Write a per-level art-inventory tool, `tools/hires/inventory.py`. During an S1 scenario run in GPU mode it collects every override key with w×h, the source (map tile, sprite bank/frame, TGA, model), scene and first tick, and links each key to its decoded source asset where the VRAM upload is known.
- [x] `S7.02` Make sure the renderer covers both engines: ENG3 draws pass through `GFX_DrawOTag` (seen in `gpu_10500`), and T28 models are drawn (S2). Note any primitive types not yet handled, per level.
- [x] `S7.03` For each level, generate jobs through `tools/hires/jobs.py` from the inventory, once the P7 pilot is approved. Checks use `tools/hires/checks.py` (dimensions, alpha, pivot × scale, frame count, seams).
- [x] `S7.04` Run a per-level performance capture with `tools/perf/` (P8): p50/p95 frame time at 3840×2160 in the busiest scene. The target is Q-05 (16.67 ms p95).
- [x] `S7.05` Add a row per level to `docs/release/content-matrix.md`.

## Done when

- Each level has an art inventory, approved art packed, a performance measurement, and a matrix row.
- The user's side-by-side play check has passed per level (`Lxx.RM.*`).

## Notes
- `S7.01` (2026-10-12, gamehercules-40): `tools/hires/inventory.py` → `extracted/meta/full-game/art-inventory/<DIR>.json`, one file per level plus `_shared_*` / `_unlinked_<ENGINE>`.
  - **Input:** the renderer's art_dump index (`working/game/art/dump/index.csv`: key, w×h, tpage, u, v, screen position, first frame, module). For the ENG3 levels it came from three cheat_select GPU runs with art_dump on (`…/gauntlet-opening/20261012-081042`, `…/chase-opening/20261012-081336`, `…/passage-opening/20261012-081627`). New blocks were copied to `extracted/art_blocks`.
  - **Links (exact content):**
    - map: the key is recomputed from the level's offline vram.bin at tpage/u/v;
    - hud: STPANEL2.DAT, for alt = 1 rows (VRAM y + 512; 198 of 263 alt rows match);
    - upload: the captured VRAM upload pairs;
    - sprite / screen / ui / level: from `remastered/manifest.json`.
  - **Static completion:** every part key of every frame of the level's banks is listed, with `seen: false` when not yet drawn. This covers routes and frames the runs did not reach.
  - **Limits:** the index has no tick or level column (first presented frame and module only), so scenes are attributed through the links. Unlinked: ENG3 1061 (runtime pages tpage 12/10/14/6, odd-sized blocks, the uploads after the first 16 captured per scene), ENG1 498, TITLE 160. ENG3 map keys come only from runtime draws until the ENG3 mesh UVs (n8 meshes, texture words 0x40658) are decoded statically.
- `S7.02` (2026-10-12, gamehercules-40): ENG3 goes through `GFX_DrawOTag` like ENG1. The first GPU-vs-EXE comparison of all three ENG3 levels found two renderer gaps, both fixed in `gpu_renderer.cpp` and deployed to `working/game/ddraw.dll`:
  - the EXE's < 0x800 polygon extent rule, missing from the GPU path, blanked about every other ENG3 frame;
  - per-OT list clearing (now accumulated per present).

  After the fixes only the known perspective-texture sampling difference remains. Details, numbers and the ENG3 primitive codes are in `docs/gpu-renderer.md` "ENG3 levels". ENG1 (incl. T28 models) was covered by the P6/P8 runs.
  - Open: the flat path 0x40c2e0 is not read, so it does not apply the extent rule.
- `S7.04` progress (2026-10-12, gamehercules-40):
  - **ENG3 levels:** measured and within Q-05 after a renderer fix (semi runs copy only their box, pixel-identical). GAUNTLET p95 10.6 ms, CYCLOPS 8.5 ms, TORMENT 14.4 ms (`docs/gpu-renderer.md`).
  - **ENG1 levels:** to re-measure with the new `ddraw.dll`. Earlier P8 numbers predate the fix.
- `S7.04` (2026-10-12, done): per-level performance at 9× (3840×2160 targets), measured on in-level frames (after the game tick resets at the level load), entry through cheat_select, with the current `ddraw.dll`. p95: TRAINING 6.6 ms, GAUNTLET 10.5, FOREST 8.6, THEBES 7.2, HYDRA 5.9, MEDUSA 9.0, CYCLOPS 8.5, TITANS 7.8, TORMENT 14.4, VORTEX 8.9. All are within Q-05 (16.67 ms).
  - CSVs: `analysis/runtime/perf/eng?_<LEVEL>_region.csv`.
  - The "busiest scene" per level is the busiest one reached in a 160 s no-input run. Later sections are re-measured from the played runs (`docs/full-game/play-sessions.md`).
- `S7.05` (2026-10-12, done): `docs/release/content-matrix.md`, generated by `tools/meta/content_matrix.py` from the project files. One row per level: map, banks, audio, spec, art keys, packed art, perf p95, play check. Re-run it after any change.
- `S7.03` (2026-10-12): P7 approved and the route switched to xBRZ (other sessions). Every manifest entry is generated, checked and packed (10,571 jobs), including the three ENG3 maps added by `tools/hires/contract_eng3.py` (gamehercules-40).
  - Per-level coverage of the inventory keys: 80–100 % (`docs/release/content-matrix.md`).
  - The uncovered keys are mainly scaled or rotated quads and runtime-composed VRAM (`art-inventory/_unlinked_*`).
