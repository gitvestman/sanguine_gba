#!/usr/bin/env python3
"""Convert the Sanguine chapel-gate puzzle's 7 tile PNGs into 32x32 Butano
sprites (4bpp) that all share ONE palette, so the puzzle image reads as a
single picture instead of 7 independently-quantized fragments with mismatched
colors at the seams.

Usage:
    python3 tools/convert_puzzle_tiles.py <source_dir> <output_dir>
"""
import argparse
import json
from pathlib import Path

from PIL import Image

TILE_SIZE = (32, 32)
TILE_COUNT = 7


def convert(source_dir: Path, output_dir: Path) -> None:
    resized = []

    for i in range(1, TILE_COUNT + 1):
        tile = Image.open(source_dir / f"tile{i}.png").convert("RGB")
        resized.append(tile.resize(TILE_SIZE, Image.LANCZOS))

    # Stack all 7 resized tiles into one tall image and quantize it once, so
    # every tile's crop indexes into the same shared 16-color palette.
    combined = Image.new("RGB", (TILE_SIZE[0], TILE_SIZE[1] * TILE_COUNT))

    for i, tile in enumerate(resized):
        combined.paste(tile, (0, i * TILE_SIZE[1]))

    quantized = combined.quantize(colors=15, method=Image.Quantize.MEDIANCUT,
                                    dither=Image.Dither.NONE)

    # Reserve palette index 0 as unused: sprites always render index 0 as
    # hardware-transparent on the GBA, and every tile pixel here is opaque,
    # so index 0 must not be a real color or a chunk of the tile would
    # vanish. Shift every index up by one and pad the palette to 16 entries.
    palette = quantized.getpalette()[:15 * 3]
    full_palette = [(0, 0, 0)] + [
        tuple(palette[i:i + 3]) for i in range(0, len(palette), 3)
    ]
    full_palette += [(0, 0, 0)] * (16 - len(full_palette))

    shifted = Image.new("P", combined.size, 0)
    shifted.putpalette([component for color in full_palette for component in color])
    quantized_pixels = quantized.load()

    for y in range(combined.height):
        for x in range(combined.width):
            shifted.putpixel((x, y), quantized_pixels[x, y] + 1)

    output_dir.mkdir(parents=True, exist_ok=True)

    for i in range(TILE_COUNT):
        crop = shifted.crop((0, i * TILE_SIZE[1], TILE_SIZE[0], (i + 1) * TILE_SIZE[1]))
        bmp_path = output_dir / f"puzzle_tile{i + 1}.bmp"
        crop.save(bmp_path, "BMP")
        bmp_path.with_suffix(".json").write_text(
            json.dumps({"type": "sprite", "bpp_mode": "bpp_4"}, indent=4) + "\n")
        print(f"tile{i + 1}.png -> {bmp_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_dir", type=Path)
    parser.add_argument("output_dir", type=Path)
    args = parser.parse_args()
    convert(args.source_dir, args.output_dir)
