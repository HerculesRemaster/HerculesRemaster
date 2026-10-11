# ghidra-mcp notes

GhidraMCP 7.0.0 GUI plugin inside Ghidra 12.1.4, project `C:\Games\Ghidra\HerculesRevertEngineer`, HTTP on `127.0.0.1:8089`.

## Start / connect

- `pwsh -File tools/ghidra/ensure_ghidra.ps1` returns at once when `/check_connection` is ok; otherwise it starts `ghidraRun.bat` and waits up to 180 s.
- Claude sessions reach the server through the uv bridge in `.mcp.json` (`Remaster/.mcp.json` is a copy of `../.mcp.json`).
- Programs in the project: `HERCULES.EXE` (base 0x400000), `ENG1.DLL`, `ENG3.DLL`, `TITLE.DLL` (base 0x10000000; imported from `backups/originals/`).
- Four programs are open, so every call needs `program=` (the server errors without it and lists the open programs).

## MCP tools that work

| tool | use |
|---|---|
| `list_open_programs`, `get_metadata(program)` | program list, base, function count |
| `import_file(file_path)` | imports, opens and auto-analyses in one call; the project file is saved already (`save_program` then says "No unsaved changes") |
| `analysis_status(program)` | `analyzing` / `analyzed` |
| `list_project_files(folder="/")` | project contents |
| `save_program(program)` | after renames/edits |

Most tools are deferred: load them with `ToolSearch("select:mcp__ghidra-mcp__<name>")`.

## HTTP API (scripted exports)

- `GET /mcp/schema` lists all 205 endpoints with method and parameter source (`query` / `body`). GET takes query params; POST takes a JSON body, with `program` in the query.
- Errors come back as HTTP 200 with `{"error": ...}`; `tools/ghidra/mcp_http.py` turns them into `GhidraError`.
- Paged lists return `{<key>: [...], count, offset, limit, total}`. Keys: `find_functions` → `functions`, `list_strings` → `strings`, `list_program_items` → `items` (`kind` = imports | exports | segments | data_items | external_locations | classes | methods | namespaces). `get_entry_points` is not paged (`entry_points`).
- The old `list_imports` / `list_exports` / `list_segments` / `list_functions` endpoints are gone (404); use `list_program_items` / `find_functions`.
- `find_functions` leaves out external (import) functions: HERCULES.EXE lists 1,103 of 1,420, and ENG1 lists 1,000 of 1,050.
- Each DLL's exports list `Ordinal_1` and `PC_DLLEngineMain` at the same address, plus `entry` (DllMain CRT).

## Scripts

- `tools/ghidra/mcp_http.py get|post <endpoint> -p <program|-> [-q k=v] [-b k=v] -o out.json [--all KEY]`
- `tools/ghidra/export_baseline.py [programs…]` → `analysis/exports/<program>/{metadata,imports,exports,entry_points,memory_blocks,strings,functions}.json`

## Pitfalls (P2)

- **Open programs with `open_program` first** (POST `{path}`; it shows them in the CodeBrowser). A program opened implicitly by `program=` on another endpoint has no analysis tool. Once analysis is queued on it (e.g. after `create_function`), every save hangs: `ProgramSaves.awaitAnalysis` → Ghidra `AutoAnalysisManager.waitForAnalysis` runs its worker inline (`getAnalysisTool() == null`), and the worker calls `waitForAnalysis` again, forever. All 3 HTTP workers get stuck and only a Ghidra restart clears it. Check `GET /tool/running_tools`: all four programs should be listed under CodeBrowser.
- `save_program` and `save_all_programs` are **GET**. A POST to them never returns.
- `get_functions` returns at most 20 functions per call (`max_functions`, `truncated: true`).
- `create_function` sometimes fails with "Not in a transaction"; retrying works.
- `rename_function` takes an address as `old_name`. Pass `strict_mode=off` for prefix names (`FS_Open`); otherwise the naming gate only warns.

## P2 scripts

- `tools/ghidra/p2_anchors.py` → `analysis/exports/p2_anchors.json` (string/import xrefs)
- `tools/ghidra/p2_contract.py` + `p2_hints.py` → `analysis/exports/p2_contract.json`, `p2_contract_hints.txt` (EXE→DLL table slots)
- `tools/ghidra/apply_annotations.py [file]` applies `analysis/annotations/*.json` (names + comments) and saves
- `tools/ghidra/dec.py <program> <name|addr> [--disasm]` prints one decompile
