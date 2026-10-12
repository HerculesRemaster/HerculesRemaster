# Authorization scope (Q-02, Q-06)

Recorded 2026-10-12 from the user's answers in P6/P7.

## Scope (Q-02)

- **Authorized:** a complete 4K remaster of Disney's Hercules (1997) for development. This includes AI processing and regeneration of all existing visual assets (sprites, characters, animations, backgrounds, textures, UI, effects), replacing the original low-resolution assets with modern 4K CGI equivalents.
- **Must be preserved:** original gameplay, level layouts, mechanics and animation behaviour. The remaster runs with the replacement assets.
- **Not authorized:** commercial distribution and redistribution. These must first be validated against the actual written Disney licence. Until that is recorded here, nothing (original files, replacement art, packages) is distributed.
- **Unchanged project rules:**
  - originals stay read-only;
  - no third-party binary downloads without the user's OK;
  - no bulk 4K art before the P7 pilot is approved;
  - replacement art never changes logical size, pivot or timing.

## Art route (Q-06)

- **Route:** full AI CGI regeneration, not a pixel-art upscale. All sprites, characters, backgrounds, animations, UI, textures and effects are recreated as 4K CGI assets that keep the original identities, poses, layouts, gameplay and animation timing. The tools are AI generation and upscaling, automated batch processing, and manual corrections where needed. Character appearance and animation continuity must hold across every frame.
- **Pipeline:** a reusable automated pipeline covering every level: extraction → AI regeneration → validation → true-colour integration.
- **Renderer requirements:**
  - native true-colour RGBA8888 overrides with transparency (PNG/WebP) and alpha blending at high resolution;
  - the original fog, shade, lighting and blending re-implemented for true-colour assets in GPU shaders;
  - the original indexed rendering kept as the fallback for assets not yet replaced;
  - 4K output and resolution-independent scaling without changing gameplay.
- **Generator** (user, 2026-10-12): online GPT Image 2.5 Sunburst, operated by the user (manual route: `docs/ai-image-guide.md`). Nothing is downloaded or installed locally.

## Route change (user, 2026-10-12): xBRZ for the whole game

- **Route now:** an algorithmic upscale of the original art, not AI regeneration. Every asset (all 10,552 sprite frames, the 7 level VRAM images, the 7 TGA screens, the HUD sheets, and the runtime catalog blocks through `smooth.py`) goes through index-preserving xBRZ at 9× (`docs/hires-pipeline.md`, "xBRZ route"). The goal: sharper, smoother art that plays on a 4K screen and stays portable to other platforms, while keeping the original assets fully intact (same palette indices, silhouettes, pivots and timing).
- **Why:** the GPT Image route looked good per frame but drew silhouettes 5–17 % off, lost detail when snapped to the original outline, and was inconsistent between frames (a block shared by many frames got one frame's redraw). It also needs a person to generate every pack.
- **Output:** 8-bit indexed PNGs (original palette indices), so the live CLUT, palette swaps, fades and semi-transparency stay exact. True-colour RGBA overrides remain supported for later hand or AI redraws, which win over the xBRZ art only where a key is redrawn.
- **Tool:** xBRZ (Zenju, GPL-3.0), vendored as source in `tools/hires/xbrz_lib/` (source and SHA-256 pinned there) and built locally with MSVC. It is an offline asset tool and is not shipped with the runtime.
- The GPT Image route (`docs/ai-image-guide.md`) stays available for optional redraws of single sequences.

## Web migration scope (W0, 2026-10-13)

The user requested implementation of the WebAssembly migration plan. The following release-scope section is
prepared from that plan; explicit confirmation of D-W0-1 is pending in
[W0-web-scope.md](../.plan/hercules-remaster/W0-web-scope.md).

- Development target: local and LAN builds. Public deployment remains outside the current authorization.
- The player supplies `HERCULES.FS`, `ENG1.DLL`, `ENG3.DLL`, `TITLE.DLL` and `HERCULES.EXE`; the EXE supplies the
  sine table and edition/hash verification data.
- Original files, derived art and game code are never hosted publicly or included in a distributable web build.
  Existing licence and distribution restrictions above continue to apply.
- D-W0-2 is confirmed: Emscripten SDK 6.0.12 is installed outside the game folder. See
  [toolchain.md](wasm/toolchain.md).
- Music policy (D-W0-3) and save persistence (D-W0-4) await the user's answers. Neither preference has been inferred
  from the absence of media or from the unverified behavior of Windows saves.

