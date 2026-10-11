# Static verification excerpts

These excerpts were collected while preparing the deferred full-game plan. They use the existing GhidraMCP GUI session and canonical read-only binaries. No game session, decoder implementation or Ghidra annotation mutation was performed for this plan.

| artifact | purpose |
|---|---|
| `dispatch-tables.json` | PE-read TITLE label/engine records and ENG1/ENG3 0x18-byte level descriptors |
| `map-archive-entries.json` | twenty gameplay BIN/CIF rows; canonical archive, normalized name hashes and payload SHA-256 checked |
| `ENG1.DLL-1001f750.c`, `ENG1.DLL-1001f9e0.c` | header/chunk loaders use the level byte to select the ENG1 descriptor path |
| `ENG3.DLL-1001a930.c` | ENG3 map loader indexes its descriptor table by the level byte |
| `TITLE.DLL-10016300.c`, `TITLE.DLL-1000c3d0.c` | TITLE engine-pointer dispatch and host callback thunk |
| `ENG1.DLL-10020300.c`, `ENG3.DLL-1001abb0.c` | engine tick initialization reaches the corresponding map loader |
| `*-xrefs.json`, `*-table-xrefs.json`, `*-dispatch-xrefs.json` | bounded supporting Ghidra cross-reference queries |
| `open-programs.json` | existing GUI project/programs/image bases at inspection time |

Decompilation response JSON is retained beside the text excerpts. The EXE palette selector is already available at workspace `analysis/exports/p2_decomp/00405760.c`; it was read before the plan update. See [level-map.md](../level-map.md) and [levels.json](../levels.json) for module+RVA, hashes, archive identities and verification boundaries.

Static evidence verifies the ten descriptor/path/module mappings. It does not prove full progression or that every descriptor is live. ID 4/Nessus and Thebes substage relationships require later runtime confirmation. Existing TRAINING demo evidence is linked separately; all new gameplay scenarios remain planned.
