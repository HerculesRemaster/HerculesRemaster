"""Name the DLL wrappers that only call an EXE interface slot (S3.01).

name_slot_thunks.py <program> <table copy address> [--apply]

The DLLs copy the EXE interface table (EXE 0x4361b8) into their own data (ENG1 0x10070f20,
ENG3 0x100407a0, TITLE: see docs/module-interface.md) and call EXE services through it. A function whose
decompilation is one call through `DAT_<copy + slot>` is named after that slot's EXE function from the
slot table in docs/module-interface.md (`| 0x06c | 0x16b0 | `AUD_LoadLevelSonix(name)` | ...`), with
the suffix `_slot`. Only FUN_ functions are renamed. Prints the proposals; --apply renames and saves.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import mcp_http as m  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]


def slot_names():
    out = {}
    for line in (ROOT / "docs" / "module-interface.md").read_text(encoding="utf-8").splitlines():
        mm = re.match(r"\|\s*(0x[0-9a-f]{3})\s*\|\s*0x[0-9a-f]+\s*\|\s*`([A-Za-z_0-9]+)", line)
        if mm:
            out[int(mm.group(1), 16)] = mm.group(2)
    return out


def main(argv):
    prog, copy = argv[0], int(argv[1], 16)
    apply = "--apply" in argv
    names = slot_names()
    funcs = m.get_all("find_functions", prog, "functions")
    done = 0
    for f in funcs:
        if not f["name"].startswith("FUN_"):
            continue
        try:
            r = m.get("force_decompile", prog, function="0x" + f["address"])
        except m.GhidraError:
            continue
        text = r.get("decompiled") if isinstance(r, dict) else r
        if not isinstance(text, str):
            continue
        body = text[text.find("{") + 1:text.rfind("}")]
        calls = re.findall(r"\(\*(?:\([^)]*\))?DAT_([0-9a-f]{8})\)", body)
        stmts = [s for s in re.split(r";", body) if s.strip() and not re.match(r"\s*(undefined|int|uint|char|short|return\s*;)", s.strip())]
        if len(calls) != 1 or len(stmts) > 3:
            continue
        slot = int(calls[0], 16) - copy
        if slot < 0 or slot >= 0x3e8 or slot not in names:
            continue
        new = f"{names[slot]}_slot"
        print(f"{f['address']} {f['name']} -> {new} (slot 0x{slot:03x})")
        if apply:
            try:
                m.post("rename_function", prog, {"old_name": "0x" + f["address"], "new_name": new, "strict_mode": "off"})
                done += 1
            except m.GhidraError as ex:
                print("  failed", ex)
    if apply:
        m.get("save_program", prog)
        print(f"renamed {done}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
