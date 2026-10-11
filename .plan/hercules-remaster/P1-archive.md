# P1 HERCULES.FS extraction + catalog

Goal: extract every archive entry byte-exactly, name entries where possible, and build a catalog that the decoders extend.
Needs: P0 workspace. Outputs: `tools/fs/`, `extracted/raw/`, `manifests/archive-manifest.json`, `docs/archive-format.md`, `tools/catalog/`.

## Tasks

- [x] Write `tools/fs/fs_index.py`. It parses the 1,024-record index with bounds checks and reports:
  - overlaps, gaps and trailing bytes
  - zero-size entries and duplicate ids
  - groups of identical payloads
- [x] Write `tools/fs/resource_hash.py`, the CTPAX name hash. It reads the packed name list from `hercstls/sources/namelist.c` at runtime and outputs name candidates per id. Don't copy the list into the repo.
- [x] Write `tools/fs/fs_extract.py`. It writes the exact bytes of each entry to `extracted/raw/<index:04d>_<id:08X>.bin`, without the TGA patch, and writes `manifests/archive-manifest.json`. Each manifest row has: index, id, offset, size, sha256, magic, name_candidate and extension.
- [x] Write `tools/fs/classify.py`. It assigns a family to each entry (by magic, size, name extension) and writes the result to `docs/archive-format.md`.
- [x] Write `tools/catalog/build_catalog.py`. It merges the manifest with the `extracted/meta/*-coverage.json` files into a catalog: index, id, name, family, decode status.
- [x] Optional: `analysis/imhex/hercules_fs.hexpat` for browsing the archive in ImHex.

All P1 tools print a summary only (totals, problem rows, output path). Run them as `rtk err .venv/Scripts/python.exe tools/fs/<tool>.py`, and inspect the manifest and catalog with `rtk json -d 2`.

Done when 820 entries are extracted, re-reading each one matches its slice of the archive, and the catalog exists.

## Notes

- 2026-10-11: The index matches FACTS. 820 entries have data, with no overlaps, gaps or trailing bytes. There are 204 zero-size records: 203 zero-id records from index 821, plus index 489 (id 439E67B2, which has no name). Duplicate ids: 0. There are 25 groups of identical payloads (53 entries), listed in `docs/archive-format.md`. Record order is not offset order.
- 2026-10-11: The name hash decodes 866 names. It resolves 804 of 821 ids with no collisions; 62 names aren't in the archive. Extension counts match FACTS. The 16 unnamed payloads include 8 `i1.3` entries with consecutive ids AEA7A830..37 (indices 631–638). `fs_extract.py` wrote 820 files (46,912,119 bytes = archive minus index), and every file re-reads equal to its slice.
- 2026-10-11: Families: esf_audio 559, anim_bin 108, tga_cmap 65 (all 320x256 8 bpp, descriptor 0), sonix_sfx 23 and sonix_mus 12 (all 452 bytes), map_cif 16 (all 336,384 bytes), icontainer_unnamed 12, map_bin 10, demo_dat 8 (4,096 bytes), lang_dat 3, unknown 4. Decoder coverage contract: `extracted/meta/<decoder>-coverage.json` (see `build_catalog.py` docstring). `rtk err` needs a backslash path (`'.venv\Scripts\python.exe'`). Next: P2.
