# Binary map

RVAs: HERCULES.EXE base 0x400000, DLLs base 0x10000000. Names follow `analysis/annotations/p2.json`. See also `module-interface.md` (EXE↔DLL table) and `resource-id-scheme.md` (archive).

## Ownership at a glance

| concern | owner | where |
|---|---|---|
| window, message loop | EXE (MFC) | idle handler 0x79a0 |
| clock / tick rate | EXE | `RT_Tick` 0x5f20, `timeBeginPeriod(1)` in startup 0x5e10 |
| game logic, levels, demo, hero | DLL (one module at a time) | tick: ENG1 0x20300, ENG3 0x1abb0, TITLE 0x5fb0 |
| file I/O | EXE (`FS_*` on HERCULES.FS), called by the DLL through PCopen/PCread slots | 0x5020–0x5340, slots 0x260–0x274 |
| asset buffers | EXE-owned 3 MiB arena (0x6177d0), carved by EXE allocators on DLL request | slots 0x38–0x78, 0xa0 |
| asset decoding | DLL for game data read via PCread. EXE for ESF sound (`AUD_CreateSfx` 0xf3d0) and level palettes (0x5760) | |
| rendering | EXE software rasteriser for the DLL's primitive list → 320×240 8-bit DIB | `GFX_DrawOTag` 0xac60 |
| input | EXE (keyboard + joystick → PSX pad bits) | 0x8040, written per tick by slot 0x258 |
| sound effects | EXE, DirectSound | init 0xfdf0, ESF loader 0xf3d0 |
| music | EXE, CD audio through MCI | 0x9c30–0xa120 |
| module/level change | DLL asks, EXE swaps DLL via longjmp | slot 0x64 → 0x4040 → 0x5cd0 → 0x5ec0 |
| shutdown | EXE | 0x5e90 |

## Startup and shutdown (EXE)

`RT_Startup` 0x5e10:

1. `timeBeginPeriod(1)`
2. 0x5750(0)
3. 0x4ce0 (registry config)
4. `FS_Init`
5. `GFX_InitDisplay` 0x72a0
6. input init 0x7bb0 (joystick)
7. 0xa0b0
8. sound init 0xfdf0 (`DirectSoundCreate`)
9. 0x1ff0 (first `MOD_ChangeModule`)
10. CD check 0x4ac0; without it: "You cannot play Hercules without the CD in the drive", exit
11. 0x6ec0(1)

`RT_Shutdown` 0x5e90:

1. `timeEndPeriod(1)`
2. writes registry `Config` (0x4da0, `HKLM\Software\Disney Interactive\Hercules\1.00`, 0x48 bytes)
3. 0xff70 (sound)
4. 0xa120 (CD close)
5. 0x74c0 (display)
6. `FS_Shutdown`

`RT_FatalError` 0x10460 calls `RT_Shutdown`, shows "Hercules Error" and exits.

## Main loop and clock

- MFC idle handler 0x79a0: display-mode switch check 0x6bb0, then `RT_Tick` when the game is active (`0xba0238 != 0`, not paused `0xba0228`); otherwise `WaitMessage`.
- `RT_Tick` 0x5f20: `timeGetTime` delta accumulated in 0x91c470, clamped to ±66 ms (0x42). **Fixed step 33 ms (0x21, ≈30 Hz)**. While the accumulator is ≥ 33 it runs catch-up ticks with `g_skipRender` (0xba01fc) = 1, at most 2. Then it always runs one rendering tick (`g_skipRender` = 0) and subtracts 33. Each tick is `MOD_RunTick` 0x5ec0 → `PC_DLLEngineMain(table, g_skipRender)`.
- 0x9cd0 (CD-audio time keeping, `timeGetTime`) runs first in every `RT_Tick`.

## Rendering

- `GFX_InitDisplay` 0x72a0:
  - Builds a reciprocal table (0x40000000 / i) at 0xa8e91c.
  - `GetDC`, `LOGPALETTE` of 256 entries, `BITMAPINFO` 8 bpp, **320 × −240 (top-down)**.
  - `CreateDIBSection` → pixels pointer `g_frameBits` 0x96e3d8, `CreatePalette`/`SelectPalette`.
  - Then `DirectDrawCreate` (0x96e3e4):
    - `SetCooperativeLevel(hwnd, 0x55)` = FULLSCREEN | NOWINDOWCHANGES | EXCLUSIVE | ALLOWMODEX.
    - `EnumDisplayModes` with callback 0x7170, which sets 0x92c488 = 1 when a mode with width 320 (0x140), height 240 (0xf0) and 8 bpp exists.
    - `SetCooperativeLevel(hwnd, 0x0c)` = NORMAL | NOWINDOWCHANGES. The game starts windowed.
- Fullscreen switch 0x6bb0 (window style popup, menu removed, cursor hidden):
  - `SetCooperativeLevel(0x55)`.
  - `GFX_SetMode` 0x6a60(320, 240):
    - `SetDisplayMode(320, 240, 8)` (vtbl 0x54).
    - `CreateSurface` primary (caps 0xa18, flip chain with 2 back buffers; falls back to 1). The decompile of the descriptor is partly garbled.
    - `GetAttachedSurface` (0x30) → back buffer 0x96e3ec.
    - `CreatePalette(DDPCAPS_8BIT)` (0x14) + `SetPalette` (0x7c).
  - Leaving fullscreen: `RestoreDisplayMode` + `SetCooperativeLevel(0x0c)`.
- `GFX_Present` 0x6620:
  - Windowed: `StretchDIBits` of the 320×240 DIB to the client size (0xba0220 × 0xba0224), then `GdiFlush`.
  - Fullscreen: `Flip(0, DDFLIP_WAIT)` on the primary (vtbl 0x2c). 0x6530 copies the frame between the DIB and the surface buffer (pitch arguments 320).
- **Render boundary:** the DLL builds a PlayStation-style ordering table of GPU primitives and passes it to `GFX_DrawOTag` (slot 0x164, 0xac60).
  - Primitive codes handled include 0xe1 (draw mode), 0xf3 (custom clip rectangle, x clamped to 0…320), 0x01 (custom image transfer → 0xa8f0), 0x2c, 0x3c, 0x74 and 0x7c, plus the 0x20/0x60 classes.
  - It rasterises into the 8-bit framebuffer and then presents (0x70c0, 0x7140, `GFX_Present`). So sprite-level draws cross the boundary, not a finished framebuffer (Q-08).
  - Textures/CLUTs live in an emulated VRAM fed by slots 0x204/0x20c (buffer 0x96e6e0).

## Input

- Keyboard: `GetAsyncKeyState` in 0x7bb0, 0x7f30, 0x83f0 (key remapping UI strings `Keyboard`, `Reset Keys`). Joystick: `joyGetPos` 0x7bb0, `joyGetPosEx` 0x7c50. No DirectInput.
- 0x8040 = keyboard bits (0x7f30) | joystick bits (0x7e90). It clears Down when Up is held and Right when Left is held, using PSX pad bits Up 0x10, Right 0x20, Down 0x40, Left 0x80. It returns 0 without the CD flag (0x43a594).
- Each tick, slot 0x258 (0xbcf0) stores `~bits` in the DLL's pad buffer (`*(0xa8e78c)`: byte 0 = 0, byte 1 = 0x40, u16 at +2). Then it runs 0xf820, 0xbdb0 and 0xbe40.

## Sound and music

- `DirectSoundCreate` in 0xfdf0. Sound effects are ESF files loaded by `AUD_CreateSfx` 0xf3d0 from `m:\language\sonix\pc\%ld.esf` (= `Q:`/`R:` drive alias in `FS_NormalizePath`).
- Sonix bank management called by the DLL: `AUD_LoadLevelSonix` 0x16b0, `AUD_LoadMusic` 0x1110 (`MUSIC.MUS`), `AUD_LoadSfx` 0x1330 (`SFX.SFX`), `AUD_StartMusic` 0x17d0/0x18f0, `AUD_StopMusic` 0x15d0, `AUD_UnloadAllSfxMusic` 0x1610.
- Music is **Red Book CD audio** through `mciSendStringA`:
  - 0x9c30 opens and checks (`open cdaudio shareable wait`, `status cdaudio media present`, `status cdaudio ready`).
  - The play path uses `set cdaudio time format tmsf` and `play cdaudio from %d to %d`.
  - 0x9e10 stops (`stop cdaudio wait`); 0xa120 closes (`close cdaudio wait`).
  - Without the disc there is no music (Q-01).

## Level change and shutdown from the DLL

- DLL → slot 0x64 `MOD_ChangeModule(dir, name)`. The EXE resets state and frees the module's block, then picks `title.dll` / `eng1.dll` / `eng3.dll` by basename `TITLE` / `ENGINE1` / `ENGINE3`, then `longjmp`s to `MOD_RunTick`, which `FreeLibrary`s the old DLL and `LoadLibrary`s the new one. Which level names map to ENGINE1 vs ENGINE3 is open (Q-07, P3).
- DLL → slot 0x238 `RT_Quit` (posts a close message), slot 0x228 `RT_FatalError`.
