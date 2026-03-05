#!/usr/bin/env python3
"""Generate novavm.inc (ca65 equates) from VgcConstants.cs and basic.sym."""

import re
import argparse
from pathlib import Path


def parse_vgc_constants(path: str) -> list[tuple[str, str, str]]:
    """Parse VgcConstants.cs, return list of (section, name, hex_value)."""
    results = []
    current_section = "General"

    const_re = re.compile(
        r'public\s+const\s+(?:int|byte|ushort)\s+(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;'
    )
    section_re = re.compile(r'//\s*[-=]{5,}')

    lines = Path(path).read_text().splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        # Detect section headers: a line of dashes, followed by a comment, followed by dashes
        if section_re.match(line.strip()) and i + 1 < len(lines):
            comment_line = lines[i + 1].strip()
            if comment_line.startswith('//'):
                current_section = comment_line.lstrip('/').strip()
                i += 1  # skip the comment line

        m = const_re.search(line)
        if m:
            name = m.group(1)
            val_str = m.group(2)
            val = int(val_str, 16) if val_str.startswith('0x') else int(val_str)
            hex_val = f"${val:04X}" if val > 0xFF else f"${val:02X}"
            results.append((current_section, name, hex_val))

        i += 1

    return results


# Curated list of ROM symbols safe for external use
ROM_EXPORT_LIST = {
    'V_INPT', 'V_OUTP', 'V_LOAD', 'V_SAVE',
    'LAB_COLD', 'LAB_WARM',
    'Smeml', 'Smemh',
    'Svarl', 'Svarh',
    'Earryl', 'Earryh',
    'Sstorl', 'Sstorh',
}


def parse_basic_sym(path: str) -> list[tuple[str, str]]:
    """Parse VICE label file, return curated (name, hex_value) pairs."""
    results = []
    sym_re = re.compile(r'^al\s+([0-9A-Fa-f]+)\s+\.(\S+)')

    for line in Path(path).read_text().splitlines():
        m = sym_re.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        name = m.group(2)
        if name in ROM_EXPORT_LIST:
            hex_val = f"${addr:04X}" if addr > 0xFF else f"${addr:02X}"
            results.append((name, hex_val))

    return results


def generate(vgc_path: str, sym_path: str | None, output_path: str):
    """Generate novavm.inc."""
    hw = parse_vgc_constants(vgc_path)
    rom = parse_basic_sym(sym_path) if sym_path else []

    max_name = max((len(n) for _, n, _ in hw), default=20)
    if rom:
        max_name = max(max_name, max(len(n) for n, _ in rom))

    out = []
    out.append("; novavm.inc -- auto-generated, DO NOT EDIT")
    src = Path(vgc_path).name
    if sym_path:
        src += f", {Path(sym_path).name}"
    out.append(f"; Sources: {src}")
    out.append("; Regenerate: python3 tools/gen_novavm_inc.py")
    out.append("")

    current_section = None
    for section, name, val in hw:
        if section != current_section:
            current_section = section
            out.append("")
            out.append(f"; --- {section} ---")
        out.append(f"{name:<{max_name}} = {val}")

    if rom:
        out.append("")
        out.append("; --- ROM Entry Points (from basic.sym) ---")
        for name, val in sorted(rom, key=lambda x: x[1]):
            out.append(f"{name:<{max_name}} = {val}")

    out.append("")
    Path(output_path).write_text("\n".join(out))
    print(f"Generated {output_path}: {len(hw)} hardware + {len(rom)} ROM symbols")


def main():
    p = argparse.ArgumentParser(description="Generate novavm.inc from VgcConstants.cs")
    p.add_argument("vgc_constants", help="Path to VgcConstants.cs")
    p.add_argument("basic_sym", nargs="?", help="Path to basic.sym (VICE label file)")
    p.add_argument("-o", "--output", default="novavm.inc", help="Output path")
    args = p.parse_args()
    generate(args.vgc_constants, args.basic_sym, args.output)


if __name__ == "__main__":
    main()
