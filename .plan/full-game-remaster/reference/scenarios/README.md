# Full-game scenarios and stage discovery

These are **deferred plan definitions**, owned by `.plan/full-game-remaster/`. The active lean plan and `Remaster/reference/scenarios/` are unchanged. Do not start captures or implement these scenarios until the user starts the full-game plan.

The [level registry](../levels.json) and [level map](../level-map.md) establish the ten public levels and their verified static Windows IDs/resources. [coverage.md](coverage.md) is the readable matrix; [coverage.json](coverage.json) holds the same scenario rows. [schema.json](schema.json) describes the scenario contract. [sources.md](../sources.md) separates primary references, local static evidence and runtime gaps.

Each `reference/scenarios/<level>/<scenario>.json` contains:

- A stable scenario ID, public level, internal ID, engine, scene group and difficulty.
- Entry method, verification status, prerequisites and candidate shortcut parameters. Password icons, checkpoint IDs and replay paths remain null until verified.
- Route/actions and semantic entry, scene, event and exit checkpoints. These are observation boundaries; they do not assert that a Hermes/game checkpoint exists in every scene.
- Expected asset categories and capture start/end boundaries, input/audio/frame/tick requirements and display metadata.
- Reference confidence, actual runtime evidence status, source IDs, run/capture paths, tick intervals, asset/specification associations and open questions.
- Difficulty comparison policy and separate capture, trace, extraction and gameplay-specification coverage.

All new scene/route/progression definitions start `PLANNED`. `REFERENCED` means the scene group is supported by a source; `INFERRED` marks a discovery target. Neither means gameplay was captured. The inherited title/demo definition is `TRACED`, has recorded artifact paths and an unknown difficulty, and does not count as full L01 completion.

There are ten linked Medium `full-progression` segments and separate scene routes. A progression segment starts before the level load and includes completion/tally and the next load; L10 includes the ending. Optional/background/secret scene passes supplement the normal route. Scene intervals may refer into a complete progression recording when their full boundaries and initial state are available; avoid recapturing identical evidence solely to create another file.

Each level has a Herculean comparison set. L01–L08 have Beginner comparison sets; L09/L10 instead have `beginner-unavailable` checks. Comparison sets list the Medium routes to match. Start with level-load resources and representative mechanics, then extend to all differing scenes. Differences in resources, actors/spawns, damage, timing, routes, phases or progression require dedicated scene comparison files. Record observed equality only for the routes/initial states compared. Confirm Beginner's L08 endpoint and normal-entry rejection before changing availability checks to `UNAVAILABLE_BY_DESIGN`.

The initial groups are deliberately provisional. Split successive sections, individual Titans, alternate paths, attacks or boss phases as actual video/state/resource boundaries are recovered. Unknown phase counts, coordinates, checkpoint IDs and timings stay null or unresolved. Vortex final objectives/actors require direct runtime/code confirmation.

## Search before adding a stage or scene

From `C:\Games\GameHercules\Remaster`, search the existing plan and source reports first:

```powershell
rg -n -i 'nessus|thebes|titans|torment|vortex' .plan/full-game-remaster/reference docs analysis/exports
rg -n -i 'MAP_PSX|ANIMPSX|DEMO5_' manifests/archive-manifest.json manifests/name-candidates.json
rg -n 'P3.ALL.|P4.ALL.|P5.ALL.' .plan/full-game-remaster
```

For newly captured runs, search normalized FS names/IDs, module/level/substage changes, checkpoint events, sequence changes and streamed-resource boundaries under `analysis/traces/full-game/`. Follow direct callers/consumers in the existing Ghidra GUI project using program+RVA. Search the primary Windows manual and actual Windows gameplay recordings for route leads; community numbering/password tables are leads to verify in this build. A filename, apparent phase or public walkthrough number alone does not establish an internal stage.

## Add and refine without changing IDs

1. Search the registry and scenario IDs for the same stage/scene/route. Refine an existing definition if the identity is unchanged; retain its ID and artifact links.
2. Record the discovery source, binary/archive hashes, actual internal level/substage ID, module/load routine, normalized map/resource paths and runtime relationship. Unverified candidates go into `levels.json`'s `extra_stage_descriptors` with `UNKNOWN` status.
3. A newly observed substage under one of the ten public levels stays attached to that level. Append a distinct stage descriptor and scene file when justified. Do not turn ID 4/Nessus or a Thebes section into an eleventh public level. Truly new user-requested content would be a separately labeled extension with an append-only ID.
4. Copy the closest existing scene JSON and assign an unused descriptive ID such as `L08.titan-ice-encounter`. Fill entry/difficulty/actions/checkpoints/categories/boundaries and questions. Set new runtime evidence to `PLANNED`; references may be `REFERENCED` or `INFERRED`.
5. For a difficulty-specific difference, append a distinct scenario such as `L05.hydra-damage-herculean`, keep the Medium comparison target and record the measured difference. Do not reuse a Medium ID for another difficulty.
6. Add the row to both coverage files, update summary counts and comparison/progression links where applicable, and add append-only `P3.ALL.*`/`P4.ALL.*`/`P5.ALL.*` tasks only if the new work needs its own task. Existing IDs and completed milestones stay intact.
7. When implementation runs later, attach actual run paths/tick ranges and update each coverage dimension separately. A partial/failed entry retains its artifacts and next action; it does not close the scene.

Future outputs use the isolated locations in [START-HERE.md](../../START-HERE.md): `analysis/traces/full-game/`, `reference/captures/full-game/`, `extracted/meta/full-game/`, `extracted/full-game/` and `docs/full-game/`. Existing lean artifacts stay at their original paths and may be linked as inherited evidence.

Before closing a row, confirm JSON/schema fields, unique IDs, valid level/difficulty mapping, real evidence files and monotonic tick/event ranges, complete capture boundaries, expected/observed/resident asset reconciliation, required successful exports and code/trace-supported behavior. These are output checks for the actual work; no plan orchestration framework or duplicate evidence ledger is required.
