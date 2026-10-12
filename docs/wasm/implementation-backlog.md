# WASM port — implementation backlog

Each task can go to one coding agent.

| field | values |
|---|---|
| priority (P) | P0 critical path, P1 needed for a playable build, P2 polish |
| size | S ≤ 1 day, M 2–4 days, L ≥ 1 week |
| risk | ids from [risks-and-open-questions.md](risks-and-open-questions.md) |
| done when | the one check that closes the task. Lean rules apply: run it once, note the result in the phase notes, write no test suites |

Shared conventions for all tasks:
- Addresses are written as module + RVA.
- Every host slot handler carries a comment naming its EXE source, e.g. `/* EXE 0x4570 RT_Malloc, p2_decomp/00404570.c */`.
- Nothing is written into `C:\Games\GameHercules` outside `Remaster\`.
- Originals are read from `working/game/` or `backups/originals/`.

## W0 — Scope

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W0-01 | Record the web release scope: local/LAN only, user-supplied originals, no hosted originals or derived art or code, public deploy blocked on the licence | `docs/authorization-scope.md` (new section) | — | P0 | S | R-13 | the section exists and the user has confirmed it |
| W0-02 | Ask the user to approve installing the Emscripten SDK (pinned version, installed outside the game folder). Record the decisions D-W0-1…4 | `.plan/hercules-remaster/FACTS.md` open questions | — | P0 | S | R-12 | answers recorded |

2026-10-13: **D-W0-2 is approved and installed** (SDK 6.0.12; see [toolchain.md](toolchain.md)).
The other W0 decisions remain to be recorded; this installation does not complete W0-02 as a whole.

## W1 — Native host (real CPU)

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W1-01 | Host skeleton: a `HostState` that holds gameState, the arena, the +0x10 block (size from W1-04; until then 64 KB) and the sine table; a 250-entry table builder; a `herc_host` Win32 x86 window and the `RT_Tick` timing port | new `runtime/host/{host.h,table.c,boot.c}`, `runtime/herc_host/{main.c,build.ps1}`; reference `p2_decomp/00405f20.c`, `00401ff0.c` | W0 | P0 | M | — | `herc_host` opens a window and calls TITLE `PC_DLLEngineMain` with our table; magic is accepted |
| W1-02 | FS slots: in-memory archive index, `FS_NormalizePath`/`FS_HashName` port (drive aliases O:/P:/Q:/R:), 16 handles, `RT_PCopen/read/lseek/close`, `RT_LoadFile`, debug open/close wrappers (0x020/0x024/0x030/0x048/0x080/0x09c/0x0c0), stream read 0x02c | new `runtime/host/fs.c`; reference `tools/fs/resource_hash.py`, `p2_decomp/0040a210.c` etc. | W1-01 | P0 | M | — | every file the title opens resolves (log matches `analysis/traces/resource-usage.jsonl` open events for the first 1,200 ticks) |
| W1-03 | Heap and RNG slots as exact ports (0x038/0x040/0x044/0x050/0x078/0x0a0/0x0a4/0x034/0x0a8, 0x094/0x098). Also check the calling convention of every slot (I-06) | new `runtime/host/{heap.c,rng.c}` | W1-01 | P0 | S | — | the arena block headers after 1,200 ticks equal a memory dump from the original at the same tick (hook DLL dump) |
| W1-04 | Resolve I-01: role and size of the table +0x10 block | Ghidra (read-only), notes in `docs/module-interface.md` | — | P0 | S | R-04 | size and initial contents documented |
| W1-05 | GTE and libgpu-style slots (0x190–0x1e4, 0x28c–0x294, 0x2b8/0x2bc, 0x04c, 0x0c8, 0x0cc, 0x0e0, 0x1f4, 0x208, 0x2b4, misc leaf helpers) | new `runtime/host/{gte.c,gpu_state.c}` | W1-01 | P0 | M | — | all such slots implemented; no "unimplemented slot" trap during the demo cycle |
| W1-06 | System slots: fatal error, quit, message pump (0x250 with the etv-missing path), frame counter 0x3e0, no CD check (R-09), Config accessors 0x224/0x240/0x248 with the 0x48-byte blob from `hercules_settings.ini` | new `runtime/host/{system.c,config.c}`; reference `ddraw_proxy.c:165-243` | W1-01 | P0 | M | R-09 | title menus, options and quit work |
| W1-07 | Module switch: port of `MOD_ChangeModule`; native `setjmp/longjmp` around the DLL entry; FreeLibrary/LoadLibrary by basename | `runtime/host/module.c` | W1-02, W1-03 | P0 | S | R-03 | title → attract demo (ENG1) → title cycle runs 3 times |
| W1-08 | Renderer hookup: refactor a copy of `gpu_renderer.cpp` to read `HostRenderState` (VRAM, palette, fog, blend, shade, flags, brightness) instead of EXE addresses; VRAM slots 0x204/0x20c/0x208 write into host VRAM; slot 0x164 calls the walker; DrawSync callback via 0xa8e780 semantics | new `runtime/herc_host/render_d3d11.cpp` (from `runtime/display/gpu_renderer.cpp`), `runtime/host/vram.c` | W1-05 | P0 | M | R-04 | title and demo frames visible; `gpu_diff.py` on 5 reference frames no worse than the proxy |
| W1-09 | Trace and checksum: per-slot log (tick, module, caller RVA, slot, args, ret) and per-tick FNV-1a of gameState+arena; output format shared with W2 | `runtime/host/trace.c` | W1-01 | P0 | S | R-02 | log written for a demo cycle |
| W1-10 | Demo-equivalence check: extract hero x/y/state per tick from host memory (field offsets from `tools/state/trace_state.py`) and compare with `analysis/traces/hero_state.csv` | small script `tools/web/hero_compare.py` | W1-07, W1-09 | P0 | S | R-04 | identical for all demo ticks |
| W1-11 | Input: keyboard → `RT_ReadPadBits` semantics (opposite directions removed, default keys from Config), slots 0x258/0x084/0x1fc/0x284/0x3c0/0x3e4 | `runtime/host/input.c`, `runtime/herc_host/input_win.c` | W1-06 | P1 | S | — | password entry works; TRAINING playable by hand |
| W1-12 | Audio (native): C ESF decoder, Sonix bank slots, voice slots, waveOut mixer; music stubbed (no track) | `runtime/host/{esf.c,sound.c}`, `runtime/herc_host/audio_win.c` | W1-06 | P1 | M | — | C decoder output equals `tools/decode/esf.py` on the 4 verified pairs; SFX audible in the demo |
| W1-13 | ENG3 entry via password (GAUNTLET, CYCLOPS, TORMENT) | — | W1-07 | P0 | S | R-03 | each ENG3 level loads and plays 60 s |

## W2 — Interpreter

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W2-01 | PE loader: map sections into guest memory, apply HIGHLOW relocations for the chosen base, fill the IAT with trap addresses, find the export `PC_DLLEngineMain`, call DllMain | new `runtime/cpu/pe.c` | W1-09 | P0 | S | — | all 3 DLLs map; relocation count equals the pefile counts (13,882 / 6,917 / 7,324) |
| W2-02 | Opcode inventory: export the TITLE disassembly from Ghidra (like `disasm_ENG*_DLL.json`) and generate the list of (opcode, ModRM form) that must be supported | extend `tools/ghidra/fn_match.py` (it produced `disasm_ENG*_DLL.json`), output `analysis/exports/full-game/disasm_TITLE_DLL.json`, `docs/wasm/opcodes.md` | — | P0 | S | R-01 | list generated |
| W2-03 | Absolute-address scan: confirm that no DLL immediate or relocated value points into EXE ranges 0x400000–0xbad000 | script over pefile + relocations | — | P1 | S | R-04 | report: zero hits, or each hit explained |
| W2-04 | CPU core: decoder + decoded cache, integer ops with lazy flags, string ops with REP, x87 subset with counters, unknown-opcode trap | new `runtime/cpu/{x86.c,x86_ops.inc,x87.c}` | W2-01, W2-02 | P0 | L | R-01, R-05 | TITLE runs 1,200 ticks under `herc_host --interp` |
| W2-05 | Traps and `cpu_call`: slot and KERNEL32 dispatch, sentinel return, re-entrancy guard, stdcall pops | `runtime/cpu/cpu_call.c`, `runtime/cpu/k32.c` | W2-01 | P0 | M | R-02 | DllMain of all 3 DLLs returns TRUE under the interpreter |
| W2-06 | Module-switch abort path (no longjmp): `HOST_ABORT` unwinding, stack reset, DETACH/ATTACH, flush the decoded cache | `runtime/cpu/cpu_call.c`, `runtime/host/module.c` | W2-05, W1-07 | P0 | S | R-03 | demo cycle ×3 under the interpreter |
| W2-07 | Trace diff: W1 vs W2a logs and checksums over the demo cycle and one ENG3 entry | `tools/web/trace_diff.py` | W2-06, W1-13 | P0 | S | — | zero differences |
| W2-08 | Emscripten headless build: host + cpu, no renderer or audio, files from a test page, prints ms/tick | new `runtime/web/{main_web.c,build.ps1}`, `web/bench.html` | W2-07, W0-02 | P0 | M | R-01 | numbers recorded for Chrome and Firefox (M1 complete) |
| W2-09 | Performance pass: profile hot opcodes, REP fast paths, block-level dispatch, memory access without per-byte bounds checks | `runtime/cpu/*` | W2-08 | P0 | M | R-01 | p95 ≤ 8 ms/tick, or a written recompilation decision |

## W3 — WebGL2 renderer

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W3-01 | Shader spike: port `kSrc` (VS, PSOpaque, PSSemi, PSResolve) to GLSL ES 3.00 with R8UI/R16UI/RGBA8 and draw one recorded OT (`gpu_<n>_ot.txt` input) | new `runtime/render/gl/shaders.glsl.h`, `spike` page | W2-08 | P0 | M | R-06 | the frame is visible and matches the D3D11 `gpu_<n>.png` by `gpu_diff` logic |
| W3-02 | OT walker in C (from `gpu_draw_ot`, `gpu_renderer.cpp:682-733`, plus the rules at `:329-380`) against `HostRenderState` | `runtime/render/gl/ot.c` | W3-01 | P0 | M | — | all primitive codes handled; multi-OT per frame |
| W3-03 | VRAM and table uploads: dirty 32×32 tiles, palette, fog, blend, shade on change; clear rule; brightness | `runtime/render/gl/textures.c` | W3-02 | P0 | S | — | title, TRAINING and an ENG3 level render |
| W3-04 | Resolve to the canvas: 4:3 integer, fit and stretch, letterbox colour, DPR | `runtime/render/gl/present.c` | W3-02 | P0 | S | — | sharp output at 1080p, 1440p and 4K |
| W3-05 | Art keys and bundle reader: FNV-1a-64 key per UV rect with tile-stamp cache (`:600-645`); `.hab` parser; decide between indexed PNG decode in C and raw index payloads | `runtime/render/gl/art.c`, `web/art.js` | W3-02 | P1 | M | R-07 | keys for 10 frames equal the D3D11 dumps |
| W3-06 | Bundle builder: per-level and common bundles at 9/6/4/3× from `working/game/art` and the manifests | new `tools/hires/bundle.py` (reuse `atlas.py`) | — | P1 | M | R-07 | bundles built; total size per scale reported |
| W3-07 | Texture LRU with a byte budget; streaming on level change | `runtime/render/gl/art.c` | W3-05 | P1 | S | R-07 | HYDRA → MEDUSA → TITANS at 4× without exceeding the budget |
| W3-08 | (optional) soft rasteriser port from `drawotag.c` + `rast_*.c` as a debug reference inside the browser | `runtime/render/soft/` | W3-02 | P2 | M | — | `soft` frames equal the original `soft_<n>.png` |
| W3-09 | Reference-frame comparison in the browser: dump the canvas at given ticks and run `gpu_diff.py` | `web/dev.js` | W3-04 | P0 | S | — | W3 gate numbers recorded |

## W4 — Audio

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W4-01 | Resolve I-04: read the voice functions 0xf710–0xfbf0 and slots 0x300–0x3a8; document volume, pan, pitch and loop rules | `docs/wasm/audio-voices.md` | — | P1 | S | — | rules documented with RVAs |
| W4-02 | Host voice model (port), emitting commands | `runtime/host/sound.c` | W4-01, W1-12 | P1 | M | — | the command stream for the demo equals `extracted/meta/full-game/sfx-events.json` |
| W4-03 | JS voice graph: AudioBuffer per ESF, AudioBufferSource per start, Gain/StereoPanner, `playbackRate` | `web/audio.js`, `runtime/web/library_herc.js` | W4-02, W2-08 | P1 | S | R-16 | SFX audible and in sync in the demo |
| W4-04 | CD-segment logic: TMSF → ms, segment table, resume after pause, volume gate (port of `cd_audio.cpp` logic + slots 0x0d4–0x0dc, 0x028/0x0c4) | `runtime/host/music.c` | W4-02 | P1 | S | — | log shows the documented segment ranges for title, password and VORTEX |
| W4-05 | Music import: user picks a track-2 file; split into segments at import (WebCodecs or `decodeAudioData` of the whole file once, then encode or store per-segment PCM) | `web/import.js` | W4-04 | P2 | M | R-08 | music plays per level |
| W4-06 | Autoplay and lifecycle: resume on Play, suspend on hidden, stop on quit | `web/audio.js` | W4-03 | P1 | S | R-16 | no console autoplay warnings |

## W5 — Input, configuration, saves

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W5-01 | Keyboard mapping with defaults from the Config blob, a remap UI and the same bit rules as 0x8040 | `web/input.js` | W2-08 | P1 | S | — | password entry and TRAINING by keyboard |
| W5-02 | Gamepad API mapping (standard layout) | `web/input.js` | W5-01 | P1 | S | — | level played with a pad |
| W5-03 | Config blob persistence (IndexedDB) and `pagehide` write | `web/storage.js`, `runtime/host/config.c` | W1-06 | P1 | S | — | options survive a reload |
| W5-04 | Resolve I-02 (saves) and implement the chosen behaviour | `runtime/cpu/k32.c`, `web/storage.js` | W2-05 | P2 | S | R-10 | behaviour equals the Windows original |
| W5-05 | Optional touch overlay | `web/touch.js` | W5-01 | P2 | S | — | playable on a tablet |

## W6 — Shell

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W6-01 | Import flow: pick files, SHA-256 check against `FACTS.md`, copy to OPFS, in-memory fallback | `web/import.js`, `web/storage.js` | W2-08 | P0 | S | — | a clean profile imports and boots |
| W6-02 | Launcher: Play gesture → AudioContext + fullscreen; settings page mirroring `hercules_display.ini` | `web/index.html`, `web/settings.js` | W6-01 | P0 | S | — | fullscreen 4:3 with letterbox |
| W6-03 | Browser input quirks: Escape, keyboard lock where available, pointer hiding, focus loss → active flag | `web/herc.js` | W6-02 | P1 | S | R-11 | pause and resume behave like the original's inactive state |
| W6-04 | Error screen for fatal traps and unknown opcodes (module + RVA shown) | `web/herc.js` | W2-05 | P1 | S | — | forced fatal shows the screen |
| W6-05 | Cadence check: presents/s and ticks/s overlay in dev mode | `web/dev.js` | W6-02 | P2 | S | R-15 | 30 ticks/s at 60 and 144 Hz displays |
| W6-06 | Optional PWA app shell (no game data in the cache) | `web/sw.js`, `web/manifest.webmanifest` | W6-02 | P2 | S | — | the app loads offline after first use |

## W7 — Build and release

| ID | objective / instructions | files | deps | P | size | risk | done when |
|---|---|---|---|---|---|---|---|
| W7-01 | Release and debug build profiles, source maps, `ASSERTIONS` in debug | `runtime/web/build.ps1` | W2-08 | P1 | S | — | both profiles build |
| W7-02 | Memory sizing: fixed heap, peak measured per level | `runtime/web/build.ps1` | W3-07 | P1 | S | R-14 | peak recorded; no growth needed |
| W7-03 | Dev and LAN server with correct MIME types and no COOP/COEP | new `tools/web/serve.py` | W6-01 | P1 | S | — | the game runs from another LAN machine |
| W7-04 | Web package: app only, plus a scan proving no original bytes or derived art are inside | `tools/package/build_web.py` | W7-01 | P1 | S | R-13 | the scan passes |
| W7-05 | Update `docs/wasm/*` with measured numbers and close the open questions | `docs/wasm/` | all | P2 | S | — | docs current |
