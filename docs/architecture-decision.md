# Architecture decision (P6)

## What crosses the DLL → EXE drawing boundary (Q-08)

The game DLLs never hand over a finished image. Each tick they build a PlayStation-style **ordering table of GPU primitives** and call `GFX_DrawOTag` (EXE 0xac60, table slot 0x164). The PC EXE rasterises it in software into a 320×240 8-bit frame and shows it (`GFX_Present` 0x6620).

| content | how it reaches the EXE |
|---|---|
| level geometry | textured quads (POLY_FT4/GT4: codes 0x2c, 0x3c) with VRAM tpage + UV + a "CLUT" field. The PC rasteriser uses that field as a fog-row index into `MAP_PSX.CIF` (`palette[fog[level][texel]]`) |
| sprites | small quads/sprites (0x74, 0x7c, 0x60 class) whose textures ENG1 unpacks into 32×32 VRAM cells each time a new frame is shown (`ANIM_CacheFrame`), drawn with raw texel indices (the sprite rasteriser 0x40cab0 copies texels into the 8-bit frame; the bank's 15-bit palettes are not used on the PC) |
| screens | TGA pixels uploaded straight into VRAM (slot 0x204) |
| control | draw-mode (0xe1), clip rectangle (0xf3), image upload (0x01) |

So every draw is still separate at the boundary: position, texture page/UV and palette are known per quad. That makes it possible to re-render at any resolution.

## Measured facts

- Fixed 33 ms simulation (30 Hz, `RT_Tick`), one present per tick. The DLLs depend on that rate (scripts count ticks).
- All formats needed to draw the game are decoded (P4/P5): sprites (`i1.4`), screens (TGA), level geometry, collision, cameras and spawns (`i2.5`; ENG3's `i1.a` still open), palettes/fog (CIF), sounds (ESF/Sonix), music (CD audio segments, not present).
- Spike `runtime/prototypes/spike` (C++17, Win32 + D3D11 + WIC, x64, no third-party code) draws the decoded TRAINING level and Hercules' run cycle at 3840×2160. The view is the original 320×240, 9× integer, pillarboxed. Result: 59.9 fps, 30.1 Hz ticks, p95 17.1 ms (`runtime/prototypes/spike/spike.log`, `spike.png`).
- The original runs on Win11 through our proxy (P2F). Our hook DLL can intercept any EXE function (P3).

## Options

| | A. Original logic + new renderer (hybrid) | B. Native re-implementation | C. A first, then B piece by piece |
|---|---|---|---|
| how | keep HERCULES.EXE + DLLs. An in-process hook replaces `GFX_DrawOTag`/`GFX_Present`: it reads the primitive list, VRAM and CIF, and draws with D3D11 at 4K. Replacement art is keyed by (VRAM page, UV rect) or by sprite bank/frame | new 64-bit program (C++ or other) that loads the decoded assets and re-implements the hero, enemies, bosses, levels, menus and audio from the spec | ship A early, port subsystems to native code where needed |
| fidelity | exact: same code, timing and demos | only as good as the RE of every entity and boss | exact at first |
| effort to a full game | low/medium: one renderer + texture replacement | very high: about 100 entity types, ENG3, TITLE menus, bosses | medium, then open-ended |
| 4K art | yes (replace textures/sprite frames; logical size and pivots unchanged, per the plan rules) | yes, unlimited | yes |
| widescreen / 60 fps | limited: 4:3 view (Q-04 default) and 30 Hz logic, though rendering can interpolate | free choice | later |
| platform | 32-bit Windows (x86 process; D3D11 works there) | any | |
| risk | relies on the original binaries (we own the copies) | large; "feels like the original" is hard | |

## Recommendation

**A (hybrid), with C kept open.** It delivers a 4K, fullscreen, faithful game in the shortest path, across every level including ENG3 and the bosses, because nothing in the game logic has to be rewritten. The decoders and the spike carry over directly: the replacement-art pipeline (P7) works on the decoded sprite frames and texture pages, and the renderer uses the same tpage/UV/fog rules the spike proved. Language: C/C++ (x86 hook DLL + D3D11 renderer), built with MSVC and Windows components only.

For the P6 slice under A: TRAINING, its first section, played in the original game through the new renderer at 4K. It includes the Stymphalian bird (enemy), a hazard and a pickup (0x2018/0x2019), with original assets and debug overlays (state, tick, frame, pivot, collision, resource id) drawn by the renderer.

Under B the slice is the spike grown into a game: hero states and physics from `gameplay-spec.md`/`tuning.json`, collision from `level.json`, then one enemy, hazard and pickup.
