# Measure the GPU renderer at the 4K scale (9x: 2880x2160 targets) in each level, in the trace window
# (HERC_GPU_SCALE=9; -Fullscreen measures on the real borderless 3840x2160 screen instead):
# starts the level through the hook harness (as tools/trace/level_sweep.ps1), logs every present
# (HERC_PERF) and prints tools/perf/report.py per level. Output: analysis/runtime/perf/<level>.csv
param([string]$Levels = "1,2,3,5,6,7,8,9,10,11", [int]$Seconds = 90, [switch]$Fullscreen)
$root = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$py = Join-Path $root ".venv\Scripts\python.exe"
$out = Join-Path $root "analysis\runtime\perf"
New-Item -ItemType Directory -Force $out | Out-Null
$csvs = @()
try {
    foreach ($l in $Levels.Split(",")) {
        $csv = Join-Path $out "$l.csv"
        $env:HERC_HOOK_PAD = "300-304:0800,400-404:8000,500-504:0800,600-604:8000"
        $env:HERC_HOOK_LEVEL = "$l"
        $env:HERC_PERF = $csv
        $env:HERC_GPU_SCALE = if ($Fullscreen) { "" } else { "9" }   # 9x targets (3840x2160 output size) in the trace window
        & (Join-Path $root "tools\trace\run_trace.ps1") -Seconds $Seconds -Name "perf_$l" -Pairs 0 -Visible -Fullscreen:$Fullscreen -Renderer gpu | Out-Null
        $csvs += $csv
    }
} finally {
    Remove-Item Env:HERC_HOOK_PAD, Env:HERC_HOOK_LEVEL, Env:HERC_PERF, Env:HERC_GPU_SCALE -ErrorAction SilentlyContinue
}
& $py (Join-Path $root "tools\perf\report.py") @csvs
