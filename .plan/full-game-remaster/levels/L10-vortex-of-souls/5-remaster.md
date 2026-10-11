# L10 Phase 5 — Rendering and remaster integration

Goal: L10 and the ending play in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- S2 model rendering (this level depends on it more than any other, since the map itself is nearly empty)
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/VORTEX.json`, `remastered/VORTEX/`, the L10 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L10.RM.01` (P8) Run every L10 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to T28 models, effects, the scrolling texture and the ending screens.
- [ ] `L10.RM.02` (S7.01) Build the VORTEX art inventory: override keys per scene, linked to the source tiles, models, banks and frames, including the final encounter and the ending/credits screens shown in the L10 flow.
- [ ] `L10.RM.03` (S7.03) Generate, check and approve the VORTEX and ending art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/VORTEX/`.
- [ ] `L10.RM.04` (S7.04) Measure performance in the busiest scene (likely the final encounter, confirm from the trace) against Q-05.
- [ ] `L10.RM.05` **[HUMAN]** The user plays L10 from entry through the credits beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L10 scenario, including the ending.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
