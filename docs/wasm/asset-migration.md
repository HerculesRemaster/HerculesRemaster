# WASM port — asset migration

Because the original DLLs run unchanged, they keep reading **original resources from HERCULES.FS through the PC file
slots**. Original assets are therefore **not converted**. The browser loads the user's `HERCULES.FS` byte-exact and serves
reads from it. Conversion applies only to:
- what the EXE used to decode (ESF sound, CIF and palettes are still read raw);
- what the remaster adds (4K art, music).

Legal column: per `docs/authorization-scope.md`, **nothing may be distributed** until the Disney licence is checked.
"User-supplied" means the user picks the file from their own installation.

## 1. Inventory and plan

| group | source | format (spec) | existing decoder | consumed by | web handling | loading | storage | legal |
|---|---|---|---|---|---|---|---|---|
| archive | `HERCULES.FS` 46,924,407 B | 1,024 × 12 B index + payloads (`docs/archive-format.md`) | `tools/fs/fs_index.py`, EXE `FS_*` | every PC slot | **raw, unchanged**. Index parsed by the host in C | user picks the file once; copied to OPFS; read whole into WASM memory at boot (45 MB) | OPFS `/originals/HERCULES.FS` | user-supplied |
| game code | `TITLE.DLL`, `ENG1.DLL`, `ENG3.DLL` | PE32 | host PE loader (new) | interpreter | raw. SHA-256 must match `.plan/hercules-remaster/FACTS.md` | eager (≈0.9 MB) | OPFS `/originals/` | user-supplied |
| sine table | `HERCULES.EXE` `.data` VA 0x437480, 8 KB | 4096 × s16 Q12 | — | slots 0x28c/0x290 via table +0x14 | copied out of the user's EXE at import time, or generated and checked against the EXE bytes | eager | OPFS (part of the import) | user-supplied |
| SFX samples | 559 ESF entries (6.9 MB) | `ESF\x03`, 16 B header + mono IMA-ADPCM high-nibble-first (`docs/formats/esf.md`) | `tools/decode/esf.py` (byte-exact vs game, 4 pairs) | slot 0x22c → DirectSound today | C port of the decoder in the host. PCM16 → `AudioBuffer` (11025/22050 Hz; WebAudio resamples) | on demand when a bank or SFX is created, cached per level | WASM memory → AudioBuffer | from the archive |
| Sonix banks | 23 `SFX.SFX` + 12 `MUSIC.MUS`, 452 B each | program → ESF id, volumes; MUS also holds CD segment records (`docs/formats/sonix.md`) | `tools/decode/sonix.py` | slots 0x06c–0x074 | raw, read by the host port of the slot code | on level load | archive | from the archive |
| level maps | 10 `MAP_PSX.BIN` (i2.5 ×7, i1.a ×3) | `docs/level-format.md`, `docs/full-game/eng3-format.md` | `tools/level/*` | DLLs (direct) | **raw** | DLL-driven (streamed 0x800 sectors in ENG1) | archive | from the archive |
| CIF tables | 16 `MAP_PSX.CIF` (336,384 B) | palette, 4 blend tables, shade, 31 fog rows | inside `level_decode.py` | `GFX_LoadCif` 0x68c0 / slot 0x244 → renderer | raw; the host uploads them as R8 textures | on level load | archive | from the archive |
| sprites | 109 `ANIMPSX.BIN` (`i1.4`, 15.1 MB) | `docs/formats/i-family.md` | `tools/decode/ifam.py` | DLLs → VRAM cells | **raw** | DLL-driven | archive | from the archive |
| screens | 65 TGA (5.4 MB) | colour-mapped TGA 320×256, top-down (`docs/formats/tga.md`) | `tools/decode/tga.py` | TITLE → VRAM upload | raw | DLL-driven | archive | from the archive |
| HUD / loading | `STPANEL1/2.DAT`, `LOADING.DAT` | raw 8 bpp | — | DLLs | raw | DLL-driven | archive | from the archive |
| demos | 8 `DEMO*.DAT` × 4 KB | `docs/demo-format.md` | `tools/state/demo.py` | ENG1 0x5ea0/0x6005 | raw | DLL-driven | archive | from the archive |
| unreferenced | i1.3 ×8, i1.9 ×1, blobs 618/744/746/759 | unknown | none | none seen at runtime | ignore; stays inside the archive | — | — | — |
| CD music | **missing** (CD track 2, ~34 min) | TMSF segments (`docs/music-transport.md`) | `runtime/display/cd_audio.cpp` (MF decode of `Track02.<ext>`) | slots 0x0d4–0x0dc → `AUD_CdPlaySegment` | the user supplies an audio file (any format `decodeAudioData` accepts: Ogg/Opus, MP3, FLAC, WAV, M4A). The host plays the [A, B) segment from the same TMSF rules | decoded once into an AudioBuffer (34 min stereo 44.1 kHz ≈ 360 MB as float32, **too large**) → decode **per segment** with WebCodecs `AudioDecoder` where supported, else pre-split into 15 files at import time | OPFS `/music/` | user-supplied |
| FMV | **missing** `e:\data\%d.etv` | unknown | none | slot 0x250 (TITLE) | none. Reproduce the EXE's open-fails path | — | — | — |
| registry Config | `HERC.REG` / registry, 0x48 B | `docs/full-game/runtime-launch.md` | `ddraw_proxy.c:165-243` (portable mode) | EXE 0x4ce0/0x4da0 → slots 0x224/0x240/0x248 | the same 72 bytes, stored verbatim | read at boot, written on settings change and on `pagehide` | IndexedDB `settings/config` (or OPFS file) | user data |
| saves | TITLE memory-card path (`bu00:…`) | 0x3c-byte records | — | TITLE via CRT | see I-02 (default: behave like Windows) | — | OPFS `/saves/` if enabled | user data |
| display settings | `hercules_display.ini` | `docs/fullscreen.md` + `ddraw_proxy.c:58-90` | proxy parser | proxy | JSON with the same keys and meanings where they apply (§4) | boot | localStorage (convenience only) | — |

## 2. Remaster art (4K overrides)

**Today:**
- Content-addressed PNGs: key = FNV-1a-64 over (w, h, texels), name `<key16>_<w>x<h>.png` at 9× (`gpu_renderer.cpp:499-560`, `docs/hires-pipeline.md`).
- Families searched in order `sprite, level, ui, screen, smooth`.
- `working/game/art` has about 212k files, ≈1.1 GB. GPU memory at 9× RGBA is 3,335 MB (`pack-report.json`).
- The art is **8-bit indexed**, so the R8 GPU footprint is about ¼ of that (≈0.83 GB).
- Missing: ENG3 level art (3 pending xBRZ jobs).

**Browser constraints:**
- A practical WebGL2 texture budget is about 0.5–1 GB on desktop GPUs, and less on integrated graphics.
- OPFS is fine for 1 GB, but a first-run download of 1 GB is not.

**Plan:**
1. **Keep the key scheme unchanged.** The WebGL renderer computes the same FNV-1a key over the same texels, so the existing art and pipeline stay valid.
2. **Bundle per level** (new tool, extends `tools/hires/atlas.py`):
   - Each bundle is a `.hab` file. It holds a header and a sorted key table (key, w, h, scale, offset, length), followed by PNG payloads (indexed PNG; the browser's PNG decoder handles palette PNGs).
   - Key → level assignment comes from the existing per-level art dumps (`working/game/art/dump/index.csv`, `extracted/art_blocks`) and the manifest's per-entry level coverage (`extracted/meta/*-coverage.json`).
   - Shared keys (HUD, hero) go into a `common` bundle.
3. **Scale variants:** the bundle builder emits 9× (source) plus downscaled 6×, 4× and 3× variants. The user or auto-detection picks one by GPU memory and canvas size. A 1080p canvas gains nothing past 4×.
4. **Streaming:** on module or level change (slot 0x064 and level ids from gameState) the host requests the bundle.
   - Keys are decoded lazily when first seen in an OT, exactly as today's worker-thread loader: the original texels draw until the PNG is ready.
   - GPU textures are tracked in an LRU with a byte budget.
5. **Upload format:** R8UI textures (`texImage2D` with `RED_INTEGER/UNSIGNED_BYTE`) for indexed art; RGBA8 for future true-colour overrides. GPU block compression (KTX2/Basis) does **not** apply to indexed data, because index values can't be interpolated.
6. **Storage:** art bundles are derived from the original art and **cannot be hosted publicly** (authorization scope). For local/LAN use they are either built locally by the user's pipeline and imported into OPFS, or served from the local dev server.

## 3. Browser storage layout (OPFS)

```text
/originals/HERCULES.FS  TITLE.DLL  ENG1.DLL  ENG3.DLL  HERCULES.EXE (or exe-data.bin with the sine table)
/music/track02.<ext>    (or /music/seg_<bank>_<seq>.<ext>)
/art/<scale>/<bundle>.hab
/saves/…                 (only if I-02 decides saves persist)
manifest.json            hashes of imported files, art bundle list, import date
```

Settings (Config blob, key map, display options) go to IndexedDB, with a localStorage fallback for display options only.
Every storage access is wrapped. If OPFS is unavailable (private window), the shell keeps the files in memory for the
session and says so.

## 4. `hercules_display.ini` semantics in the browser

| ini key | browser meaning |
|---|---|
| `mode`, `monitor`, `window_scale`, `dpi_aware`, `hide_cursor` | replaced by the Fullscreen API, a canvas sized to CSS pixels × `devicePixelRatio`, and a cursor-hide CSS class |
| `scale` (integer / fit), `aspect`, `border_color`, `filter` | kept: integer-scale 4:3 with letterbox colour, or fit, or stretch; nearest filter for the resolve pass |
| `vsync`, `fps_cap` | rAF presents at the display rate. The game stays at 30 Hz; `fps_cap` only limits presents |
| `renderer` (`gpu`/soft) | `webgl2` (default); `soft` is a debug path if the EXE rasteriser is ported (optional, see backlog W3-08) |
| `art_dir`, `art_families`, `art_dump` | art bundle selection and family order; dump goes to a downloadable zip in dev mode |
| `music_volume` | GainNode value |
| `portable`, `log`, `dev` | portable is implicit (no registry); `log` → console; `dev` → overlay |

## 5. Startup memory budget (desktop, hypothesis to be measured in W7)

| item | size |
|---|---|
| WASM module + host | ≤ 2 MB |
| guest memory block | 64 MB |
| HERCULES.FS in memory | 45 MB (could be replaced by OPFS sync reads in a worker later) |
| SFX AudioBuffers (one level) | ≤ 20 MB |
| art (common + one level at 4×) | about 100–200 MB GPU, ≤ 50 MB download from OPFS |

## 6. Fidelity checks

These are run once per change, not as suites:
- Archive reads: SHA-256 of the imported file matches `FACTS.md`, and the host index parser reports the 820/204/203 counts from `manifests/fs-index-report.json`.
- ESF: the C decoder output equals `tools/decode/esf.py` for the 4 verified pairs in `analysis/traces/pairs/`.
- Art: the renderer computes the same keys as D3D11 (compare the key list of `gpu_<n>.txt` dumps for the same frame).
