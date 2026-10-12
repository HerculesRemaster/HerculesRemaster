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

- [x] `S4.01` (P4.ALL.05) Build a bank → engine → level index from the string tables and runtime bank loads.
  - SRC1 banks are referenced from ENG1 strings, and SRC3 banks from ENG3 strings. Both are **VERIFIED-STATIC** by string presence.
  - The level a bank belongs to is a **LEAD** until a runtime load in that level confirms it.
  - Record the result in `animation-coverage.json`.
- [x] `S4.02` (P4.ALL.03) Recover banks 503 and 601 (unsupported variants) and the `i1.3` entries (631–638) and `i1.9` entry (409) from their direct consumers. Keep `UnsupportedVariant` until the rule comes from code.
- [ ] `S4.03` (P4.ALL.05) Extend `anim.py` beyond the hero. Cover sequence order and durations in original ticks, pivots/attachments, mirroring and draw order. Take hitboxes from the per-actor collision code only (image bounds don't establish hitboxes).
- [x] `S4.04` (P4.ALL.06) Define the encounter template in `encounters.md`:
  - actor, spawn kind and type-table entry (spawn/stub/update RVAs)
  - states, with entry conditions, sequence, sound and effect for each
  - telegraphs, attack windows, vulnerable windows
  - damage taken and dealt (`HERO_RequestDamage` ENG1 0x266a0)
  - phase changes, defeat/exit, difficulty guards
  - evidence label per row

  The L01 Stymphalian bird (spawn 0x433d0, update 0x43440, `docs/gameplay-spec.md`) is the worked example.
- [x] `S4.05` (P4.ALL.07) For each bank, compare offline frames against runtime VRAM/frame pairs from the level where it is used. Cell-packed VRAM uploads are not necessarily complete sprites.
- [x] `S4.06` Render contact sheets and anim-viewer pages per level and entity, with source IDs and palettes.

## Done when

- Every bank loaded at runtime in any level decodes, or carries an explicit code-backed unsupported reason.
- `encounters.md` holds the template, and level plans link their filled entries.

## Notes
- `S4.01` (2026-10-12): `tools/decode/bank_index.py` → `extracted/meta/full-game/animation-coverage.json`.
  - Bank tables: ENG1 0x5cee0 (81 ids) and ENG3 0x39548 (10 ids), 0x14-byte records `{ptr path, u32 frames, ptr data, u32, u16 state}`.
  - Id 0 is empty: its path is the empty directory-prefix string (ENG1 0x6ffc0 / ENG3 0x3fc80).
  - All 89 named banks map to archive entries and extracted folders.
  - ENG1 level lists (0x5a690 + 0x18·id → u8 ids ending 0xff, plus a list per start at start +8; loader 0x3270, freeing 0x3910) give a VERIFIED-STATIC level for 42 banks:
    - TITANS: PEGASUS, RockT, RtRock, Harp, Spy (no hero bank);
    - HYDRA: BOSS\HYDRA; MEDUSA: BOSS\MEDUSA + SKELWAR; VORTEX: BOSS\HADES;
    - FOREST: NESSUS\Basic, Water, Griffon, …; THEBES: Thebians, Minotaur, Harpies; TRAINING: PHIL, TRAINING, TOPS, STYBIRD.
  - The other banks (hero extras, letters, Nessus moves, Titans, gods) are loaded by actor code: LEAD until a runtime load confirms them. ENG3 levels are LEAD (S3).
- `S4.04` (2026-10-12): `docs/full-game/gameplay/encounters.md`: template (bank, spawn/type entry, states, telegraphs, attack and vulnerable windows, damage, phases, defeat, difficulty guards, effects, evidence per row) and a per-level index.
  - Worked example: TRAINING bird kind 0x38. Every state of `HERO_BirdUpdate` 0x43440; contact damage `HERO_RequestDamage(bird, 0x11)` = index 1 → 8/18/22 by difficulty (corrected 2026-10-12: index = ((n+1) & ~0x10) − 1); one-hit defeat `HERO_BirdDefeat` 0x43e20; no difficulty reads.
  - New names: `HERO_BirdChooseAction` 0x43b50, `HERO_BirdSteer` 0x43d80, `HERO_FindHitSource` 0x26de0.
  - Open: anim-script sound opcodes, drop table 0x43ff0, telegraph frames.
- `S4.02` (2026-10-12): no direct consumer exists in this build for 503 and 601 (`i1.4`), 631–638 (`i1.3`) or 409 (`i1.9`). No module string hashes to their ids, neither bank table lists them, and no trace opens them. They stay `UnsupportedVariant` ("unreferenced by this build"; `docs/formats/i-family.md`); re-check against S6.08.
  - Side finding: ENG1 level ids 0 PLAYROOM and 4 NESSUS have configs but no map file in the archive.
- `S4.06` (2026-10-12): `tools/decode/contact_sheets.py` → `extracted/full-game/sprites/<bank>/contact.png` (89 banks, frame ids on a checker grid; palettes as decoded in P4) and `extracted/full-game/sprites/index.html`.
  - The index groups banks by level (ENG1 lists; other banks LEAD) and lists engine, bank id, path, archive id, frame counts (table/decoded), sheet and anim-viewer links.
  - HYDRA's boss bank holds only effects (acid, rocks, explosions, children); the Hydra itself is drawn from T28 models (S2.01).
- `S4.01` addendum: `extracted/meta/full-game/bank-requests.json` lists the constant ids passed to `ANIM_RequestBank` 0x3520 (frees: `ANIM_FreeBank` 0x3870). Banks requested by code:
  - NESSUS Punch..Defeat (69–75) by `HERO_NessusSpawn`/`HERO_NessusUpdate`. That code is FOREST's kind 0x2f, spawned by FOREST trigger 0x53 (L03.RE.03), so these banks are VERIFIED-STATIC for FOREST.
  - LavaT (50) by 0x37fa0 and TornadoT (51) by 0x38490 (callers not yet tied to a level).
  - HC_DUNIT (5) by `HERO_SetEndSequence`.
- `S4.01` addendum 2 (2026-10-12): spawn kinds of class 0x1000 are **bank-streaming triggers** (`FUN_10026480` → `FUN_10028290`), fired when the camera is 120–180 units before them:
  - 0x10XX → `ANIM_RequestBank`(XX); 0x18XX → `ANIM_FreeBank`(XX);
  - 0x14XX/0x1CXX → request/free the HERCULES letter banks in bitmask XX (table 0x53800 = banks 6, 7, 8, 9, 10, 11, 7, 12; requested only while the letter is uncollected, gs+0x1c).
  - `bank_index.py` now lists them per map (`eng1_map_triggers`) and adds the levels of constant `ANIM_RequestBank` calls made by level handlers (`code_requests`; e.g. the Nessus moves → FOREST).
  - **71 of 89** banks now have a VERIFIED-STATIC level. The rest are drawn by type-table kinds whose load path is still open (TITANS gods/Titans, Grif/Merv/Pig, Meg, Archerc, FPhil, ThBurnt) or have no loader at all (PAIN, PANIC). Each carries that label.
  - Also new: the anim-table byte (+0xc actors / +6 markers and pickups) is each kind's sprite bank id (obj+0x22). Runtime anim events confirm it for the bird (46) and GENERIC1 (38). `L*-types.json` has a `bank` per kind.
- `S4.05` progress: `tools/decode/bank_runtime.py` adds a `runtime` block per bank to `animation-coverage.json`.
  - Pixel level: frames whose every offline part key is in the runtime catalog. Usage: frames shown in hook anim events, by level.
  - STYBIRD: all 115 shown frames pixel-identical to the offline decode. Hero banks 1/2/4 partly matched (only scenes with art dumps count).
  - The ENG1 entry sweep (art_dump on per level) adds catalog keys per level.
- `S4.05` done (2026-10-12) for the ENG1 banks seen at runtime. The ENG3 banks are with S3 (gamehercules-40).
  - The ENG1 entry sweep (art_dump on) and earlier runs grew the runtime key catalog `extracted/art_blocks` from 4,709 to 18,187 keys. `bank_runtime.py`: 1,282 of 8,625 ENG1 offline frames are pixel-matched by runtime blocks.
  - **Strict check** (`extracted/meta/full-game/bank-runtime-strict.json`): within each art-dump run, frames shown at runtime vs that run's own dumped keys.
    - Fully matched: HERCULES 154, HC_DIE 76, PEGASUS 87, BOSS\HYDRA 45, BOSS\MEDUSA 97, THOLD 20, Harp 8.
    - Mostly matched: HADES 314/322, SKELWAR 68/81, XTRA_DB1 233/303, GENERIC1 55/74, THFAT 12/20.
    - Unmatched-only: LETTERH 22, STYBIRD frame 30.
  - Anim events also fire for objects that are culled off-screen and never drawn, so an unmatched shown frame is not evidence of a decode error. No decoder mismatch is established; the matched frames confirm the offline decode pixel for pixel.
  - Banks not yet seen at runtime need their scenes first (phase-2 traversals).

- `S4.03` progress (2026-10-12, gamehercules-40):
  - **Hitboxes:** `tools/decode/anim.py` now writes code-backed `hitboxes` for every frame it exports, in both engines: the frame record boxes as `GFX_FrameHitbox` reads them (ENG3 0x23860 = ENG1 0x30e40, identical code by fingerprint; named in both programs together with `LVL_ObjOverlap` ENG1 0x2a660 / ENG3 0x1eff0 and `LVL_ObjWorldBox` ENG1 0x2a880 / ENG3 0x1f1a0). Box = 6 s8: z0, y0, x0·2, z1, y1, x1·2, scaled by obj+0x3a and mirrored by obj+0x54 0x10/0x20.
  - **Models:** ENG3 model hitboxes are in `level.json` `models[].hitboxes`.
  - **Coverage:** 37 `anim.json` files, 1764 frames with hitboxes (`--src3` adds the ENG3 banks from the full-game runs).
  - **Open before the tick:**
    - attachments (parent/child objects) and draw order;
    - the per-actor boxes that handlers test themselves instead of the frame box (the ENG3 kinds list them in `L*-types.json` "damage", e.g. `|z|<32, |x|<32`);
    - the ENG1 per-actor equivalents.
