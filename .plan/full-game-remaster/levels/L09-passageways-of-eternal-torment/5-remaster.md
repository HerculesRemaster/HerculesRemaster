# L09 Phase 5 — Rendering and remaster integration

Goal: L09 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline; `S7.02` ENG3 renderer coverage, first exercised by `L02.RM.01`)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/TORMENT.json`, `remastered/TORMENT/`, the L09 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L09.RM.01` (P8) Run every L09 Medium and Herculean scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to fog and semi-transparent draws (both are active in the ENG3 `gpu_10500` overlay) and any primitive not seen in L02/L07.
- [ ] `L09.RM.02` (S7.01) Build the TORMENT art inventory: override keys per scene, linked to the source map data, banks and frames, every section included.
- [ ] `L09.RM.03` (S7.03) Generate, check and approve the TORMENT art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/TORMENT/`.
- [ ] `L09.RM.04` (S7.04) Measure performance in the busiest scene (as found in `L09.TR.03`–`TR.05`) against Q-05.
- [ ] `L09.RM.05` **[HUMAN]** The user plays L09 from entry to the L10 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L09 Medium and Herculean scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
