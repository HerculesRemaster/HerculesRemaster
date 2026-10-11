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
