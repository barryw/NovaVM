#!/usr/bin/env python3
"""Generate LaTeX appendix pages with 16x16 glyph grids for each font.

Each font gets a full 16x16 chart (256 chars) on a single landscape-rotated
page, with glyphs large enough to clearly see individual pixels.
"""

import sys
import os

FONT_DIR = os.path.join(os.path.dirname(__file__), "../../e6502.Avalonia/Resources")
FONTS = [
    ("cp437.bin",         "CP437 (IBM/DOS)",
     "The default font (slot~0).  This is the standard IBM Code Page~437 character set "
     "used by the original IBM~PC and compatibles.  Characters \\$00--\\$1F contain "
     "symbols (smileys, card suits, arrows, musical notes) rather than control codes.  "
     "Characters \\$80--\\$FF include accented letters, line-drawing box characters, "
     "Greek letters, and mathematical symbols."),
    ("petscii_upper.bin", "PETSCII Uppercase",
     "The Commodore~64 character set~1 (slot~1, uppercase mode).  Standard ASCII letters "
     "appear in uppercase.  Shift+letter produces graphic symbols --- line segments, "
     "blocks, checkerboards, card suits, and geometric shapes --- in the \\$80--\\$9F "
     "and \\$A0--\\$BF ranges.  These are the iconic PETSCII graphic characters "
     "used in countless C64 programs and demos."),
    ("petscii_lower.bin", "PETSCII Lowercase",
     "The Commodore~64 character set~2 (slot~2, lowercase mode).  Unshifted letters produce "
     "lowercase; Shift+letter produces uppercase.  The graphic symbols from character "
     "set~1 remain accessible at the same code points.  This mode is selected by the "
     "C64's Shift+Commodore key combination."),
]

GLYPH_W = 8
GLYPH_H = 8
GRID_COLS = 16
GRID_ROWS = 16

# mm per font pixel — target ~9mm per glyph cell on an A4 landscape page
PIX = 0.90
CELL_PAD = 1.6  # mm padding around glyph within cell
CELL_W = GLYPH_W * PIX + CELL_PAD
CELL_H = GLYPH_H * PIX + CELL_PAD
HEADER_W = 5.0  # mm for row/col header labels


def rle_rows(font_data: bytes, char_code: int):
    """Yield (row_y, runs) where runs is list of (start_x, width) for set pixels."""
    glyph_offset = char_code * GLYPH_H
    for py in range(GLYPH_H):
        if glyph_offset + py >= len(font_data):
            continue
        row_byte = font_data[glyph_offset + py]
        if row_byte == 0:
            continue
        runs = []
        px = 0
        while px < GLYPH_W:
            if row_byte & (0x80 >> px):
                start = px
                while px < GLYPH_W and (row_byte & (0x80 >> px)):
                    px += 1
                runs.append((start, px - start))
            else:
                px += 1
        if runs:
            yield py, runs


def render_font(font_data: bytes, label: str, title: str, description: str) -> str:
    """Return LaTeX for one font appendix chapter with two half-page charts."""
    lines = []
    lines.append(f"% === {title} glyph chart (auto-generated) ===")
    lines.append(f"\\chapter{{Font Glyphs: {title}}}")
    lines.append(f"\\label{{sec:font-{label}}}")
    lines.append("")
    lines.append(description)
    lines.append("")
    lines.append("Each cell shows the glyph for one character code.  Rows are the high")
    lines.append("nibble; columns are the low nibble.")
    lines.append("For example, character \\$41 is in row~4, column~1.")
    lines.append("Select this font with \\texttt{FONT " + ("0" if "CP437" in title else "1" if "Upper" in title else "2") + "} or")
    lines.append("\\texttt{POKE \\$A007, " + ("0" if "CP437" in title else "1" if "Upper" in title else "2") + "}.")
    lines.append("")

    # Two halves: $00-$7F and $80-$FF
    for half in range(2):
        hi_label = f"\\${'0' if half == 0 else '8'}0--\\${'7' if half == 0 else 'F'}F"
        lines.append(f"\\subsection*{{Characters {hi_label}}}")
        lines.append("")
        lines.append("\\begin{center}")
        lines.append("\\begin{tikzpicture}[x=1mm, y=-1mm]")

        half_rows = 8  # 8 rows per half

        # Column headers
        for c in range(GRID_COLS):
            cx = HEADER_W + c * CELL_W + CELL_W / 2
            lines.append(f"  \\node[font=\\scriptsize\\ttfamily\\bfseries,retroblue] "
                         f"at ({cx:.2f},{CELL_H * 0.4:.2f}) {{\\${c:01X}}};")

        # Row headers + glyphs
        for r in range(half_rows):
            row_nibble = half * 8 + r
            ry = CELL_H + r * CELL_H  # first data row starts at y=CELL_H

            # Row header
            lines.append(f"  \\node[font=\\scriptsize\\ttfamily\\bfseries,retroblue] "
                         f"at ({HEADER_W * 0.5:.2f},{ry + CELL_H / 2:.2f}) "
                         f"{{\\${row_nibble:01X}x}};")

            for c in range(GRID_COLS):
                char_code = row_nibble * 16 + c
                cx = HEADER_W + c * CELL_W
                cy = ry

                # Cell border
                lines.append(f"  \\draw[retrogray!40] ({cx:.2f},{cy:.2f}) "
                             f"rectangle ({cx + CELL_W:.2f},{cy + CELL_H:.2f});")

                # Glyph pixels using RLE for efficiency
                gx0 = cx + CELL_PAD / 2
                gy0 = cy + CELL_PAD / 2

                for py, runs in rle_rows(font_data, char_code):
                    for sx, sw in runs:
                        x1 = gx0 + sx * PIX
                        y1 = gy0 + py * PIX
                        x2 = x1 + sw * PIX
                        y2 = y1 + PIX
                        lines.append(f"  \\fill[retroink] ({x1:.2f},{y1:.2f}) "
                                     f"rectangle ({x2:.2f},{y2:.2f});")

        lines.append("\\end{tikzpicture}")
        lines.append("\\end{center}")
        lines.append("")

    return "\n".join(lines)


def main():
    out_dir = os.path.join(os.path.dirname(__file__), "chapters")
    all_tex = []

    for fname, title, desc in FONTS:
        path = os.path.join(FONT_DIR, fname)
        if not os.path.exists(path):
            print(f"Warning: {path} not found, skipping", file=sys.stderr)
            continue
        data = open(path, "rb").read()
        label = fname.replace(".bin", "").replace("_", "-")
        tex = render_font(data, label, title, desc)
        all_tex.append(tex)
        print(f"Generated glyphs for {title} ({len(data)} bytes, {len(data)//8} chars)")

    out_path = os.path.join(out_dir, "appendix-f-font-glyphs.tex")
    header = ("% =============================================================================\n"
              "% Appendix F — Font Glyph Charts\n"
              "% Auto-generated by gen_font_appendix.py — do not edit by hand\n"
              "% =============================================================================\n\n")

    with open(out_path, "w") as f:
        f.write(header)
        f.write("\n\n".join(all_tex))
        f.write("\n")

    print(f"Wrote {out_path}")


if __name__ == "__main__":
    main()
