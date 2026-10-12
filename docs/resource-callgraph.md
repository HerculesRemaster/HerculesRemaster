# Resource call graph

How bytes get from HERCULES.FS into the game. HERCULES.EXE RVAs (base 0x400000).

```
HERCULES.FS (one FILE*, opened by FS_Init 0x5020)
  └─ FS_Open 0x5130 ── FS_NormalizePath 0x4e10 ── FS_HashName 0x4fe0 ── binary search g_fsIndex
     FS_Read 0x5210 / FS_Seek 0x52c0 / FS_Tell 0x5340 / FS_Close 0x51f0
        │
        ├─ RT_PCopen 0xa210 / RT_PCread 0xa2f0 / RT_PClseek 0xa280 / RT_PCclose 0xa350
        │     (table slots 0x270 / 0x274 / 0x26c / 0x260; 16 descriptors at 0x96e640)
        │     └─ DLL game code: maps, sprites, animation, demo data … read into
        │        RT_Malloc'd blocks of the EXE arena (0x6177d0, 3 MiB), decoded in the DLL
        │
        ├─ AUD_CreateSfx 0xf3d0 (slot 0x22c)
        │     m:\language\sonix\pc\%ld.esf → ESF → DirectSound buffers (EXE)
        │
        ├─ GFX_LoadLevelPalette 0x5760 (slot 0x244)
        │     o:\<level>\map_psx.bin-style names (o: = M:\GRAFIX\MAPS\), seek 0x12,
        │     read 0x300 bytes = 256 × RGB palette → 256 palette entries (0x6780)
        │
        └─ 0x68c0 (EXE, not yet named): FS_Open/FS_Read/FS_Close
```

Debug-print loaders around the same calls: `RT_LoadFile` 0x3b50 (slot 0x68, "Loading %s (0x%x) size %d"), 0x3c50 (slot 0x80, "Opening %s"), 0x3d90 (slot 0x20, "Unable to open %s (%d)"). They format paths with `"%s%s"` (0x436024) and log through 0x63c0.

## Drive aliases seen in names

| alias | archive path | used by |
|---|---|---|
| `o:\` | `M:\GRAFIX\MAPS\` | level palette loader (`o:\cyclops\map_psx.bin`, `o:\forest\…`, `o:\gauntlet\…`) |
| `p:\` | `M:\GRAFIX\CHOP\` | |
| `q:\`, `r:\` | `M:\LANGUAGE\SONIX\` | |
| `m:\` | as is | ESF (`m:\language\sonix\pc\%ld.esf`) |

## Open for P3

- Which DLL functions call PCopen and with which names (runtime trace gives the full list, including the 17 ids the namelist doesn't resolve).
- 0x68c0's role.
