# Verified Windows level map

Public titles follow the supplied list and are corroborated by the [Windows manual](https://cdn.akamai.steamstatic.com/steam/apps/987400/manuals/disneys_hercules_manual.pdf?t=1558107352). Exact title/loading-screen pixels in this installed build still require capture. Internal identifiers, engine descriptors, resource paths/hashes and load routines below are verified from the canonical local binaries/archive; runtime level completion is pending.

| public level | internal ID / label | engine descriptor RVA | MAP_PSX.BIN index / resource ID | MAP_PSX.CIF index / resource ID | family |
|---|---|---|---|---|---|
| L01 Your Basic D.I.D. | 1 / TRAINING GROUND | ENG1.DLL+0x5a6a8 | 565 / `5ED398E4` | 564 / `5ECB98E5` | i2.5 |
| L02 Hero's Gauntlet | 2 / TRAINING GAUNTLET | ENG3.DLL+0x38558 | 557 / `57E5A3D6` | 556 / `57DDA3D7` | i1.a |
| L03 The Centaur's Forest | 3 / FOREST OF CENTAURS | ENG1.DLL+0x5a6d8 | 515 / `49716801` | 516 / `497267F9` | i2.5 |
| L04 The Big Olive | 5 / VISIT TO THEBES | ENG1.DLL+0x5a708 | 513 / `496E5AF9` | 514 / `496F5AF1` | i2.5 |
| L05 Hydra Canyon | 6 / HYDRA BATTLE | ENG1.DLL+0x5a720 | 541 / `527C4FA9` | 517 / `4A7C50A9` | i2.5 |
| L06 Medusa's Lair | 7 / MEDUSA BATTLE | ENG1.DLL+0x5a738 | 530 / `508159E4` | 531 / `508259DC` | i2.5 |
| L07 Cyclops Attack | 8 / CYCLOPS CHASE | ENG3.DLL+0x385e8 | 491 / `4482B8EF` | 494 / `4582B0EF` | i1.a |
| L08 Titan Flight | 9 / BATTLE OF TITANS | ENG1.DLL+0x5a768 | 538 / `526D69FA` | 539 / `526E69F2` | i2.5 |
| L09 Passageways of Eternal Torment | 10 / PASSAGEWAYS TORMENT | ENG3.DLL+0x38618 | 520 / `4B83C8E3` | 521 / `4C83C0E3` | i1.a |
| L10 Vortex of Souls | 11 / VORTEX OF SOULS | ENG1.DLL+0x5a798 | 518 / `4B806805` | 519 / `4B8167FD` | i2.5 |

| level | archive directory | map load | TITLE descriptor RVA |
|---|---|---|---|
| L01 | `M:\GRAFIX\MAPS\TRAINING\` | ENG1.DLL+0x1f750 | 0x234bc |
| L02 | `M:\GRAFIX\MAPS\GAUNTLET\` | ENG3.DLL+0x1a930 | 0x234c8 |
| L03 | `M:\GRAFIX\MAPS\FOREST\` | ENG1.DLL+0x1f750 | 0x234d4 |
| L04 | `M:\GRAFIX\MAPS\THEBES\` | ENG1.DLL+0x1f750 | 0x234ec |
| L05 | `M:\GRAFIX\MAPS\HYDRA\` | ENG1.DLL+0x1f750 | 0x234f8 |
| L06 | `M:\GRAFIX\MAPS\MEDUSA\` | ENG1.DLL+0x1f750 | 0x23504 |
| L07 | `M:\GRAFIX\MAPS\CYCLOPS\` | ENG3.DLL+0x1a930 | 0x23510 |
| L08 | `M:\GRAFIX\MAPS\TITANS\` | ENG1.DLL+0x1f750 | 0x2351c |
| L09 | `M:\GRAFIX\MAPS\TORMENT\` | ENG3.DLL+0x1a930 | 0x23528 |
| L10 | `M:\GRAFIX\MAPS\VORTEX\` | ENG1.DLL+0x1f750 | 0x23534 |

ENG1 uses descriptor table RVA `0x5a690` with 0x18-byte records indexed by the shared level byte; header load is `ENG1.DLL+0x1f750`, streamed chunks use `+0x1f9e0`. ENG3 uses table RVA `0x38528`, also 0x18-byte records; map load is `ENG3.DLL+0x1a930`. These are actual decompiled routines, not proposed LVL names.

TITLE's label/engine table starts at RVA `0x234b0` with 0x0c-byte records, including ID 0 PLAYROOM and ID 4 NESSUS BATTLE. Its engine-pointer table at `0x22288` is used by `TITLE.DLL+0x16300` to dispatch through thunk `+0xc3d0` to EXE callback slot 0x64 (`HERCULES.EXE+0x4040` → module request `+0x5cd0` → load `+0x5c20`).

The EXE palette selector `HERCULES.EXE+0x5760` switches over IDs 1/2/3/5/6/7/8/9/10/11. `O:\` normalizes to `M:\GRAFIX\MAPS\`; the palette path is converted to the relevant CIF path by the EXE consumer. The index IDs above are file-resource hashes, distinct from gameplay level IDs.

Archive SHA-256: `affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4`. All twenty gameplay BIN/CIF name hashes and payload hashes were checked against this archive. Complete offsets/sizes/hashes are in [levels.json](levels.json) and [map-archive-entries.json](binary-analysis/map-archive-entries.json). Binary hashes are also recorded in the registry.

**Unresolved stage relationships:** ID 4 is `NESSUS BATTLE`, with `O:\NESSUS\` in the ENG1 descriptor but no separately named Nessus BIN/CIF in the catalog. The runtime relationship to L03 remains unknown. Thebes' reported second part stays a substage/scene discovery target under L04; demo filenames alone do not prove a second map or ID. Do not renumber L04 or create an eleventh public level from these leads.

**Difficulty:** Medium is the canonical complete route. The Windows manual states the final two public levels are unavailable on Beginner. Availability and behavior still need build-runtime confirmation; Herculean comparisons cover all ten levels.

**Runtime evidence:** existing demo traces open TRAINING BIN (565 / 5ED398E4) and CIF (564 / 5ECB98E5) at tick 1134, then return to title around tick 2017. This corroborates the TRAINING/ENG1 load only. No other level's live load or full completion is claimed here.

Sources: [dispatch tables](binary-analysis/dispatch-tables.json), [ENG1 loader](binary-analysis/ENG1.DLL-1001f750.c), [ENG3 loader](binary-analysis/ENG3.DLL-1001a930.c), [TITLE dispatch](binary-analysis/TITLE.DLL-10016300.c), [EXE palette selector](../../../analysis/exports/p2_decomp/00405760.c), [canonical edition](../../../docs/canonical-edition.md), [resource consumers](../../../docs/resource-consumers.md).
