# W0 — Web migration scope and toolchain

## Goal

Record the four browser-port decisions before starting the native host in W1.
The user requested implementation of the migration plan on 2026-10-13.

## Inputs

- Existing development authorization in `docs/authorization-scope.md`.
- `docs/wasm/migration-roadmap.md`, W0, and `docs/wasm/implementation-backlog.md`, W0-01/W0-02.
- Installed and verified Emscripten SDK 6.0.12 (`docs/wasm/toolchain.md`).

## Tasks

- [x] Prepare the web release-scope section in `docs/authorization-scope.md` using the plan's local/LAN default.
- [ ] D-W0-1: obtain the user's confirmation of local/LAN scope and finalize the release-scope section (W0-01).
- [x] D-W0-2: record SDK installation approval; install pinned Emscripten outside the game folder and verify compilation.
- [ ] D-W0-3: record whether to proceed without music or accept the user's own CD track 2 rip.
- [ ] D-W0-4: record whether saves use browser storage or reproduce Windows behavior.
- [ ] Record all confirmed decisions in `FACTS.md` and close W0-02.
- [ ] Acceptance gate: all four decisions are recorded and local development has no unresolved scope question.

## Notes

2026-10-13: SDK 6.0.12 is installed at `C:\Users\Thanh\emsdk`; C11/C++17 WASM compilation and Node execution passed.
The three remaining policy questions have been sent to the user; their suggested answers are not confirmations.
W1 starts with the native x86 host and DLL table handshake, before any browser shell or interpreter.
