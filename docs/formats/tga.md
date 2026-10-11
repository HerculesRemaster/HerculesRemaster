# TGA screens

65 entries: `M:\GRAFIX\MOVIE\SCREENS\*.TGA` (21), `M:\LANGUAGE\GRAFIX\MOVIE\SCREENS\*.TGA` (32: slideshow/story screens, `LEV*`), `…\SCREENS\PSWD\*.TGA` (12). Decoder: `tools/decode/tga.py` → `extracted/images/tga/<index>_<name>.png` (indexed) + `.json`; coverage `extracted/meta/images-tga-coverage.json` (64 ok, 1 truncated).

## Layout (identical for all 65)

| offset | value |
|---|---|
| 0 | id length 0, colour-map type 1, image type 1 (colour-mapped, uncompressed) |
| 3 | colour map: first 0, length 256, entry size 24 (BGR) |
| 8 | x/y origin 0, width 320, height 256, 8 bpp, descriptor 0 |
| 18 | palette, 768 bytes |
| 786 | pixels, 320 × 256 bytes |

## Rule from the game

TITLE loads the file whole with `RT_LoadFile` (slot 0x68). The EXE upload at +0xA67A sends payload bytes **786 …** to VRAM as a 160 × 256-unit rectangle (slot 0x204, `GFX upload` 0xa710), file row 0 → VRAM row y. **The file's first row is the top row** even though descriptor 0 means bottom-up in the TGA spec. That is why the CTPAX tool patches byte 0x11; `tga.py` follows the game instead. Verified: `analysis/traces/pairs/0584` and `0545` (decode = upload bytes, `tools/decode/compare_pairs.py`).

The screens are 256 rows tall; the 320 × 240 display shows part of them. Transparency is not used for these screens; the PNGs keep the full palette.

`0742_LEV8` is 5,120 bytes (16 rows) short of its header. The missing bottom rows are filled with index 0 and the file is flagged `ok_truncated`.

Palette conversion: TGA entries are B, G, R (`tools/decode/palette.py` `tga_palette`).
