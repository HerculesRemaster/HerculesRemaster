# Record a human play of one level for later deterministic replay (full-game phase-2 traces).
# The tracer enters the level through an S6-verified password (scripted pad), then hands the pad to the player
# (HERC_HOOK_PAD_HANDOVER, hook event "pad_handover"). The window is visible and keyboard input is enabled.
# Play until the level is done (or the time runs out). Everything the game read from the pad is in
# <run dir>/input.jsonl; tools/trace/run_scenario.ps1 -Entry replay -Replay <that file> replays it exactly
# (RNG is reseeded at game state reset, replays are deterministic).
# Usage: record_play.ps1 -Scenario L03.forest-main-route -Password 14 [-Minutes 15] [-Frames]
param([Parameter(Mandatory = $true)][string]$Scenario, [Parameter(Mandatory = $true)][int]$Password,
      [int]$Minutes = 15, [switch]$Frames)
$ErrorActionPreference = "Stop"
if (Get-Process HERCULES -ErrorAction SilentlyContinue) { throw "game already running" }
$env:HERC_HOOK_PAD_HANDOVER = "1"
try {
    & (Join-Path $PSScriptRoot "run_scenario.ps1") -Scenario $Scenario -Entry password -Password $Password `
        -Seconds ($Minutes * 60) -Hero -WithInput -Frames:$Frames
} finally {
    Remove-Item Env:HERC_HOOK_PAD_HANDOVER -ErrorAction SilentlyContinue
}
