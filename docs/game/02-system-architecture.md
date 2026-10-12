# 02. Current System Architecture

## Overview

```
 HERCULES.EXE  (host: window, menus, input, DirectDraw, DirectSound, main loop)
    |-- loads --> TITLE.DLL   title screen, menus, score "tally", demo mode
    |-- loads --> ENG1.DLL    gameplay engine (hero logic, enemies, levels set 1)
    |-- loads --> ENG3.DLL    gameplay engine (levels set 3)
    '-- reads --> HERCULES.FS single packed archive of all assets (1,024 entries)

 Windows 95 API layer:  DDRAW.dll  DSOUND.dll  (dinput referenced in INI)  GDI32  USER32  KERNEL32  COMCTL32  comdlg32  ADVAPI32
```

Game genre: 2D side-scrolling action platformer based on Disney's *Hercules* (1997) [Inferred from title, "Restart Level", demo mode, hero state names]. There is no "ENG2": the numbering suggests an engine variant per game area or level group, with ENG2 cut or merged [Unknown].

## HERCULES.EXE [Verified from imports/strings]

- Windows GUI app with menus (Display menu, "About Hercules"), dialogs, a "Restart Level" command, window titles for states: running, "Demo Mode", "Waiting".
- Registry check: on start-up reads `HKLM\SOFTWARE\Disney Interactive\Hercules\1.00`. If missing it shows **"Hercules is not installed."** and quits (this is why `HERC.REG` exists). On 64-bit Windows the 32-bit app reads the `Wow6432Node` path, which is why the `.reg` writes there.
- Other error strings: "Cannot start Hercules from here.", "Hercules Error", "Hercules Notice".
- Opens `hercules.fs` from the working directory (calls `SetCurrentDirectoryA`).
- Graphics: `DirectDrawCreate` (DirectDraw, DirectX 3 era). Cooperative level strings: fullscreen exclusive vs. windowed. This is a 2D surface-blit renderer, not Direct3D (`D3D=0` in `DISNEY.INI`).
- Audio: `DirectSoundCreate`. Debug strings "LoadLevelSonix Music %d Sfx %d" and "ERROR Specified vabid is not loaded" show it uses the **Sonix** sound system (Sony PlayStation-style "VAB" sound banks, i.e. a PS1 port heritage) [Inferred]. Music not included in this copy.
- Input: `GetAsyncKeyState`, `GetKeyState`, `GetCursorPos`; `DINPUT=1` in INI [Inferred: keyboard polling, DirectInput optional].
- Keyboard mapping saved in registry `Config` blob, containing Win32 virtual-key codes 0x26 (Up) 0x28 (Down) 0x25 (Left) 0x27 (Right) 0x58 (X) 0x5A (Z) 0x43 (C) 0x44 (D) 0x53 (S). So default controls: arrows to move plus Z/X/C/D/S for actions [Verified from blob; action meaning Unknown].
- Exports/imports used to talk to DLLs: `GetPos`, `GetPosEx` and others appear in strings. The DLL API surface has not been disassembled yet [Unknown].

## Engine DLLs [Verified sizes/sections; behaviour Inferred]

| DLL | Code (.text) | Data (.data) | Notes |
|-----|-------------:|-------------:|-------|
| ENG1 | ~300 KB | ~160 KB | Source tree `SRC1\HERO\HERCULES`, modules HC_DIE, HC_DUNIT, HC_THROW. Biggest engine. |
| ENG3 | ~195 KB | ~70 KB | Source tree `SRC3\HERO\HERCULES`. |
| TITLE | ~128 KB | ~53 KB | Front end and tally screen; error text "Unable to malloc memory for tally screen". |

All import only `kernel32` and `user32` (tiny import tables, ~1.4 KB): they **do not call DirectX themselves**. They compute game state and draw into buffers/callbacks provided by the EXE [Inferred]. That is the key to modernization: the draw path is likely a narrow interface between DLL and EXE (see doc 05, "re-host" option).

## Rendering model [Inferred]

- Base resolution 640x480 (ReadMe: "increase resolution to 640x480"); likely also a smaller native size (e.g. 320x240 doubled) [Unknown].
- 8-bit palettized or 16-bit surfaces blitted with DirectDraw; sprite and tile assets decoded from `HERCULES.FS`.
- Frame timing and logic rate unknown; tied to the Windows timer. Modern CPUs may run it too fast [Unknown, test].

## Audio model [Inferred]

- Sound effects come out of `HERCULES.FS` (VAB-style banks, "vabid"); music was streamed from the game CD (audio tracks), hence absent.
- Modernization needs replacement music (see doc 07).

## Original platform baseline [Verified, DISNEY.INI]

Windows 95, Pentium 90 MHz, 16 MB RAM, 4x CD-ROM, DirectX 3.0 (DirectDraw/DirectSound/DirectInput). Installed copy came from `F:\Herc\`, build date 1997-09-12, language English.

## Current-state limitations

1. Needs a registry key and a fixed working directory to start.
2. Modern Windows may break DirectDraw fullscreen (needs windowed mode or compatibility shims; see doc 03).
3. Fixed low resolution, no scaling or filtering, no widescreen.
4. No music.
5. Game logic is closed in 32-bit DLLs without source.
6. Assets are in proprietary formats (doc 04).
