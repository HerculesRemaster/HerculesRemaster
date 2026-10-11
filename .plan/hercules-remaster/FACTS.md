# Facts (plan-time probes, 2026-10-11)

## Originals (`C:\Games\GameHercules`, copied to `backups/originals/`)

| file | bytes | sha256 |
|---|---:|---|
| HERCULES.EXE | 275,968 | b3ab3ee9e5c973bb245d8eed795e8e7065d7dfc371e412e602b2059af894a551 |
| ENG1.DLL | 481,280 | 01b78ae814892007bb6031b2470f10e09bdd912fdb8d30de9f4cfea7c638f757 |
| ENG3.DLL | 272,896 | aefa1147deae55c4cb2290282f5142df00baa49a9cd59f671f7a3ce5b8b7fca7 |
| TITLE.DLL | 180,736 | b17a37fb1bd5002c54cedb6dba1509bc1ca2d8483fa9724d8e4ade04fe79188b |
| HERCULES.FS | 46,924,407 | affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4 |

There is no disc image and there are no CD-audio tracks. Music isn't included.

## PE

- All x86 (0x14C), linked 1997-08-20.
- **HERCULES.EXE:** base 0x400000, entry RVA 0x11ea0, MFC-style, no `.reloc`. `.data` has a virtual size of 7.8 MB (large static buffers).
  - Imports: DDRAW `DirectDrawCreate`, DSOUND `DirectSoundCreate`.
  - WINMM: `joyGetPos(Ex)`, `joyGetDevCapsA`, `mciSendStringA`, `timeBeginPeriod/EndPeriod`, `timeGetTime`.
  - Also ADVAPI32 registry calls. No DirectInput.
- **ENG1/ENG3/TITLE.DLL:** base 0x10000000. Each has exactly one export, `PC_DLLEngineMain`, at RVA 0x2c180 / 0x1f830 / 0xc1f0, and imports from KERNEL32 only. The `GetPos`/`GetPosEx` names claimed in older docs are only strings.

## HERCULES.FS

- Index: 1,024 × 12-byte records (`id, offset, size`, little-endian; 0x3000 bytes).
  - 820 records are non-empty and 204 are zero-size.
  - 203 zero-id records form the tail from index 821.
  - No gaps and no overlaps; the payloads end exactly at the file length.
- Magic counts:

  | magic | count | likely format |
  |---|---:|---|
  | `ESF\x03` | 559 | audio |
  | `i1.4` | 109 | image container |
  | `00 01 01 00` | 65 | colour-mapped TGA |
  | `00000000` | 39 | |
  | `00000005` | 10 | |
  | `i1.3` | 8 | image container |
  | `i2.5` | 7 | image container |
  | `i1.a` | 3 | image container |
  | `i1.9` | 1 | image container |
  | other | 19 | |

- Third-party CTPAX tools (`C:\Games\GameHercules\hercstls`):
  - Name hash: `id = Σ toupper(c) << ((i&3)*8) + strlen` over the full path (e.g. `M:\GRAFIX\MAPS\…`).
  - `namelist.c` holds 866 names (SZDD/LZSS) and resolves 804 of the 821 non-zero ids.
  - Extensions: `.ESF` 559, `.BIN` 118, `.TGA` 65, `.SFX` 23, `.CIF` 16, `.MUS` 12, `.DAT` 11 (DEMO*.DAT).
  - `unhercfs` patches TGA byte 0x11 when it extracts. Raw extraction must not.
- ESF format: 16-byte header, then IMA-ADPCM data (mono, high nibble first). File length = 16 + size/4.

  | offset | type | field |
  |---|---|---|
  | 0 | 4 bytes | magic `ESF\x03` |
  | 4 | u32 | PCM byte size |
  | 8 | u16 | sample rate (11025 / 22050) |
  | 10 | u16 | padding |
  | 12 | u8 | loop |
  | 13 | u8 | bps8 |
  | 14 | u16 | padding |

## Tools

| tool | location / notes |
|---|---|
| Ghidra 12.1.4 | `C:\Games\GameHercules\ghidra_12.1.4_PUBLIC` |
| ghidra-mcp 7.0.0 | bethington/ghidra-mcp, source `C:\Games\GameHercules\ghidra-mcp`. GUI plugin installed in `%APPDATA%\ghidra\ghidra_12.1.4_PUBLIC\Extensions\GhidraMCP` (205 endpoints, TCP `127.0.0.1:8089` + UDS). Claude reaches it through the uv bridge `bridge-mcp-ghidra` (`.mcp.json`). Project `C:\Games\Ghidra\HerculesRevertEngineer`. Guide: `..\Ghidra-Headless-MCP_GUIDE.md` (sections 2–3 cover the GUI plugin) |
| Java | 21 |
| x64dbg | `C:\Games\GameHercules\x96dbg\release\x32` (incl. `headless.exe`). Guide: `..\x32dbg_GUIDE.md` |
| Procmon | needs elevation |
| ImHex | `--pl` CLI |
| rtk | Output condenser on PATH (WinGet). The global `PreToolUse` hook `rtk hook claude` covers the **Bash tool only**. Rewrites: `git`, `cat`→`rtk read`, `grep`, `ls`, `find`, `curl`. `.venv/Scripts/python.exe …` and `pwsh -File …` pass through unchanged. Dry-run a rewrite with `rtk hook check "<cmd>"`; see savings with `rtk gain`. |
| others | MSVC 2022 x86 (vcvars32), Python 3.13 + uv, Rust, .NET, ffmpeg, 7z |
| machine | i9-10900K, RTX 3090, 16 GB, 3840x2160 @ 60 Hz |

Game registry: `HKLM\SOFTWARE\WOW6432Node\Disney Interactive\Hercules\1.00` (already imported).

## Display target (P2F)

| item | value |
|---|---|
| monitor | 3840×2160 @ 60 Hz, Win11 (DPI scaling may be 150–200 %) |
| game internal frame | 320×240, 8-bit palettised DIB; window shows it 2× = 640×480 |
| original fullscreen | `SetDisplayMode(320,240,8)`, not a real mode on Win11 |
| target | borderless window on the full monitor, 9× integer = 2880×2160, 480 px pillarbox each side, nearest filter |
| config | `working/game/hercules_display.ini` (keys in P2F) |
| route | own x86 proxy `ddraw.dll` in `working/game` (DPI aware + IAT hooks on StretchDIBits/window calls); third-party wrapper only with user OK |

## Open questions

| # | question | default until answered |
|---|---|---|
| Q-01 | Does the user have the original disc (CD audio)? | treat music as missing. Transport (P2): Red Book CD audio via MCI `play cdaudio from %d to %d` (EXE 0x9c30–0xa120) |
| Q-02 | Authorization scope (art, music, AI processing, distribution)? | user 2026-10-12: full 4K remaster with AI regeneration of all visual assets authorized; gameplay/layouts/timing preserved; no distribution until validated against the written Disney licence -> `docs/authorization-scope.md` |
| Q-03 | Runtime language/engine? | P6 (user, 2026-10-12): **A, hybrid**: original EXE/DLL game logic + an in-process x86 C++ D3D11 4K renderer replacing GFX_DrawOTag/GFX_Present |
| Q-04 | 4K aspect: borders, crop or extended camera? | borders |
| Q-05 | Performance budget? | 60 fps, 16.67 ms p95 at 3840x2160 |
| Q-06 | Art route: AI upscale, hand redraw or mix? | user 2026-10-12: full AI CGI regeneration + true-colour RGBA overrides in the renderer (fog/shade/blend in shaders, indexed fallback); generator: online GPT Image 2.5 Sunburst, run by the user (manual route) -> `docs/authorization-scope.md`, `docs/ai-image-guide.md` |
| Q-07 | Which levels use ENG1 vs ENG3? | P5: ENG1 level table (0x68af0) = ids 0, 1 TRAINING, 3 FOREST, 5 THEBES, 6 HYDRA, 7 MEDUSA, 9 TITANS, 11 VORTEX (i2.5 maps); ENG3 = GAUNTLET, CYCLOPS, TORMENT (i1.a maps) |
| Q-08 | Does the DLL boundary expose sprite-level draws or only a framebuffer? | P2: primitive level. DLL passes a PSX-style ordering table to EXE `GFX_DrawOTag` 0xac60, which rasterises into a 320×240 8-bit DIB |
| Q-09 | Does the game start on Win11 without a DirectDraw wrapper? | answered by P2F |
| Q-10 | Fullscreen route: own proxy `ddraw.dll` or an approved third-party wrapper (cnc-ddraw/dgVoodoo2/DxWnd)? | own proxy; ask only if it stalls (P2F) |
| Q-11 | Fullscreen look: integer 9× with borders, fit, or stretch to 16:9? | integer 9× + borders, switchable in `hercules_display.ini` |
