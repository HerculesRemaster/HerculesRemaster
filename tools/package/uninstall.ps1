# Hercules Remaster {version} - uninstaller.
# Usage:  .\uninstall.ps1 -GameDir "C:\path\to\Hercules"
# Removes every file the installer copied (remaster-backup\installed-files.txt) and restores the
# files it had replaced from remaster-backup\.
param([Parameter(Mandatory = $true)][string]$GameDir)
$ErrorActionPreference = "Stop"
$backup = Join-Path $GameDir "remaster-backup"
$list = Join-Path $backup "installed-files.txt"
if (-not (Test-Path $list)) { throw "no installation record in $backup" }
foreach ($rel in Get-Content $list) {
    $dst = Join-Path $GameDir $rel
    if (Test-Path $dst) { Remove-Item $dst -Force }
    $old = Join-Path $backup $rel
    if (Test-Path $old) { Copy-Item $old $dst -Force }
}
$art = Join-Path $GameDir "art"
if ((Test-Path $art) -and -not (Get-ChildItem $art -Recurse -File)) { Remove-Item $art -Recurse -Force }
Remove-Item $backup -Recurse -Force
"Hercules Remaster removed from $GameDir"
