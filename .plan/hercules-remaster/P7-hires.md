# P7 4K asset pipeline + pilot

Goal: a resumable pipeline that turns original assets into 4K replacements without changing size, pivot or timing, and a pilot set the user approves in motion.
Needs: P4, P5, P6.
Outputs: `tools/hires/`, `remastered/`, `docs/hires-pipeline.md`, `docs/art-guide.md`.

## Pipeline

- [x] **[HUMAN]** Ask the user:
  - the authorization scope (art, music, AI processing, distribution), recorded in `docs/authorization-scope.md` (Q-02)
  - the art route: which AI upscaler/model and licence, hand redraw, or a mix (Q-06)
- [x] Derive the logical view → 3840x2160 scale, the aspect policy (Q-04) and per-family texture budgets.
- [x] Write a replacement-manifest schema and `tools/hires/contract.py`. Each entry holds source identity, geometry, timing, provenance and output.
- [x] Renderer: true-colour RGBA overrides (Q-06): fog/shade/blend in colour space, indexed fallback, palette fades applied to true-colour pixels.
- [x] Write `tools/hires/jobs.py`, a resumable job state machine:
  - states: pending → generated → review → approved → packed
  - identity = hash(source + settings)
  - bounded retries
- [x] Write `tools/hires/checks.py`, which checks dimensions, alpha, pivot × scale, frame count and tile seams.
- [x] Write `tools/hires/atlas.py`, which packs approved assets with padding/extrusion and reports texture memory.
- [x] Add the generator adapter for the approved route. If it needs a tool, pin its version and SHA-256 with the user's OK.

## Pilot

- [x] Choose the pilot set: hero idle/run/attack, one enemy, one background section, one repeating tile, one UI element.
- [x] Make reference sheets per sequence and the art-guide pilot section.
- [x] Generate the pilot through `jobs.py` until the checks pass. Render side-by-side GIFs at game speed and run the slice at 3840x2160 with the pilot assets.
- [x] **[HUMAN]** The user approves the pilot in motion or lists corrections. (Delegated: "please run and review for yourself", 2026-10-12.)

Done when the pilot is approved. Bulk art starts only after that.

## Notes
- Q-02/Q-06 answered (2026-10-12): full AI CGI regeneration authorized, no distribution; true-colour overrides → `docs/authorization-scope.md`. Generator/model still to be chosen (adapter task).
- Scale 9 (2880×2160 pillarboxed), budgets and true-colour rules → `docs/hires-pipeline.md`. The CIF tables are the PSX formulas quantized to the nearest palette colour (shade c·b/128, blend (s+d)/2, s+d, d−s, d+s/4); fog rows are identity in all 7 levels.
- `tools/hires/contract.py` → `remastered/manifest.json`: 10,552 sprite frames (83,569 part keys, mirrored parts share keys), 7 level VRAM images (15,879 keys incl. sky strips), 7 TGA screens. Covers 4,554 of the 4,712 runtime catalog keys; the rest are scaled/rotated quads.
- True-colour renderer: index target R16 (bit 8 = true colour) + colour target RGBA8 (alpha-blended for RGBA art). Semi: exact tables when both pixels are indexed, else PSX formulas; shade c·b/128. Indexed PNG overrides stay exact. Regression: same diff counts as before; RGBA test (bank 560 parts, tinted, soft alpha) drew correctly at 30.3 fps. The EXE never fades through the palette (`GFX_SetPaletteEntry` callers: init, CIF, level palette, video), so true-colour pixels need no palette transform.
- `tools/hires/{common,jobs,checks,atlas}.py` + adapters `nearest` (pipeline test only) and `manual` (`remastered/incoming/`). Checks: size, alpha (≤3 % of opaque texels), pivot (centroid ≤1 px), seams for level tiles; bank frame count in pack. End-to-end test on bank 560 (443 frames, nearest): 2,701 key files, 270 MB at 9× RGBA; packed art drew in place at 30.3 fps; test outputs removed. Sprite references use the bank palette, which equals the CIF palette on the hero's 35 indices (≤7, 15-bit rounding) in all 7 levels.
- Pilot set → `tools/hires/pilot.json`, `docs/art-guide.md`: hero idle/run/sword thrust (bank 560), bird flap/dive (485), TRAINING VRAM image + tile 936 (576,0,64,64), HUD sheet `STPANEL1.DAT` (raw 256×512 8-bit, found by texel search; new `ui` family with all gauge widths/heights). `tools/hires/refsheet.py` → `remastered/reference/` (sheets on a common pivot + game-speed GIFs; ticks from runtime anim events). Catalog coverage now 4,673 of 4,708 keys. `art_dump` also writes `dump/index.csv` (key, VRAM rect, screen position, frame, module).
- Generator (user, 2026-10-12): online GPT Image 2.5 Sunburst, run by the user, so no download or pinning. Adapter `manual` + `tools/hires/prompts.py` (packs: input sheet on magenta at the model's output size, prompt, layout; level pages split on free cut lines into ≤160-texel pieces at 7×) + `tools/hires/fit.py` (magenta key with despill, premultiplied Lanczos to 9×, silhouette snap, writes `remastered/incoming/`). Guide `docs/ai-image-guide.md`. 60 pilot packs in `remastered/prompts/`. Fit + checks tested with simulated outputs; checks catch shifts and size errors, seam check limited to opaque edges.
- Label fix: bank 560 frames 465–468 are the fall loop (script 0x53df8), and bank 765 frames 1–8 (path HC_DIE) are the breathing idle of script 0x53830 (`anim_labels.json`, pilot, art guide).
- Next: [HUMAN] the user generates the pilot packs; then jobs run/review/approve/pack and the in-game pilot check.
- 2026-10-12: user results for 57 of 60 packs taken (training_page_1_0_1, 1_0_2, 2_1_2 still coming). The model drew figures 5–17 % larger and shifted inside cells, so the silhouette snap cut the run frames apart (IoU 0.17–0.6). `fit.py` now aligns each frame cell (scale ±30 % + offset, max silhouette IoU) before the snap: run_1/run_3/idle/sword 2–3 reach IoU 0.7–0.88 and look whole. hero_run_2 (frames 4–7, 14, 15) stays at 0.56–0.72 with different poses: regenerate. `tools/hires/motion.py` → `remastered/review/motion/*.gif` (original | new at game speed). Bird flap/dive and sword thrust 1 were already approved+packed (05:33) and were not re-taken. TRAINING pieces: seam warnings 601→1319 of 2537 tiling edge pairs as pieces were added; judge in game.
- 2026-10-12 (later): the last three TRAINING pieces (training_page_1_0_1, 1_0_2, 2_1_2) taken; all 60 packs are now in review except the already packed bird and sword-thrust-1 frames. TRAINING seam check: 1,472 of 2,537 tiling edge pairs differ across independently redrawn pieces; judge in game. Waiting on the user: approve/reject per pack (`jobs.py approve|reject`), regenerate hero_run_2.
- 2026-10-12 (session 3): user delegated the pilot review to Claude. Review found (a) stair-step cuts (face sliced at the nose, hands, feet): `fit.snap` used the 9× nearest silhouette + 4 px; now blurred/re-thresholded rounded mask, default `--grow 9` (1 logical px); (b) magenta fringes on foliage/figure edges: `fit.key_out` despills pink within 6 px of the key. All packs re-taken. `jobs.py preview` packs review+approved art into `working/game/art_preview/` (states unchanged). hero_run_2 rejected (poses differ, head/feet lost after align) → regenerate; the rest approved after the sheet review.
- Pilot review in game (TRAINING, 9×): sky band showed a grid: the background upload (VRAM 1536,256 512×256) holds 16-row panorama strips whose flat cells the game reuses as sky fill, and the model painted clouds into them. `atlas.py` now (a) makes self-tiling level blocks periodic (edge mismatch spread over a band), (b) gives flat background blocks (dither-blurred std ≤ 10) a smooth upscale of the source instead of the redraw (restricted to background uploads: elsewhere the reference palette is not the block's CLUT, flat ground turned purple), (c) holds a pilot sequence back until all its frames are approved (hero_run waits for hero_run_2). Pilot approved; open: regenerate hero_run_2 (frames 4–7, 14, 15).
- 2026-10-12 (route change, user): the whole game switches from GPT Image regeneration to an algorithmic upscale: index-preserving xBRZ at 9× (`docs/authorization-scope.md`, `docs/hires-pipeline.md` "xBRZ route"). New: `tools/hires/xbrz_lib/` (vendored xBRZ source, GPL-3.0, pinned; `build.bat` → `xbrz.dll` via MSVC), `tools/hires/xbrz_index.py` (xBRZ 6× on colours → linear 9× → per-pixel pick of a 3×3-neighbour source index: original indices only), generator `adapters/xbrz.py` (sprites per frame canvas from bank texels; level/ui/screen per block with wrap where the source tiles, plus `<stem>.blocks/` sidecars that `atlas.py` and `checks.seams` prefer). `jobs.py run --jobs N` (process pool) and `review --no-sheets`; `smooth.py --method xbrz` for catalog blocks. Screen references are decoded from the TGA bytes (the manifest's reference PNG names did not exist). First results: sprites/UI/screens 0–0.2 % alpha diff, pivot ≤ 0.04 px; FOREST 0 broken tiling edges with sidecars (94 of 1050 when cut from the shared canvas). Old jobs saved as `remastered/jobs.ai-pilot.json`.

