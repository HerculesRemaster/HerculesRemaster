# L05 Phase 5 — Rendering and remaster integration

Goal: L05 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- S2 model rendering
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/HYDRA.json`, `remastered/HYDRA/`, the L05 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L05.RM.01` (P8) Run every L05 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to T28 model draws (S7.02), the arc camera, and the Hydra's sprite/model layering.
- [ ] `L05.RM.02` (S7.01) Build the HYDRA art inventory: override keys per scene, linked to the source tiles, bank 0804 frames and model textures.
- [ ] `L05.RM.03` (S7.03) Generate, check and approve the HYDRA art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/HYDRA/`.
- [ ] `L05.RM.04` (S7.04) Measure performance in the busiest part of the Hydra fight against Q-05.
- [ ] `L05.RM.05` **[HUMAN]** The user plays L05 from entry to the L06 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L05 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
