# P6 Runtime architecture + playable slice

Goal: choose how the remaster runs, then build a playable level section with the original assets.
Needs: P4, P5 with their all-ten-level reference/extraction/specification gates met. The slice is an implementation milestone; it does not reduce P3–P5 to one-level discovery.
Outputs: `runtime/`, `docs/architecture-decision.md`, `docs/slice-definition.md`.

## Architecture

- [ ] From P2/P3, summarise what the DLLs hand the EXE for drawing: sprite calls, tile lists or a framebuffer. This answers Q-08.
- [ ] Optional prototype: an x86 C host (`runtime/prototypes/host32/`, MSVC vcvars32) that loads TITLE.DLL and calls `PC_DLLEngineMain` with logging stubs. Note how far it runs.
- [ ] Spike: a 64-bit renderer (`runtime/prototypes/spike/`) that draws one decoded level layer and a hero frame at 3840x2160 with a fixed timestep at the measured rate.
- [ ] Write `docs/architecture-decision.md`: options, limits, recommendation.
- [ ] **[HUMAN]** The user picks the architecture and the language/engine (Q-03).

## Slice

- [ ] Choose the slice: one level section with one enemy, one hazard and one pickup.
- [ ] Build the runtime skeleton:
  - fixed timestep at the measured Hz
  - world units independent of output resolution
  - assets loaded by catalog key, with an override directory for replacement art
- [ ] Render the level (layers, parallax, camera limits) with its collision.
- [ ] Implement the hero's movement, jump, attack, damage and restart from `gameplay-spec.md`/`tuning.json`, with animation anchors.
- [ ] Add the enemy, hazard and pickup, sound from `extracted/audio`, and keyboard + gamepad input.
- [ ] Add debug overlays behind a dev flag: state, tick, frame, pivot, collision, resource ID.
- [ ] **[HUMAN]** The user plays the slice beside the original and lists differences to fix.

Done when the slice plays with the original assets and feels like the original.

## Notes
