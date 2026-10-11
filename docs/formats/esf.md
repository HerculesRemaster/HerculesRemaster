# ESF sound effects

559 entries (`M:\LANGUAGE\SONIX\PC_<id>.ESF`). Decoder: `tools/decode/esf.py` → `extracted/audio/<index>_PC_<id>.wav` + `.json`, survey in `extracted/meta/esf-survey.json`, coverage in `audio-coverage.json` (559 ok).

## Layout (from `AUD_CreateSfx` HERCULES.EXE 0xf3d0)

| offset | type | field | use in the game |
|---|---|---|---|
| 0 | char[4] | `ESF\x03` | compared as u32 0x03465345; otherwise fatal `PC_CreateSFX(1)` |
| 4 | u32 | pcm_bytes | decoded size. The game reads `pcm_bytes >> 2` ADPCM bytes from offset 16 |
| 8 | u16 | rate | DirectSound buffer frequency (11,025: 418 files, 22,050: 141) |
| 10 | u16 | — | 0 in every file |
| 12 | u8 | loop | stored per SFX (0xa98a18[id]); passed to `Play` as the looping flag (30 files) |
| 13 | u8 | bps8 | 1 = 8-bit DirectSound buffer (505 files), 0 = 16-bit (54) |
| 14 | u16 | — | 0 in every file |

File length is exactly `16 + floor(pcm_bytes / 4)` for all 559.

## Decoder (`AUD_DecodeAdpcm` 0x2410, called as `(src, dst = 0xa99848, pcm_bytes, &state)`)

- IMA-ADPCM, mono, **high nibble first**, `pcm_bytes / 2` samples.
- The state starts at predictor 0, step index 0 (`state = 0` at the call 0xf5f2).
- Index table (u32 ×16) at EXE 0x435e08; step table (u32 ×89) at 0x435e48. Index is clamped to 0..88, predictor to −32768..32767.
- `diff = step>>3 (+step if b2) (+step>>1 if b1) (+step>>2 if b0)`, negated if b3.
- With an odd sample count the last sample reads one nibble past the data; the game takes the next byte in its buffer, and the tool uses 0.
- Output to DirectSound: bps8 = 0 copies s16. bps8 = 1 writes `0x7f - (sample >> 8)` per byte (the game's own conversion). The WAVs keep the full 16-bit decode.

## Runtime check

`analysis/traces/pairs/{0035,0078,0357,0398}`: the game's decoder output equals the tool's decode of the archive payload byte for byte (the tool's decoder is shared by `tools/trace/collect_pairs.py`).

## Ids

ESF `<id>` is the value in the Sonix banks (`sonix.md`): `AUD_CreateSfx(id)` formats `m:\language\sonix\pc_%ld.esf`. Ids ≥ 1000 get a different DirectSound flag set (`id < 1000 ? 0xe2 : 0xe8` in 0xf3d0). The game keeps four voices per SFX (`DuplicateSoundBuffer`).
