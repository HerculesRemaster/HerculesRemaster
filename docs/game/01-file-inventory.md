# 01. File Inventory

Root: `C:\Users\Thanh\Downloads\GameHercules` (not a git repository; 18 entries including `docs/`).

## Game files

| File | Size (bytes) | Type | Role |
|------|-------------:|------|------|
| `HERCULES.EXE` | 275,968 | PE32, i386, built 1997-08-20 | Main executable: window, menus, DirectDraw/DirectSound setup, loads engine DLLs, reads `hercules.fs` |
| `ENG1.DLL` | 481,280 | PE32 DLL, built 1997-08-20 | Gameplay engine #1 (largest; hero, enemies, physics, throw/die/unit logic) |
| `ENG3.DLL` | 272,896 | PE32 DLL, built 1997-08-20 | Gameplay engine #3 (smaller sibling of ENG1) |
| `TITLE.DLL` | 180,736 | PE32 DLL, built 1997-08-20 | Title screen, menus, "tally" (score) screen, demo mode |
| `HERCULES.FS` | 46,924,407 | Custom packed archive | All game data: 1,024 entries (see doc 04) |
| `HERCULES.HLP` | 46,876 | Windows Help (WinHelp) | Customer-support / system-requirement help (not gameplay help) |
| `DISNEY.INI` | 2,256 | INI text | Installer-generated platform and DirectX record |
| `HERC.REG` | 664 | Registry export | Registry keys the EXE needs to believe it is "installed" |
| `SETUP.BAT` | 43 | Batch | Runs `regedit HERC.REG` |
| `ReadMe.txt` | 646 | Text | Redistributor's launch notes (see below) |
| `unins000.dat` | 11,804 | Inno Setup uninstall log | Left over from a repack installer, not used by the game |

## Unrelated / non-game files

| File | Size | Notes |
|------|-----:|-------|
| `Icons/cgn-favorites.ico`, `cgn-help.ico`, `cgn-pdf.ico`, `cgn-settings.ico`, `cgn-text.ico` | 161,862 each | Repacker's installer icons. **Not part of the game.** Safe to ignore or delete. |

## ReadMe.txt summary [Verified]

Written by a third-party download site, not Disney. It says:
1. Run `SETUP.BAT`.
2. Run `HERCULES.EXE`. If it says "Hercules is not installed", run `HERC.REG` and retry.
3. Video problems: Alt+Enter or the Windows key toggles fullscreen/windowed; use the Display menu for 640x480.
4. **Music is not included; sound effects should work.**

It also contains a download-site URL and password. Do not carry these into any derived project.

## Binary facts [Verified]

- All four PE files are 32-bit x86 (`Machine 0x14C`), 5 sections each (`.text .rdata .data .idata .reloc`), linked 1997-08-20 with Microsoft Visual C++ (MSVC runtime strings: "R6xxx", "unexpected heap error").
- The DLLs have relocation tables (`.reloc`), so they are relocatable and load at arbitrary addresses.
- Debug path strings left in the binaries reveal the original build layout:
  - `m:\pc\eng1\release\eng1.dll`, `m:\pc\eng3\release\eng3.dll`, `m:\pc\title\release\title.dll`
  - `P:\SRC1\HERO\HERCULES`, `P:\SRC1\HERO\HERCULES\HC_DIE`, `\HC_DUNIT`, `\HC_THROW`, `P:\SRC3\HERO\HERCULES`
  - These hint at module names: hero "Hercules" with states die / dunit (death unit?) / throw.
- Directory layout when installed: `C:\Hercules\` (see doc 03).

## What is missing

- Source code, headers, debug symbols (`.pdb`, `.map`): none.
- Music tracks (audio CD tracks or separate files): absent per the ReadMe.
- Video/FMV files: none seen outside `HERCULES.FS` (not yet checked inside it).
- Documentation of any file format: none.
