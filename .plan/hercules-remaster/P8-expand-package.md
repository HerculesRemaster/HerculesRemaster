# P8 Expand, optimize, package

Goal: extend the proven workflow level by level, hit 4K performance on the target machine, and ship an installable build.
Needs: P7.
Outputs: `docs/release/content-matrix.md`, `tools/perf/`, `tools/package/`.

## Tasks

- [ ] Write `docs/release/content-matrix.md`: one row per level, with a status for layout, collision, enemies, scripts, camera, sound, checkpoints, scoring and completion.
- [x] Add a per-family original/remastered asset switch to the runtime.
- [ ] Extend levels one at a time (decode → spec → implement) and update the matrix.
- [x] Generate the 4K art for each level through the P7 pipeline. (Since 2026-10-12: index-preserving xBRZ for every manifest entry via `jobs.py` + `smooth.py --method xbrz` for catalog blocks; optional redraws per sequence go through the prompts route.)
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

- 2026-10-12: `tools/hires/smooth.py`: Scale3x twice (exactly 9×) on palette indices for all 96,988 known blocks (contract keys + runtime catalog) → `art/smooth/` (275 MB, indexed, so CLUTs/fades/semi stay exact). The renderer takes the first folder that has a key (`gpu_set_art`: art_dir, then art_families in order), so `smooth` is listed last and redrawn art wins. Checked in FOREST (ENG1) and GAUNTLET (ENG3); the translucent ENG3 hero is the original's own effect (same in the soft frame).
- Perf with all art (`run_perf.ps1`, 9× targets, 90 s, `report.py --from-frame 1700` skips the intro): p95 render cost TRAINING 10.3, GAUNTLET 4.5, FOREST 8.4, THEBES 13.9, HYDRA 13.6, MEDUSA 14.5, CYCLOPS 14.6, TITANS 13.2, TORMENT 15.2 ms (budget 16.67); present interval p50 33.0 ms (30 Hz tick), ≤2 stalls >50 ms per run; art 158–950 MB. VORTEX (id 11) cannot start from a fresh game in the harness.
- Package: installer copies `art\` as one folder (≈97k files); uninstaller removes it recursively.
- 2026-10-12 (xBRZ route, whole game): `jobs.py add . --generator xbrz`, `run --jobs 3` (sprites, 540 s) + levels one at a time with `--lenient` (out of memory with parallel levels on this machine: commit charge ~37 of 40 GB), `review --no-sheets`, `approve .`, `pack` → 10,568 entries, 91,379 key files, 3.3 GB of index textures if all were resident (sprite 2,127 MB, level 1,153, screen 51, ui 4). All sprites pass size/alpha/pivot. 5 tiny frames (dots, 1-texel lines) use the Scale3x² fallback. Levels: seam warnings 3–110 per level, all on partly transparent UV-bbox blocks whose source edges are just under the 12 tiling threshold; no visible seams in TRAINING/FOREST at 30.3 fps (`level_sweep.ps1 -Levels 1,3`). `smooth.py --method xbrz` → `art/smooth` (96,988 blocks, 299 MB); the old Scale3x set is in `backups/art_smooth_scale3x/`. Bank 482 (TORMENT, ENG3): the decoded references treat PSX colour 0x0000 as transparent for non-zero indices, while the runtime drops index 0 only, so sprite alpha/checks use the index canvas. The 0.2.0 package was built from the Scale3x art: rebuild after the art is final.

- 2026-10-12 (user: portable, private, web later): the game needs `HKLM\Software\Disney Interactive\Hercules\1.00` → `Path` (HERCULES.EXE 0x4a30: no key = "Hercules is not installed", exit; else SetCurrentDirectory) and reads/writes `Config` 0x48 bytes (0x4ce0 / `RT_Shutdown` → 0x4da0); only the EXE imports ADVAPI32. The "CD check" flag 0x43a594 is never written (static non-zero in this edition). Proxy `portable = on` (env `HERC_PORTABLE`) IAT-hooks the 5 registry calls for that key: Path = proxy folder, Config as hex in `hercules_settings.ini`. `build_package.py --version 0.3.0 --art working/game/art --portable backups/originals` → `dist/HerculesRemaster-0.3.0-portable/` (game + proxy + art, 188k files, 567 MiB, no zip, private only). Ran once from there with cwd %TEMP%: Path/Config served, 29.9 fps, Config saved on exit. A browser version: superseded on 2026-10-13 by `docs/wasm/` (original DLLs in an x86 interpreter under a C/WASM host; no rewrite).
