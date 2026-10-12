# P8 Expand, optimize, package

Goal: extend the proven workflow level by level, hit 4K performance on the target machine, and ship an installable build.
Needs: P7.
Outputs: `docs/release/content-matrix.md`, `tools/perf/`, `tools/package/`.

## Tasks

- [ ] Write `docs/release/content-matrix.md`: one row per level, with a status for layout, collision, enemies, scripts, camera, sound, checkpoints, scoring and completion.
- [ ] Add a per-family original/remastered asset switch to the runtime.
- [ ] Implement all ten levels from the P3–P5 registry, scene traces, decoded assets and gameplay specifications, updating the matrix. Any newly discovered scene/resource/behavior gap reopens its P3–P5 coverage cells; preserve existing task/scenario IDs.
- [ ] Generate the 4K art for each level through the P7 pipeline.
- [ ] Write `tools/perf/`. It measures frame-time p50/p95, stalls, texture memory and load times at 3840x2160 in busy scenes. The target is 16.67 ms p95 (Q-05).
- [ ] Write `tools/package/`. It produces a reproducible build with versioned resource manifests, notices and settings/save behaviour.
- [ ] **[HUMAN]** The user installs and runs the package on a clean Windows 11 (Sandbox or VM).

Done when all levels play in 4K within the budget from an installed package.

## Notes
