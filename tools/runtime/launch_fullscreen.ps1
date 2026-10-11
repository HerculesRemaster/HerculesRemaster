# Install the ddraw.dll proxy + hercules_display.ini into working/game, start the game, take a
# screenshot at -ShotAt s, check the window covers the monitor and the image rectangle, close it.
# -KeepIni keeps an existing working/game/hercules_display.ini; -Seconds > ShotAt runs longer
# (fps is read from the proxy log). Output: analysis/runtime/fullscreen/<stamp>/.
param([int]$ShotAt = 10, [int]$Seconds = 0, [switch]$KeepIni, [switch]$NoClose)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot "win32.ps1")
$game = Join-Path $root "working\game"
$proxy = Join-Path $root "runtime\display\bin\ddraw.dll"
if (-not (Test-Path $proxy)) { throw "build runtime\display\build.bat first" }
if (Get-Process HERCULES -ErrorAction SilentlyContinue) { throw "game already running" }
Copy-Item $proxy (Join-Path $game "ddraw.dll") -Force
$ini = Join-Path $game "hercules_display.ini"
if (-not $KeepIni -or -not (Test-Path $ini)) { Copy-Item (Join-Path $root "runtime\display\hercules_display.ini") $ini -Force }
# log on for this run (fps + image rectangle), restored afterwards
$iniText = Get-Content $ini -Raw
Set-Content $ini ($iniText -replace "(?m)^log\s*=\s*\w+", "log = on") -NoNewline
$logFile = Join-Path $game "hercules_display.log"
Remove-Item $logFile -ErrorAction SilentlyContinue

$out = Join-Path $root ("analysis\runtime\fullscreen\" + (Get-Date -Format "yyyyMMdd-HHmmss"))
New-Item -ItemType Directory -Force $out | Out-Null
if ($Seconds -lt $ShotAt + 1) { $Seconds = $ShotAt + 1 }
$p = Start-Process (Join-Path $game "HERCULES.EXE") -WorkingDirectory $game -PassThru
Start-Sleep -Seconds $ShotAt
$w = [HercWin]::Windows([uint32]$p.Id) | Where-Object Class -like "Afx:400000*" | Select-Object -First 1
$mon = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
$shot = Join-Path $out "shot_${ShotAt}s.png"
if ($w) {
    [void][HercWin]::SetForegroundWindow($w.Handle)
    Start-Sleep -Milliseconds 700
    $w = [HercWin]::Windows([uint32]$p.Id) | Where-Object Class -like "Afx:400000*" | Select-Object -First 1
    if ([HercWin]::Title($w.Handle) -like "*Waiting*") { Write-Warning "game is not in the foreground; screenshot skipped" }
    else { Save-Screenshot $shot $w.Handle }
}
Start-Sleep -Seconds ($Seconds - $ShotAt)
if (-not $NoClose) {
    [void]$p.CloseMainWindow()
    if (-not $p.WaitForExit(5000)) { Stop-Process -Id $p.Id -Force }
}
Set-Content $ini $iniText -NoNewline
Start-Sleep -Milliseconds 500
if (Test-Path $logFile) { Copy-Item $logFile (Join-Path $out "hercules_display.log") }

$winRect = if ($w) { "$($w.Left),$($w.Top),$($w.Right),$($w.Bottom)" } else { "none" }
$monRect = "$($mon.Left),$($mon.Top),$($mon.Right),$($mon.Bottom)"
$img = (Select-String -Path (Join-Path $out "hercules_display.log") -Pattern "image (\d+),(\d+) (\d+)x(\d+)" -ErrorAction SilentlyContinue | Select-Object -Last 1)
$fps = (Select-String -Path (Join-Path $out "hercules_display.log") -Pattern "present ([\d.]+) fps" -ErrorAction SilentlyContinue |
        ForEach-Object { [double]$_.Matches[0].Groups[1].Value })
$imgRect = if ($img) { $g = $img.Matches[0].Groups; "$($g[1].Value),$($g[2].Value) $($g[3].Value)x$($g[4].Value)" } else { "?" }
$bars = & (Join-Path $root ".venv\Scripts\python.exe") -c @"
from PIL import Image
im = Image.open(r'$shot').convert('RGB'); w, h = im.size
x0 = (w - 2880) // 2 if w >= 2880 else 0
def black(box):
    return max(max(c) for c in im.crop(box).getextrema()) <= 8
print('left_bar_black=%s right_bar_black=%s size=%dx%d' % (black((0, 0, max(x0, 1), h)), black((w - max(x0, 1), 0, w, h)), w, h))
"@
$result = [ordered]@{ window = $winRect; monitor = $monRect; window_is_monitor = ($winRect -eq $monRect)
    image = $imgRect; image_ok = ($imgRect -eq "480,0 2880x2160"); fps = $fps; bars = "$bars"; shot = $shot }
$result | ConvertTo-Json | Set-Content -Encoding utf8 (Join-Path $out "result.json")
Write-Output "window=$winRect monitor=$monRect match=$($result.window_is_monitor) image=$imgRect ok=$($result.image_ok) fps=$($fps -join ',') $bars -> $out"
