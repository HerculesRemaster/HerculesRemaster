# Canonical edition

Recorded 2026-10-11 by task `00a.A.P1.T1` (evidence: `.plan/hercules-remaster/evidence/00a/00a.A.P1.T1.txt`).
Master §1.1, §6.1-6.3.

## Decision

**Canonical copy: the installed game files in `C:\Games\GameHercules`** (project decision). No second
edition, installer, disc image or earlier analysed folder exists on this machine to compare against, so
there is nothing to merge or diff. The earlier documents name another path
(`C:\Users\Thanh\Downloads\GameHercules`, docs/README.md); that folder was not inspected and its findings are
re-measured here before use.

## Game files

| file | bytes | sha256 | role |
|---|---:|---|---|
| HERCULES.EXE | 275,968 | b3ab3ee9e5c973bb245d8eed795e8e7065d7dfc371e412e602b2059af894a551 | host executable, PE32 x86 |
| ENG1.DLL | 481,280 | 01b78ae814892007bb6031b2470f10e09bdd912fdb8d30de9f4cfea7c638f757 | engine DLL, single export `PC_DLLEngineMain` |
| ENG3.DLL | 272,896 | aefa1147deae55c4cb2290282f5142df00baa49a9cd59f671f7a3ce5b8b7fca7 | engine DLL, single export `PC_DLLEngineMain` |
| TITLE.DLL | 180,736 | b17a37fb1bd5002c54cedb6dba1509bc1ca2d8483fa9724d8e4ade04fe79188b | title/front-end DLL, single export `PC_DLLEngineMain` |
| HERCULES.FS | 46,924,407 | affa380e45918448895932cd466ea9a02096b8dcf08644f91b9da5ec34cd48b4 | resource archive, 1,024 index records |
| HERCULES.HLP | 46,876 | a7420dc688388487f1863b7d6140cf4c86128f791fd055db7bc572e7181f0e2f | WinHelp file |
| DISNEY.INI | 2,256 | 49f59acbc3abfbc508e9a8862d7eead89af8aff88fbb7507302c94510881d375 | installer platform record |
| HERC.REG | 664 | 18826333085747ae307b9fcfe7255baf946d1f8a96d8a5c50b3708ce27fb877b | registry import for the 32-bit view (key present in HKLM) |
| SETUP.BAT | 43 | 65a6ef0a3602d75c01c1e606d7bcb2d31e8a32d633ce2e6b32ebe0bf7af3f431 | runs `regedit HERC.REG` |
| ReadMe.txt | 646 | f9398c5fd3b54fbf49bcf1c8a7ce0031c18c68827a1fdba9130e0782a5ede540 | redistributor notes (provenance only; its site text stays out of project files) |
| unins000.dat | 11,804 | a39420ad325bf476d520945a11a1689c2ecf7a30ba4fc4b0ccfd2909205fe232 | repack uninstaller log, not used by the game |

File dates (filesystem): EXE 1997-09-13, DLLs 1997-08-20, HERCULES.FS 1997-08-12, HERC.REG 2017-10-19.

## Missing media

- **No disc image, CUE/BIN sheet, CD-audio track or music file** was found in `C:\Games\GameHercules`
  (depth 3, tool folders excluded; see evidence). `ReadMe.txt` states music is not included.
  The EXE imports `mciSendStringA`, consistent with CD-audio music (not yet confirmed);
  10a documents the transport (R34). Open question Q-01 asks the owner for the disc.
- No installer (InstallShield) files are present; the folder is an installed copy plus a repack's helpers.

## Related project records in the root (not game files)

| file | bytes | sha256 | note |
|---|---:|---|---|
| LICENSE | 9,034 | ccd9ada38620f8ad890a2cd3a3a59cba36345c3b89e417edcc8b48257675fd02 | Titled as a reverse-engineering and remastering licence agreement for this game. Its status line reads "DRAFT - NOT EXECUTED"; term, fee and jurisdiction are placeholders; both signature blocks name Tensormorph AI. As written it excludes public release, sale and distribution (§4, §8, §10). Recorded for the owner's authorization record (master §1, Q-02, task 14a.B.P2.T1); this plan does not interpret it. |

Other root items (`ghidra_12.1.4_PUBLIC`, `ghidra-mcp`, `x96dbg`, `ProcessMonitor`, `GameExtractor`,
`noesisv4474`, `pefile-2024.8.26`, `hercstls`, `Icons`, guides and `download.ps1`) are tools, guides or
unrelated files, not part of the game edition.
