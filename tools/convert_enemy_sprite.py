#!/usr/bin/env python3
"""Convert one Sanguine enemy PNG into a Butano sprite (4bpp, 15 colors +
transparent), centered and padded/cropped into a fixed square size — shown
overlaid on the room background during Combat::fight, matching how the
source project overlays SpriteAnimations.*() enemy art over the room scene.

Usage:
    python3 tools/convert_enemy_sprite.py <source.png> <output.bmp> <size>
"""
import argparse
import json
from pathlib import Path

from PIL import Image


def convert(source_png: Path, output_bmp: Path, size: int) -> None:
    source = Image.open(source_png).convert("RGBA")

    # Downscale to fit within the target box if larger; never upscale (these
    # are small pixel-art creatures, not full-size portraits).
    scale = min(1.0, size / max(source.width, source.height))
    new_size = (max(1, round(source.width * scale)), max(1, round(source.height * scale)))
    resized = source.resize(new_size, Image.LANCZOS) if scale < 1.0 else source

    canvas = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    offset = ((size - resized.width) // 2, (size - resized.height) // 2)
    canvas.alpha_composite(resized, offset)

    alpha = canvas.split()[3]
    transparent_mask = alpha.point(lambda a: 255 if a < 128 else 0)

    opaque_bg = Image.new("RGBA", (size, size), (0, 0, 0, 255))
    opaque_bg.alpha_composite(canvas)
    rgb = opaque_bg.convert("RGB")

    quantized = rgb.quantize(colors=15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    palette = quantized.getpalette()[:15 * 3]
    full_palette = [(255, 0, 255)] + [
        tuple(palette[i:i + 3]) for i in range(0, len(palette), 3)
    ]
    full_palette += [(0, 0, 0)] * (16 - len(full_palette))

    indexed = Image.new("P", (size, size), 0)
    indexed.putpalette([component for color in full_palette for component in color])
    quantized_pixels = quantized.load()

    for y in range(size):
        for x in range(size):
            if transparent_mask.getpixel((x, y)):
                indexed.putpixel((x, y), 0)
            else:
                indexed.putpixel((x, y), quantized_pixels[x, y] + 1)

    output_bmp.parent.mkdir(parents=True, exist_ok=True)
    indexed.save(output_bmp, "BMP")
    output_bmp.with_suffix(".json").write_text(
        json.dumps({"type": "sprite", "bpp_mode": "bpp_4"}, indent=4) + "\n")
    print(f"{source_png} -> {output_bmp} ({size}x{size})")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("output_bmp", type=Path)
    parser.add_argument("size", type=int)
    args = parser.parse_args()
    convert(args.source_png, args.output_bmp, args.size)
