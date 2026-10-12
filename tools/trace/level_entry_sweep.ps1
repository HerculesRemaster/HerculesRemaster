# Level entry sweep (ENG1 level plans, phase 2 entry + S4.05 + S5.01): for each password table index, run
# tools/trace/run_scenario.ps1 -Entry password with the hero log, the GPU renderer's art_dump switched on
# (working/game/hercules_display.ini, restored afterwards) and copy the art dump index rows written during
# that run into <run dir>/art_index.csv (key,w,h,tpage,alt,u,v,sx,sy,frame,module: first draw of each new key).
# Usage: level_entry_sweep.ps1 -Runs "L03.forest-main-route=14,L04.thebes-main-route=15" [-Seconds 60]
param([Parameter(Mandatory = $true)][string]$Runs, [int]$Seconds = 60, [string]$Pad = "")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$ini = Join-Path $root "working\game\hercules_display.ini"
$index = Join-Path $root "working\game\art\dump\index.csv"
$saved = Get-Content $ini -Raw
try {
    (($saved -replace '(?m)^art_dump\s*=\s*\w+', 'art_dump = on')) | Set-Content $ini -NoNewline
    foreach ($r in $Runs.Split(",")) {
        $scenario, $pw = $r.Split("=")
        if (Get-Process HERCULES -ErrorAction SilentlyContinue) { throw "game already running" }
        $before = if (Test-Path $index) { (Get-Item $index).Length } else { 0 }
        $line = & (Join-Path $PSScriptRoot "run_scenario.ps1") -Scenario $scenario -Entry password -Password ([int]$pw) -Seconds $Seconds -Hero -Pad $Pad 2>&1 | Select-Object -Last 1
        $dir = Join-Path $root (($line -split ":")[0].Trim())
        if ((Test-Path $index) -and (Test-Path $dir)) {
            $fs = [System.IO.File]::Open($index, "Open", "Read", "ReadWrite")
            $fs.Seek($before, "Begin") | Out-Null
            $rd = New-Object System.IO.StreamReader($fs)
            $rd.ReadToEnd() | Set-Content (Join-Path $dir "art_index.csv") -NoNewline
            $rd.Close()
        }
        Write-Output $line
    }
} finally {
    $saved | Set-Content $ini -NoNewline
}
