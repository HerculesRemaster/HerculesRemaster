# i-family containers

| tag | count | content | status |
|---|---:|---|---|
| `i1.4` | 109 | sprite animation banks (`…\ANIMPSX.BIN`, `P:` = `M:\GRAFIX\CHOP\`) | 107 decoded (10,552 frames), 2 unsupported variants (503, 601) |
| `i2.5` | 7 | level maps (`M:\GRAFIX\MAPS\<LEVEL>\MAP_PSX.BIN`) | P5 |
| `i1.a` | 3 | level maps (CYCLOPS, TORMENT, GAUNTLET `MAP_PSX.BIN`) | P5 |
| `i1.3` | 8 | unnamed (631–638), large | P5 (probably maps) |
| `i1.9` | 1 | unnamed (409) | P5 |

Decoder: `tools/decode/ifam.py` → `extracted/sprites/<index>_<path>/f<frame>.png` (RGBA, transparent background), `bank.json`, `palettes.png`; coverage `extracted/meta/sprites-coverage.json`; contact sheets `extracted/images/_sheets/` (`tools/viewer/contact_sheet.py`). Map tags raise `UnsupportedVariant` here.

## `i1.4` sprite bank (rules from ENG1.DLL)

`P` = file + 4, the pointer ENG1 calls `puVar2` in 0x3cd0.

| where | type | meaning |
|---|---|---|
| file+0 | char[4] | `i1.4` |
| P+0 | u16 | frame count (e.g. Hercules 593) |
| P+2 | u16 | palette count |
| P+4·k | u32 | frame *k* entry (k = 1 … count; slot 0 is the count word. Frame `count` exists: ENG3 draws CYCLOPS 167/167, CYEND 33/33, ALIEN1 828/828, SOULS 20/20 at runtime, 2026-10-12): `offset << 8 \| flags`. `offset` is relative to P (0 = no frame). `flags & 0x1f` = palette; `0x40` = keep cached (cells allocated from the reserved range 0x54–0x6f in 0x4ca0) |
| P+4·count+4, +8 | u32 ×2 | copied to a global by 0x3cd0 when a frame is first cached (meaning open) |
| P+4·count+12 | u16 × palettes | palette offsets (relative to P) |

The bank table in ENG1 (0x5cee0, 0x14 per bank: name pointer, u16 count, …, +0xc data) points at the loaded file.

### Palette (0x3cd0 → 0x5780)

`u16 n, u16 n, n × u16 PSX colour` (BGR555, bit 15 = STP). 0x5780 builds a 256-entry CLUT: n = 256 is copied as is; otherwise entry 0 = 0 (transparent) and entries 1…n−1 get `| 0x8000`, repeated every n entries. PSX 0x0000 = transparent. The palette used is the object's (`obj+0x1c`) unless that is 0xff, in which case it is the frame entry's `flags & 0x1f`. Runtime (`demo_pal`): Hercules is drawn as two objects, one with the entry palette (0, 256 colours) and one with `obj+0x1c` = 3 (32 colours, other entries 0, so only part of the sprite shows: an overlay layer). The draw wrapper 0x3bd0 can draw a second frame (`obj+0x36`) when object flag 0x8000000 is set.

### Frame record (P + offset)

| field | type | notes |
|---|---|---|
| parts | u8 | |
| boxes | s8 | |
| boxes × 6 bytes | | per-frame boxes (collision/hit; meaning of the 6 bytes is P5 work) |
| clip | s8 × 4 | passed to 0x30d00 (off-screen test) |
| dx, dy | s8, s8 | frame offset; dx is negated when the object's flip bit (`obj+0x54` & 0x10) is set (0x4010) |
| flags[0] | u8 | flags of part 0 |
| parts × {size u8, x s8, y s8, flags[i+1] u8} | | draw quad i: width = `(size & 15)·8`, height = `(size >> 4)·8`, position = `(dx + 2x, dy + 2y)` from the object's x/y |
| pixel stream | | starts right after the last part record |

### Pixel stream (0x4ca0)

For each part, in order:

- `flags & 0x80`: no pixels of its own. The texture is the previous block at offset `((flags & 15)·8, (flags >> 1) & 0x38)`.
- Otherwise, a block of `((flags & 3)·8 + 8)` × `((flags & 0xc)·2 + 8)` bytes, row-major, byte RLE:
  - signed byte ≤ 0 (0x00, 0x80–0xff): one literal pixel;
  - 0x02: end of block;
  - other 0x01–0x7f: run length, then the value byte.
- The game copies the block into a 32 × 32 VRAM cell (8 bpp, 16-px quadrants chosen by the cell allocator) through staging 0x72870. 0x50d0 uploads the queued cells (slot 0x204).

### PC colours

On the PC the sprite rasteriser (EXE 0x40cab0) copies texel bytes straight into the 8-bit frame, so in-game sprite colours are `CIF palette[texel]` of the current level, not the bank's 15-bit palette. The bank palettes give the PSX colours. The PNGs use the bank palettes (closest to the art as drawn); the GPU renderer uses the PC rule.

### Checks

Hercules, Phil, the title menu bank and others are visually correct: shapes, palettes, cape and transparency match the game (contact sheets). The runtime frame sequence for Hercules is in `docs/resource-consumers.md`.

### Open

Banks 503 and 601 (no names) have small palettes (68-byte stride) and a stream that doesn't follow the ENG1 RLE. They are probably drawn by other code (TITLE or ENG3); `UnsupportedVariant` until that code is read.

**S4.02 (2026-10-12): no consumer in this build.** The unnamed entries are 503 and 601 (`i1.4`), 631–638 (`i1.3`, consecutive ids 0xAEA7A830..37) and 409 (`i1.9`).
- No path string in HERCULES.EXE, TITLE, ENG1 or ENG3 hashes to their ids. The scan covered the strings as written, with P: mapped to `M:\GRAFIX\CHOP\`, and with the known leaf names (`ANIMPSX.BIN`, `MAP_PSX.BIN`, …).
- Neither bank table (ENG1 0x5cee0, ENG3 0x39548; `tools/decode/bank_index.py`) names them, and no trace run opens them.
- 409 matches the invented combination `M:\GRAFIX\CHOP\SRC1\BOSS\MEDUSA\MAP_PSX.BIN`. Nothing in the code opens that path, and the additive hash collides easily, so it is a LEAD only.
- Result: they stay `UnsupportedVariant`, reason "unreferenced by this build". Re-check this against the linked full-game run (S6.08).
