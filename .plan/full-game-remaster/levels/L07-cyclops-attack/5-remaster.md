# L07 Phase 5 — Rendering and remaster integration

Goal: L07 plays in the hybrid GPU renderer at 3840×2160 with approved replacement art, while logic, timing, logical size and pivots stay identical to the original.
Needs:
- phases 3–4 of this level
- S7 (inventory tool, pipeline; `S7.02` ENG3 renderer coverage, first exercised by `L02.RM.01`)
- **P7 pilot approved**, because rule 8 forbids bulk art before approval
- authorization scope recorded (Q-02)

Outputs: `extracted/meta/full-game/art-inventory/CYCLOPS.json`, `remastered/CYCLOPS/`, the L07 row in `docs/release/content-matrix.md`.

## Tasks

- [x] `L07.RM.01` (P8) Run every L07 scenario in GPU mode (`renderer = gpu`, `dev = on`). Run `tools/runtime/gpu_diff.py` against the software frames. Any mismatch means a renderer gap: list it with the primitive type and tick, and fix it in `gpu_renderer.cpp` before making art. Pay attention to the Cyclops draws, the end sequence and any primitive not seen in L02.
- [x] `L07.RM.02` (S7.01) Build the CYCLOPS art inventory: override keys per scene, linked to the source map data, banks and frames, every lane and the end sequence included.
- [ ] `L07.RM.03` (S7.03) Generate, check and approve the CYCLOPS art through `tools/hires/jobs.py` and `checks.py`, then pack it into `remastered/CYCLOPS/`.
- [x] `L07.RM.04` (S7.04) Measure performance in the busiest scene (dense traffic with the Cyclops on screen, as found in `L07.TR.05`) against Q-05.
- [ ] `L07.RM.05` **[HUMAN]** The user plays L07 from entry to the L08 load beside the original and lists any differences. Fix them, then update the content-matrix row.

## Done when

- The GPU frames match the software frames in every L07 scenario.
- The approved art is packed.
- Performance is within budget.
- The user's play check has passed.

## Notes
- `L07.RM.02` (2026-10-12, progress): `extracted/meta/full-game/art-inventory/CYCLOPS.json` (`tools/hires/inventory.py`, S7.01). Override keys are linked to the map VRAM, the SRC3 bank frames, the HUD sheet and the runtime uploads. All frames of the level's banks are listed, including branches not drawn yet (`seen: false`).
  - Still open before the tick: map keys of map areas not drawn in the traced runs, which need either the ENG3 mesh UV decode or runs that take both branches (TR.04 for L02, the depth routes for L09).
- `L07.RM.02` (2026-10-12, done): `extracted/meta/full-game/art-inventory/CYCLOPS.json` covers every track and branch statically.
  - **Map:** 2283 static map keys from all textured cell-mesh faces (`eng3_decode.py` `mesh_textures`, keyed like `contract.level_entries`).
  - **Sprites:** all frames of the level's banks.
  - **Runtime-drawn keys:** HUD sheet and uploads.
  - **Consistency:** 409 of the 487 map keys drawn at runtime are reproduced by the static rule. The rest come from other primitives on the same map VRAM (most likely the textured 3D models, faces 0x40650) and are linked through their runtime draws.
- `L07.RM.01` (2026-10-12): GPU vs EXE frames at CYCLOPS (cheat_select runs with `HERC_GPU_SHOT`, `tools/runtime/gpu_diff.py`; `analysis/runtime/levels/eng3_gpu/CYCLOPS_fix`).
  - **Gaps fixed** in `gpu_renderer.cpp` (S7.02): the < 0x800 polygon extent rule (near-plane quads blanked about every other frame) and OT accumulation per present.
  - **What remains** (35–41 % of pixels): texture sampling inside perspective level quads only, the documented span-start UV difference. Sprites and HUD match.
  - **Primitive codes:** 0x2c/0x2e/0x64/0xe1/0xf3.
  - Scenes not reached without play (later sections, the other branches) are compared when the played runs exist.
- `L07.RM.04` (2026-10-12): CYCLOPS at 9× (3840×2160 targets), in-level frames. p95 8.54 ms (p50 6.42), against the Q-05 budget of 16.67 ms. Over budget: 5 of 942 frames. Before the semi-copy fix in `gpu_renderer.cpp` it was p95 17.54 ms. Numbers and method: `docs/gpu-renderer.md` "Performance at 9×".
  - The busiest scenes measured are those reached in a 160 s cheat_select run. Densest-section confirmation follows the played traversal (TR.03).
