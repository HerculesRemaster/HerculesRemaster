# P2 Binary map (Ghidra)

Goal: with ghidra-mcp, find the archive reader and ID scheme, recover the EXE ↔ DLL contract and the engine's ownership of loop, input, sound and drawing. Record everything as program + RVA, apply names in Ghidra and export them.
Needs: P0 (ghidra-mcp), P1 (catalog for cross-referencing).
Outputs:
- `docs/binary-map.md`
- `docs/resource-callgraph.md`
- `docs/module-interface.md`
- `analysis/annotations/p2.json`

## Tasks

Token budget: ghidra-mcp output isn't condensed by rtk. Page listings (`offset`/`limit`), filter strings and functions by name, decompile one function at a time, and save bulk listings to `analysis/exports/` via `tools/ghidra/mcp_http.py` instead of pulling them into chat.

- [x] Collect anchors with `search_strings`/`get_xrefs_to`. Strings: `hercules.fs`, `Hercules is not installed`, `PC_DLLEngineMain`, `LoadLevelSonix Music %d Sfx %d`, `Demo Mode`. Imports: `DirectDrawCreate`, `DirectSoundCreate`, `mciSendStringA`, `timeGetTime`, `LoadLibraryA`, `GetProcAddress`.
- [x] Locate the archive path: open, the 0x3000-byte index read, the ID lookup (linear, sorted or hashed) and the payload read. Name them `FS_*`.
- [x] Recover the name → ID computation from the code and compare it with the CTPAX formula (`tools/fs/resource_hash.py`). Write `docs/resource-id-scheme.md`.
- [x] Trace `LoadLibraryA`/`GetProcAddress("PC_DLLEngineMain")` in the EXE. Decompile each DLL's `PC_DLLEngineMain` and recover the argument struct and callback table (`create_struct`), the lifecycle codes and the allocator pairs. Write `docs/module-interface.md`.
- [x] Answer the engine questions with RVAs in `docs/binary-map.md`:
  - who owns the main loop and the clock (`timeGetTime`/`timeBeginPeriod`)
  - who decodes assets and who owns the buffers
  - what crosses the render boundary
  - input (`GetAsyncKeyState`/`joyGetPosEx`)
  - sound (DirectSound/Sonix) and music (`mciSendStringA`)
  - level change and shutdown
- [x] Recover the DirectDraw setup from the COM vtable calls (SetCooperativeLevel, SetDisplayMode, CreateSurface, palette): width, height, bpp and surface layout.
- [x] Export the applied names to `analysis/annotations/p2.json`.

Done when the archive path, the ID scheme and the `PC_DLLEngineMain` contract are documented with RVAs.

## Notes

- Anchors: `tools/ghidra/p2_anchors.py` → `analysis/exports/p2_anchors.json`.
- Archive reader in the EXE: `FS_Init` 0x5020, `FS_Open` 0x5130 (normalize 0x4e10 → hash 0x4fe0 → binary search; index must be strictly ascending), `FS_Read` 0x5210, `FS_Seek` 0x52c0, `FS_Tell` 0x5340, `FS_Close` 0x51f0. One shared `FILE*`.
- ID = CTPAX hash over the normalized path. Drive aliases O:/P:/Q:/R: map to GRAFIX\MAPS, GRAFIX\CHOP and LANGUAGE\SONIX; chars are signed. `resource_hash.py` now has `normalize()`. Still 804/821 resolved. → `docs/resource-id-scheme.md`.
- The DLLs are PSX-style game code. The EXE passes a 250-dword table (EXE 0x4361b8, magic 0x5ac00cac: gameState 0x615c98, 3 MiB arena 0x6177d0, 244 callbacks such as PCopen/PCread, Malloc/Free, Sonix/VAB, DrawOTag). Each `PC_DLLEngineMain(table, skipRender)` call is one tick. Module switch = slot 0x64 → longjmp → reload DLL. → `docs/module-interface.md`.
- EXE owns the clock (fixed 33 ms ≈ 30 Hz, ≤2 catch-up ticks without rendering), input (keyboard/joystick → PSX pad bits), DirectSound SFX (ESF), CD-audio music (MCI) and rendering. The DLL submits a primitive list; the EXE rasterises it into a **320×240 8-bit DIB** and shows it with StretchDIBits (windowed) or `SetDisplayMode(320,240,8)` + Flip (fullscreen). → `docs/binary-map.md`, `docs/resource-callgraph.md`.
- 87 names in `analysis/annotations/p2.json`, applied and saved. ghidra-mcp save-hang pitfall → `docs/ghidra-mcp-notes.md`.
- Next (P3): which level names select ENGINE1 vs ENGINE3 (Q-07), the PCopen name list at runtime (17 unresolved ids), the meaning of table+0x10/+0x14.

