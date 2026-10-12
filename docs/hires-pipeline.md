# 4K replacement-art pipeline (P7)

Scope and route: `docs/authorization-scope.md`: AI CGI regeneration, true-colour overrides, indexed fallback. Runtime: `docs/gpu-renderer.md`.

## Scale, aspect, budgets

- **Logical view:** 320×240 (all game logic, positions and pivots).
- **Output:** 3840×2160 → integer scale 9 → a 2880×2160 view, pillarboxed (Q-04 "borders": 480 px black bars). Art scale k = 9, so one art pixel equals one screen pixel. Art at another integer k (e.g. 6 for distant level textures) is still valid; the renderer requires only the same integer factor on both axes.
- **Hardware:** RTX 3090 (24 GB), but the game is a 32-bit process. Decoding stays on the worker thread and is transient, and textures live on the GPU (`IMMUTABLE`).

| family | source | 1× texels | 9× RGBA8 | resident when |
|---|---|---:|---:|---|
| sprite frames | 109 `i1.4` banks, 10,552 frames (canvas = frame bbox) | 47.3 M (all banks) | 14.3 GB (whole game, on disk) | per loaded bank; typical level ≈ 10–20 banks |
| level textures | level VRAM images: 1536×512 + 512×256 8-bit texels (`vram.bin`) | 0.92 M per level | 297 MB per level | while the level runs |
| screens / UI | TGA 320×256 (130 files), HUD/menu banks | 82 K per TGA | 25 MB per TGA | while shown |

- **Resident budget:** ≤ 2 GB per level at 9× RGBA8. Block compression (BC7, 4:1) is a P8 option if it is exceeded.

## Runtime keys (what the renderer replaces)

Each textured primitive is keyed by FNV-1a 64 of (w, h, its w×h VRAM texels) (`gpu_renderer.cpp`). An override is `<art_dir>/<key>_<w>x<h>.png` at k·w × k·h. Content keys mean the pipeline never needs runtime addresses, only the texels the game uploads.

| family | how the key is derived offline | check |
|---|---|---|
| sprite part | `ANIM_BuildSpriteQuads` (ENG1 0x4010) draws each frame part as a w×h rect of the part's block (`i1.4` RLE block, sub-rect `sx, sy` for reused blocks). Mirrored parts are keyed like unmirrored ones | 3,249 of the 4,712 catalog keys (TRAINING demo + title) come from bank frames |
| level quad | the T30 texture words give UV and tpage per quad; the key is the UV bounding box in the level VRAM image | P7 contract |
| screen / strip | content search of the catalog blocks in the decoded TGA / VRAM images | P7 contract |

**Authoring unit.** Art is generated per **source image**: a whole sprite frame canvas, a level VRAM image, or a TGA. The pipeline then slices it into key files. So neighbouring parts and tiles come from one image (no seams), and a frame's pivot is unchanged: same canvas, same origin × k.

## True-colour rules (for RGBA art)

Fitted against the CIF tables of all 7 levels. Each table entry is the palette colour nearest to the formula: shade 90–97 %, blend 94–100 % of sampled entries.

| table | rule in colour space |
|---|---|
| fog rows (31 per level) | identity in every level's CIF (all rows = 0…255), so no change. The flat-fill LOD (fog off) uses the art colour at the averaged UV |
| shade `shade[b][i]` | `min(c · b / 128, 255)` |
| blend mode 0 / 1 / 2 / 3 | `(s + d) / 2`, `min(s + d, 255)`, `max(d − s, 0)`, `min(d + s / 4, 255)` |

## Manifest (`remastered/manifest.json`, built by `tools/hires/contract.py`)

```json
{
  "version": 1,
  "scale": 9,
  "entries": [{
    "id": "sprite/0560/f0001",
    "family": "sprite | level | screen",
    "source": {"kind": "i1.4", "index": 560, "name": "...", "frame": 1, "sha256": "<source bytes>"},
    "geometry": {"size": [48, 64], "origin": [26, 62],
                 "blocks": [{"key": "<16 hex>", "rect": [x, y, w, h]}]},
    "timing": {"note": "frame shown by ENG1 scripts; never changed"},
    "reference": "extracted/.../f0001.png",
    "output": "remastered/art/sprite/0560/f0001.png"
  }]
}
```

- **`blocks[].rect`** is in source-image pixels (1×). The output image is `size × scale` and is cut at `rect × scale` into `<key>_<w>x<h>.png`.

## Jobs, checks, packing

- `tools/hires/jobs.py`: resumable job states `pending → generated → review → approved → packed`. Job identity = sha256(source + settings), with bounded retries.
- `tools/hires/checks.py`: verifies
  - size = source size × k;
  - alpha: transparent where the source is transparent, opaque where it is not;
  - pivot (origin × k unchanged);
  - frame count per bank;
  - tile seams across block edges.
- `tools/hires/atlas.py`: slices approved images into key files in `working/game/art/` and reports texture memory.

## xBRZ route (whole game, 2026-10-12)

Generator `xbrz` (`tools/hires/adapters/xbrz.py`) on every manifest entry; `tools/hires/smooth.py` covers the runtime catalog blocks that no entry holds. Route decision: `docs/authorization-scope.md`.

**Algorithm** (`tools/hires/xbrz_index.py`):
1. xBRZ 6× on the texel colours (`tools/hires/xbrz_lib`, built with MSVC on first use).
2. A linear resample from 6× to 9× (premultiplied alpha).
3. Index pick. Each 9× pixel takes the palette index of the texel in its 3×3 source neighbourhood whose colour is closest (centre texel first on ties).

The output is an 8-bit indexed PNG with the original indices only:
- no new colours, and index 0 stays transparent;
- the runtime applies the live CLUT, so palette swaps, fades and semi-transparency stay exact;
- the PNG palette is only for viewers.

**Per family:**

| family | unit | indices | colours |
|---|---|---|---|
| sprite | the whole frame canvas, so outlines continue across parts | the frame's parts pasted in draw order (i1.4 texels, as `contract.py` keys them) | decoded reference frame |
| level | each manifest block, on its own | `vram.bin` | level palette |
| ui | each manifest block, on its own | raw `STPANEL*.DAT` | TRAINING palette |
| screen | each manifest block, on its own | TGA pixels in file row order (as the blocks were searched) | TGA palette |

**Per-block families (level, ui, screen):**
- Edges are clamped. They wrap where the source block tiles (opposite edges within 12 mean RGB, the `checks.seams` test).
- Overlapping rects of one canvas cannot all keep their exact edges: a level has the UV bounding box, the exact rect and the mirrored rect of one texture. So the generator also writes each block's own result to a sidecar folder next to the image, `<stem>.blocks/<key>_<w>x<h>.png` (`common.blocks_dir`).
- `atlas.py` packs the sidecar file, and `checks.seams` tests it, instead of the rect cut from the canvas.
- The canvas is still written, for review: blocks pasted smallest first, tiling blocks last.

**Transparency:** the runtime drops texel index 0 only (`gpu_renderer.cpp`). The decoded sprite references also drop PSX colour 0x0000, which some ENG3 palettes use for other indices (bank 482, TORMENT). So for sprites the generator uses `index != 0` as the silhouette, and `checks.py` tests indexed sprite art against the index canvas (`common.sprite_indices`).

**Files:** written to `remastered/incoming/xbrz/<id>.png` (plus `.blocks/`) and hard-linked to the entry's output path.

**Checks** (first runs): sprites, UI and screens have 0–0.2 % alpha difference and pivot moves ≤ 0.04 px; levels have ≤ 0.01 % alpha difference and no broken tiling edges with the sidecar.

**Whole-game run** (from `tools/hires`):

```
py jobs.py add . --generator xbrz        # every manifest entry (identity change resets older jobs)
py jobs.py run ^sprite/ --jobs 3         # worker processes; sprites in bank-sorted chunks of 16
py jobs.py run . --lenient               # levels, UI, screens one at a time (a level needs ~1 GB); seams warn
py jobs.py review . --no-sheets          # no side-by-side sheets for 10k entries
py jobs.py approve .
py jobs.py pack                          # art/<family>/ (sprite, level, ui, screen); sidecars used
py smooth.py --out ../../working/game/art/smooth   # catalog fallback, xBRZ where the catalog has colours
```

**Tiny sprites:** a frame whose xBRZ silhouette differs from the index canvas by more than `checks.ALPHA_MAX` (dots, 1-texel lines; 5 frames in the game) uses index Scale3x twice instead.

**Seam warnings:** levels report 3–110 broken pairs each. All are on partly transparent UV-bounding-box blocks whose source edges sit just under the tiling threshold, not on repeating textures. No seams were visible in game.

**Timing and memory:**
- Sprites take 0.1–0.4 s per frame, levels 2.5–4.5 min each.
- `--jobs 4` keeps the commit charge low (each worker holds the manifest and one bank's decode).
- `smooth.py` keeps existing files, so write it to a fresh folder to rebuild.
