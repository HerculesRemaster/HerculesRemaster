"""P2 anchors: string and import xrefs -> referencing functions.

Writes analysis/exports/p2_anchors.json and prints one line per anchor.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
PROGRAMS = ["HERCULES.EXE", "ENG1.DLL", "ENG3.DLL", "TITLE.DLL"]
STRINGS = ["hercules.fs", "not installed", "PC_DLLEngineMain", "LoadLevelSonix",
           "Demo Mode", ".dll", ".DLL", ".FS", ".fs"]
IMPORTS = ["DirectDrawCreate", "DirectSoundCreate", "mciSendStringA", "timeGetTime",
           "timeBeginPeriod", "LoadLibraryA", "GetProcAddress", "FreeLibrary",
           "joyGetPosEx", "joyGetPos", "GetAsyncKeyState", "CreateFileA", "ReadFile",
           "SetFilePointer", "_lopen", "_lread", "_llseek", "OpenFile"]


def xrefs(prog: str, addr: str) -> list[dict]:
    try:
        return m.get("get_xrefs_to", prog, address=addr, limit=200).get("references", [])
    except m.GhidraError as e:
        return [{"error": str(e)}]


def main() -> int:
    out: dict = {}
    for prog in PROGRAMS:
        p = out.setdefault(prog, {"strings": [], "imports": []})
        seen = set()
        for term in STRINGS:
            for s in m.get("search_strings", prog, search_term=term, limit=50).get("matches", []):
                if s["address"] in seen:
                    continue
                seen.add(s["address"])
                p["strings"].append({**s, "xrefs": xrefs(prog, s["address"])})
        imports = json.loads((ROOT / f"analysis/exports/{prog}/imports.json").read_text())
        for imp in imports:
            if imp["name"] in IMPORTS:
                p["imports"].append({**imp, "xrefs": xrefs(prog, imp["address"])})
    dst = ROOT / "analysis/exports/p2_anchors.json"
    m.write_json(dst, out)
    for prog, p in out.items():
        for kind in ("strings", "imports"):
            for a in p[kind]:
                funcs = sorted({x.get("from_function") or x.get("error", "?")[:40] for x in a["xrefs"]})
                label = a.get("value", a.get("name"))[:50].replace("\n", " ")
                print(f"{prog} {a['address']} {label!r}: {len(a['xrefs'])} -> {', '.join(funcs)[:200]}")
    print(f"-> {dst}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
