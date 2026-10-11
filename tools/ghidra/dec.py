"""Print the decompilation of one function: dec.py <program> <name|address> [--disasm]."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

prog, fn = sys.argv[1], sys.argv[2]
if "--disasm" in sys.argv:
    r = m.get("disassemble_function", prog, address=fn)
else:
    r = m.get("force_decompile", prog, function=fn)
if isinstance(r, dict):
    r = r.get("decompiled") or r.get("code") or r.get("decompilation") or r.get("disassembly") or r
print(r if isinstance(r, str) else __import__("json").dumps(r, indent=1))
