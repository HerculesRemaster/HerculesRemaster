# Disney's Hercules Action Game: Modernization Documentation

Context pack for upgrading the 1997 Windows 95 game *Disney's Hercules Action Game* (Disney Interactive, 1997) into a modern-graphics game.

## Reading order

| # | Document | Purpose |
|---|----------|---------|
| 01 | [File inventory](01-file-inventory.md) | Every file in the folder: what it is, size, role |
| 02 | [Current system architecture](02-system-architecture.md) | How the original game is built and runs (EXE + 3 DLLs + archive) |
| 03 | [Configuration, registry and install](03-config-and-install.md) | `DISNEY.INI`, `HERC.REG`, `SETUP.BAT`, run problems on Win10/11 |
| 04 | [HERCULES.FS asset archive](04-asset-archive.md) | Archive layout we decoded, plus the unknown asset formats |
| 05 | [Modernization strategy](05-modernization-strategy.md) | Options, recommended approach, target architecture |
| 06 | [Roadmap and task list](06-roadmap-and-tasks.md) | Phased checklist of work, with exit criteria |
| 07 | [Risks, legal and open questions](07-risks-legal-open-questions.md) | IP, technical risk, what we do not know yet |
| 08 | [Reverse-engineering guide](08-reverse-engineering-guide.md) | Tools, commands and a step-by-step plan for the unknown formats |

## One-paragraph summary

The folder holds a complete, runnable copy of a 32-bit Windows 95 game: `HERCULES.EXE` (launcher/engine host), three engine DLLs (`ENG1`, `ENG3`, `TITLE`), a 46.9 MB packed asset archive (`HERCULES.FS`, 1,024 entries), a help file, install config files, and 5 unrelated icon files. There is **no source code**. The game uses DirectDraw (2D, software-blitted) and DirectSound. Music is missing from this copy. "Modern graphics" therefore means one of two things: re-hosting the original engine behind a new renderer, or rebuilding the game in a modern engine using extracted and re-authored assets. See doc 05.

## Facts vs. inferences

Documents mark each claim:
- **[Verified]**: read directly from files in this folder.
- **[Inferred]**: reasoned from evidence; needs confirmation.
- **[Unknown]**: not yet determined.

Analysis date: 2026-10-11. Folder: `C:\Users\Thanh\Downloads\GameHercules`. Not a git repo.
