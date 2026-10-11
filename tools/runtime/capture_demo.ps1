# Record 60 s of the attract demo without touching the desktop: run the hooked game minimized
# (tools/trace/run_trace.ps1 -Frames: windowed GDI path, kept active, no input), let the title
# idle until the demo loads (an open of M:\DEMO\DEMO*.DAT), then encode the exact presented
# 320x240 frames from that point with ffmpeg and measure the cadence.
# Output: reference/captures/auto/demo-<stamp>.mkv + .json
param([int]$Seconds = 60, [int]$IdleMax = 60, [int]$Scale = 3)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$py = Join-Path $root ".venv\Scripts\python.exe"
$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$name = "demo-capture-$stamp"
& pwsh -NoProfile -File (Join-Path $root "tools\trace\run_trace.ps1") -Seconds ($IdleMax + $Seconds + 5) -Name $name -Frames -Pairs 0 | Out-Null
$run = Join-Path $root "analysis\traces\runs\$name"
$demo = Get-Content (Join-Path $run "resource-usage.jsonl") | ForEach-Object { $_ | ConvertFrom-Json } |
        Where-Object { $_.ev -eq "open" -and $_.name -match "\\DEMO\\DEMO" } | Select-Object -First 1
if (-not $demo) { throw "the demo did not start within $IdleMax s" }
$outDir = Join-Path $root "reference\captures\auto"
New-Item -ItemType Directory -Force $outDir | Out-Null
$video = Join-Path $outDir "demo-$stamp.mkv"
$cad = & $py (Join-Path $PSScriptRoot "frames_to_video.py") (Join-Path $run "frames.bin") $video --start-ms $demo.t --seconds $Seconds --scale $Scale | ConvertFrom-Json
$info = [ordered]@{
    video = $video; demo_file = $demo.name; demo_open_ms = $demo.t; demo_open_tick = $demo.tick
    display = [ordered]@{ captured = "game frames at present time (StretchDIBits source), not the screen"
        game_framebuffer = "320x240 8bpp palettised DIB (EXE g_frameBits 0x96e3d8)"; video_scale = $Scale
        run_mode = "windowed GDI path (HERC_HOOK_WINDOWED, HERC_DISPLAY_MODE=windowed), minimized" }
    cadence = $cad; trace = $run
}
$info | ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 ($video -replace "\.mkv$", ".json")
Remove-Item (Join-Path $run "frames.bin")
Write-Output "demo=$($demo.name) at $([math]::Round($demo.t/1000,1)) s, frames=$($cad.frames) present_fps=$($cad.present_fps) median=$($cad.median_interval_ms) ms ticks/present=$(($cad.ticks_per_present | ConvertTo-Json -Compress)) -> $video"
