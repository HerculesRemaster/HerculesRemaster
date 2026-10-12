# Task identities and completed work

The lean P3/P4/P5 files have checklists without inline IDs and remain unchanged in `../hercules-remaster/`. This separate plan copies their checked milestones and preserves equivalent IDs from `../_archive/hercules-remaster-full-2026-10-11.7z`. This is an alias index, not a reset or reactivation of the archived plan/tool. The archive is unchanged. Existing tasks omitted by the lean revision remain archived with their original identities.

| archived ID(s) | current work / retained milestone |
|---|---|
| `06a.B.P1.T1` | P3 working-copy task, completed in P2F |
| `06a.B.P2.T1` | P3 launch-original task, completed in P2F |
| `06a.B.P2.T2` | P3 demo capture task, completed; presented-frame capture replaced gdigrab |
| `09a.A.P1.T1` | P3 tracer choice, completed |
| `09a.B.P1.T1` | P3 OS read/archive mapping, completed title/demo milestone |
| `09a.B.P1.T2` | P3 FS lookup/consumer logging, completed title/demo milestone |
| `09a.B.P1.T3` | P3 image/ESF decoder pairs, completed title/demo milestone |
| `09a.B.P1.T4` | P3 character frame and sound events, completed demo milestone |
| `09a.C.P1.T1`, `09a.D.P1.T1` | P3 completed resource-consumer chain/docs; all-level extension is P3.ALL.13 |
| `06b.A.P1.T1` | scenario design extended by P3.ALL.02 |
| `06b.B.P1.T1`, `06b.B.P2.T3` | capture/input tooling and sessions, extended by P3.ALL.04–09; optional elevated Procmon remains optional |
| `06b.C.P1.T1`, `06b.D.P1.T1` | scenario coverage/handoff, extended by P3.ALL.12–13 |
| `10a.A.P1.T1` | P4 ESF survey, completed |
| `10a.B.P1.T1` | P4 ESF codec recovery, completed; u32 tables found via runtime anchor |
| `10a.B.P2.T1`, `10a.B.P2.T2` | P4 ESF decoder/batch export, completed (559/559) |
| `10a.B.P2.T3` | P4 Sonix/music-transport classification, completed |
| `10a.C.P1.T2` | P4 optional HUMAN listening check, original state retained |
| `10b.A.P1.T1` | P4 image/palette survey |
| `10b.B.P1.T1` | P4 TGA decoder |
| `10b.B.P2.T1`, `10b.B.P2.T2`, `10b.B.P2.T3` | P4 i-family recovery/decoder/contact sheets |
| `10c.B.P1.T1` | P4 compare runtime image pairs |
| `10c.A.P1.T1` | P4 animation-table candidates |
| `10c.B.P2.T1`, `10c.B.P2.T2`, `10c.B.P2.T3` | P4 animation metadata/viewer/hero sequences |
| `10a.D.P1.T1`, `10b.D.P1.T1`, `10c.D.P1.T1` | format documentation, extended by P4.ALL.11 |
| `11a.A.P1.T1` | P5 level-to-load-code mapping |
| `11a.B.P1.T1`, `11a.B.P1.T2`, `11a.B.P1.T3` | P5 structures/decoder/preview |
| `11a.B.P1.T4`, `11a.C.P1.T1`, `11a.C.P1.T2` | archived landmark/preview checks, extended by P5.ALL.03 |
| `11b.A.P1.T1` | P5 hero/update/demo anchors |
| `11b.B.P1.T1`, `11b.B.P1.T2` | P5 timing/demo reader |
| `11b.B.P2.T1`, `11b.B.P2.T2` | P5 state traces and hero/first-enemy specification |
| `11b.B.P2.T3`, `11b.C.P1.T1` | archived behavior/replay checks, extended by P5.ALL.05–12 |
| `11a.D.P1.T1`, `11b.D.P1.T1` | level/gameplay documentation, extended by P5.ALL.13 |

New work is append-only under `P3.ALL.*`, `P4.ALL.*` and `P5.ALL.*`. Existing P0/P1/P2/P2F files and completed checkbox rows are not rewritten. P3's phase gains new work while its title/demo milestone stays completed. P4's completed audio, image and hero-animation milestone remains checked while scene attribution and other families are expanded. No all-level capture or decode is declared complete merely because its definition exists.

## Per-level and shared task families (added 2026-10-12)

[levels/](levels/README.md) adds the append-only ID families `Lxx.{RE,TR,AS,GP,RM}.NN` (one per level plan, L02–L10, plus L03's Nessus tasks) and `S1.NN`–`S7.NN` (shared dependencies). Each one names the `P3.ALL.*`, `P4.ALL.*`, `P5.ALL.*` or P6–P8 task it slices. Those global IDs and every alias above are unchanged; L01 has no new task family and keeps its lean-plan tasks.
