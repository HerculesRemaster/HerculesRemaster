# Disney's Hercules (1997) — Remastering with x32dbg on Windows 11

x32dbg is one of the most useful tools for reverse engineering `HERCULES.EXE`, especially when you want to understand the rendering engine, recover asset-loading logic, and eventually replace the original graphics with modern 4K assets.

However, x32dbg is a dynamic debugger, not a graphics remastering tool. You will use it to discover how Hercules works internally, then combine those discoveries with Ghidra, ImHex, and a modern rendering engine.

## 1. Recommended remastering workflow

HERCULES.EXE — Original Game (1997)

32-bit executable, original game assets and runtime

x32dbg

Runtime behavior, breakpoints, graphics calls

Ghidra

Decompile game code and reconstruct functions

ImHex

Inspect binary assets and file formats

Reconstructed Game Engine & Asset Pipeline

Sprites · Backgrounds · Animations · Levels · Rendering

Hercules 4K Remaster

Remastered assets, modern GPU rendering, preserved gameplay

For your project, the priority is to discover:

1. Rendering: Which graphics API Hercules actually uses, its internal resolution, color format, and how frames are displayed.
2. Asset loading: How sprites, backgrounds, animation frames, palettes, and levels are read and decoded.
3. Game logic: Character movement, collision detection, camera coordinates, animations, and level transitions.
4. Replacement points: Where you can introduce a modern renderer and load 4K assets without breaking gameplay.

## 2. Open Hercules with x32dbg

Use the executable from your installation:

```
C:\Disney\Hercules\HERCULES.EXE
```

First, make a separate working copy:

```
New-Item -ItemType Directory -Force "C:\Games\HerculesRE"

Copy-Item "C:\Disney\Hercules" `
  "C:\Games\HerculesRE\Original" -Recurse

Get-FileHash "C:\Games\HerculesRE\Original\HERCULES.EXE" `
  -Algorithm SHA256
```

Keep the original ISO and installation files intact.

Then:

1. Launch `x32dbg.exe` from the [official x64dbg project](https://x64dbg.com/).
2. Select File → Open.
3. Open `C:\Games\HerculesRE\Original\HERCULES.EXE`.
4. When the debugger pauses, inspect the CPU, Registers, Stack, Memory Map, and Symbols views.
5. Press `F9` to continue execution.

Important: x32dbg debugs 32-bit Windows programs. If your Hercules executable is 32-bit, this is the correct debugger even though Windows 11 is 64-bit. The original 16-bit installer, if present, is a separate issue.

### Essential shortcuts

| Shortcut  | Action                                                      |
| --------- | ----------------------------------------------------------- |
| `F9`      | Run / continue                                              |
| `F2`      | Set or remove a breakpoint                                  |
| `F7`      | Step into an instruction                                    |
| `F8`      | Step over an instruction                                    |
| `Ctrl+F9` | Execute until function return                               |
| `Ctrl+G`  | Navigate to an address in the CPU disassembly               |
| `Alt+F9`  | Return to user code when supported by the configured action |
| `Ctrl+F2` | Restart debugging                                           |

At this stage, do not edit assembly instructions. First, record which functions matter and how they interact.

If the game still displays its earlier CD error, that is also useful debugging evidence. You can trace the file and drive-access calls responsible for the failed disc detection before investigating graphics.

## 3. Important discovery: Hercules already has an asset extraction tool

For your remastering project, this is particularly useful.

The archived Steam release of Disney's Hercules contains the following files. Your 1997 CD edition may differ, so first verify which ones exist in your installation.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://steamdb.info&sz=32)

SteamDB

| File            | What to investigate                               |
| --------------- | ------------------------------------------------- |
| `HERCULES.EXE`  | Main executable and game startup                  |
| `HERCULES.FS`   | Game asset archive                                |
| `ENG1.DLL`      | Game module; investigate its functions            |
| `ENG3.DLL`      | Game module; investigate its functions            |
| `TITLE.DLL`     | Title-related game module                         |
| `VIDDATA\*.ETV` | Video-related files                               |
| `ddraw.dll`     | DirectDraw wrapper included in this later release |

This is the archived Steam file inventory, not confirmation that your CD contains identical files or DLL versions.

Even more useful, CTPAX-X released Disney's Hercules tools v1.1, including:

- `UNHERCFS` — an unpacker for `HERCULES.FS`.
- `ESFTOWAV` — an `.ESF` to `.WAV` audio converter.
- C source code for studying the extraction algorithms.

Source: [Disney's Hercules tools v1.1 — CTPAX-X](https://www.ctpax-x.org/index.php?goto=files&lang=en&show=114).&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.ctpax-x.org&sz=32)

CTPAX-X

This potentially saves considerable reverse-engineering effort. Before attempting to reconstruct an entire archive format in x32dbg, examine `UNHERCFS` and verify its output against your original files. Inspect or compile its source rather than blindly executing an old binary.

First, inventory your current installation:

```
Get-ChildItem "C:\Disney\Hercules" -Recurse -File |
    Select-Object FullName, Length |
    Out-File "C:\Games\HerculesRE\file-inventory.txt"
```

Your first asset milestone should be a lossless extraction of `HERCULES.FS`, assuming your build contains it. Then use x32dbg to map extracted resources to the corresponding loading and decoding functions.

## 4. Trace asset loading with x32dbg

Start the debugger with Hercules paused before its game code starts.

In the command bar at the bottom of x32dbg, enter these commands individually:

```
bp kernel32:CreateFileA
bp kernel32:CreateFileW
bp kernel32:ReadFile
bp kernel32:SetFilePointer
```

These are Windows file API breakpoints. x32dbg accepts exported function names and module-qualified addresses.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://help.x64dbg.com&sz=32)

x64dbg documentation

+1

Press `F9`.

### Example: Inspect `CreateFileA`

When the game opens a file, the debugger will pause at `CreateFileA`.

For a 32-bit process using the normal Win32 calling convention, the stack contains:

| Expression | Meaning                     |
| ---------- | --------------------------- |
| `[ESP]`    | Return address              |
| `[ESP+4]`  | Pointer to filename         |
| `[ESP+8]`  | Desired access flags        |
| `[ESP+C]`  | Sharing flags               |
| `[ESP+10]` | Security attributes pointer |
| `[ESP+14]` | Creation disposition        |

To inspect the filename:

1. Find `ESP` in the registers pane.
2. In the Stack pane, locate `[ESP+4]`.
3. Right-click the pointer and follow it in Dump.
4. View it as an ANSI string for `CreateFileA`, or UTF-16 for `CreateFileW`.
5. Switch to the Call Stack view to identify the Hercules function responsible for opening that file.

If the filename is `HERCULES.FS`, you have located the entry point into the archive-loading path.

### Automatically log filenames

Instead of manually stopping every time, use a logging breakpoint:

```
bp kernel32:CreateFileA
bplog kernel32:CreateFileA, "OPEN A: {ansi@[esp+4]}"
bpcond kernel32:CreateFileA, 0
```

For Unicode filenames:

```
bp kernel32:CreateFileW
bplog kernel32:CreateFileW, "OPEN W: {utf16@[esp+4]}"
bpcond kernel32:CreateFileW, 0
```

The `bpcond` setting makes the breakpoint collect log entries without normally pausing execution. This is documented x32dbg functionality.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://help.x64dbg.com&sz=32)

x64dbg documentation

+2

To preserve the results, right-click the Log pane and choose Redirect Log, then write to a file such as:

```
C:\Games\HerculesRE\asset-open-log.txt
```

If filenames include many system resources, identify the calls originating from `HERCULES.EXE`, `ENG1.DLL`, or other relevant game modules.

Next objective: Find the function that opens the archive, seeks to an asset offset, reads its bytes, and decodes them into pixel or animation data.

Remember that `CreateFileA` will only reveal the archive filename. Individual assets inside `HERCULES.FS` might be accessed through file offsets and indexes rather than separate filenames.

## 5. Discover the rendering engine

For a 4K remaster, this is the most important x32dbg investigation after identifying the asset archive.

First, open the Symbols view to inspect loaded modules. Determine whether Hercules uses DirectDraw, GDI, or its own software rendering routines.

### Step A — Trace DirectDraw initialization

If `ddraw.dll` is not loaded yet, configure a DLL-load breakpoint:

```
bpdll ddraw.dll,l
```

If it is already loaded, you can inspect its exports directly.

Once DirectDraw is loaded, set:

```
bp ddraw:DirectDrawCreate
bp ddraw:DirectDrawCreateEx
```

Only use the exports actually present in the loaded DLL. Press `F9`.

If a breakpoint is hit, inspect the Call Stack and follow the caller into Hercules code.

### Step B — Find the actual drawing functions

DirectDraw uses COM interfaces. Methods such as `SetDisplayMode`, `CreateSurface`, `Lock`, `Blt`, and `Flip` normally appear as calls through interface vtables, not ordinary DLL exports.

You need to follow the interface pointer returned by `DirectDrawCreate` or `DirectDrawCreateEx` and identify the relevant calls.

Example of an indirect COM method call (illustrative assembly)

```
mov eax, [esi]          ; Load interface vtable
push ...                ; Method arguments
push esi                ; Interface "this" pointer
call dword ptr [eax+XX] ; Invoke method
```

`XX` depends on the specific interface and method. Identify it using the correct DirectDraw interface definition rather than guessing.

Relevant functions:

| Function          | Why it matters                      |
| ----------------- | ----------------------------------- |
| `SetDisplayMode`  | Original resolution and color depth |
| `CreateSurface`   | Framebuffer and sprite surfaces     |
| `Lock` / `Unlock` | Direct pixel-buffer access          |
| `Blt` / `BltFast` | Sprite and background transfers     |
| `Flip`            | Frame presentation                  |
| `CreatePalette`   | Indexed-color palette handling      |

Microsoft retains documentation for these interfaces, including [IDirectDraw7](https://learn.microsoft.com/en-us/windows/win32/api/ddraw/nn-ddraw-idirectdraw7) and [IDirectDrawSurface7](https://learn.microsoft.com/en-us/windows/win32/api/ddraw/nn-ddraw-idirectdrawsurface7).&#x20;

[image](https://www.google.com/s2/favicons?domain=https://learn.microsoft.com&sz=32)

Microsoft Learn

+1

If those breakpoints never trigger, investigate the GDI path instead:

```
bp gdi32:BitBlt
bp gdi32:StretchDIBits
bp gdi32:SetDIBitsToDevice
```

### Step C — Discover internal resolution

Record the arguments supplied to `SetDisplayMode`, or inspect the dimensions of allocated framebuffers if using software rendering.

For example, a possible 640×480 mode corresponds to:

```
Width:  640 = 0x280
Height: 480 = 0x1E0
```

Those are example values, not verified values for your executable.

Once you locate the graphics initialization routine, record its module, relative virtual address (RVA), input arguments, and callers. Import those findings into Ghidra and give the routine a meaningful name such as `InitializeDisplay`.

Do not immediately patch 640×480 to 3840×2160. Resolution constants may also control framebuffer memory allocation, sprite positioning, and camera behavior.

## 6. Trace sprite decoding and animation

After finding the archive-reading function, the next objective is to follow a resource from the file into memory and finally onto the screen.

```
1. Read asset bytes from HERCULES.FS

2. Decompress or decode resource

3. Convert palette / pixel data

4. Build sprite or animation frame

5. Copy sprite into rendering surface

6. Present completed frame
```

A useful technique is to stop immediately after `ReadFile` returns, note the destination buffer address, then inspect it in the Dump view.

If you identify a writable memory buffer containing decoded pixels, you can set a hardware write breakpoint on a properly aligned four-byte location:

```
bph ADDRESS,w,4
```

Replace `ADDRESS` with the verified address. This breakpoint can help identify code writing into that particular part of the buffer. It cannot track every write across an entire image.

When the decoder or image construction routine is identified, document:

- Sprite dimensions and pixel format.
- Compression and palette transformations.
- Transparency / color-key behavior.
- Animation frame ordering and timing.
- Character anchor points, hitboxes, and screen coordinates.

These metadata are essential. A regenerated 4K sprite must not silently change the character's collision geometry or animation timing.

## 7. Choose how to implement the remaster

There are two realistic approaches.

| Approach                  | Description                                                                 | Recommendation             |
| ------------------------- | --------------------------------------------------------------------------- | -------------------------- |
| Graphics wrapper          | Preserve the game executable and intercept/modernize drawing                | Best first prototype       |
| Reconstructed game engine | Reimplement game systems using reversed specifications and extracted assets | Best long-term flexibility |

For your first prototype, test a DirectDraw compatibility layer such as [cnc-ddraw](https://github.com/FunkyFr3sh/cnc-ddraw) or [dxwrapper](https://github.com/elishacloud/dxwrapper). Both provide approaches to modernizing legacy DirectDraw compatibility, but neither automatically turns original sprite art into HD artwork.&#x20;


Keep each wrapper test isolated in a separate copy of the game directory. Do not mix multiple replacement `ddraw.dll` implementations.

### 4K rendering consideration

If the game uses a 4:3 virtual viewport, a faithful 4K display can preserve that viewport as 2880×2160 with side borders.

To occupy the full 3840×2160 screen without distortion, the remaster needs a wider virtual camera, adjusted UI layout, and potentially changes to world visibility.

My recommended long-term architecture is a new 32-bit-compatible game logic layer initially, paired with a modern renderer such as Direct3D 11. You could later migrate to a native 64-bit runtime without keeping the original executable running.

## 8. What to complete in your first x32dbg session

Reverse-engineering checklist

0/8

Launch Hercules under x32dbg without modifying the binary

Identify loaded game DLLs and graphics libraries

Capture the file-open log and locate HERCULES.FS

Identify the archive-reading function

Trace DirectDraw or GDI initialization

Record the internal resolution and framebuffer pixel format

Identify at least one sprite-decoding or drawing routine

Save the relevant RVAs and findings for Ghidra

Use these tools for different parts of the work:

- x32dbg: Discover runtime behavior and establish where data flows.
- Ghidra: Reconstruct relevant functions and document the original game logic.
- ImHex: Analyze extracted binary structures, sprite formats, and archives.
- Asset pipeline: Decode original artwork losslessly, regenerate or redraw it in HD, and preserve its metadata.
- Modern renderer: Display the remastered assets at 4K without tying gameplay coordinates to physical screen pixels.

The most productive next step is to inspect `HERCULES.FS` and `ENG1.DLL` first, if they exist in your version. `UNHERCFS` gives you a potential shortcut to extraction, while x32dbg can determine how those resources are loaded and rendered. That is a stronger foundation for a remaster than beginning with arbitrary patches to the display resolution.
