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
- [ ] `S5.03` Map the Sonix level directories (for example `q:\NESSUS\` and `q:\HERC_NES\` in ENG1 strings) to the runtime bank loads per level.
- [ ] `S5.04` If the user supplies the disc (Q-01), cut track 2 at the documented segment times into `extracted/audio/music/`. Otherwise keep it marked missing; never count it as extracted.
- [ ] `S5.05` Write `docs/full-game/audio.md`: an audio table per level, with links from each level plan.

## Done when

- Every level's scenes list their observed SFX and music segment, with evidence ticks.
- Missing CD audio is explicit.

## Notes
