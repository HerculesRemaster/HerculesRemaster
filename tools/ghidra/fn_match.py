"""Carry function names between builds of the same source (ENG1 -> ENG3/TITLE) by code fingerprint.

fn_match.py <named program> <target program> [--out analysis/exports/full-game/fn_match_<target>.json] [--apply]

Every function of both programs is disassembled through ghidra-mcp (cached in
analysis/exports/full-game/disasm_<program>.json). A fingerprint is the mnemonic sequence with operands
normalised: registers kept, memory/branch targets and constants > 0xff replaced by placeholders, so
the same C code compiled at another address gives the same fingerprint. Functions whose fingerprint is
unique in both programs are matched; named source functions (not FUN_/thunk/CRT) give a proposed name.
--apply renames the target functions (strict_mode off) with a comment "matched <source>@<addr>" and saves.
Functions shorter than 8 instructions are ignored (too generic).
"""
import argparse
import hashlib
import json
import re
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "analysis" / "exports" / "full-game"
HEX = re.compile(r"0x([0-9a-fA-F]+)")


def norm(ins):
    if "mnemonic" in ins:
        mn, ops = ins["mnemonic"], ins.get("operands") or ""
    else:
        mn, _, ops = (ins.get("instruction") or "").partition(" ")
    def rep(mm):
        v = int(mm.group(1), 16)
        return mm.group(0) if v <= 0xff else "C"
    ops = HEX.sub(rep, ops)
    if mn.startswith("J") or mn == "CALL":
        ops = "T"
    return f"{mn} {ops}"


def disasm_all(prog):
    cache = OUT / f"disasm_{prog.replace('.', '_')}.json"
    if cache.exists():
        return json.loads(cache.read_text(encoding="utf-8"))
    funcs = m.get_all("find_functions", prog, "functions")
    out = {}
    for f in funcs:
        addr = f.get("address") or f.get("entry_point")
        if f.get("is_external") or f.get("is_thunk"):
            continue
        try:
            r = m.get("disassemble_function", prog, address="0x" + addr)
        except m.GhidraError:
            continue
        ins = r.get("instructions") if isinstance(r, dict) else None
        if not ins:
            continue
        out[addr] = {"name": f.get("name"), "ins": [norm(i) for i in ins]}
    OUT.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps(out), encoding="utf-8")
    return out


def fp(ins):
    return hashlib.sha1("\n".join(ins).encode()).hexdigest()


def named(n):
    return n and not n.startswith(("FUN_", "thunk_", "_", "Ordinal_", "entry")) and n not in ("PC_DLLEngineMain",)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("dst")
    ap.add_argument("--out", type=Path)
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args(argv)
    A, B = disasm_all(a.src), disasm_all(a.dst)
    fa = {k: fp(v["ins"]) for k, v in A.items() if len(v["ins"]) >= 8}
    fb = {k: fp(v["ins"]) for k, v in B.items() if len(v["ins"]) >= 8}
    ca, cb = Counter(fa.values()), Counter(fb.values())
    inv_a = {h: k for k, h in fa.items() if ca[h] == 1}
    matches = []
    for k, h in fb.items():
        if cb[h] == 1 and h in inv_a:
            sa = inv_a[h]
            matches.append({"target": k, "target_name": B[k]["name"], "source": sa, "source_name": A[sa]["name"],
                            "instructions": len(B[k]["ins"])})
    prop = [x for x in matches if named(x["source_name"]) and not named(x["target_name"])]
    out = a.out or OUT / f"fn_match_{a.dst.replace('.', '_')}.json"
    out.write_text(json.dumps({"source": a.src, "target": a.dst, "matched": len(matches), "proposed": prop}, indent=1), encoding="utf-8")
    print(f"{a.src} -> {a.dst}: {len(A)}/{len(B)} functions, {len(matches)} unique matches, {len(prop)} names to carry -> {out}")
    if a.apply:
        ok = 0
        for x in prop:
            try:
                m.post("rename_function", a.dst, {"old_name": "0x" + x["target"], "new_name": x["source_name"], "strict_mode": "off"})
                m.post("set_comment", a.dst, {"address": "0x" + x["target"],
                                              "comment": f"fn_match: same code as {a.src} {x['source_name']} @0x{x['source']}", "type": "plate"})
                ok += 1
            except m.GhidraError as ex:
                print("rename failed", x["target"], ex)
        m.get("save_program", a.dst)
        print(f"applied {ok}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
