# WASM port — repository audit

Audit date: 2026-10-13. Scope: `C:\Games\GameHercules` (originals) and `C:\Games\GameHercules\Remaster` (workspace).
This audit supports the browser/WebAssembly plan in this folder ([README](README.md)). Labels: **confirmed**
(read in code, binary or Ghidra), **hypothesis**, **unknown**.

## 1. Originals (game root, read-only)

| file | bytes | role | notes |
|---|---:|---|---|
| `HERCULES.EXE` | 275,968 | platform layer (MFC app): window, DirectDraw, DirectSound, MCI CD audio, joystick, registry, archive reader, software rasteriser, 250-dword function table | base 0x400000, no `.reloc`, 112 KERNEL32 imports + 9 other DLLs |
| `TITLE.DLL` | 180,736 | title, menus, password screen, FMV calls, save slots | 46 KERNEL32 imports, export `PC_DLLEngineMain` RVA 0xc1f0 |
| `ENG1.DLL` | 481,280 | level engine for TRAINING, FOREST, THEBES, HYDRA, MEDUSA, TITANS, VORTEX | 52 imports, export RVA 0x2c180 |
| `ENG3.DLL` | 272,896 | level engine for GAUNTLET, CYCLOPS, TORMENT | 52 imports, export RVA 0x1f830 |
| `HERCULES.FS` | 46,924,407 | resource archive (1,024 × 12-byte index, 820 used entries) | `docs/archive-format.md` |
| `HERC.REG`, `SETUP.BAT` | 664 / 43 | registry import: Path, Data, 0x48-byte `Config` blob | |
| `DISNEY.INI`, `HERCULES.HLP`, `unins000.dat`, `ReadMe.txt` | | installer leftovers | not used at runtime |

Hashes: `.plan/hercules-remaster/FACTS.md`, `manifests/SHA256SUMS`. Copies: `backups/originals/` (gitignored).
**Not present anywhere (confirmed by a file search of the game root):** CD audio (track 2), a disc image, `.etv` FMV files.

PE facts (pefile, confirmed):

| | ENG1 | ENG3 | TITLE |
|---|---|---|---|
| SizeOfImage | 0x7e000 | 0x4b000 | 0x36000 |
| entry point (MSVC CRT `_DllMainCRTStartup`) | 0x46730 | 0x2a480 | 0x1d950 |
| HIGHLOW relocations | 13,882 | 6,917 | 7,324 |
| instructions (Ghidra) | 87,529 | 55,738 | 35,289 |

All three DLLs: linker 5.0 (MSVC 5), no TLS, no resources, no load config, `.text` RX only.

## 2. Workspace layout (`Remaster/`)

| path | content | relevance to the web port |
|---|---|---|
| `runtime/display/` | the shipping runtime. An x86 proxy `ddraw.dll`: `ddraw_proxy.c` (592 lines), `gpu_renderer.cpp` (953, D3D11, inline HLSL), `cd_audio.cpp` (180, Media Foundation + waveOut), `build.bat` (`cl /LD /MT`), `hercules_display.ini` template | **reuse**: renderer rules and shaders, CD-segment logic, ini semantics, portable Config blob |
| `runtime/hooks/` | `hercules_hook.c` (888), `herc_inject.c` (60), `build.ps1` | reference for slot tracing; its hook list doubles as an index of EXE entry points |
| `runtime/prototypes/spike/` | `spike.cpp` (223): standalone x64 D3D11 level and sprite renderer | proves decoded-asset rendering at 4K; not used by the port |
| `tools/fs/` | `fs_index.py`, `fs_extract.py`, `classify.py`, `resource_hash.py` | archive index and name-hash reference implementations |
| `tools/decode/` | `esf.py`, `sonix.py`, `level_sonix.py`, `tga.py`, `ifam.py`, `palette.py`, `anim.py`, … | format references; ESF decode must be ported to C |
| `tools/level/` | `level_decode.py` (i2.5 + CIF), `eng3_decode.py` (i1.a), previews and spec generators | not needed at runtime (the DLLs read levels themselves) |
| `tools/state/` | `demo.py`, `hero_anim.py`, `trace_state.py`, `tuning_export.py` | demo decoding for replay checks |
| `tools/hires/` | xBRZ pipeline (`jobs.py`, `xbrz_index.py`, `xbrz_lib/` GPL source, `smooth.py`, `atlas.py`, `contract*.py`) | **reuse** `atlas.py` for web art bundles |
| `tools/runtime/` | launchers, `gpu_diff.py` (39 lines), `frame_cadence.py`, `export_spike.py` | **reuse** `gpu_diff.py` for frame comparison |
| `tools/trace/` | `run_trace.ps1`, `level_sweep.ps1`, scenario and record scripts | the reference-frame capture path |
| `tools/package/` | `build_package.py`, install/uninstall scripts | Windows add-on packaging model (no game files in the zip) |
| `tools/ghidra/` | 15 MCP helper scripts incl. `p2_contract.py`, `apply_annotations.py` | regenerate contract exports |
| `tools/viewer/` | static HTML anim/level viewers | only existing browser code; not reusable for the runtime |
| `analysis/exports/` | per-binary Ghidra exports (`functions.json`, `imports.json`, …); `p2_contract.json` (250 slots); `p2_contract_hints.txt`; `p2_decomp/` (165 EXE functions); `eng1/` (28), `hero_funcs/` (78), `hero_states/` (15); `drawotag.c`, `rast_*.c` (5) | **primary source** for re-implementing the EXE side |
| `analysis/exports/full-game/` | `disasm_ENG1_DLL.json` (1259 fn), `disasm_ENG3_DLL.json` (727 fn), game-state field maps, ENG3 tables | instruction-coverage input for the interpreter |
| `analysis/annotations/` | 12 JSON files of Ghidra names/comments | |
| `analysis/runtime/` | captures: `gpu/`, `levels/`, `sweep/`, `ui/`, `art-check/`, `perf/`, … (264 `soft_<n>.png` / `gpu_<n>.png` pairs) | golden frames for the WebGL renderer |
| `analysis/traces/` | `resource-usage.jsonl`, `hero_state.csv` (per-tick hero state during the attract demo), `pairs/`, `runs/` (43), `full-game/` | per-tick reference for interpreter equivalence |
| `extracted/` | raw payloads (820), decoded audio/images/sprites/levels/demos; `meta/` tracked | not needed at runtime; `meta/` gives coverage data |
| `remastered/` | `manifest.json` (10,571 entries, scale 9), `jobs.json` (10,568 packed, 3 pending: ENG3 levels), `art/` canvases | art source for the web bundles |
| `working/game/` | runtime copy with proxy, `hercules_display.ini`, `art/` (≈1.1 GB packed key PNGs) | the art the renderer loads today |
| `dist/` | 0.1.0 / 0.2.0 / 0.3.0 zips, 0.3.0-portable (private) | Windows packaging precedent |
| `reference/` | `passwords.json`, `tuning.json`, `captures/`, `CGI/` | password level-select table |
| `web/` | **empty** | |
| `docs/` | specs: `module-interface.md`, `binary-map.md`, `gpu-renderer.md`, `music-transport.md`, `demo-format.md`, `level-format.md`, `archive-format.md`, `authorization-scope.md`, `fullscreen.md`, `runtime-launch.md`, `formats/`, `full-game/` | cited throughout this folder |
| `.plan/hercules-remaster/` | lean phase plan P0–P8 (P0–P7 done, P8 in progress) | |

The Ghidra project is `C:\Games\Ghidra\HerculesRevertEngineer.gpr`. All four binaries are open and analysed: 1,513 EXE functions, 1,349 ENG1, 816 ENG3, 478 TITLE. The project is served by ghidra-mcp on 127.0.0.1:8089. Inline scripts are disabled (`GHIDRA_MCP_ALLOW_SCRIPTS` unset).

## 3. What exists for a browser build

| item | state |
|---|---|
| C host / `herc_host` / SDL | **absent**. `runtime/prototypes/host32` was skipped in P6 (`P6-runtime-slice.md:33`) |
| x86 emulator / interpreter / lifter | **absent** |
| emscripten toolchain, `.wasm`, WebGL/WebGPU code | **absent** |
| `web/` | empty directory |
| prior browser note | `P8-expand-package.md:31` ("browser needs route B"). This plan replaces it with a route that keeps the original DLLs |

## 4. Decompiled code coverage

There is **no full decompile**.
- **EXE:** 165 functions are exported (`p2_decomp/`). They cover every non-stub slot target of the table plus the rasterisers (`drawotag.c`, `rast_*.c`).
- **DLLs:** partial snippets only (≈120 functions). Full disassembly dumps exist for ENG1 and ENG3; TITLE has none (Ghidra holds it).

## 5. Inconsistencies found

| item | values | resolution |
|---|---|---|
| used-slot counts | `p2_contract.json` 119/105/119 vs `module-interface.md:54` 122/108/122 | the contract counts callers other than `PC_DLLEngineMain`; the doc counts every reference. Not a blocker |
| demo length | `demo-format.md` 884 ticks vs `tools/state/demo.py` 881 | read the DLL loop bound 0x374 against the header size in W1 |
| demo count | doc 9 vs archive 8 (DEMO.DAT = DEMO5_2) | 8 files |
| index target format | `gpu-renderer.md:17` R8_UINT vs code `gpu_renderer.cpp:225` R16_UINT | code wins (R16_UINT) |
| `manifests/catalog.json` | every row `status: raw` | stale; use `extracted/meta/*-coverage.json` |
| `docs/release/content-matrix.md` | "art packed 0" | stale vs `pack-report.json` |
