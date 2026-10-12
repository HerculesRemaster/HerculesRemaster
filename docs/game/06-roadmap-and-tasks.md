# 06. Roadmap and Task List

Check items off as done. Each phase has an exit criterion.

## Phase 0: Preserve and baseline
- [ ] Copy the folder to a read-only backup; generate `SHA256SUMS` for every file
- [ ] `git init` the working project (original data excluded via `.gitignore`)
- [ ] Run the game on this machine (SETUP.BAT as admin, then HERCULES.EXE); record result in doc 03
- [ ] Try compatibility mode and a ddraw wrapper (dgVoodoo2/cnc-ddraw) if it fails
- [ ] Optional: set up a Windows 98/XP VM for a true reference
- [ ] Play every level; **record video + notes** (level list, enemies, bosses, items, secrets, controls, score rules)
- [ ] Measure native frame rate and internal resolution
- [ ] Remove or ignore unrelated files (`Icons/`, `unins000.dat`)

**Exit:** the game runs; full reference footage exists; level/enemy checklist written.

## Phase 1: Asset extraction
- [ ] Write `fs_extract.py` (index = 1,024 x {id, offset, size}); dump to `assets_raw/` + `manifest.json`
- [ ] Entropy and magic report per entry
- [ ] Decode `i*.*` family (offset tables first)
- [ ] Decode `ESF\x03` sprite/sound format (559 entries)
- [ ] Identify palettes, then render everything to PNG
- [ ] Identify sound banks (VAB-like), export WAV
- [ ] Find text strings and level data
- [ ] Build an **asset browser** (contact sheet) so entries can be labelled by eye
- [ ] Map IDs to meaning (hero, enemies, tiles, backgrounds, UI); record in `asset-catalog.md`

**Exit:** all graphics and sounds exported to standard formats with a catalog.

## Phase 2: Reverse-engineer the logic
- [ ] Disassemble `HERCULES.EXE` (Ghidra/IDA Free): resource lookup (ID scheme), DLL load, callback table
- [ ] Document the EXE<->DLL interface (exports, callbacks)
- [ ] Disassemble ENG1/ENG3: hero state machine (walk, jump, attack, throw, die), physics constants, enemy AI, collision
- [ ] Decode level format (tilemap, collision layers, spawn tables, triggers)
- [ ] Decode `Config` registry blob
- [ ] Extract tuning constants into a data file (gravity, speeds, hit points, timers)

**Exit:** rules document + level data in an open format (JSON/TMX).

## Phase 3: Quick-win modernization (Level A, optional bridge)
- [ ] Wrapper-based GPU scaling, windowed/borderless, correct timing
- [ ] Replace registry dependency with a launcher that sets it up per user (HKCU) or a patch if feasible

**Exit:** the original plays smoothly in HD on Windows 11.

## Phase 4: New engine foundation
- [ ] Choose engine (Godot 4 recommended); create repo structure (doc 05)
- [ ] Renderer: sprite batch, atlas loading, camera, parallax, fixed timestep
- [ ] Input: keyboard (default arrows + Z/X/C/D/S), gamepad, rebinding
- [ ] Audio: SFX playback, music streaming, volume settings
- [ ] Load converted levels; render level 1 with original assets

**Exit:** level 1 is visible and scrollable at a stable 60 fps.

## Phase 5: Gameplay port
- [ ] Hero controller matching reference footage
- [ ] Enemy types, projectiles, hazards, pickups
- [ ] Boss fights
- [ ] Level logic: triggers, checkpoints, restart level, win/lose
- [ ] Score, lives, health, tally screen
- [ ] Title/menu/options (resolution, controls, audio)
- [ ] Replay/regression tests against reference captures

**Exit:** all levels playable start to finish, parity with the original.

## Phase 6: Graphics upgrade (Level B/C)
- [ ] Decide art route: AI/manual upscale, hand redraw, or new 3D models
- [ ] Layered parallax backgrounds, 16:9/21:9 camera
- [ ] 2D dynamic lighting, normal maps, particles, post-processing (bloom, colour grade)
- [ ] Animation smoothing / extra frames
- [ ] Optional: 3D hero and enemies on a 2.5D track

**Exit:** visual target approved, consistent style across all levels.

## Phase 7: Audio, polish, release
- [ ] Original-quality or new music (not in this copy: compose or license)
- [ ] Controller UX, settings, save/progress
- [ ] Performance pass; test on low/mid/high hardware
- [ ] Packaging and installer; crash reporting
- [ ] Legal review before any public release (doc 07)

## Suggested order of first 10 actions
1. Backup + hashes. 2. Run the game and record all levels. 3. `fs_extract.py`. 4. Contact sheet of entries. 5. Open EXE in Ghidra, find the `.fs` loader. 6. Decode `i1.4`. 7. Decode `ESF3`. 8. Choose engine. 9. Level 1 prototype with real assets. 10. Review scope A/B/C.
