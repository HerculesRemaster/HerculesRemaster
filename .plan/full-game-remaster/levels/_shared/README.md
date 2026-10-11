# Shared dependencies

Engine-wide work that more than one level needs lives here, so level plans don't duplicate it. Each shared plan has its own checklist with `Sn.NN` IDs. Each task names the global `P3.ALL.*`, `P4.ALL.*`, `P5.ALL.*` or P6–P8 task it slices. Level plans cite these IDs as dependencies; they never re-implement them.

| plan | blocks | slices |
|---|---|---|
| [S1 runtime harness](S1-runtime-harness.md) | every `Lxx.TR.*` | P3.ALL.03–06, P3.ALL.11–12 |
| [S2 ENG1 map completion](S2-eng1-map-completion.md) | `Lxx.AS.*`/`Lxx.GP.*` for L03 L04 L05 L06 L08 L10 (and L01 gaps) | P5.ALL.01–03 (ENG1), P4.ALL.03–04 (i2.5) |
| [S3 ENG3 engine](S3-eng3-engine.md) | all phases of L02 L07 L09 | P5.ALL.01–03 (ENG3), P5.ALL.09, P4.ALL.03 (i1.a), P3.ALL.06 (ENG3 hooks) |
| [S4 actors and animation](S4-actors-animation.md) | `Lxx.AS.*` actor/boss tasks | P4.ALL.03, P4.ALL.05–07 |
| [S5 audio and music](S5-audio-music.md) | `Lxx.AS.*` audio tasks | P4.ALL.01–02 |
| [S6 progression and difficulty](S6-progression-difficulty.md) | entry methods, difficulty comparisons, the final linked run | P3.ALL.07, P3.ALL.09–10, P5.ALL.11–12 |
| [S7 remaster integration](S7-remaster-integration.md) | every `Lxx.RM.*` | P7/P8 |

Already built, and reused rather than rebuilt (paths relative to `Remaster/`):

| area | existing tools and docs |
|---|---|
| decoders | `tools/decode/{ifam,anim,esf,sonix,tga,palette}.py`, plus `compare_pairs.py` |
| i2.5 levels | `tools/level/level_decode.py` (raises on any tag other than i2.5), `render_preview.py`, `tools/viewer/level_viewer/` |
| state | `tools/state/{demo,hero_anim,trace_state}.py` |
| tracing | `runtime/hooks/hercules_hook.c` and `herc_inject.c` (`build.ps1`), `tools/trace/{run_trace.ps1,collect_pairs.py,map_reads.py}` |
| runtime | `tools/runtime/{make_working_copy.ps1,launch_original.ps1,capture_demo.ps1,gpu_diff.py}` |
| renderer | `runtime/display/{ddraw_proxy.c,gpu_renderer.cpp,hercules_display.ini}`; hybrid route A in `docs/architecture-decision.md`; texel-hash art swap in `docs/gpu-renderer.md` |
| specs | `docs/{gameplay-spec,level-format,demo-format,resource-consumers,music-transport}.md`, `docs/formats/*.md` |

**Hybrid route A** keeps the original DLL logic and redraws the PSX-style ordering table that `GFX_DrawOTag` (EXE 0xac60) receives. "Gameplay reconstruction" in these plans therefore means code- and trace-backed specifications. They are needed for art and timing integration, coverage, and a possible later native port. These plans do not re-implement the logic.
