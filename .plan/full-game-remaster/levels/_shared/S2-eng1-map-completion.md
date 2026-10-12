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

- [x] `S2.01` (P5.ALL.02) Decompile `GFX_DrawModel` ENG1 0x31540 and its caller. Recover the T28 model record (vertices/faces/UV/tpage/transform) with `create_struct` (`LVL_`/`GFX_` prefixes). This matters most for HYDRA/MEDUSA/VORTEX, whose maps have few layers.
- [x] `S2.02` (P5.ALL.02) Recover T44, T4c, T50 (shade), T40 (attributes) and section fields 1 and 3 from their direct consumers. Keep each field marked unknown, with its offset, until a consumer is found.
- [x] `S2.03` (P5.ALL.01) Resolve the VRAM overrun. Trace the ENG1 0x1f9e0 stream-chunk loads and see whether the VRAM image is streamed or partial, or whether the decoder's size rule is wrong. Validate against a runtime VRAM dump (`HERC_HOOK` vramdump, as in `analysis/traces/runs/vramdump`) for one affected map.
- [x] `S2.04` (P5.ALL.03) Extend `render_preview.py` and the level viewer to draw models, the sky, flat quads, spawns with type names, triggers and camera records. Render all seven ENG1 maps.
- [x] `S2.05` (P5.ALL.01) For each ENG1 level, record the per-level config at 0x68af0 + 0x48·id: the type-table pointer at +0x1c (TRAINING: 0x67608) and the other fields. Write `extracted/meta/full-game/eng1-level-config.json` keyed by ID and annotated with RVAs.
- [x] `S2.06` Update `docs/level-format.md` (confirmed RVAs; Open section shrinks) and `level-decode-coverage.json`.

## Done when

- All seven ENG1 maps decode without overrun warnings, or each remaining warning has a code-backed explanation.
- Every map has a layer/composite/collision/spawn/camera preview.
- The T28 models render in at least HYDRA's preview.
- The fields still unknown are listed, each with the consumer that needs to be checked next.

## Notes
- `S2.01` (2026-10-12):
  - Models are T34 entries n_types−1+k (self-relative s32), k = object frame +0x34; layout in `docs/level-format.md` § Models.
  - Ghidra: structs `LVL_ModelHeader`/`LVL_ModelGroup`/`LVL_ModelQuad`; `GFX_DrawModel` prototype; names `GFX_SetModelOffset` 0x31d30, `GFX_DrawObjectModel` 0x313e0, `GFX_DrawModelChildren` 0x31470, `GFX_DrawModelList` 0x31330, `GFX_TransformModelVerts` 0x318d0.
  - `level_decode.py --out extracted/full-game/levels` adds `models`. All 252 decode except 8 TRAINING non-mesh records.
  - `tools/level/render_models.py` writes `models.png` per level; HYDRA's are Hydra head poses, boulders and platforms.
- `S2.05` (2026-10-12): `extracted/meta/full-game/eng1-level-config.json`, 24 fields with consumers. The record holds three object pools; class = kind & 0x7000, dispatched by `LVL_SpawnVisible` 0x262a0:
  - actors (0): +2 count, 0x134 bytes; per-kind +0x18 anim, +0x1c {spawn, stub, update}.
  - markers (0x4000): +4 count + 5, 0xec bytes; +0x20 anim, +0x24 {init, tick}; checkpoints are entries 0x0a..0x11.
  - pickups (0x2000, plus effects/children): +6 count + 5, 0x94 bytes; +0x28/+0x2c.
  - 0x1000-class kinds are not spawned there (consumer open).
  - Also: +0x38/+0x44 attribute functions/flags, +0x40 spawn templates, +0x30 unused. +8..+0x16 are still unknown, each with its consumers listed.
  - (Corrected the same day: an earlier note split the pools by 0x6000/0x8000.)
- `S2.02` (2026-10-12): `docs/level-format.md` § Tables.
  - T50 = corner light levels indexed by T54 flags >> 4; table size = max index + 1 in all seven maps; exported as `lights`.
  - Section bytes 1/3 have no ENG1 reader. T44: corrected the same day; it is the parallax background tile map read by `LVL_DrawBackgroundTiles` 0x2ff0 (that function wasn't defined in Ghidra when the first xref scan ran).
  - T4c = keyed interpolation lists for the camera (0x20ea0; only TITANS has n_e = 2).
  - T40 = per-attribute s8 grids (`LVL_CellAttribute`), whose values go through the level-config +0x44/+0x38 tables.
- `S2.03` static part: the "overrun" is the tail of the 256×256 image at VRAM (768,256).
  - `LVL_LoadMap` reads only header-size bytes into a (size & ~0x7ff) + 0x1000 buffer, but `LVL_UploadVram` always uploads the full rect, so the rows past the header hold whatever follows in memory: from y 481 in FOREST/VORTEX/HYDRA, from y 323 in MEDUSA.
  - No block or model texture of these maps samples those rows.
  - Runtime VRAM dump of one affected map still to do (game process shared with S6.03).
- `S2.04` progress: all seven maps rendered into `extracted/full-game/levels/<L>/` (layers, composite, collision overlay, `models.png`, `viewer.html` with a models panel).
  - `render_preview.py --dir` draws flat quads, spawn markers with class colours and names (pickups from `HERO_CollectPickup`, checkpoints 0x400a+), camera-record ticks, and models placed by spawn-code rules (HYDRA kind 0x31 → models 9/10 at x 0x394/0xdea, ENG1 0x1dd00).
  - Open: sky/background, attribute (trigger) overlay, the remaining kind→model rules (bosses are spawned by update code: HYDRA has 2 spawns, MEDUSA 6, VORTEX 0).
- `S2.06` (2026-10-12): `docs/level-format.md` § Models and § Tables, Open section updated; `extracted/meta/full-game/level-decode-coverage.json` (per-part status, per-level counts, warnings, previews).
- `S2.04` background: per-level background hooks found (ENG1 0x5df00 + 0x1c·level +0x14 → 0x70d24, called by 0x2da90): TRAINING 0x42320, FOREST 0xe400, THEBES 0x36c40, HYDRA 0x1efa0, TITANS 0x3c600, VORTEX 0x2fa0; none for MEDUSA. Clear colours and T44 scroll rates are in `docs/level-format.md` § Background hooks. Rendering T44 waits on its texture page/CLUT (runtime).
- `S2.04` triggers: collision previews outline every cell with an attribute (bits 24–30) and label it `a<id>`. In HYDRA, attribute 1 marks the two arena wall columns (level flags & 7 = blocking). Remaining for S2.04: rendering the T44 background (page/CLUT at runtime) and the other model-spawn rules.
- `S2.03` done (2026-10-12): checked against runtime VRAM. Run `analysis/traces/full-game/l05-hydra-canyon/canyon-entry/20261012-063658-debug_level-medium` (DEBUG ONLY entry, HYDRA, dumps at ticks 1500/2500).
  - The 768×512 image and the read part of the 256×256 image match the decoded VRAM byte for byte.
  - The rows past the map header (y ≥ 481, x ≥ 768) hold other, stable data at runtime.
  - The decoder now mirrors the engine: it reads image bytes only up to the header size and zero-fills the rest, with a `notes` entry per map. There are no more overrun warnings (FOREST, VORTEX, MEDUSA and HYDRA carry the note).
- `S2.04` done (2026-10-12): every map has layer/composite/collision previews with spawns, camera ticks and attribute (trigger) cells, plus models.png, background.png and viewer.html.
  - `tools/level/render_background.py` renders T44 from page 0x9c/0x9e (draw modes 0x1f320 confirm the page). Tile: u = t & 0xf8, v = t >> 8 & 0xf8, page bit 8, mode bits = 2× horizontal/vertical scale. MEDUSA has no T44.
  - Models are placed where a spawn rule is static (HYDRA pillars). Boss models (Hydra heads/necks, Medusa) are posed by code each tick; see `encounters.md`.

- `S2.03` follow-up (2026-10-12): all seven ENG1 maps now checked through in-game entries (cheat level select, no memory writes, dumps at ticks 4200/5000): img0 and the loaded img1 rows are byte-identical for TRAINING, THEBES, MEDUSA, TITANS, VORTEX, and FOREST except its two animated rects (runtime frames = T38 table frames). The unloaded img1 tails (FOREST/VORTEX 31 rows, MEDUSA 189 rows) are not the file continuation at runtime (leftover memory, mostly zero, unchanged between dumps). Runs under `analysis/traces/full-game/<level>/full-progression/2026101213*-cheat_select-medium` and `...14*`; per-level results in each L*.AS.02 note.
