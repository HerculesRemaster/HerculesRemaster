# Music transport

The PC game has no music files. Music is Red Book CD audio played through MCI (HERCULES.EXE):

| RVA | name | MCI |
|---|---|---|
| 0x9c30 | `AUD_CdReady` | `open cdaudio shareable wait`, `status cdaudio media present`, `status cdaudio ready` |
| 0x9e60 | play segment | `set cdaudio time format tmsf`, `play cdaudio from %d to %d` (start/end as MCI TMSF integers) |
| 0x9f60 | resume | the same play, continuing from the elapsed time (`timeGetTime`) after a pause |
| 0x9e10 | `AUD_CdStop` | `stop cdaudio wait` |
| 0xa120 | close | `close cdaudio wait` |

Chain: DLL → slot 0xd4 `AUD_StartMusic(vab, seq)` → `0xc250`/`0xc280` → `0x9e60(start, end)`. The positions come from the level's `MUSIC.MUS` sequence records (`formats/sonix.md`). Music plays only when the options flag 0xb9e865 is 1, and pauses when the window is inactive (`RT_UpdateActiveState` 0x5a50).

## Segments (track:min:sec, all on track 2)

| level bank | segment |
|---|---|
| OPTIONS (title/menu) | seq 1 2:03:11–2:04:25, seq 2 2:04:25–2:06:55, seq 3 2:02:18–2:03:11, seq 4 2:00:00–2:02:18 |
| TRAINING | 2:04:25–2:06:55 |
| GAUNTLET | 2:06:55–2:08:55 |
| FOREST | 2:08:55–2:12:53 |
| NESSUS | 2:12:53–2:15:29 |
| THEBES | 2:15:29–2:18:33 |
| HYDRA | 2:18:33–2:20:56 |
| MEDUSA | 2:20:56–2:22:19 |
| CYCLOPS | 2:22:19–2:25:10 |
| TITANS | 2:25:10–2:28:33 |
| TORMENT | 2:28:33–2:31:08 |
| VORTEX | 2:31:08–2:34:20 |

Unused records hold "track 0 → 2:00:00". Track 2 is one ~34-minute audio track that holds every piece back to back; track 1 is the data track.

There is no disc image and there are no audio tracks in the preserved files (Q-01). A remaster needs the music from the original disc (rip track 2 and cut it at these times) or a replacement.

## Remaster: CD audio emulation (no disc)

`runtime/display/cd_audio.cpp` (in the proxy `ddraw.dll`). If `music\Track02.<ext>` (or `Track02.<ext>`) exists next to `HERCULES.EXE`, the proxy IAT-hooks `WINMM!mciSendStringA` and answers every `cdaudio` command itself; other MCI strings and the no-file case go to the real MCI unchanged. Only the strings above are referenced by the EXE (the `status cdaudio length/type track` and `number of tracks` strings at 0x43a5b4–0x43a608 have no references).

- `status cdaudio media present` / `ready` → `"true"` (compared in `AUD_CdReady` with the string at 0x437350); `open`, `set … time format tmsf`, `close` → 0.
- `play cdaudio from A to B`: A and B are MCI TMSF (`track | min << 8 | sec << 16 | frames << 24`; `AUD_CdPlaySegment` builds them from the record bytes `[unused, sec, min, track]` via `AUD_SeqPlay` `rec >> 8`). The range [A, B) of the track file is decoded with Media Foundation (FLAC, WAV, MP3, M4A, WMA; Ogg only with an installed decoder) and streamed to waveOut; playback stops at B like the disc. `AUD_CdResume` restarts from the elapsed time, which works unchanged.
- `stop` / `close` / `pause` stop the stream.
- `music_volume` (ini, 0–100) sets the waveOut volume. The game's own music gate is `AUD_StartMusic` 0x17d0: game state +0x1b14 = level music volume (+0x1b04, from the level's Sonix record) × master (+0x1b0c, 256) >> 8 (`FUN_00401000`); music also needs the options flag 0xb9e865.

Checked with a silent 36-minute test track: title (2:00:00–2:02:18 → 0–138 s), password screen (2:03:11–2:04:25 → 191–265 s) and VORTEX (2:31:08–2:34:20 → 1868–2060 s) requests decode exactly those ranges (`hercules_display.log`, `log = on`).
