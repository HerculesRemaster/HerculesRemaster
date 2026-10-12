# WASM port — risks and open questions

Severity: **H** blocks the port or fidelity, **M** costs weeks, **L** is local. Likelihood: H/M/L.
Every H risk names the experiment that settles it and the backlog task that runs it.

## 1. Risks

| id | risk | sev | lik | mitigation / experiment | task |
|---|---|:-:|:-:|---|---|
| R-01 | interpreter too slow for 30 Hz in the browser (dynamic instruction count unknown) | H | M | measure guest instr/tick natively in W2a, then ms/tick in Chrome and Firefox (M1). Optimise: decoded cache, lazy flags, REP fast paths. If p95 > 8 ms → static recompilation with interpreter fallback | W2-08, W2-09 |
| R-02 | hidden host→guest re-entrancy beyond the known callbacks (gs+0xb0/0xb4, 0xa8e780) | H | M | W1 logs every slot. In W2 the trap dispatcher asserts that no guest code runs while the host is in a non-reentrant slot. Search the decompiles for calls through EXE globals that hold DLL addresses | W1-09, W2-05 |
| R-03 | module switch mid-tick leaves host state inconsistent (open files, voices, VRAM) | H | M | follow `MOD_ChangeModule`'s reset exactly (`p2_decomp/00404040.c`); diff W1 and W2 checksums across every switch in the demo cycle | W1-07, W2-06 |
| R-04 | an unidentified EXE side effect that DLLs observe (e.g. `GFX_DrawOTag` writes back into the OT, frame-counter semantics, slot 0x3e0) | H | M | W1 is the experiment: with the real CPU, any missing side effect shows up as a divergence from `hero_state.csv` or a hang | W1-10 |
| R-05 | x87 paths reached during gameplay (ENG3 CRT `__ftol`, `__fpmath`) | M | L | count executions per x87 opcode in W2a over all levels. Emulate with IEEE double; switch to softfloat only if results differ | W2-04 |
| R-06 | WebGL2 integer targets and MRT differ from D3D11 (R16UI outputs, copy of the destination box) | M | M | shader port spike on one TITLE frame before porting the full OT walker | W3-01 |
| R-07 | 4K art memory: 3.3 GB RGBA / 0.83 GB R8 at 9× | H | H | per-level bundles, scale variants (3/4/6/9×), LRU budget; measure GPU memory in Chrome | W3-06, W3-07 |
| R-08 | music: no source media in the project | M | H | user-supplied track; without it the game plays silent music exactly like a PC without the CD (the EXE's own behaviour) | W4-05 |
| R-09 | CD-presence checks (`RT_Startup` 0x4ac0, `RT_ReadPadBits` flag 0x43a594) | L | L | EXE-side only. The flag is static non-zero in this edition and the portable build runs without a disc (`P8-expand-package.md` notes), so the host simply omits the check | W1-06 |
| R-10 | save path through CRT `CreateFileA("bu00:…")` | M | M | trace on Windows what CreateFileA returns for that name (Process Monitor or a hook). Copy that behaviour by default | W5-04 |
| R-11 | browser input limits: Escape exits fullscreen; keyboard lock exists only in Chromium; the gamepad needs a button press to connect | L | H | map the game's pause and back actions away from Escape by default; show the controller state in the shell | W6-03 |
| R-12 | the emscripten toolchain is a third-party download (project rule 7) | M | M | resolved 2026-10-13: user approved D-W0-2; pinned SDK 6.0.12 installed outside the game folder ([toolchain.md](toolchain.md)) | W0-02 |
| R-13 | legal: hosting anything derived from the game | H | H | local/LAN only; user-supplied originals; art bundles built locally; public deploy blocked (W0) | W0-01 |
| R-14 | WASM memory pressure: guest 64 MB + FS 45 MB + audio + art | M | L | fixed 256 MB heap first; move FS to OPFS sync reads in a worker if needed | W7-02 |
| R-15 | timing drift: rAF jitter vs the 33 ms accumulator | L | M | identical clamp and catch-up rules; frame cadence measured with `tools/runtime/frame_cadence.py` logic in the shell (presents/ticks per second) | W6-05 |
| R-16 | AudioContext autoplay policy; latency of AudioBufferSource starts | L | M | create or resume only on Play; schedule voice starts at `currentTime` (no look-ahead needed at 30 Hz) | W4-06 |

## 2. Open questions (from the audit)

| id | question | why it matters | how to answer |
|---|---|---|---|
| I-01 | what is table +0x10 (EXE 0xba25c0)? In Ghidra its only reference is the table itself; the DLLs use it heavily (`p2_contract.json` slot 4 users) | the host must allocate it with the right size and initial contents | list DLL accesses `[copy+0x10]` and the offsets used; compare with EXE globals near 0xba25c0 (pad buffer 0xba2a40 and heap tables 0xba29c0 are nearby, hypothesis: a shared PSX-style work area) |
| I-02 | do saves persist on the PC (TITLE CRT `_open` on `bu00:B-sces-00891…`)? | decides KERNEL32 file semantics and OPFS saves | Process Monitor during save/load in the original; or a hook on CreateFileA in `hercules_hook.c` |
| I-03 | `.etv` FMV format and the effect of missing files | slot 0x250 behaviour | decompile `FUN_00405450` open-fail branch; FMV ids from `docs/full-game/gameplay/progression.md` |
| I-04 | the full roles of audio slots 0x300–0x3a8 and voice functions 0xf710–0xfbf0 | exact SFX behaviour (volume, pan, pitch, looping) | read the decompiles; compare with `sfx-events.json` |
| I-05 | is gs+0x108 = 0x801ffff0 ever dereferenced? | guest memory layout (it would need a mapping at 0x801ff…) | the guard trap in W2a reports it; watch it under W1 |
| I-06 | are all 244 slots cdecl, and do any return 64-bit values? | trap ABI | check caller `add esp,n` patterns in W1, by slot |
| I-07 | do ENG3's x87 instructions run during gameplay? | x87 accuracy needs | W2a execution counters |
| I-08 | the flat-fill rasteriser 0x40c2e0 (`GFX_PolyFlat`) hasn't been read | renderer correctness for flat primitives | read `rast_*` neighbours; ENG3 scenes show no flat codes (`docs/gpu-renderer.md`) |
| I-09 | demo length 881 vs 884 ticks; 8 vs 9 demo files | demo gate | read ENG1 0x6005 bound against the header size |
| I-10 | which CRT behaviour does `GetVersion` select (NT vs Win9x branches) | KERNEL32 stub values | step DllMain in W1 with a breakpoint on the GetVersion caller |
| I-11 | the `FUN_00410a30(0x13546547)` call at boot (CRT `srand`?) | whether CRT `rand` feeds game logic | decompile 0x410a30; search the DLLs for CRT rand use (none expected: the DLLs use slot 0x098) |
| I-12 | the ENG3 renderer's remaining 12–44 % difference (span-start UV sampling) | W3 gate is relative to D3D11, so it doesn't block | keep the existing open item in `docs/gpu-renderer.md` |

## 3. Assumptions

- The user's HERCULES.EXE is available for the sine table and hash checks (it is never executed).
- A desktop browser with WebGL2 and OPFS: current Chrome, Edge or Firefox. Safari and mobile are a later milestone.
- The game's 30 Hz logic and 4:3 view are kept. No widescreen and no 60 Hz logic.
- Route A on Windows (`runtime/display/`) remains the reference implementation and keeps shipping.
