#!/usr/bin/env python3
"""Convert one Sanguine item icon PNG (28x28 white silhouette on
transparency) into a Butano 32x32 sprite: a 16-color indexed BMP + JSON.

Usage:
    python3 tools/convert_item_icon.py <source.png> <output.bmp>
"""
import argparse
import json
from pathlib import Path

from PIL import Image

SPRITE_SIZE = (32, 32)
# Index 0 is the transparent color for sprites; it must not appear in the
# icon itself. Every icon gets this same 16-entry palette so they all share
# one hardware sprite palette.
PALETTE = [(255, 0, 255), (255, 255, 255)] + [(0, 0, 0)] * 14


def convert(source_png: Path, output_bmp: Path) -> None:
    icon = Image.open(source_png).convert("RGBA")
    alpha = icon.split()[3]

    indexed = Image.new("P", SPRITE_SIZE, 0)
    indexed.putpalette([component for color in PALETTE for component in color])

    offset = ((SPRITE_SIZE[0] - icon.width) // 2, (SPRITE_SIZE[1] - icon.height) // 2)
    for y in range(icon.height):
        for x in range(icon.width):
            if alpha.getpixel((x, y)) >= 128:
                indexed.putpixel((x + offset[0], y + offset[1]), 1)

    output_bmp.parent.mkdir(parents=True, exist_ok=True)
    indexed.save(output_bmp, "BMP")
    output_bmp.with_suffix(".json").write_text(
        json.dumps({"type": "sprite", "bpp_mode": "bpp_4"}, indent=4) + "\n")
    print(f"{source_png} -> {output_bmp}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("output_bmp", type=Path)
    args = parser.parse_args()
    convert(args.source_png, args.output_bmp)
