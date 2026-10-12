# Hercules Reverse Engineering and 4K Remaster

## Master implementation document

**Version:** 1.0  
**Date:** 11 October 2026  
**Target:** Disney’s Hercules Action Game (1997), Windows edition  
**Development and playback target:** Windows 11  
**Primary outcome:** A reproducible reverse-engineering workflow that supports a faithful game reconstruction and a consistent, high-resolution asset pipeline.

## 1. Purpose and scope

This document organizes the Hercules project into sequential, evidence-driven stages: preserve the original material, establish a playable reference, understand the executable and archive, decode resources, recover game behavior, select an implementation architecture, and upgrade the presentation for a 4K display.

Reverse engineering here means recovering enough verified structure and behavior to implement the intended result. It does not mean automatically recovering the original source code. Decompiler output is an interpretation of compiled instructions and must be checked against assembly, data, and runtime observations.

The intended first product is a faithful 2D remaster foundation, followed by higher-resolution artwork and optional presentation improvements. A full 3D remake is a separate scope decision.

The project owner reports receiving authorization from Disney. This document proceeds on that project context; it does not independently establish the terms of that authorization. Record the agreed scope for binaries, artwork, music, AI-assisted asset processing, and distribution in the project records.

### 1.1 Document basis

This document consolidates the eight existing project documents listed in Section 20. Their binary findings are treated as **previously reported evidence**, not as measurements repeated while preparing this document. The original executable, archive, and running Windows installation were not inspected during this consolidation.

Paths mentioned in earlier project context refer to potentially different copies:

Main root: `C:\Games\GameHercules`

Do not assume these copies are identical. Select a canonical edition and compare hashes before carrying findings from one copy to another.

## 2. Success criteria

The project is successful when:

1. Original inputs are preserved and identified by hashes.
2. A reference version launches reproducibly and provides reliable gameplay observations.
3. The archive extractor accounts for every entry without silently discarding unknown data.
4. Required graphics, animation metadata, audio, and level data have documented decoding paths.
5. A representative level is playable with the original assets and measured behavior.
6. High-resolution replacements preserve animation timing, alignment, collision behavior, and visual identity.
7. The final build runs at 3840 × 2160 under an agreed performance budget on named test hardware.
8. Tools, source, configuration, and asset manifests allow another developer to reproduce the build.

Use milestone gates rather than a fixed calendar estimate. Decoder complexity and the host/DLL interface remain significant uncertainties.

## 3. Evidence rules

Use these labels throughout the repository:

| Label      | Meaning                                                                                   |
| ---------- | ----------------------------------------------------------------------------------------- |
| `REPORTED` | An existing document states the finding; not independently reproduced in the current work |
| `VERIFIED` | Reproduced against a named binary hash, data sample, or runtime trace                     |
| `INFERRED` | A working explanation supported by evidence but not yet demonstrated                      |
| `UNKNOWN`  | No adequate explanation or measurement exists                                             |
| `DECIDED`  | A project choice, not a discovered property of the original game                          |

Every important finding should include its source hash, module and relative virtual address where relevant, evidence location, confidence, and next verification step. Store runtime load addresses separately from module-relative addresses.

Do not infer an asset’s meaning from its filename, magic bytes, size, entropy, or visual plausibility alone. These are useful leads, not proof.

### 3.1 Starting inventory

| Component      | Existing documentation reports                                   | Required confirmation                                           |
| -------------- | ---------------------------------------------------------------- | --------------------------------------------------------------- |
| `HERCULES.EXE` | PE32 x86 host; 275,968 bytes; DirectDraw and DirectSound imports | PE metadata, imports, file hash, observed host responsibilities |
| `ENG1.DLL`     | 481,280 bytes; gameplay-related code                             | Exports, callbacks, state ownership, level usage                |
| `ENG3.DLL`     | 272,896 bytes; another gameplay module                           | Relationship to ENG1 and level usage                            |
| `TITLE.DLL`    | 180,736 bytes; title/menu/tally responsibilities                 | Actual lifecycle and rendering interface                        |
| `HERCULES.FS`  | 46,924,407 bytes; 1,024 index records                            | Layout and integrity against the canonical copy                 |
| Configuration  | Registry values, `DISNEY.INI`, and launch-directory dependencies | Exact accesses during startup                                   |
| Music          | Missing from the previously analyzed folder                      | Disc-image contents and runtime audio access                    |

The existing archive category counts total **1,023**, although the reported index contains **1,024** entries. Recompute classification from the actual file. Preserve this discrepancy until resolved.

Other points to verify:

- An unsorted resource index does not establish whether lookup uses a linear scan, a hash table, or another structure.
- Payload sizes summing to the data-region size do not alone prove the absence of overlaps or gaps.
- Limited DLL imports do not prove a simple callback interface. Modules may receive function pointers, shared buffers, or pointers to complex structures.
- A recognizable number such as 11,025 in an `ESF` header suggests an audio hypothesis; it does not establish the format.
- The original logical resolution, simulation frequency, pixel format, and music transport remain open questions.

## 4. Working environment and tool roles

Use the following as a role-based tool plan. Pin and record the actual versions selected during setup; this document does not assert compatibility for a particular release or plugin.

| Role                          | Candidate tools                                        | Expected output                                                       |
| ----------------------------- | ------------------------------------------------------ | --------------------------------------------------------------------- |
| Binary triage                 | PE inspection tools, Detect It Easy, strings utilities | Architecture, sections, imports, exports, packer indicators           |
| Static analysis               | Ghidra or IDA                                          | Annotated functions, structures, cross-references, subsystem maps     |
| Dynamic analysis              | x32dbg for the reported x86 game                       | Call traces, memory snapshots, argument and return-value observations |
| File and registry observation | Process Monitor                                        | Actual startup and resource-access dependencies                       |
| Structured binary inspection  | ImHex or 010 Editor                                    | Reusable archive and payload structure definitions                    |
| Extraction and conversion     | Python, `struct`, Pillow, NumPy as needed              | Deterministic parsers, decoded resources, reports                     |
| Audio inspection              | WAV export and an audio editor                         | Verified sample formats and playback checks                           |
| Reference capture             | Video capture and frame comparison tools               | Timestamped scenarios and comparison material                         |
| Compatibility experiments     | A suitable DirectDraw wrapper                          | A runnable reference configuration                                    |
| Automation                    | Ghidra scripts, headless jobs, optional MCP bridge     | Repeatable analysis and bounded agent access                          |

Start with one primary analysis database. A second decompiler can resolve difficult cases, but manually maintained names and types should not diverge without an export strategy.

A modern graphics capture tool may require a compatible wrapper/backend. Do not assume it can capture the original DirectDraw path or reconstruct individual sprites from a final framebuffer.

### 4.1 Ghidra, headless analysis, and MCP

These serve different purposes:

- **Interactive Ghidra/IDA:** inspect difficult control flow, correct types, and review cross-references.
- **Headless analysis:** repeat imports, run known scripts, and export repeatable results.
- **MCP integration:** allow an assistant to query or annotate an analysis session through the capabilities the selected bridge actually exposes.

Evaluate any MCP bridge with a small test project before using it on the main database. Confirm program selection, address interpretation, supported operations, error reporting, and backup behavior. Begin with reading functions, strings, and references; enable mutation only after verifying that annotations affect the intended program.

MCP does not replace a debugger, establish a file format, or make decompiled output authoritative.

## 5. Project layout and reproducibility

Use a dedicated workspace, for example `C:\Projects\HerculesRemaster\`, with these responsibilities:

| Directory            | Contents                                                       |
| -------------------- | -------------------------------------------------------------- |
| `originals/`         | Preserved source binaries, archives, and disc-image references |
| `working/`           | Copies used for runtime and compatibility experiments          |
| `analysis/`          | Ghidra/IDA projects, exports, traces, evidence records         |
| `tools/`             | Extractors, decoders, validators, conversion scripts           |
| `assets/raw/`        | Exact extracted payloads                                       |
| `assets/decoded/`    | Lossless decoded graphics, audio, and metadata                 |
| `assets/remastered/` | Reviewed replacement artwork                                   |
| `assets/runtime/`    | Packed atlases and runtime-ready resources                     |
| `reference/`         | Gameplay recordings, scenario definitions, measured values     |
| `runtime/`           | Host, renderer, or reconstructed game                          |
| `tests/`             | Parser fixtures, behavioral checks, replay comparisons         |
| `docs/`              | Specifications, decisions, task status, unresolved questions   |

Track code and text specifications in version control. Keep source media, analysis databases, and large generated assets in appropriately controlled storage with versioned manifests. Preserve originals independently of Git.

Record command line, tool version, input hash, configuration hash, output hashes, and timestamp for every reproducible transformation. Make jobs resumable by stable asset identity and input/configuration hashes.

## 6. Phase 0 — Preserve, identify, and baseline

**Objective:** Establish trustworthy inputs and a usable behavioral reference.

### Procedure

1. Inventory the disc image, installer, installed game, and previously analyzed folder.
2. Hash all relevant inputs using SHA-256; record sizes and relative paths.
3. Compare editions and select the canonical copy. Record differences rather than merging files arbitrarily.
4. Preserve originals and create a separate working copy.
5. Record Windows build, display settings, audio configuration, executable working directory, registry dependencies, and installed compatibility components.
6. Launch the reference game. If startup fails, capture the exact message and file/registry trace before changing anything.
7. Separate installation detection, missing-resource paths, disc detection, graphics initialization, and audio failures into distinct issues.
8. Change one compatibility setting at a time. Record the result and rollback method.
9. Capture a short representative scenario as soon as the game runs: title screen, movement, jump, attack, damage, and level restart.
10. Expand coverage across the game as analysis progresses. A complete playthrough is useful, but need not block archive work.

Measure the game’s image dimensions and update cadence where possible. Video output frequency alone does not establish simulation frequency; duplicated or dropped frames can mislead measurements.

**Outputs:** `file-inventory.json`, `SHA256SUMS`, `baseline-environment.md`, `startup-trace`, `reference-scenarios.md`.

**Gate G0:** Inputs are identified and preserved; either a repeatable playable reference exists or the specific startup blocker is documented. Static archive and binary analysis may continue while a runtime blocker is investigated.

## 7. Phase 1 — Map the executable and module boundaries

**Objective:** Recover the resource path and host/module contract before attempting a broad source reconstruction.

### Procedure

1. Import the canonical EXE and each DLL into the primary analysis tool. Confirm architecture and image metadata from the binaries.
2. Export imports, exports, strings, entry points, and module hashes.
3. Find references to archive names, module names, startup errors, and configuration keys.
4. Trace startup through window creation, configuration reads, resource initialization, module loading, and the main loop.
5. Locate the archive-open, index-read, resource-lookup, payload-read, and payload-dispatch functions.
6. Trace dynamic module loading and symbol resolution. Determine which exports actually exist; names found only in strings are leads.
7. Identify callback tables, shared state, buffers, allocator/free pairs, and module lifecycle functions.
8. Name functions conservatively. Use provisional names until their behavior is supported by evidence.
9. Record addresses as module plus RVA, together with the binary hash.

Prioritize these questions:

- Who owns the game loop and simulation clock?
- Who decodes resources?
- Who owns decoded buffers and releases them?
- Does rendering cross the boundary as sprites, tiles, draw commands, or a completed framebuffer?
- How are input, sound, level changes, and shutdown communicated?

**Outputs:** `binary-map.md`, `resource-callgraph.md`, `module-interface.md`, exported annotations.

**Gate G1:** The archive-loading path is identified and the host/module boundary is documented well enough to guide runtime tracing. Full decompilation is not required.

## 8. Phase 2 — Validate and extract the archive

**Objective:** Produce a complete, lossless inventory of archive payloads.

The existing documentation reports this layout for its particular copy:

| Field          | Reported representation                                |
| -------------- | ------------------------------------------------------ |
| Index length   | 1,024 records × 12 bytes = 12,288 bytes (`0x3000`)     |
| Record field 1 | Resource ID, little-endian unsigned 32-bit             |
| Record field 2 | Absolute payload offset, little-endian unsigned 32-bit |
| Record field 3 | Payload size, little-endian unsigned 32-bit            |

Treat this as a candidate specification until validated against the selected archive and reader code.

### Extractor requirements

1. Check that the complete index fits in the file.
2. Validate every offset and length using bounds checks that cannot overflow.
3. Sort non-empty payload intervals by offset and explicitly report overlaps and gaps.
4. Record zero-length and very short entries without pretending they contain a magic header.
5. Report duplicate IDs and identical payload hashes; preserve the original index number.
6. Do not use the resource ID alone as a unique output filename.
7. Extract exact bytes without modifying headers or trying to decode them.
8. Write a machine-readable manifest and a human-readable summary.
9. Verify that each output equals its source slice.
10. Recompute magic classifications and resolve the existing 1,023/1,024 discrepancy.

Each manifest record should contain:

`source_archive_sha256`, `entry_index`, `resource_id_hex`, `offset`, `size`, `magic_hex`, `payload_sha256`, `output_path`, `validation_status`, `classification_status`.

Add semantic names as annotations later. Keep raw identity stable.

Entropy and size histograms help select representative samples; they do not establish compression or content type.

**Outputs:** `fs_extract.py`, raw payloads, `archive-manifest.json`, `archive-validation.json`, `archive-format.md`.

**Gate G2:** Every index record is accounted for, extraction is byte-exact, and integrity exceptions are explicit. Unknown formats remain valid inventory entries.

## 9. Phase 3 — Correlate resources with runtime behavior

**Objective:** Connect archive entries to actual consumers and game scenarios.

### Procedure

1. Trace archive opening and reading at the operating-system API boundary.
2. Use the static call graph to locate the game’s internal resource lookup.
3. Break or instrument there and log requested IDs, calling module/RVA, scenario, and destination buffer information.
4. Capture a title-screen load, one gameplay load, one character animation, and one sound event.
5. Follow buffers into the decoder or consumer.
6. When practical, capture both input payload bytes and decoded output for the same resource.
7. Determine buffer dimensions, stride, pixel/sample format, palette references, and ownership from the actual consumer.

A buffered or memory-mapped implementation may not produce a file read for every resource request. Use API traces to locate internal boundaries, then observe those boundaries directly.

Log enough information to reproduce the observation. Avoid heavy per-frame tracing when it distorts timing.

**Outputs:** `resource-usage.jsonl`, captured input/output pairs, `resource-consumers.md`.

**Gate G3:** At least one representative resource has a traceable path from archive entry to a verified image, sound, or level-data consumer.

## 10. Phase 4 — Decode graphics, audio, and compound resources

**Objective:** Convert verified formats into lossless, documented intermediate assets.

### 10.1 Choose the next format by evidence

The reported tags include `i1.4`, `i1.3`, `i1.9`, `i1.a`, `i2.5`, and `ESF\x03`. Do not assume these are all image formats or that one is necessarily easier than another.

Start with the family for which a runtime consumer and small sample are understood. For each family:

1. Compare several small and large payloads.
2. Mark confirmed headers, version fields, counts, and tables.
3. Determine whether offsets are absolute, relative to the payload, or relative to another block.
4. Validate lengths against both the file and the decoder’s accesses.
5. Recover decompression from the consuming code when structure inspection is insufficient.
6. Implement a bounded decoder with explicit errors for unsupported cases.
7. Compare decoded output to a captured original buffer or another independent reference.
8. Expand from a small fixture set to the full family and report coverage.

### 10.2 Image decoding

Recover dimensions, stride, row order, bit depth, palette selection, transparency rules, compression, and clipping. A plausible image is not sufficient: incorrect palettes or row alignment can still produce recognizable output.

Preserve indexed pixels and palette metadata when relevant, alongside RGBA previews. Record palette changes, shared palettes, transparent indices, and any effects that depend on index values.

### 10.3 Animation decoding

Recover more than individual frame images:

- Sequence membership and frame order.
- Frame duration in original timing units.
- Canvas bounds, trimmed bounds, pivot, and anchor convention.
- Facing direction and mirroring.
- Attachment points and draw order.
- Animation events and associated hitbox/hurtbox changes where present.

Use an animation viewer to validate full loops and transitions. Contact sheets alone will not reveal timing errors or flicker.

### 10.4 Audio decoding

For `ESF` and other candidates, verify sample rate, channel count, encoding, sample length, and loop metadata through multiple headers and the audio playback path. Do not infer a codec solely from a `vabid` string or possible console heritage.

Export lossless WAV only after the encoding is established. Keep the original payload and metadata. Check duration, clipping, playback speed, looping, and channel order.

### 10.5 Level and compound data

Separate tile references, visual layers, collision data, object spawns, triggers, and camera information when those structures are found. Preserve unrecognized fields and original offsets for later investigation.

**Outputs:** format specifications, decoders, fixtures, PNG/WAV previews, animation metadata, contact sheets, coverage report.

**Gate G4:** Assets required for a representative playable slice decode reproducibly and match observed behavior. Full-game decoding continues as later content is integrated.

## 11. Phase 5 — Recover level structure and gameplay rules

**Objective:** Build a behavioral specification that can drive reconstruction or validate a replacement host.

### Level reconstruction

1. Identify dimensions, coordinate units, tile sizes, layer order, and parallax factors.
2. Associate graphical layers with their palettes and tilesets.
3. Recover collision shapes, slope rules, platform behavior, and camera limits.
4. Map object IDs to spawns, enemies, pickups, hazards, and triggers.
5. Render a static level preview with toggles for each visual and collision layer.
6. Compare landmark positions and object placement with the reference game.

### Gameplay reconstruction

Study one behavior at a time: idle, movement, jump, landing, attack, damage, death, restart, then enemy and boss behavior.

For each behavior, record:

- Entry and exit conditions.
- State variables and coordinate units.
- Update order and timing source.
- Constants, including their original numeric representation where relevant.
- Input buffering and cancellation rules.
- Collision, damage, animation, and sound events.
- Reference scenario and acceptable comparison tolerance.

Do not replace fixed-point arithmetic or timing assumptions with a new convention without measuring the effect. Video can corroborate behavior but is weaker evidence than instrumented state when camera motion or frame capture obscures it.

Create scenario-based checks for jump trajectories, movement distance, hit timing, enemy reactions, checkpoint behavior, and level completion. If deterministic input replay is unavailable, use repeated controlled runs with documented tolerances.

**Outputs:** `level-format.md`, converted level data, `gameplay-spec.md`, `tuning.json`, reference scenarios.

**Gate G5:** One level and its essential behaviors can be explained and reproduced from documented data and rules.

## 12. Phase 6 — Select the runtime architecture

Choose after understanding what crosses the rendering boundary.

| Approach              | What is retained                            | Main advantage                                                 | Main uncertainty                                                                                 |
| --------------------- | ------------------------------------------- | -------------------------------------------------------------- | ------------------------------------------------------------------------------------------------ |
| Compatibility wrapper | Original EXE, DLLs, and game data           | Fast route to a usable behavioral reference                    | Final-frame interception may expose too little information for high-resolution asset replacement |
| Replacement host      | Original gameplay DLLs; new host services   | Potentially preserves gameplay while replacing presentation    | ABI, shared state, memory ownership, and rendering granularity                                   |
| Reimplementation      | Recovered specifications and converted data | Full control over rendering, content, and platform integration | Effort needed to reproduce behavior accurately                                                   |

A wrapper can improve presentation, but true high-resolution resource replacement or expanded camera coverage requires suitable engine-level interception or changes. Determine feasibility from the observed pipeline.

For a replacement host, document calling conventions, structure packing, callbacks, allocation/free pairing, initialization order, and threading. The reported x86 DLLs require a compatible process; a 64-bit host cannot load them directly as ordinary in-process modules. A separate process bridge adds serialization and synchronization work.

For a reimplementation, select an engine based on the team’s existing strengths and the verified requirements. Existing notes propose Godot, Unity, or a custom runtime; no engine is mandated by this document.

Do not label a reconstruction “clean-room” merely because it is newly written. That term describes a specific separation process, not an automatic property of a rewrite.

**Default sequencing:** Keep a runnable original as reference, prove one representative slice with original assets, then commit to the long-term runtime architecture.

**Output:** `architecture-decision.md`, including a small prototype, measured limitations, and fallback route.

**Gate G6:** The selected approach demonstrates the control needed for the intended asset replacement and preserves essential behavior.

## 13. Phase 7 — Build a playable vertical slice

**Objective:** Prove the complete data-to-game pipeline before bulk art production.

Implement a representative section with:

- Original decoded background and tiles.
- Character movement, jump, attack, damage, and restart.
- One enemy, one hazard, and one pickup.
- Correct animation sequencing and anchors.
- Camera movement and collision visualization.
- Sound playback and basic input handling.

Keep world coordinates and gameplay rules independent of output resolution. Match the measured simulation cadence and decouple rendering where the architecture supports it.

Provide debug overlays for state, simulation tick, animation frame, pivot, collision geometry, and resource ID. These are development tools and should not intrude into the final player interface.

**Gate G7:** A complete playable sequence using original assets matches the reference within documented tolerances. Remaining differences are understood before replacement art is introduced.

## 14. Phase 8 — Establish the 4K asset pipeline

**Objective:** Upgrade artwork while preserving its relationship to the game.

### 14.1 Define 4K correctly

3840 × 2160 is an output target, not a requirement that every texture be 4K. Determine texture size from the largest intended on-screen size, camera scale, and filtering strategy.

For example, if a verified logical view were 640 × 480, fitting it within 3840 × 2160 would use a scale of 4.5 and occupy 2880 × 2160. This is an illustration, not a claim that Hercules uses that exact logical view.

Choose aspect-ratio behavior explicitly: preserve the original view with borders, crop, or extend camera coverage. Extended coverage requires checks for level bounds, enemy activation, hidden areas, and scripted encounters.

### 14.2 Preserve a stable asset contract

Every replacement should map to a stable original asset key. Keep:

| Data            | Requirement                                                                 |
| --------------- | --------------------------------------------------------------------------- |
| Source identity | Archive hash, entry index, subresource/frame index                          |
| Geometry        | Logical bounds, crop rectangle, pivot, anchor convention                    |
| Timing          | Original frame duration, sequence order, events                             |
| Gameplay        | Collision and attachment metadata independent of decorative pixels          |
| Art provenance  | Source references, generator/model version if used, settings, review status |
| Output          | Image dimensions, alpha convention, color space, content hash               |

Higher pixel dimensions must not silently increase an object’s world-space size. Map replacement pixels back to the original logical dimensions and anchors.

### 14.3 Pilot before batch production

Select a small but representative set: an idle loop, a run loop, an attack, an enemy, a background section, a repeating tile, and a UI element.

1. Establish approved character proportions, line weight, palette, shading, and texture detail.
2. Decode and preserve original alpha, palette, pivots, and timing.
3. Prepare reference images for the whole character and animation sequence.
4. Generate or redraw replacements with consistent references and documented settings.
5. Compare adjacent frames and play the full sequence at game speed.
6. Correct shape drift, flicker, broken silhouettes, unstable detail, and edge contamination.
7. Check alignment, attachments, seams, and readability in the playable slice.
8. Approve the pilot before processing the remaining asset families.

Independent frame generation can introduce temporal inconsistency. Use sequence-level review and shared references; a fixed seed alone does not guarantee consistency.

### 14.4 Batch job design

Use explicit states:

`pending`, `processing`, `generated`, `validation_failed`, `review_required`, `approved`, `packed`.

Maintain immutable source assets, per-job provenance, bounded retries, cost/time records, and resumable outputs. Include all meaningful generation settings in the job identity. Approval is a distinct step from successful file creation.

Automatic checks should cover dimensions, alpha bounds, naming, stable identity, anchor transforms, required frame count, and tile seams. Visual review should cover character identity, anatomy, line quality, animation consistency, and scene composition.

Pack approved assets into atlases with appropriate padding, edge handling, and filtering. Evaluate memory use before selecting a uniform upscale factor: increasing both dimensions by a factor of four creates sixteen times as many texels before compression and mipmaps.

**Outputs:** art guide, replacement manifest, pilot assets, resumable batch pipeline, reviewed runtime atlases.

**Gate G8:** The pilot looks consistent in motion at target display size and does not alter gameplay alignment or event timing.

## 15. Phase 9 — Expand content, optimize, and package

Scale the proven workflow across levels and resource families. Track content by level and behavior, not only by the number of decoded files.

For each level, check layout, collision, enemy placement, scripts, camera, sound, checkpoints, scoring, and completion. Preserve a switch to original assets where feasible so visual and gameplay regressions can be isolated.

Set a measured performance budget for named hardware. An initial target of 60 presentation frames per second allows approximately 16.67 ms per frame; this is a project target, not a measured result or a reason to change the original simulation rate.

Measure frame-time distribution, stalls, texture memory, CPU usage, audio stability, and load time. Test representative busy scenes, not only the title screen.

Package the build with versioned resource manifests, required notices, save/settings behavior, and a reproducible build procedure. Verify installation and launch in a clean Windows 11 environment.

**Gate G9:** The intended content is playable, visual and behavioral regressions are resolved or explicitly accepted, performance targets pass, and the build can be reproduced and installed independently.

## 16. AI-assisted analysis operating rules

Use assistants for bounded research and implementation tasks with explicit evidence requirements.

Every task should state:

1. The exact binary or asset hash and module under analysis.
2. The question to answer.
3. Available evidence and current hypotheses.
4. Allowed changes and expected output files.
5. A verification procedure.
6. Stop conditions and unresolved questions.

Good task example: “Identify the resource lookup routine for the selected `HERCULES.EXE`; report module-relative addresses, calling convention evidence, resource-ID comparisons, and a runtime verification plan.”

Avoid unbounded tasks such as “decompile the whole game and rewrite it.” Require assistants to distinguish observed instructions from inferred semantics, preserve unknown fields, and report unsupported format variants explicitly.

Keep a task ledger with `ready`, `in_progress`, `blocked`, and `verified` states. Save durable notes after each investigation so another session can continue without repeating the work.

## 17. Validation matrix

| Area            | Validation                                                           | Passing evidence                                   |
| --------------- | -------------------------------------------------------------------- | -------------------------------------------------- |
| Inputs          | Compare hashes and edition metadata                                  | Canonical manifest and preserved originals         |
| Archive         | Bounds, interval coverage, duplicate handling, byte-exact extraction | Complete validation report                         |
| Decoder         | Compare against captured original output where available             | Representative fixtures and family coverage        |
| Animation       | Sequence playback, pivots, duration, event checks                    | Stable loops and correct transitions               |
| Levels          | Layer overlays and landmark comparison                               | Correct geometry, objects, collision, and triggers |
| Gameplay        | Controlled scenarios and state comparison                            | Recorded tolerances met                            |
| Replacement art | In-motion review and asset-contract checks                           | Approved pilot and per-family review               |
| 4K presentation | Aspect ratio, UI legibility, seams, frame times                      | Captures and measurements on named hardware        |
| Release         | Clean installation and reproducible build                            | Independent launch and build verification          |

Tests should target actual risks: malformed offsets, decoder bounds, palette selection, timing, ownership errors, and gameplay divergence. Do not count an extractor validating its own assumptions as independent format confirmation.

## 18. Risk register and fallback paths

| Risk                       | Early signal                                      | Response                                                             |
| -------------------------- | ------------------------------------------------- | -------------------------------------------------------------------- |
| Wrong source edition       | Hashes or file sizes differ                       | Keep edition-specific manifests and revalidate assumptions           |
| Reference game blocked     | Startup failure prevents tracing                  | Continue static work while resolving the exact runtime dependency    |
| Misclassified payload      | Header guesses conflict with consumer behavior    | Follow the runtime buffer and revise the format hypothesis           |
| Decoder uncertainty        | Plausible output differs from original buffer     | Revisit stride, palette, bounds, and decompression semantics         |
| Host interface too complex | Extensive shared state or framebuffer-only output | Limit the host prototype and evaluate reimplementation               |
| Gameplay mismatch          | Divergence despite matching artwork               | Check timing, arithmetic, update order, and collision rules          |
| AI animation instability   | Flickering details or changing proportions        | Stop the batch; refine references and sequence-level correction      |
| Widescreen side effects    | Early spawns or exposed unfinished regions        | Retain original framing until wider coverage is validated            |
| Excess texture memory      | Stalls or high residency cost                     | Use per-asset resolution budgets, paging, and suitable packing       |
| Missing audio              | Reference folder lacks tracks                     | Inspect the canonical disc/media and document the restoration source |

## 19. First implementation backlog

Execute these actions in order, allowing read-only static work to continue when runtime setup is blocked:

1. Select and hash the canonical game copy.
2. Create the workspace and preservation manifest.
3. Reproduce startup and capture a short reference scenario.
4. Import the EXE and DLLs into the analysis database.
5. Locate the archive reader and validate the proposed index layout.
6. Implement bounded, byte-exact extraction and resolve count discrepancies.
7. Trace one resource from lookup through its consumer.
8. Decode one representative graphic or sound with independent verification.
9. Build a resource catalog and animation viewer as decoded content becomes available.
10. Recover one level section, collision layer, and core character behavior.
11. Prototype the runtime architecture and record the decision.
12. Build a playable slice using original assets.
13. Approve a small 4K art pilot in motion.
14. Expand decoding, gameplay coverage, and asset production incrementally.

Do not make bulk asset generation the first milestone. The first critical milestone is a verified mapping from original bytes to an understood runtime asset, followed by a playable slice that proves replacement compatibility.

## 20. Source documents and maintenance

This master document consolidates the following project material read on 11 October 2026:

- `README(2).md` — project overview and document map.
- `01-file-inventory.md` — reported files, sizes, and binary metadata.
- `02-system-architecture.md` — reported host, DLL, rendering, input, and audio structure.
- `04-asset-archive.md` — reported archive layout and payload classification.
- `05-modernization-strategy.md` — runtime and visual modernization options.
- `06-roadmap-and-tasks.md` — phased implementation backlog.
- `07-risks-legal-open-questions.md` — earlier uncertainties and risk notes.
- `08-reverse-engineering-guide.md` — initial extraction and analysis procedure.

`03-config-and-install.md` is referenced by the source set but was not available among the eight supplied documents read for this consolidation. Its contents have not been assumed.

This is an implementation plan, not a declaration that its milestones have been completed. Update findings only when linked evidence supports the change. For each milestone, record the input hashes, completed artifacts, verification result, unresolved blockers, and next action.

### Completion checklist

- [ ] G0: Canonical inputs preserved and baseline status documented.
- [ ] G1: Resource path and module boundaries mapped.
- [ ] G2: Archive extraction complete and byte-exact.
- [ ] G3: Resource-to-runtime correlation demonstrated.
- [ ] G4: Vertical-slice resources decoded and verified.
- [ ] G5: Level structure and essential behavior specified.
- [ ] G6: Runtime architecture demonstrated and selected.
- [ ] G7: Original-asset playable slice matches the reference.
- [ ] G8: High-resolution pilot approved in motion.
- [ ] G9: Full intended build validated, optimized, and reproducible.
