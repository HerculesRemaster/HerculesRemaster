# Art guide (4K replacements)

Route (2026-10-12): index-preserving xBRZ upscale of the original art for the whole game; optional AI or hand redraws on top (`docs/authorization-scope.md`). Pipeline and checks: `docs/hires-pipeline.md`. The rules below apply to every route; the xBRZ route meets them by construction (same canvas, indices and silhouette).

## Hard rules (checked by `tools/hires/checks.py`)

- **Canvas:** an output is the source canvas × 9 (sprite frame bbox, level VRAM image 2048×512, HUD sheet 256×512, TGA 320×256). Never crop, pad or recentre: the pivot is the source origin × 9.
- **Silhouette:** opaque where the original texel is opaque and transparent where it is transparent, per logical pixel (≤ 3 % may differ). Gameplay boxes and hit tests come from the original data, so a larger silhouette would show hits that look wrong. Soft edges are fine (RGBA alpha).
- **Timing:** one output per original frame. The game decides when each frame is shown (`remastered/reference/timing.json`); never add, drop or merge frames.
- **Tiles:** a level texture whose source tiles (matching opposite edges) must tile in the output.
- **Format:** PNG, RGBA (true colour) or 8-bit indexed (CIF palette indices, exact original lighting). True-colour art gets the game's shading and semi-transparency in colour space.

## Consistency

- **One character model:** a character uses one model sheet for every frame of every sequence, with the same proportions, costume and colour identity as the original (Hercules: red-brown tunic, blue cape, bronze bracers; bird: dark blue-black plumage, red eye).
- **Light:** light from the top left, as in the original art. Don't bake rim light or drop shadows into sprites; the game draws its own shadow (semi-transparent shade primitive).
- **Generation:**
  - generate per sequence in play order, with a fixed style reference and seed;
  - review the GIF at game speed for flicker before approving;
  - mirrored facing is drawn by the game from the same frame, so art must look right when mirrored (no text, no one-sided logos).
- **Level textures:** generate per texture rect (the manifest blocks), not the atlas as one picture, because neighbouring rects in VRAM are unrelated. Then compose them into the atlas canvas. Repeated rects must stay identical.
- **UI:**
  - glyphs keep their boxes, and digits stay legible at 1×-equivalent size;
  - the health bar and gauge are drawn as growing slices of one strip, so the strip must read correctly when cut at any width or height.

## Pilot set (`tools/hires/pilot.json`, sheets in `remastered/reference/`)

`tools/hires/refsheet.py` writes for each sprite sequence:
- a sheet: frames at 3× on a common pivot (red cross), with frame number, display ticks, canvas and pivot;
- a GIF at game speed (33 ms per tick).

Ticks come from the runtime anim events (TRAINING demo).

| item | source | frames / region | ticks per frame |
|---|---|---|---|
| hero idle | bank 765 (path `HERO\HERCULES\HC_DIE`; the breathing idle of script 0x53830) | 1–8 (loop 1…8, 7…4) | 7, then 4 (8: 5) |
| hero run | bank 560 | 2, 3, 10–15, 4–9 (loop from 10) | 1, 1, then 2 |
| hero attack | bank 560, sword thrust | 211–217, 246, 247 | 2 (217: 1) |
| enemy | bank 485 `ALIEN2\STYBIRD` (Stymphalian bird) | flap 9–16 (loop), dive 17–24 | 1 (23, 24: 2) |
| background section | `level/TRAINING` VRAM image | all textures of the P6 slice | static |
| repeating tile | `level/TRAINING` texture 936, VRAM rect (576, 0, 64, 64) | most used quad texture (27 quads), tiles vertically | static |
| UI element | `ui/STPANEL1` (`STPANEL1.DAT`, 256×512 HUD sheet) | clouds, orbs, health bar, gauge, fonts | orb frames by game code |

Approval: the pilot runs in the P6 slice at 3840×2160 next to the original, with side-by-side GIFs at game speed. Bulk art starts only after the user approves it in motion.
