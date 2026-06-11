#!/usr/bin/env python3
"""Build the Nova Chess title splash NVG from a source PNG."""

from __future__ import annotations

import argparse
import json
import struct
from collections import Counter, deque
from pathlib import Path

from PIL import Image


WIDTH = 320
HEIGHT = 200

C64_PALETTE = [
    (0, 0, 0),
    (255, 255, 255),
    (136, 0, 0),
    (170, 255, 238),
    (204, 68, 204),
    (0, 204, 85),
    (0, 0, 170),
    (238, 238, 119),
    (221, 136, 85),
    (102, 68, 0),
    (255, 119, 119),
    (51, 51, 51),
    (119, 119, 119),
    (170, 255, 102),
    (0, 136, 255),
    (187, 187, 187),
]

EGA_PALETTE = [
    (0, 0, 0),
    (0, 0, 170),
    (0, 170, 0),
    (0, 170, 170),
    (170, 0, 0),
    (170, 0, 170),
    (170, 85, 0),
    (170, 170, 170),
    (85, 85, 85),
    (85, 85, 255),
    (85, 255, 85),
    (85, 255, 255),
    (255, 85, 85),
    (255, 85, 255),
    (255, 255, 85),
    (255, 255, 255),
]

RESAMPLERS = {
    "nearest": Image.Resampling.NEAREST,
    "box": Image.Resampling.BOX,
    "bilinear": Image.Resampling.BILINEAR,
    "bicubic": Image.Resampling.BICUBIC,
    "lanczos": Image.Resampling.LANCZOS,
}


def weighted_distance(a: tuple[int, int, int], b: tuple[int, int, int]) -> int:
    dr = a[0] - b[0]
    dg = a[1] - b[1]
    db = a[2] - b[2]
    return 30 * dr * dr + 59 * dg * dg + 11 * db * db


def nearest_palette_index(
    rgb: tuple[int, int, int],
    palette: list[tuple[int, int, int]],
    forbidden_colors: set[int],
) -> int:
    candidates = [index for index in range(len(palette)) if index not in forbidden_colors]
    return min(candidates, key=lambda index: weighted_distance(rgb, palette[index]))


def scale_source(path: Path, resample: str) -> Image.Image:
    source = Image.open(path).convert("RGBA")
    background = Image.new("RGBA", source.size, (0, 0, 0, 255))
    background.alpha_composite(source)
    return background.convert("RGB").resize((WIDTH, HEIGHT), RESAMPLERS[resample])


def quantize(
    image: Image.Image,
    palette: list[tuple[int, int, int]],
    forbidden_colors: set[int],
) -> bytes:
    pixels = image.load()
    output = bytearray(WIDTH * HEIGHT)
    for y in range(HEIGHT):
        row = y * WIDTH
        for x in range(WIDTH):
            output[row + x] = nearest_palette_index(pixels[x, y], palette, forbidden_colors)
    return bytes(output)


def parse_color_family(value: str) -> set[int]:
    colors = {int(part, 0) & 0x0F for part in value.split(",") if part.strip()}
    if len(colors) < 2:
        raise argparse.ArgumentTypeError("color family must contain at least two palette indexes")
    return colors


def normalize_family_components(
    pixels: bytearray,
    families: list[set[int]],
    min_size: int,
    dominance: float,
) -> None:
    for family in families:
        visited = bytearray(WIDTH * HEIGHT)
        for start, color in enumerate(pixels):
            if color not in family or visited[start]:
                continue

            component: list[int] = []
            counts: Counter[int] = Counter()
            queue = deque([start])
            visited[start] = 1
            while queue:
                index = queue.popleft()
                component.append(index)
                counts[pixels[index]] += 1

                x = index % WIDTH
                y = index // WIDTH
                neighbors = []
                if x > 0:
                    neighbors.append(index - 1)
                if x < WIDTH - 1:
                    neighbors.append(index + 1)
                if y > 0:
                    neighbors.append(index - WIDTH)
                if y < HEIGHT - 1:
                    neighbors.append(index + WIDTH)

                for neighbor in neighbors:
                    if not visited[neighbor] and pixels[neighbor] in family:
                        visited[neighbor] = 1
                        queue.append(neighbor)

            dominant_color, dominant_count = counts.most_common(1)[0]
            if len(component) < min_size:
                continue
            if dominant_count / len(component) < dominance:
                continue
            for index in component:
                pixels[index] = dominant_color


def parse_rect(value: str) -> tuple[int, int, int, int, int]:
    parts = [int(part, 0) for part in value.split(",") if part.strip()]
    if len(parts) not in (4, 5):
        raise argparse.ArgumentTypeError("rect must be x,y,width,height[,color]")
    x, y, width, height = parts[:4]
    color = parts[4] if len(parts) == 5 else 0
    if width < 0 or height < 0:
        raise argparse.ArgumentTypeError("rect width and height must be non-negative")
    return x, y, width, height, color & 0x0F


def erase_rects(pixels: bytearray, rects: list[tuple[int, int, int, int, int]]) -> None:
    for x, y, width, height, color in rects:
        x0 = max(0, x)
        y0 = max(0, y)
        x1 = min(WIDTH, x + width)
        y1 = min(HEIGHT, y + height)
        for py in range(y0, y1):
            row = py * WIDTH
            for px in range(x0, x1):
                pixels[row + px] = color


def encode_nvg(pixels: bytes, palette: list[tuple[int, int, int]]) -> bytes:
    packed = bytearray()
    for y in range(HEIGHT):
        row = y * WIDTH
        for x in range(0, WIDTH, 2):
            left = pixels[row + x] & 0x0F
            right = pixels[row + x + 1] & 0x0F if x + 1 < WIDTH else 0
            packed.append((left << 4) | right)

    header_size = 16
    palette_bytes = 16 * 3
    payload_offset = header_size + palette_bytes

    data = bytearray(b"NVG2")
    data.extend(struct.pack("<HH", WIDTH, HEIGHT))
    data.append(0x02)  # embedded RGB palette
    data.append(0xFF)  # no transparent color key
    data.extend(struct.pack("<HI", payload_offset, len(packed)))
    for r, g, b in palette:
        data.extend((r & 0xFF, g & 0xFF, b & 0xFF))
    data.extend(packed)
    return bytes(data)


def encode_raw(pixels: bytes) -> bytes:
    return pixels


def write_preview(path: Path, pixels: bytes, palette: list[tuple[int, int, int]]) -> None:
    image = Image.new("RGB", (WIDTH, HEIGHT))
    image.putdata([palette[index] for index in pixels])
    path.parent.mkdir(parents=True, exist_ok=True)
    image.save(path)


def write_novadraw(path: Path, pixels: bytes, source_png: bytes) -> None:
    header = {
        "version": 1,
        "width": WIDTH,
        "height": HEIGHT,
        "backdropOpacity": 0.7,
        "backdropImageOpacity": 1.0,
        "backdropImageLength": len(source_png),
        "paintedPixelLength": WIDTH * HEIGHT,
        "imageCount": 1,
        "selectedImageIndex": 0,
        "imageNames": ["Scaled PNG"],
        "gridOn": True,
        "gridMajor": 16,
        "foregroundColor": 15,
        "backgroundColor": 0,
        "currentTool": "pencil",
        "previousTool": "pencil",
        "zoom": 3.0,
        "panOffsetX": 0.0,
        "panOffsetY": 0.0,
        "backdropPlacementMode": "fitToCanvas",
        "backdropScale": 1.0,
        "backdropOffsetX": 0.0,
        "backdropOffsetY": 0.0,
    }
    header_bytes = json.dumps(header, separators=(",", ":")).encode("utf-8")
    data = bytearray(b"NDRW1")
    data.extend(struct.pack("<I", len(header_bytes)))
    data.extend(header_bytes)
    data.extend(pixels)
    data.extend(b"\x01" * (WIDTH * HEIGHT))
    data.extend(source_png)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="Source PNG/JPEG artwork")
    parser.add_argument("output", type=Path, help="Output graphics file")
    parser.add_argument(
        "--format",
        choices=["nvg", "raw"],
        default="nvg",
        help="Output format. nvg emits native packed NVG2; raw emits a full 320x200 graphics-plane image.",
    )
    parser.add_argument(
        "--resample",
        choices=sorted(RESAMPLERS),
        default="nearest",
        help="Resize filter. nearest preserves pixel-art edges.",
    )
    parser.add_argument(
        "--palette",
        choices=["c64", "ega"],
        default="c64",
        help="Nova framebuffer palette to target.",
    )
    parser.add_argument("--preview", type=Path, help="Optional 320x200 PNG preview")
    parser.add_argument("--novadraw", type=Path, help="Optional generated NovaDraw project")
    parser.add_argument(
        "--forbid-color",
        action="append",
        default=[],
        type=lambda value: int(value, 0),
        help="Palette index to exclude while quantizing. May be repeated.",
    )
    parser.add_argument(
        "--normalize-family",
        action="append",
        default=[],
        type=parse_color_family,
        help="Comma-separated palette indexes to normalize per connected component.",
    )
    parser.add_argument(
        "--normalize-min-size",
        type=int,
        default=16,
        help="Minimum connected-component size for --normalize-family.",
    )
    parser.add_argument(
        "--normalize-dominance",
        type=float,
        default=0.75,
        help="Dominant color ratio required for --normalize-family.",
    )
    parser.add_argument(
        "--erase-rect",
        action="append",
        default=[],
        type=parse_rect,
        help="Erase/fill a rect after quantization: x,y,width,height[,color]. May be repeated.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    palette = C64_PALETTE if args.palette == "c64" else EGA_PALETTE
    forbidden_colors = {color & 0x0F for color in args.forbid_color}
    image = scale_source(args.source, args.resample)
    pixels = bytearray(quantize(image, palette, forbidden_colors))
    normalize_family_components(
        pixels,
        args.normalize_family,
        args.normalize_min_size,
        args.normalize_dominance,
    )
    erase_rects(pixels, args.erase_rect)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    encoded = encode_raw(bytes(pixels)) if args.format == "raw" else encode_nvg(bytes(pixels), palette)
    args.output.write_bytes(encoded)
    if args.preview:
        write_preview(args.preview, pixels, palette)
    if args.novadraw:
        write_novadraw(args.novadraw, pixels, args.source.read_bytes())


if __name__ == "__main__":
    main()
