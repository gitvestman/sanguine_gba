#!/usr/bin/env python3
"""Convert one of the Sanguine source project's horizontal flame-flicker
sprite sheets (torch_front_10x38px.png, candle_flame_6x9px.png — see
graphics/rooms/Torches_front.tres / Candle_Frames.tres, both looped
AnimatedSprite2D frame sequences) into a single Butano multi-frame sprite:
frames stacked vertically, one shared 15-color palette, each frame padded
into the smallest GBA sprite shape that fits it (GBA sprites only support a
fixed set of (width, height) pairs, and none of the source frame sizes are
one of them).

Usage:
    python3 tools/convert_flame_frames.py <source.png> <frame_w> <frame_h> \
        <count> <box_w> <box_h> <output.bmp>
"""
import argparse
import json
from pathlib import Path

from PIL import Image


def convert(source_png: Path, frame_w: int, frame_h: int, count: int,
            box_w: int, box_h: int, output_bmp: Path) -> None:
    sheet = Image.open(source_png).convert("RGBA")
    assert sheet.size == (frame_w * count, frame_h), (
        f"expected a {frame_w * count}x{frame_h} sheet, got {sheet.size}")

    frames = []
    offset = ((box_w - frame_w) // 2, (box_h - frame_h) // 2)

    for i in range(count):
        frame = sheet.crop((i * frame_w, 0, (i + 1) * frame_w, frame_h))
        canvas = Image.new("RGBA", (box_w, box_h), (0, 0, 0, 0))
        canvas.alpha_composite(frame, offset)
        frames.append(canvas)

    # Stack every frame into one tall image and quantize it once, so they
    # all index into the same shared palette (see convert_puzzle_tiles.py).
    combined = Image.new("RGBA", (box_w, box_h * count))

    for i, frame in enumerate(frames):
        combined.paste(frame, (0, i * box_h))

    alpha = combined.split()[3]
    transparent_mask = alpha.point(lambda a: 255 if a < 128 else 0)

    opaque_bg = Image.new("RGBA", combined.size, (0, 0, 0, 255))
    opaque_bg.alpha_composite(combined)
    rgb = opaque_bg.convert("RGB")

    quantized = rgb.quantize(colors=15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    palette = quantized.getpalette()[:15 * 3]
    full_palette = [(255, 0, 255)] + [
        tuple(palette[i:i + 3]) for i in range(0, len(palette), 3)
    ]
    full_palette += [(0, 0, 0)] * (16 - len(full_palette))

    indexed = Image.new("P", combined.size, 0)
    indexed.putpalette([component for color in full_palette for component in color])
    quantized_pixels = quantized.load()

    for y in range(combined.height):
        for x in range(combined.width):
            if transparent_mask.getpixel((x, y)):
                indexed.putpixel((x, y), 0)
            else:
                indexed.putpixel((x, y), quantized_pixels[x, y] + 1)

    output_bmp.parent.mkdir(parents=True, exist_ok=True)
    indexed.save(output_bmp, "BMP")
    output_bmp.with_suffix(".json").write_text(
        json.dumps({"type": "sprite", "bpp_mode": "bpp_4", "height": box_h}, indent=4) + "\n")
    print(f"{source_png} -> {output_bmp} ({box_w}x{box_h} x{count} frames)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_png", type=Path)
    parser.add_argument("frame_w", type=int)
    parser.add_argument("frame_h", type=int)
    parser.add_argument("count", type=int)
    parser.add_argument("box_w", type=int)
    parser.add_argument("box_h", type=int)
    parser.add_argument("output_bmp", type=Path)
    args = parser.parse_args()
    convert(args.source_png, args.frame_w, args.frame_h, args.count,
            args.box_w, args.box_h, args.output_bmp)
