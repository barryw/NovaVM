#!/usr/bin/env python3
"""Convert a binary file into a C header with a `static const uint8_t`
array. Used to embed EhBASIC + extension ROMs into the NovaHost ESP32
firmware so the FPGA can be reflashed with fresh ROM at every boot
without re-synthesizing the bitstream.

Usage: bin2header.py <input.bin> <output.h> <symbol_name>
"""

import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 4:
        sys.stderr.write("usage: bin2header.py <input.bin> <output.h> <symbol>\n")
        return 2

    in_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2])
    symbol = sys.argv[3]

    data = in_path.read_bytes()

    lines = [
        f"// Auto-generated from {in_path.name} by tools/bin2header.py — do not edit.",
        f"// Size: {len(data)} bytes",
        "#pragma once",
        "",
        "#include <stdint.h>",
        "#include <stddef.h>",
        "",
        f"static const size_t {symbol}_LEN = {len(data)};",
        f"static const uint8_t {symbol}[{len(data)}] = {{",
    ]

    # 16 bytes per line for readability
    for offset in range(0, len(data), 16):
        chunk = data[offset:offset + 16]
        bytes_str = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"    {bytes_str},")

    lines.append("};")
    lines.append("")

    out_path.write_text("\n".join(lines))
    print(f"{in_path} → {out_path} ({len(data)} bytes as {symbol})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
