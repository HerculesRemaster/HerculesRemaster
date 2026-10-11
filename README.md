# Hercules remaster workspace

Reverse engineering and 4K remaster workspace for *Disney's Hercules* (1997, Windows). The original game in
`C:\Games\GameHercules` is never modified. This repository holds the tools, specifications, manifests and the
implementation plan.

- Master document: `../Hercules-Reverse-Engineering-Master-Document.md` (treated as a plan; its findings are leads to verify).
- Implementation plan (lean): [`.plan/hercules-remaster/`](.plan/hercules-remaster/README.md). Start with
  [CONTINUE.md](.plan/hercules-remaster/CONTINUE.md).
- Edition record: [docs/canonical-edition.md](docs/canonical-edition.md).

## Layout

```text
.plan/hercules-remaster/   lean phase plan (P0-P8) with checklists
backups/originals/         read-only copies of the game files (gitignored)
manifests/                 SHA256SUMS, file-inventory.json, archive manifests, provenance/*.jsonl
working/game/              runtime copy for experiments (gitignored)
analysis/                  exports/, annotations/, imhex/, traces/ (Ghidra project: C:\Games\Ghidra\HerculesRevertEngineer)
extracted/                 raw/ payloads and decoded audio/images/sprites/levels (gitignored), meta/ coverage (tracked)
reference/                 environment, startup traces, captures, scenarios, tuning, performance
remastered/                hi-res pipeline jobs, manifests, pilot, runtime atlases
runtime/                   prototypes, hook DLL, the new runtime
tools/                     common/ (preserve, provenance) and per-area tools
tests/                     pytest suites per tool area
docs/                      specifications and project records
```

Originals, extracted payloads, decoded media and Ghidra databases stay out of Git (master §5); manifests
identify them by SHA-256.

## Reproduce

Requires uv (0.13+) and Python 3.13 (uv provides it). From this directory:

```sh
uv sync                                                   # creates .venv from pyproject.toml + uv.lock
.venv/Scripts/python.exe tools/common/preserve.py          # copy originals read-only, write manifests
.venv/Scripts/python.exe tools/common/preserve.py --verify # re-hash copies and sources
sha256sum -c manifests/SHA256SUMS                          # independent check (Git Bash)
.venv/Scripts/python.exe -m pytest -q                      # all test suites
.venv/Scripts/python.exe tools/common/provenance.py preserve  # recorded preservation runs
```

`preserve.py` refuses to overwrite a copy whose hash differs from its source (exit 2) and records each run in
`manifests/provenance/preserve.jsonl`.

## Rules

- Never write into `C:\Games\GameHercules` outside this folder; never modify `backups/originals/`.
- Record addresses as module + RVA (RVA = VA - image base) so they can be re-checked in Ghidra.
- Tasks marked `[HUMAN]` in the plan need a person; see HUMAN_GATES.md.
- Use `python` for the plan tool and `.venv/Scripts/python.exe` for project tools (`python3` is the Microsoft Store stub on this machine).
