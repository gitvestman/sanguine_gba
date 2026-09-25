#!/usr/bin/env python3
"""Convert one of the source project's intro parallax tree-silhouette
layers (graphics/Intro/intro_skog_lager0N_960x540.png — transparent above
the treeline, solid black below) into a horizontally-tileable Butano
background: a 256x256 canvas (the smallest map size Butano supports),
transparent everywhere except a horizontal band holding the silhouette,
scaled to exactly 256px wide so it wraps around the screen every 256px as
the game scrolls its x position each frame — see RoomScreen::show_intro().

Not a perfect seamless tile (the source treeline pattern doesn't repeat),
but the on-screen window is only 240px, and the scroll during a few
intro pages is brief — a visible seam once every 256px of travel is an
acceptable trade for a real parallax effect on GBA hardware.

Usage:
    python3 tools/convert_intro_tree_layer.py <source.png> <output.bmp> <band_height>
"""
import argparse
import json
from pathlib import Path

from PIL import Image

CANVAS_SIZE = (256, 256)
CANVAS_WIDTH = 256


def convert(source_png: Path, output_bmp: Path, band_height: int) -> None:
    source = Image.open(source_png).convert("RGBA")

    # Crop a horizontal strip containing the treeline + enough solid ground
    # below it (the top ~40% of the source has both), then squash it to
    # exactly 256x<band_height> — a stretch, not a 1:1 crop, but these are
    # flat stylized silhouettes, so the distortion isn't noticeable.
    strip = source.crop((0, 0, source.width, round(source.height * 0.4)))
    strip = strip.resize((CANVAS_WIDTH, band_height), Image.LANCZOS)

    canvas = Image.new("RGBA", CANVAS_SIZE, (0, 0, 0, 0))
    band_top = (CANVAS_SIZE[1] - band_height) // 2
    canvas.alpha_composite(strip, (0, band_top))

    alpha = canvas.split()[3]
    transparent_mask = alpha.point(lambda a: 255 if a < 128 else 0)

    # Only two real colors needed (transparent sky, solid silhouette), but
    # build a full 16-entry table for a normal bpp_4 sprite/bg palette.
    palette = [(0, 0, 0)] * 16
    palette[1] = (20, 20, 28)

    indexed = Image.new("P", CANVAS_SIZE, 0)
    indexed.putpalette([component for color in palette for component in color])

    for y in range(CANVAS_SIZE[1]):
        for x in range(CANVAS_SIZE[0]):
            indexed.putpixel((x, y), 0 if transparent_mask.getpixel((x, y)) else 1)

    output_bmp.parent.mkdir(parents=True, exist_ok=True)
    indexed.save(output_bmp, "BMP")
    output_bmp.with_suffix(".json").write_text(
        json.dumps({"type": "regular_bg", "bpp_mode": "bpp_4"}, indent=4) + "\n")
    print(f"{source_png} -> {output_bmp}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("output_bmp", type=Path)
    parser.add_argument("band_height", type=int)
    args = parser.parse_args()
    convert(args.source_png, args.output_bmp, args.band_height)
