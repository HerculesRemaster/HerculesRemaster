"""Verify the TITLE password table in the running game (full-game S6.03).

password_check.py [--entries 0-35] [--seconds 45]

For each entry of analysis/exports/full-game/password_table.json this enters the four icons on the
password screen with the scripted pad and records what the game does:
  menu: DOWN to PASSWORDS (tick 300), CROSS (340); the screen opens with every slot at value 24 and
  the cursor on slot 1. Slot order 1, 2, 3, then LEFT x3 to slot 0. UP steps a slot one icon along its
  cycle (runtime, analysis/traces/runs/pswd2): slots 1-3 go up their value list, slot 0 goes down.
  Accept with START, then START twice more past the level card.
Watches: icon object frames, TITLE result 0x2bb3c. Outcome = the first level_load after accept
(level, difficulty, start) or none. Each run lands in analysis/traces/full-game/_passwords/<entry>/;
results go to reference/passwords.json (accepted entries only, with their observed initial state)
and a full report to analysis/exports/full-game/password_check.json.
"""
import argparse
import json
import os
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LISTS = {0: [8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 89], 1: [8, 16, 24, 32, 40, 48, 56, 65, 73, 81, 89],
         2: [8, 16, 24, 32, 40, 48, 56, 64, 72, 81, 89], 3: [8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 89]}
UP, DOWN, LEFT, RIGHT, START, CROSS = 0x10, 0x40, 0x80, 0x20, 0x08, 0x4000
GAP = 44


def ups(slot, value):
    i = LISTS[slot].index(value)
    return (2 - i) % 11 if slot == 0 else (i - 2) % 11


def pad_for(values):
    steps = [(300, DOWN), (340, CROSS)]
    t = 620                       # the screen takes input from about tick 562 (state 4)
    def press(b, gap=GAP):
        nonlocal t
        steps.append((t, b)); t += gap
    def set_slot(slot):                     # shorter way round: DOWN steps the other way
        n = ups(slot, values[slot])
        for _ in range(n if n <= 5 else 0):
            press(UP)
        for _ in range(11 - n if n > 5 else 0):
            press(DOWN)
    for slot in (1, 2, 3):
        set_slot(slot)
        if slot < 3:
            press(RIGHT, 40)
    for _ in range(3):
        press(LEFT, 40)
    set_slot(0)
    t += 30
    accept = t
    press(START, 120)
    press(START, 120)
    press(START, 120)
    return ",".join(f"{a}-{a + 2}:{b:04x}" for a, b in steps), accept


CHEAT_LEVELS = [1, 2, 3, 5, 6, 7, 8, 9, 10, 11]   # TITLE 0x22030: cheat menu index -> level id (gs+0x2e = 0)


def cheat_pad(level_index, lives=False, table=None):
    """In-game cheat level select (S6): password entry 36 (result 100) then 37 (101) sets gs+0x30 |= 0x80000000
    (TITLE HERO_PasswordCheck 0xcd30; results >= 100 keep the password screen, cursor stays on slot 0).
    TRIANGLE leaves the screen; the main menu (MOD_MainMenu 0x7510) then has a 5th item (index 4) that opens
    HERO_CheatMenuInput 0x16f0 (CHEAT.TGA): RIGHT = next level (CHEAT_LEVELS), UP = toggle gs+0x30 bit 8
    (no life lost, HERO_Die), START = leave with gs[0] = level, and the level loads without further input.
    Verified at runtime: analysis/traces/full-game/l02-heros-gauntlet/full-progression/20261012-073417-password-config."""
    if table is None:
        table = json.loads((ROOT / "analysis/exports/full-game/password_table.json").read_text())["entries"]
    va, vb = table[36]["slot_values"], table[37]["slot_values"]
    pad, _ = pad_for(va)
    steps = pad.split(",")
    t = int(steps[-1].split("-")[0]) + 200
    def press(b, gap=44):
        nonlocal t
        steps.append(f"{t}-{t + 2}:{b:04x}"); t += gap
    for slot in (0, 1, 2, 3):
        L = LISTS[slot]
        d = L.index(vb[slot]) - L.index(va[slot])
        n = (-d if slot == 0 else d) % 11
        for _ in range(n if n <= 5 else 0):
            press(UP)
        for _ in range(11 - n if n > 5 else 0):
            press(DOWN)
        if slot < 3:
            press(RIGHT, 40)
    t += 30
    press(START, 300)                 # 101
    press(0x1000, 200)                # TRIANGLE: leave the password screen
    for _ in range(5):
        press(DOWN, 40)
    t += 40
    press(CROSS, 200)                 # cheat menu
    for _ in range(level_index):
        press(RIGHT, 60)
    if lives:
        press(UP, 60)
    press(START, 200)                 # leave: the selected level loads by itself ~150 ticks later (runtime:
    return ",".join(steps)            # .../gauntlet-middle/20261012-085034), so no further presses


def events(path):
    for line in open(path, encoding="utf-8", errors="replace"):
        try:
            yield json.loads(line)
        except ValueError:
            pass


def running():
    r = subprocess.run(["powershell", "-NoProfile", "-Command", "[bool](Get-Process HERCULES -ErrorAction SilentlyContinue)"],
                       capture_output=True, text=True)
    return "True" in r.stdout


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--entries", default="0-35")
    ap.add_argument("--seconds", type=int, default=80)
    a = ap.parse_args(argv)
    lo, hi = (int(x) for x in a.entries.split("-"))
    table = json.loads((ROOT / "analysis/exports/full-game/password_table.json").read_text())["entries"]
    report_p = ROOT / "analysis/exports/full-game/password_check.json"
    report = json.loads(report_p.read_text()) if report_p.exists() else {}
    for e in table[lo:hi + 1]:
        vals = e["slot_values"]
        pad, accept = pad_for(vals)
        out = ROOT / "analysis/traces/full-game/_passwords" / f"{e['index']:02d}"
        out.mkdir(parents=True, exist_ok=True)
        if running():
            print("game busy; stopping"); break
        env = dict(os.environ, HERC_HOOK_PAD=pad, HERC_HOOK_DIFFICULTY="",
                   HERC_HOOK_WATCH="s0=title+0x2a278@0x34:2,s1=title+0x2a27c@0x34:2,s2=title+0x2a280@0x34:2,"
                                   "s3=title+0x2a274@0x34:2,result=title+0x2bb3c:4")
        subprocess.run(["pwsh", "-NoProfile", "-File", str(ROOT / "tools/trace/run_trace.ps1"), "-Seconds", str(a.seconds),
                        "-OutDir", str(out), "-Pairs", "0", "-Renderer", "gpu"], env=env, capture_output=True)
        ev = list(events(out / "resource-usage.jsonl"))
        base = {"s0": 0x1e3, "s1": 0x121, "s2": 0x182, "s3": 0xc0}
        final = {}
        for x in ev:
            if x["ev"] == "watch" and x["name"] in base and x["tick"] <= accept:
                final[x["name"]] = x["value"] - base[x["name"]]
        entered = [final.get(k) for k in ("s0", "s1", "s2", "s3")]
        result = [x["value"] for x in ev if x["ev"] == "watch" and x["name"] == "result" and x["tick"] >= accept - 5]
        load = next((x for x in ev if x["ev"] == "level_load" and x["tick"] > accept), None)
        r = {"index": e["index"], "slot_values": vals, "icons": [LISTS[s].index(v) for s, v in enumerate(vals)],
             "entered": entered, "entered_ok": entered == vals, "result_after_accept": result[:3],
             "table_result": e["result"], "table_difficulty": e["difficulty"],
             "loaded": {k: load.get(k) for k in ("tick", "path", "level", "start", "difficulty", "lives")} if load else None,
             "run": str(out.relative_to(ROOT)).replace("\\", "/")}
        report[str(e["index"])] = r
        print(e["index"], "entered", entered == vals, "result", result[:2], "->",
              (load["level"], load["difficulty"], load["start"]) if load else None)
        report_p.write_text(json.dumps(report, indent=1), encoding="utf-8")
    ok = [r for r in report.values() if r["entered_ok"] and r["loaded"]]
    pw = {"_doc": "Passwords accepted in this build (S6.03): icons = position 0..10 in each slot's value list "
                  "(slot 0: 8..80, 89; slot 1: 8..56, 65, 73, 81, 89; slot 2: 8..72, 81, 89; slot 3: 8..80, 89). "
                  "Entered and observed with tools/trace/password_check.py; difficulty 0 Beginner, 1 Medium, 2 Herculean.",
          "passwords": [{k: r[k] for k in ("index", "icons", "slot_values", "table_result", "table_difficulty", "loaded", "run")}
                        for r in sorted(ok, key=lambda r: r["index"])]}
    (ROOT / "reference").mkdir(exist_ok=True)
    (ROOT / "reference/passwords.json").write_text(json.dumps(pw, indent=1), encoding="utf-8")
    print(f"{len(ok)} accepted and loaded -> reference/passwords.json")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
