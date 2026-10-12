# Resource consumers (runtime-traced)

Traced with `runtime/hooks/hercules_hook.dll` (`tools/trace/run_trace.ps1`). RVAs: HERCULES.EXE base 0x400000, DLLs 0x10000000. Traces: `analysis/traces/resource-usage.jsonl` (90 s title + demo), `analysis/traces/runs/*`. Verified pairs: `analysis/traces/pairs/<index>/` (`tools/trace/collect_pairs.py`).

## End to end: sound effect 768 (`m:\language\sonix\pc_768.esf`, archive index 357)

| step | where | what the trace shows |
|---|---|---|
| request | `title.dll+0xC440` (thunk of table slot 0x22c) during title start-up, tick 8 | DLL asks the EXE to create SFX 768 |
| path | `AUD_CreateSfx` EXE 0xf3d0 | `sprintf("m:\language\sonix\pc_%ld.esf", 768)` |
| lookup | `FS_Open` 0x5130 ← EXE+0xF442 | `FS_NormalizePath` + `FS_HashName` → id `0910F823`, binary search → index 357 (offset 46,488,145, size 1,460), handle 358 |
| header | `FS_Read` ← EXE+0xF45E | 16 bytes → 0xa99838: magic `ESF\x03` (checked: `PC_CreateSFX(1)`), PCM bytes 5,778, rate 22,050, loop 0, bps8 1 |
| payload | `FS_Read` ← EXE+0xF584 | 1,444 bytes (= 5,778/4) → end of the 1 MiB ADPCM area, `0xb99847 - size/4` |
| decoder | `AUD_DecodeAdpcm` EXE 0x2410 (src, dst = 0xa99848, pcmBytes, state = 0) | IMA-ADPCM, high nibble first, index table 0x435e08, step table 0x435e48 → 2,889 s16 samples |
| consumer | DirectSound buffer (IDirectSound vtbl 0x0c CreateSoundBuffer, `Lock` 0x2c / `Unlock` 0x4c), 4 voices per SFX (`DuplicateSoundBuffer` 0x14) | bps8 = 0 copies s16; bps8 = 1 writes 8-bit as `0x7f - (sample >> 8)` |
| play | `AUD_StartVoice` 0xf940 (table slots 0xb0 / 0x388 → EXE+0xBFBC) | e.g. tick 201 `sfx 806` channel 1 from `title.dll+0xCC39`; volume/pan + `Play` in 0xfbf0 |

Verification (`analysis/traces/pairs/0357/`): `adpcm_in.bin` equals payload bytes 16..; `pcm_out.s16le` equals an independent Python port of 0x2410 run on the archive payload (all four captured ESF pairs: 35, 78, 357, 398). With an odd sample count, the last sample reads one nibble past the data. That nibble comes from the next byte in the game's buffer and is excluded from the comparison.

## End to end: a full-screen TGA (`M:\GRAFIX\MOVIE\SCREENS\MENUb.TGA`, index 584)

`title.dll+0xC40A` → `RT_LoadFile` (slot 0x68, EXE 0x3b50, "Loading %s") → `RT_PCopen`/`FS_Open` → `FS_Read` of the whole file. The upload is from EXE+0xA67A to VRAM: `GFX upload` 0xa710 (slot 0x204), rect x = 320, y = 256, 160 × 256 16-bit units, source = file offset **786** = 18-byte TGA header + 768-byte palette. The pixels go to VRAM unchanged: 8 bpp indexed 320 × 256. The palette (BGR) is at file bytes 18..785. Verified in `pairs/0584` (`preview.png`) and `pairs/0545` (`t015.TGA`). `LOADING.DAT` (466) is uploaded raw from offset 0 as 80 × 200 units.

## Sprite frames: one character during the demo (ENG1, TRAINING)

- Banks are loaded at the level start (tick 1134) by `RT_LoadFile` from `P:\SRC1\…\ANIMPSX.BIN` (P: = `M:\GRAFIX\CHOP\`). Hercules = index 560, 499,026 bytes.
- Bank table: ENG1 0x5cee0, 0x14 bytes per bank, laid out as `{char *name, u16 frames, …, +0xc data}`. Bank 1 = `P:\SRC1\HERO\HERCULES`, 593 frames.
- `ANIMPSX.BIN` = magic `i1.4`, u16 frame count (593), u16 10, then u32 frame entries `offset << 8 | flags`. Offsets are relative to file + 4, and frame *n* uses entry *n*.
- The loaded bank block is `[8-byte header][count × u32 per-frame VRAM cache state][file]`. Trace check: bank data 0x82ED74 + 8 + 593·4 = 0x82F6C0 = file buffer 0x82F6BC + 4.
- ENG1 0x3cd0 draws an object's current frame. Object fields: `+0x22` bank, `+0x34` frame, `+0x0c/+0x0e` x/y. On a cache miss, 0x4ca0 allocates 32-px VRAM cells and unpacks the frame into staging 0x72870. 0x50d0 uploads the queued cells (`eng1.dll+0x2C800` → slot 0x204).

Hercules' consecutive frame requests (`analysis/traces/runs/demo_anim`, `"ev":"anim"`, object 0x90FF80; tick, frame, x, y):

```
1154 f2 (130,181)  1155 f3  1156 f10 (137,181)  1158 f11  1160 f12  1162 f13  1164 f14  1166 f15 (161,181)
1168 f4 (163,181)  1170 f5  1172 f6  1174 f7  1176 f8
1177 f85 (155,181) 1180 f86 (160,153) 1183 f87 (163,146) 1186 f88 (163,149) 1188 f89 (163,156)
1190 f90 (155,165) 1192 f91 (145,173) 1194 f92 (138,180) 1196 f93 (134,183) 1198 f90 (130,185)
1199 f383 (136,193) 1202 f384 … 1222 f391 (139,156)
1235 f18 (138,184) 1238 f19 … 1250 f23 (130,181)   1252 f2   1253 f3 …
```

A frame changes every 1–3 ticks (33–100 ms). Frames 85–93 follow a jump arc (y 181 → 146 → 185). The decode of the frame data itself (`i1.4` frame records) is P4 work.

## Who opens what (demo trace)

| caller (DLL return address) | resources |
|---|---|
| `title.dll+0xC40A` / `+0xC2BF` / `+0x3431` (RT_LoadFile thunks) | `LOADING.DAT`, `MENUb.TGA`, slideshow `t0xx.TGA` (one per ~2 s), `title\menu1\ANIMPSX.BIN` |
| `title.dll+0xC420` / `+0xC440` | `q:\OPTIONS\MUSIC.MUS`, `SFX.SFX`, ESF banks (`pc_768.esf` …) |
| `title.dll+0x8091` | `o:\title\mainmenu\map_psx.cif` |
| `eng1.dll+0x2C3BB` / `+0x2C3AA` / `+0x2C300` | `q:\TRAINING\MUSIC.MUS`/`SFX.SFX`, `q:\HERC_TRA\SFX.SFX`, `O:\TRAINING\MAP_PSX.BIN`, `stpanel1.dat`, `M:\DEMO\DEMO1_1.DAT`, `o:\training\map_psx.cif`, character `ANIMPSX.BIN` banks |
| `eng1.dll+0x204D5` | streaming reads of `MAP_PSX.BIN` from offset 1,105,920 on (10 KiB chunks) |
| `eng1.dll+0x30EA1` / `+0x30E71` / `+0x55D0` | banks loaded on demand during play (`PHIL\REACT`, `PHIL\CLIMB`, `XTRA_DB1\LETTER*`, `ALIEN1\GENERIC1`) |

OS level: every `FS_Read` byte range is covered by a CRT `ReadFile` of `hercules.fs` (EXE+0x13FBC). `tools/trace/map_reads.py` maps them to 253 archive entries, with 0 uncovered.
