# P4 Asset decoders: audio, images, animation

Goal: decode every asset family from rules found in the game's code, and export PNG/WAV plus metadata.
Needs: P1 and P2. Animation also needs P3.
Outputs:
- `tools/decode/`, `tools/viewer/`
- `extracted/{audio,images,sprites}/`, `extracted/meta/*-coverage.json`
- `docs/formats/`

## Audio (ESF, Sonix, music)

- [x] Survey the 559 ESF headers: size field vs payload length, rates, loop/bps8 flags, outliers.
- [x] In Ghidra, `search_byte_patterns` for the IMA step table (`07 00 08 00 09 00 0A 00 0B 00 0C 00 0D 00 0E 00 10 00`) and follow its xrefs to the decoder. Note the nibble order, predictor/step init and clamping (`AUD_*`).
- [x] Write `tools/decode/esf.py` (16-bit PCM WAV + sidecar JSON) and batch-decode to `extracted/audio/`. Write `extracted/meta/audio-coverage.json`.
- [x] Classify the Sonix `.SFX/.MUS/.BIN` banks through the `LoadLevelSonix` / `vabid` consumers. Find the music transport from the `mciSendStringA` strings. Write `docs/formats/sonix.md` and `docs/music-transport.md`.
- [x] **[HUMAN]** Spot-listen to a few WAVs for pitch, speed and loops.

## Images + palettes

- [x] Survey the TGA headers (65, colour-mapped type 1), the i-family tags and offset tables (i1.4/i1.3/i2.5/i1.a/i1.9), CIF and BIN, grouped by name directory.
- [x] Write `tools/decode/tga.py`. It outputs an indexed PNG, palette JSON and an RGBA preview, and handles orientation (descriptor bit 5) and transparency.
- [x] In Ghidra, find the consumers of the `i1.4`/`i2.5` tags (`search_byte_patterns 69 31 2E 34`, xrefs, decompile). Recover the header, offset table, sub-block types, compression, palette selection and transparency (`GFX_*`).
- [x] Write `tools/decode/ifam.py` and `palette.py`. Raise UnsupportedVariant for unknown variants and preserve unknown fields.
- [x] Write `tools/viewer/contact_sheet.py` and render sheets per family to `extracted/images/_sheets/`.
- [x] If P3 pairs exist, compare the decoded output with the runtime buffer (`tools/decode/compare_pairs.py`).

## Animation (needs P3)

- [x] List the candidate animation tables per character from the P3 frame requests and Ghidra: `HC_*` path strings, frame-table reads (`ANIM_*`).
- [x] Recover sequences, order, durations (in original ticks), pivots, mirroring, draw order and hitboxes. Write `tools/decode/anim.py` → `extracted/sprites/<entity>/anim.json`.
- [x] Build `tools/viewer/anim_viewer/`, a static HTML viewer: original tick rate, pivot/hitbox overlay, loops.
- [x] Decode all hero sequences (idle, run, jump, attack, damage, death).

Batch decoders print totals and failures only (a per-family ok/unsupported/error count). Run them with `rtk err`, and inspect `*-coverage.json` with `rtk json -d 2`.

Done when audio, images and hero animations decode, and `docs/formats/{esf,tga,i-family,animation}.md` describe the rules with RVAs.

## Notes

- ESF: 559/559 decoded (`tools/decode/esf.py`). Header confirmed from `AUD_CreateSfx` 0xf3d0; file = 16 + floor(pcm_bytes/4). The decoder `AUD_DecodeAdpcm` 0x2410 was found via the P3 trace: its tables are u32 (0x435e08 index, 0x435e48 step), so the u16 byte pattern can't match. The decode equals the runtime pairs byte for byte. → `docs/formats/esf.md`.
- Sonix: SFX.SFX/MUSIC.MUS = 0x44 header + s16 ESF id[128] + s8 volume[128]; MUS has 5 CD-segment records. All music is CD track 2 time ranges (MCI tmsf). → `tools/decode/sonix.py`, `docs/formats/sonix.md`, `docs/music-transport.md`.
- TGA: 65 files, all type 1 / 256×24-bit palette / 320×256 8bpp / descriptor 0; the game uploads rows top-down from offset 786 (decode = runtime upload). 64 ok + 1 truncated (LEV8). → `docs/formats/tga.md`.
- i1.4 sprite banks: format from ENG1 0x3cd0 (frame table, palettes), 0x4ca0 (part layout + byte RLE), 0x4010 (draw quads), 0x5780 (CLUT). 107/109 banks, 10,552 frames decoded, visually correct; 503/601 are another variant (UnsupportedVariant). Map tags (i2.5/i1.a/i1.3/i1.9) belong to P5 (levels). → `docs/formats/i-family.md`, contact sheets `extracted/images/_sheets/`.
- compare_pairs: 4 ESF + 2 TGA decode exactly like the game's buffers.
- Animation is code-driven (frame numbers set by entity routines, ENG1 0x1e320 …). anim.json per bank from runtime traces (clips, ticks, transitions, loops, sequences) plus visual labels; viewer.html per bank. Hercules: run, idle, jump, flips, attacks, damage, throws, death. → `docs/formats/animation.md`.

- 2026-10-12: user spot-listen: sounds right.
