# Disney's Hercules (1997) — Reverse Engineering with bethington/Ghidra-MCP

This guide uses [bethington/ghidra-mcp](https://github.com/bethington/ghidra-mcp), not `mrphrazer/ghidra-headless-mcp`.

The objective is to connect Ghidra + Ghidra MCP + Claude Code on Windows 11 to reverse engineer the original Disney's Hercules executable, recover its rendering and asset-loading architecture, and prepare a modern 4K remaster.

The repository currently documents 209 MCP tools, supporting disassembly, decompilation, cross-references, call graphs, P-code data-flow analysis, structure reconstruction, batch documentation, and optional live debugging.

## 1. Recommended architecture

Claude Code

AI orchestration, analysis, code generation

MCP / stdio

Python bridge — `bridge-mcp-ghidra`

MCP tools, discovery, routing, batch operations

HTTP localhost:8089

Ghidra 12.1.4 + GhidraMCP extension

Disassembler, decompiler, xrefs, P-code, data types

HERCULES.EXE + Ghidra project

Analysis database and original executable

Rendering engine

Asset formats

Game logic

Important distinction: This implementation uses a Ghidra extension with a local HTTP server, connected to Claude Code through a Python MCP bridge. It supports both GUI and headless operation.

For your project, I recommend beginning with Ghidra GUI mode, because it allows you to inspect disassembly, decompiled C, memory, symbols, and graphics-related structures visually while Claude Code performs automated analysis. Move to headless batch processing once the asset formats have been identified.

## 2. Install on Windows 11

### Step 1 — Prerequisites

| Component   | Requirement                                    |
| ----------- | ---------------------------------------------- |
| Ghidra      | 12.1.4 or compatible                           |
| Java        | JDK 21                                         |
| Python      | 3.10+; recommend 3.12                          |
| Maven       | 3.9+ for recommended setup                     |
| uv          | Recommended Python package/environment manager |
| Git         | To clone the repository                        |
| Claude Code | MCP-capable installation                       |

These match the project's documented prerequisites.

Official downloads: [Ghidra](https://github.com/NationalSecurityAgency/ghidra/releases) · [Java JDK](https://adoptium.net/) · [Maven](https://maven.apache.org/download.cgi) · [uv](https://docs.astral.sh/uv/).

Verify the tools in PowerShell:

```
java -version
py -3.12 --version
mvn -version
uv --version
git --version
```

### Step 2 — Organize your project

Assume these paths:

```
C:\Tools\
  ghidra_12.1.4_PUBLIC\
  ghidra-mcp\

C:\Games\GameHercules\
  HERCULES.EXE
  original\
  analysis\
  docs\
  extracted-assets\
  remaster\

```

Create a backup of the executable:

```
$GameRoot = "C:\Games\GameHercules"
$Exe = "$GameRoot\HERCULES.EXE"
$GhidraPath = "C:\Tools\ghidra_12.1.4_PUBLIC"

New-Item -ItemType Directory -Force `
  "$GameRoot\original", `
  "$GameRoot\analysis", `
  "$GameRoot\docs", `
  "$GameRoot\extracted-assets", `
  "$GameRoot\remaster"

Copy-Item $Exe "$GameRoot\original\HERCULES.EXE"

Get-FileHash $Exe -Algorithm SHA256
```

If the original game resides in `C:\Disney\Hercules`, adjust `$Exe` accordingly.

### Step 3 — Clone bethington/ghidra-mcp

\<CodeBlock language="powershell">New-Item -ItemType Directory -Force "C:\Tools"

cd C:\Tools

git clone [https://github.com/bethington/ghidra-mcp.git](https://github.com/bethington/ghidra-mcp.git)

cd C:\Tools\ghidra-mcp\</CodeBlock>

### Step 4 — Build and deploy the Ghidra extension

Use the repository's actual setup workflow:

\<CodeBlock language="powershell"># Verify environment py -3.12 -m tools.setup preflight \` --ghidra-path "C:\Tools\ghidra_12.1.4_PUBLIC"

# Install dependencies

py -3.12 -m tools.setup ensure-prereqs \` --ghidra-path "C:\Tools\ghidra_12.1.4_PUBLIC"

# Build GhidraMCP extension

py -3.12 -m tools.setup build

# Deploy extension and initialize Ghidra

py -3.12 -m tools.setup deploy \` --ghidra-path "C:\Tools\ghidra_12.1.4_PUBLIC"

The deployment script installs the extension into Ghidra's user profile, enables it, starts Ghidra as needed, and checks server health. It may close an already-running matching Ghidra instance, so save any existing Ghidra work first.

If you prefer not to use Maven, the repository also supports its included Gradle wrapper; see the build instructions in the README.

### Step 5 — Verify the MCP server

In Ghidra:

1. Open the Ghidra project window.
2. Go to File → Configure → Utility → Configure.
3. Ensure `GhidraMCPPlugin` is enabled.
4. Check Tools → GhidraMCP → Server Status.

The default HTTP address is `http://127.0.0.1:8089`.

Run:

```
Invoke-RestMethod `
  "http://127.0.0.1:8089/check_connection"

Invoke-RestMethod `
  "http://127.0.0.1:8089/mcp/health"
```

You should receive an `ok` status for the connectivity check. The plugin reports the currently selected program when one is available.

## 3. Connect bethington/Ghidra-MCP to Claude Code

This repository uses the executable `bridge-mcp-ghidra`, not the Python entry point from the other Headless MCP project.

First, find `uv.exe`:

```
where.exe uv
```

Create:

`C:\Games\GameHercules\.mcp.json`

```
{
  "mcpServers": {
    "ghidra": {
      "command": "C:\\Users\\YOUR_USER\\.local\\bin\\uv.exe",
      "args": [
        "run",
        "--directory",
        "C:\\Tools\\ghidra-mcp",
        "bridge-mcp-ghidra",
        "--transport",
        "stdio"
      ],
      "env": {
        "GHIDRA_MCP_URL": "http://127.0.0.1:8089"
      }
    }
  }
}
```

Replace `YOUR_USER` with the actual path returned by `where.exe uv`.

This follows the repository's documented MCP configuration. The bridge uses lazy tool loading by default, so you don't need to load all 209 tools into Claude Code's context immediately.

+1

Now launch Claude Code:

```
cd C:\Games\GameHercules
claude
```

Inside Claude Code, run:

```
/mcp
```

Confirm that `ghidra` is connected and approve the project MCP server if requested.

## 4. Import HERCULES.EXE into Ghidra

For the initial GUI-assisted workflow:

1. Launch Ghidra and select File → New Project → Non-Shared Project.
2. Store the project under `C:\Games\GameHercules\analysis`.
3. Name the project `Hercules_RE`.
4. Select File → Import File and choose `C:\Games\GameHercules\HERCULES.EXE`.
5. Open the imported executable in CodeBrowser.
6. Accept Auto Analyze and enable the relevant default analyzers.

Verify the processor language, binary format, image base, entry points, imported libraries, and memory sections. For a legacy Windows executable, 32-bit x86/PE is a likely outcome, but it must be confirmed from the actual file.

Now Claude Code can query and annotate the program through the MCP bridge.

## 5. The actual MCP tools for reverse engineering

The current 7.0.0 API consolidated several older tool names. In particular, `decompile_function` was replaced by `get_functions` using `fields="decompiled_code"`. This matters because many older tutorials will produce unknown-tool errors

Use this mapping:

| Task                       | bethington/Ghidra-MCP tool                    |
| -------------------------- | --------------------------------------------- |
| Binary metadata            | `get_metadata`                                |
| Entry points               | `get_entry_points`                            |
| Imports / memory sections  | `list_program_items`                          |
| Find functions             | `find_functions`                              |
| Disassemble function       | `disassemble_function`                        |
| Decompile C-like code      | `get_functions`                               |
| Incoming references        | `get_xrefs_to`                                |
| Outgoing references        | `get_xrefs_from`                              |
| Function call graph        | `get_function_call_graph`                     |
| Analyze control flow       | `analyze_control_flow`                        |
| Trace data flow            | `analyze_dataflow`                            |
| Inspect raw P-code         | `get_function_pcode`                          |
| Search strings             | `search_strings`                              |
| Find binary patterns       | `search_byte_patterns`                        |
| Recover structures         | `create_struct`, `analyze_struct_field_usage` |
| Rename recovered functions | `rename_function`                             |
| Document functions         | `apply_documentation`                         |

These methods are listed in the repository's current API reference.

### Step A — Discover the executable architecture

Ask Claude Code:

```
Using the connected ghidra MCP server:

1. Call get_metadata.
2. Call get_entry_points.
3. Call list_program_items with kind="imports".
4. Call list_program_items with kind="segments".
5. Call find_functions using pagination.
6. Call list_strings.

Identify:
- CPU architecture and executable format
- Entry point and image base
- Imported graphics and file I/O libraries
- Function count
- Memory sections
- Strings related to resources, assets, levels and rendering

Use actual MCP responses. Do not infer addresses.
```

You can also inspect the Ghidra HTTP server directly using PowerShell:

```
$Base = "http://127.0.0.1:8089"

# Program metadata
Invoke-RestMethod "$Base/get_metadata"

# Entry points
Invoke-RestMethod "$Base/get_entry_points"

# Imported functions and libraries
Invoke-RestMethod "$Base/list_program_items?kind=imports&limit=100"

# First 100 discovered functions
Invoke-RestMethod "$Base/find_functions?offset=0&limit=100"
```

For large binaries, always use pagination instead of retrieving all functions at once.

### Step B — Disassemble and decompile functions

After locating a real function address, ask Claude Code to invoke:

```
disassemble_function
get_functions

```

For decompilation, the documented interface is:

```
get_functions(
  function="<actual_function_address>",
  fields="decompiled_code,signature,callers,callees,xrefs"
)
```

Replace the placeholder with an actual Ghidra address.

You can use its HTTP API directly:

```
# Replace with an address actually returned by Ghidra
$Address = "0x00401000"

Invoke-RestMethod (
  "$Base/get_functions?function=$Address" +
  "&fields=decompiled_code,signature,callers,callees,xrefs"
)
```

This returns reconstructed C-like code together with selected function metadata.

Do not treat the decompiled output as the game's original source code. Function names, data types, variable boundaries, and some control-flow constructs may need manual correction.

### Step C — Analyze cross-references

Suppose Ghidra identifies a function importing `ReadFile`.

Your goal is to discover which game functions call it, then which functions call those functions.

```
1. Find ReadFile in the import inventory.
2. Recover its actual import/thunk address.
3. Invoke get_xrefs_to for that address.
4. Decompile the referencing functions.
5. Invoke get_function_call_graph.
6. Investigate their callers and callees.
7. Record confirmed file-reader functions.
```

Conceptually:

ReadFile / filesystem APIs

Resource file reader

Asset loading subsystem

Level or scene initialization

Illustrative call chain to investigate, not recovered Hercules code.

Use the same technique for graphics API imports. If Ghidra finds DirectDraw, GDI, or other graphics calls, trace their actual consumers.

### Step D — Recover data flow through P-code

This is especially useful when trying to understand proprietary sprite formats and pixel transformations.

The repository provides the tool:

`analyze_dataflow(address, variable, direction, max_steps)`

It supports forward and backward value propagation through the decompiler's P-code graph.

Example:

```
analyze_dataflow(
  address="<real_function_address>",
  variable="param_1",
  direction="forward",
  max_steps=50
)
```

This is a schematic invocation: choose a variable that actually appears in the selected function.

You should perform two passes.

Backward analysis: Start from a graphics buffer, output pointer, or memory write. Trace which inputs and transformations produced the data.

Forward analysis: Start from a loaded asset buffer and trace its movement through parsing, decompression, palette conversion, and rendering.

For a suspected image-decoding function, ask Claude:

```
Analyze the selected function's data flow.

Identify:
- Source buffer and length
- Destination buffer and capacity
- File offsets and header fields
- Loop termination conditions
- Pixel depth and stride calculations
- Palette indices or color conversion
- Compression or run-length decoding patterns
- Output width and height
- Callers and consumers of the result

Use get_function_pcode, analyze_dataflow,
get_xrefs_to, get_xrefs_from, and get_functions.

Report confirmed findings separately from hypotheses.
```

This is more productive than interpreting one decompiled function in isolation.

## 6. Recover the Hercules rendering system

For a 4K remaster, I would prioritize five investigations.

| Investigation          | What you need to recover                                  |
| ---------------------- | --------------------------------------------------------- |
| Display initialization | Screen dimensions, pixel format, display mode, backbuffer |
| Sprite renderer        | Source/destination coordinates, clipping, transparency    |
| Background renderer    | Tiles, layers, parallax and scrolling                     |
| Animation              | Frames, sequence tables, durations, origin points         |
| Camera                 | World-to-screen transformation, scrolling bounds          |

Start by searching the actual import inventory for graphics functions, such as:

- `DirectDrawCreate`, `DirectDrawCreateEx`
- `BitBlt`, `StretchBlt`
- `CreateDIBSection`, `SetDIBitsToDevice`
- `GetDC`, `ReleaseDC`

These are candidates, not confirmed Hercules imports.

When you find a renderer-related function, use:

```
get_functions
get_function_call_graph
get_xrefs_to
analyze_control_flow
get_function_pcode
analyze_dataflow
```

Focus on identifying the original mathematical behavior:

\\[ x\_{\text{screen}}=x\_{\text{world}}-x\_{\text{camera}} \\]

\\[ y\_{\text{screen}}=y\_{\text{world}}-y\_{\text{camera}} \\]

The actual Hercules implementation may use different conventions, offsets, fixed-point arithmetic, or scaling. Recover those from the executable before translating them.

For the remaster, I recommend maintaining original gameplay coordinates and implementing a separate resolution-independent presentation system. This avoids inadvertently changing collision detection, movement, and animation timing when rendering at 3840×2160.

## 7. Reconstruct the asset decoder

This should be the next milestone after renderer discovery.

Your available game assets may be stored as loose files, proprietary containers, compressed streams, or executable resources. We cannot assume their format before inspecting the installation and ISO contents.

The workflow should be:

1. Inventory assets: Record filenames, sizes, hashes, headers, and possible container signatures from the ISO and installed directory.
2. Trace resource loading: Find file I/O functions, then follow the references to index readers and decoders.
3. Recover data structures: Identify header layouts, offsets, compression fields, palettes, animation frame metadata, and alignment requirements.
4. Build a standalone extractor: Implement the verified format decoder in Python or Rust.
5. Validate outputs: Compare extracted image dimensions, colors, frame count, and reconstructed pixel data against runtime observations.

Use ImHex to inspect the container bytes and Procmon to determine which files the game actually reads. Ghidra MCP supplies the algorithms and data structures used by the engine.

Target asset-processing pipeline

Original game resources

Reverse-engineered asset decoder

PNG / metadata / animation sequences

4K asset regeneration and validation

Modern renderer and gameplsay compatibility

Preserve sprite pivots, collision masks, animation order, transparency, and palette metadata separately from the regenerated artwork. Those details are necessary for accurately reproducing the game's original behavior.

## 8. Optional: dynamic debugging

Unlike the earlier headless-only setup, bethington/Ghidra-MCP includes Ghidra TraceRmi debugger integration in GUI mode. For Windows PE executables, the documented backend is `dbgeng`.

Relevant tools include:

| Tool                      | Purpose                              |
| ------------------------- | ------------------------------------ |
| `debugger_launch_offers`  | Discover launch options              |
| `debugger_launch`         | Launch an executable under debugging |
| `debugger_set_breakpoint` | Set execution breakpoints            |
| `debugger_registers`      | Read processor registers             |
| `debugger_read_memory`    | Inspect runtime memory               |
| `debugger_step`           | Step into, over, or out              |
| `debugger_stack_trace`    | Inspect execution stack              |

To use them, open Ghidra's Window → Debugger and inspect the launch offers before starting the target.

For Hercules, I would use x32dbg alongside Ghidra whenever runtime graphics behavior or legacy Windows compatibility makes debugging inside Ghidra difficult.

Run the original executable only in a trusted, isolated environment. Keep script execution disabled unless needed; the MCP project's script endpoints are disabled by default for security.

## 9. Master prompt for Claude Code

Once Ghidra has imported `HERCULES.EXE`, use this prompt to orchestrate the full investigation.

```
# Disney's Hercules (1997) — Reverse Engineering

Role:
Senior Windows x86 reverse engineer, Ghidra analyst,
graphics engineer, and game asset-format researcher.

Repository:
https://github.com/bethington/ghidra-mcp

Target:
C:\Games\GameHercules\HERCULES.EXE

Project root:
C:\Games\GameHercules

Environment:
- Windows 11 x64
- Ghidra 12.1.4
- bethington/ghidra-mcp
- Claude Code

OBJECTIVE

Recover the original Hercules game architecture to
support a modern 4K remaster.

Prioritize:
1. Disassembly
2. Decompilation
3. Cross-references and call graphs
4. P-code data-flow analysis
5. Graphics rendering
6. Asset loading and decompression
7. Sprite and animation structures
8. Game loop and camera architecture

PHASE 1: ENVIRONMENT VALIDATION

- Check the active ghidra MCP connection.
- Verify that HERCULES.EXE is loaded.
- Call get_metadata and get_entry_points.
- Inspect imports, memory sections and strings.
- Enumerate functions with pagination.
- Check analysis_status.

Use search_tools and load_tool_group as needed.
Inspect the live MCP schemas before invoking tools.

PHASE 2: STATIC ANALYSIS

- Use disassemble_function.
- Use get_functions with fields=decompiled_code.
- Use get_xrefs_to and get_xrefs_from.
- Use get_function_call_graph.
- Use analyze_control_flow.
- Use get_function_pcode and analyze_dataflow.

Recover confirmed control flow and value propagation.
Record the actual addresses of identified functions.

PHASE 3: RENDERING ARCHITECTURE

Identify:
- Graphics APIs
- Display initialization
- Frame buffer management
- Sprite drawing
- Transparency and palettes
- Scaling and clipping
- Background layers
- Camera and scrolling
- Frame timing

Construct documented call graphs and function maps.

PHASE 4: ASSET FORMATS

Inventory all available original resource files.

Identify and reverse engineer:
- Resource index tables
- File and frame headers
- Offsets and sizes
- Compression algorithms
- Palette interpretation
- Sprite pixel formats
- Animation metadata
- Background and tile organization

Do not invent binary format specifications.

PHASE 5: DATA STRUCTURE RECOVERY

Infer, validate and document:
- Sprite descriptors
- Frame metadata
- Image and surface descriptors
- Level and camera state
- Resource handles
- Animation sequences

Use Ghidra type tools to refine recovered structures.
Preserve original offsets and calling conventions.

PHASE 6: DOCUMENTATION

Create:
docs/reverse-engineering/

Required documents:
- 00-binary-inventory.md
- 01-imports-and-entry-points.md
- 02-functions-registry.md
- 03-renderer-architecture.md
- 04-resource-loading.md
- 05-asset-format-specifications.md
- 06-data-structures.md
- 07-call-graphs.md
- 08-data-flow.md
- 09-remaster-integration-plan.md

For each recovered function document:
- Address
- Proposed name
- Calling convention
- Parameters and return values
- Evidence
- Callers and callees
- Relevant references
- Confidence level
- Outstanding questions

PHASE 7: EXTRACTION PLAN

Design a standalone Python or Rust extractor
based only on verified asset format specifications.

Plan validation against original game behavior.

CONSTRAINTS

- Never overwrite original binaries or assets.
- Keep Ghidra database modifications auditable.
- Do not patch the executable during discovery.
- Never fabricate function addresses or code.
- Verify findings across xrefs, decompilation and data flow.
- Distinguish fact from hypothesis.
- Avoid dumping the entire binary into model context.
- Save incremental analysis results to disk.

Begin with reconnaissance, then prioritize the
rendering and asset-loading subsystems.
```

## 10. Troubleshooting

| Problem                          | Solution                                                           |
| -------------------------------- | ------------------------------------------------------------------ |
| MCP connection fails             | Check `/check_connection`, Ghidra plugin status, and port 8089     |
| `spawn uv ENOENT`                | Use the absolute `uv.exe` path in `.mcp.json`                      |
| Tool unavailable                 | Call `search_tools` or `load_tool_group`                           |
| `decompile_function` unknown     | Use `get_functions` with `fields="decompiled_code"`                |
| Ghidra returns an internal error | Confirm the correct program is loaded and auto-analysis completed  |
| Extension not found              | Verify Ghidra version compatibility and the deployment path        |
| 209 tools overload context       | Keep default lazy loading enabled                                  |
| Wrong program selected           | Supply explicit program selectors when analyzing multiple binaries |

The repository recommends verifying actual tool availability from `http://127.0.0.1:8089/mcp/schema`; the running server is authoritative over examples or old documentation.&#x20;

### Recommended execution order

Your first deliverable should be a verified rendering and asset-loading architecture, not a replacement engine.

Once you know how Hercules decodes sprite frames, constructs graphics surfaces, and draws sprites, the rest of the remaster becomes much more systematic: extract original assets, regenerate them at high resolution, preserve gameplay metadata, and implement a modern rendering path.

For your Windows 11 setup, bethington/Ghidra-MCP in GUI mode is the appropriate starting point, with Claude Code managing the analysis and Ghidra providing the verifiable binary evidence.
