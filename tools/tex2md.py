#!/usr/bin/env python3
"""
tex2md.py — Convert NovaBASIC LaTeX documentation to Markdown with YAML frontmatter.

Usage:
    python tools/tex2md.py

Reads all .tex chapter files from docs/manual/chapters/ and writes:
  - Guide files to docs/help/guides/
  - Individual command/function reference files to docs/help/reference/commands/ and functions/
"""

import os
import re
import sys

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEX_DIR = os.path.join(REPO_ROOT, "docs", "manual", "chapters")
HELP_DIR = os.path.join(REPO_ROOT, "docs", "help")
GUIDES_DIR = os.path.join(HELP_DIR, "guides")
CMD_DIR = os.path.join(HELP_DIR, "reference", "commands")
FN_DIR = os.path.join(HELP_DIR, "reference", "functions")

# Chapter mapping: tex filename -> (output filename, category)
CHAPTER_MAP = {
    "00-foreword.tex": ("foreword.md", "General"),
    "01-welcome.tex": ("welcome.md", "General"),
    "02-first-session.tex": ("first-session.md", "General"),
    "03-language-fundamentals.tex": ("language-fundamentals.md", "Language"),
    "04-graphics-and-sprites.tex": ("graphics-and-display.md", "Graphics"),
    "05-sound-and-music.tex": ("sound-and-music.md", "Sound"),
    "06-expanded-memory.tex": ("expansion-memory.md", "Memory"),
    "07-assembly-and-special-chips.tex": ("assembly.md", "Assembly"),
    "08-dma-and-blitter.tex": ("dma-and-blitter.md", "DMA"),
    "09-networking.tex": ("networking.md", "Networking"),
    "appendix-memory-map.tex": ("memory-map.md", "Reference"),
    "appendix-limits-and-errors.tex": ("limits-and-errors.md", "Reference"),
}

SECTION_CATEGORIES = {
    "Program Control": "Program Control",
    "Flow Control": "Flow Control",
    "Variables and Data": "Variables",
    "Input / Output": "I/O",
    "Memory": "Memory",
    "Bit Operations": "Bit Operations",
    "File Operations": "File I/O",
    "Text Display": "Display",
    "Graphics": "Graphics",
    "Sprites": "Sprites",
    "Sound and Music": "Sound",
    "Graphics File I/O": "File I/O",
    "Expansion Memory": "Memory",
    "DMA (Direct Memory Access)": "DMA",
    "Blitter (2D Rectangle Operations)": "DMA",
    "Network": "Networking",
    "Interrupts and Machine Code": "Assembly",
    "Miscellaneous": "Miscellaneous",
    "Numeric Functions": "Math",
    "String Functions": "String",
    "Sprite and Graphics Functions": "Sprites",
    "Music Functions": "Sound",
    "DMA and Blitter Functions": "DMA",
    "Network Functions": "Networking",
}

CATEGORY_GUIDES = {
    "Graphics": "graphics-and-display",
    "Sprites": "graphics-and-display",
    "Sound": "sound-and-music",
    "Memory": "expansion-memory",
    "DMA": "dma-and-blitter",
    "Networking": "networking",
    "Assembly": "assembly",
    "File I/O": "first-session",
    "Display": "language-fundamentals",
}

# ---------------------------------------------------------------------------
# Pre-processing: join multi-line table rows and epigraphs
# ---------------------------------------------------------------------------

def preprocess_tex(tex: str) -> str:
    """Join continuation lines inside longtable/tabular rows and handle
    multi-line epigraphs so the line-by-line converter sees complete entries."""
    lines = tex.split("\n")
    result = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Multi-line epigraph: collect everything from \epigraph to the
        # closing }% or } on a line by itself after the attribution
        if stripped.startswith("\\epigraph"):
            buf = stripped
            # Count braces to find the complete epigraph
            depth = 0
            for ch in buf:
                if ch == '{':
                    depth += 1
                elif ch == '}':
                    depth -= 1
            while depth > 0 and i + 1 < len(lines):
                i += 1
                next_line = lines[i].strip()
                buf += " " + next_line
                for ch in next_line:
                    if ch == '{':
                        depth += 1
                    elif ch == '}':
                        depth -= 1
            result.append(buf)
            i += 1
            continue

        # Inside a table environment, join continuation lines (lines that
        # don't start with \\ command and don't contain & but are part of a
        # multi-line cell). We detect this by: if we're inside a longtable
        # or tabular and a line contains & but doesn't end with \\, the
        # next lines are continuations until we see \\
        # IMPORTANT: only match real table separators (&), NOT escaped \&
        has_table_amp = ("&" in stripped and
                         re.search(r'(?<!\\)&', stripped) is not None)
        # Allow lines starting with table data (\$ for hex, \cmd{}, \texttt{}, etc.)
        # but skip LaTeX structural commands (\begin, \end, \toprule, etc.)
        starts_with_table_data = (
            stripped.startswith("\\$") or
            stripped.startswith("\\cmd{") or
            stripped.startswith("\\texttt{") or
            stripped.startswith("\\textbf{") or
            stripped.startswith("\\textit{")
        )
        starts_with_latex_cmd = (stripped.startswith("\\") and
                                 not starts_with_table_data)
        if (has_table_amp and
            not stripped.endswith("\\\\") and
            not re.search(r'\\\\\s*(\[[\d.]+(?:pt|ex)\])?\s*$', stripped) and
            not starts_with_latex_cmd and
            not stripped.startswith("%")):
            # Possible multi-line table row
            buf = stripped
            while i + 1 < len(lines):
                next_stripped = lines[i + 1].strip()
                if not next_stripped or next_stripped.startswith("%"):
                    i += 1
                    continue
                if (next_stripped.startswith("\\toprule") or
                    next_stripped.startswith("\\midrule") or
                    next_stripped.startswith("\\bottomrule") or
                    next_stripped.startswith("\\endhead") or
                    next_stripped.startswith("\\endfoot") or
                    next_stripped.startswith("\\end{") or
                    next_stripped.startswith("\\begin{") or
                    next_stripped.startswith("\\multicolumn") or
                    ("&" in next_stripped and not buf.rstrip().endswith("\\\\")
                     and re.search(r'\\\\\s*(\[[\d.]+(?:pt|ex)\])?\s*$', buf))):
                    break
                # This is a continuation line
                i += 1
                buf += " " + next_stripped
                if re.search(r'\\\\\s*(\[[\d.]+(?:pt|ex)\])?\s*$', buf):
                    break
            result.append(buf)
            i += 1
            continue

        result.append(line)
        i += 1

    return "\n".join(result)


# ---------------------------------------------------------------------------
# Inline LaTeX -> Markdown conversion
# ---------------------------------------------------------------------------

def _convert_latex_cmd(text: str, cmd: str, prefix: str, suffix: str) -> str:
    """Convert all occurrences of a LaTeX command like \\texttt{...} using brace-counting."""
    result = []
    i = 0
    search = cmd + "{"
    while i < len(text):
        idx = text.find(search, i)
        if idx == -1:
            result.append(text[i:])
            break
        result.append(text[i:idx])
        brace_pos = idx + len(cmd)
        content, end = _extract_brace_content(text, brace_pos)
        if content is None:
            result.append(text[idx:])
            break
        # For backtick-wrapped content, strip any inner \textit{}, \textbf{}
        if prefix == "`":
            content = re.sub(r'\\textit\{([^}]*)\}', r'\1', content)
            content = re.sub(r'\\textbf\{([^}]*)\}', r'\1', content)
        result.append(prefix + content + suffix)
        i = end
    return "".join(result)


def _convert_cmd_blocks(text: str) -> str:
    """Convert all \\cmd{...} blocks to `...`, handling nested braces."""
    result = []
    i = 0
    while i < len(text):
        # Look for \cmd{
        idx = text.find("\\cmd{", i)
        if idx == -1:
            result.append(text[i:])
            break
        result.append(text[i:idx])
        # Extract brace-balanced content
        content, end = _extract_brace_content(text, idx + 4)  # idx+4 points to '{'
        if content is None:
            result.append(text[idx:])
            break
        # Strip inner \textit{}, \textbf{} etc. from the content
        inner = re.sub(r'\\textit\{([^}]*)\}', r'\1', content)
        inner = re.sub(r'\\textbf\{([^}]*)\}', r'\1', inner)
        inner = inner.replace("~", " ")
        result.append("`" + inner + "`")
        i = end
    return "".join(result)


def _inline_format(text: str) -> str:
    """Convert inline LaTeX formatting to Markdown."""
    # Very first: convert escaped special chars inside braces so they
    # survive the extraction of \texttt{}, \cmd{}, etc.
    text = text.replace("\\$", "$")
    text = text.replace("\\#", "#")
    text = text.replace("\\&", "&")
    text = text.replace("\\_", "_")
    text = text.replace("\\^{}", "^")

    # First, resolve \textit{} and \textbf{} INSIDE \cmd{} by doing inner
    # formatting first, then wrapping with backticks.
    # Handle nested constructs: \cmd{COLOR \textit{fg},\textit{bg}}
    text = _convert_cmd_blocks(text)

    # \texttt{...} -> `...` (with brace-counting for nested commands)
    text = _convert_latex_cmd(text, "\\texttt", "`", "`")
    # \textbf{...} -> **...**
    text = _convert_latex_cmd(text, "\\textbf", "**", "**")
    # \textit{...} -> *...*
    text = _convert_latex_cmd(text, "\\textit", "*", "*")
    # \emph{...} -> *...*
    text = _convert_latex_cmd(text, "\\emph", "*", "*")
    # \verb|...| -> `...`
    text = re.sub(r'\\verb\|([^|]*)\|', r'`\1`', text)

    # Special characters
    text = text.replace("\\textasciitilde", "~")
    text = text.replace("\\textasciicircum", "^")
    text = text.replace("\\textbackslash", "\\")

    # Math mode
    text = re.sub(r'\$\\times\$', 'x', text)
    text = re.sub(r'\$\\approx\s*([\d.]+)\\ldots\$', r'~\1', text)
    text = re.sub(r'\$\\approx\s*([\d.]+)\$', r'~\1', text)
    text = re.sub(r'\$([^$]+)\$', lambda m: _convert_math(m.group(1)), text)

    # Thin space
    text = text.replace("\\,", "")
    # Non-breaking space
    text = text.replace("~", " ")
    # LaTeX quotes
    text = text.replace("``", '"')
    text = text.replace("''", '"')
    # Em dash
    text = text.replace("---", " -- ")
    # Ellipsis
    text = text.replace("\\ldots", "...")
    text = text.replace("\\enspace", " ")
    text = text.replace("\\normalfont", "")

    # Line break commands -> space
    text = re.sub(r'\\\\\[[\d.]+(?:pt|ex)\]', ' ', text)
    text = text.replace("\\\\", " ")

    # (Escaped special chars already handled at top of function)

    # Remove stray LaTeX formatting commands BEFORE brace stripping
    text = re.sub(r'\\color\{[^}]*\}', '', text)
    text = re.sub(r'\\(?:raggedright|arraybackslash|small|ttfamily|normalfont|itshape|par)\s*', '', text)

    # Remove leftover {} braces (but not inside backticks)
    # Do multiple passes for nested braces
    for _ in range(3):
        text = re.sub(r'(?<!\\)\{([^{}]*)\}', r'\1', text)

    # Clean up multiple spaces
    text = re.sub(r'  +', ' ', text)
    return text.strip()


def _convert_math(expr: str) -> str:
    """Convert simple LaTeX math expressions to plain text."""
    expr = expr.replace("\\times", "x")
    expr = expr.replace("\\approx", "~")
    expr = expr.replace("\\pi", "pi")
    expr = re.sub(r'\\(?:textit|mathrm|text)\{([^}]*)\}', r'\1', expr)
    expr = expr.replace("\\frac{1}{8}", "1/8")
    expr = re.sub(r'\\frac\{([^}]*)\}\{([^}]*)\}', r'\1/\2', expr)
    expr = expr.replace("\\pm", "+/-")
    expr = expr.replace("\\sim", "~")
    expr = expr.replace("\\leq", "<=")
    expr = expr.replace("\\geq", ">=")
    expr = expr.replace("\\neq", "!=")
    expr = expr.replace("\\ldots", "...")
    expr = expr.replace("\\rightarrow", "->")
    expr = expr.replace("\\longrightarrow", "->")
    expr = re.sub(r'e\^\{([^}]*)\}', r'e^(\1)', expr)
    expr = re.sub(r'2\^\{([^}]*)\}', r'2^(\1)', expr)
    expr = re.sub(r'\{([^}]*)\}', r'\1', expr)
    return expr.strip()


# ---------------------------------------------------------------------------
# Table parsing
# ---------------------------------------------------------------------------

def _parse_table_row(line: str) -> list:
    """Parse a LaTeX table row into cells."""
    # Remove trailing \\ and [Xpt]
    line = re.sub(r'\\\\\s*(\[[\d.]+(?:pt|ex)\])?\s*$', '', line)
    line = line.rstrip()
    if not line.strip() or line.strip() == "\\":
        return None
    # Split on unescaped & only (not \&)
    cells = re.split(r'(?<!\\)&', line)
    result = []
    for cell in cells:
        cell = _inline_format(cell.strip())
        result.append(cell)
    return result


def _format_table(rows: list) -> list:
    """Format table rows as Markdown pipe table."""
    if not rows:
        return []
    max_cols = max(len(r) for r in rows)
    for r in rows:
        while len(r) < max_cols:
            r.append("")
    lines = []
    header = rows[0]
    lines.append("| " + " | ".join(header) + " |")
    lines.append("| " + " | ".join(["---"] * max_cols) + " |")
    for row in rows[1:]:
        lines.append("| " + " | ".join(row) + " |")
    return lines


# ---------------------------------------------------------------------------
# Main LaTeX -> Markdown converter
# ---------------------------------------------------------------------------

def convert_tex_to_md(tex: str) -> str:
    """Convert LaTeX body text to Markdown."""
    # Pre-process to join multi-line constructs
    tex = preprocess_tex(tex)
    lines = tex.split("\n")
    out = []
    in_lstlisting = False
    in_itemize = False
    in_enumerate = False
    in_description = False
    in_retrobox = False
    in_tabular = False
    in_longtable = False
    in_callout = None
    in_tikz = False
    enum_counter = 0
    table_rows = []
    table_header_done = False

    for line in lines:
        stripped = line.strip()

        # Skip pure LaTeX comments
        if stripped.startswith("%"):
            continue

        # Skip common LaTeX-only commands
        if any(stripped.startswith(cmd) for cmd in [
            "\\label{", "\\addcontentsline", "\\markboth",
            "\\medskip", "\\bigskip", "\\noindent", "\\centering",
            "\\toprule", "\\midrule", "\\bottomrule", "\\endhead",
            "\\endfoot", "\\hline", "\\par",
        ]):
            continue

        # Skip tikzpicture
        if "\\begin{tikzpicture}" in stripped:
            in_tikz = True
            continue
        if "\\end{tikzpicture}" in stripped:
            in_tikz = False
            continue
        if in_tikz:
            continue

        # Tabular
        if "\\begin{tabular}" in stripped:
            in_tabular = True
            table_rows = []
            table_header_done = False
            continue
        if "\\end{tabular}" in stripped:
            in_tabular = False
            if table_rows:
                out.extend(_format_table(table_rows))
                out.append("")
            table_rows = []
            continue
        if in_tabular:
            if stripped in ("\\toprule", "\\midrule", "\\bottomrule", "\\hline"):
                if table_rows and not table_header_done:
                    table_header_done = True
                continue
            if stripped.startswith("\\") and "&" not in stripped and not stripped.startswith("\\$"):
                continue
            row = _parse_table_row(stripped)
            if row:
                table_rows.append(row)
            continue

        # Longtable
        if "\\begin{longtable}" in stripped:
            in_longtable = True
            table_rows = []
            table_header_done = False
            continue
        if "\\end{longtable}" in stripped:
            in_longtable = False
            if table_rows:
                out.extend(_format_table(table_rows))
                out.append("")
            table_rows = []
            continue
        if in_longtable:
            if stripped in ("\\toprule", "\\midrule", "\\bottomrule",
                            "\\endhead", "\\endfoot", "\\hline"):
                if table_rows and not table_header_done and stripped in ("\\midrule", "\\hline"):
                    table_header_done = True
                continue
            if "\\multicolumn" in stripped:
                continue
            if stripped.startswith("\\") and "&" not in stripped:
                continue
            row = _parse_table_row(stripped)
            if row:
                table_rows.append(row)
            continue

        # begin/end center
        if stripped in ("\\begin{center}", "\\end{center}"):
            continue

        # Code blocks
        if "\\begin{lstlisting}" in stripped:
            in_lstlisting = True
            out.append("```basic")
            continue
        if "\\end{lstlisting}" in stripped:
            in_lstlisting = False
            out.append("```")
            out.append("")
            continue
        if in_lstlisting:
            out.append(line.rstrip())
            continue

        # Retrobox
        if "\\begin{retrobox}" in stripped:
            in_retrobox = True
            out.append("```")
            continue
        if "\\end{retrobox}" in stripped:
            in_retrobox = False
            out.append("```")
            out.append("")
            continue
        if in_retrobox:
            rline = stripped
            # Remove line-break commands
            rline = re.sub(r'\\\\\s*(\[[\d.]+(?:pt|ex)\])?\s*', '', rline)
            rline = rline.replace("\\\\", "")
            # Strip \texttt{} and \textbf{} etc but don't add backtick/bold formatting
            rline = re.sub(r'\\cmd\{([^}]*)\}', r'\1', rline)
            rline = re.sub(r'\\texttt\{([^}]*)\}', r'\1', rline)
            rline = re.sub(r'\\textbf\{([^}]*)\}', r'\1', rline)
            rline = re.sub(r'\\textit\{([^}]*)\}', r'\1', rline)
            # Handle multi-line \textit{...} (opening brace here, close on later line)
            rline = re.sub(r'\\textit\{', '', rline)
            rline = re.sub(r'\\textbf\{', '', rline)
            rline = re.sub(r'\\texttt\{', '', rline)
            rline = rline.replace("\\$", "$")
            rline = rline.replace("\\_", "_")
            # LaTeX quotes
            rline = rline.replace("``", '"')
            rline = rline.replace("''", '"')
            # Em dash
            rline = rline.replace("---", " -- ")
            rline = re.sub(r'(?<!\\)\{([^{}]*)\}', r'\1', rline)
            rline = re.sub(r'(?<!\\)\{([^{}]*)\}', r'\1', rline)
            rline = rline.replace("\\longrightarrow", "->")
            # Math mode: strip $ delimiters around simple expressions
            rline = re.sub(r'\$([^$]+)\$', lambda m: _convert_math(m.group(1)), rline)
            rline = re.sub(r'\\(?:raggedright|arraybackslash|small|ttfamily|normalfont|centering)\s*', '', rline)
            # Strip any stray braces left from multi-line LaTeX constructs
            rline = rline.replace("{", "").replace("}", "")
            rline = re.sub(r'  +', ' ', rline)
            if rline.strip():
                out.append(rline.strip())
            continue

        # Callout environments
        callout_match = re.match(r"\\begin\{(notebox|warningbox|tipbox|tryitbox)\}", stripped)
        if callout_match:
            ctype = callout_match.group(1).replace("box", "")
            in_callout = ctype
            out.append(f"::: {ctype}")
            continue
        if re.match(r"\\end\{(notebox|warningbox|tipbox|tryitbox)\}", stripped):
            in_callout = None
            out.append(":::")
            out.append("")
            continue

        # Lists
        if "\\begin{itemize}" in stripped:
            in_itemize = True
            continue
        if "\\end{itemize}" in stripped:
            in_itemize = False
            out.append("")
            continue
        if "\\begin{enumerate}" in stripped:
            in_enumerate = True
            enum_counter = 0
            continue
        if "\\end{enumerate}" in stripped:
            in_enumerate = False
            out.append("")
            continue
        if "\\begin{description}" in stripped:
            in_description = True
            continue
        if "\\end{description}" in stripped:
            in_description = False
            out.append("")
            continue

        if (in_itemize or in_enumerate or in_description) and "\\item" in stripped:
            item_text = stripped
            if in_description:
                m = re.match(r"\\item\[(.*?)\]\s*(.*)", item_text)
                if m:
                    label = _inline_format(m.group(1))
                    body = _inline_format(m.group(2))
                    out.append(f"- **{label}** {body}")
                else:
                    item_text = re.sub(r"\\item\s*", "", item_text)
                    out.append(f"- {_inline_format(item_text)}")
            elif in_enumerate:
                enum_counter += 1
                item_text = re.sub(r"\\item\s*", "", item_text)
                out.append(f"{enum_counter}. {_inline_format(item_text)}")
            else:
                item_text = re.sub(r"\\item\s*", "", item_text)
                out.append(f"- {_inline_format(item_text)}")
            continue

        # Chapter headings (skip - title is in frontmatter)
        if stripped.startswith("\\chapter"):
            continue

        # Section headings (use brace-counting for nested \texttt{...})
        heading_matched = False
        for heading_cmd, heading_level in [
            ("\\subsubsection", "####"), ("\\subsection", "###"), ("\\section", "##"),
        ]:
            if stripped.startswith(heading_cmd):
                # Find the opening brace
                rest = stripped[len(heading_cmd):]
                if rest.startswith("*"):
                    rest = rest[1:]
                if rest.startswith("{"):
                    offset = len(stripped) - len(rest)
                    content, _ = _extract_brace_content(stripped, offset)
                    if content is not None:
                        out.append(f"{heading_level} {_inline_format(content)}")
                        out.append("")
                        heading_matched = True
                break
        if heading_matched:
            continue

        # Epigraph (pre-processed to single line)
        if stripped.startswith("\\epigraph"):
            ep = _parse_epigraph(stripped)
            if ep:
                out.append(ep)
                out.append("")
            continue

        # Skip }{  (stray epigraph continuation that shouldn't exist after preprocess)
        if stripped.startswith("}{") or stripped == "}":
            continue

        # Lines starting with { that are just LaTeX group wrappers
        if stripped.startswith("{\\raggedleft") or stripped.startswith("\\raggedleft"):
            # Extract date or text from foreword-style date lines
            # Remove \color{...} BEFORE stripping braces so the regex can match
            text = re.sub(r'\\color\{[^}]*\}', '', stripped)
            text = re.sub(r'[{}]', '', text)
            text = re.sub(r'\\(?:raggedleft|itshape|par)\s*', '', text).strip()
            if text:
                out.append(f"*{text}*")
                out.append("")
            continue

        # Skip formatting-only lines
        if stripped in ("", "\\raggedleft", "\\itshape"):
            out.append("")
            continue

        # Skip other begin/end environments we haven't handled
        if stripped.startswith("\\begin{") or stripped.startswith("\\end{"):
            continue

        # "Expected output:" bold text (don't double-wrap)
        if stripped.startswith("\\textbf{Expected"):
            out.append(_inline_format(stripped))
            out.append("")
            continue

        # Regular text
        text = _inline_format(stripped)
        if text.strip():
            out.append(text)

    return _clean_output("\n".join(out))


def _parse_epigraph(line: str) -> str:
    """Parse a pre-processed (single-line) epigraph."""
    # Strip all % characters first (LaTeX comment markers used as line joiners)
    cleaned = line.replace("%", " ")
    # Normalize whitespace
    cleaned = re.sub(r'\s+', ' ', cleaned).strip()

    # Match \epigraph{...}{...}
    # Use greedy matching with brace counting
    m = re.search(r'\\epigraph\{', cleaned)
    if not m:
        return ""

    start = m.end()
    # Find the quote content (first brace group)
    quote, pos = _extract_brace_content(cleaned, start - 1)
    if quote is None:
        return ""

    # Find the attribution (second brace group)
    # Skip whitespace between groups
    rest = cleaned[pos:].lstrip()
    if not rest.startswith("{"):
        return f"> {_clean_epigraph_text(quote)}"

    attr, _ = _extract_brace_content(rest, 0)
    if attr is None:
        return f"> {_clean_epigraph_text(quote)}"

    quote_text = _clean_epigraph_text(quote)
    attr_text = _clean_epigraph_text(attr)
    # Remove leading dashes
    attr_text = re.sub(r'^-+\s*', '', attr_text)

    if attr_text:
        return f'> {quote_text} -- {attr_text}'
    return f'> {quote_text}'


def _extract_brace_content(text: str, start: int):
    """Extract content between matched braces starting at text[start] == '{'."""
    if start >= len(text) or text[start] != '{':
        return None, start
    depth = 0
    i = start
    while i < len(text):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return text[start + 1:i], i + 1
        i += 1
    # Unmatched - return what we have
    return text[start + 1:], len(text)


def _clean_epigraph_text(text: str) -> str:
    """Clean up epigraph text."""
    text = re.sub(r'\\itshape\s*', '', text)
    text = text.replace("``", '"').replace("''", '"')
    text = _inline_format(text)
    # Remove stray quotes at start/end if they look like LaTeX quote artifacts
    text = text.strip('"').strip()
    return text


def _clean_output(text: str) -> str:
    """Clean up the converted markdown output."""
    text = re.sub(r'\n{3,}', '\n\n', text)
    lines = [l.rstrip() for l in text.split("\n")]
    return "\n".join(lines).strip() + "\n"


# ---------------------------------------------------------------------------
# Guide file generation
# ---------------------------------------------------------------------------

def generate_guide(tex_filename: str, md_filename: str, category: str):
    """Convert a chapter .tex file to a guide .md file."""
    tex_path = os.path.join(TEX_DIR, tex_filename)
    if not os.path.exists(tex_path):
        print(f"  SKIP {tex_filename} (not found)")
        return

    with open(tex_path, "r", encoding="utf-8") as f:
        tex = f.read()

    # Extract chapter title using brace-counting for nested commands
    m = re.search(r"\\chapter\*?\{", tex)
    if m:
        content, _ = _extract_brace_content(tex, m.end() - 1)
        title = _inline_format(content) if content else md_filename.replace(".md", "").replace("-", " ").title()
    else:
        title = md_filename.replace(".md", "").replace("-", " ").title()

    body = convert_tex_to_md(tex)

    fm = f'---\ntitle: "{title}"\ntype: guide\ncategory: {category}\n---\n'

    md_path = os.path.join(GUIDES_DIR, md_filename)
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(fm + "\n" + body)

    print(f"  GUIDE {md_filename} ({len(body)} bytes)")


# ---------------------------------------------------------------------------
# Command reference parsing and generation
# ---------------------------------------------------------------------------

def parse_command_reference():
    """Parse appendix-command-reference.tex into individual command/function entries."""
    tex_path = os.path.join(TEX_DIR, "appendix-command-reference.tex")
    if not os.path.exists(tex_path):
        print("  SKIP appendix-command-reference.tex (not found)")
        return

    with open(tex_path, "r", encoding="utf-8") as f:
        tex = f.read()

    # Pre-process to join multi-line rows
    tex = preprocess_tex(tex)

    commands = []
    functions = []
    current_section = None
    current_is_function = False
    in_longtable = False
    table_rows = []

    for line in tex.split("\n"):
        stripped = line.strip()

        m = re.match(r"\\subsection\*\{(.+?)\}", stripped)
        if m:
            if table_rows and current_section:
                entries = _extract_entries(table_rows, current_section, current_is_function)
                if current_is_function:
                    functions.extend(entries)
                else:
                    commands.extend(entries)
                table_rows = []
            current_section = m.group(1)
            current_is_function = "Function" in current_section
            continue

        m = re.match(r"\\section\{(.+?)\}", stripped)
        if m:
            section_title = m.group(1)
            if "Function" in section_title:
                current_is_function = True
            elif "Command" in section_title:
                current_is_function = False
            continue

        if "\\begin{longtable}" in stripped:
            in_longtable = True
            table_rows = []
            continue
        if "\\end{longtable}" in stripped:
            in_longtable = False
            if table_rows and current_section:
                entries = _extract_entries(table_rows, current_section, current_is_function)
                if current_is_function:
                    functions.extend(entries)
                else:
                    commands.extend(entries)
            table_rows = []
            continue

        if in_longtable:
            if stripped in ("\\toprule", "\\midrule", "\\bottomrule",
                            "\\endhead", "\\endfoot", "\\hline"):
                continue
            if stripped.startswith("\\normalfont"):
                continue
            row = _parse_table_row(stripped)
            if row and len(row) >= 2:
                table_rows.append(row)

    return commands, functions


def _extract_entries(rows: list, section: str, is_function: bool) -> list:
    """Extract individual command/function entries from table rows."""
    entries = []
    category = SECTION_CATEGORIES.get(section, "Miscellaneous")

    for row in rows:
        if len(row) < 2:
            continue
        syntax = row[0].strip()
        description = row[1].strip()
        if not syntax or not description:
            continue
        if syntax in ("**Syntax**", "**Function**", "**Command**", "Syntax", "Function", "**Purpose**", "**Returns**"):
            continue

        name = _extract_name(syntax, is_function)
        if not name:
            continue

        entries.append({
            "name": name,
            "syntax": syntax,
            "description": description,
            "category": category,
            "is_function": is_function,
            "section": section,
        })

    return entries


def _extract_name(syntax: str, is_function: bool) -> str:
    """Extract the canonical command/function name from syntax string."""
    s = syntax.replace("`", "").strip()

    if s.startswith("MUSIC "):
        parts = s.split()
        if len(parts) >= 2 and parts[1] in ("PLAY", "STOP", "TEMPO", "LOOP", "PRIORITY"):
            return f"MUSIC_{parts[1]}"
        return "MUSIC"

    if s.startswith("COPPER "):
        parts = s.split()
        if len(parts) >= 2 and parts[1] in ("ADD", "CLEAR", "ON", "OFF", "LIST", "USE"):
            return f"COPPER_{parts[1]}"
        return "COPPER"

    if s.startswith("SPRITE ") and "DATA" not in s and "SHAPE" not in s and "SET" not in s:
        return "SPRITE"

    if s.startswith("LOOP"):
        return "LOOP"

    # Functions: NAME(...) or NAME$(...) -> NAME or NAME$
    m = re.match(r'([A-Z_]+\$?)\s*[\[(]', s)
    if m:
        return m.group(1).replace("$", "_STR")

    # Commands: first word
    m = re.match(r'([A-Z_]+\$?)', s)
    if m:
        return m.group(1).replace("$", "_STR")

    return ""


def generate_command_files(commands: list):
    """Generate individual command reference files."""
    seen = {}
    for cmd in commands:
        name = cmd["name"]
        if name not in seen:
            seen[name] = cmd
        else:
            existing = seen[name]
            if cmd["description"] not in existing["description"]:
                existing["description"] += " " + cmd["description"]
    for name, cmd in seen.items():
        _write_ref_file(cmd, is_function=False)


def generate_function_files(functions: list):
    """Generate individual function reference files."""
    seen = {}
    for fn in functions:
        name = fn["name"]
        if name not in seen:
            seen[name] = fn
        else:
            existing = seen[name]
            if fn["description"] not in existing["description"]:
                existing["description"] += " " + fn["description"]
    for name, fn in seen.items():
        _write_ref_file(fn, is_function=True)


def _write_ref_file(entry: dict, is_function: bool):
    """Write a single command or function reference file."""
    name = entry["name"]
    category = entry["category"]
    syntax = entry["syntax"]
    description = entry["description"]

    fname = name.lower().replace("_str", "_s").replace("$", "_s")
    fname = fname.replace(" ", "_")
    ref_type = "function" if is_function else "command"
    out_dir = FN_DIR if is_function else CMD_DIR

    keywords = [name.lower()]
    if category.lower() not in keywords:
        keywords.append(category.lower())

    see_also = []
    guide = CATEGORY_GUIDES.get(category)
    if guide:
        see_also.append(guide)

    kw_str = ", ".join(keywords)
    sa_str = ", ".join(see_also) if see_also else ""

    fm_lines = [
        "---",
        f'title: "{name}"',
        f"type: {ref_type}",
        f"category: {category}",
        f"keywords: [{kw_str}]",
        f'syntax: "{_escape_yaml(syntax)}"',
    ]
    if sa_str:
        fm_lines.append(f"see_also: [{sa_str}]")
    fm_lines.append("---")

    body = description
    if guide:
        guide_title = guide.replace('-', ' ').title()
        body += f"\n\nSee the [{guide_title}](/docs/help/guides/{guide}.md) guide for more details."

    content = "\n".join(fm_lines) + "\n\n" + body + "\n"

    out_path = os.path.join(out_dir, fname + ".md")
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(content)


def _escape_yaml(s: str) -> str:
    """Escape string for YAML double-quoted value."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    for d in [GUIDES_DIR, CMD_DIR, FN_DIR]:
        os.makedirs(d, exist_ok=True)

    print("Converting chapter files to guides...")
    for tex_file, (md_file, category) in CHAPTER_MAP.items():
        generate_guide(tex_file, md_file, category)

    print("\nParsing command reference...")
    result = parse_command_reference()
    if result:
        commands, functions = result
        print(f"  Found {len(commands)} commands and {len(functions)} functions")

        print("\nGenerating command reference files...")
        generate_command_files(commands)
        print(f"  Wrote command files to {CMD_DIR}")

        print("\nGenerating function reference files...")
        generate_function_files(functions)
        print(f"  Wrote function files to {FN_DIR}")

    print("\nDone.")


if __name__ == "__main__":
    main()
