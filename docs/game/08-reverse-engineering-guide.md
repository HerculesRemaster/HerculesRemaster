# 08. Reverse-Engineering Guide

How to turn the unknown formats into usable data. Work on copies, never on the original files.

## Tooling

| Need | Tool |
|------|------|
| Disassembler / decompiler | Ghidra (free) or IDA Free; load as PE x86 32-bit |
| Dynamic debugging | x32dbg (run EXE, break on `CreateFileA`/`ReadFile` for `hercules.fs`) |
| API tracing | API Monitor, Process Monitor (registry + file access) |
| Hex / structure | ImHex (pattern language), 010 Editor |
| Scripting | Python 3 (`struct`, `numpy`, `Pillow`) |
| Pack inspection | Detect It Easy (confirm the EXE/DLLs are not packed; section names look standard so likely unpacked) |
| DirectDraw capture | dgVoodoo2/cnc-ddraw debug builds, or RenderDoc via a wrapper |

## Step 1: Extract the container

Index format is already known (see doc 04). Reference extractor:

```python
import struct, hashlib, json, os, sys
src, out = sys.argv[1], sys.argv[2]          # HERCULES.FS, assets_raw/
data = open(src, 'rb').read()
os.makedirs(out, exist_ok=True)
man = []
for i in range(1024):
    eid, off, size = struct.unpack_from('<III', data, i * 12)
    blob = data[off:off + size]
    magic = blob[:4]
    tag = magic.decode('ascii', 'replace') if magic[:3] == b'ESF' or magic[:1] == b'i' else 'bin'
    name = f'{i:04d}_{eid:08X}.{tag.strip(chr(0))}.bin'
    open(os.path.join(out, name), 'wb').write(blob)
    man.append(dict(index=i, id=f'{eid:08X}', offset=off, size=size, magic=magic.hex(),
                    sha256=hashlib.sha256(blob).hexdigest()))
json.dump(man, open(os.path.join(out, 'manifest.json'), 'w'), indent=1)
```

(Validate: `sum(size) + 12288 == filesize`; this holds for the file here.)

## Step 2: Find how the EXE reads resources

1. In Ghidra, search strings for `hercules.fs`; follow cross-references to the open/read routine.
2. Look for a loop reading 0x3000 bytes (the index) and a search over 12-byte records: this reveals how `id` is computed (hash of a name? packed fields?).
3. Look for callers that pass IDs: this gives a name for each asset type.
4. Confirm with x32dbg: break after the index read and watch the IDs requested while a level loads.

## Step 3: Decode the `i*.*` family (`i1.4`, `i1.3`, `i1.9`, `i1.a`, `i2.5`)

Observed: 4-char tag, then a table of increasing 4-byte values (likely offsets or cumulative sizes), then data.
- Hex-dump a small one, test "table of N offsets -> N sub-blocks".
- Sub-block candidates: palette (768 or 512 bytes), bitmap rows, RLE spans, tile maps.
- Render guesses to PNG with Pillow using candidate palettes; correct by eye.

## Step 4: Decode `ESF\x03`

Header seen: `45 53 46 03 | 4E D0 00 00 | 11 2B 00 00 | 00 01 00 00 | 00 00 00 00 | ...`
- Two uint32 after the magic (0xD04E = 53,326 and 0x2B11 = 11,025) may be sizes, or 11,025 might be an audio sample rate! 11,025 Hz is a common SFX rate [Inferred]; so ESF may be a **sound** container, not images. Check other ESF entries: if the second value is often 11025/22050/44100, they are audio.
- If audio: strip the header, try raw PCM8/16 or ADPCM (PS1 VAG/ADPCM matches "vab" strings) and play in Audacity.
- If not: look at the decompressor in the DLL (search for loops writing to output with literal/match tokens).

## Step 5: Identify palettes and render

Look for entries of exactly 768 (RGB) or 1024 (RGBA) bytes, or 512 (RGB555/565 x256). Apply to decoded index images.

## Step 6: Disassemble the DLLs

- ENG1/ENG3/TITLE import only `kernel32`/`user32`, so game logic does not touch DirectX; find exported functions (`GetPos`, `GetPosEx` are visible names) and the callback table the EXE passes in.
- Name the hero state machine from strings `HC_DIE`, `HC_DUNIT`, `HC_THROW`.
- Dump constants near physics code (gravity, jump velocity, speeds).

## Step 7: Ground truth for gameplay

Use recorded video (Phase 0) plus a frame counter: measure jump height/length, enemy patrol ranges, damage values. Store as `tuning.json`.

## Step 8: Validation

Every decoder must produce an artefact you can see or hear (PNG, WAV, level render) and the manifest must show decoded/total counts. Track progress in `asset-catalog.md`:

| index | id | magic | size | decoded? | what it is |
|-------|----|-------|-----:|:--------:|------------|

## Safety notes

- Analyse files only; do not run unknown installers. `unins000.dat` is a data log, not executable.
- Run the game in a VM if you are unsure about the origin of this copy (third-party repack).
- Never commit original game data to a public repo.
