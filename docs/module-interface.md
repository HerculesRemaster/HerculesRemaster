# EXE ↔ DLL module interface

Source: Ghidra decompilation of HERCULES.EXE (base 0x400000) and ENG1/ENG3/TITLE.DLL (base 0x10000000). Full slot dump with per-DLL users and decompile hints: `analysis/exports/p2_contract.json` (made by `tools/ghidra/p2_contract.py` + `p2_hints.py`).

**Summary.** The DLLs are the game code (PlayStation-style: PCopen/PCread files, InitHeap/Malloc heaps, VAB/Sonix sound, an ordering table of GPU primitives). The EXE is the platform layer. It owns the window, clock, input, sound, CD audio, archive and the 320×240 8-bit framebuffer, and it exposes them through one table of 244 callbacks. Each `PC_DLLEngineMain` call runs **one game tick**.

## Entry point

`void __cdecl PC_DLLEngineMain(int *table, uint flags)`. It is the only export (ordinal 1) of each DLL.

| DLL | export RVA | table copy | heap-init thunk | per-tick input thunk | tick function |
|---|---|---|---|---|---|
| ENG1.DLL | 0x2c180 | 0x70f20 | 0x2c330 | 0x2c880 | 0x20300 |
| ENG3.DLL | 0x1f830 | 0x407a0 | 0x1f9c0 | 0x1fec0 | 0x1abb0 |
| TITLE.DLL | 0xc1f0 | 0x2bb40 | 0xc390 | 0xca00 | 0x5fb0 |

On the first call (a DLL-global "first" flag), the DLL:

1. Copies 250 dwords (0x3e8 bytes) from `table` into its own copy and returns if `copy[0] != 0x5ac00cac`. A 250-iteration loop meant to reject null slots tests only `table[0]`.
2. Calls slot 0x50 with `(table[2], table[3])` (heap base and size), then clears the flag.

On every call it then:

1. calls slot 0x258 (input),
2. stores `flags & 1` at `gameState + 0x40`,
3. runs one tick.

`flags & 1` = "catch-up tick, don't render". The EXE sets it in `RT_Tick` (below), and the EXE's `DrawOTag` (slot 0x164) skips rasterising and presenting while it is set.

## EXE side

| EXE RVA | name | role |
|---|---|---|
| 0x5cd0 | `MOD_RequestModule(dir, name)` | `sprintf("%s%s")`, `_splitpath`. Basename `TITLE` / `ENGINE1` / `ENGINE3` selects `m:\pc\title\release\title.dll` / `…\eng1\…\eng1.dll` / `…\eng3\…\eng3.dll`. If a module is running (`g_modRunning` 0x91c464), sets `g_modSwitch` (0x91c460) and `longjmp(g_modJmp 0x91bfd8)`; otherwise loads it at once. |
| 0x5c20 | `MOD_LoadModule` | `FreeLibrary` of the old one. `LoadLibraryA` takes the path after the last `\` (`title.dll`, `eng1.dll`, `eng3.dll`, searched in the normal DLL order), then `GetProcAddress("PC_DLLEngineMain")` into `g_modEntry` (0x91c45c). Fatal `"%s"` on failure. |
| 0x5ec0 | `MOD_RunTick` | `g_modRunning = 1`, `setjmp(g_modJmp)`; if a switch is pending → `MOD_LoadModule`; `g_modEntry(&g_modTable, g_skipRender)`; `g_modRunning = 0`. |
| 0x4040 | `MOD_ChangeModule` (slot 0x64) | Resets EXE-side state (frees the block named at `gameState+0xe4`, heap reset slot 0x44, clears `gameState+0xb0/+0xb4`, sets `gameState+0x108 = 0x801ffff0`, a PSX stack-top address), then `MOD_RequestModule`. Also called at startup from 0x1ff0. |
| 0x4365a0 | `g_modTablePtr` | → 0x4361b8 (`g_modTable`) |

A level/module change is therefore a `longjmp` out of the running DLL's tick back into `MOD_RunTick`. That tick's DLL stack frames are discarded, the new DLL is loaded and its first tick runs in the same frame.

## Table layout (EXE 0x4361b8, 0x3e8 bytes)

| offset | value | meaning |
|---|---|---|
| 0x000 | 0x5ac00cac | magic, checked by every DLL |
| 0x004 | 0x615c98 | shared game-state struct (EXE `.data`). Known fields: +0x40 skip-render flag, +0x48/+0x4c random state, +0xb0/+0xb4, +0xe4 block id, +0x108 |
| 0x008 | 0x6177d0 | game heap base (EXE `.data`) |
| 0x00c | 0x300000 | game heap size (3 MiB) |
| 0x010 | 0xba25c0 | EXE data, used by many DLL functions; meaning open |
| 0x014 | 0x437480 | EXE data; meaning open |
| 0x018–0x3e4 | 244 code pointers | callbacks below; many point at stubs |

Stubs: 0x40be30 = `ret`, 0x40a340 = `return 0`, 0x40bd40 = `return 1`. Used slots (any DLL references the slot in its table copy): ENG1 122, ENG3 108, TITLE 122.

## Identified callbacks

Names come from the EXE function's debug/format strings, the imports it reaches, or its code. "likely" marks a name taken from shape only.

| slot | EXE RVA | name | evidence |
|---|---|---|---|
| 0x020 | 0x3d90 | file open (debug) | `"Unable to open %s (%d)"` |
| 0x030 | 0x3d30 | file close (debug) | `"Closed"`, `"Failed"` |
| 0x038 | 0x48a0 | `RT_CompactMalloc` | `"CompactMalloc Heap %d (0x%x) Size…"` |
| 0x040 | 0x4750 | `RT_Free` | `"Free id %d (0x%x) Size %d"` |
| 0x044 | 0x47c0 | heap reset | walks heap table 0xba29c0/0xba29d0 and frees |
| 0x050 | 0x4450 | `RT_SetHeapRange(base, size)` | stores 0xba29e0/0xba29e4; called once with table+8/+0xc |
| 0x064 | 0x4040 | `MOD_ChangeModule(dir, name)` | see above |
| 0x068 | 0x3b50 | `RT_LoadFile` | `"Loading %s (0x%x) size %d"`, `"Unable to open %s"`, `"Loaded"` |
| 0x06c | 0x16b0 | `AUD_LoadLevelSonix(name)` | `"LoadLevelSonix setting Music %d Sfx %d"` |
| 0x070 | 0x1110 | `AUD_LoadMusic` (TITLE only) | `"LoadMusic %d %d"`, `MUSIC.MUS`, `"Unable to down load VAB"` |
| 0x074 | 0x1330 | `AUD_LoadSfx` (TITLE only) | `"LoadSfx %d %d"`, `SFX.SFX` |
| 0x078 | 0x4570 | `RT_Malloc` | `"Malloc Heap %d (0x%x) Size %d"`, `"Malloc FAILED…"` |
| 0x080 | 0x3c50 | file open (debug, ENG1) | `"Opening %s"` |
| 0x094 | 0x4260 | `RT_RandMod(n)` | `Rand() % n` (ENG1 calls it from 147 places) |
| 0x098 | 0x4280 | `RT_Rand` | additive generator on gameState+0x48/+0x4c, 16-bit |
| 0x0a0 | 0x4690 | `RT_ReMalloc` | `"ReMalloc Heap %d…"` |
| 0x0a4 | 0x4470 | `RT_ReInitHeap` (TITLE) | `"ReInitheap Heap %d…"` |
| 0x0d4 | 0x17d0 | `AUD_StartMusic` | `"StartMusic 0x%x 0x%x 0x%x"`, `"ERROR Specified vabid is not loaded"` |
| 0x0d8 | 0x18f0 | `AUD_StartMusic` variant (TITLE) | same strings |
| 0x0dc | 0x15d0 | `AUD_StopMusic` | calls CD-audio stop 0x9e10 |
| 0x0e4 | 0x1610 | `AUD_UnloadAllSfxMusic` | `"UnLoadAllSfxMusic"` |
| 0x164 | 0xac60 | `GFX_DrawOTag(ot)` | walks the primitive linked list (code byte at +7), rasterises into the 320×240 DIB, then presents; skipped while the skip-render flag is set |
| 0x190–0x1e4 | 0xb2f0–0xb920 | matrix/vector state (libgte-like) | 16-bit matrix and vector globals at 0xa8e708–0xa8e778 |
| 0x204, 0x20c | 0xa710, 0xa8f0 | image transfer into emulated VRAM | buffer 0x96e6e0; 0xa8f0 is also called by `GFX_DrawOTag` for primitive code 0x01 |
| 0x228 | 0x10460 | `RT_FatalError(msg)` | `MessageBox "Hercules Error"`, exit |
| 0x22c | 0xf3d0 | `AUD_CreateSfx` | `FS_Open("m:\language\sonix\pc\%ld.esf")`, `"PC_CreateSFX(1)"` |
| 0x238 | 0x104d0 | `RT_Quit` | `SendNotifyMessageA` to the main window |
| 0x244 | 0x5760 | `GFX_LoadLevelPalette` | opens `o:\<level>\map_psx.bin`-style names, seeks 0x12, reads 0x300 bytes (TGA header + 256 RGB), sets 256 palette entries |
| 0x250 | 0x5450 | message pump / `%d.etv` playback (TITLE) | `PeekMessage`/`DispatchMessage`, `"e:\data\%d.etv"` |
| 0x258 | 0xbcf0 | `RT_PollInput` (per tick) | writes `~buttons` (16-bit, PSX active-low) to the pad buffer at `*(0xa8e78c)+2`, byte 1 = 0x40 |
| 0x260 | 0xa350 | `RT_PCclose` | → `FS_Close` |
| 0x26c | 0xa280 | `RT_PClseek` | → `FS_Seek` + `FS_Tell` |
| 0x270 | 0xa210 | `RT_PCopen(name, mode)` | → `FS_Open`; at most 16 open files, mode must be 0 |
| 0x274 | 0xa2f0 | `RT_PCread` | → `FS_Read`, `"PCread(1)"` |
| 0x280 | 0xa4a0 | likely `PutDrawEnv` | copies 0x5c bytes (PSX `DRAWENV` size) to 0x96e680 |
| 0x3e0 | 0xac10 | frame counter | returns and increments 0xa8e700 |

The other used slots are listed in `p2_contract.json`. They are mostly sprite and VRAM helpers (0x108–0x1e4, 0x28c–0x2bc) and sound-channel helpers (0x300–0x3a8, globals 0xa8e790/0xa8e7d0).

## Allocators

One 3 MiB arena (EXE 0x6177d0) is split into numbered heaps (`RT_InitHeap` 0x4510, tables 0xba29c0 start / 0xba29d0 end). Block header: the first dword is `(size/4 - 2) | 0x80000000`, and free words are set to 1. DLLs allocate only through slots 0x78/0x40/0xa0/0x38/0x44. Pairs: `RT_Malloc`↔`RT_Free`, `RT_ReMalloc`, `RT_CompactMalloc`, `RT_ReInitHeap`/heap reset on module change. The DLLs never call the CRT `malloc` for game data. They import only KERNEL32.
