# Sources and verification boundaries

All paths in the workspace column are relative to `C:\Games\GameHercules\Remaster`; paths beginning `reference/` in plan-owned links are relative to this full-game plan. Full-game implementation is deferred.

| source ID | source | use and limitation |
|---|---|---|
| windows-manual | [Disney's Hercules Windows manual](https://cdn.akamai.steamstatic.com/steam/apps/987400/manuals/disneys_hercules_manual.pdf?t=1558107352) | Primary published reference: public level names, available controls, route/encounter groups, password/F3 entry and Beginner restrictions. Does not verify internal IDs or this installed build's exact displayed text. |
| build-static-map | [level map](level-map.md), [dispatch tables](binary-analysis/dispatch-tables.json) | Direct local binary/PE/Ghidra inspection: internal ID, module, descriptor, map load routine and resource path. Static verification is separate from a live load. |
| archive-inventory | `manifests/archive-manifest.json`, `archive-families.json`, `catalog.json`, `docs/archive-format.md` | Canonical 1,024-record inventory; 820 nonempty payloads, ten BIN and ten gameplay CIF maps; names are verified by the game's normalized hash. |
| binary-reports | `docs/binary-map.md`, `module-interface.md`, `resource-id-scheme.md`, `resource-callgraph.md`, `analysis/exports/p2_decomp/00405760.c` | Existing clock, callbacks, FS/palette/render and module-switch findings. New excerpts are kept in this plan's `reference/binary-analysis/`. |
| existing-runtime | `docs/resource-consumers.md`, `docs/runtime-launch.md`, `analysis/traces/runs/demo90`, `demo_anim`, `demo-capture-20261012-003622`, `reference/captures/auto/demo-20261012-003622.json` | Completed title/TRAINING attract-demo, frame requests, sound events and presented-frame capture. No full level/progression claim. |
| existing-decoders | `docs/formats/{esf,tga,i-family,animation,sonix}.md`, `docs/music-transport.md`, `extracted/meta/audio-coverage.json`, `extracted/meta/sonix.json`, `analysis/traces/pairs/` | Completed audio/image/hero-animation milestone, including 559 ESF exports, 64 valid TGA screens, 107 i1.4 banks (10,552 frames) and Sonix/music metadata; retain existing decoders/coverage. Truncated LEV8 and unsupported banks 503/601 remain explicit limitations. External CD music remains absent. |
| canonical-edition | `docs/canonical-edition.md`, `manifests/SHA256SUMS`, `backups/originals/` | Named edition and hashes; originals stay read-only. |

The manual supports training/traversal/combat and hidden routes; forest background paths and Nessus; Thebes stairs, enemies, harpy rescues and debris; Hydra and Medusa encounters; Cyclops alternate routes and traffic; Pegasus flight, elemental Titans and god rescues; and torment passage hazards. Exact phase counts, routes, coordinates, timings and final-level objectives remain runtime discovery targets. The final two levels require Medium or Herculean. The manual's password/F3 descriptions are entry candidates until their prerequisites and accepted values are checked in this build.

The externally found [community password guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1746381038) is a **search lead only**, including its reported Big Olive parts. Do not use its numbering/spelling/passwords as internal-ID evidence. Prefer the primary Windows manual and this build's own loaders/accepted inputs.

New scene evidence must name the binary/archive hashes, entry/difficulty, run path, exact ticks/events and observed outcome. Keep `REFERENCED` or `INFERRED` scene design separate from `CAPTURED`, `TRACED` and `VERIFIED` runtime status. A missing external movie/audio source is a source gap, not a successful extraction.
