"""Pipeline test generator: the reference scaled by k with nearest neighbour. Not replacement art."""
from PIL import Image


def generate(entry, reference, out_path, scale, settings) -> bool:
    reference.resize((reference.width * scale, reference.height * scale), Image.NEAREST).save(out_path)
    return True
