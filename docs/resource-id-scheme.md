# Resource ID scheme (HERCULES.FS)

Source: HERCULES.EXE decompiled in Ghidra (RVAs from base 0x400000). Python copy: `tools/fs/resource_hash.py` (`normalize`, `herchash`).

## Archive reader

| RVA | name | role |
|---|---|---|
| 0x5020 | `FS_Init` | `PC_InitFileSys`: `fopen("hercules.fs","rb")`, reads the 0x3000-byte index into `g_fsIndex` (0x9177d0), counts entries up to the first id 0 (`g_fsCount` 0x91bbd4), fatal error `PC_InitFileSys(3)` unless ids are strictly ascending. Sets all 1,024 positions (`g_fsPos` 0x91a7d0) to -1. Called from 0x5e10. |
| 0x5110 | `FS_Shutdown` | `fclose` |
| 0x5130 | `FS_Open(name, mode)` | `FS_NormalizePath` → `FS_HashName` → binary search over the sorted index; fatal `PC_fopen("%s")` when not found. Returns handle = index + 1 and sets its position to 0. |
| 0x51f0 | `FS_Close(h)` | position = -1 |
| 0x5210 | `FS_Read(buf, size, count, h)` | `fread` semantics, clamped to the entry size. The shared `FILE*` is re-seeked only when the handle differs from the last one (`g_fsLastHandle` 0x4365a4) or the position is 0. |
| 0x52c0 | `FS_Seek(h, off, whence)` | `SEEK_SET/CUR/END` (0/1/2), clamped to the entry size |
| 0x5340 | `FS_Tell(h)` | position |

One `FILE*` is shared by all handles; per-entry state is only the read position. The archive is opened once and never written. The index entry for handle `h` is `g_fsIndex[h-1]` = `{id, offset, size}`.

Callers of `FS_Open` (EXE): 0x5760 (palette loader: seek 0x12, read 0x300 = TGA header + 256×RGB; not yet a function in Ghidra), 0x68c0, 0xa210, 0xf3d0. The DLLs reach the archive through the EXE (see `module-interface.md`).

## Name → path (`FS_NormalizePath`, 0x4e10)

Works in a static buffer (0x91b7d0, name copied to +100 so prefixes can be written in front):

1. Uppercase `a`–`z` only.
2. If the name has no drive (`name[1] != ':'`), prefix `M:` (and `\` if it doesn't start with one).
3. Remap the drive letter:

   | drive | becomes |
   |---|---|
   | `M:` | unchanged |
   | `O:\` | `M:\GRAFIX\MAPS\` |
   | `P:\` | `M:\GRAFIX\CHOP\` |
   | `Q:\`, `R:\` | `M:\LANGUAGE\SONIX\` |
   | any other | `M:` + the path after the first directory (`C:\HERC\GRAFIX\A.BIN` → `M:\GRAFIX\A.BIN`) |

## Path → id (`FS_HashName`, 0x4fe0)

```
id = Σ (signed char)path[i] << ((i & 3) * 8)  +  strlen(path)      (u32, wraps)
```

This is the CTPAX formula. Differences from it: the game hashes the normalized path (so drive aliases resolve to the same id) and sign-extends bytes ≥ 0x80 (no archive name uses them). With the namelist, 804 of the 821 non-zero ids resolve; the remaining 17 need names from runtime tracing (P3).
