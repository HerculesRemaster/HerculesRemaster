# S4 Actors, animation and encounter template

Goal: decode every actor bank used by the ten levels, and define one encounter-specification template that every level's enemy and boss work fills in.
Needs:
- P4 inherited milestone: `tools/decode/{ifam,anim}.py`, `tools/viewer/anim_viewer/`. 107/109 i1.4 banks and 10,552 frames are decoded; banks 503 and 601 are unsupported.
- `tools/state/hero_anim.py`
- S1 for runtime sequence logs
- S3 for the ENG3 frame-request hook

Slices: `P4.ALL.03`, `P4.ALL.05`–`P4.ALL.07`.
Outputs:
- `extracted/full-game/sprites/<bank>/anim.json`
- `extracted/meta/full-game/animation-coverage.json`
- `docs/full-game/gameplay/encounters.md` (template plus per-level index)
- `docs/formats/animation.md`, `docs/formats/i-family.md`

## Tasks

- [ ] `S4.01` (P4.ALL.05) Build a bank → engine → level index from the string tables and runtime bank loads.
  - SRC1 banks are referenced from ENG1 strings, and SRC3 banks from ENG3 strings. Both are **VERIFIED-STATIC** by string presence.
  - The level a bank belongs to is a **LEAD** until a runtime load in that level confirms it.
  - Record the result in `animation-coverage.json`.
- [ ] `S4.02` (P4.ALL.03) Recover banks 503 and 601 (unsupported variants) and the `i1.3` entries (631–638) and `i1.9` entry (409) from their direct consumers. Keep `UnsupportedVariant` until the rule comes from code.
- [ ] `S4.03` (P4.ALL.05) Extend `anim.py` beyond the hero. Cover sequence order and durations in original ticks, pivots/attachments, mirroring and draw order. Take hitboxes from the per-actor collision code only (image bounds don't establish hitboxes).
- [ ] `S4.04` (P4.ALL.06) Define the encounter template in `encounters.md`:
  - actor, spawn kind and type-table entry (spawn/stub/update RVAs)
  - states, with entry conditions, sequence, sound and effect for each
  - telegraphs, attack windows, vulnerable windows
  - damage taken and dealt (`HERO_RequestDamage` ENG1 0x266a0)
  - phase changes, defeat/exit, difficulty guards
  - evidence label per row

  The L01 Stymphalian bird (spawn 0x433d0, update 0x43440, `docs/gameplay-spec.md`) is the worked example.
- [ ] `S4.05` (P4.ALL.07) For each bank, compare offline frames against runtime VRAM/frame pairs from the level where it is used. Cell-packed VRAM uploads are not necessarily complete sprites.
- [ ] `S4.06` Render contact sheets and anim-viewer pages per level and entity, with source IDs and palettes.

## Done when

- Every bank loaded at runtime in any level decodes, or carries an explicit code-backed unsupported reason.
- `encounters.md` holds the template, and level plans link their filled entries.

## Notes
