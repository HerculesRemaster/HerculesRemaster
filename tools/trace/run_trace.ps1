# Run working/game/HERCULES.EXE with runtime/hooks/bin/hercules_hook.dll injected for -Seconds,
# windowed and minimized (no focus steal), running unfocused, keyboard input disabled (the
# demo runs by itself). -Frames dumps every presented frame to frames.bin.
# Output: analysis/traces/runs/<name>/{resource-usage.jsonl,input.jsonl,pairs/} (or -OutDir)
param([int]$Seconds = 60, [string]$Name = "", [int]$Pairs = 16, [switch]$Fullscreen, [switch]$WithInput,
      [switch]$Frames, [switch]$Visible, [string]$VramTicks = "", [switch]$Hero, [string]$Renderer = "",
      [string]$OutDir = "")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$bin = Join-Path $root "runtime\hooks\bin"
if (Get-Process HERCULES -ErrorAction SilentlyContinue) { throw "game already running" }
if (-not $Name) { $Name = Get-Date -Format "yyyyMMdd-HHmmss" }
$out = if ($OutDir) { $OutDir } else { Join-Path $root "analysis\traces\runs\$Name" }   # -OutDir: full-game runs
New-Item -ItemType Directory -Force $out | Out-Null
$env:HERC_HOOK_DIR = $out
$env:HERC_HOOK_WINDOWED = if ($Fullscreen) { "0" } else { "1" }
$env:HERC_HOOK_ACTIVE = "1"
$env:HERC_HOOK_NOINPUT = if ($WithInput) { "0" } else { "1" }
$env:HERC_HOOK_PAIRS = "$Pairs"
$env:HERC_HOOK_VRAM_TICKS = $VramTicks
$env:HERC_HOOK_HERO = if ($Hero) { "1" } else { "0" }
$env:HERC_RENDERER = $Renderer
$env:HERC_HOOK_FRAMES = if ($Frames) { Join-Path $out "frames.bin" } else { "" }
$env:HERC_DISPLAY_MODE = if ($Fullscreen) { "" } else { "windowed" }     # P2F proxy, if installed
$env:HERC_INJECT_SHOW = if ($Visible -or $Renderer -eq "gpu") { "" } else { "minimized" }   # the GPU renderer needs a real client area
$log = Join-Path $out "resource-usage.jsonl"
foreach ($try in 1, 2) {
    $r = & (Join-Path $bin "herc_inject.exe") (Join-Path $root "working\game\HERCULES.EXE") (Join-Path $bin "hercules_hook.dll") $Seconds
    # a start sometimes stalls right after the index read (seen once minimized); retry once
    if ((Get-Content $log | Measure-Object -Line).Lines -gt 10) { break }
    Write-Warning "trace stalled at startup (try $try)"
}
$ev = @{}
Get-Content $log | ForEach-Object { $e = ($_ | ConvertFrom-Json).ev; $ev[$e] = 1 + [int]$ev[$e] }
$pairs = (Get-ChildItem (Join-Path $out "pairs") -Directory -ErrorAction SilentlyContinue).Count
Write-Output "$r events: $(($ev.GetEnumerator() | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Value)" }) -join ' ') pairs=$pairs -> $out"
