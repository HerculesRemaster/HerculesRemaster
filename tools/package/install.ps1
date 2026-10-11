# Hercules Remaster {version} - installer.
# Usage (PowerShell, from this folder):  .\install.ps1 -GameDir "C:\path\to\Hercules"
# Copies ddraw.dll, hercules_display.ini and art\ next to HERCULES.EXE. Files it replaces are backed
# up to <GameDir>\remaster-backup\ first; uninstall.ps1 restores them. The game's own files, its
# registry settings and its saved progress are not touched.
param([Parameter(Mandatory = $true)][string]$GameDir, [switch]$Force)
$ErrorActionPreference = "Stop"
$here = $PSScriptRoot
$manifest = Get-Content (Join-Path $here "package-manifest.json") -Raw | ConvertFrom-Json
if (-not (Test-Path (Join-Path $GameDir "HERCULES.EXE"))) { throw "HERCULES.EXE not found in $GameDir" }

# The proxy patches code at fixed addresses, so it must match the edition it was built against.
foreach ($p in $manifest.game_edition.PSObject.Properties) {
    $f = Join-Path $GameDir $p.Name
    if (-not (Test-Path $f)) { throw "missing $($p.Name) in $GameDir" }
    $h = (Get-FileHash $f -Algorithm SHA256).Hash.ToLower()
    if ($h -ne $p.Value -and -not $Force) { throw "$($p.Name) is a different edition ($h). Use -Force to install anyway." }
}

$backup = Join-Path $GameDir "remaster-backup"
New-Item -ItemType Directory -Force $backup | Out-Null
$log = @()
foreach ($rel in $manifest.files.PSObject.Properties.Name) {
    if ($rel -in @("install.ps1", "uninstall.ps1")) { continue }
    $src = Join-Path $here $rel
    $dst = Join-Path $GameDir $rel
    if ((Test-Path $dst) -and -not (Test-Path (Join-Path $backup $rel))) {
        New-Item -ItemType Directory -Force (Split-Path (Join-Path $backup $rel)) | Out-Null
        Copy-Item $dst (Join-Path $backup $rel)
    }
    New-Item -ItemType Directory -Force (Split-Path $dst) | Out-Null
    Copy-Item $src $dst -Force
    $log += $rel
}
$log | Set-Content (Join-Path $backup "installed-files.txt")
Copy-Item (Join-Path $here "uninstall.ps1") (Join-Path $backup "uninstall.ps1") -Force
"Installed Hercules Remaster $($manifest.version): $($log.Count) files into $GameDir"
"Start HERCULES.EXE. Settings: hercules_display.ini. Uninstall: $backup\uninstall.ps1 -GameDir `"$GameDir`""
