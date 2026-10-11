"""P2: EXE -> DLL interface table (EXE 0x4361b8, 250 dwords, magic 0x5ac00cac).

For each slot: EXE target, its size and callees, and per DLL the thunk that reads the slot
from the DLL's copy of the table plus how many places call that thunk.
Creates missing EXE functions at slot targets. Writes analysis/exports/p2_contract.json.
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
EXE = "HERCULES.EXE"
TABLE = 0x4361B8
SLOTS = 250
HEADER = 6  # magic, game state, heap base, heap size, 2 data pointers
DLL_COPY = {"ENG1.DLL": 0x10070F20, "ENG3.DLL": 0x100407A0, "TITLE.DLL": 0x1002BB40}


def funcs(prog: str, addrs: list[str], fields: str) -> dict:
    out = {}
    for i in range(0, len(addrs), 20):  # server max_functions=20
        r = m.get("get_functions", prog, functions=",".join(addrs[i:i + 20]), fields=fields)
        out.update(r.get("functions", {}))
    return out


def main() -> int:
    raw = bytes(m.get("read_memory", EXE, address=f"{TABLE:08x}", length=SLOTS * 4)["data"])
    vals = struct.unpack(f"<{SLOTS}I", raw)
    targets = sorted({v for v in vals[HEADER:]})
    created = 0
    info = funcs(EXE, [f"0x{t:08x}" for t in targets], "callees")
    for t in targets:
        f = info.get(f"0x{t:08x}")
        if not f or "error" in f or f.get("address") != f"{t:08x}":
            try:
                m.post("create_function", EXE, {"address": f"0x{t:08x}"})
                created += 1
            except m.GhidraError as e:
                print(f"create {t:08x}: {e}")
    if created:
        info = funcs(EXE, [f"0x{t:08x}" for t in targets], "callees")

    dll_refs: dict[str, dict[int, list]] = {}
    for dll, base in DLL_COPY.items():
        per = dll_refs.setdefault(dll, {})
        for s in range(SLOTS):
            refs = m.get("get_xrefs_to", dll, address=f"{base + s * 4:08x}", limit=50).get("references", [])
            if not refs:
                continue
            thunks = sorted({r.get("from_function", r["from_address"]) for r in refs})
            per[s] = []
            for th in thunks:
                n = None
                if not th.startswith("1"):  # function name -> count its callers
                    fi = funcs(dll, [th], "callers").get(th, {})
                    n = fi.get("caller_count")
                per[s].append({"from": th, "callers": n})

    slots = []
    for s, v in enumerate(vals):
        e = {"slot": s, "offset": f"0x{s * 4:03x}", "value": f"0x{v:08x}"}
        if s >= HEADER:
            f = info.get(f"0x{v:08x}", {})
            e["exe_name"] = f.get("name")
            e["size"] = f.get("size")
            e["callees"] = [c["name"] for c in f.get("callees", [])]
        e["dll"] = {d: dll_refs[d].get(s) for d in DLL_COPY if dll_refs[d].get(s)}
        slots.append(e)
    dst = ROOT / "analysis/exports/p2_contract.json"
    m.write_json(dst, {"table": f"0x{TABLE:08x}", "dll_copy": {k: f"0x{v:08x}" for k, v in DLL_COPY.items()},
                       "slots": slots})
    used = {d: len(dll_refs[d]) for d in DLL_COPY}
    print(f"slots={SLOTS} unique_targets={len(targets)} created_functions={created} used_slots={used} -> {dst}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
