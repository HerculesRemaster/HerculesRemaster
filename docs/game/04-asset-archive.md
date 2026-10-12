# 04. HERCULES.FS Asset Archive

46,924,407 bytes. Holds essentially all game content. Analysis below was done by parsing the file directly.

## Container layout [Verified]

- Starts with a **fixed index of 1,024 entries x 12 bytes = 12,288 bytes** (0x3000). First data starts at offset 12,288.
- Each entry is three little-endian uint32:

| Offset | Field | Notes |
|-------:|-------|-------|
| 0 | `id` | Resource key (probably a hash or packed type/slot code; see below) |
| 4 | `offset` | Absolute file offset of the payload |
| 8 | `size` | Payload byte length |

- All 1,024 entries are in range (offset+size <= file length). Sum of sizes = 46,912,119 bytes (index + 12,288 = file size, i.e. no gaps or trailing data).
- Payloads are **not stored in index order** and the table is **not sorted by `id`**; so look-up is likely by hash search.
- 204 entries have size < 4 (empty/placeholder slots).

## Entry IDs [Inferred]

Examples: `005C2A6A`, `010AF223`, `010AF323`, `010AF523`, `010BF223`, `010CF423`. Pattern: the low 16 bits repeat patterns like `F223 F323 F523 F723 F923`, and the byte above groups them in families (0x010A, 0x010B, 0x010C...; also 0x0201xx / 0x020Axx). Suggests `id = (group << 16) | (kind/slot << 8 | tag)` or a name hash. Needs confirmation by disassembling the lookup routine in `HERCULES.EXE`.

## Payload types seen (first-4-byte magic) [Verified counts]

| Magic | Count | Interpretation |
|-------|------:|----------------|
| `ESF\x03` ("ESF" version 3) | 559 | Most common type. Header after magic: `4E D0 00 00` `11 2B 00 00` ...; probably an Eurocom/"E" Sprite or Sound Format, versioned. Body looks compressed (high entropy). **Format unknown.** |
| `i1.4` | 109 | ASCII-tagged binary, followed by sorted-looking 4-byte offsets (an offset table). Likely animation/sprite/level container "version 1.4". |
| `i1.3` | 8 | Same family, older version tag |
| `i2.5` | 7 | Same family, v2.5. These are the **largest** entries (1.0-1.3 MB each), likely level maps or full-screen bitmaps |
| `i1.a` | 3 | Family variant, also large (1.0-1.2 MB) |
| `i1.9` | 1 | Family variant |
| other binary | 132 | Unclassified (some beginning `00 01 01 00`, `00 00 00 00`); palettes, tables, sound banks, scripts [Unknown] |
| tiny (<4 B) | 204 | empty |

Largest payload: entry #515 (1,304,576 bytes, `i2.5`).

By size (bucketed by powers of 4): 204 entries are under 4 bytes, 41 are 256 B to 1 KB, 178 are 1-4 KB, 300 are 4-16 KB, 135 are 16-64 KB, 120 are 64-256 KB, 39 are 256 KB-1 MB, and 7 are over 1 MB. The mix is many small items (probably frames, tables, SFX) plus a few large ones (probably backgrounds or levels).

## Probable content categories [Inferred]

Not yet verified, but typical of the game: character sprites (Hercules, Phil, Pegasus, enemies, bosses), tile sets, parallax backgrounds, level maps, palettes, sound effect banks (VAB-like, "vabid"), UI/title screens, tally screen graphics, text strings. No music and probably no FMV.

## How the EXE reaches data [Inferred]

`HERCULES.EXE` opens `hercules.fs` by relative name, reads the 12 KB index into memory, then looks up resources by `id` and reads (offset,size) payloads for the DLLs. DLLs receive decoded resources through the EXE.

## Tools to build first

1. `fs_extract.py`: dump all 1,024 entries to `extracted/<index>_<id>.<magic>.bin` plus `manifest.json` (index, id, offset, size, magic, SHA-256).
2. `entropy_report.py`: per-entry Shannon entropy to separate compressed from raw data.
3. Format decoders, one at a time, validated by rendering to PNG: `i*.*` family first (offset tables = easiest), then `ESF3`.

Details in doc 08.
