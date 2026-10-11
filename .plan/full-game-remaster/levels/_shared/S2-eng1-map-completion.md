# S2 ENG1 map completion (i2.5)

Goal: finish the i2.5 map format and decode/preview **all seven ENG1 maps**, so the ENG1 level plans start from complete layers, collision, spawns, cameras and models.
Needs: P5 inherited decoder `tools/level/level_decode.py`, `render_preview.py`, `tools/viewer/level_viewer/`; `docs/level-format.md`.
Slices: `P5.ALL.01`–`P5.ALL.03` (ENG1 part), `P4.ALL.03`–`P4.ALL.04` (i2.5 part).
Outputs:
- `extracted/full-game/levels/<DIR>/` (preview PNGs, `viewer.html`, the `level.json` that is already decoded for `TRAINING FOREST THEBES HYDRA MEDUSA TITANS VORTEX`, re-exported only if the decoder changes)
- `docs/level-format.md` (extended)
- `extracted/meta/full-game/level-decode-coverage.json`

**VERIFIED-STATIC** (`docs/level-format.md`):
- ENG1's per-level table 0x68af0 (0x48 bytes per level ID, terminated by 999) lists IDs 0, 1, 3, 5, 6, 7, 9 and 11. **There is no entry for ID 4.**
- `LVL_LoadMap` 0x1f750 hangs on a map whose tag doesn't match the engine.
- T28 points to 3D models drawn by `GFX_DrawModel` 0x31540.

**Open** (`docs/level-format.md` § Open):
- T28 models and sky
- flat-colour quads
- T44, T4c, T50, attribute table T40, section fields 1 and 3

**Decode warnings:** VRAM overrun on HYDRA (3,156 B), MEDUSA (84,096 B) and VORTEX (13,816 B).

## Tasks

- [ ] `S2.01` (P5.ALL.02) Decompile `GFX_DrawModel` ENG1 0x31540 and its caller. Recover the T28 model record (vertices/faces/UV/tpage/transform) with `create_struct` (`LVL_`/`GFX_` prefixes). This matters most for HYDRA/MEDUSA/VORTEX, whose maps have few layers.
- [ ] `S2.02` (P5.ALL.02) Recover T44, T4c, T50 (shade), T40 (attributes) and section fields 1 and 3 from their direct consumers. Keep each field marked unknown, with its offset, until a consumer is found.
- [ ] `S2.03` (P5.ALL.01) Resolve the VRAM overrun. Trace the ENG1 0x1f9e0 stream-chunk loads and see whether the VRAM image is streamed or partial, or whether the decoder's size rule is wrong. Validate against a runtime VRAM dump (`HERC_HOOK` vramdump, as in `analysis/traces/runs/vramdump`) for one affected map.
- [ ] `S2.04` (P5.ALL.03) Extend `render_preview.py` and the level viewer to draw models, the sky, flat quads, spawns with type names, triggers and camera records. Render all seven ENG1 maps.
- [ ] `S2.05` (P5.ALL.01) For each ENG1 level, record the per-level config at 0x68af0 + 0x48·id: the type-table pointer at +0x1c (TRAINING: 0x67608) and the other fields. Write `extracted/meta/full-game/eng1-level-config.json` keyed by ID and annotated with RVAs.
- [ ] `S2.06` Update `docs/level-format.md` (confirmed RVAs; Open section shrinks) and `level-decode-coverage.json`.

## Done when

- All seven ENG1 maps decode without overrun warnings, or each remaining warning has a code-backed explanation.
- Every map has a layer/composite/collision/spawn/camera preview.
- The T28 models render in at least HYDRA's preview.
- The fields still unknown are listed, each with the consumer that needs to be checked next.

## Notes
