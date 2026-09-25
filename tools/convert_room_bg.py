#!/usr/bin/env python3
"""Convert one Sanguine room PNG (RGBA, 180x140) into a Butano regular_bg
asset: a 256x256 indexed BMP + companion JSON, centered on a solid border.

Usage:
    python3 tools/convert_room_bg.py <source.png> <output_dir> [--border R,G,B] [--bpp4]

--bpp4 quantizes to 15 colors (+ the border/transparent one) and tags the
output bpp_4 instead of the default bpp_8 (256 colors). Needed whenever more
than one background will be on screen at once (an 8bpp background claims the
*entire* shared 256-color BG palette bank by itself — see Phase 4g's title
screen/intro, which stacks the castle art under 3 parallax tree layers and
hit exactly this as a real "no more available colors" crash).
"""
import argparse
import json
from pathlib import Path

from PIL import Image

CANVAS_SIZE = (256, 256)


def convert(source_png: Path, output_dir: Path, border: tuple[int, int, int], bpp4: bool) -> None:
    room = Image.open(source_png).convert("RGBA")

    canvas = Image.new("RGB", CANVAS_SIZE, border)
    offset = ((CANVAS_SIZE[0] - room.width) // 2, (CANVAS_SIZE[1] - room.height) // 2)
    canvas.paste(room.convert("RGB"), offset, mask=room.split()[3])

    max_colors = 15 if bpp4 else 256

    # Pixel art: no dithering, so flat areas stay a single palette entry.
    indexed = canvas.quantize(colors=max_colors, method=Image.Quantize.MEDIANCUT,
                               dither=Image.Dither.NONE)

    if bpp4:
        # quantize() only guarantees <= max_colors, and a bpp_4 palette must
        # be exactly 16 entries — pad with the border color if it used fewer.
        palette = indexed.getpalette()
        colors = [tuple(palette[i:i + 3]) for i in range(0, len(palette), 3)]
        colors = (colors + [border] * 16)[:16]
        indexed.putpalette([component for color in colors for component in color])

    # GBA hardware always draws background palette index 0 as transparent
    # (showing the backdrop color, set to near-black in main.cpp). Put the
    # border color there so that slot is spent on padding rather than on a
    # real art color — otherwise whatever quantize() happens to put first
    # (e.g. the dungeon's orange eyes) silently never renders.
    border_index = indexed.getpixel((0, 0))
    if border_index != 0:
        order = list(range(len(indexed.getpalette()) // 3))
        order[0], order[border_index] = order[border_index], order[0]
        indexed = indexed.remap_palette(order)

    output_dir.mkdir(parents=True, exist_ok=True)
    stem = source_png.stem
    bmp_path = output_dir / f"{stem}.bmp"
    json_path = output_dir / f"{stem}.json"

    indexed.save(bmp_path, "BMP")
    tags = {"type": "regular_bg"}

    if bpp4:
        tags["bpp_mode"] = "bpp_4"

    json_path.write_text(json.dumps(tags, indent=4) + "\n")

    print(f"{source_png} -> {bmp_path} ({indexed.width}x{indexed.height}, "
          f"{len(indexed.getpalette()) // 3} palette slots)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("output_dir", type=Path)
    parser.add_argument("--border", default="0,0,0",
                         help="R,G,B fill color for the padded border (default black)")
    parser.add_argument("--bpp4", action="store_true",
                         help="Quantize to 15 colors and tag bpp_4 (see module docstring)")
    args = parser.parse_args()

    border_rgb = tuple(int(c) for c in args.border.split(","))
    convert(args.source_png, args.output_dir, border_rgb, args.bpp4)
