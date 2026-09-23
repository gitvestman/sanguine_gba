#!/usr/bin/env python3
"""Generate the dice-roll sprite: a d20 seen face-on (a hexagon), dark red
with a bone-colored rim, drawn at 3 rotations for the tumble animation.
A hexagon repeats every 60 degrees, so frames at 0/20/40 degrees loop
seamlessly. Writes a 32x96 (3 frames of 32x32) 16-color BMP + JSON.

Usage:
    python3 tools/make_die_sprite.py assets/sprites/die.bmp
"""
import argparse
import json
import math
from pathlib import Path

from PIL import Image, ImageDraw

FRAME = 32
FRAMES = 3
CENTER = (FRAME - 1) / 2
OUTER_RADIUS = 15
INNER_RADIUS = 12

TRANSPARENT = 0
RIM = 1
FILL = 2
BEVEL = 3
PALETTE = [(255, 0, 255), (230, 220, 200), (110, 14, 14), (150, 34, 30)] + [(0, 0, 0)] * 12


def hexagon(radius: float, angle_degrees: float, y_offset: int) -> list[tuple[float, float]]:
    points = []
    for i in range(6):
        angle = math.radians(angle_degrees + 90 + i * 60)
        points.append((CENTER + radius * math.cos(angle), y_offset + CENTER + radius * math.sin(angle)))
    return points


def main(output_bmp: Path) -> None:
    image = Image.new("P", (FRAME, FRAME * FRAMES), TRANSPARENT)
    image.putpalette([component for color in PALETTE for component in color])
    draw = ImageDraw.Draw(image)

    for frame in range(FRAMES):
        angle = frame * 60 / FRAMES
        y_offset = frame * FRAME
        draw.polygon(hexagon(OUTER_RADIUS, angle, y_offset), fill=RIM)
        draw.polygon(hexagon(OUTER_RADIUS - 1.5, angle, y_offset), fill=BEVEL)
        draw.polygon(hexagon(INNER_RADIUS, angle, y_offset), fill=FILL)

    output_bmp.parent.mkdir(parents=True, exist_ok=True)
    image.save(output_bmp, "BMP")
    output_bmp.with_suffix(".json").write_text(
        json.dumps({"type": "sprite", "bpp_mode": "bpp_4", "height": FRAME}, indent=4) + "\n")
    print(f"wrote {output_bmp}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output_bmp", type=Path)
    main(parser.parse_args().output_bmp)
