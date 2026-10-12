# Continue the full-game plan (when requested)

This plan is **deferred**. Preparing/editing its documents does not authorize starting the full-game implementation. When the user asks to start or resume it, run from `C:\Games\GameHercules\Remaster` and follow this file. The lean plan in `.plan/hercules-remaster` keeps its own workflow and progress.

1. Read `.plan/full-game-remaster/README.md` and `START-HERE.md`. Reuse verified inherited P0/P1/P2/P2F work; pick the first incomplete full-game task whose inputs exist. P2F remains first after P2 and its completed user check does not need approval again.
2. Read the phase file, its inputs and `FACTS.md`. For P3–P5 read this plan's `reference/levels.json`, `reference/level-map.md`, `reference/sources.md`, `reference/scenarios/README.md` and `reference/scenarios/coverage.md`, plus relevant workspace inventory/reports/traces. Public level numbers are not internal IDs.
3. Implement the first unticked full-game task. Preserve inherited completed work. Update only this plan's checklists/coverage and use the isolated full-game output paths in `START-HERE.md`; do not change `.plan/hercules-remaster`.
4. When the phase's all-level completion gate is met, update the README state. P3's completed title/demo tasks and P4's completed decoders remain completed milestones; they do not close the newly expanded full-game scope.

**Per-level plans.** When the user names a level, or when the current step is level work, use `levels/<Lxx-slug>/`:
1. Read its README, FACTS and the phase file.
2. Check that the shared S-plan tasks it lists are done. If they aren't, do those first, in `levels/_shared/`.
3. Implement the first unticked `Lxx.*` task, tick it and add 1–3 lines to that file's Notes.
4. Update the phase-state table in the level README and the coverage cells.

When a level's slice of a global `P3.ALL.*`, `P4.ALL.*` or `P5.ALL.*` task closes, add a note to the global phase file. Tick the global task only when every level and shared slice of it is done. Move facts from LEAD/UNRESOLVED to VERIFIED only with a cited artifact.

Keep outputs concise: preserve existing logs/pairs/captures and record evidence paths plus tick/event ranges in scenario JSON and the coverage matrix. Do not create duplicate evidence ledgers or reset completed work. Use Medium for linked L01–L10 progression; record Beginner/Herculean differences and Beginner's L09/L10 exclusion. Refine and append stages/scenes through the discovery workflow rather than changing existing task/scenario IDs.

## Rules

1. **Originals are read-only.** Never write to `C:\Games\GameHercules` outside `Remaster\` or to `backups/originals/`. Run the game only from `working/game/`.
2. **Addresses** are written as `program` + RVA (EXE base 0x400000, DLL base 0x10000000).
3. **Don't invent** formats, addresses or numbers. A decode that looks plausible is not confirmed: take the rule from the game's code (Ghidra) or from runtime data.
4. **ghidra-mcp** (bethington/ghidra-mcp 7.0.0, GUI plugin) is the RE tool. Ghidra 12.1.4 runs with the GhidraMCP plugin, which serves `127.0.0.1:8089` over the project `C:\Games\Ghidra\HerculesRevertEngineer`. Start it with `tools/ghidra/ensure_ghidra.ps1` and keep Ghidra open (the server stops when the last Ghidra window closes). Use the `mcp__ghidra-mcp__*` tools (find them with `search_tools`). Pass `program=` on every call and `save_program` after edits. Rename prefixes: `FS_` archive, `MOD_`/`RT_` contract/runtime, `AUD_` audio, `GFX_` images, `ANIM_` animation, `LVL_` levels, `HERO_` gameplay. No headless server and no second Ghidra instance on this project.
5. **Run one game process at a time.**
6. **[HUMAN]** tasks: ask the user and wait for an answer. Don't guess.
7. **No third-party binary downloads** (DirectDraw wrappers, upscalers) without the user's OK. Our own proxy `ddraw.dll` (P2F, built from `runtime/display/`) is allowed, and only in `working/game/`.
8. **No bulk 4K art before the P7 pilot is approved.** Replacement art never changes logical size, pivot or timing.
9. **Python:** project tools use `.venv/Scripts/python.exe`. `python3` is the Store stub and doesn't work.
10. **Git:** don't commit unless asked. Originals, extracted payloads, Ghidra DBs and media stay gitignored. Keep the redistributor's site text and password from `ReadMe.txt` out of every project file.
    Scenario definitions, schemas, coverage metadata and source references are tracked text; large run outputs and media remain ignored.
11. **Save tokens with rtk** (see FACTS → Tools):
    - Run shell commands through the **Bash** tool. Its `rtk hook claude` hook rewrites `git`, `cat`, `grep`, `ls`, `find` and `curl` to compact `rtk` forms. The PowerShell tool has no hook, so prefix `rtk` yourself there (`rtk git status`).
    - The hook doesn't rewrite project scripts. Wrap noisy ones yourself: `rtk err .venv/Scripts/python.exe tools/…` (errors and warnings only), `rtk summary pwsh -File tools/….ps1`, `rtk log <file>` for traces and debugger logs.
    - Look at large outputs, don't print them: `rtk json --keys-only <file>` (or `-d 2`) for exports and manifests, `rtk read -m 80 <file>` for a code preview, `rtk read --head-lines N` / `--tail-lines N`, `rtk wc`, `rtk diff`.
    - Project tools print a short summary (counts, output paths, problems) and write bulk data to files. No per-entry stdout; add a `--verbose` flag if needed.
    - ghidra-mcp results don't go through rtk. Use filters and limits (`offset`/`limit`, name filters, one function per decompile), and keep long listings in `analysis/exports/` files rather than in chat.
    - Use `rtk proxy <cmd>` only when the condensed output is unusable (empty, garbled or contradicts the exit code). `rtk recall <hash>` restores an elided part.
