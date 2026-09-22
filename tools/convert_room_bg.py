#!/usr/bin/env python3
"""Convert one Sanguine room PNG (RGBA, 180x140) into a Butano regular_bg
asset: a 256x256 indexed BMP + companion JSON, centered on a solid border.

Usage:
    python3 tools/convert_room_bg.py <source.png> <output_dir> [--border R,G,B]
"""
import argparse
import json
from pathlib import Path

from PIL import Image

CANVAS_SIZE = (256, 256)
MAX_COLORS = 256


def convert(source_png: Path, output_dir: Path, border: tuple[int, int, int]) -> None:
    room = Image.open(source_png).convert("RGBA")

    canvas = Image.new("RGB", CANVAS_SIZE, border)
    offset = ((CANVAS_SIZE[0] - room.width) // 2, (CANVAS_SIZE[1] - room.height) // 2)
    canvas.paste(room.convert("RGB"), offset, mask=room.split()[3])

    indexed = canvas.quantize(colors=MAX_COLORS, method=Image.Quantize.MEDIANCUT)

    output_dir.mkdir(parents=True, exist_ok=True)
    stem = source_png.stem
    bmp_path = output_dir / f"{stem}.bmp"
    json_path = output_dir / f"{stem}.json"

    indexed.save(bmp_path, "BMP")
    json_path.write_text(json.dumps({"type": "regular_bg"}, indent=4) + "\n")

    print(f"{source_png} -> {bmp_path} ({indexed.width}x{indexed.height}, "
          f"{len(indexed.getpalette()) // 3} palette slots)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("output_dir", type=Path)
    parser.add_argument("--border", default="0,0,0",
                         help="R,G,B fill color for the padded border (default black)")
    args = parser.parse_args()

    border_rgb = tuple(int(c) for c in args.border.split(","))
    convert(args.source_png, args.output_dir, border_rgb)
