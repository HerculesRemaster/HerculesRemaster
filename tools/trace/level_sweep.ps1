# Start each level through the hook's level harness (HERC_HOOK_PAD presses Start on the title menu,
# HERC_HOOK_LEVEL rewrites the next engine request) and record, per level:
#   analysis/runtime/sweep/<level>/  gpu_<n>.png / soft_<n>.png / gpu_<n>.txt snapshots (GPU renderer)
#   analysis/runtime/sweep/<level>/display.log  the proxy log (fps lines)
# Then run tools/runtime/gpu_diff.py on the snapshots and print one line per level.
param([string]$Levels = "1,2,3,5,6,7,8,9,10,11", [int]$Seconds = 80, [string]$Frames = "1700,1900,2100")
$root = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$py = Join-Path $root ".venv\Scripts\python.exe"
$ini = Join-Path $root "working\game\hercules_display.ini"
$iniText = Get-Content $ini -Raw
($iniText -replace '(?m)^log = \w+', 'log = on') | Set-Content $ini -NoNewline     # fps lines in the proxy log
try {
    foreach ($l in $Levels.Split(",")) {
        $out = Join-Path $root "analysis\runtime\sweep\$l"
        New-Item -ItemType Directory -Force $out | Out-Null
        $env:HERC_HOOK_PAD = "300-304:0800,400-404:8000,500-504:0800,600-604:8000"
        $env:HERC_HOOK_LEVEL = "$l"
        $env:HERC_GPU_SHOT = "$out"
        $env:HERC_GPU_SHOT_FRAME = $Frames
        & (Join-Path $PSScriptRoot "run_trace.ps1") -Seconds $Seconds -Name "sweep_$l" -Pairs 0 -Visible -Renderer gpu | Out-Null
        Copy-Item (Join-Path $root "working\game\hercules_display.log") (Join-Path $out "display.log") -Force
        $fps = (Select-String -Path (Join-Path $out "display.log") -Pattern "gpu present ([\d.]+) fps" |
                ForEach-Object { [double]$_.Matches[0].Groups[1].Value } | Select-Object -Last 3) -join "/"
        $diff = foreach ($f in $Frames.Split(",")) {
            if (Test-Path (Join-Path $out "gpu_$f.png")) {
                $m = (Select-String -Path (Join-Path $out "gpu_$f.txt") -Pattern "^module (\S+) map (\S+)").Matches
                $d = & $py (Join-Path $root "tools\runtime\gpu_diff.py") $out $f
                $pct = $d -replace '.*differ ', ''
                $map = Split-Path (Split-Path $m[0].Groups[2].Value -Parent) -Leaf
                "$f $($m[0].Groups[1].Value) $map $pct"
            } else { "$f -" }
        }
        "level $l  fps $fps  |  " + ($diff -join "  |  ")
    }
} finally {
    $iniText | Set-Content $ini -NoNewline
    Remove-Item Env:HERC_HOOK_PAD, Env:HERC_HOOK_LEVEL, Env:HERC_GPU_SHOT, Env:HERC_GPU_SHOT_FRAME -ErrorAction SilentlyContinue
}
