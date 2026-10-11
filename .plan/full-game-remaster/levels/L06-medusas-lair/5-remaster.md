# L06 Phase 5 — Rendering and remaster integration

Goal: L06 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- S2 model rendering
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/MEDUSA.json`, `remastered/MEDUSA/`, the L06 row in `docs/release/content-matrix.md`.

## Tasks

- [ ] `L06.RM.01` (P8) Run every L06 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to T28 model draws (S7.02), the animated texture, gaze/projectile effects and any reflection effect.
- [ ] `L06.RM.02` (S7.01) Build the MEDUSA art inventory: override keys per scene, linked to the source tiles, banks (0816 and the confirmed skeleton banks), frames and model textures.
- [ ] `L06.RM.03` (S7.03) Generate, check and approve the MEDUSA art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/MEDUSA/`.
- [ ] `L06.RM.04` (S7.04) Measure performance in the busiest scene (the Medusa fight with projectiles active) against Q-05.
- [ ] `L06.RM.05` **[HUMAN]** The user plays L06 from entry to the L07 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L06 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
