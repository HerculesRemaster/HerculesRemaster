# Guide: Extract Disney's Hercules (1997) Assets with ImHex 1.38.1

For remastering Disney's Hercules on Windows 11, use ImHex 1.38.1 to reverse-engineer the original game asset formats, then extract the assets in batches for AI upscaling, redrawing, and eventual integration into a modern rendering engine.

There is an important discovery: Disney's Hercules already has a dedicated archive extraction tool called `UNHERCFS`, which was released with source code. This can save considerable reverse-engineering work.

## 1. Understand which files contain the game assets

The known PC distribution includes these important files:

| File                                | Purpose                                                            | Priority |
| ----------------------------------- | ------------------------------------------------------------------ | -------- |
| `HERCULES.FS`                       | Main game data archive; primary extraction target                  | Critical |
| `HERCULES.EXE`                      | Main executable; investigate loading, rendering and gameplay       | High     |
| `ENG1.DLL`, `ENG3.DLL`, `TITLE.DLL` | Game-related DLLs to inspect for asset loading and engine behavior | High     |
| `VIDDATA\*.ETV`                     | Original game video data                                           | Medium   |
| `MUSIC\*.ogg`                       | Music in certain digital editions                                  | Medium   |
| Extracted `.ESF`                    | Game audio format supported by a known conversion utility          | Medium   |

The Steam version lists `HERCULES.FS` at approximately 44.75 MiB, alongside numerous `.ETV` files. Your original CD/ISO edition may have a different directory layout.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://steamdb.info\&sz=32)

SteamDB

+1



Existing extraction tools:

- [Disney's Hercules Tools v1.1 — UNHERCFS + ESFTOWAV](https://www.ctpax-x.org/index.php?goto=files\&lang=en\&show=114) — archive unpacker and audio converter, with C source code.
- [ImHex Hex Editor documentation](https://docs.werwolv.net/imhex/views/hex-editor) — hexadecimal inspection and editing.
- [ImHex GitHub repository](https://github.com/WerWolv/ImHex).

## 2. Prepare your Windows working directory

Use the following structure, keeping original files separate from extracted and edited assets.

```
C:\Games\GameHercules\
│
├── HERCULES.EXE
├── HERCULES.FS           (if present)
├── ENG1.DLL
├── ENG3.DLL
├── TITLE.DLL
│
└── Remaster\
    ├── backups\
    ├── analysis\
    │   └── imhex\
    ├── tools\
    │   └── unhercfs\
    ├── extracted\
    │   ├── raw\
    │   ├── images\
    │   ├── sprites\
    │   └── audio\
    ├── remastered\
    │   ├── images_4k\
    │   └── sprites_4k\
    └── manifests\
```

First, locate the archive from PowerShell:

```
Get-ChildItem "C:\Games" -Recurse -File `
  -Include *.FS,*.ETV,*.ESF `
  -ErrorAction SilentlyContinue |
  Select-Object FullName,Length
```

If `HERCULES.FS` is not in your installed game directory, inspect `C:\Games\HerculesInstaller` and the mounted original ISO. Work on a copy rather than modifying the archive directly.

## 3. Open `HERCULES.FS` in ImHex 1.38.1

Step 1

Load the main asset archive

Open ImHex and select `File → Open File`, then choose:

`C:\Games\GameHercules\HERCULES.FS`

If the archive exists in another location, open that file instead.

Step 2

Enable the analysis views

In the `View` menu, enable:

- Hex Editor
- Data Inspector
- Data Information
- Find
- Pattern Editor
- Pattern Data
- Bookmarks

Step 3

Install format-detection definitions

Go to `Extras → Content Store`. Install the available Magic Files database and useful format patterns.

Step 4

Analyze the file

Open `Data Information`, choose the archive's address range, and click `Analyze page`.

Examine the detected file type, byte distribution and entropy graph.

ImHex supports exactly these hexadecimal inspection, pattern analysis, format-detection and entropy-analysis workflows.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://docs.werwolv.net\&sz=32)

WerWolv's Documentation Page

+2



### What to look for

HEX BYTES

48 45 52 43 55 4C 45 53 00 00 00 00 10 00 00 00 20 00 00 00 40 00 00 00

ASCII VIEW

HERCULES ........  ...\@...

Illustrative hex layout only—not a verified header from Hercules.FS. Your actual file may look completely different.

The main questions to answer are whether the archive starts with a recognizable signature, whether it has an index of filenames and whether its entries contain compressed or uncompressed data.

## 4. Search for embedded assets and filenames

Open `View → Find` and use the following approaches.

| Search mode    | Search target                     | Why                                            |
| -------------- | --------------------------------- | ---------------------------------------------- |
| Strings        | ASCII strings, minimum length 4–6 | Discover filenames and internal resource names |
| Sequences      | `.BMP`, `.WAV`, `.ESF`            | Find references to possible asset formats      |
| Binary Pattern | `42 4D`                           | Possible BMP signature                         |
| Binary Pattern | `52 49 46 46`                     | Possible RIFF audio or related data            |
| Binary Pattern | `89 50 4E 47 0D 0A 1A 0A`         | PNG signature                                  |
| Binary Pattern | `FF D8 FF`                        | JPEG start signature                           |

Set the range to `Entire Data`, perform each search, and click results to jump to the corresponding hexadecimal offsets.

These are generic format signatures, not confirmed descriptions of the Hercules archive. No matches may simply mean that the game uses proprietary formats or compression.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://docs.werwolv.net\&sz=32)

WerWolv's Documentation Page

+1



### Inspect candidate fields

When you select a suspicious four-byte sequence, the `Data Inspector` helps you interpret it as a 32-bit integer. Start with Little Endian for conventional Windows binary structures, but verify the actual archive format before assuming the field's meaning.

For example, if a field contains:

```
00 10 00 00
```

Its unsigned little-endian value is `0x00001000`, or 4096 decimal.

It could represent an offset, size, count, identifier, or something else. Compare multiple entries before assigning a meaning.

## 5. Extract one known byte range manually

Once you have established the exact start offset and size of an individual resource:

1. Navigate to its start address in the Hex Editor.
2. Select the resource's complete byte range.
3. Verify the selection size.
4. Use the selection-export option to save the raw bytes as a separate file.
5. Open the extracted file again in ImHex to verify that its contents match the original range.

ImHex has supported exporting selections to files since its v1.33 series.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://newreleases.io\&sz=32)

NewReleases



The same range can be copied reproducibly using PowerShell or Python after recording its offset and length.

Important: `Edit → Open selection view` creates another view of the selected bytes; it does not independently unpack or decompress the archive.

Do not attempt to carve a sprite until you know its actual boundaries. If the bytes represent compressed data, exporting them only creates a copy of the compressed resource.

## 6. Use the dedicated Hercules extractor for batch processing

This is the recommended approach once you have inspected the original archive.

Visit [Disney's Hercules Tools v1.1](https://www.ctpax-x.org/index.php?goto=files\&lang=en\&show=114) and obtain the tool package.

It documents two utilities:

- `UNHERCFS` — unpacks `HERCULES.FS`.
- `ESFTOWAV` — converts extracted `.ESF` audio into `.WAV`.&#x20;

  [image](https://www.google.com/s2/favicons?domain=https://www.ctpax-x.org\&sz=32)

  CTPAX-X



Extract the utilities into:

```
C:\Games\GameHercules\Remaster\tools\unhercfs\
```

Open PowerShell:

```
cd "C:\Games\GameHercules\Remaster\tools\unhercfs"

Get-ChildItem
```

Identify the actual executable names and inspect their help or bundled C source for command-line arguments. I could verify the tools' existence, but not their exact invocation syntax, so don't assume they take a particular set of parameters.

Run the unpacker against a copy of `HERCULES.FS` and place the results in `Remaster\extracted\raw`.

After extraction, inventory the resulting files:

```
$root = "C:\Games\GameHercules\Remaster\extracted\raw"

Get-ChildItem $root -Recurse -File |
    Group-Object Extension |
    Sort-Object Count -Descending |
    Select-Object Count, Name
```

This shows which asset extensions actually exist in your version.

## 7. Use ImHex Pattern Language to investigate the archive structure

If you want to understand the archive rather than relying entirely on `UNHERCFS`, open `View → Pattern Editor`.

Paste this minimal diagnostic pattern:

```

#pragma endian little

struct HeaderProbe {
    u8 first_bytes[16];
    u32 candidate_value_0;
    u32 candidate_value_1;
    u32 candidate_value_2;
    u32 candidate_value_3;
};

HeaderProbe header @ 0x00;

```

Click the Play button, then inspect `Pattern Data`.

This pattern interprets the first 32 bytes as a set of diagnostic fields. It is not a verified Hercules FS structure. Its purpose is to help you examine values systematically.

Once you have the extractor's C source, use it to identify the real archive layout and create an accurate `.hexpat` parser with:

- Archive header and version information
- File index and resource entry count
- Resource offsets and sizes
- Compression information
- Embedded asset identifiers

The Pattern Language can map binary structures to highlighted hexadecimal regions, making this considerably easier than manually tracking offsets.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://docs.werwolv.net\&sz=32)

WerWolv's Documentation Page

+1



## 8. Convert and remaster the extracted assets

Extraction is only the first phase. Some assets may contain indexed-color pixels, separate palettes, compressed frames, or animation metadata rather than ready-to-use images.

Use this pipeline:

HERCULES.FS

Original game archive

UNHERCFS + ImHex

Archive extraction and structure analysis

Format decoders

Pixels, palettes, audio, frames and metadata

Lossless image exports

RGBA PNG + animation metadata

AI remastering pipeline

Upscaling, inpainting, and frame consistency

Modern Hercules rendering

4K assets, animation and modern graphics engine

For animated sprites, preserve original frame order, transparency, pivot positions and timing. Otherwise, improved images can still produce visibly broken animations.
