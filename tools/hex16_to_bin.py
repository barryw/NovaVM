#!/usr/bin/env python3
"""Convert a text file of 16-bit hex values (one per line) into a packed
little-endian binary. Used to turn rom/f6581_curve.hex (4096 × 16-bit
filter curve entries) into a flat binary NovaHost can stream to SDRAM.

Usage: hex16_to_bin.py <input.hex> <output.bin>
"""

import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        sys.stderr.write("usage: hex16_to_bin.py <input.hex> <output.bin>\n")
        return 2

    in_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2])

    out = bytearray()
    for line_no, raw in enumerate(in_path.read_text().splitlines(), 1):
        token = raw.strip()
        if not token or token.startswith("//"):
            continue
        try:
            value = int(token, 16)
        except ValueError:
            sys.stderr.write(f"{in_path}:{line_no}: not hex: {token!r}\n")
            return 1
        if value < 0 or value > 0xFFFF:
            sys.stderr.write(f"{in_path}:{line_no}: out of range: {value:#x}\n")
            return 1
        out.append(value & 0xFF)         # low byte first (little-endian)
        out.append((value >> 8) & 0xFF)  # then high byte

    out_path.write_bytes(bytes(out))
    print(f"{in_path} → {out_path} ({len(out)} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
