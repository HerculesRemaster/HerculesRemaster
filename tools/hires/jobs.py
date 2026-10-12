"""Resumable replacement-art jobs: pending -> generated -> review -> approved -> packed.

<sel> is a prompt pack name (remastered/prompts/<pack>, e.g. bird_dive) or a regex over manifest ids.

jobs.py take [<pack>] [--force] [--grow N] [--no-snap] [--no-align]
                                                          one step per downloaded sheet, always saved as
                                                          remastered/prompts/<pack>/<pack>.png (no pack =
                                                          every pack that has one): fit.py into
                                                          remastered/incoming/<pack>/, then add + run +
                                                          review; failed checks other than size only warn.
                                                          An unchanged sheet is skipped unless --force
jobs.py add <sel> [--generator NAME] [--settings JSON]    create/refresh jobs (default generator: manual)
jobs.py run [<sel>] [--limit N]                           generate pending jobs and run the checks
jobs.py review [<sel>]                                    generated -> review, writes side-by-side sheets
jobs.py approve <sel> | reject <sel> [--note TEXT]        review -> approved / back to pending
jobs.py pack [--allow-partial]                            approved -> packed (atlas.py)
jobs.py status [<sel>]

State: remastered/jobs.json. Job identity = sha256(source sha256 + entry id + generator + settings +
scale); a changed identity puts the job back to pending. A failed generation or failed check counts
an attempt; after MAX_ATTEMPTS the job is 'failed'. Generators live in tools/hires/adapters/<name>.py
and expose generate(entry, reference, out_path, scale, settings) -> True (written) / False (not ready).
"""
import argparse
import hashlib
import importlib
import json
import re
import sys
import time
from pathlib import Path

from PIL import Image

import checks
import common

MAX_ATTEMPTS = 3
sys.path.insert(0, str(Path(__file__).parent / "adapters"))


def load() -> dict:
    if common.JOBS.exists():
        return json.loads(common.JOBS.read_text(encoding="utf-8"))
    return {"version": 1, "jobs": {}}


def save(db: dict) -> None:
    common.JOBS.parent.mkdir(parents=True, exist_ok=True)
    tmp = common.JOBS.with_suffix(".tmp")
    tmp.write_text(json.dumps(db, indent=1), encoding="utf-8")
    tmp.replace(common.JOBS)                       # atomic: a crash never leaves a half-written state


def identity(entry, generator, settings, scale) -> str:
    blob = json.dumps([entry["source"].get("sha256"), entry["id"], generator, settings, scale], sort_keys=True)
    return hashlib.sha256(blob.encode()).hexdigest()


def log(job, state, note=""):
    job["history"].append([time.strftime("%Y-%m-%d %H:%M:%S"), state, note])
    job["state"] = state


def matcher(sel):
    """Pack name -> the entry ids in its layout; anything else -> regex search over the entry id."""
    if common.is_pack(sel):
        return {c["entry"] for c in common.layout(sel)["cells"]}.__contains__
    try:
        rx = re.compile(sel or ".*")
    except re.error as ex:
        raise SystemExit(f"{sel!r} is neither a pack in {common.PROMPTS} nor a valid regex ({ex}). "
                         'Use a pack name such as bird_dive, or e.g. "^sprite/0485/f00(1[7-9]|2[0-4])$"')
    return lambda i: bool(rx.search(i))


def select(db, pattern, states=None):
    m = matcher(pattern)
    return [j for j in db["jobs"].values() if m(j["entry"]) and (states is None or j["state"] in states)]


def new_job(entry, generator, settings, scale, note):
    j = {"entry": entry["id"], "identity": identity(entry, generator, settings, scale), "generator": generator,
         "settings": settings, "attempts": 0, "state": "pending", "checks": [], "history": []}
    log(j, "pending", note)
    return j


def cmd_add(db, a):
    settings = json.loads(a.settings or "{}")
    scale = common.manifest()["scale"]
    m = matcher(a.pattern)
    n = 0
    for e in common.manifest()["entries"]:
        if not m(e["id"]):
            continue
        j = db["jobs"].get(e["id"])
        if j and j["identity"] == identity(e, a.generator, settings, scale):
            continue
        db["jobs"][e["id"]] = new_job(e, a.generator, settings, scale, "added" if not j else "identity changed")
        n += 1
    save(db)
    print(f"{n} jobs added or reset")


def run_job(db, j, lenient=False) -> bool:
    """Generate + check one pending job; True when it reached 'generated'. lenient: only a size failure
    blocks (the art is judged by eye in review); other failed checks are kept as a warning."""
    e = common.entries_by_id()[j["entry"]]
    scale = common.manifest()["scale"]
    gen = importlib.import_module(j["generator"])
    out = common.output_path(e)
    out.parent.mkdir(parents=True, exist_ok=True)
    try:
        written = gen.generate(e, common.reference_image(e), out, scale, j["settings"])
    except Exception as ex:  # noqa: BLE001 - recorded as a failed attempt
        j["attempts"] += 1
        log(j, "failed" if j["attempts"] >= MAX_ATTEMPTS else "pending", f"generator error: {ex!r}")
        save(db)
        return False
    if not written:
        return False                               # e.g. manual art not delivered yet: no attempt counted
    res = checks.check(e, Image.open(out), scale)
    j["checks"] = [[n, bool(ok), d] for n, ok, d in res]
    bad = "; ".join(f"{n}: {d}" for n, ok, d in res if not ok)
    hard = [n for n, ok, _ in res if not ok and (not lenient or n == "size")]
    if hard:
        j["attempts"] += 1
        log(j, "failed" if j["attempts"] >= MAX_ATTEMPTS else "pending", bad)
    else:
        log(j, "generated", f"warning: {bad}" if bad else "")
    save(db)
    if bad:
        print(f"{'FAIL' if hard else 'warn'} {j['entry']}: {bad}")
    return not hard


def cmd_run(db, a):
    todo = select(db, a.pattern, {"pending"})[: a.limit or None]
    for j in todo:
        run_job(db, j)
    print(f"ran {len(todo)} jobs")


def review_job(j):
    e = common.entries_by_id()[j["entry"]]
    k = common.manifest()["scale"]
    sheet_dir = common.REMASTERED / "review"
    sheet_dir.mkdir(parents=True, exist_ok=True)
    ref = common.reference_image(e)
    out = Image.open(common.output_path(e)).convert("RGBA")
    big = ref.resize((ref.width * k, ref.height * k), Image.NEAREST)
    sheet = Image.new("RGBA", (big.width * 2 + 16, big.height), (40, 40, 48, 255))
    if any(not ok for _, ok, _ in j["checks"]):
        sheet.paste((200, 40, 40, 255), (big.width, 0, big.width + 16, big.height))   # red divider: check warning
    sheet.alpha_composite(big, (0, 0))
    sheet.alpha_composite(out, (big.width + 16, 0))
    sheet.save(sheet_dir / (j["entry"].replace("/", "_") + ".png"))
    log(j, "review", j["history"][-1][2])


def cmd_review(db, a):
    js = select(db, a.pattern, {"generated"})
    for j in js:
        review_job(j)
    save(db)
    print(f"{len(js)} jobs in review -> {common.REMASTERED / 'review'}")


def take_pack(db, pack, image, a) -> bool:
    """Fit one downloaded sheet and send the pack's frames to review. Skipped (False) when this exact
    sheet was already taken for every frame and none of them is waiting to be redone."""
    import fit
    sheet = hashlib.sha256(image.read_bytes()).hexdigest()
    ids = list(dict.fromkeys(c["entry"] for c in common.layout(pack)["cells"]))
    js = [db["jobs"].get(i) for i in ids]
    if not a.force and all(j and j.get("sheet") == sheet and j["state"] not in ("pending", "failed") for j in js):
        print(f"{pack}: {image.name} unchanged since the last take, skipped (--force to redo)")
        return False
    print(f"{pack}: taking {image}")
    fit.fit(pack, image, a.grow, a.no_snap, a.no_align)
    ents = common.entries_by_id()
    scale = common.manifest()["scale"]
    for i, j in zip(ids, js):
        if j and j["generator"] == "manual":
            j["attempts"] = 0
            log(j, "pending", f"take {pack}")
        else:
            j = db["jobs"][i] = new_job(ents[i], "manual", {}, scale, f"take {pack}")
        j["sheet"] = sheet
        if run_job(db, j, lenient=True):
            review_job(j)
    save(db)
    cmd_status(db, argparse.Namespace(pattern=pack))
    return True


def cmd_take(db, a):
    if a.pack:
        if not common.is_pack(a.pack):
            raise SystemExit(f"no pack {a.pack!r} in {common.PROMPTS}")
        image = common.download_path(a.pack)
        if not image.is_file():
            raise SystemExit(f"save the model's result as {image}")
        todo = [(a.pack, image)]
    else:                                          # every pack whose download is saved next to its prompt
        todo = sorted((p.parent.name, p) for p in common.PROMPTS.glob("*/*.png")
                      if p.stem == p.parent.name and common.is_pack(p.stem))
        if not todo:
            raise SystemExit(f"no downloads found: save each result as {common.PROMPTS}\\<pack>\\<pack>.png")
    taken = [pack for pack, image in todo if take_pack(db, pack, image, a)]
    if taken:
        print(f"sheets in {common.REMASTERED / 'review'} (red divider = check warning)\nnext: "
              + "; ".join(f"jobs.py approve {p}" for p in taken) + "   (or reject <pack> --note \"why\")")


def cmd_decide(db, a, approve):
    js = select(db, a.pattern, {"review"})
    for j in js:
        log(j, "approved" if approve else "pending", a.note or "")
        if not approve:
            j["attempts"] = 0
    save(db)
    print(f"{len(js)} jobs {'approved' if approve else 'rejected'}")


def cmd_pack(db, a):
    import atlas
    ids = [j["entry"] for j in db["jobs"].values() if j["state"] in ("approved", "packed")]
    r = atlas.pack(ids, allow_partial=a.allow_partial)
    for i in r["packed"]:
        if db["jobs"][i]["state"] != "packed":
            log(db["jobs"][i], "packed")
    save(db)
    print(json.dumps(r["report"], indent=1))
    if r["report"]["skipped"] and not a.allow_partial:
        print("skipped banks have frames not approved yet (a bank is packed only when complete); "
              "to see the approved frames in game now: jobs.py pack --allow-partial")


def cmd_status(db, a):
    from collections import Counter
    js = select(db, a.pattern)
    print(dict(Counter(j["state"] for j in js)))
    for j in js:
        if j["state"] == "failed":
            print("failed", j["entry"], j["history"][-1][2])


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("take"); p.add_argument("pack", nargs="?")
    p.add_argument("--force", action="store_true")
    p.add_argument("--grow", type=int, default=4); p.add_argument("--no-snap", action="store_true"); p.add_argument("--no-align", action="store_true")
    p = sub.add_parser("add"); p.add_argument("pattern"); p.add_argument("--generator", default="manual"); p.add_argument("--settings")
    p = sub.add_parser("run"); p.add_argument("pattern", nargs="?"); p.add_argument("--limit", type=int)
    p = sub.add_parser("review"); p.add_argument("pattern", nargs="?")
    for name in ("approve", "reject"):
        p = sub.add_parser(name); p.add_argument("pattern"); p.add_argument("--note")
    p = sub.add_parser("pack"); p.add_argument("--allow-partial", action="store_true")
    p = sub.add_parser("status"); p.add_argument("pattern", nargs="?")
    a = ap.parse_args(argv)
    db = load()
    {"take": cmd_take, "add": cmd_add, "run": cmd_run, "review": cmd_review, "pack": cmd_pack, "status": cmd_status,
     "approve": lambda d, x: cmd_decide(d, x, True), "reject": lambda d, x: cmd_decide(d, x, False)}[a.cmd](db, a)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
