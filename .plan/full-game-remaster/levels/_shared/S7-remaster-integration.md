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

- [ ] `S7.01` Write a per-level art-inventory tool, `tools/hires/inventory.py`. During an S1 scenario run in GPU mode it collects every override key with w×h, the source (map tile, sprite bank/frame, TGA, model), scene and first tick, and links each key to its decoded source asset where the VRAM upload is known.
- [ ] `S7.02` Make sure the renderer covers both engines: ENG3 draws pass through `GFX_DrawOTag` (seen in `gpu_10500`), and T28 models are drawn (S2). Note any primitive types not yet handled, per level.
- [ ] `S7.03` For each level, generate jobs through `tools/hires/jobs.py` from the inventory, once the P7 pilot is approved. Checks use `tools/hires/checks.py` (dimensions, alpha, pivot × scale, frame count, seams).
- [ ] `S7.04` Run a per-level performance capture with `tools/perf/` (P8): p50/p95 frame time at 3840×2160 in the busiest scene. The target is Q-05 (16.67 ms p95).
- [ ] `S7.05` Add a row per level to `docs/release/content-matrix.md`.

## Done when

- Each level has an art inventory, approved art packed, a performance measurement, and a matrix row.
- The user's side-by-side play check has passed per level (`Lxx.RM.*`).

## Notes
