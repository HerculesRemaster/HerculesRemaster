# Fullscreen on the 4K Windows 11 display (P2F)

The original game runs fullscreen on the 3840×2160 monitor through our own `ddraw.dll` proxy. The image is a borderless, sharp 9× integer scale of the 320×240 frame (2880×2160 at x = 480, black pillarbox bars), at the game's own 30 Hz. Only `working/game/` is changed.

## Baseline (no proxy)

- The game starts on Win11 without a wrapper or compatibility settings. The CD check passes, and the title bar shows "Disney's Hercules", not "- Demo Mode".
- The display mode is not saved: `RT_Startup` (EXE 0x5e10) calls `GFX_RequestFullscreen(1)` (0x6ec0) on every start. While the game is the active app, the idle handler then runs `GFX_SwitchDisplayMode` (0x6bb0): `SetCooperativeLevel(0x55)` + `SetDisplayMode(320,240,8)` + a primary flip chain.
  - **On Win11 this is broken**: the screen goes black, and only a thin strip of the image appears near the top-left. Windows emulates the 8-bit low-resolution mode.
- When the game is not the active app (`g_appActive` 0xba0238 < 2), `RT_UpdateActiveState` (0x5a50) titles it "Disney's Hercules - Waiting", requests windowed mode and stops ticking.
- The windowed client is 320×240 logical (the process is DPI-unaware, so Windows bitmap-stretches it at 225 % scaling). Menu Display → 320/480/640 Window calls `GFX_SetWindowClient` (0x99a0).
- `GFX_Present` (0x6620) uses `StretchDIBits` from the 320×240 DIB to the full client size. Setting the window to borderless 3840×2160 by hand fills the screen (stretched to 16:9). So the present path accepts any client size.
- The registry `Config` blob has no display byte. Bytes 0x0c and 0x18 are "Game Play" options (0x6210/0x6390).

## Route

`runtime/display/ddraw_proxy.c` builds an x86 `ddraw.dll` that sits next to `HERCULES.EXE`. Windows loads it before the system DLL.

- `DirectDrawCreate` is forwarded to `%SystemRoot%\SysWOW64\ddraw.dll`.
- **The game stays on its windowed GDI path.** `GFX_RequestFullscreen` (0x6ec0) is patched in memory to always store 0, so the broken mode switch never runs.
  - This differs from the plan's idea of faking `SetCooperativeLevel`/`SetDisplayMode` success. That would leave the game drawing through a DirectDraw flip chain that can't work without exclusive mode.
- `GFX_CmdFullscreen` (0x9990: menu "320x240 Fullscreen", Alt+Enter) jumps to the proxy, which toggles between borderless and windowed.
- IAT patches in HERCULES.EXE:
  - `CreateWindowExA`: finds the main frame, subclasses it (hides the cursor, paints the border colour) and applies the mode.
  - `SetWindowPos`: the game's own size/position changes are ignored for the main window.
  - `SetMenu`: menu hidden while borderless.
  - `ShowCursor`: cursor hidden while borderless.
  - `StretchDIBits`: the 320×240 frame is drawn into the rectangle chosen by `scale`/`aspect`. It uses `COLORONCOLOR` (nearest) or `HALFTONE` (linear) and fills the bars. Then `DwmFlush` runs when `vsync = on`.
  - `AdjustWindowRectEx`: pass-through, kept for the windowed frame maths.
- The process is made per-monitor DPI aware (`SetProcessDpiAwarenessContext(PER_MONITOR_AWARE_V2)`), so Windows no longer bitmap-stretches the window.
- The code patches check the original bytes at 0x6ec0 and 0x9990 first and are skipped (with a log line) if they differ.
- Game timing is not touched.

## Config: `working/game/hercules_display.ini`

Template: `runtime/display/hercules_display.ini`. Missing keys use these defaults.

| key | values | default | notes |
|---|---|---|---|
| mode | borderless, windowed, exclusive | borderless | exclusive = no patches, original DirectDraw path (debug only) |
| monitor | primary, 1, 2 … | primary | index in `EnumDisplayMonitors` order |
| scale | integer, fit, stretch | integer | integer = 9× on 2160p (2880×2160) |
| filter | nearest, linear | nearest | |
| aspect | 4:3, 16:9 | 4:3 | 16:9 only matters with `scale = stretch` |
| border_color | RRGGBB | 000000 | |
| window_scale | N | 3 | windowed client = 320N × 240N, centred on the work area |
| vsync | on, off | on | `DwmFlush` after each present |
| fps_cap | 0 | 0 | reserved; game timing is never changed |
| dpi_aware | on, off | on | |
| hide_cursor | on, off | on | borderless only |
| log | on, off | off | `working/game/hercules_display.log` (fps every 5 s, image rectangle) |

The proxy strips inline `;` comments and surrounding spaces.

## Build, install, launch

1. `runtime\display\build.bat` → `runtime\display\bin\ddraw.dll` (MSVC x86; the "vswhere.exe is not recognized" line comes from vcvars32 and is harmless).
2. `pwsh -File tools/runtime/launch_fullscreen.ps1` does three things:
   - copies `ddraw.dll` and the ini template into `working/game` (`-KeepIni` keeps an edited ini);
   - turns logging on for the run, starts the game, captures the game window at 10 s (`PrintWindow`, window content only), checks the window and image rectangles, and closes the game;
   - writes `analysis/runtime/fullscreen/<stamp>/result.json`.
3. To play, just start `working/game/HERCULES.EXE` once the proxy and ini are in place.

## Measured (2026-10-12, i9-10900K / RTX 3090, 3840×2160 @ 60 Hz)

| mode | window | image | present rate |
|---|---|---|---|
| borderless, integer, nearest | 0,0 3840×2160 (= monitor) | 480,0 2880×2160, black bars | 30.3 fps steady (first 5 s 24 fps while loading) |
| windowed, window_scale 3 | client 960×720, centred, with menu | 0,0 960×720 | 24 fps while starting; pauses when not the active app (original behaviour) |

The game presents once per rendering tick (30 Hz). The GDI stretch to 2880×2160 keeps up, so the D3D11 renderer from the plan was not needed. It is only worth doing if the play check shows tearing or stutter.

## Known issues / notes

- Like the original, the game pauses ("Waiting") when it isn't the active app, e.g. after Alt-Tab. Clicking it resumes.
- In windowed mode the menu's 320/480/640 Window items no longer resize the window (`SetWindowPos` is ignored for the main window). Use `window_scale` instead.
- At startup `GFX_InitDisplay` still calls `SetCooperativeLevel(0x55)` and immediately goes back to `0x0c`. This has no visible effect.
- Dialogs (F2 options, F1 help, About) are normal windows above the borderless window.
