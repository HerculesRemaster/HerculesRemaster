# GPU renderer (hybrid route)

`runtime/display/gpu_renderer.cpp` is C++ D3D11, built into the x86 proxy `ddraw.dll` by `runtime/display/build.bat`. It is enabled with `renderer = gpu` in `hercules_display.ini`, or with env `HERC_RENDERER=gpu`. The original EXE and DLLs still run unchanged; the renderer only replaces what reaches the screen.

## Hooks (`ddraw_proxy.c`)

| EXE | hook | what |
|---|---|---|
| 0x40ac60 `GFX_DrawOTag` | `h_DrawOTag` | `gpu_draw_ot(ot)` walks the ordering table into vertex runs, unless `g_skipRender` 0xba01fc is set (catch-up tick). Then the original rasteriser runs, so the 8-bit frame and all game state stay exact |
| StretchDIBits (IAT) | `h_StretchDIBits` | `gpu_present(hwnd)` draws and presents instead of GDI; falls back to GDI if D3D11 fails |
| 0x405130 `FS_Open` | `h_FsOpen` (dev only) | last archive file name and index for the overlay |

Detours check the original bytes first. They chain safely when the hook DLL has already patched the same function with a `JMP`.

## Pipeline

1. **Indices.** Primitives are drawn into a persistent `R8_UINT` target of (320·s)×(240·s), where s = min(w/320, h/240) (9 at 3840×2160). Coordinates stay in the game's 320×240 units, so world and screen logic do not depend on the output size.
2. **Resolve.** Indices go through the current palette into the centred viewport, which is pillarboxed.
3. **Present.** It uses a FLIP_DISCARD swap chain with vsync. Game timing stays the EXE's 30 Hz `RT_Tick`, one present per tick.

The 8-bit rules match the EXE rasteriser:

| rule | EXE |
|---|---|
| primitives 0x24 FT3, 0x2c FT4, 0x3c GT4, 0x64 SPRT (w,h at +16), 0x74 SPRT_8, 0x7c SPRT_16; 0xe1 draw mode; code bit 1 = semi-transparent | `GFX_DrawOTag` 0xac60 |
| VRAM at 0x96e6e0: 8 columns × 256-byte rows per half; second MB 0xa6e6e0 when `tpage & 0x180 == 0x180` | upload 0xa710 |
| texel 0 = transparent; sprites copy texel (u+i, v+j) to pixel (x+i, y+j), linear in the page (u carries into v), blend when semi | 0xcab0, 0xc8f0 |
| an axis-aligned quad whose UV rect has the same size (not mirrored) is drawn by the sprite copy: opaque raw, semi blend, semi + brightness ≠ 0x80 shade + blend | 0xd420 → 0xcab0, 0xe680 → 0xc8f0, 0xdd70 → 0xc700 |
| polygon edges start at the vertex with UV = texel (no half-texel offset); rows y0…y1−1, spans [floor(xl), floor(xr)); fetch `page[(v&255)<<8 \| (u&255)]` | 0xd420 |
| opaque polygon fog: level = (clut >> 6) − 480 (0 if > 30); fog row 0x92c390 + level·256 when the CIF fog rows differ (0x96e3e0), otherwise the texel at ((Σu)>>2, (Σv)>>2) fills the polygon (triangles count the last vertex twice). Opaque polygons are never shaded | 0xcc50, 0xc2e0 |
| semi polygon: brightness byte (+4) ≠ 0x80 → `blend[mode][shade[b][src]·256 + dst]` (shade 0x91c488), else `blend[mode][src·256 + dst]` (0x92e3d0); no fog | 0xdd70, 0xe680 |
| clip rectangle 0xf3 (y0 = byte +6, −0xf0 if > 0xef; y1 = y0 + s16 +10): polygons clip rows to [y0, y1), sprites to [0, y1); x is always [0, 320) | 0xac60 |
| clear only when 0x96e698 is set, with palette index 0x96e424 | 0x6ee0 |
| frame brightness (fades): before present every frame pixel becomes `shade[b][pixel]` when b (0x436a0c) ≠ 0x80. It rewrites the persistent frame, so the GPU applies it in place to both targets (true colour: c·b/128) | `GFX_FrameBrightness` 0x70c0 |
| palette = BITMAPINFO colours at `*(0x96e3f4) + 0x28` | |

Measured at 30.3 fps at 3840×2160. `tools/runtime/gpu_diff.py <dir> <frames>` compares `gpu_<n>.png` with the EXE's `soft_<n>.png`, sampling each block at its first sub-pixel. Title screen: 0 mismatching pixels apart from rotated effect quads. Gameplay: 0 on the hero, HUD, letters and pickups. The remaining 3–16 % is texture sampling inside perspective level quads: the EXE starts each span with the edge's UV at floor(xl), while the GPU samples the exact affine UV at each sub-pixel (intended at 9×).

## Targets (true colour)

- **Index target** (`R16_UINT`): the palette index, with bit 8 set where a true-colour pixel was written.
- **Colour target** (`RGBA8`): always holds the pixel's colour.
- **Opaque draws** write both targets. The colour target is alpha-blended, so RGBA art gets soft edges.
- **Semi draws** read copies of both targets:
  - both pixels indexed → the EXE's `blend` table (exact);
  - otherwise → the PSX formula in colour space, weighted by the art's alpha.
- **Resolve:** flagged pixels show the colour target; the others show `palette[index]`. Content without RGBA art therefore stays identical to the EXE rules.
- **Palette:** the EXE never fades through the palette (`GFX_SetPaletteEntry` is only called by init, `GFX_LoadCif`, `GFX_LoadLevelPalette` and the video player), so true-colour pixels need no palette transform.

## Replacement art (`art_dir`, `art_dump`)

- **Key.** Each textured primitive gets a key: FNV-1a 64 of (w, h, the w×h VRAM texels it samples). Sprites use their w×h rect. Polygons use the UV bounding box; flat-filled polygons are skipped. The key follows the content, not the VRAM position, so a sprite frame keeps its key wherever ENG1's cell allocator puts it.
- **Override file.** It is `<art_dir>/<key16hex>_<w>x<h>.png` (default `working/game/art/`) at k·w × k·h, the same integer k on both axes. An 8-bit indexed PNG is used as CIF palette indices (0 = transparent, exact rules). Any other PNG is true colour (RGBA8, alpha = coverage).
- **Drawing.** The override is drawn into the same quad. Indexed art goes through the index rules; RGBA art uses the colour rules (shade c·b/128, PSX blend formulas, fog identity). Logical size, pivot and timing cannot change.
- **Loading.** A worker thread loads files. Until a file is ready, the block draws with its original texels.
- **Key cache.** Keys are cached per UV rect. A key is re-hashed only when one of its 32×32 VRAM tiles changed (per-tile dirty stamps each frame).
- **Dump.** `art_dump = on` (or env `HERC_ART_DUMP`) writes each new block once to `<art_dir>/dump/`, indexed, with the current palette. TRAINING demo + title: 10,911 blocks → `extracted/art_blocks/` (the catalog for P7).
- **Check.** A temporary test pattern (every block 4× with a white frame) drew over HUD, sky, letters, hero parts and level textures, at 28–30 fps in a 60 s demo with 10,911 overrides. Polygons sample texel centres, so half of the outermost texel of an override is not shown.

## Dev overlay (`dev = on`, env `HERC_DEV`)

It is a GDI text strip (Consolas) with:
- line 1: frame (game state +0x38), module, scale;
- line 2: hero state (0x72108), frame (+0x34), world x/y/z (+0/+4/+8), vx/vy (+0xa8/+0xb8);
- line 3: last archive file.

It also draws:
- a red pivot cross at the hero's screen position (+0xc/+0xe);
- the hero collision box (hero+0x88, `{x0,y0,z0,x1,y1,z1}` world, built by 0x2a880, tested by 0x2a7a0), placed relative to the hero's world/screen position. It fits the sprite, so the hero's plane is 1:1.

## Debug captures

- `HERC_GPU_SHOT=<dir>` + `HERC_GPU_SHOT_FRAME=<n>` writes:
  - `gpu_<n>.png` (back buffer);
  - `soft_<n>.png` (the EXE's own 320×240 frame);
  - `gpu_<n>.txt` (clear/fog flags, vertex/run counts, art counters).
- `log = on` logs the fps every few seconds, plus the module and offset of the first access violation.

## ENG3 levels (2026-10-12, full-game S7.02 / L02/L07/L09 RM.01)

Two fixes came out of the first ENG3 GPU-vs-EXE comparison (`analysis/runtime/levels/eng3_gpu/`, `HERC_GPU_SHOT` during cheat_select runs):

- **Polygon extent rule.** The EXE rasterisers (0x40d420 opaque, 0x40e680 semi, 0x40dd70, 0x40cc50 fog) draw a polygon only when its vertex extents are below 0x800 in x and y. ENG3 emits near-plane quads projected to huge coordinates (e.g. −3105…8352 × −20386…28766 in CYCLOPS). The EXE drops them; the GPU drew them over the whole screen, so roughly every other frame showed only the clear colour and the HUD (94% of pixels wrong). `Poly` now applies the same rule. The flat-colour path 0x40c2e0 is not read yet and is left unchanged.
- **Several OTs per frame.** The EXE draws every `GFX_DrawOTag` into the same frame without clearing in between, so the primitive lists now accumulate until the next present. Every captured ENG3 frame so far had one OT (`ot_calls` in `gpu_<n>.txt`).

Debug additions: `gpu_<n>.txt` has `ot_calls` and `frame_brightness`; `gpu_<n>_ot.txt` lists every polygon and sprite in OT order (code, colour, bounding box, tpage, semi, clip rows).

Results after the fixes (pixels that differ, 3× windowed):

| level | frames | range | what remains |
|---|---|---|---|
| GAUNTLET | 17 | 12–27 % | texture sampling inside perspective level quads (the span-start UV rule above) |
| CYCLOPS | 9 | 35–41 % | the same; sprites, Cyclops, hero and HUD match |
| TORMENT | 17 | 24–44 % | the same (dense textured cave geometry) |

ENG3 primitive codes seen: 0x2c/0x2e (textured quads, opaque/semi), 0x64 (sprites), 0xe1, 0xf3. There are no Gouraud or flat-untextured codes in these scenes.

### Performance at 9× (Q-05, 2026-10-12)

Each semi-transparent run used to copy both full render targets (index and colour, 2880×2160 at 9×) so the shader could read the destination. The renderer now copies only the run's screen box (vertex bounds + 1 texel, clipped to the run's clip rows). The output is pixel-identical: 17 TORMENT frames compared before and after (`analysis/runtime/levels/eng3_gpu/TORMENT_fix` vs `TORMENT_region3`).

In-level frames only (vertices > 2000), `HERC_PERF` + `HERC_GPU_SCALE=9`, entry through cheat_select:

| level | p95 before | p50 after | p95 after | frames over 16.67 ms after |
|---|---|---|---|---|
| GAUNTLET | 21.60 ms | 7.43 ms | 10.55 ms | 7 of 1240 |
| CYCLOPS | 17.54 ms | 6.42 ms | 8.54 ms | 5 of 942 |
| TORMENT | 24.59 ms | 6.64 ms | 14.37 ms | 24 of 1125 |

CSV files: `analysis/runtime/perf/eng3_<LEVEL>.csv` (before) and `eng3_<LEVEL>_region.csv` (after). Both runs have isolated spikes (max 27–96 ms). Their cause has not been analysed yet.

ENG1 levels with the same renderer, in-level frames (after the game tick resets at the level load), 9×, cheat_select entry (`analysis/runtime/perf/eng1_<LEVEL>_region.csv`): p95 TRAINING 6.6 ms, FOREST 8.6, THEBES 7.2, HYDRA 5.9, MEDUSA 9.0, TITANS 7.8, VORTEX 8.9. All ten levels are within Q-05; the per-level table is generated in `docs/release/content-matrix.md`.
