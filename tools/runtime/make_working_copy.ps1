# Copy backups/originals to working/game (the only place the game is run from).
# -Force replaces an existing working copy. ReadMe.txt (redistributor text) is skipped.
param([switch]$Force)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$src = Join-Path $root "backups\originals"
$dst = Join-Path $root "working\game"
if (Test-Path $dst) {
    if (-not $Force) { Write-Output "working copy exists: $dst (use -Force to replace)"; exit 0 }
    Remove-Item -Recurse -Force $dst
}
New-Item -ItemType Directory -Force $dst | Out-Null
$files = Get-ChildItem $src -File | Where-Object Name -ne "ReadMe.txt"
foreach ($f in $files) {
    $t = Join-Path $dst $f.Name
    Copy-Item $f.FullName $t
    (Get-Item $t).IsReadOnly = $false
}
$bytes = ($files | Measure-Object Length -Sum).Sum
Write-Output "copied $($files.Count) files ($bytes bytes) -> $dst"
