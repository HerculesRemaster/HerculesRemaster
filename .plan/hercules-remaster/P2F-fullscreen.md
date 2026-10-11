# P2F Fullscreen on a 4K Windows 11 display

Goal: make the **original** game run fullscreen on the 3840×2160 monitor before any remaster work. It should look sharp, keep the 4:3 aspect, and not trigger a broken mode switch. Change only `working/game/`. The originals stay untouched.
Needs: P2 (render path RVAs). Does P3's first two tasks (working copy, launcher) because it needs them.
Outputs:
- `working/game/` (playable fullscreen copy)
- `runtime/display/` (x86 proxy `ddraw.dll` source + build script), `working/game/hercules_display.ini`
- `tools/runtime/make_working_copy.ps1`, `tools/runtime/launch_original.ps1`, `tools/runtime/launch_fullscreen.ps1`
- `docs/fullscreen.md` (route chosen, config keys, how to launch, known issues)

## Background (from P2, `docs/binary-map.md`)

- Internal frame: **320×240, 8-bit palettised** DIB (`GFX_InitDisplay` 0x72a0, pixels `g_frameBits` 0x96e3d8).
- Windowed: `GFX_Present` 0x6620 calls `StretchDIBits` from the 320×240 DIB to the client size (0xba0220 × 0xba0224). The window you see (640×480) is a 2× stretch of that DIB.
- Fullscreen switch 0x6bb0, called from the idle handler 0x79a0: `SetCooperativeLevel(0x55)` → `GFX_SetMode` 0x6a60 → `SetDisplayMode(320,240,8)` + flip chain + 8-bit palette. Win11 has no real 320×240×8 mode, so this path is unreliable.
- Registry `Config` blob (0x48 bytes, read 0x4ce0 / written 0x4da0) probably stores the display setting.
- 4K maths: 2160 / 240 = **9**. Integer 9× scaling gives 2880×2160, with 480 px pillarbox bars left and right. Q-04's default is borders.

## Config (`working/game/hercules_display.ini`)

The proxy reads this file at startup. Missing keys use the defaults shown.

```ini
[display]
mode = borderless      ; borderless | windowed | exclusive (exclusive = original SetDisplayMode path, debugging only)
monitor = primary      ; primary | 1 | 2 ...
scale = integer        ; integer (9x on 4K) | fit (largest 4:3, may be non-integer) | stretch (fill 16:9, distorts)
filter = nearest       ; nearest | linear
aspect = 4:3           ; 4:3 | 16:9 (16:9 only with scale = stretch)
border_color = 000000
window_scale = 3       ; windowed mode only: client = 320*N x 240*N
vsync = on
fps_cap = 0            ; 0 = game's own 30 Hz clock. Never change game timing here.
dpi_aware = on         ; process is per-monitor DPI aware, so Windows doesn't bitmap-stretch the window
hide_cursor = on
log = off              ; on → working/game/hercules_display.log
```

## Tasks

- [x] Finish and run `tools/runtime/make_working_copy.ps1` (`backups/originals` → `working/game`) and `tools/runtime/launch_original.ps1` (start, screenshots at 2/5/10/30 s, exit code). These are P3's first two tasks. Tick them there as well. Record the baseline in `docs/fullscreen.md`: window size, whether the game starts, whether its fullscreen toggle works on Win11, and the DPI scaling Windows applies.
- [x] Ghidra: find what sets the window size and what triggers the fullscreen switch 0x6bb0 (menu command ID, hotkey, `Config` byte). Find the caller of `CreateWindowExA`/`AdjustWindowRect`/`SetWindowPos`/`MoveWindow` and the menu resource. Decode the display byte(s) in the `Config` blob. Name these `GFX_*`/`RT_*`, save, and append the names to `analysis/annotations/p2.json`.
- [x] Quick check with no code: set the window to borderless and monitor-sized using `win32.ps1` (SetWindowLong + SetWindowPos) on a running game. If `StretchDIBits` follows the new client size, the image fills the screen, stretched to 16:9. This shows whether the present path tolerates any client size.
- [x] Build the fullscreen proxy `runtime/display/ddraw.dll` (our own code, MSVC x86, `build.bat` via vcvars32). It goes in `working/game/` only. Windows loads it before the system ddraw because it sits next to the EXE.
  - It forwards `DirectDrawCreate` (and any other used exports) to `%SystemRoot%\SysWOW64\ddraw.dll`.
  - On attach it reads `hercules_display.ini`, calls `SetProcessDpiAwarenessContext(PER_MONITOR_AWARE_V2)` when `dpi_aware = on`, and IAT-patches `HERCULES.EXE`'s `StretchDIBits`, `CreateWindowExA`, `SetWindowPos`/`MoveWindow`/`AdjustWindowRect` and `ShowCursor`.
- [x] `mode = borderless`: create or convert the main window to WS_POPUP covering the chosen monitor. Make the game's fullscreen switch a no-op: return success from `SetCooperativeLevel(0x55)` and `SetDisplayMode` without changing the desktop mode, and keep presenting through the windowed `StretchDIBits` path. The hooked `StretchDIBits` draws the 320×240 DIB into the destination rectangle chosen by `scale`/`aspect` (9× = 2880×2160 at x = 480), fills the bars with `border_color`, and sets `SetStretchBltMode` to COLORONCOLOR for nearest. Fall back to the Ghidra-found window path if the IAT hooks miss the call.
- [x] If GDI stretching to 4K drops below 30 fps or tears, present through D3D11 instead: upload the 320×240 indexed frame plus the palette as textures, run a point- or linear-sampled quad shader, and flip with vsync. Keep the GDI path as `renderer = gdi` and add `renderer = d3d11` to the config.
- [x] `mode = windowed`: the client area is 320·`window_scale` × 240·`window_scale`, centred, with a normal frame. Mouse and cursor behave normally.
- [x] Write `tools/runtime/launch_fullscreen.ps1`. It checks that the proxy and ini are in `working/game`, starts the game, takes a screenshot at 10 s, checks that the window rectangle equals the monitor rectangle and that the image rectangle is 2880×2160 at x = 480, and closes the game.
- [x] **[HUMAN]** play check at 4K: title, menu, the demo, one level. Is the image sharp, is the aspect 4:3, do keyboard/joystick, alt-tab and exit work, and does it run at 30 Hz with no tearing?
- [x] (not needed: the proxy works) **[HUMAN] fallback only:** if the proxy route stalls, ask whether a third-party DirectDraw wrapper (e.g. cnc-ddraw, dgVoodoo2, DxWnd) may be downloaded into `working/game`. Rule 7 applies.
- [x] Write `docs/fullscreen.md`: the route, the config keys, launch steps, measured fps, and known issues.

Token budget: the proxy logs to `hercules_display.log` (off by default). Read it with `rtk log`, and run scripts as `rtk summary pwsh -File …`.

Done when `launch_fullscreen.ps1` shows a borderless 3840×2160 window with a sharp 9× 4:3 image, and the user has confirmed the play check.

## Notes

- Baseline: the game starts on Win11 with no wrapper. Its own fullscreen (`SetDisplayMode(320,240,8)`) is broken: black screen with a thin image strip. `RT_Startup` requests fullscreen on every start (0x6ec0(1)). The display mode isn't in `Config`.
- Display menu: 32800 fullscreen/Alt+Enter → `GFX_CmdFullscreen` 0x9990; 32801/32868/32867 → `GFX_SetWindowClient` 0x99a0 (320/480/640). Names applied and saved (p2.json).
- Proxy route: the game stays windowed/GDI (0x6ec0 patched to 0), the proxy makes the frame a borderless monitor popup, and `StretchDIBits` draws a 9× nearest image at 480,0 2880×2160. Alt+Enter toggles borderless/windowed.
- `launch_fullscreen.ps1`: window = monitor 3840×2160, image 480,0 2880×2160, bars black, **30.3 fps** steady. GDI is fast enough, so no D3D11 (task closed as not needed). Windowed: client 960×720 centred.
- Screenshots now use `PrintWindow` (game window only), so other apps never end up in a capture.
- Waiting on the [HUMAN] play check. The wrapper fallback isn't needed.

- 2026-10-12: user play check **passed** (sharp 4:3, input, Alt+Enter, Alt-Tab, exit). Performance tuning deferred; focus is RE and asset extraction.
