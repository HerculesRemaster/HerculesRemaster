# P0 Setup: workspace + Ghidra/ghidra-mcp

Goal: a hash-anchored workspace and a running ghidra-mcp (GUI plugin) server over the EXE and the three DLLs.
Outputs: `backups/originals/`, `manifests/`, `.venv`, `tools/common/`, `tools/ghidra/`, Ghidra project `C:\Games\Ghidra\HerculesRevertEngineer`, `.mcp.json`.

## Workspace (done)

- [x] Inventory the originals into `docs/canonical-edition.md`.
- [x] Set up `.gitignore`/`.gitattributes` and a uv `.venv` (pillow, numpy, pefile).
- [x] Write `tools/common/preserve.py`. It copies the originals to read-only `backups/originals/` and writes `manifests/SHA256SUMS` and `file-inventory.json`. `--verify` re-checks them.
- [x] Write `tools/common/provenance.py`. It records each transformation (command, versions, hashes) to `manifests/provenance/<tool>.jsonl`.

## Ghidra + ghidra-mcp

The RE server is bethington/ghidra-mcp's **GUI plugin** inside Ghidra 12.1.4, not the headless server.

- [x] GhidraMCP 7.0.0 plugin installed and enabled (user setup). Ghidra GUI serves `127.0.0.1:8089`; the bridge in `../.mcp.json` answers.
- [x] Use the existing project `C:\Games\Ghidra\HerculesRevertEngineer`. HERCULES.EXE is already imported and auto-analysed.
- [x] Write `tools/ghidra/ensure_ghidra.ps1`: idempotent. If `/check_connection` is ok, return. Otherwise start `ghidraRun.bat` (it reopens the last project) and wait up to 180 s for the server.
- [x] Write `tools/ghidra/mcp_http.py`, a small stdlib client for scripted exports against the same `:8089` HTTP API: GET/POST with a mandatory `program=` and errors raised as exceptions. Responses are written to files, and stdout gets a one-line summary (endpoint, count, path) so that results stay out of context.
- [x] Import ENG1.DLL, ENG3.DLL and TITLE.DLL from `backups/originals/` with `import_file` and run auto-analysis. Save.
- [x] Export a per-program baseline to `analysis/exports/<program>/*.json`: imports, exports, entry points, memory blocks, strings, function list. Check the exports with `rtk json --keys-only` and `rtk wc`, not with `cat`.
- [x] Add `Remaster/.mcp.json` (same bridge config as `../.mcp.json`) so sessions started in `Remaster\` get the tools.
- [x] Note the tool names and calling conventions that work in `docs/ghidra-mcp-notes.md` (short).

Done when the `mcp__ghidra-mcp__*` tools answer for all four programs.

## Notes

- 2026-10-11: Workspace done. Preserve `--verify` passes for 11 files. Existing `tests/common/` is left as is; no new tests are planned.
- 2026-10-11: Switched from the headless server to the GUI plugin (user decision). The check through the bridge passes: `/check_connection` returns `server_kind=gui, version 7.0.0`, and `get_metadata(program=HERCULES.EXE)` reports base 0x400000 with 1,420 functions. HERCULES.EXE was imported from `C:\Games\GameHercules\HERCULES.EXE`, whose sha256 matches `backups/originals/` (b3ab3ee9…).
- 2026-10-11: `ensure_ghidra.ps1` returns ok against the running server. Its cold-start path (`ghidraRun.bat` + 180 s wait) hasn't been run yet. `mcp_http.py` pages lists and raises on `{"error"}`. The bridge's old `list_imports`/`list_functions` endpoints return 404; `list_program_items` and `find_functions` replace them (see `docs/ghidra-mcp-notes.md`).
- 2026-10-11: The DLLs were imported from `backups/originals/` and auto-analysed: ENG1 has 1,050 functions, ENG3 670 and TITLE 446, all at base 0x10000000. Each exports `PC_DLLEngineMain` (ENG1.DLL+0x2c180, ENG3.DLL+0x1f830, TITLE.DLL+0xc1f0), which matches FACTS. The baseline is in `analysis/exports/<program>/` (`export_baseline.py`). `find_functions` leaves out external thunks. P0 is done.
