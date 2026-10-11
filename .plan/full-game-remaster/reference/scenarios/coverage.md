# Scene coverage matrix

Plan status: **deferred**. Definitions are prepared; new full-game captures, extraction and gameplay specifications are pending. The inherited title/demo row records completed groundwork only.

90 definitions: 59 scene routes, ten full-level Medium progression segments, 18 difficulty comparison sets, two Beginner availability checks and one inherited demo reference.

BEGINNER L09/L10 rows are availability checks. Mark them unavailable-by-design only after this build's entry/endpoint behavior is observed. They are not missing Beginner playthroughs.

| level | Medium progression/scenes | Beginner comparisons/checks | Herculean comparisons | runtime completion |
|---|---:|---|---:|---|
| L01 Your Basic D.I.D. | 6 planned | matched set planned | 1 planned | pending |
| L02 Hero's Gauntlet | 7 planned | matched set planned | 1 planned | pending |
| L03 The Centaur's Forest | 8 planned | matched set planned | 1 planned | pending |
| L04 The Big Olive | 8 planned | matched set planned | 1 planned | pending |
| L05 Hydra Canyon | 6 planned | matched set planned | 1 planned | pending |
| L06 Medusa's Lair | 6 planned | matched set planned | 1 planned | pending |
| L07 Cyclops Attack | 7 planned | matched set planned | 1 planned | pending |
| L08 Titan Flight | 7 planned | matched set planned | 1 planned | pending |
| L09 Passageways of Eternal Torment | 7 planned | availability check planned | 1 planned | pending |
| L10 Vortex of Souls | 7 planned | availability check planned | 1 planned | pending |

Expected categories, actions, entry alternatives and semantic checkpoints are in each JSON. Numerical routes/checkpoints/ticks remain unresolved until measured. Difficulty comparison sets list the Medium scenes to match and must split into dedicated scene files when differences are observed.

| scenario | scene/group | difficulty | reference | capture | trace | assets | gameplay spec |
|---|---|---|---|---|---|---|---|
| [L01.full-progression](l01-your-basic-did/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L01.training-areas](l01-your-basic-did/training-areas.json) | Training areas and Phil gates | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L01.traversal](l01-your-basic-did/traversal.json) | Ledges, poles and objects | Medium | REFERENCED | PENDING | PARTIAL | PENDING | PENDING |
| [L01.combat-exercises](l01-your-basic-did/combat-exercises.json) | Sword, punches, power punch and ground slam | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L01.secrets](l01-your-basic-did/secrets.json) | Hidden routes and collectibles | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L01.checkpoint-retry](l01-your-basic-did/checkpoint-retry.json) | Checkpoint and training retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L01.difficulty-beginner](l01-your-basic-did/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L01.difficulty-herculean](l01-your-basic-did/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.full-progression](l02-heros-gauntlet/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.gauntlet-opening](l02-heros-gauntlet/gauntlet-opening.json) | Opening gauntlet section | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L02.gauntlet-middle](l02-heros-gauntlet/gauntlet-middle.json) | Middle gauntlet sections | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.gauntlet-final](l02-heros-gauntlet/gauntlet-final.json) | Final gauntlet and completion | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.hazards](l02-heros-gauntlet/hazards.json) | Hazard interactions | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L02.rescue-events](l02-heros-gauntlet/rescue-events.json) | Damsel rescue events | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L02.checkpoint-retry](l02-heros-gauntlet/checkpoint-retry.json) | Gauntlet death and retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.difficulty-beginner](l02-heros-gauntlet/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L02.difficulty-herculean](l02-heros-gauntlet/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L03.full-progression](l03-the-centaurs-forest/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L03.forest-main-route](l03-the-centaurs-forest/forest-main-route.json) | Main forest routes | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L03.background-paths](l03-the-centaurs-forest/background-paths.json) | Background paths and returns | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L03.forest-secrets](l03-the-centaurs-forest/forest-secrets.json) | Hidden areas and destructibles | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L03.nessus-approach](l03-the-centaurs-forest/nessus-approach.json) | Forest-to-Nessus entry | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L03.nessus-attack-damage](l03-the-centaurs-forest/nessus-attack-damage.json) | Nessus attack and damage discovery | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L03.nessus-defeat](l03-the-centaurs-forest/nessus-defeat.json) | Nessus defeat and forest completion | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L03.checkpoint-retry](l03-the-centaurs-forest/checkpoint-retry.json) | Forest/boss retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L03.difficulty-beginner](l03-the-centaurs-forest/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L03.difficulty-herculean](l03-the-centaurs-forest/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L04.full-progression](l04-the-big-olive/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L04.streets](l04-the-big-olive/streets.json) | Thebes streets and route sections | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L04.background-stairs](l04-the-big-olive/background-stairs.json) | Background stairs and paths | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L04.enemy-encounters](l04-the-big-olive/enemy-encounters.json) | Thugs, skeletons and Minotaur | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L04.rescue-events](l04-the-big-olive/rescue-events.json) | Harpies and citizen rescues | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L04.destruction-secrets](l04-the-big-olive/destruction-secrets.json) | Destruction, falling debris and secrets | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L04.section-transition](l04-the-big-olive/section-transition.json) | Second-part/section discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L04.checkpoint-completion](l04-the-big-olive/checkpoint-completion.json) | City retries and completion | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L04.difficulty-beginner](l04-the-big-olive/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L04.difficulty-herculean](l04-the-big-olive/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.full-progression](l05-hydra-canyon/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.canyon-entry](l05-hydra-canyon/canyon-entry.json) | Canyon encounter entry | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L05.hydra-attacks](l05-hydra-canyon/hydra-attacks.json) | Hydra attack cycles | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.hydra-damage-phases](l05-hydra-canyon/hydra-damage-phases.json) | Hydra damage and phase discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.hydra-defeat](l05-hydra-canyon/hydra-defeat.json) | Hydra defeat and exit | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.death-retry](l05-hydra-canyon/death-retry.json) | Hydra death and retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.difficulty-beginner](l05-hydra-canyon/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L05.difficulty-herculean](l05-hydra-canyon/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L06.full-progression](l06-medusas-lair/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L06.lair-traversal](l06-medusas-lair/lair-traversal.json) | Lair traversal and entry | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L06.skeleton-encounters](l06-medusas-lair/skeleton-encounters.json) | Skeleton encounters | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L06.gaze-projectiles](l06-medusas-lair/gaze-projectiles.json) | Medusa gaze and venom | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L06.reflection-defeat](l06-medusas-lair/reflection-defeat.json) | Reflection and defeat | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L06.death-retry](l06-medusas-lair/death-retry.json) | Lair/Medusa retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L06.difficulty-beginner](l06-medusas-lair/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L06.difficulty-herculean](l06-medusas-lair/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.full-progression](l07-cyclops-attack/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.chase-opening](l07-cyclops-attack/chase-opening.json) | Opening Cyclops chase | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L07.chase-middle](l07-cyclops-attack/chase-middle.json) | Successive chase sections | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.alternate-lanes](l07-cyclops-attack/alternate-lanes.json) | Stairs and alternate lanes | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L07.obstacles](l07-cyclops-attack/obstacles.json) | Columns, citizens and chariots | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L07.chase-completion](l07-cyclops-attack/chase-completion.json) | Final chase and completion | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.death-retry](l07-cyclops-attack/death-retry.json) | Cyclops chase retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.difficulty-beginner](l07-cyclops-attack/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L07.difficulty-herculean](l07-cyclops-attack/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L08.full-progression](l08-titan-flight/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L08.flight-opening](l08-titan-flight/flight-opening.json) | Pegasus entry and opening flight | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L08.flight-sections](l08-titan-flight/flight-sections.json) | Successive Pegasus flight sections | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L08.titan-encounters](l08-titan-flight/titan-encounters.json) | Ice, Wind, Lava and Rock Titans | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L08.god-rescues](l08-titan-flight/god-rescues.json) | God rescue events | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L08.zeus-completion](l08-titan-flight/zeus-completion.json) | Final rescue/flight completion discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L08.death-retry](l08-titan-flight/death-retry.json) | Flight death and retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L08.difficulty-beginner](l08-titan-flight/difficulty-beginner.json) | Beginner differential traces | Beginner | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L08.difficulty-herculean](l08-titan-flight/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L09.full-progression](l09-passageways-of-eternal-torment/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L09.passage-opening](l09-passageways-of-eternal-torment/passage-opening.json) | Opening torment passage | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.passage-sections](l09-passageways-of-eternal-torment/passage-sections.json) | Successive passage sections | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.skeletons-lost-souls](l09-passageways-of-eternal-torment/skeletons-lost-souls.json) | Skeletons and lost souls | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.chariot-stalactites](l09-passageways-of-eternal-torment/chariot-stalactites.json) | Chariot and falling stalactites | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.river-falls](l09-passageways-of-eternal-torment/river-falls.json) | River Styx fall/death | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.passage-completion](l09-passageways-of-eternal-torment/passage-completion.json) | Final passage and Vortex transition | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L09.beginner-unavailable](l09-passageways-of-eternal-torment/beginner-unavailable.json) | Beginner availability boundary | Beginner | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L09.difficulty-herculean](l09-passageways-of-eternal-torment/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.full-progression](l10-vortex-of-souls/full-progression.json) | Complete level and linked progression | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.vortex-entry](l10-vortex-of-souls/vortex-entry.json) | Vortex entry and initial control | Medium | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L10.souls-traversal](l10-vortex-of-souls/souls-traversal.json) | Souls and vortex traversal discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.final-encounter](l10-vortex-of-souls/final-encounter.json) | Final encounter and phase discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.rescue-objective](l10-vortex-of-souls/rescue-objective.json) | Final rescue/objective discovery | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.completion-ending](l10-vortex-of-souls/completion-ending.json) | Final completion, ending and credits | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.death-retry](l10-vortex-of-souls/death-retry.json) | Final-level failure and retry | Medium | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L10.beginner-unavailable](l10-vortex-of-souls/beginner-unavailable.json) | Beginner availability boundary | Beginner | REFERENCED | PENDING | PENDING | PENDING | PENDING |
| [L10.difficulty-herculean](l10-vortex-of-souls/difficulty-herculean.json) | Herculean differential traces | Herculean | INFERRED | PENDING | PENDING | PENDING | PENDING |
| [L01.title-demo-baseline](l01-your-basic-did/title-demo-baseline.json) | Inherited title/TRAINING attract-demo evidence | Unknown | REFERENCED | CAPTURED | TRACED | PARTIAL | PENDING |

Completion requires linked Medium progression, complete required scene intervals, observed difficulty differences, confirmed Beginner exclusions, successful required exports and code/trace-supported gameplay. New stage/scene discoveries append IDs and rows; never overwrite an existing ID or count a planned category as a captured asset.
