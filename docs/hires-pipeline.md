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
