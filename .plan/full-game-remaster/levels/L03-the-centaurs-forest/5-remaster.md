# L03 Phase 5 — Rendering and remaster integration

Goal: L03 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- S2 model rendering
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/FOREST.json`, `remastered/FOREST/`, the L03 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L03.RM.01` (P8) Run every L03 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to T28 models and the 11 FOREST layers.
- [ ] `L03.RM.02` (S7.01) Build the FOREST art inventory: override keys per scene, linked to the source tiles, banks and frames, Nessus included.
- [ ] `L03.RM.03` (S7.03) Generate, check and approve the FOREST and Nessus art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/FOREST/`.
- [ ] `L03.RM.04` (S7.04) Measure performance in the busiest scene (dense background or the Nessus fight) against Q-05.
- [ ] `L03.RM.05` **[HUMAN]** The user plays L03 from entry to the L04 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L03 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
