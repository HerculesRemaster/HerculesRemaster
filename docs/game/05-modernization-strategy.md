# 05. Modernization Strategy

Goal: turn the 1997 2D DirectDraw game into a modern-computer-graphics game.

## Define "modern" first (decision needed)

| Level | Description | Effort |
|-------|-------------|--------|
| A. Faithful remaster | Same 2D gameplay, but GPU rendering, HD/4K scaling, filters, widescreen, controller, modern audio | Low-Medium |
| B. HD-2D | A plus new high-res/redrawn art, lighting, particles, parallax layers, post-processing | Medium-High |
| C. Full 3D/2.5D reimagining | New models, camera, animation, same level design | High |

Recommendation: **ship A first as the foundation, then grow into B.** A produces a playable result early, and proves the asset pipeline that B and C also need.

## Approach options

### Option 1: Wrap / re-host the original binaries
Keep `HERCULES.EXE` and DLLs; replace `ddraw.dll` with a wrapper (dgVoodoo2, cnc-ddraw, or a custom one that renders through Direct3D 11/Vulkan).
- Pros: days, not months; plays exactly like the original.
- Cons: limited to scaling/filters; no real new art; no widescreen; still a 32-bit Win95-era binary; needs a registry key.
- Use it as: quick win, plus the **behavioural reference** for verification.

### Option 2: Custom host for the original DLLs
Write a new 32-bit loader that loads `ENG1/ENG3/TITLE.DLL`, supplies the callbacks the EXE normally supplies (file read, blit, sound, input), and renders with a modern API.
- Needs the DLL interface reverse-engineered (they import only kernel32/user32, so the interface is narrow).
- Pros: original logic, fully controllable rendering and input; a path to enhanced graphics layered on the original game state.
- Cons: reverse-engineering risk, 32-bit process (a bridge is needed for a 64-bit engine).

### Option 3: Clean-room rebuild in a modern engine (recommended long-term)
Re-implement gameplay in Unity, Godot 4, or a custom C++/Rust (SDL3 + wgpu/bgfx) engine. Extract the data (levels, collision, animation timings) and re-author the art.
- Pros: full freedom, cross-platform, 64-bit, controller, mod support, modern graphics (PBR 2.5D, lighting, shaders).
- Cons: most work; needs the game rules (physics numbers, enemy AI, level scripts) recovered from DLLs and playthrough videos.
- Hybrid: use Option 1 to capture ground truth, Option 2 insight to read the data, then Option 3.

## Recommended path

1. Phase 0: run the original, capture reference video (Option 1).
2. Extract all assets from `HERCULES.FS` (doc 08).
3. Decide A/B/C scope with the extracted assets in hand.
4. Build the new engine (Option 3) using Godot 4 for speed, unless the team prefers Unity or custom C++.
5. Re-author or AI-upscale art, replace audio, compare against reference video continuously.

## Engine choice guide

| Engine | Good for | Notes |
|--------|----------|-------|
| Godot 4 | 2D/2.5D, free, small, GDScript/C# | Best fit for Level A/B. Strong 2D lighting and shader support. |
| Unity | 2D/3D, large ecosystem | Heavier; licence terms to check. |
| Unreal 5 | High-fidelity 3D (Level C) | Overkill for a 2D platformer. |
| SDL3 + wgpu/bgfx (custom) | Maximum control, tiny binary | Most work; best if you want to run original data formats directly. |

## Target architecture (Option 3)

```
 game/
  assets_raw/     (never committed) extracted originals from HERCULES.FS
  assets/         converted: PNG atlases, JSON animations, OGG/WAV audio, tilemaps (TMX/JSON)
  tools/          fs_extract, decoders, atlas packer, validator
  engine/         renderer (GPU sprite batch + lighting), input (kb/gamepad), audio, scene/state
  gameplay/       hero controller, enemies, bosses, items, level scripts
  ui/             title, menus, tally screen, settings (resolution, keybinds)
  tests/          replay tests comparing positions/frames to reference captures
```

Rendering design: sprite batcher with texture atlases; logical resolution 640x480 or 16:9 canvas with extended camera; parallax layers; normal-mapped sprites + dynamic lights; bloom/colour grading; selectable CRT/pixel-perfect filter. Fixed-timestep logic (e.g. 30 or 60 Hz, matching the original once measured) decoupled from render rate.

## Quality gate

Each level must pass: layout matches reference video, hero move set and jump arcs match within tolerance, all enemies/items present, collisions equal, 60 fps at 1080p on mid-range hardware, controller + keyboard work.
