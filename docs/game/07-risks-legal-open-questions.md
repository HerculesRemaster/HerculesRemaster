# 07. Risks, Legal Notes and Open Questions

## Legal / IP

- *Hercules* characters, story, art, and music are owned by Disney. The game was published by Disney Interactive (1997). Original developer is not named in the files; the source-path style suggests a console-port studio [Unknown].
- The `ReadMe.txt` shows this copy came from a third-party download site, not an official release. Its provenance and licence are unclear.
- Practical guidance (not legal advice):
  - Personal study, preservation, and private experiments with a copy you hold are the lowest-risk uses.
  - **Distributing a remaster, or the original assets, publicly or commercially needs Disney's permission.** Fan projects of this kind are commonly taken down.
  - Safest public deliverable: tools and engine code only (extractor, loader, engine) that require the user to supply their own game files; do not redistribute `HERCULES.FS` or extracted art.
  - If the goal is a commercial game, plan to replace all Disney IP: new characters, art, music, and story.
- Keep the repacker's website text/password out of any project files.

## Technical risks

| Risk | Impact | Mitigation |
|------|--------|-----------|
| Unknown compression in `ESF3` | Blocks sprite extraction | Reverse the decoder in the DLL/EXE; common formats (RLE/LZ) first; entropy analysis |
| ID scheme unknown | Cannot label assets | Disassemble resource lookup; correlate IDs with usage |
| Game logic only in x86 DLLs | Slow to re-create | Combine disassembly with frame-by-frame video analysis; unit-test against replays |
| Missing music | Weaker experience | Compose/license new music; check if Sonix/CD tracks exist elsewhere |
| 32-bit/DirectDraw on Windows 11 | Game may not run | ddraw wrapper, VM |
| Physics feel mismatch | "Doesn't feel right" | Extract constants; measure from video; tune with side-by-side overlays |
| Low-res source art | Blurry upscales | Redraw or AI-assisted upscale with manual cleanup; consistent style guide |
| Scope creep (A -> C) | Never finishes | Gate by phase exit criteria; ship A first |

## Open questions (need answers)

1. **Target level:** Faithful remaster (A), HD-2D (B), or 3D reimagining (C)?
2. **Engine preference:** Godot, Unity, Unreal, or custom?
3. **Platform:** Windows only, or also Linux/macOS/Steam Deck/mobile?
4. **Purpose:** personal learning, portfolio, public fan release, or commercial? (This decides the legal route.)
5. **Art route:** AI upscale, hand redraw, or new 3D models? Who makes the art?
6. **Music:** Do you have the game CD audio tracks, or should we create new music?
7. **Does the game run on this PC now?** (Doc 03 needs real results.)
8. **How many levels are there?** (Not discoverable from files yet; play-through will tell.)
9. **Is there an official `ENG2`** or was it removed? Which levels use ENG1 vs ENG3?
10. **Team and timeline:** solo or team; hobby pace or deadline?

## Known unknowns summary

ESF3 format, `i*.*` format, ID scheme, DLL interface, level format, `Config` blob layout, frame rate and native resolution, number and order of levels, whether FMV exists inside the archive, how sound banks are structured.
