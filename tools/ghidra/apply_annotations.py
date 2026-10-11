"""Apply analysis/annotations/<phase>.json to the open Ghidra programs.

Format: {program: {"functions": {addr: [name, comment]}, "globals": {addr: [name, comment]}}}.
Functions missing at an address are created. Comments go to the plate comment.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]


def main(argv: list[str]) -> int:
    path = Path(argv[1]) if len(argv) > 1 else ROOT / "analysis/annotations/p2.json"
    ann = json.loads(path.read_text(encoding="utf-8"))
    ok = fail = 0
    for prog, a in ann.items():
        for addr, (name, comment) in a.get("functions", {}).items():
            try:
                try:
                    m.post("rename_function", prog, {"old_name": addr, "new_name": name, "strict_mode": "off"})
                except m.GhidraError:
                    m.post("create_function", prog, {"address": addr, "name": name})
                if comment:
                    m.post("set_comment", prog, {"address": addr, "comment": comment, "type": "plate"})
                ok += 1
            except m.GhidraError as e:
                fail += 1
                print(f"{prog} {addr} {name}: {e}")
        for addr, (name, comment) in a.get("globals", {}).items():
            try:
                m.post("rename_symbol", prog, {"target": addr, "new_name": name, "kind": "auto", "strict_mode": "off"})
                if comment:
                    m.post("set_comment", prog, {"address": addr, "comment": comment, "type": "repeatable"})
                ok += 1
            except m.GhidraError as e:
                fail += 1
                print(f"{prog} {addr} {name}: {e}")
        m.get("save_program", prog)
    print(f"applied={ok} failed={fail} from {path}")
    return 1 if fail else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
