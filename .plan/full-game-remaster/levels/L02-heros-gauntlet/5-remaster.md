# L02 Phase 5 — Rendering and remaster integration

Goal: L02 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline; `S7.02` ENG3 renderer coverage)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/GAUNTLET.json`, `remastered/GAUNTLET/`, the L02 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L02.RM.01` (P8) Run every L02 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. This is the first full ENG3 level through the renderer; `gpu_10500` only shows one attract frame. Pay attention to ENG3-specific primitives found in `S7.02`.
- [ ] `L02.RM.02` (S7.01) Build the GAUNTLET art inventory: override keys per scene, linked to the source map data, banks and frames, both branches of every crossroad included.
- [ ] `L02.RM.03` (S7.03) Generate, check and approve the GAUNTLET art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/GAUNTLET/`.
- [ ] `L02.RM.04` (S7.04) Measure performance in the busiest scene (the densest hazard section found in `L02.TR.03`) against Q-05.
- [ ] `L02.RM.05` **[HUMAN]** The user plays L02 from entry to the L03 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L02 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
