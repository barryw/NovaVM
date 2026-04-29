#!/usr/bin/env python3
"""Convert a PNG logo into NovaVM boot-splash graphics assets.

The hardware boot splash uses a 320x200 4-bit VGC canvas. Output format
`NVG1` is a tiny row-span RLE:

    magic "NVG1"
    uint16_le width
    uint16_le height
    uint32_le span_count
    repeated spans:
        uint16_le linear_gfx_addr
        uint8     run_length
        uint8[]   non-zero palette indices

Black pixels are omitted; NovaHost clears the graphics plane before applying
the spans.
"""

from __future__ import annotations

import argparse
import colorsys
import struct
from pathlib import Path

from PIL import Image


NOVA_PALETTE = [
    (0x00, 0x00, 0x00),
    (0xFF, 0xFF, 0xFF),
    (0x88, 0x00, 0x00),
    (0xAA, 0xFF, 0xEE),
    (0xCC, 0x44, 0xCC),
    (0x00, 0xCC, 0x55),
    (0x00, 0x00, 0xAA),
    (0xEE, 0xEE, 0x77),
    (0xDD, 0x88, 0x55),
    (0x66, 0x44, 0x00),
    (0xFF, 0x77, 0x77),
    (0x33, 0x33, 0x33),
    (0x77, 0x77, 0x77),
    (0x88, 0xFF, 0x66),
    (0x00, 0x88, 0xFF),
    (0xBB, 0xBB, 0xBB),
]

GRAY_INDICES = (0, 11, 12, 15, 1)


def luma(rgb: tuple[int, int, int]) -> float:
    r, g, b = (channel / 255.0 for channel in rgb)
    return (0.2126 * r) + (0.7152 * g) + (0.0722 * b)


def nearest_palette_index(rgb: tuple[int, int, int]) -> int:
    """Map logo RGB to a Nova palette index.

    A straight RGB nearest-color pass turns the logo's saturated indigo into
    gray because the fixed C64-like palette has no true lavender. This mapper
    keeps low-saturation pixels on the grayscale ramp, but maps saturated pixels
    by hue so blue/purple/cyan logo art stays chromatic on real hardware.
    """
    r, g, b = rgb
    hue, sat, value = colorsys.rgb_to_hsv(r / 255.0, g / 255.0, b / 255.0)
    luminance = luma(rgb)

    # The source PNG is RGB with near-black compression/antialiasing pixels.
    # Treat those as transparent black so the boot clear remains clean.
    if value < 0.045 or luminance < 0.022:
        return 0

    if sat < 0.18:
        return min(
            GRAY_INDICES,
            key=lambda index: (
                abs(luma(NOVA_PALETTE[index]) - luminance)
                + (0.08 * abs((max(NOVA_PALETTE[index]) / 255.0) - value))
            ),
        )

    degrees = hue * 360.0

    if degrees < 20.0 or degrees >= 335.0:
        return 10 if value >= 0.50 else 2
    if degrees < 44.0:
        return 8 if value >= 0.45 else 9
    if degrees < 72.0:
        return 7
    if degrees < 145.0:
        return 13 if value >= 0.50 else 5
    if degrees < 195.0:
        return 3 if value >= 0.55 else 14
    if degrees < 230.0:
        return 14 if value >= 0.45 else 6
    if degrees < 270.0:
        return 14 if value >= 0.62 else 6
    if degrees < 320.0:
        return 4 if value >= 0.35 else 6
    return 10 if value >= 0.50 else 2


def convert(input_path: Path) -> list[int]:
    image = Image.open(input_path).convert("RGB")
    image = image.resize((320, 200), Image.Resampling.LANCZOS)
    pixels = image.get_flattened_data() if hasattr(image, "get_flattened_data") else image.getdata()
    return [nearest_palette_index(pixel) for pixel in pixels]


def write_raw(indices: list[int], output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(bytes(indices))


def write_preview(indices: list[int], output_path: Path) -> None:
    image = Image.new("RGB", (320, 200))
    image.putdata([NOVA_PALETTE[index] for index in indices])
    output_path.parent.mkdir(parents=True, exist_ok=True)
    image.save(output_path)


def write_rle(indices: list[int], output_path: Path) -> tuple[int, int]:
    spans: list[tuple[int, bytes]] = []
    for y in range(200):
        x = 0
        row = y * 320
        while x < 320:
            if indices[row + x] == 0:
                x += 1
                continue
            start = x
            data = bytearray()
            while x < 320 and indices[row + x] != 0:
                data.append(indices[row + x])
                x += 1
            spans.append((row + start, bytes(data)))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("wb") as f:
        f.write(b"NVG1")
        f.write(struct.pack("<HHI", 320, 200, len(spans)))
        for addr, data in spans:
            if len(data) > 255:
                raise ValueError(f"span too long at {addr}: {len(data)}")
            f.write(struct.pack("<HB", addr, len(data)))
            f.write(data)
    return len(spans), sum(len(data) for _, data in spans)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--raw", type=Path, help="optional raw 320x200 .gfx output")
    parser.add_argument("--preview", type=Path, help="optional RGB PNG preview")
    args = parser.parse_args()

    indices = convert(args.input)
    spans, nonzero = write_rle(indices, args.output)
    if args.raw:
        write_raw(indices, args.raw)
    if args.preview:
        write_preview(indices, args.preview)
    print(
        f"wrote {args.output} ({spans} spans, {nonzero} non-black pixels)"
    )


if __name__ == "__main__":
    main()
