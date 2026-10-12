# L08 Phase 5 — Rendering and remaster integration

Goal: L08 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- S2 model rendering
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/TITANS.json`, `remastered/TITANS/`, the L08 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L08.RM.01` (P8) Run every L08 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to T28 models, the flight camera's scroll and any depth scaling of sprites.
- [ ] `L08.RM.02` (S7.01) Build the TITANS art inventory: override keys per section, linked to the source tiles, models, banks and frames, including Pegasus, Titans, gods and the L08 transition screens.
- [ ] `L08.RM.03` (S7.03) Generate, check and approve the TITANS art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/TITANS/`.
- [ ] `L08.RM.04` (S7.04) Measure performance in the busiest section (a Titan encounter or dense flight section) against Q-05.
- [ ] `L08.RM.05` **[HUMAN]** The user plays L08 from entry to the L09 load beside the original and lists any differences, including flight feel and input timing. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L08 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
