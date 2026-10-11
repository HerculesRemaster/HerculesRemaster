"""Add decompile hints (strings, imports/named callees, globals, size) to p2_contract.json slots.

Decompiles each unique EXE slot target once (cached in analysis/exports/p2_decomp/).
Prints one line per used slot to analysis/exports/p2_contract_hints.txt.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "analysis/exports/p2_contract.json"
CACHE = ROOT / "analysis/exports/p2_decomp"


def decomp(addr: str) -> str:
    p = CACHE / f"{addr}.c"
    if p.exists():
        return p.read_text(encoding="utf-8")
    r = m.get("force_decompile", "HERCULES.EXE", function=addr)
    text = r if isinstance(r, str) else (r.get("decompiled") or r.get("code") or json.dumps(r))
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(text, encoding="utf-8")
    return text


def hints(code: str) -> dict:
    lines = [l for l in code.splitlines() if l.strip()]
    sig = next((l for l in lines if "(" in l and not l.startswith("/*")), "")
    calls = sorted(set(re.findall(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", code)) - {"if", "while", "for", "switch", "return"})
    named = [c for c in calls if not c.startswith(("FUN_", "LAB_")) and c not in sig.split("(")[0]]
    return {
        "sig": sig.strip(),
        "lines": len(lines),
        "strings": sorted(set(re.findall(r"\bs_[A-Za-z0-9_]+", code)))[:6],
        "named_calls": named[:10],
        "fun_calls": [c for c in calls if c.startswith("FUN_")][:8],
        "globals": sorted(set(re.findall(r"\b(?:DAT|g)_[0-9A-Za-z_]+", code)))[:6],
    }


def main() -> int:
    d = json.loads(SRC.read_text())
    out = []
    for e in d["slots"][6:]:
        h = hints(decomp(e["value"][2:]))
        e["hints"] = h
        if e["dll"]:
            users = " ".join(f"{k.split('.')[0]}:{sum((x['callers'] or 0) for x in v)}" for k, v in e["dll"].items())
            out.append(f"{e['offset']} {e['value']} L{h['lines']} [{users}] {h['sig'][:60]} | "
                       f"{','.join(h['named_calls'])[:90]} | {','.join(h['strings'])[:80]} | {','.join(h['globals'])[:60]}")
    m.write_json(SRC, d)
    dst = ROOT / "analysis/exports/p2_contract_hints.txt"
    dst.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"used_slots={len(out)} -> {dst}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
