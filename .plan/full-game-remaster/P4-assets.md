# P4 All-level asset extraction and decoders

Plan status: **deferred for later implementation**. Registry/scenario documents live in this plan's `reference/`; new run/export/specification outputs use the isolated paths in [START-HERE.md](START-HERE.md). Existing lean tools/outputs below are inherited inputs.

Goal: extract/decode assets for **all ten levels and their scenes**: shared and level-specific graphics, palettes, animations, enemies/bosses/NPCs, effects, UI, audio and transitions. Derive rules from code and runtime buffers; export lossless assets plus source/scene metadata.
Needs: P1, P2 and incremental P3 evidence by family/scene. P2F remains prerequisite to new runtime sampling. P4 completion requires P3's full-game inventory and scene coverage.
Outputs:
- `tools/decode/`, `tools/viewer/`
- `extracted/{audio,images,sprites}/`, `extracted/meta/*-coverage.json`
- `docs/formats/`
- Future `extracted/meta/full-game/{level-assets,scene-asset-coverage,image-coverage,animation-coverage,level-asset-coverage}.json`
- Future `extracted/full-game/{images,sprites}/`, `docs/full-game/asset-gaps.md`

Existing tasks, their checked states, decoders and notes are retained. [Task identities](TASK-IDENTITIES.md) preserve equivalent archived IDs. Additions use `P4.ALL.*`; a decoded hero/demo is an intermediate milestone.

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

## All-level extraction tasks

- [ ] `P4.ALL.01` Extend the completed 559/559 ESF decoder and Sonix-bank classification with scenario associations: dialogue, combat, rescue, hazards, bosses, flight/chase, UI and completion. Bind bank/program/ESF IDs to sound-play events and channels across both engines. Preserve the completed decode work.
- [ ] `P4.ALL.02` Map music segment requests to every level/scene, retaining the verified MCI/CD-audio transport and parsed bank metadata. External CD tracks are absent: mark them `missing_source_media`; a `.MUS` segment table is not extracted PCM music.
- [ ] `P4.ALL.03` Extend image surveys/decoders to every observed variant: `i1.4`, `i1.3`, `i2.5`, `i1.a`, `i1.9`, non-i animation headers and CIF subtypes. Include all seven `i2.5` gameplay maps and all three `i1.a` maps (GAUNTLET/CYCLOPS/TORMENT). Recover unknown rules from direct code consumers/runtime pairs; retain UnsupportedVariant and unknown fields.
- [ ] `P4.ALL.04` Export all-level backgrounds/tiles, props, breakables, pickups, HUD/menu glyphs, portraits, effects and transition screens. Preserve indexed pixels, scene palette/CLUT, transparency, logical geometry and draw metadata. Recover palette animation, layer/foreground occlusion and flight/chase variants where observed.
- [ ] `P4.ALL.05` Extend animation beyond hero idle/run/jump/attack/damage/death to every observed enemy, boss, NPC, mount, hazard, effect and special movement form. Recover sequence order/duration in original ticks, pivots/attachments, mirroring, draw order and code-derived hitboxes; image bounds do not establish hitboxes.
- [ ] `P4.ALL.06` Export encounter phase/state → sequence → frame → sound/effect links for Nessus, Hydra, Medusa, Cyclops, Titan encounters and final-level actors/events. Preserve per-difficulty differences and distinguish inferred phase names from observed states.
- [ ] `P4.ALL.07` Compare offline decode/palette output against P3 buffers for **both engines** and every consumed family/variant, including streamed chunks and previously unnamed assets. Cell-packed VRAM uploads are not necessarily complete sprites. Render contact sheets and animation previews per level/scene/entity, with source IDs and palettes.
- [ ] `P4.ALL.08` Build `level-assets.json` and `scene-asset-coverage.json` from the P1 catalog, P3 scenarios/usage and decoder coverage. Each association records archive hash/index/ID/name/range, family/variant, decoder/version, output identity, consumer program+RVA, palette/animation references, difficulty and evidence ticks. Track shared/resident, level, scene and difficulty usage as many-to-many relationships.
- [ ] `P4.ALL.09` Reconcile **all 820 payloads**, sixteen unnamed payloads and the unresolved zero-size nonzero ID against all 1,024 index records. Keep decoded/unsupported/failed/unobserved/missing-source statuses explicit. Preserve aliases and all source identities when identical payloads are deduplicated. Unobserved does not prove unused.
- [ ] `P4.ALL.10` Resolve dynamic/unnamed resources through normalized runtime paths and direct consumers; verify name hashes before extending name candidates. Compare each scene's expected categories with observed resources, resident assets and successful exports; queue missing pairs/phases in P3 and append newly discovered scenes without changing IDs.
- [ ] `P4.ALL.11` Extend `docs/formats/{esf,tga,i-family,animation,sonix}.md` with confirmed RVAs and variants. Extend the existing catalog coverage contract; publish `docs/full-game/asset-gaps.md` with affected scenes, evidence and the exact next action. Do not replace working decoders simply to match a new output layout.

Done when every runtime-used asset across all ten levels/scenes and differing difficulties exports with metadata and matching runtime samples, all archive entries have explicit statuses, and formats are documented with RVAs. Required unsupported/failed assets block completion. Missing external CD audio remains an explicit source limitation with transport metadata and is never counted as extracted audio. The original audio/images/hero criterion remains a milestone.

## Notes

- ESF: 559/559 decoded (`tools/decode/esf.py`). Header confirmed from `AUD_CreateSfx` 0xf3d0; file = 16 + floor(pcm_bytes/4). The decoder `AUD_DecodeAdpcm` 0x2410 was found via the P3 trace: its tables are u32 (0x435e08 index, 0x435e48 step), so the u16 byte pattern can't match. The decode equals the runtime pairs byte for byte. → `docs/formats/esf.md`.
- Sonix: SFX.SFX/MUSIC.MUS = 0x44 header + s16 ESF id[128] + s8 volume[128]; MUS has 5 CD-segment records. All music is CD track 2 time ranges (MCI tmsf). → `tools/decode/sonix.py`, `docs/formats/sonix.md`, `docs/music-transport.md`.
- TGA: 65 files, all type 1 / 256×24-bit palette / 320×256 8bpp / descriptor 0; the game uploads rows top-down from offset 786 (decode = runtime upload). 64 ok + 1 truncated (LEV8). → `docs/formats/tga.md`.
- i1.4 sprite banks: format from ENG1 0x3cd0 (frame table, palettes), 0x4ca0 (part layout + byte RLE), 0x4010 (draw quads), 0x5780 (CLUT). 107/109 banks, 10,552 frames decoded, visually correct; 503/601 are another variant (UnsupportedVariant). Map tags (i2.5/i1.a/i1.3/i1.9) belong to P5 (levels). → `docs/formats/i-family.md`, contact sheets `extracted/images/_sheets/`.
- compare_pairs: 4 ESF + 2 TGA decode exactly like the game's buffers.
- Animation is code-driven (frame numbers set by entity routines, ENG1 0x1e320 …). anim.json per bank from runtime traces (clips, ticks, transitions, loops, sequences) plus visual labels; viewer.html per bank. Hercules: run, idle, jump, flips, attacks, damage, throws, death. → `docs/formats/animation.md`.

- 2026-10-12: user spot-listen: sounds right.
