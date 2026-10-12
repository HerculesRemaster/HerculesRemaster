# Hercules in the browser — WebAssembly migration plan

Status: **implementation requested (2026-10-13); W0 scope and decisions in progress.**
Emscripten 6.0.12 is installed. Local/LAN scope, music and save policies await confirmation;
the checklist is [W0-web-scope.md](../../.plan/hercules-remaster/W0-web-scope.md). No porting code has been written.

Target: the original ENG1/ENG3/TITLE.DLL game logic runs in a browser tab. A C host compiled to WASM provides the
250-entry platform table that HERCULES.EXE used to provide. An x86-32 interpreter runs the DLLs, and a WebGL2 port of
the existing D3D11 renderer keeps the 4K art overrides. Everything runs on the client.

| doc | content |
|---|---|
| [toolchain.md](toolchain.md) | installed Emscripten 6.0.12 pin, PowerShell activation and reproduction |
| [repository-audit.md](repository-audit.md) | what exists: originals, runtime, tools, exports, assets, gaps |
| [current-architecture.md](current-architecture.md) | how the game and today's remaster work |
| [platform-compatibility.md](platform-compatibility.md) | all 250 table entries and 55 KERNEL32 imports, classified |
| [x86-execution-strategy.md](x86-execution-strategy.md) | interpreter vs recompiler vs rewrite; CPU and memory design |
| [target-architecture.md](target-architecture.md) | modules, JS↔WASM API, sequence, timing, WebGL2 notes |
| [asset-migration.md](asset-migration.md) | asset inventory, storage, art bundles, music |
| [migration-roadmap.md](migration-roadmap.md) | W0–W7 phases, gates, critical path, first milestone, go/no-go |
| [risks-and-open-questions.md](risks-and-open-questions.md) | risks with experiments; open RE questions |
| [implementation-backlog.md](implementation-backlog.md) | ≈55 tasks, one per coding agent |

There is no separate testing-strategy document, by user decision (lean project: no test suites or evidence files).
Each phase has a short acceptance gate in the roadmap instead.

## Executive summary

1. **Current state.**
   - Route A ships on Windows: the original EXE plus our `ddraw.dll` proxy with a D3D11 4K renderer, CD-audio emulation and portable registry.
   - Every used format is decoded, 10,568 of 10,571 art entries are packed at 9×, and there are 264 golden frame pairs plus a per-tick hero trace of the attract demo.
   - There is no host, emulator or web code yet (`web/` is empty).
   - Music and FMV media are absent from the originals.
2. **Reuse vs rewrite.**
   - Reused unchanged: the DLLs and HERCULES.FS (raw); the art key scheme and art files; the reference frames and `gpu_diff.py`.
   - Ported: the renderer rules and shaders (HLSL → GLSL ES 3.0); the CD-segment logic and Config handling; the ESF decoder (Python → C).
   - Re-implemented in C from the 165 exported decompiles: every EXE slot.
   - New: the x86 interpreter, the PE loader, the KERNEL32 stubs and the JS shell.
3. **Recommended architecture.**
   - A C host provides the 250-entry table. A hand-written x86-32 interpreter runs the unmodified DLLs from a flat guest memory, with DLLs rebased via `.reloc`, slots and imports as trap addresses, and the module switch as an interpreter abort.
   - Static recompilation is the fallback, only if benchmarks demand it.
   - Rebuilding the game from decompiled C is rejected.
4. **Compatibility.**
   - The 244 code slots are 178 portable (80 are shared stubs), 23 small adaptations, 36 browser replacements and 7 interpreter integrations.
   - The DLLs import only 55 KERNEL32 functions, all used by the MSVC 5 CRT.
   - No SEH, TLS, MMX/SSE or self-modifying code. x87 appears only in ENG3's CRT.
   - The RNG is seeded with constants at boot, so runs are deterministic.
5. **Asset readiness.**
   - Original assets need no conversion: the DLLs read HERCULES.FS themselves.
   - Ready: art (except 3 ENG3 level jobs).
   - Needed: per-level art bundles at several scales to fit browser GPU memory.
   - Missing: music (user-supplied track 2) and `.etv` FMV (reproduce the missing-file path).
6. **Roadmap.**

   | phase | content |
   |---|---|
   | W0 | scope and toolchain approval |
   | W1 | native C host with the real CPU |
   | W2 | interpreter, native then WASM |
   | W3 | WebGL2 renderer |
   | W4 | audio |
   | W5 | input and config |
   | W6 | browser shell |
   | W7 | build and release |

7. **Critical path.** W0 → W1 → W2a → W2b → W3 → W6.
8. **Main risks.**
   - Interpreter speed: measured at M1.
   - 4K art memory: handled by bundles, scale variants and an LRU.
   - Hidden re-entrancy or EXE side effects: W1 exposes them with the real CPU.
   - Legal: local/LAN only.
9. **First milestone (M1).** TITLE runs 1,200 ticks on `herc_host` (real CPU), then under the native interpreter with identical traces, then in Chrome with ms/tick reported.
10. **Go/no-go.**
    - W1 attract demo correct.
    - Interpreter traces equal native.
    - p95 ≤ 8 ms/tick in desktop Chrome and Firefox.
    - WebGL2 frames match D3D11.
    - W0 decisions recorded.
