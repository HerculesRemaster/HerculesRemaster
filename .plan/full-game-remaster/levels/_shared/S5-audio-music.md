# S5 Audio and music binding

Goal: bind the already-decoded audio to levels, scenes and events, and keep the CD-music transport metadata per level. No audio decoder is rewritten.
Needs:
- P4 inherited milestone: 559/559 ESF (`tools/decode/esf.py`), parsed Sonix banks (`docs/formats/sonix.md`), CD segments (`docs/music-transport.md`)
- S1 sound-play events; S3 for the ENG3 sound calls

Slices: `P4.ALL.01`–`P4.ALL.02`.
Outputs: `extracted/meta/full-game/audio-scene-coverage.json`, `docs/full-game/audio.md`.

**VERIFIED-STATIC** (`docs/music-transport.md`):
- Music is Red Book CD audio on track 2, played via MCI (`play cdaudio from %d to %d`, EXE 0x9e60).
- The chain is DLL slot 0xd4 `AUD_StartMusic` → 0xc250/0xc280 → 0x9e60, with positions taken from each level's `MUSIC.MUS`.
- Each level bank has its own segment: TRAINING, GAUNTLET, FOREST, **NESSUS**, THEBES, HYDRA, MEDUSA, CYCLOPS, TITANS, TORMENT, VORTEX.

There is no disc image (Q-01), so music stays `missing_source_media`.

## Tasks

- [ ] `S5.01` (P4.ALL.01) Log the sound-play calls (bank/program/ESF ID, channel, caller RVA) in both engines. Bind each one to scenario/scene/event in `audio-scene-coverage.json`.
- [ ] `S5.02` (P4.ALL.02) Record every `AUD_StartMusic(vab, seq)` call per scene, with level bank, sequence and segment times. The separate NESSUS segment is a key L03 input: record when it starts relative to the FOREST segment.
- [x] `S5.03` Map the Sonix level directories (for example `q:\NESSUS\` and `q:\HERC_NES\` in ENG1 strings) to the runtime bank loads per level.
- [ ] `S5.04` If the user supplies the disc (Q-01), cut track 2 at the documented segment times into `extracted/audio/music/`. Otherwise keep it marked missing; never count it as extracted.
- [x] `S5.05` Write `docs/full-game/audio.md`: an audio table per level, with links from each level plan.

## Done when

- Every level's scenes list their observed SFX and music segment, with evidence ticks.
- Missing CD audio is explicit.

## Notes
- `S5.03` (2026-10-12): `tools/decode/level_sonix.py` → `extracted/meta/full-game/audio-banks.json` (with archive indices). VERIFIED-STATIC, runtime-confirmed for TRAINING (`analysis/traces/runs/s102`) and HYDRA (`sweep_6`).
  - Each ENG1 descriptor's +0x14 is a Sonix record read by `AUD_LoadLevelSonix` (EXE 0x16b0): `[ff, music vol, sfx vol] [fe, x] music-id sfx-ids… ff` ('c' skipped). Ids index the EXE directory table 0x435178.
  - Per level: music `q:\<LEVEL>\`, SFX = the level dir plus `q:\HERC_<xxx>\` (hero SFX). Volumes are 70–80 / 100–112.
  - ID 0 uses VORTEX's banks; ID 4 uses `q:\NESSUS\` only.
  - FOREST additionally streams `q:\NESSUS\` SFX and music mid-level (`AUD_ForestStreamTick` ENG1 0xe670, L03.RE.03).
  - `q:\HERC_NES\` has no reader.
- `S5.05` (2026-10-12): `docs/full-game/audio.md`, one row per level (music/SFX banks with archive ids, volumes, CD segment, runtime evidence; music marked missing_source_media). Every level README links it. SFX per scene stays S5.01/S5.02 (runtime).
- `S5.01`/`S5.02` progress (2026-10-12): the hook now logs `audio` events by wrapping the EXE interface-table slots (0x4361b8, before any module copies it): 0x06c `AUD_LoadLevelSonix`, 0x070 `AUD_LoadMusic`, 0x074 `AUD_LoadSfx`, 0x0d4/0x0d8 `AUD_StartMusic`, 0x0dc `AUD_StopMusic`. Fields: args, return, level, DLL caller. Sound plays: the existing `sfx_play` (`AUD_StartVoice`). First run: TITLE LoadMusic(12,1), LoadSfx(25,2), StartMusic(0,4); GAUNTLET LoadLevelSonix(descriptor byte list). Binding to scenes (coverage) is next.
- `S5.01`/`S5.02` progress (2026-10-12): `tools/trace/audio_scenes.py` → `extracted/meta/full-game/audio-scene-coverage.json`, built from every trace.
  - Scenes are module/level stretches between loads. Per scene: sfx ids with count, first tick and DLL caller, plus the engine audio calls (gamehercules-40's slot hooks).
  - Observed across both engines: TITLE 72 scenes / 54 sfx; TRAINING 21 / 38; THEBES 8 / 30; FOREST 6 / 40; GAUNTLET 7 / 26; HYDRA, MEDUSA, TITANS, VORTEX, CYCLOPS and TORMENT 2–4 each.
  - Music (S5.02): every level start calls `AUD_LoadLevelSonix(record)` then `AUD_StartMusic(0, 0)` (sequence 0 of the level's MUSIC.MUS). The title starts sequence 4, and sequence 1 after START GAME.
  - Still open:
    - sfx binding to events (the hook's caller is the sound helper; the requesting handler needs a deeper stack or the handler sound lists in `L*-types.json`);
    - the Nessus music start inside FOREST (needs a traced Nessus approach);
    - audio-slot events for the four boss levels (their runs predate that hook).

- `S5.01` progress (2026-10-12, gamehercules-40):
  - **Hook:** the `sfx_request` hook (ENG1 0x6da0 / ENG3 `AUD_PlaySfx` 0xbc47) logs the handler sound id, object, object kind and caller.
  - **Rule (VERIFIED-RUNTIME):** EXE voice id of the following `sfx_play` = id & 0x7ff. The high bits are play flags (0x4000 positional; 0x2000/0x6000/0xe000 seen).
  - **Binding:** `tools/trace/sfx_events.py` → `extracted/meta/full-game/sfx-events.json`, per level and voice: request ids, object kinds with their identification, callers, plays and refusals. GAUNTLET 32, CYCLOPS 35 and TORMENT 21 voices are bound so far, e.g. punching bag 0x40 → 129/130 and hero → 57/58.
  - **Open:** the ENG1 levels need runs made with the new DLL (07:55); per-scenario binding follows from `audio-scene-coverage.json`.
- `S5.02` progress (2026-10-12, gamehercules-40, ENG3):
  - **Music calls:** the only music call seen in GAUNTLET/CYCLOPS/TORMENT scenes is `AUD_StartMusic(0, 0)` from eng3 0x1acd1 at the level-load tick, right after `AUD_LoadLevelSonix` (GAUNTLET 5, CYCLOPS 3, TORMENT 4 calls; scenes from runs made before the audio slot hook have none) (descriptor +0x14 record: music bank `q:\<LEVEL>\MUSIC.MUS`, `docs/full-game/audio.md`). No other music call occurs during play in the traced stretches (`audio-scene-coverage.json`).
  - **Segment times:** the CD track 2 segments are in `docs/music-transport.md` (GAUNTLET 2:06:55–2:08:55, CYCLOPS 2:22:19–2:25:10, TORMENT 2:28:33–2:31:08). The audio stays `missing_source_media` (Q-01).
  - **Open:** a music change at completion/tally needs a traced completion (played run). ENG1 scenes are session 22's.
