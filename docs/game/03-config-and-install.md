# 03. Configuration, Registry and Install

## Install flow

`SETUP.BAT` runs `%windir%\regedit HERC.REG`, which imports registry keys so `HERCULES.EXE` finds its "install". There is no real installer.

## HERC.REG [Verified]

Key: `HKLM\SOFTWARE\Wow6432Node\Disney Interactive\Hercules\1.00`

| Value | Data | Meaning |
|-------|------|---------|
| `DATA` | `DATA\` | Data sub-folder (not present here; actual data is `HERCULES.FS` beside the EXE) |
| `User Path` | `.\` | Save/user files path (relative to current folder) |
| `PATH` | `.\` | Install path |
| `BitVersion` | `32 Bit` | |
| `EntryPoint` | `.\Hercules.exe` | |
| `Help` | `.\hercules.hlp` | |
| `Uninstall` | `C:\WINDOWS\uninst.exe -fC:\Hercules\DeIsL1.isu` | InstallShield remnant; dead |
| `Config` | 64-byte binary blob | Controls and options; includes key codes 26 28 25 27 58 5A 43 44 53 (Up Down Left Right X Z C D S) |

Note: because paths are relative (`.\`), run the EXE with its own folder as current directory, otherwise it fails with "Hercules is not installed" or cannot find `hercules.fs`.

The `Config` blob layout is not decoded. Bytes `05 CA AB 07` look like a header/magic; later bytes hold flags and the key list [Unknown].

## DISNEY.INI [Verified]

Installer-generated environment record. Relevant lines:
- `BasePlatformOS=WINDOWS_95`, `BasePlatformCpu=PENTIUM`, `BasePlatformRam=16000`, `BasePlatformSpeed=90`, `BasePlatformCDSpeed=4X`
- `DDRAW=1 DSOUND=1 DINPUT=1 DPLAY=0 D3D=0` (DirectDraw, DirectSound, DirectInput used; no DirectPlay networking; no Direct3D)
- `Language=ENGLISH`, `Version=1.00`, `DATE=9-12-1997`
- `[Direct Draw Drivers]` from the original developer test machine (Matrox Millennium II), not relevant now.
- Paths inside (`F:\Herc\`, `C:\HERCULES\`) are the original machine's; the game appears to treat the INI as informational.

## Running on Windows 11 today

Practical, in order of effort:

1. **Direct:** run `SETUP.BAT` as administrator (HKLM write needs elevation), then `HERCULES.EXE`. Use Alt+Enter for windowed mode.
2. **Compatibility mode:** right-click EXE > Properties > Compatibility: Windows 95/98 mode, 256 colours, 640x480, disable fullscreen optimizations, run as admin.
3. **DirectDraw wrapper:** drop `ddraw.dll` from `dgVoodoo2` or `cnc-ddraw` next to the EXE; fixes fullscreen, scaling, timing on modern GPUs.
4. **VM/emulator:** 86Box, PCem or VirtualBox with Windows 98 for a reference baseline. **Record gameplay video of every level here as ground truth** for the rebuild.

Record what actually happens in step 1-3 and add it to this file [Unknown today].

## Cleanup suggestions

- `Icons/` and `unins000.dat` are not needed by the game.
- `ReadMe.txt` carries a download-site promotion; keep only as provenance.
- Keep an untouched backup of the original folder (read-only, hash recorded) before experimenting. Create `SHA256SUMS` for all files.
