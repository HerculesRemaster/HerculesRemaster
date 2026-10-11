# Continue

Run this from `C:\Games\GameHercules\Remaster`. It works for a first start and for resuming.

1. Read `.plan/hercules-remaster/README.md` (phase table) and pick the first phase that is not done and whose inputs are done. P2F (fullscreen on 4K) goes first after P2.
2. Read that phase file (and `FACTS.md` if you need numbers). Do not read the other phase files.
3. Implement the first unticked task. When it works, tick it, add a short note, and move on to the next task.
4. When the phase is done, update the README state and stop. Start the next phase in a fresh session.

Keep it lean: no test suites, no evidence files, no long reports. Write code, run it once to confirm it works, and record findings in the phase notes or the phase's docs output.

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
11. **Save tokens with rtk** (see FACTS → Tools):
    - Run shell commands through the **Bash** tool. Its `rtk hook claude` hook rewrites `git`, `cat`, `grep`, `ls`, `find` and `curl` to compact `rtk` forms. The PowerShell tool has no hook, so prefix `rtk` yourself there (`rtk git status`).
    - The hook doesn't rewrite project scripts. Wrap noisy ones yourself: `rtk err .venv/Scripts/python.exe tools/…` (errors and warnings only), `rtk summary pwsh -File tools/….ps1`, `rtk log <file>` for traces and debugger logs.
    - Look at large outputs, don't print them: `rtk json --keys-only <file>` (or `-d 2`) for exports and manifests, `rtk read -m 80 <file>` for a code preview, `rtk read --head-lines N` / `--tail-lines N`, `rtk wc`, `rtk diff`.
    - Project tools print a short summary (counts, output paths, problems) and write bulk data to files. No per-entry stdout; add a `--verbose` flag if needed.
    - ghidra-mcp results don't go through rtk. Use filters and limits (`offset`/`limit`, name filters, one function per decompile), and keep long listings in `analysis/exports/` files rather than in chat.
    - Use `rtk proxy <cmd>` only when the condensed output is unusable (empty, garbled or contradicts the exit code). `rtk recall <hash>` restores an elided part.
