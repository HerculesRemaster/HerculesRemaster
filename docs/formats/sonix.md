# Sonix banks: SFX.SFX and MUSIC.MUS

35 banks (23 `SFX.SFX`, 12 `MUSIC.MUS`) under `M:\LANGUAGE\SONIX\<LEVEL>\`, opened as `q:\<LEVEL>\…` (`q:` → `M:\LANGUAGE\SONIX\`; directory table EXE 0x435178). Decoder: `tools/decode/sonix.py` → `extracted/meta/sonix.json`.

These are the PlayStation VAB/SEQ banks reduced to lookup tables. The PC game plays ESF files through DirectSound, and music from CD audio.

## Layout (both files, 452 bytes)

| offset | type | meaning | code |
|---|---|---|---|
| 0x00 | u32 | body offset (0 in every file) | `AUD_LoadSfx` 0x1330 / `AUD_LoadMusic` 0x1110 → `0xc090` ("down load VAB") |
| 0x04 | u32 | head offset (0 in every file) | → `0xc040` ("open VAB": registers the head in slot table 0xa8e790[16]) |
| 0x08–0x1b | u32 ×5 | MUSIC.MUS only: offsets of 5 sequence records (0x1c, 0x24, 0x2c, 0x34, 0x3c) | stored in game state +0x24c… |
| head+0x44 | s16 ×128 | ESF id per program (0 = none) | `0xc090` calls `AUD_CreateSfx(id)` for every non-zero id; `0xbe60` returns it as the tone's FX id |
| head+0x144 | s8 ×128 | program volume | `0xbec0`, used by the play call |

Loading (`AUD_LoadLevelSonix` 0x16b0, table slot 0x6c): the level's byte list sets the music/SFX volume (`0xff, music, sfx`), then one `MUSIC.MUS` (vab slot 1), then one `SFX.SFX` per byte until 0xff (slots 2…; 'c' is skipped). The FX id → (vab, program) map is built in game state +0x2f0 (3 bytes per FX id). "Two or more sound effects have been assigned the same attribute number" is logged on clashes.

Playing (table slot 0x388 → `0xbf20(vab, program, volL, volR)`): `AUD_StartVoice(esf_id[program])`. Then volume/pan via 0xc160 with `((v · vol)² >> 14) · 0x81 >> 7` for each side.

## Sequences = CD-audio segments (`docs/music-transport.md`)

A sequence record is two u32 CD positions (start, end). Each u32 is the bytes `[unused, sec, min, track]`. `AUD_StartMusic` (slot 0xd4) → `0xc250` (open) → `0xc280` (play) → `0x9e60` → MCI `play cdaudio from <tmsf> to <tmsf>`.

## Totals

896 programs, all ESF ids present in the archive. Every music segment is on track 2.
