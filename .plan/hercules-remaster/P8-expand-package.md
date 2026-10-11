# P8 Expand, optimize, package

Goal: extend the proven workflow level by level, hit 4K performance on the target machine, and ship an installable build.
Needs: P7.
Outputs: `docs/release/content-matrix.md`, `tools/perf/`, `tools/package/`.

## Tasks

- [ ] Write `docs/release/content-matrix.md`: one row per level, with a status for layout, collision, enemies, scripts, camera, sound, checkpoints, scoring and completion.
- [x] Add a per-family original/remastered asset switch to the runtime.
- [ ] Extend levels one at a time (decode → spec → implement) and update the matrix.
- [ ] Generate the 4K art for each level through the P7 pipeline.
- [x] Write `tools/perf/`. It measures frame-time p50/p95, stalls, texture memory and load times at 3840x2160 in busy scenes. The target is 16.67 ms p95 (Q-05).
- [x] Write `tools/package/`. It produces a reproducible build with versioned resource manifests, notices and settings/save behaviour.
- [ ] **[HUMAN]** The user installs and runs the package on a clean Windows 11 (Sandbox or VM).

Done when all levels play in 4K within the budget from an installed package.

## Notes
- Started 2026-10-12 while P7 waits on the user's pilot images (art-independent tasks only; no art generated).
- Renderer fixes found by checking all levels: `GFX_FrameBrightness` EXE 0x70c0 (fades: shade[b] over the frame before present, applied in place; GAUNTLET was 100 % off, now sampling-level); decreasing-UV rule (mirrored parts sampled one texel low: blue edge streaks in THEBES).
- Level harness (trace hook only): `HERC_HOOK_PAD` scripted pad + `HERC_HOOK_LEVEL` (rewrites the next ENGINE1/ENGINE3 request, game state +0 = level, +1/+2 = 0). `tools/trace/level_sweep.ps1`: TRAINING, GAUNTLET, FOREST, THEBES, HYDRA, MEDUSA, CYCLOPS, TITANS, TORMENT load and render at ~30 fps; VORTEX from a fresh game is fatal `PC_fopen("\ANIMPSX.BIN")` (needs progress state). Flaky start stalls (tick 10–21) need a rerun.
- Art switch: `art_families` (ini) + art/<family>/ (atlas.py), F11 toggles all art. Perf: `HERC_PERF` CSV (CPU/Present/GPU timestamps), `HERC_GPU_SCALE=9`, `tools/perf/{run_perf.ps1,report.py}` (not yet run). Package: `tools/package/build_package.py` (MSVC /Brepro, deterministic zip, manifest with sha256s, edition check, install/uninstall with backup); 0.1.0 built twice with identical sha256.
- Open: content matrix (superseded by the full-game plan's per-level plans, user 2026-10-12), perf run, art per level (after P7), [HUMAN] clean-VM install.

