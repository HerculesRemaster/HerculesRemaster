"""Hand-made or externally generated art: picks up remastered/incoming/<pack>/<id with '/' -> '_'>.png
(newest one if several packs hold the entry; the old flat incoming/<id>.png still works).

Returns False (not ready, no attempt counted) until the file exists; then it is copied to the
entry's output path and checked like any generated image.
"""
import shutil

import common


def generate(entry, reference, out_path, scale, settings) -> bool:
    src = common.incoming_file(entry["id"])
    if src is None:
        return False
    shutil.copyfile(src, out_path)
    return True
