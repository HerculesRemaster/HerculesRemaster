# Ensure the Ghidra GUI (with the GhidraMCP plugin) is serving 127.0.0.1:8089.
# Idempotent: returns at once when /check_connection answers ok. Otherwise starts
# ghidraRun.bat (it reopens the last project, HerculesRevertEngineer) and waits.
param(
    [string]$Url = "http://127.0.0.1:8089",
    [string]$GhidraHome = "C:\Games\GameHercules\ghidra_12.1.4_PUBLIC",
    [int]$TimeoutSec = 180
)
$ErrorActionPreference = "Stop"

function Test-Server {
    try {
        $r = Invoke-RestMethod -Uri "$Url/check_connection" -TimeoutSec 5
        return ($r.status -eq "ok")
    } catch { return $false }
}

if (Test-Server) { Write-Output "ghidra-mcp: ok ($Url)"; exit 0 }

$run = Join-Path $GhidraHome "ghidraRun.bat"
if (-not (Test-Path $run)) { Write-Error "ghidraRun.bat not found: $run"; exit 2 }

Write-Output "ghidra-mcp: not answering, starting $run"
Start-Process -FilePath $run -WorkingDirectory $GhidraHome -WindowStyle Minimized

$deadline = (Get-Date).AddSeconds($TimeoutSec)
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Seconds 3
    if (Test-Server) { Write-Output "ghidra-mcp: ok ($Url)"; exit 0 }
}
Write-Error "ghidra-mcp: no answer from $Url after $TimeoutSec s (open the project and enable GhidraMCP?)"
exit 1
