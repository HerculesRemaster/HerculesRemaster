"""Export a per-program baseline from the running GhidraMCP server.

Writes analysis/exports/<program>/{metadata,imports,exports,entry_points,
memory_blocks,strings,functions}.json and prints one summary line per program.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
PROGRAMS = ["HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL"]


def export(program: str, out_root: Path) -> dict:
    out = out_root / program
    data = {
        "metadata": m.get("get_metadata", program),
        "imports": m.get_all("list_program_items", program, "items", kind="imports"),
        "exports": m.get_all("list_program_items", program, "items", kind="exports"),
        "entry_points": m.get("get_entry_points", program).get("entry_points", []),
        "memory_blocks": m.get_all("list_program_items", program, "items", kind="segments"),
        "strings": m.get_all("list_strings", program, "strings"),
        "functions": m.get_all("find_functions", program, "functions"),
    }
    for name, obj in data.items():
        m.write_json(out / f"{name}.json", obj)
    return {k: len(v) for k, v in data.items() if isinstance(v, list)}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("programs", nargs="*", default=PROGRAMS)
    ap.add_argument("--out", type=Path, default=ROOT / "analysis" / "exports")
    a = ap.parse_args(argv)
    for program in a.programs:
        try:
            counts = export(program, a.out)
        except m.GhidraError as e:
            print(f"{program}: error: {e}", file=sys.stderr)
            return 1
        print(f"{program}: " + " ".join(f"{k}={v}" for k, v in counts.items()) + f" -> {a.out / program}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
