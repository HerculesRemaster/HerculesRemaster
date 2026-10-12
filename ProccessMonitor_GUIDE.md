# Using Process Monitor to Reverse Engineer Disney's Hercules (1997)

Goal: Use Microsoft's `Procmon.exe` on Windows 11 to discover how Disney's Hercules loads its assets, identify game resource files, and prepare a batch-extraction pipeline for a modern 4K remaster.

Microsoft Process Monitor

Free Windows diagnostic utility.

Official version: v4.11, September 2026

[Download from Microsoft](https://learn.microsoft.com/en-us/sysinternals/downloads/procmon)

## 1. Understand what Procmon can extract

Procmon does not directly extract graphics or decode proprietary image formats. Instead, it records which files the executable opens, reads, writes, and loads.

| Objective                          | Can Procmon do it?              |
| ---------------------------------- | ------------------------------- |
| Identify game asset files          | Yes                             |
| Discover runtime file access paths | Yes                             |
| Monitor CD/ISO reads               | Yes, for visible filesystem I/O |
| Find missing game files            | Yes                             |
| Locate DLL dependencies            | Yes                             |
| Identify archive loading activity  | Yes                             |
| Extract images from archives       | No                              |
| Decompress sprites and animations  | No                              |
| Reveal asset bytes in memory       | No                              |

### Important discovery: HERCULES.FS

The PC game's Steam distribution includes the following resources:

| File                                | Purpose for investigation                         |
| ----------------------------------- | ------------------------------------------------- |
| `HERCULES.FS`                       | Major resource container; approximately 44.75 MiB |
| `HERCULES.EXE`                      | Main executable                                   |
| `ENG1.DLL`, `ENG3.DLL`, `TITLE.DLL` | Game DLLs worth reverse engineering               |
| `VIDDATA\*.ETV`                     | Proprietary video files                           |
| `MUSIC\Track02.ogg`                 | Audio in the Steam distribution                   |

These are verified for the Steam version; your original 1997 CD edition may differ.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://steamdb.info&sz=32)

SteamDB

+1

The most important target for your remaster investigation is `HERCULES.FS`, if it exists in your installation.

Procmon can tell us when the game opens this file and how it reads from it. ImHex and Ghidra can then help determine the internal archive structure.

## 2. Prepare the game and Procmon

Using your existing Windows 11 setup, the relevant locations are:

```
C:\Games\Hercules_AG.iso
C:\Games\HerculesInstaller\
C:\Disney\Hercules\HERCULES.EXE
```

If you have moved your game to `C:\Games\GameHercules`, use that installation instead.

First, open PowerShell and inventory your installed game files:

```
$GameDir = "C:\Disney\Hercules"

Get-ChildItem $GameDir -Recurse -File |
    Sort-Object Length -Descending |
    Select-Object FullName, Length |
    Format-Table -AutoSize
```

Check whether `HERCULES.FS` exists:

```
Get-ChildItem $GameDir -Recurse -Filter "*.FS"
```

Also inspect the mounted ISO, since some resources may reside on the CD rather than in the installed directory.

### Start Process Monitor

1. Extract the official Process Monitor ZIP.
2. Run `Procmon64.exe` as Administrator. It can monitor a 32-bit game on 64-bit Windows.
3. Accept the license agreement.
4. Press `Ctrl + E` to stop automatic capture.
5. Press `Ctrl + X` to clear currently displayed events.

Microsoft documents these capture controls and the 64-bit executable selection.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://learn.microsoft.com&sz=32)

Microsoft Learn

+1

## 3. Configure filters for HERCULES.EXE

Open Filter → Filter... or press `Ctrl + L`.

Add the following filter:

Process Name

is

HERCULES.EXE

Include

Click Add, then Apply and OK.

For your first capture, leave operation and path filtering unrestricted. Once you understand the game startup, you can filter specific events.

Keep these event categories enabled: File System Activity, Process and Thread Activity, and Registry Activity. You can disable network events if unnecessary.

Do not enable Drop Filtered Events during exploration, because you may need to inspect previously hidden entries.

## 4. Record how the game loads assets

1. Start a clean recording

   In Procmon, press `Ctrl + E` to start capturing.

2. Launch Hercules
   ```
   Start-Process "C:\Disney\Hercules\HERCULES.EXE" `
       -WorkingDirectory "C:\Disney\Hercules"
   ```
   If you normally run Hercules through a compatibility launcher, launch it that way instead.
3. Observe the title screen

   Watch for reads from `HERCULES.FS`, DLLs, video files, and CD locations.

4. Load the first playable level

   Move Hercules, attack, and trigger effects. Each operation helps identify when the executable requests game resources.

5. Stop recording

   Return to Procmon and press `Ctrl + E`.

6. Save the capture

   Select File → Save, choose All events, and save in native `.PML` format, for example:

   `C:\Games\HerculesAnalysis\procmon\level01.pml`

Repeat separate recordings for the introduction, menus, level transitions, bosses, and cutscenes. Small, scene-specific traces are much easier to compare than one enormous recording.

For a long recording, use File → Backing Files to write the trace to disk instead of accumulating it in virtual memory.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://learn.microsoft.com&sz=32)

Microsoft Learn

## 5. Identify which files contain the assets

Open Tools → File Summary after stopping the recording.

This gives you a summarized list of the files accessed by the game, including event counts and I/O activity.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://documentation.help&sz=32)

Process Monitor Documentation

For each relevant file, investigate the following operations:

| Operation                  | Meaning                                   |
| -------------------------- | ----------------------------------------- |
| `CreateFile`               | The game opens or attempts to open a file |
| `ReadFile`                 | Reads data from an opened file            |
| `CreateFileMapping`        | Creates a file-backed memory mapping      |
| `Load Image`               | Loads an executable module or DLL         |
| `QueryOpen` / file queries | Probes file existence or metadata         |

Double-click an event to inspect its properties.

For a `ReadFile` event, look for information such as:

Example: ReadFile

SUCCESS

Path

C:\Disney\Hercules\HERCULES.FS

Offset

0x00100000

Length

65536 bytes

Illustrative event only, not captured from your installation.

This information helps you identify which parts of the resource archive are accessed at different gameplay moments.

A limitation: an archive may be loaded into memory all at once, or mapped into memory. In that case, Procmon alone cannot tell you which internal sprite was used.

## 6. Correlate assets with gameplay

Create multiple short captures, then compare file access behavior.

| Capture | Test scenario       | Information to investigate                |
| ------- | ------------------- | ----------------------------------------- |
| 01      | Game startup        | DLLs, configuration, primary archives     |
| 02      | Main menu           | UI resources, title assets                |
| 03      | First level loading | Level-specific archive reads              |
| 04      | Character movement  | Additional reads or memory-only animation |
| 05      | Combat effects      | Sound/effect resource access              |
| 06      | Cutscene playback   | `.ETV` video files                        |
| 07      | Level transition    | Resource loading/unloading patterns       |

If the game reads new regions of `HERCULES.FS` during a level transition, compare those offsets against its internal archive structure later.

For more precise analysis, examine an event's Stack tab to determine which executable or DLL code initiated the access. Even without function names, module offsets can guide your work in Ghidra or x32dbg.

## 7. Export the results for automated analysis

Save another copy of the Procmon trace as `CSV`, using Displayed events after applying your Hercules process filter.

For example:

```
C:\Games\HerculesAnalysis\
├── procmon\
│   ├── startup.pml
│   ├── level01.pml
│   └── Hercules_FileIO.csv
├── analysis\
├── extracted\
└── remastered\
```

Use PowerShell to generate a file-access inventory:

```
$CsvPath = "C:\Games\HerculesAnalysis\procmon\Hercules_FileIO.csv"
$OutPath = "C:\Games\HerculesAnalysis\analysis\file_inventory.csv"

New-Item (Split-Path $OutPath) -ItemType Directory -Force | Out-Null

Import-Csv $CsvPath |
    Where-Object {
        $_.'Process Name' -ieq 'HERCULES.EXE' -and
        $_.Operation -in @('CreateFile', 'ReadFile', 'CreateFileMapping') -and
        $_.Result -eq 'SUCCESS'
    } |
    Group-Object Path |
    Sort-Object Count -Descending |
    Select-Object Count, Name |
    Export-Csv $OutPath -NoTypeInformation
```

The resulting CSV identifies frequently accessed files. It is not yet a list of individual sprites or textures.

## 8. Extract HERCULES.FS using a dedicated tool

I found an existing extractor specifically written for Disney's Hercules.

Disney's Hercules Tools v1.1

2012

CTPAX-X Team — source code in C

The package documents two utilities:

- `UNHERCFS` — unpacks `HERCULES.FS`.
- `ESFTOWAV` — converts `.ESF` audio to `.WAV`.

[Open the extractor's official project page](https://www.ctpax-x.org/index.php?goto=files&lang=en&show=114)&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.ctpax-x.org&sz=32)

CTPAX-X

This is potentially much more efficient than reverse engineering the entire archive format from scratch.

Recommended sequence:

1. Back up the original `HERCULES.FS`.
2. Obtain and inspect the extractor's source code.
3. Verify the command-line parameters or compile the C source.
4. Unpack into a separate output directory.
5. Inventory the resulting files, formats, and sizes.
6. Use ImHex to inspect unknown formats and Ghidra to understand their loaders.

Because this is a third-party archive utility, scan any downloaded executable and prefer compiling the source yourself. I haven't verified its compatibility with your specific 1997 ISO.

For `.ETV` video extraction, [MultimediaWiki's ETV specification](https://wiki.multimedia.cx/index.php/ETV) documents the file header, frames, palettes, and decompression structure.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://wiki.multimedia.cx&sz=32)

MultimediaWiki

## 9. Combine Procmon with your other reverse-engineering tools

HERCULES.EXE + Game Resources

Original Windows game

Procmon

Discover files, paths, I/O and DLL access

ImHex

Inspect binary headers, offsets, chunks and palettes

Ghidra + x32dbg

Reverse engineer loading and decoding functions

Asset Extraction Pipeline

Extract sprites, backgrounds, palettes, animation metadata

4K Remaster Pipeline

AI upscaling, asset regeneration, validation, engine integration

For the final 4K remaster, preserve the original sprite dimensions, animation timing, pivot points, palettes, and alignment metadata before generating higher-resolution replacements.

Higher-resolution images alone will not make the old rendering engine support 4K. That requires a separate rendering modernization or replacement strategy.

## 10. If Hercules still cannot start

Since your earlier Windows 11 installation encountered a CD verification error, Procmon can also help identify the failing file operations.

Use these additional filters individually:

| Filter                      | What it reveals                        |
| --------------------------- | -------------------------------------- |
| `Result is NAME NOT FOUND`  | Files or paths the game cannot find    |
| `Result is PATH NOT FOUND`  | Missing directories                    |
| `Result is ACCESS DENIED`   | Permission problems                    |
| `Path contains HERCULES.FS` | Attempts to access the primary archive |
| `Path contains VIDDATA`     | Video-file access                      |

Also inspect registry events for `DISNEY.INI` and the game's historical configuration under `HKLM\SOFTWARE\Disney Interactive`, including any 32-bit registry redirection.

A missing-file result is not automatically the cause of failure; look for repeated failures without later successful fallback.&#x20;
