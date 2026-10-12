# Full-game scenario runner (plan: .plan/full-game-remaster/levels/_shared/S1-runtime-harness.md).
# Runs one scenario and writes analysis/traces/full-game/<level-slug>/<scenario>/<run-id>/:
#   resource-usage.jsonl (trace + identity/state events), input.jsonl (pad bits per change),
#   frames.bin (-Frames: frame<->tick index + pixels), run.json (tools/trace/run_meta.py)
# Entry methods:
#   attract     title idle -> attract demo (no input)
#   new_game    title: CROSS on START GAME (tick 300), START past the level card (tick 420)
#   replay      the pad bits of an earlier run's input.jsonl (-Replay <file>)
#   password    -Password <table index>: enters that password's icons (tools/trace/password_check.py,
#               verified in S6.03) and accepts; difficulty comes from the password; -Pad appends gameplay input
#   debug_level DEBUG ONLY, not an entry method: rewrites the next engine request to -DebugLevel
#               (HERC_HOOK_LEVEL); labelled as such in run.json
# -Difficulty Beginner|Medium|Herculean presets the F2 "Game Play" option (HERC_HOOK_DIFFICULTY);
# "Config" keeps the player's saved option.
param([Parameter(Mandatory = $true)][string]$Scenario, [ValidateSet("attract", "new_game", "password", "replay", "debug_level", "cheat_select")][string]$Entry = "new_game",
      [ValidateSet("Beginner", "Medium", "Herculean", "Config")][string]$Difficulty = "Medium", [int]$Seconds = 120,
      [string]$Replay = "", [string]$Pad = "", [int]$DebugLevel = -1, [int]$Password = -1, [switch]$Frames, [string]$Renderer = "gpu",
      [string]$VramTicks = "", [switch]$Hero, [int]$DebugStart = -1, [switch]$WithInput, [int]$CheatLevel = -1, [switch]$CheatLives)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$py = Join-Path $root ".venv\Scripts\python.exe"
$sc = Get-ChildItem (Join-Path $root ".plan\full-game-remaster\reference\scenarios") -Recurse -Filter *.json |
      Where-Object { (Get-Content $_.FullName -Raw | ConvertFrom-Json).scenario_id -eq $Scenario } | Select-Object -First 1
if (-not $sc) { throw "scenario $Scenario not found" }
$def = Get-Content $sc.FullName -Raw | ConvertFrom-Json
$runId = (Get-Date -Format "yyyyMMdd-HHmmss") + "-$Entry-$($Difficulty.ToLower())"
$out = Join-Path $root "analysis\traces\full-game\$($def.level_slug)\$($sc.BaseName)\$runId"
New-Item -ItemType Directory -Force $out | Out-Null

$env:HERC_HOOK_DIFFICULTY = @{ Beginner = "0"; Medium = "1"; Herculean = "2"; Config = "" }[$Difficulty]
$env:HERC_HOOK_LEVEL = ""
$env:HERC_HOOK_START = if ($DebugStart -ge 0) { "$DebugStart" } else { "" }   # DEBUG ONLY: labels run.json debug_only
$env:HERC_HOOK_PAD = switch ($Entry) {
    "attract" { "" }
    "new_game" { if ($Pad) { "300-303:4000,420-423:0008,$Pad" } else { "300-303:4000,420-423:0008" } }
    "replay" { & $py (Join-Path $PSScriptRoot "run_meta.py") pad $Replay }
    "password" { $pw = & $py -c "import sys, json; sys.path.insert(0, sys.argv[1]); import password_check as p; e = json.load(open(sys.argv[2]))['entries'][int(sys.argv[3])]; print(p.pad_for(e['slot_values'])[0])" $PSScriptRoot (Join-Path $root "analysis\exports\full-game\password_table.json") $Password
                 if ($Pad) { "$pw,$Pad" } else { $pw } }
    "cheat_select" { & $py -c "import sys; sys.path.insert(0, sys.argv[1]); import password_check as p; print(p.cheat_pad(int(sys.argv[2]), sys.argv[3] == '1'))" $PSScriptRoot $CheatLevel ([int][bool]$CheatLives) }
    "debug_level" { $env:HERC_HOOK_LEVEL = "$DebugLevel"; "300-303:4000,420-423:0008" }
}
& (Join-Path $PSScriptRoot "run_trace.ps1") -Seconds $Seconds -OutDir $out -Pairs 16 -Renderer $Renderer -Frames:$Frames -VramTicks $VramTicks -Hero:$Hero -WithInput:$WithInput | Out-Null
& $py (Join-Path $PSScriptRoot "run_meta.py") meta $out --scenario $Scenario --entry $Entry --difficulty $Difficulty `
    --replay "$Replay" --debug-level $DebugLevel
Remove-Item Env:HERC_HOOK_DIFFICULTY, Env:HERC_HOOK_LEVEL, Env:HERC_HOOK_PAD, Env:HERC_HOOK_START -ErrorAction SilentlyContinue
