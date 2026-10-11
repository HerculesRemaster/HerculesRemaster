# S3 ENG3 engine and i1.a maps

Goal: give ENG3 (L02 GAUNTLET, L07 CYCLOPS, L09 TORMENT) the same level of understanding ENG1 already has: named tick/loader/entity/hero/camera code, an i1.a map decoder and preview, ENG3 trace hooks, and the "runner" movement model. This blocks every phase of L02/L07/L09.
Needs: P2 Ghidra project `C:\Games\Ghidra\HerculesRevertEngineer` (ENG3.DLL open), S1 hooks, `analysis/exports/ENG3.DLL/`. ENG1 counterparts serve as patterns.
Slices: `P5.ALL.01`–`P5.ALL.03` (ENG3), `P5.ALL.09`, `P4.ALL.03` (i1.a), `P3.ALL.06` (ENG3 hooks).
Outputs:
- `tools/level/eng3_decode.py` (i1.a) → `extracted/full-game/levels/{GAUNTLET,CYCLOPS,TORMENT}/level.json` + previews
- `docs/full-game/eng3-format.md`
- `docs/full-game/gameplay/chase.md` (runner model; shared by L02/L07/L09)
- `runtime/hooks/` ENG3 hook additions

## What is already known

**VERIFIED-STATIC:**
- Export `PC_DLLEngineMain` is at ENG3 0x1f830, the tick at 0x1abb0 (`reference/binary-analysis/ENG3.DLL-1001abb0.c`), and the map loader at 0x1a930 (`ENG3.DLL-1001a930.c`).
- The descriptor table is at 0x38528, with 0x18-byte records: ID 2 at 0x38558, ID 8 at 0x385e8, ID 10 at 0x38618.
- The loader has the same tag check and hang as ENG1. The BIN tag is `i1.a` (`69312e61`); every CIF is 336,384 B.
- Descriptor path pointers lead to `O:\GAUNTLET\` (0x10038660), `O:\CYCLOPS\` (0x10038654) and `O:\TORMENT\` (0x10038648). The second descriptor field holds 0x129e28, 0x125184 and 0x10903c (`reference/binary-analysis/dispatch-tables.json`). Each value equals that level's full BIN payload size in `levels.json`, whereas ENG1's field +4 is only the header size (`docs/level-format.md`). This suggests ENG3 loads the whole map at once with no streaming, which fits `stream_chunk: null`. Confirm it in S3.02.
- `analysis/exports/ENG3.DLL/strings.json` lists only GAUNTLET and TORMENT. Its string scan missed `O:\CYCLOPS\`, so treat that export as incomplete.
- The ENG3 strings also contain the SRC3 bank paths (HERCULES, XTRA_DB1, LETTERS, ALIEN1, CYCLOPS, CYEND, PEOPLE, TORMENT, SOULS), all DEMO paths, and `stpanel2.dat`.
- `analysis/exports/ENG3.DLL/functions.json` has 620 functions; only CRT functions are named.

**VERIFIED-RUNTIME:** `analysis/runtime/levels/gpu_10500.txt` shows `module ENG3 map o:\gauntlet\map_psx.cif` at frame 10500 of an unattended GPU-renderer run, with the GPU renderer drawing the frame (6312 verts). The run was presumably in the attract demo cycle; the file records no demo ID. This shows `GFX_DrawOTag` already covers ENG3 draws.

**UNRESOLVED:**
- the i1.a layout
- the ENG3 per-level config table (the counterpart of ENG1 0x68af0)
- entity type table, hero code, camera/runner logic
- whether ENG3 has its own stream-chunk loader (`levels.json` has `stream_chunk: null`)
- the purpose of `stpanel2.dat`

## Tasks

- [ ] `S3.01` (P5.ALL.01) Name the ENG3 skeleton in Ghidra, using the ENG1 rename prefixes:
  - `MOD_Tick` 0x1abb0 and `LVL_LoadMap` 0x1a930
  - the callees of `PC_DLLEngineMain`
  - EXE slot calls through the 108-slot table
  - ENG3's private copies of the draw/anim code (`docs/formats/animation.md` says ENG3 and TITLE have their own copies)

  `save_program` after each batch.
- [ ] `S3.02` (P5.ALL.01) From 0x1a930 and its callees, recover how the descriptor 0x18-byte record is used: path, header size, start points. Recover the per-level config table, if it exists, and the remaining descriptor fields. Also re-run the ENG3 string export so it includes the descriptor-referenced paths that the first scan missed (`O:\CYCLOPS\` at 0x10038654).
- [ ] `S3.03` (P5.ALL.02) Recover the i1.a header and sections with `create_struct`: layers/track segments, collision/lanes, spawn/obstacle tables, camera, palette refs. Keep unknown fields with their offsets. Do not force the i2.5 schema onto them.
- [ ] `S3.04` (P4.ALL.03) Write `tools/level/eng3_decode.py`. It raises `UnsupportedVariant` on unknowns, uses the existing CIF palette/fog code from `tools/decode/palette.py`, and exports all three maps.
- [ ] `S3.05` (P5.ALL.03) Write an ENG3 preview: track/lanes, obstacles, spawns, camera path. Either extend `tools/viewer/level_viewer/` or write a separate `eng3_viewer`.
- [ ] `S3.06` (P3.ALL.06) Add ENG3 hooks for module tick, map load, spawn/despawn, hero state and position, the ENG3 frame-request function (counterpart of ENG1 0x3cd0) and the sound-play calls. Feed them into S1 events.
- [ ] `S3.07` (P5.ALL.09) Decompile the ENG3 hero/runner update and specify it in `docs/full-game/gameplay/chase.md`:
  - forced forward motion, speed control (Up/Down is a LEAD from the walkthrough)
  - jump, lane/depth changes, branch selection
  - obstacle collision, fall/death
  - completion trigger, difficulty guards

  Keep constants in their original representation.
- [ ] `S3.08` (P5.ALL.06) Decode `DEMO2_1` through ENG3's demo reader. Its JSON start index is 0. Use it as the first ENG3 replay/timing check. Note whether ENG3 uses the same `docs/demo-format.md` layout.
- [ ] `S3.09` Write `docs/full-game/eng3-format.md` with RVAs, and record the remaining unknowns.

## Done when

- The three i1.a maps decode with explained or explicitly unknown fields, and render previews.
- The ENG3 tick, loader, hero, entity and camera code is named.
- ENG3 hooks emit S1 events during the GAUNTLET attract demo.
- `chase.md` covers the runner rules shared by L02/L07/L09, each rule backed by code or a trace.

## Notes
