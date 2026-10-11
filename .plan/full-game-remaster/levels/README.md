# Per-level implementation plans

Plan status: **deferred**, the same as the parent [full-game plan](../README.md). These documents split the all-level P3–P5 work, plus the P6–P8 integration, into ten level plans that can each be executed on their own. Engine-wide work lives in [shared dependencies](_shared/README.md). Writing or editing these documents does not start captures, decoding or gameplay implementation.

The global phase files [P3](../P3-runtime-trace.md), [P4](../P4-assets.md) and [P5](../P5-levels-gameplay.md) remain the cross-level completion gates, and their `P3.ALL.*`, `P4.ALL.*` and `P5.ALL.*` IDs remain authoritative. Each level task names the global task it slices. Ticking a level task records that level's share of that global task. A global task closes only when every level slice and shared slice it lists has closed.

## Levels

Static identities below are copied from [levels.json](../reference/levels.json) and [level-map.md](../reference/level-map.md), and are **VERIFIED-STATIC**. Runtime load is traced only for L01, and only in demo mode (`analysis/traces/runs/demo90`). Beyond that, L02 has one runtime observation: the GPU-renderer dev overlay `analysis/runtime/levels/gpu_10500.txt` shows `module ENG3 map o:\gauntlet\map_psx.cif` at frame 10500 of an unattended GPU-renderer run. That is presumably the attract demo cycle; the file itself records no demo ID or tick.

| plan | internal ID / label | engine / family | ENG descriptor | BIN idx / ID | CIF idx / ID | TITLE desc | state |
|---|---|---|---|---|---|---|---|
| [L01 Your Basic D.I.D.](L01-your-basic-did/README.md) | 1 TRAINING GROUND | ENG1 / i2.5 | ENG1+0x5a6a8 | 565 / `5ED398E4` | 564 / `5ECB98E5` | 0x234bc | index; slice done in lean plan |
| [L02 Hero's Gauntlet](L02-heros-gauntlet/README.md) | 2 TRAINING GAUNTLET | ENG3 / i1.a | ENG3+0x38558 | 557 / `57E5A3D6` | 556 / `57DDA3D7` | 0x234c8 | todo (blocked by S3) |
| [L03 The Centaur's Forest](L03-the-centaurs-forest/README.md) | 3 FOREST OF CENTAURS | ENG1 / i2.5 | ENG1+0x5a6d8 | 515 / `49716801` | 516 / `497267F9` | 0x234d4 | todo |
| [L04 The Big Olive](L04-the-big-olive/README.md) | 5 VISIT TO THEBES | ENG1 / i2.5 | ENG1+0x5a708 | 513 / `496E5AF9` | 514 / `496F5AF1` | 0x234ec | todo |
| [L05 Hydra Canyon](L05-hydra-canyon/README.md) | 6 HYDRA BATTLE | ENG1 / i2.5 | ENG1+0x5a720 | 541 / `527C4FA9` | 517 / `4A7C50A9` | 0x234f8 | todo |
| [L06 Medusa's Lair](L06-medusas-lair/README.md) | 7 MEDUSA BATTLE | ENG1 / i2.5 | ENG1+0x5a738 | 530 / `508159E4` | 531 / `508259DC` | 0x23504 | todo |
| [L07 Cyclops Attack](L07-cyclops-attack/README.md) | 8 CYCLOPS CHASE | ENG3 / i1.a | ENG3+0x385e8 | 491 / `4482B8EF` | 494 / `4582B0EF` | 0x23510 | todo (blocked by S3) |
| [L08 Titan Flight](L08-titan-flight/README.md) | 9 BATTLE OF TITANS | ENG1 / i2.5 | ENG1+0x5a768 | 538 / `526D69FA` | 539 / `526E69F2` | 0x2351c | todo |
| [L09 Passageways of Eternal Torment](L09-passageways-of-eternal-torment/README.md) | 10 PASSAGEWAYS TORMENT | ENG3 / i1.a | ENG3+0x38618 | 520 / `4B83C8E3` | 521 / `4C83C0E3` | 0x23528 | todo (blocked by S3) |
| [L10 Vortex of Souls](L10-vortex-of-souls/README.md) | 11 VORTEX OF SOULS | ENG1 / i2.5 | ENG1+0x5a798 | 518 / `4B806805` | 519 / `4B8167FD` | 0x23534 | todo |

ID 4 `NESSUS BATTLE` (ENG1+0x5a6f0, TITLE 0x234e0, `O:\NESSUS\`) is **not** a public level. It is handled as an L03 substage question in [L03](L03-the-centaurs-forest/README.md). Thebes' reported second part is an L04 substage question. There are exactly ten public levels.

Shared loaders, all VERIFIED-STATIC:

| role | location |
|---|---|
| ENG1 header load | ENG1+0x1f750 |
| ENG1 stream chunks | ENG1+0x1f9e0 |
| ENG1 descriptor table | ENG1+0x5a690 (0x18-byte records) |
| ENG3 load | ENG3+0x1a930 |
| ENG3 descriptor table | ENG3+0x38528 (0x18-byte records) |
| TITLE dispatch | TITLE 0x16300 → 0xc3d0 → EXE slot 0x64 (EXE 0x4040 → 0x5cd0 → 0x5c20) |
| palette selector | EXE 0x5760 |

`HERCULES.FS` SHA-256 is `affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4`.

## Shared dependencies

| plan | scope | needed by |
|---|---|---|
| [S1 runtime harness](_shared/S1-runtime-harness.md) | scenario runner, level/substage/difficulty identity, entry methods, `run.json`, frame↔tick sync, hook extensions | every level's trace phase |
| [S2 ENG1 map completion](_shared/S2-eng1-map-completion.md) | i2.5 open fields (T28 models/sky, T44/T4c/T50/T40), VRAM overruns, previews for all seven ENG1 maps | L03 L04 L05 L06 L08 L10 |
| [S3 ENG3 engine](_shared/S3-eng3-engine.md) | ENG3 tick/loader/entity/runner/camera RE, i1.a decoder, ENG3 draw hook | L02 L07 L09 |
| [S4 actors and animation](_shared/S4-actors-animation.md) | SRC1/SRC3 bank decode for all actors, encounter-spec template, unsupported banks 503/601, i1.3/i1.9 | all levels with enemies or bosses |
| [S5 audio and music](_shared/S5-audio-music.md) | SFX/ESF/Sonix → scene binding, CD-segment music per level, missing CD audio | all levels |
| [S6 progression and difficulty](_shared/S6-progression-difficulty.md) | passwords, tally, lives/continues, checkpoints, difficulty encoding, Beginner L08 end and L09/L10 lockout, linked Medium run | all levels; closes the game |
| [S7 remaster integration](_shared/S7-remaster-integration.md) | per-level art inventory, gpu_renderer overrides, P7 pipeline reuse | every level's remaster phase (after P7 pilot approval) |

## Files in each level folder (L02–L10)

| file | content |
|---|---|
| `README.md` | identity (IDs, hashes, RVAs, engine), scope, dependencies, phase/state table, scenario links, acceptance criteria |
| `FACTS.md` | evidence split into VERIFIED-STATIC / VERIFIED-RUNTIME / LEAD / UNRESOLVED, with source paths |
| `1-reverse-engineering.md` | Ghidra work: descriptor/type table, actor spawn/update handlers, level-specific mode/boss code |
| `2-runtime-trace.md` | scenario captures, complete traversal, difficulty comparisons |
| `3-assets.md` | map/layers/palette, sprite banks, effects, audio, per-scene asset coverage |
| `4-gameplay.md` | mechanics, enemies, bosses, scripts and checkpoints, specified into `docs/full-game/levels/<Lxx>.md` |
| `5-remaster.md` | renderer integration, art inventory, 4K art through S7, side-by-side play check |

Evidence labels used in every FACTS file:

- **VERIFIED-STATIC**: read from the canonical binaries or archive and cited to a file in this repo.
- **VERIFIED-RUNTIME**: observed in a trace or capture of this build, with the run path and tick range.
- **LEAD**: from the manual, the walkthrough (`C:\Games\GameHercules\WALKTHROUGH_GAME.md`), sprite or bank names, demo filenames or community password lists. A lead tells you where to look; it is not a result.
- **UNRESOLVED**: an open question, plus the next trace or decompilation that would close it.

Task IDs are append-only, in the form `Lxx.{RE,TR,AS,GP,RM}.NN` for levels and `Sn.NN` for shared plans. Never renumber them. When a stage or scene is found, append it using the [discovery workflow](../reference/scenarios/README.md). Scenario IDs and existing task IDs stay stable.

## Implementation order

1. **S1** harness and the identity/difficulty-encoding part of **S6** (`S6.01`–`S6.03`). Every level's runtime phase needs both.
2. Two independent tracks, which can run in parallel:
   - **ENG1 track:** S2 + S4 (ENG1 banks) → L03 (resolves Nessus) → L04 (resolves the Thebes substage) → L05 → L06 → L08 → L10. L01 gaps can be closed at any point on this track.
   - **ENG3 track:** S3 → L02 (establishes the runner model) → L07 → L09.
3. S5 audio binding runs alongside each level's asset phase.
4. S7 and each level's `5-remaster.md` start only after the P7 pilot is approved (rule 8 in [CONTINUE](../CONTINUE.md)).
5. Close with S6: the linked Medium L01→L10→ending run (`P3.ALL.07`), the Beginner/Herculean comparisons and the Beginner availability checks.

Within one level the phases run in this order: RE and trace run in parallel, then assets, then gameplay, then remaster. An earlier phase can be re-opened when a later phase finds a gap.

## Output paths

All paths are relative to `Remaster/`; the inherited outputs stay where they are.

| location | content |
|---|---|
| `analysis/traces/full-game/<level-slug>/<scenario>/<run-id>/` | `run.json`, `resource-usage.jsonl`, `input.jsonl`, `checkpoints.jsonl`, `pairs/` |
| `reference/captures/full-game/<level-slug>/<scenario>/<run-id>/` | video, audio, frame↔tick index |
| `extracted/full-game/levels/<DIR>/`, `extracted/full-game/{images,sprites}/` | level and asset exports |
| `extracted/meta/full-game/` | scene and asset coverage, gameplay coverage, tuning |
| `docs/full-game/levels/<Lxx>.md` | level specification |
| `docs/full-game/` | shared specifications (`gameplay/*.md`, `eng3-format.md`, `resource-consumers.md`) |
| `remastered/<DIR>/` | approved replacement art |
