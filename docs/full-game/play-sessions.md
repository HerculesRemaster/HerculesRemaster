# Play sessions needed (human input)

The tasks below can't be scripted: a no-input runner dies at the first heavy hazard section (GAUNTLET x ≈ 13800, CYCLOPS x ≈ 10000, TORMENT x ≈ 3900). Each session records everything the game reads from the pad. Replays are deterministic, so one recorded play feeds every later analysis of that route (`run_scenario.ps1 -Entry replay -Replay <run>\input.jsonl`).

**How to record:**
- Run the command shown for the session from `C:\Games\GameHercules\Remaster` in PowerShell, with no other HERCULES.EXE running.
- The tracer enters the level by password, then hands you the pad (keyboard). Play until the next level has loaded or the timer ends. The run lands in `analysis/traces/full-game/<level>/<scenario>/<run id>/`.

```
.\tools\trace\record_play.ps1 -Scenario <scenario> -Password <n> -Minutes <m>
```

## Runner levels (ENG3) — L02, L07, L09

| # | scenario / password | what to do | feeds |
|---|---|---|---|
| 1 | `L02.full-progression` / 13 (Medium) | Play GAUNTLET to the end and let the tally and the L03 load happen. At **both** crossroads (end of the first and second stretch) keep to the **left** half of the path. Get hit once by a punching bag and once by a sliding wall. | L02.TR.01/03/04/05/09, AS.01, RM.04 densest scene |
| 2 | `L02.gauntlet-final` / 13 | Same level, but keep to the **right** half at both crossroads. Lose one life after passing a Pegasus (winged horse) checkpoint. | L02.TR.04 (other branch), TR.07 |
| 3 | `L07.full-progression` / 19 (Medium) | Play CYCLOPS to the end and the L08 load. Use the stairs / raised walkways whenever you see them; dodge a chariot and a falling column at least once. | L07.TR.01/03/04/05/06/09, RE.03 (stairs) |
| 4 | `L07.death-retry` / 19 | Lose all lives in different places (to see the game-over path). | L07.TR.07, S6 game over / continue |
| 5 | `L09.full-progression` / 21 (Medium) | Play TORMENT to the end and the L10 load. At the fork near the end of the first passage, keep to one side (left half of the path). | L09.TR.01/03/04/05/07/10 |
| 6 | `L09.passage-sections` / 21 | Keep to the **other** side (right half) at the fork. Fall into the River Styx once on purpose. | L09.TR.03 (other route), TR.06 controlled fall |
| 7 | difficulty sets: `L02.difficulty-beginner` / 1, `L02.difficulty-herculean` / 25, `L07.difficulty-beginner` / 7, `L07.difficulty-herculean` / 31, `L09.difficulty-herculean` / 33 | Play the same route as sessions 1, 3 and 5 as closely as you can. | L02/L07.TR.08, L09.TR.08 |

## Platform and boss levels (ENG1): L03, L04, L05, L06, L08, L10

Open-loop input stalls at the first obstacle (FOREST walk stopped at x ≈ 2100). Passwords are the Medium entries unless stated otherwise. A death-retry session can be skipped if you already died in the matching places during the main session; mention which run it was.

| # | scenario / password | what to do | feeds |
|---|---|---|---|
| 10 | `L03.full-progression` / 14 (Medium) | Play FOREST to the end, including the Nessus fight inside it, and let the tally and the L04 load happen. Take side paths and pick up what you see. In the fight, ride Nessus at least once and finish with a charged Power Punch. | L03.TR.03/05/08/09 |
| 11 | `L03.checkpoint-retry` / 14 | Die once before and once after a checkpoint, once inside the Nessus fight, then lose all lives. | L03.TR.06 |
| 12 | `L04.full-progression` / 15 (Medium) | Play THEBES to the end and the L05 load. Rescue every citizen you can, break destructible objects, fight each enemy type you meet. | L04.TR.03/04/05/08/09 |
| 13 | `L04.checkpoint-completion` / 16 (the second Medium THEBES password) | Play until you meet the first Hermes marker, die before and after it, then lose all lives. | L04.TR.05/06 (and what password 16 starts at) |
| 14 | `L05.full-progression` / 17 (Medium) | Fight the Hydra until all heads are cut, then let the tally and the L06 load happen. | L05.TR.02/03/04/05/08/09 |
| 15 | `L05.death-retry` / 17 | Die once early in the fight and once late (after several cuts), then lose all lives. | L05.TR.06 |
| 16 | `L06.full-progression` / 18 (Medium) | Play MEDUSA's lair, fight the skeletons, defeat Medusa, let the tally and the L07 load happen. Get grabbed by Medusa at least once. | L06.TR.02/03/04/05/08/09 |
| 17 | `L06.death-retry` / 18 | Die once in the lair and once in the Medusa fight, then lose all lives. | L06.TR.06 |
| 18 | `L08.full-progression` / 20 (Medium) | Fly TITANS to the end: free every god you can, meet each Titan, reach Zeus, let the tally and the L09 load happen. | L08.TR.02–07/10/11 |
| 19 | `L08.death-retry` / 20 | Die in a normal section and during a Titan encounter, then lose all lives. | L08.TR.08 |
| 20 | `L10.full-progression` / 22 (Medium), `-Minutes 25` | Play VORTEX to the end and watch the ending, credits and return to title without pressing anything. | L10.TR.02–07/11/12 |
| 21 | `L10.death-retry` / 22 | Die once before and once inside the final fight, then lose all lives. | L10.TR.08 |
| 22 | difficulty sets: Beginner `L03.difficulty-beginner` / 2, `L04` / 3, `L05` / 5, `L06` / 6; Herculean `L03.difficulty-herculean` / 26, `L04` / 27, `L05` / 29, `L06` / 30, `L08` / 32, `L10` / 34 | Play the same route as the matching Medium session as closely as you can. Beginner TITANS is session 8; Beginner has no L09/L10 entry. | L03–L06.TR.07, L08.TR.09, L10.TR.09 |

## Progression (S6)

| # | how | what to do | feeds |
|---|---|---|---|
| 8 | `record_play.ps1 -Scenario L08.full-progression -Password 8 -Minutes 20` (TITANS, **Beginner**) | Finish TITANS and watch what follows (static analysis and three routes so far say: the Beginner ending). | S6.06 |
| 9 | new game, Medium, from L01 to the ending (one long session, or several linked sessions continuing by the password shown after each level) | Play the whole game. | S6.08 linked run, all level TR full-progression rows |

The difficulty passwords come from `reference/passwords.json` (S6.03, runtime-verified). Beginner has no L09/L10 entry by design.
