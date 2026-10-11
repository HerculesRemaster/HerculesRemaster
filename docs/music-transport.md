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
