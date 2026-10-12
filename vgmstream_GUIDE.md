# Disney's Hercules (1997) — Complete vgmstream CLI Extraction & Reverse Engineering Guide

For your Hercules remaster project on Windows 11, I recommend using vgmstream alongside a Hercules-specific archive extractor, rather than relying on vgmstream alone.

The important discovery is that the original PC game uses a proprietary `HERCULES.FS` resource archive, and a dedicated extraction toolkit already exists for it.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.ctpax-x.org&sz=32)

CTPAX-X

## 1. Understand the Hercules audio architecture

| Source                        | Contents                               | Recommended tool                            |
| ----------------------------- | -------------------------------------- | ------------------------------------------- |
| `HERCULES.FS`                 | Packaged game assets, including sounds | `UNHERCFS`                                  |
| `.ESF`                        | Proprietary sound data                 | `ESFTOWAV`                                  |
| Other supported audio streams | Music, effects, audio banks            | `vgmstream-cli.exe`                         |
| CD audio track                | Original soundtrack                    | CDDA ripping tool                           |
| `MUSIC\Track02.ogg`           | Music in digital releases              | vgmstream / FFmpeg                          |
| `VIDDATA\*.ETV`               | Video/cutscene resources               | Separate video reverse-engineering pipeline |

The original CD contains CD-DA music, while the Steam version includes a `MUSIC\Track02.ogg` file. The soundtrack is approximately 34 minutes long.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.pcgamingwiki.com&sz=32)

PCGamingWiki

+2

Recommended extraction architecture

Original Hercules installation / CD image

HERCULES.FS

UNHERCFS → ESFTOWAV

CDDA / OGG

CD ripper / FFmpeg

vgmstream-cli.exe

Probe supported streams, decode, export loop metadata and subsongs

Hercules Remaster Audio Library

Original WAVs · Loop metadata · Asset IDs · CSV/JSON manifests

## 2. Install and configure vgmstream

Official sources:

- [vgmstream downloads](https://vgmstream.org/)
- [vgmstream GitHub repository](https://github.com/vgmstream/vgmstream)
- [Official CLI documentation](https://github.com/vgmstream/vgmstream/blob/master/doc/USAGE.md)

Download the Windows 64-bit command-line build. Extract the EXE and its accompanying DLLs into the same directory. Some codecs require the supplied external libraries.

I suggest this structure:

```
C:\Tools\
  vgmstream\
    vgmstream-cli.exe
    *.dll
  HerculesTools\
    UNHERCFS.exe
    ESFTOWAV.exe

C:\Games\
  HerculesRemaster\
    original\
    extracted\
    audio\
      raw\
      decoded\
      music\
      metadata\
      enhanced\
```

Open PowerShell:

```
$vgm = "C:\Games\GameHercules\vgmstream-win64\vgmstream-cli.exe"

# Display every command and option
& $vgm -h

# Show decoder version and supported extensions
& $vgm -V

# Save capability information
New-Item "C:\Games\HerculesRemaster\audio\metadata" `
  -ItemType Directory -Force | Out-Null

& $vgm -V |
  Out-File "C:\Games\HerculesRemaster\audio\metadata\vgmstream-version.json" -Encoding utf8
```

The following commands use the currently documented flags from vgmstream's CLI source.

## 3. Complete `vgmstream-cli.exe` command reference

General syntax:

```
vgmstream-cli.exe [options] <input-file> ...
```

### Essential decoding and metadata commands

| Flag      | Function                                                          |
| --------- | ----------------------------------------------------------------- |
| `-h`      | Display complete CLI help                                         |
| `-V`      | Print version and supported extensions as JSON                    |
| `-m`      | Show audio metadata without decoding                              |
| `-I`      | Print metadata as JSON                                            |
| `-o FILE` | Set output WAV filename                                           |
| `-i`      | Ignore playback looping and decode the stream once                |
| `-L`      | Export WAV with a `smpl` loop metadata chunk                      |
| `-w`      | Allow the original decoded sample format instead of forcing PCM16 |
| `-W N`    | Force WAV output sample format                                    |
| `-O`      | Decode for testing without writing a file                         |

Important: Use `-I -m` together when probing metadata. `-I` changes the information format but does not, by itself, prevent decoding.

### Looping options

| Flag      | Function                                                    |
| --------- | ----------------------------------------------------------- |
| `-l 2.0`  | Number of loops, default 2                                  |
| `-f 10.0` | Fade duration in seconds, default 10                        |
| `-d 0.0`  | Fade delay after looping                                    |
| `-F`      | Disable the fade and play the remaining stream              |
| `-e`      | Enable end-to-end looping when no loop points exist         |
| `-E`      | Force end-to-end looping even if loop points exist          |
| `-c`      | Continuously loop to standard output; requires `-p` or `-P` |

### Subsongs, channels, and output options

| Flag   | Function                                           |
| ------ | -------------------------------------------------- |
| `-s N` | Decode subsong N, numbered from 1                  |
| `-S N` | Decode up to subsong N; `-S 0` means all           |
| `-2 N` | Select stereo-channel pair N, numbered from 0      |
| `-D N` | Downmix to a maximum of N channels                 |
| `-p`   | Stream decoded output to stdout                    |
| `-P`   | Stream to stdout even when connected to a terminal |

Output format values:

| Command | WAV output            |
| ------- | --------------------- |
| `-W 1`  | PCM 16-bit            |
| `-W 2`  | PCM 24-bit            |
| `-W 3`  | PCM 32-bit            |
| `-W 4`  | 32-bit floating point |

### Advanced reverse-engineering and testing flags

| Flag    | Function                                            |
| ------- | --------------------------------------------------- |
| `-v`    | Validate extensions                                 |
| `-r`    | Reset-test decoding and create an additional output |
| `-k N`  | Seek to sample N before decoding                    |
| `-K N`  | Perform a second seek to sample N                   |
| `-k -2` | Seek to the loop start                              |
| `-k -3` | Seek to the loop end                                |
| `-B N`  | Force a decoding buffer size in samples             |
| `-t`    | Print external tags from `!tags.m3u`                |
| `-T`    | Print a stream title                                |
| `-b`    | Print batch-variable commands                       |
| `-x`    | Print an ADX encoder command                        |
| `-g`    | Print an OGG encoder command                        |

The last two encoder-related flags do not make vgmstream an audio encoder; they provide commands for external encoders.

## 4. Practical extraction commands

Set the source and output directories:

```
$vgm = "C:\Games\GameHercules\vgmstream-win64\vgmstream-cli.exe"

$source = "C:\Games\HerculesRemaster\extracted"
$output = "C:\Games\HerculesRemaster\audio\decoded"

New-Item -ItemType Directory -Force $output | Out-Null
```

### A. Inspect one audio stream

The following uses a hypothetical `SOUND01.adx` filename to demonstrate the syntax; it is not a verified Hercules asset name.

```
& $vgm -m "$source\SOUND01.adx"
```

For JSON metadata:

```
& $vgm -I -m "$source\SOUND01.adx"
```

To save that JSON:

```
& $vgm -I -m "$source\SOUND01.adx" |
  Out-File "$output\SOUND01.json" -Encoding utf8
```

Metadata can identify codec, channel count, sample rate, length, stream name, and available loop information.

### B. Decode the complete original stream

```
& $vgm -i `
  -o "$output\SOUND01.wav" `
  "$source\SOUND01.adx"
```

This is particularly useful for effects and dialogue: it outputs one playback of the encoded stream, rather than adding repeated music loops.

### C. Preserve loop metadata for the remaster

```
& $vgm -L `
  -o "$output\SOUND01_loop.wav" `
  "$source\SOUND01.adx"
```

`-L` outputs the stream once and writes available loop points into the WAV `smpl` chunk. It is useful for implementing sample-accurate music loops in your replacement engine.

### D. Export at a selected sample format

```
# PCM24
& $vgm -i -W 2 `
  -o "$output\SOUND01_24bit.wav" `
  "$source\SOUND01.adx"

# Floating point
& $vgm -i -W 4 `
  -o "$output\SOUND01_float.wav" `
  "$source\SOUND01.adx"

# Preserve the original decoded sample format where possible
& $vgm -i -w `
  -o "$output\SOUND01_original_format.wav" `
  "$source\SOUND01.adx"
```

For remastering, keep the original encoded file as the archival master. Use WAV for editing and enhancement. Converting an 8-bit or lossy source to 24-bit does not recreate lost details.

### E. Extract all subsongs from a sound bank

For a format with multiple internal audio entries:

```
& $vgm -s 1 -S 0 -i `
  -o "$output\bank_?04s_?n.wav" `
  "$source\SOUNDBANK.fsb"
```

The output name uses:

- `?04s` — zero-padded four-digit subsong index
- `?n` — internal stream name, or fallback name
- `?f` — input filename

Example output:

```
bank_0001_attack.wav
bank_0002_jump.wav
bank_0003_hit.wav
```

These are illustrative names, not confirmed Hercules assets. The bank must actually support subsongs. Do not use `-S 0` indiscriminately on unknown large containers because thousands of output files may be produced.

## 5. Extract the actual Hercules sound assets

This is the most important step for your particular game.

A project-specific package called Disney's Hercules tools v1.1 provides:

- `UNHERCFS`: unpack `HERCULES.FS`
- `ESFTOWAV`: convert proprietary `.ESF` sounds to `.WAV`

Its publisher also includes C source code, making it useful for investigating the archive and sound codec directly.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.ctpax-x.org&sz=32)

CTPAX-X

[Disney's Hercules tools v1.1 — CTPAX-X](https://www.ctpax-x.org/index.php?goto=files&lang=en&show=114)

Your existing game installation is expected to contain `HERCULES.FS`; verify that before proceeding:

```
$game = "C:\Disney\Hercules"

Get-ChildItem $game -Recurse -File |
  Select-Object FullName, Length

Get-ChildItem $game -Recurse -Filter "*.FS" -File
```

Back up the source first:

```
New-Item "C:\Games\HerculesRemaster\original" `
  -ItemType Directory -Force | Out-Null

Copy-Item `
  "C:\Disney\Hercules\HERCULES.FS" `
  "C:\Games\HerculesRemaster\original\HERCULES.FS"

Get-FileHash `
  "C:\Games\HerculesRemaster\original\HERCULES.FS" `
  -Algorithm SHA256
```

Then inspect the two utility executables and their included documentation/source:

```
cd C:\Tools\HerculesTools

.\UNHERCFS.exe
.\ESFTOWAV.exe
```

I could verify the utilities' purposes, but not their exact argument syntax. Use the usage output or bundled C source to determine invocation rather than assuming their flags match vgmstream.

Once extraction succeeds, enumerate every `.ESF`:

```
Get-ChildItem `
  "C:\Games\HerculesRemaster\extracted" `
  -Recurse -File -Filter "*.ESF" |
  Select-Object FullName, Length
```

Process those files with `ESFTOWAV`, retaining their filenames or resource IDs. The `.ESF` decoder is the more relevant starting point for Hercules audio than attempting to force vgmstream to decode the `.FS` archive.

## 6. Automated batch vgmstream processing with PowerShell

Use this script after unpacking the archives. It preserves subdirectories, probes candidate files, exports JSON metadata, logs failures, and creates WAV files only for audio that vgmstream successfully recognizes.

```
$ErrorActionPreference = "Stop"

$vgm = "C:\Games\GameHercules\vgmstream-win64\vgmstream-cli.exe"
$src = "C:\Games\HerculesRemaster\extracted"
$dst = "C:\Games\HerculesRemaster\audio\decoded"

if (!(Test-Path -LiteralPath $vgm)) {
    throw "vgmstream not found: $vgm"
}

if (!(Test-Path -LiteralPath $src)) {
    throw "Source folder not found: $src"
}

New-Item -ItemType Directory -Force -Path $dst |
    Out-Null

# Candidate audio formats, not a claim that Hercules
# contains all of these.
$extensions = @(
    ".wav", ".ogg", ".adx", ".adp",
    ".vag", ".xa", ".str", ".fsb",
    ".wem", ".hca", ".bnk"
)

$results = @(
    Get-ChildItem -LiteralPath $src -Recurse -File |
    Where-Object {
        $extensions -contains $_.Extension.ToLowerInvariant()
    } |
    ForEach-Object {

        $file = $_

        $relative = $file.FullName.Substring(
            $src.TrimEnd('\').Length
        ).TrimStart('\')

        $relativeDir = Split-Path $relative -Parent

        $targetDir = if ($relativeDir) {
            Join-Path $dst $relativeDir
        } else {
            $dst
        }

        New-Item -ItemType Directory -Force `
            -Path $targetDir | Out-Null

        $wav = Join-Path $targetDir ($file.Name + ".wav")
        $json = Join-Path $targetDir ($file.Name + ".json")

        # Probe only: no audio decoding.
        $probe = (& $vgm -I -m $file.FullName 2>&1 |
            Out-String).Trim()

        $probeExit = $LASTEXITCODE
        $decodeExit = -1
        $message = ""

        if ($probeExit -eq 0) {

            Set-Content -LiteralPath $json `
                -Value $probe -Encoding UTF8

            # Preserve loop points where available.
            $message = (& $vgm -L -o $wav `
                $file.FullName 2>&1 |
                Out-String).Trim()

            $decodeExit = $LASTEXITCODE
        } else {
            $message = $probe
        }

        $success = (
            $decodeExit -eq 0 -and
            (Test-Path -LiteralPath $wav)
        )

        [PSCustomObject]@{
            Source     = $file.FullName
            Output     = $wav
            ProbeExit  = $probeExit
            DecodeExit = $decodeExit
            Success    = $success
            Message    = $message
        }
    }
)

$report = Join-Path $dst "extraction_manifest.csv"

$results |
    Export-Csv -LiteralPath $report `
        -NoTypeInformation -Encoding UTF8

$results |
    Group-Object Success |
    Select-Object Name, Count

Write-Host "Manifest: $report"
```

This is a candidate-format scan rather than a complete Hercules extractor. It intentionally leaves proprietary `.ESF` conversion to the dedicated utility.

For auditability, retain the source files and associate each output with its source hash, asset identifier, and eventual runtime event.

## 7. Extract music from the original Hercules CD

The retail PC version uses CD-DA music. This matters because your earlier `C:\Games\Hercules_AG.iso` may contain only the CD's data track.

A conventional data-only ISO cannot preserve a mixed-mode disc's separate audio track. A proper `BIN/CUE` or equivalent full-disc image is needed to retain that audio.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://www.old-games.ru&sz=32)

Форум Old-Games.RU. Всё о старых играх

+1

If you have an appropriate `Hercules_AG.bin` and `Hercules_AG.cue`, a tool such as `bchunk` can extract WAV audio tracks:

```
bchunk -w `
  "C:\Games\Hercules_AG.bin" `
  "C:\Games\Hercules_AG.cue" `
  "C:\Games\HerculesRemaster\audio\music\Track"
```

This requires an installed `bchunk` build. The `-w` option outputs CD audio tracks as WAV.

If you have the Steam or GOG edition containing `Track02.ogg`, decode it directly:

```
& $vgm -i `
  -o "C:\Games\HerculesRemaster\audio\music\Track02.wav" `
  "C:\Disney\Hercules\MUSIC\Track02.ogg"
```

Adjust the input path to the actual digital installation.

One limitation: the original approximately 34-minute CD audio program is not necessarily stored as fourteen separately addressable files. You will need to identify musical sections and associate them with level transitions or runtime music events.

## 8. Reverse-engineer the audio-to-gameplay mapping

After extraction, the primary challenge becomes determining which sounds belong to which actions, characters, levels, and game states.

I recommend creating a structured audio asset database:

| Field                     | Example                       |
| ------------------------- | ----------------------------- |
| `asset_id`                | `hercules.attack.001`         |
| `source_file`             | Original `.ESF` or other file |
| `source_offset`           | Byte offset within archive    |
| `original_codec`          | From header/decoder analysis  |
| `sample_rate`             | Measured source rate          |
| `loop_start` / `loop_end` | Source sample positions       |
| `event_name`              | `player.sword.swing`          |
| `level_id`                | `L03`                         |
| `actor_id`                | `hercules`                    |
| `enhanced_file`           | Remastered audio path         |
| `confidence`              | Verified / inferred / unknown |

For binary reverse engineering with Ghidra or x32dbg, investigate the code that reads `HERCULES.FS`, decodes ESF resources, and invokes the sound playback routines. Windows APIs worth investigating include `waveOutWrite`, DirectSound interfaces, and CD-audio MCI calls such as `mciSendCommandA` or `mciSendStringA`, depending on what the actual executable imports.

The goal is not merely extracting thousands of sound files. It is recovering the associations:

```
Level → Scene → Actor → Event → Audio resource ID
                                ↓
                          Original sound
                                ↓
                          Enhanced sound
                                ↓
                          Remaster runtime
```

This will let the remaster support level-specific ambience, modern spatial audio, layered combat effects, dynamic music, and localized dialogue while preserving the original game's timing and recognizable sound identity.

## Recommended execution order

1. Inspect `HERCULES.FS` and preserve a SHA-256 verified original.
2. Run `UNHERCFS` to extract the resource archive.
3. Use `ESFTOWAV` to decode Hercules's proprietary sound data.
4. Use vgmstream to probe and export any additional supported audio streams.
5. Recover the CDDA soundtrack separately from a complete disc image, or decode the digital edition's OGG.
6. Build an audio manifest and map each asset to gameplay events through runtime tracing.
7. Remaster audio non-destructively, retaining unprocessed originals and loop points for comparison.

The most valuable tools for your specific title are therefore UNHERCFS + ESFTOWAV + vgmstream-cli, with a separate CDDA extraction path. The existence of Hercules-specific C source also gives you a practical foundation for integrating archive extraction and audio decoding directly into your automated remaster pipeline.
