# WASM port — x86 execution strategy

How the original game logic (ENG1.DLL, ENG3.DLL, TITLE.DLL) runs in a browser. HERCULES.EXE is **not**
executed. Its platform services are re-implemented by the C host (see [platform-compatibility.md](platform-compatibility.md)).

## 1. What the binaries require (measured)

Sources:
- pefile on `backups/originals/*.DLL`;
- Ghidra (live, `search_instructions`, 2026-10-13);
- `analysis/exports/full-game/disasm_ENG{1,3}_DLL.json`.

| property | ENG1 | ENG3 | TITLE | consequence |
|---|---|---|---|---|
| instructions (static) | 87,529 | 55,738 | 35,289 | small code base, about 180k instructions total |
| distinct mnemonics | 69 | 80 | not counted (Ghidra holds it) | 386-class integer core |
| x87 instructions | 0 | 15, all CRT (`__fpmath` 0x1002a280, `__ftol` 0x1002a2e0, `FUN_1002b180`, `FUN_1002ceb0`) | 0 (no FLD/FNSTCW found) | the game logic is integer/fixed-point. A minimal x87 subset is enough |
| MMX/SSE, CPUID, RDTSC, IN/OUT, INT | none | none | none (byte scan) | |
| `fs:` segment / SEH | none | none | none | no exception-frame emulation. The DLLs don't import `RtlUnwind` |
| self-modifying code | none possible | none | none | `.text` is RX and `VirtualProtect` isn't imported. Decoded instructions can be cached |
| TLS directory | no | no | no | the CRT still calls `TlsAlloc`/`TlsGetValue` (stub) |
| `.reloc` | 13,882 | 6,917 | 7,324 | **can be rebased** to any guest address |
| indirect jumps | switch tables `jmp [reg*4+tbl]`, table thunks `jmp [copy+off]` (TITLE: ≥60 seen) | same | same | the interpreter handles them natively. A recompiler needs Ghidra jump-table recovery |
| indirect calls | object handler tables (`call [reg*4+0x1004d6f8]`, `call [eax+edx*8+4]`) | same | same | need a dynamic-dispatch fallback in any recompiler |
| imports | KERNEL32 only, 52 | 52 | 46 | all used only by the statically linked MSVC 5 CRT (§4) |
| absolute EXE addresses in DLL code | none found for gameState 0x615c…, VRAM 0x96e… | none found | | every EXE datum is reached through table pointers. A full scan is backlog W2-03 |

Frequent non-trivial instructions: `REP MOVSD/STOSD/MOVSB/STOSB`, `REPNE SCASB`, `REPE CMPSB`, `MOVSW`, `SETcc`
(102/46), `BT/BTS`, `SHRD` (1), `JECXZ`, `IDIV` (71/33), `IMUL` (388/329), `CDQ`, `LEAVE`.

## 2. Options

| | A. Interpreter in WASM (recommended) | B. Static recompilation x86 → C → WASM | C. Rebuild from decompiled C |
|---|---|---|---|
| what runs | original machine code, decoded at run time | C generated from the original machine code | hand-cleaned Ghidra C |
| fidelity | exact by construction: flags, wrap-around, memory layout | exact if the lifter is exact; it carries the same memory model | **not exact**: ~300 partial functions exist, compiler semantics differ, uninitialised reads, aliasing |
| coverage risk | low: everything the CPU reaches runs | medium: indirect targets missed by static CFG need an interpreter fallback | very high: about 2,600 functions across three DLLs, mostly unnamed |
| performance | the main risk (§6) | 5–20× faster than interpretation (hypothesis; typical for lifted code) | native speed |
| debuggability | best: single-step, trace by module+RVA, compare against native | medium | n/a |
| effort | medium: ~4–6k lines of C | high: lifter, CFG recovery, verification | prohibitive |
| distribution | ships no game code. The user supplies the DLLs | the generated C is derived from the game code, so it **cannot be shipped** under `docs/authorization-scope.md` | same problem |

**Decision:**
- **Primary: A.**
- **Fallback: B**, used only when §6 triggers fire. It reuses A's memory model, traps and host, so the two can run side by side (the interpreter executes the targets B misses).
- **C is rejected.**

Under A and B the legal position is the same as Route A today: the user supplies the original files, and nothing derived from the code is distributed. B's generated C would be derived code, which is a further reason to keep A.

## 3. Interpreter design (A)

### 3.1 Guest memory model

- One contiguous guest block in WASM linear memory. **Guest address = offset into the block**, checked by mask
  or bounds. No page tables.
- The DLLs are **rebased via `.reloc`** to a low guest base. They are never loaded at 0x10000000, which would need
  ≥256 MB of address space. Debug output converts addresses back to `module + RVA`, so traces stay comparable with
  Ghidra and the native host.
- Proposed layout (64 MB block; sizes to confirm in W1):

| range | content |
|---|---|
| 0x0000_0000–0x0000_FFFF | null guard: any access traps with a diagnostic |
| 0x0001_0000 | **trap page**: one 16-byte stub address per host function (244 slots + ≤60 KERNEL32 imports + sentinels) |
| 0x0002_0000 | host-owned data that DLLs reach through table pointers: the table itself, gameState (≥0x1b20 bytes; init zeroes 0x6cd dwords), sine table (8 KB, copied from EXE `.data` RVA 0x37480 out of the user's HERCULES.EXE, or regenerated after a byte check), the 0xba25c0 block (size TBD, W1-04), pad buffers |
| 0x0010_0000 | 3 MiB game arena (table +8/+0xc) |
| 0x0040_0000 | loaded DLL image (SizeOfImage ≤ 0x7e000, reused for each module) |
| 0x0080_0000 | CRT process heap for `HeapAlloc`/`VirtualAlloc` (bump + free list, 8 MB) |
| top − 1 MB | guest stack (ESP starts at the top, 16-byte aligned) |

`gs+0x108 = 0x801ffff0` (a PSX stack-top value) is written by `MOD_ChangeModule`. If a DLL ever dereferences it,
the guard traps and reports it. This is an open question (I-05).

### 3.2 CPU core

- **State:** 8 GPRs, EIP, EFLAGS with lazy evaluation (last op, operands, result), x87 stack of 8 × double plus
  control/status words, and the segment model fixed flat (CS/DS/ES/SS = 0, no FS use).
- **Decoder:** prefix handling (66, F2, F3, 0F), ModRM/SIB, imm8/16/32.
  - A decoded-instruction cache keyed by guest EIP is valid for the life of a module, because there is no SMC.
  - The cache is flushed on module switch.
- **Instruction set:** the union of mnemonics in the three DLLs, generated from the Ghidra dumps (backlog W2-02).
  - Unknown opcodes trap with module+RVA. That is the "unsupported instruction" signal; nothing is silently skipped.
- **x87:** FLD/FST/FSTP/FILD/FISTP/FADD/FSUB/FMUL/FDIV(+R/P)/FCOM(P)/FNSTSW/FNSTCW/FLDCW/FCHS/FABS/FXCH, emulated
  with IEEE double.
  - Every use is in ENG3's CRT, so a per-instruction trace counter shows whether gameplay ever reaches one (I-07).
  - Exact 80-bit softfloat only if a case is found.

### 3.3 Host calls (traps)

- **Platform slots:** the host writes the table copy-source with trap addresses. Each DLL copies the table on its
  first tick.
  - When EIP lands in the trap page, the core calls `host_fn[idx](cpu)`.
  - The host reads cdecl arguments at `[esp+4…]`, writes EAX (and EDX for 64-bit returns), then simulates `ret`.
  - All slots are cdecl. This is a hypothesis to confirm in W1 by checking the caller stack adjustments; backlog W1-03.
- **KERNEL32 imports:** the PE loader fills each DLL's IAT with trap addresses. These are `__stdcall`, so the host
  pops the argument bytes.
- **Data pointers:** table +4 (gameState), +8 (arena), +0x10 (0xba25c0 block) and +0x14 (sine table) are guest
  addresses of host-owned blocks.

### 3.4 Host → guest calls

The host needs `cpu_call(guest_fn, args…)` for:
- DllMain ATTACH/DETACH;
- `PC_DLLEngineMain(table, flags)` once per tick;
- the stream callbacks at gs+0xb0/0xb4 (slot 0x02c);
- the DrawSync-style callback at EXE 0xa8e780 (slots 0x140/0x168/0x204/0x20c via `FUN_0040bd80`).

`cpu_call` pushes the arguments and a **sentinel return address** in the trap page, then runs until EIP reaches the
sentinel. Nesting works because the host C stack mirrors the guest nesting depth, which is shallow.

### 3.5 Module switch without longjmp

Slot 0x064 (`MOD_ChangeModule`) runs the EXE-side reset in C, records the requested module and returns
`HOST_ABORT`. The run loop unwinds every `cpu_call` frame back to the tick driver. The tick driver then:
1. restores ESP to the stack top;
2. calls DllMain(DETACH) on the old DLL;
3. maps the new DLL (rebase, IAT, DllMain ATTACH);
4. calls the new `PC_DLLEngineMain` in the same tick.

This mirrors `MOD_RunTick` 0x5ec0. No setjmp/longjmp is needed in the WASM host, which avoids Emscripten's
setjmp cost and wasm-exceptions requirements.

### 3.6 Tracing and comparison

- Every trap can log `tick, module, caller RVA, slot, args, result`.
- The same log format is produced by `herc_host` in W1, where the real CPU runs the DLLs natively.
- A per-tick checksum of gameState and the arena (FNV-1a) is cheap.

Divergence between interpreter and native is found by diffing the two logs. This is the main correctness tool of
W2. It is a debug feature of the host, not a separate test suite.

## 4. KERNEL32 surface (CRT only)

52 functions in ENG1/ENG3, 46 in TITLE. TITLE drops 9 locale/interlocked functions and adds `CreateFileA`, `ReadFile` and `SetEndOfFile`.
They group as:

| group | functions | host behaviour |
|---|---|---|
| process / startup | GetVersion, GetCommandLineA, GetStartupInfoA, GetModuleHandleA, GetModuleFileNameA, GetEnvironmentStrings(W), FreeEnvironmentStrings(A/W), GetCurrentProcess, GetCurrentThreadId, ExitProcess, TerminateProcess | constant values (Win95-era version, a fake command line `"herc"`, an empty environment). Exit/terminate → fatal host error |
| TLS / sync | TlsAlloc, TlsFree, TlsGetValue, TlsSetValue, InitializeCriticalSection, DeleteCriticalSection, Enter/LeaveCriticalSection, InterlockedIncrement/Decrement, SetLastError, GetLastError | single-thread trivial implementations |
| memory | HeapCreate, HeapDestroy, HeapAlloc, HeapFree, VirtualAlloc, VirtualFree | guest CRT heap region (§3.1). Game data never uses it (the arena does), but CRT stdio buffers might |
| std handles / files | GetStdHandle, GetFileType, SetHandleCount, SetStdHandle, WriteFile, SetFilePointer, FlushFileBuffers, CloseHandle; TITLE: CreateFileA, ReadFile, SetEndOfFile | stdout/stderr → console log. Files: see the save-path decision (open question I-02) |
| locale / strings | GetACP, GetOEMCP, GetCPInfo, MultiByteToWideChar, WideCharToMultiByte, GetStringTypeA/W, LCMapStringA/W, GetLocaleInfoA/W | code page 1252 tables, ASCII-only conversions |
| dynamic linking | LoadLibraryA, GetProcAddress | used by one CRT function per DLL (ENG1 0x100496a0, ENG3 0x1002e810, TITLE 0x1001f5f0). Hypothesis: lazy `MessageBoxA` for CRT fatal errors. Return NULL and log |

Exact per-DLL lists: [platform-compatibility.md §3](platform-compatibility.md#3-kernel32-imports).

## 5. Static recompilation (B), if triggered

1. Export the function boundaries, basic blocks and recovered jump tables from Ghidra (all 2,600 functions).
2. Emit C per function, one `switch` over guest block addresses. Keep the same `cpu` struct, guest memory and lazy
   flags (or flag liveness analysis), and the same traps.
3. Resolve indirect calls and jumps through a table of guest-address → C function. On a miss, fall back to the
   interpreter. Hybrid execution therefore needs no complete CFG.
4. Compare against the interpreter with the §3.6 trace log.

Cost: a lifter of ~3–5k lines. Its output is derived game code, so it is generated locally at build time from the user's DLLs and never distributed. This means a build step in the user's browser or on their machine, which is a further reason to prefer A.

## 6. Performance budget and triggers

- A 30 Hz tick leaves 33.3 ms of wall time per tick. The target is **≤ 8 ms p95 for guest execution per tick** on a mid-range desktop browser, leaving room for rendering, audio and catch-up ticks. The original catch-up runs up to 3 ticks in one frame (2 skip-render + 1).
- The dynamic instruction count per tick is **unknown**. It is measured in W2 with the native build of the interpreter (instruction counter per tick across the attract demo and a heavy level such as HYDRA or CYCLOPS).
- Expectation (hypothesis): a decoded-cache interpreter in WASM sustains about 100–300 M guest instructions/s, which gives about 0.8–2.4 M instructions per 8 ms.

**Triggers for B:**
1. p95 guest time > 8 ms/tick in Chrome or Firefox on the reference desktop after the W2 optimisation pass (decoded-instruction cache, lazy flags, fast paths for `REP MOVS/STOS`, memory access without bounds re-checks inside a block);
2. or the mobile milestone needs it.

## 7. Native-first ladder

| step | executes DLLs with | purpose |
|---|---|---|
| W1 `herc_host` (Win32 x86) | the real CPU via `LoadLibrary` | validates the C platform layer (slots, FS, heap, RNG, renderer state) independently of any emulator |
| W2a `herc_host --interp` (x64 native) | the interpreter, same host code | debug the CPU with native tools; diff traces against W1 |
| W2b emscripten build | the interpreter in WASM | browser performance and integration |
