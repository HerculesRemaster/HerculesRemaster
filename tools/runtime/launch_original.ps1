# Launch working/game/HERCULES.EXE from its folder, log window titles/dialogs, take screenshots,
# then close the game. Output: analysis/runtime/launch/<stamp>/{log.json,shot_<s>s.png}.
param(
    [int[]]$ShotsAt = @(2, 5, 10, 30),
    [int]$RunSeconds = 0,           # 0 = last screenshot + 1 s
    [string]$Exe = "HERCULES.EXE",
    [string]$OutDir = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot "win32.ps1")
$game = Join-Path $root "working\game"
if (-not (Test-Path (Join-Path $game $Exe))) { throw "no working copy; run tools/runtime/make_working_copy.ps1" }
if (Get-Process -Name ([IO.Path]::GetFileNameWithoutExtension($Exe)) -ErrorAction SilentlyContinue) { throw "game already running" }
if (-not $OutDir) { $OutDir = Join-Path $root ("analysis\runtime\launch\" + (Get-Date -Format "yyyyMMdd-HHmmss")) }
New-Item -ItemType Directory -Force $OutDir | Out-Null
if ($RunSeconds -le 0) { $RunSeconds = ($ShotsAt | Measure-Object -Maximum).Maximum + 1 }

$p = Start-Process -FilePath (Join-Path $game $Exe) -WorkingDirectory $game -PassThru
$t0 = Get-Date
$events = [Collections.Generic.List[object]]::new()
$seen = @{}
$shots = [Collections.Generic.List[int]]::new($ShotsAt)
while (-not $p.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt $RunSeconds) {
    $el = [math]::Round(((Get-Date) - $t0).TotalSeconds, 1)
    foreach ($w in [HercWin]::Windows([uint32]$p.Id)) {
        $key = "$($w.Handle)|$($w.Title)|$($w.Class)"
        if (-not $seen.ContainsKey($key)) {
            $seen[$key] = 1
            $events.Add([ordered]@{ t = $el; hwnd = "$($w.Handle)"; title = $w.Title; class = $w.Class
                rect = "$($w.Left),$($w.Top),$($w.Right),$($w.Bottom)"; text = [HercWin]::ChildText($w.Handle) })
        }
    }
    if ($shots.Count -and $el -ge $shots[0]) {
        $s = $shots[0]; $shots.RemoveAt(0)
        $main = [HercWin]::Windows([uint32]$p.Id) | Select-Object -First 1
        if ($main) {
            [void][HercWin]::SetForegroundWindow($main.Handle)
            Save-Screenshot (Join-Path $OutDir "shot_${s}s.png") $main.Handle
        }
    }
    Start-Sleep -Milliseconds 250
}
$exited = $p.HasExited
if (-not $exited) {
    [void]$p.CloseMainWindow()
    if (-not $p.WaitForExit(5000)) { Stop-Process -Id $p.Id -Force }
}
$p.WaitForExit()
$log = [ordered]@{
    exe = $Exe; started = $t0.ToString("o"); exited_by_itself = $exited
    seconds = [math]::Round(((Get-Date) - $t0).TotalSeconds, 1); exit_code = $p.ExitCode
    windows = $events
}
$log | ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 (Join-Path $OutDir "log.json")
$titles = ($events | ForEach-Object { "$($_.title) [$($_.class)]" }) -join "; "
Write-Output "exit=$($p.ExitCode) self_exit=$exited windows=$($events.Count): $titles -> $OutDir"
