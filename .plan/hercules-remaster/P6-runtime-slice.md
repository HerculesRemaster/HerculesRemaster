# P6 Runtime architecture + playable slice

Goal: choose how the remaster runs, then build a playable level section with the original assets.
Needs: P4, P5.
Outputs: `runtime/`, `docs/architecture-decision.md`, `docs/slice-definition.md`.

## Architecture

- [x] From P2/P3, summarise what the DLLs hand the EXE for drawing: sprite calls, tile lists or a framebuffer. This answers Q-08.
- [x] Optional prototype: an x86 C host (`runtime/prototypes/host32/`, MSVC vcvars32) that loads TITLE.DLL and calls `PC_DLLEngineMain` with logging stubs. Note how far it runs.
- [x] Spike: a 64-bit renderer (`runtime/prototypes/spike/`) that draws one decoded level layer and a hero frame at 3840x2160 with a fixed timestep at the measured rate.
- [x] Write `docs/architecture-decision.md`: options, limits, recommendation.
- [x] **[HUMAN]** The user picks the architecture and the language/engine (Q-03).

## Slice

- [x] Choose the slice: one level section with one enemy, one hazard and one pickup.
- [x] Build the runtime skeleton:
  - fixed timestep at the measured Hz
  - world units independent of output resolution
  - assets loaded by catalog key, with an override directory for replacement art
- [x] Render the level (layers, parallax, camera limits) with its collision.
- [x] Implement the hero's movement, jump, attack, damage and restart from `gameplay-spec.md`/`tuning.json`, with animation anchors.
- [x] Add the enemy, hazard and pickup, sound from `extracted/audio`, and keyboard + gamepad input.
- [x] Add debug overlays behind a dev flag: state, tick, frame, pivot, collision, resource ID.
- [x] **[HUMAN]** The user plays the slice beside the original and lists differences to fix.

Done when the slice plays with the original assets and feels like the original.

## Notes

- Q-08: primitive-level boundary (PSX ordering table → EXE `GFX_DrawOTag` 0xac60 → 8-bit software rasteriser). → `docs/architecture-decision.md`.
- Optional x86 host: skipped. Under the chosen hybrid route the original EXE is the host.
- Spike (`runtime/prototypes/spike`, C++/D3D11/WIC x64): TRAINING + hero run cycle at 3840×2160, 9× pillarbox, 59.9 fps, 30.1 Hz ticks, p95 17.1 ms.
- 2026-10-12: the user chose **A (hybrid, C++)**: keep the original game logic, replace the renderer in-process.
- Slice → `docs/slice-definition.md`: TRAINING x 1214–10240 (start 0 → past the birds). Enemy 0x38 bird ×5, hazard 0x36 ×5 (`HERO_Hazard36Update` ENG1 0x3e770 → `HERO_RequestDamage` 0x266a0), pickups 0x2019 ×60 / 0x2018 ×22. Under A, the hero, enemy, hazard, pickups, sound, timing and input are the original code. Gamepad = WinMM joystick (`RT_PollJoystick` EXE 0x7c50).
- Skeleton/render → `docs/gpu-renderer.md`: index pipeline at 9× in 320×240 logical units, original 30 Hz tick. Replacement art is keyed by FNV-1a of texel content (sprites, hero parts, level textures); the override dir `art/` holds indexed k× PNGs and is loaded on a worker. The dump catalog is `extracted/art_blocks/` (10,911 blocks). Test pattern on everything: 28–30 fps.
- Overlay (`dev = on`): frame, module, hero state/frame/pos/vel, last file, pivot cross, hero box (hero+0x88). Fixed a stack over-read in the trace hook (`dll_caller`, `hercules_hook.dll+0x11e3`) that crashed some runs.
- Play check 1 (user): thin lines and rectangle edges around the title characters, hero parts and birds. Cause: polygon UVs were offset by +0.5, so block edges sampled neighbouring VRAM texels; opaque polygons were also shaded and semi ones fogged. Fixed from the EXE: axis-aligned 1:1 quads → sprite copy (0xd420/0xe680/0xdd70 → 0xcab0/0xc8f0/0xc700); UV = texel at the vertex, page wrap; shade only on semi with brightness ≠ 0x80; fog only on opaque; 0xf3 clip rows. `tools/runtime/gpu_diff.py`: title 0 %, hero/HUD/pickups 0 mismatching pixels; the rest is texture sampling inside 3D level quads (hi-res by design).
- Play check 2 (user, 2026-10-12): pass. P6 done.
