# Audio per level (S5.05)

Sources:
- `extracted/meta/full-game/audio-banks.json`, made by `tools/decode/level_sonix.py` (S5.03; VERIFIED-STATIC, runtime-confirmed for TRAINING and HYDRA).
- CD segments: `docs/music-transport.md`.
- Sonix formats: `docs/formats/sonix.md`.

`q:\` is `M:\LANGUAGE\SONIX\` in HERCULES.FS. Music is Red Book CD audio (track 2, MCI). With no disc, every music cell is **missing_source_media** (Q-01); the `MUSIC.MUS` files only hold the VAB/sequence data and segment positions.

| level | engine | music bank (MUSIC.MUS, archive idx) | SFX banks (SFX.SFX, archive idx) | volumes music/sfx | CD segment (track 2) | notes |
|---|---|---|---|---|---|---|
| L01 TRAINING (1) | ENG1 | `q:\TRAINING\` 745 | `q:\TRAINING\` 597, `q:\HERC_TRA\` 593 | 70 / 112 | 2:04:25–2:06:55 | runtime: `analysis/traces/runs/s102` |
| L02 GAUNTLET (2) | ENG3 | `q:\GAUNTLET\` 643 | `q:\GAUNTLET\` 585, `q:\HERC_GAU\` 583 | 70 / 110 | 2:06:55–2:08:55 | ENG3 descriptor 0x38528+2·0x18, +0x14 → 0x382c0, loaded through the same slot 0x6c (eng3 0x1ac63). Runtime: password-13 run, tick 1787 |
| L03 FOREST (3) | ENG1 | `q:\FOREST\` 600 | `q:\HERC_FOR\` 582, `q:\FOREST\` 612 | 80 / 110 | 2:08:55–2:12:53 | mid-level `q:\NESSUS\` SFX + music stream at the Nessus trigger (ENG1 0xe670; L03.RE.03) |
| (Nessus) | ENG1 | `q:\NESSUS\` 589 | `q:\NESSUS\` 605 | 128 / 128 | 2:12:53–2:15:29 | streamed inside FOREST. The ID-4 descriptor names the same banks but has no map |
| L04 THEBES (5) | ENG1 | `q:\THEBES\` 591 | `q:\THEBES\` 607, `q:\HERC_THE\` 594 | 70 / 110 | 2:15:29–2:18:33 | |
| L05 HYDRA (6) | ENG1 | `q:\HYDRA\` 613 | `q:\HERC_HYD\` 586, `q:\HYDRA\` 475 | 80 / 110 | 2:18:33–2:20:56 | runtime: `analysis/traces/runs/sweep_6` |
| L06 MEDUSA (7) | ENG1 | `q:\MEDUSA\` 576 | `q:\HERC_MED\` 590, `q:\MEDUSA\` 588 | 80 / 110 | 2:20:56–2:22:19 | |
| L07 CYCLOPS (8) | ENG3 | `q:\CYCLOPS\` 580 | `q:\CYCLOPS\` 587, `q:\HERC_CYC\` 581 | 75 / 110 | 2:22:19–2:25:10 | record 0x38398 |
| L08 TITANS (9) | ENG1 | `q:\TITANS\` 592 | `q:\TITANS\` 608, `q:\HERC_TIT\` 596 | 72 / 110 | 2:25:10–2:28:33 | record has `fe 2f` (`RT_StubVoid(0x2f)`, then EXE 0x1f40) |
| L09 TORMENT (10) | ENG3 | `q:\TORMENT\` 572 | `q:\HERC_TOR\` 595, `q:\TORMENT\` 578 | 70 / 110 | 2:28:33–2:31:08 | record 0x38440 has `fe 28` (like VORTEX) |
| L10 VORTEX (11) | ENG1 | `q:\VORTEX\` 604 | `q:\VORTEX\` 615, `q:\HERC_VOR\` 598 | 72 / 100 | 2:31:08–2:34:20 | record has `fe 28` |
| title/menu | TITLE | `q:\OPTIONS\` | `q:\OPTIONS\` | | seq 1–4 (see `music-transport.md`) | runtime: every trace |

## Open

- **SFX per scene/event (S5.01) and music-start ticks per scene (S5.02):** these need runtime sound-play logs. The hook's `sfx_play` events exist; their binding to scenarios is not done.
- **ENG3 levels:** the records are decoded (S3.02 consumer = slot 0x6c, same format as ENG1). ENG3 starts music with slot 0x0d4 at eng3 0x1acd1, right after the load.
- **`q:\HERC_NES\`:** has no reader.
