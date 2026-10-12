# WASM port — platform compatibility (250-entry table + KERNEL32)

The EXE hands each DLL a 250-dword table (EXE 0x4361b8): 6 header dwords and **244 code slots**. The new C host
must provide every entry. This page lists all 250 entries, generated from `analysis/exports/p2_contract.json`. EXE
names come from the live Ghidra project (2026-10-13), and each entry is classified for the WASM host. The KERNEL32
imports follow in §3.

## 1. Summary

| class | entries | meaning |
|---|---:|---|
| portable | 178 | exact C port of the EXE function from `analysis/exports/p2_decomp/<VA>.c`, operating on guest memory. Includes 80 slots that point at the 3 shared stubs (`RT_StubVoid` 0x40be30 ×39, `RT_StubZero` 0x40a340 ×35, `RT_StubOne` 0x40bd40 ×6) |
| small adaptation | 23 | same logic, but backed by a browser service (archive in memory or OPFS, host renderer state, settings store) |
| browser replacement | 36 | the EXE used Win32/DirectX/MCI. Re-implemented on WebGL2, WebAudio, the Gamepad/keyboard API or the JS shell |
| interpreter integration | 7 | needs CPU-level support: guest stack abort (module switch), host→guest callbacks, game reset |
| header data | 6 | magic and pointers to host-owned guest blocks |
| **total** | **250** | 244 code slots + 6 header dwords |

| area | slots | notes |
|---|---:|---|
| stub | 80 | trivial |
| audio (Sonix banks, SPU-style voices, CD music) | 32 | 0x028/0x0c4, 0x06c–0x074, 0x0ac–0x0b4, 0x0d0–0x0e4, 0x22c/0x230, 0x300–0x3a8 |
| GTE-like fixed-point math | 28 | 0x190–0x1e4, 0x28c–0x294, 0x2b8/0x2bc; globals 0xa8e708–0xa8e778 |
| unused by DLLs | 21 | no DLL references them. The host still installs a logging trap |
| misc leaf helpers | 20 | small functions with no globals (libgpu-like packers, memmove) |
| graphics / VRAM / draw env | 18 | including `GFX_DrawOTag` (0x164) and the VRAM uploads (0x204/0x20c) |
| archive / file | 14 | PCopen/PCread/PClseek/PCclose, `RT_LoadFile`, debug open/close wrappers, stream read |
| heap | 11 | EXE allocator over the 3 MiB arena |
| system | 7 | fatal error, quit, message pump + FMV, re-init, game reset, frame counter |
| input | 6 | pad pointer registration, poll, latch |
| config | 3 | accessors for the 0x48-byte Config blob (0xb9e858) |
| rng | 2 | `RT_Rand`, `RT_RandMod` |
| module | 1 | `MOD_ChangeModule` |
| debug | 1 | GDI `TextOutA` debug text (0x254) |

No slot is left "unknown": every non-stub slot has an exported decompile (165 files). A few roles are still only
named by shape: the audio voice helpers and the 0xba25c0 block. They are tracked in
[risks-and-open-questions.md](risks-and-open-questions.md). The classification was produced by a heuristic over
callees and globals, then corrected by hand for 28 slots after reading their decompiles.

## 2. Slots that shape the host design

| slot | EXE | what the host must do |
|---|---|---|
| +0x000–0x014 | header | magic 0x5ac00cac. gameState, arena (3 MiB) and the +0x10 block are guest allocations owned by the host. +0x14 is the 4096 × s16 Q12 sine table (cos = index − 0x400), used by `FUN_0040b9e0` (slot 0x28c) and `FUN_0040bb00` (slot 0x290). The host copies it from the user's HERCULES.EXE (`.data`, VA 0x437480) |
| 0x050 | `RT_SetHeapRange` | called by each DLL on its first tick with table +8/+0xc |
| 0x064 | `MOD_ChangeModule` | EXE-side reset (`p2_decomp/00404040.c`), then abort the guest run and switch the DLL in the same tick ([x86-execution-strategy.md §3.5](x86-execution-strategy.md#35-module-switch-without-longjmp)) |
| 0x02c | stream read | `FUN_00403f00`: reads 5 × 0x800 bytes and calls the DLL callbacks at gs+0xb0/0xb4 → `cpu_call` |
| 0x140, 0x168, 0x204, 0x20c | DrawSync / VRAM | reach `FUN_0040bd80`, which calls the guest callback at EXE 0xa8e780 when 0xa8e788 ≠ 0. That callback is registered by slot 0x284 |
| 0x094 / 0x098 | `RT_RandMod` / `RT_Rand` | exact port. The state stays in guest gameState (+0x48/+0x4c), seeded 0x13546547/0xecab9ab8 by the host's boot (port of `FUN_00401ff0`) |
| 0x164 | `GFX_DrawOTag` | the WebGL2 renderer walks the OT in guest memory. It skips while skipRender is set, and still runs the EXE's side effects the DLLs can observe (the DrawSync callback, flags) |
| 0x258 | `RT_PollInput` | writes the active-low pad word to `*(registered pad ptr)+2`, byte 1 = 0x40 |
| 0x260–0x274 | PC file API | 16 handles, mode 0, names normalised and hashed like `FS_NormalizePath`/`FS_HashName` (reference `tools/fs/resource_hash.py`) |
| 0x22c | `AUD_CreateSfx` | opens `m:\language\sonix\pc\%ld.esf`, decodes IMA-ADPCM (`AUD_DecodeAdpcm` 0x2410, reference `tools/decode/esf.py`), creates a WebAudio buffer |
| 0x250 | message pump / FMV | TITLE only. No `e:\data\%d.etv` files exist. Reproduce the EXE's missing-file path, not a fake video |
| 0x058, 0x25c | re-init, game reset | not referenced by the DLLs. Host-internal boot and reset paths |

## 3. KERNEL32 imports

55 distinct functions (ENG1 52, ENG3 52, TITLE 46; pefile). Every reference comes from CRT code at the top of each
`.text`. They are `__stdcall`, so the host trap pops the arguments.

| function | E1 | E3 | T | host behaviour |
|---|:-:|:-:|:-:|---|
| CloseHandle | x | x | x | close host handle; TRUE |
| CreateFileA |  |  | x | TITLE save path only, see I-02 (default: fail like Windows on `bu00:` names, INVALID_HANDLE_VALUE) |
| DeleteCriticalSection | x | x | x | no-op |
| EnterCriticalSection | x | x | x | no-op |
| ExitProcess | x | x | x | fatal host stop with exit code |
| FlushFileBuffers | x | x | x | TRUE |
| FreeEnvironmentStringsA | x | x | x | TRUE |
| FreeEnvironmentStringsW | x | x | x | TRUE |
| GetACP | x | x | x | 1252 |
| GetCPInfo | x | x | x | cp1252 info (MaxCharSize 1) |
| GetCommandLineA | x | x | x | guest string `"herc"` |
| GetCurrentProcess | x | x | x | pseudo handle -1 |
| GetCurrentThreadId | x | x | x | constant 1 |
| GetEnvironmentStrings | x | x | x | pointer to an empty block in guest memory |
| GetEnvironmentStringsW | x | x | x | pointer to an empty wide block |
| GetFileType | x | x | x | FILE_TYPE_CHAR for std handles, else FILE_TYPE_UNKNOWN |
| GetLastError | x | x | x | host last-error variable |
| GetLocaleInfoA | x | x |  | fixed en-US answers or 0 |
| GetLocaleInfoW | x | x |  | fixed en-US answers or 0 |
| GetModuleFileNameA | x | x | x | `C:\HERCULES\<module>` |
| GetModuleHandleA | x | x | x | NULL -> fake EXE handle; DLL name -> guest image base |
| GetOEMCP | x | x | x | 437 |
| GetProcAddress | x | x | x | NULL (log the name) |
| GetStartupInfoA | x | x | x | zeroed STARTUPINFO with cb set |
| GetStdHandle | x | x | x | fake handles for in/out/err |
| GetStringTypeA | x | x |  | ASCII ctype table |
| GetStringTypeW | x | x |  | ASCII ctype table |
| GetVersion | x | x | x | a Win95-style value (hypothesis; confirm the CRT branch in W1) |
| HeapAlloc | x | x | x | guest CRT heap |
| HeapCreate | x | x | x | heap id |
| HeapDestroy | x | x | x | TRUE |
| HeapFree | x | x | x | guest CRT heap free |
| InitializeCriticalSection | x | x | x | no-op |
| InterlockedDecrement | x | x |  | plain decrement (single thread) |
| InterlockedIncrement | x | x |  | plain increment |
| LCMapStringA | x | x |  | ASCII upper/lower |
| LCMapStringW | x | x |  | ASCII upper/lower |
| LeaveCriticalSection | x | x | x | no-op |
| LoadLibraryA | x | x | x | NULL (log the name; hypothesis: CRT lazy MessageBoxA) |
| MultiByteToWideChar | x | x |  | byte -> u16 widening |
| ReadFile |  |  | x | TITLE save path (I-02) |
| SetEndOfFile |  |  | x | TITLE save path (I-02) |
| SetFilePointer | x | x | x | host file seek; std handles -> 0 |
| SetHandleCount | x | x | x | return the argument |
| SetLastError | x | x | x | store host last-error |
| SetStdHandle | x | x | x | TRUE |
| TerminateProcess | x | x | x | fatal host stop |
| TlsAlloc | x | x | x | index 0..63 |
| TlsFree | x | x | x | TRUE |
| TlsGetValue | x | x | x | slot array |
| TlsSetValue | x | x | x | slot array |
| VirtualAlloc | x | x | x | guest CRT region (reserve and commit both satisfied) |
| VirtualFree | x | x | x | TRUE |
| WideCharToMultiByte | x | x | x | u16 -> byte narrowing |
| WriteFile | x | x | x | std handles -> console log; files -> I-02 |

## 4. Full table

Columns:
- **#**: dword index.
- **off**: byte offset in the table.
- **EXE RVA**: the target function (RVA from 0x400000).
- **DLL users**: DLLs whose code, other than `PC_DLLEngineMain`, references the slot (E1 = ENG1, E3 = ENG3, T = TITLE).
- **class / difficulty / note**: as in §1.

Every non-stub target has a decompile at `analysis/exports/p2_decomp/00<VA>.c`. The table is regenerated from `p2_contract.json` by a one-off script; re-run the classification if the contract changes.

| # | off | EXE RVA | EXE name | bytes | DLL users | area | class | difficulty | WASM implementation note |
|---:|---|---|---|---:|---|---|---|---|---|
| 0 | 0x000 |  |  |  | - | header | header data | - | magic 0x5ac00cac |
| 1 | 0x004 |  |  |  | E1/E3/T | header | header data | - | gameState ptr (guest alloc) |
| 2 | 0x008 |  |  |  | - | header | header data | - | heap base (guest alloc) |
| 3 | 0x00c |  |  |  | - | header | header data | - | heap size 0x300000 |
| 4 | 0x010 |  |  |  | E1/E3/T | header | header data | - | EXE data 0xba25c0, meaning unknown; host must allocate a zeroed block of the right size (RE) |
| 5 | 0x014 |  |  |  | E1/E3/T | header | header data | - | sine table 4096 x s16 Q12; host copies table from EXE .data at 0x37480 |
| 6 | 0x018 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 7 | 0x01c | 0x00a340 | RT_StubZero | 3 | E1/E3/T | stub | portable | trivial | stub return 0 (shared stub) |
| 8 | 0x020 | 0x003d90 | FUN_00403d90 | 258 | E1/E3/T | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 9 | 0x024 | 0x003ea0 | FUN_00403ea0 | 84 | E1/E3/T | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 10 | 0x028 | 0x001000 | FUN_00401000 | 132 | T | audio | portable | low | music volume ramp on gameState+0x1b04..0x1b1c; calls a void stub (volume set) -> host music gain |
| 11 | 0x02c | 0x003f00 | FUN_00403f00 | 314 | E1/E3/T | fs | interpreter integration | medium | streamed read; calls DLL callbacks gs+0xb0/0xb4 (host->guest call) |
| 12 | 0x030 | 0x003d30 | FUN_00403d30 | 86 | E1/E3/T | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 13 | 0x034 | 0x004890 | FUN_00404890 | 16 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 14 | 0x038 | 0x0048a0 | RT_CompactMalloc | 330 | E1 | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 15 | 0x03c | 0x001d30 | FUN_00401d30 | 42 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 16 | 0x040 | 0x004750 | RT_Free | 102 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 17 | 0x044 | 0x0047c0 | FUN_004047c0 | 99 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 18 | 0x048 | 0x0040d0 | FUN_004040d0 | 168 | E1/E3/T | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 19 | 0x04c | 0x001bd0 | FUN_00401bd0 | 291 | E1/E3/T | gfx | portable | low | libgpu-style display/draw env init (320x240 double buffer); host state only |
| 20 | 0x050 | 0x004450 | RT_SetHeapRange | 20 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 21 | 0x054 | 0x004510 | RT_InitHeap | 86 | - | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 22 | 0x058 | 0x010690 | FUN_00410690 | 193 | - | system | interpreter integration | medium | EXE re-init (RT_InitHeap and friends); not referenced by DLLs (host-internal) |
| 23 | 0x05c | 0x010760 | FUN_00410760 | 85 | E1/E3/T | system | portable | low | gameState field reset; exact port |
| 24 | 0x060 | 0x001d00 | FUN_00401d00 | 43 | E1/E3/T | system | portable | low | resets frame counter/pad state; exact port |
| 25 | 0x064 | 0x004040 | MOD_ChangeModule | 143 | E1/E3/T | module | interpreter integration | high | abort guest run, unload, load next DLL, same host tick |
| 26 | 0x068 | 0x003b50 | RT_LoadFile | 256 | E1/E3/T | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 27 | 0x06c | 0x0016b0 | AUD_LoadLevelSonix | 283 | E1/E3 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 28 | 0x070 | 0x001110 | AUD_LoadMusic | 543 | T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 29 | 0x074 | 0x001330 | AUD_LoadSfx | 659 | T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 30 | 0x078 | 0x004570 | RT_Malloc | 285 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 31 | 0x07c | 0x0043a0 | FUN_004043a0 | 176 | E1 | misc | portable | low | memmove (overlap-safe dword copy); exact port on guest memory |
| 32 | 0x080 | 0x003c50 | FUN_00403c50 | 130 | E1 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 33 | 0x084 | 0x0041a0 | FUN_004041a0 | 151 | E1/E3/T | input | portable | low | pad latch: ~pad word (0xba2a40 buffer) -> gs+0x5c held, gs+0x5e pressed; exact port |
| 34 | 0x088 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 35 | 0x08c | 0x001d80 | FUN_00401d80 | 72 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 36 | 0x090 | 0x001090 | FUN_00401090 | 31 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 37 | 0x094 | 0x004260 | RT_RandMod | 26 | E1/E3/T | rng | portable | low | exact port; state lives in guest gameState |
| 38 | 0x098 | 0x004280 | RT_Rand | 96 | E1/E3 | rng | portable | low | exact port; state lives in guest gameState |
| 39 | 0x09c | 0x003d00 | FUN_00403d00 | 35 | E1 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 40 | 0x0a0 | 0x004690 | RT_ReMalloc | 184 | E1/E3 | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 41 | 0x0a4 | 0x004470 | RT_ReInitHeap | 154 | T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 42 | 0x0a8 | 0x004830 | FUN_00404830 | 87 | - | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 43 | 0x0ac | 0x001ee0 | FUN_00401ee0 | 83 | T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 44 | 0x0b0 | 0x001dd0 | FUN_00401dd0 | 186 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 45 | 0x0b4 | 0x001e90 | FUN_00401e90 | 78 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 46 | 0x0b8 | 0x001b90 | FUN_00401b90 | 59 | E1/E3/T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 47 | 0x0bc | 0x001b40 | FUN_00401b40 | 68 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 48 | 0x0c0 | 0x003ce0 | FUN_00403ce0 | 32 | E1 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 49 | 0x0c4 | 0x0010b0 | FUN_004010b0 | 35 | T | audio | portable | low | sets music volume pair then 0x028 |
| 50 | 0x0c8 | 0x004300 | FUN_00404300 | 151 | E1/E3/T | gfx | portable | low | SetDefDrawEnv/SetDefDispEnv pair (libgpu); host state |
| 51 | 0x0cc | 0x0042e0 | FUN_004042e0 | 24 | E1/E3/T | gfx | portable | low | VSync-like: frame counter + void stub |
| 52 | 0x0d0 | 0x001a00 | FUN_00401a00 | 312 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 53 | 0x0d4 | 0x0017d0 | AUD_StartMusic | 273 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 54 | 0x0d8 | 0x0018f0 | AUD_StartMusicAlt | 258 | T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 55 | 0x0dc | 0x0015d0 | AUD_StopMusic | 63 | E1/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 56 | 0x0e0 | 0x004240 | FUN_00404240 | 28 | E1/E3/T | gfx | portable | low | PutDrawEnv + PutDispEnv |
| 57 | 0x0e4 | 0x001610 | AUD_UnloadAllSfxMusic | 154 | E1/E3/T | heap | portable | low | port EXE allocator to C over guest memory (exact block layout) |
| 58 | 0x0e8 | 0x001090 | FUN_00401090 | 31 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 59 | 0x0ec | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 60 | 0x0f0 | 0x001f50 | FUN_00401f50 | 11 | E1/E3/T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 61 | 0x0f4 | 0x00be30 | RT_StubVoid | 1 | T | stub | portable | trivial | stub void (shared stub) |
| 62 | 0x0f8 | 0x00bd40 | RT_StubOne | 6 | T | stub | portable | trivial | stub return 1 (shared stub) |
| 63 | 0x0fc | 0x00bd40 | RT_StubOne | 6 | T | stub | portable | trivial | stub return 1 (shared stub) |
| 64 | 0x100 | 0x00bd40 | RT_StubOne | 6 | T | stub | portable | trivial | stub return 1 (shared stub) |
| 65 | 0x104 | 0x00a340 | RT_StubZero | 3 | T | stub | portable | trivial | stub return 0 (shared stub) |
| 66 | 0x108 | 0x00ac50 | FUN_0040ac50 | 15 | E1/E3/T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 67 | 0x10c | 0x00a150 | FUN_0040a150 | 56 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 68 | 0x110 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 69 | 0x114 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 70 | 0x118 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 71 | 0x11c | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 72 | 0x120 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 73 | 0x124 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 74 | 0x128 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 75 | 0x12c | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 76 | 0x130 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 77 | 0x134 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 78 | 0x138 | 0x00a140 | FUN_0040a140 | 6 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 79 | 0x13c | 0x00a190 | FUN_0040a190 | 6 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 80 | 0x140 | 0x00ac00 | FUN_0040ac00 | 8 | E1/E3/T | gfx | interpreter integration | medium | DrawSync-like: FUN_0040bd80 calls the guest callback at 0xa8e780 (set via 0x284) -> host->guest call; returns 0 |
| 81 | 0x144 | 0x00ac20 | FUN_0040ac20 | 32 | E1/E3/T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 82 | 0x148 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 83 | 0x14c | 0x00be10 | FUN_0040be10 | 18 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 84 | 0x150 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 85 | 0x154 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 86 | 0x158 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 87 | 0x15c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 88 | 0x160 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 89 | 0x164 | 0x00ac60 | GFX_DrawOTag | 1676 | E1/E3/T | gfx | browser replacement | high | WebGL2 OT renderer (port of gpu_renderer.cpp) |
| 90 | 0x168 | 0x00ac40 | thunk_FUN_0040bd80 | 5 | T | gfx | interpreter integration | medium | thunk to FUN_0040bd80: may call guest callback 0xa8e780 |
| 91 | 0x16c | 0x00a340 | RT_StubZero | 3 | E1/E3/T | stub | portable | trivial | stub return 0 (shared stub) |
| 92 | 0x170 | 0x00bda0 | FUN_0040bda0 | 16 | E1/E3/T | input | portable | low | pad buffer pointer bookkeeping |
| 93 | 0x174 | 0x00bd40 | RT_StubOne | 6 | - | stub | portable | trivial | stub return 1 (shared stub) |
| 94 | 0x178 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 95 | 0x17c | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 96 | 0x180 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 97 | 0x184 | 0x00bcd0 | FUN_0040bcd0 | 5 | T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 98 | 0x188 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 99 | 0x18c | 0x00bd40 | RT_StubOne | 6 | T | stub | portable | trivial | stub return 1 (shared stub) |
| 100 | 0x190 | 0x00b6b0 | FUN_0040b6b0 | 58 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 101 | 0x194 | 0x00b6f0 | FUN_0040b6f0 | 36 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 102 | 0x198 | 0x00b430 | FUN_0040b430 | 106 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 103 | 0x19c | 0x00b840 | FUN_0040b840 | 35 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 104 | 0x1a0 | 0x00b4b0 | FUN_0040b4b0 | 98 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 105 | 0x1a4 | 0x00b740 | FUN_0040b740 | 202 | E3 | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 106 | 0x1a8 | 0x00b920 | FUN_0040b920 | 78 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 107 | 0x1ac | 0x00b320 | FUN_0040b320 | 271 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 108 | 0x1b0 | 0x00b880 | FUN_0040b880 | 50 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 109 | 0x1b4 | 0x00b8f0 | FUN_0040b8f0 | 33 | E1/E3 | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 110 | 0x1b8 | 0x00b8c0 | FUN_0040b8c0 | 40 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 111 | 0x1bc | 0x00b2f0 | FUN_0040b2f0 | 31 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 112 | 0x1c0 | 0x00b4a0 | FUN_0040b4a0 | 13 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 113 | 0x1c4 | 0x00b810 | FUN_0040b810 | 37 | E1/E3 | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 114 | 0x1c8 | 0x00b720 | FUN_0040b720 | 26 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 115 | 0x1cc | 0x00b610 | FUN_0040b610 | 71 | - | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 116 | 0x1d0 | 0x00b520 | FUN_0040b520 | 71 | - | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 117 | 0x1d4 | 0x00b5c0 | FUN_0040b5c0 | 71 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 118 | 0x1d8 | 0x00b570 | FUN_0040b570 | 71 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 119 | 0x1dc | 0x00b660 | FUN_0040b660 | 71 | E1/E3 | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 120 | 0x1e0 | 0x00b310 | FUN_0040b310 | 13 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 121 | 0x1e4 | 0x00b870 | FUN_0040b870 | 16 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 122 | 0x1e8 | 0x00a3a0 | FUN_0040a3a0 | 20 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 123 | 0x1ec | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 124 | 0x1f0 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 125 | 0x1f4 | 0x00a3f0 | FUN_0040a3f0 | 136 | E3/T | gfx | portable | low | GetTPage-style bit packing |
| 126 | 0x1f8 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 127 | 0x1fc | 0x00bd30 | FUN_0040bd30 | 15 | - | input | portable | low | pad buffer pointer bookkeeping |
| 128 | 0x200 | 0x00a3c0 | FUN_0040a3c0 | 20 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 129 | 0x204 | 0x00a710 | FUN_0040a710 | 469 | E1/E3/T | gfx | small adaptation | low-medium | VRAM/draw-env/display state owned by host renderer |
| 130 | 0x208 | 0x00a640 | FUN_0040a640 | 193 | T | gfx | portable | low | LoadTPage-style: VRAM upload (0x204) + tpage bits |
| 131 | 0x20c | 0x00a8f0 | FUN_0040a8f0 | 771 | E1/E3/T | gfx | small adaptation | low-medium | VRAM/draw-env/display state owned by host renderer |
| 132 | 0x210 | 0x00bce0 | FUN_0040bce0 | 5 | T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 133 | 0x214 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 134 | 0x218 | 0x00bd60 | FUN_0040bd60 | 11 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 135 | 0x21c | 0x00bd50 | RT_ReadPadBits | 5 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 136 | 0x220 | 0x00bd70 | FUN_0040bd70 | 11 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 137 | 0x224 | 0x004b50 | FUN_00404b50 | 100 | T | config | small adaptation | low | Config blob (0xb9e858, 0x48 bytes) accessors -> host settings |
| 138 | 0x228 | 0x010460 | RT_FatalError | 101 | - | system | browser replacement | low | fatal error -> JS error screen, stop loop |
| 139 | 0x22c | 0x00f3d0 | AUD_CreateSfx | 826 | T | audio | browser replacement | medium | ESF load + ADPCM decode -> WebAudio buffer |
| 140 | 0x230 | 0x00f710 | FUN_0040f710 | 80 | T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 141 | 0x234 | 0x005750 | FUN_00405750 | 10 | T | gfx | small adaptation | low-medium | VRAM/draw-env/display state owned by host renderer |
| 142 | 0x238 | 0x0104d0 | RT_Quit | 67 | T | system | browser replacement | low | quit -> return to launcher |
| 143 | 0x23c | 0x0070c0 | FUN_004070c0 | 118 | E1/E3/T | gfx | small adaptation | low | frame brightness state (0x436a0c, 0x91c488) -> renderer uniform |
| 144 | 0x240 | 0x004b10 | FUN_00404b10 | 51 | T | config | small adaptation | low | Config blob (0xb9e858, 0x48 bytes) accessors -> host settings |
| 145 | 0x244 | 0x005760 | GFX_LoadLevelPalette | 673 | E1/E3/T | gfx | small adaptation | low | reads palette from FS; host FS + palette state |
| 146 | 0x248 | 0x004bc0 | FUN_00404bc0 | 32 | T | config | small adaptation | low | Config blob (0xb9e858, 0x48 bytes) accessors -> host settings |
| 147 | 0x24c | 0x007f10 | FUN_00407f10 | 29 | E1/E3/T | gfx | small adaptation | low-medium | VRAM/draw-env/display state owned by host renderer |
| 148 | 0x250 | 0x005450 | FUN_00405450 | 741 | T | system | browser replacement | medium | message pump + e:\data\%d.etv FMV (files missing); replace with no-op/FMV stub |
| 149 | 0x254 | 0x006440 | FUN_00406440 | 236 | - | debug | browser replacement | low | GDI TextOutA debug text; route to console/overlay |
| 150 | 0x258 | 0x00bcf0 | RT_PollInput | 59 | E1/E3/T | input | browser replacement | low | write ~buttons to *(pad ptr)+2 from keyboard/gamepad state |
| 151 | 0x25c | 0x005740 | FUN_00405740 | 15 | - | system | interpreter integration | high | game reset: boot init FUN_00401ff0 -> MOD_ChangeModule(TITLE); not referenced by DLLs (host-internal) |
| 152 | 0x260 | 0x00a350 | RT_PCclose | 74 | E1/E3 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 153 | 0x264 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 154 | 0x268 | 0x00a1a0 | FUN_0040a1a0 | 98 | - | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 155 | 0x26c | 0x00a280 | RT_PClseek | 102 | - | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 156 | 0x270 | 0x00a210 | RT_PCopen | 109 | E1/E3 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 157 | 0x274 | 0x00a2f0 | RT_PCread | 73 | E1/E3 | fs | small adaptation | low | HERCULES.FS in memory/OPFS via host FS |
| 158 | 0x278 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 159 | 0x27c | 0x00a340 | RT_StubZero | 3 | E1/E3/T | stub | portable | trivial | stub return 0 (shared stub) |
| 160 | 0x280 | 0x00a4a0 | FUN_0040a4a0 | 23 | E1/E3/T | gfx | small adaptation | low-medium | VRAM/draw-env/display state owned by host renderer |
| 161 | 0x284 | 0x00bdf0 | FUN_0040bdf0 | 23 | T | gfx | interpreter integration | low | registers guest DrawSync-style callback (0xa8e780/84/88); host stores guest address |
| 162 | 0x288 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 163 | 0x28c | 0x00b9e0 | FUN_0040b9e0 | 280 | E1 | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 164 | 0x290 | 0x00bb00 | FUN_0040bb00 | 300 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 165 | 0x294 | 0x00bc30 | FUN_0040bc30 | 159 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 166 | 0x298 | 0x00a340 | RT_StubZero | 3 | T | stub | portable | trivial | stub return 0 (shared stub) |
| 167 | 0x29c | 0x00a340 | RT_StubZero | 3 | T | stub | portable | trivial | stub return 0 (shared stub) |
| 168 | 0x2a0 | 0x00be30 | RT_StubVoid | 1 | T | stub | portable | trivial | stub void (shared stub) |
| 169 | 0x2a4 | 0x00a5f0 | FUN_0040a5f0 | 24 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 170 | 0x2a8 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 171 | 0x2ac | 0x00a540 | FUN_0040a540 | 23 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 172 | 0x2b0 | 0x00a610 | FUN_0040a610 | 42 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 173 | 0x2b4 | 0x00a4c0 | FUN_0040a4c0 | 128 | T | gfx | portable | low | SetDrawMode-style: builds 0xe1 draw-mode word |
| 174 | 0x2b8 | 0x00b970 | FUN_0040b970 | 20 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 175 | 0x2bc | 0x00b990 | FUN_0040b990 | 10 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 176 | 0x2c0 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 177 | 0x2c4 | 0x00a560 | FUN_0040a560 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 178 | 0x2c8 | 0x00a570 | FUN_0040a570 | 9 | T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 179 | 0x2cc | 0x00a580 | FUN_0040a580 | 9 | E1/E3 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 180 | 0x2d0 | 0x00a590 | FUN_0040a590 | 9 | E1 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 181 | 0x2d4 | 0x00a5a0 | FUN_0040a5a0 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 182 | 0x2d8 | 0x00a5b0 | FUN_0040a5b0 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 183 | 0x2dc | 0x00a5c0 | FUN_0040a5c0 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 184 | 0x2e0 | 0x00a5d0 | FUN_0040a5d0 | 9 | T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 185 | 0x2e4 | 0x00b9a0 | FUN_0040b9a0 | 14 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 186 | 0x2e8 | 0x00a480 | FUN_0040a480 | 29 | E1 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 187 | 0x2ec | 0x00a340 | RT_StubZero | 3 | E1/E3/T | stub | portable | trivial | stub return 0 (shared stub) |
| 188 | 0x2f0 | 0x00a5e0 | FUN_0040a5e0 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 189 | 0x2f4 | 0x00a3e0 | FUN_0040a3e0 | 9 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 190 | 0x2f8 | 0x00b9b0 | FUN_0040b9b0 | 14 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 191 | 0x2fc | 0x00bce0 | FUN_0040bce0 | 5 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 192 | 0x300 | 0x00c0f0 | FUN_0040c0f0 | 14 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 193 | 0x304 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 194 | 0x308 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 195 | 0x30c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 196 | 0x310 | 0x00c0e0 | FUN_0040c0e0 | 11 | E1/E3/T | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 197 | 0x314 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 198 | 0x318 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 199 | 0x31c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 200 | 0x320 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 201 | 0x324 | 0x00be30 | RT_StubVoid | 1 | E1/E3 | stub | portable | trivial | stub void (shared stub) |
| 202 | 0x328 | 0x00c100 | FUN_0040c100 | 56 | - | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 203 | 0x32c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 204 | 0x330 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 205 | 0x334 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 206 | 0x338 | 0x00c250 | AUD_SeqOpen | 38 | - | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 207 | 0x33c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 208 | 0x340 | 0x00c280 | AUD_SeqPlay | 41 | - | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 209 | 0x344 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 210 | 0x348 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 211 | 0x34c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 212 | 0x350 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 213 | 0x354 | 0x00c2b0 | AUD_CdStop | 5 | - | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 214 | 0x358 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 215 | 0x35c | 0x00c200 | FUN_0040c200 | 24 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 216 | 0x360 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 217 | 0x364 | 0x00c2d0 | FUN_0040c2d0 | 16 | - | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 218 | 0x368 | 0x00c220 | FUN_0040c220 | 18 | E1/E3 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 219 | 0x36c | 0x00be30 | RT_StubVoid | 1 | E1/E3 | stub | portable | trivial | stub void (shared stub) |
| 220 | 0x370 | 0x00be30 | RT_StubVoid | 1 | E1/E3/T | stub | portable | trivial | stub void (shared stub) |
| 221 | 0x374 | 0x00c1c0 | FUN_0040c1c0 | 53 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 222 | 0x378 | 0x00be60 | FUN_0040be60 | 85 | E1 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 223 | 0x37c | 0x00bec0 | FUN_0040bec0 | 85 | E1 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 224 | 0x380 | 0x00a340 | RT_StubZero | 3 | E1 | stub | portable | trivial | stub return 0 (shared stub) |
| 225 | 0x384 | 0x00c140 | FUN_0040c140 | 19 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 226 | 0x388 | 0x00bf20 | AUD_SpuPlay | 189 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 227 | 0x38c | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 228 | 0x390 | 0x00c160 | FUN_0040c160 | 91 | E1/E3/T | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 229 | 0x394 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 230 | 0x398 | 0x00c2c0 | FUN_0040c2c0 | 4 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 231 | 0x39c | 0x00bfe0 | FUN_0040bfe0 | 90 | E1 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 232 | 0x3a0 | 0x00c040 | AUD_VabOpenHead | 71 | E1 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 233 | 0x3a4 | 0x00c090 | AUD_VabLoadBody | 73 | E1 | audio | browser replacement | medium | Sonix/SPU voice or CD music -> WebAudio |
| 234 | 0x3a8 | 0x00c240 | FUN_0040c240 | 5 | E1 | misc | portable | low | small leaf, no globals; port from p2_decomp |
| 235 | 0x3ac | 0x00bd40 | RT_StubOne | 6 | - | stub | portable | trivial | stub return 1 (shared stub) |
| 236 | 0x3b0 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 237 | 0x3b4 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 238 | 0x3b8 | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 239 | 0x3bc | 0x00a340 | RT_StubZero | 3 | - | stub | portable | trivial | stub return 0 (shared stub) |
| 240 | 0x3c0 | 0x00bde0 | FUN_0040bde0 | 13 | - | input | portable | low | pad buffer pointer bookkeeping |
| 241 | 0x3c4 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 242 | 0x3c8 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 243 | 0x3cc | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 244 | 0x3d0 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 245 | 0x3d4 | 0x00be30 | RT_StubVoid | 1 | - | stub | portable | trivial | stub void (shared stub) |
| 246 | 0x3d8 | 0x00a340 | RT_StubZero | 3 | T | stub | portable | trivial | stub return 0 (shared stub) |
| 247 | 0x3dc | 0x00b9c0 | FUN_0040b9c0 | 26 | - | unused | portable | low | not referenced by any DLL; host trap logs + mirrors decomp |
| 248 | 0x3e0 | 0x00ac10 | FUN_0040ac10 | 16 | E1/E3/T | gte | portable | low | fixed-point matrix/vector math; exact C port |
| 249 | 0x3e4 | 0x00bdd0 | FUN_0040bdd0 | 16 | E1/E3/T | input | portable | low | pad buffer pointer bookkeeping |
