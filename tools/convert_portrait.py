#!/usr/bin/env python3
"""Convert one Sanguine character portrait PNG into a 32x64 Butano sprite
(4bpp, 15 colors + transparent — the GBA has one shared 256-color OBJ
palette table, and an 8bpp sprite claims the whole thing for itself, leaving
none for the font/item icons/dice sprites also on screen; 4bpp keeps it to a
16-color bank like everything else).

Usage:
    python3 tools/convert_portrait.py <source.png> <output.bmp>
"""
import argparse
import json
from pathlib import Path

from PIL import Image

SPRITE_SIZE = (32, 64)


def convert(source_png: Path, output_bmp: Path) -> None:
    portrait = Image.open(source_png).convert("RGBA")

    # Center-crop to the sprite's 1:2 aspect ratio (narrower than the
    # source, which is roughly 64x87) before scaling down, so the character
    # isn't squeezed — better to lose a sliver off each side than distort
    # the face.
    target_aspect = SPRITE_SIZE[0] / SPRITE_SIZE[1]
    crop_width = round(portrait.height * target_aspect)
    left = (portrait.width - crop_width) // 2
    cropped = portrait.crop((left, 0, left + crop_width, portrait.height))
    resized = cropped.resize(SPRITE_SIZE, Image.LANCZOS)

    # The source portrait is vignetted (fully transparent corners, with a
    # soft alpha falloff at the edge — not a hard cutout), so a single exact
    # color match can't find "the transparent pixels": LANCZOS resampling
    # blends those semi-transparent edge pixels toward whatever backdrop
    # they're composited onto, which then shows up as a stray colored fringe
    # once that blended color survives quantization. Alpha threshold instead.
    alpha = resized.split()[3]
    transparent_mask = alpha.point(lambda a: 255 if a < 128 else 0)

    # Composite onto black (not magenta) before quantizing, so the fringe
    # blends toward a color already in the portrait's own dark background
    # instead of introducing a magenta-tinted cluster into the palette.
    canvas = Image.new("RGBA", SPRITE_SIZE, (0, 0, 0, 255))
    canvas.alpha_composite(resized)
    rgb = canvas.convert("RGB")

    quantized = rgb.quantize(colors=15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)

    # Reserve index 0 for the transparent color (magenta, matching the item
    # icon convention) and pad to a full 16-entry table — bpp_4 sprites need
    # exactly one 16-color palette bank, not however many quantize() actually
    # used.
    quantized_palette = quantized.getpalette()[:15 * 3]
    full_palette = [(255, 0, 255)] + [
        tuple(quantized_palette[i:i + 3]) for i in range(0, len(quantized_palette), 3)
    ]
    full_palette += [(0, 0, 0)] * (16 - len(full_palette))

    indexed = Image.new("P", SPRITE_SIZE, 0)
    indexed.putpalette([component for color in full_palette for component in color])

    quantized_pixels = quantized.load()
    for y in range(SPRITE_SIZE[1]):
        for x in range(SPRITE_SIZE[0]):
            if transparent_mask.getpixel((x, y)):
                indexed.putpixel((x, y), 0)
            else:
                indexed.putpixel((x, y), quantized_pixels[x, y] + 1)

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
