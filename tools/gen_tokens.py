#!/usr/bin/env python3
"""Extract EhBASIC token tables from basic.asm and emit tokens.json."""

import re
import json
import argparse
from pathlib import Path


def resolve_equates(lines: list[str]) -> dict[str, int]:
    """Parse TK_ and XTK_ equate chains, resolving forward references."""
    equates: dict[str, str] = {}  # name -> raw RHS expression
    eq_re = re.compile(r'^(TK_\w+|XTK_\w+|TKX_PREFIX)\s*=\s*(.+?)(?:\s*;.*)?$')

    for line in lines:
        stripped = line.strip()
        m = eq_re.match(stripped)
        if m:
            equates[m.group(1)] = m.group(2).strip()

    # Resolve values
    resolved: dict[str, int] = {}

    def resolve(name: str) -> int:
        if name in resolved:
            return resolved[name]
        expr = equates[name]
        # Try direct hex
        if expr.startswith('$'):
            val = int(expr[1:], 16)
        elif expr.startswith('0x'):
            val = int(expr, 16)
        elif expr.isdigit():
            val = int(expr)
        else:
            # Expression like TK_END+1 or TK_LT-TK_PLUS
            m_add = re.match(r'(\w+)\s*\+\s*(\d+)', expr)
            m_sub = re.match(r'(\w+)\s*-\s*(\w+)', expr)
            if m_add:
                val = resolve(m_add.group(1)) + int(m_add.group(2))
            elif m_sub:
                val = resolve(m_sub.group(1)) - resolve(m_sub.group(2))
            else:
                # Just a reference to another name
                val = resolve(expr)
        resolved[name] = val
        return val

    for name in equates:
        resolve(name)

    return resolved


def parse_keyword_tables(lines: list[str], equates: dict[str, int]) -> dict[int, str]:
    """Parse TAB_ASCx, TAB_STAR, etc. to extract token->keyword mappings."""
    # Build reverse map: TK name -> value
    tk_to_val = {k: v for k, v in equates.items() if k.startswith('TK_')}
    val_to_tk = {v: k for k, v in tk_to_val.items()}

    # Map from token value to keyword string
    token_keywords: dict[int, str] = {}

    # Current first letter for TAB_ASCx tables
    current_prefix = ''

    # Regex for TAB_ASCx table labels
    tab_asc_re = re.compile(r'^TAB_ASC([A-Z])\s*$')
    # Operator table labels and their prefix characters
    op_tables = {
        'TAB_STAR': '*', 'TAB_PLUS': '+', 'TAB_MNUS': '-',
        'TAB_SLAS': '/', 'TAB_LESS': '<', 'TAB_EQUL': '=',
        'TAB_MORE': '>', 'TAB_QEST': '?', 'TAB_POWR': '^',
    }
    # LBB_ label regex
    lbb_re = re.compile(r'^LBB_\w+\s*$')

    in_table = False

    for line in lines:
        stripped = line.strip()

        # Check for TAB_ASCx
        m = tab_asc_re.match(stripped)
        if m:
            current_prefix = m.group(1)
            in_table = True
            continue

        # Check for operator tables
        for tab_name, char in op_tables.items():
            if stripped.startswith(tab_name) and (len(stripped) == len(tab_name) or stripped[len(tab_name)] in ' \t'):
                current_prefix = char
                in_table = True
                break

        # Skip LBB_ labels
        if lbb_re.match(stripped):
            continue

        # Parse .byte directives within tables
        if in_table and stripped.startswith('.byte'):
            byte_content = stripped[5:].strip()
            # Remove trailing comments (but careful with quoted strings)
            # Find the comment that's outside quotes
            comment_stripped = _strip_comment(byte_content)

            # Check if this is a terminator line
            if comment_stripped.strip() == '$00' or comment_stripped.strip() == '0':
                in_table = False
                continue

            # Parse the byte directive to extract string + token
            keyword = _parse_byte_entry(comment_stripped, current_prefix, equates)
            if keyword:
                kw_str, tk_val = keyword
                token_keywords[tk_val] = kw_str

    return token_keywords


def _strip_comment(s: str) -> str:
    """Strip trailing ; comment, respecting quoted strings."""
    in_quote = False
    for i, c in enumerate(s):
        if c == '"':
            in_quote = not in_quote
        elif c == ';' and not in_quote:
            return s[:i].strip()
    return s.strip()


def _parse_byte_entry(byte_args: str, prefix: str, equates: dict[str, int]) -> tuple[str, int] | None:
    """Parse a .byte directive like '"BS(",TK_ABS' and return (keyword, token_value)."""
    # Split into parts by comma, respecting quotes
    parts = _split_byte_args(byte_args)
    if not parts:
        return None

    # Find the token reference (TK_xxx) - it's the last part that resolves to a token
    tk_name = None
    tk_val = None
    string_parts = []

    for part in parts:
        part = part.strip()
        if part in equates and part.startswith('TK_'):
            tk_name = part
            tk_val = equates[part]
        elif part == '$00' or part == '0':
            continue  # terminator
        elif part.startswith('"') and part.endswith('"'):
            string_parts.append(part[1:-1])
        elif part.startswith('$'):
            # Raw hex byte - skip (like $FF placeholder)
            pass

    if tk_val is None:
        return None

    # Build the keyword: prefix + string parts
    suffix = ''.join(string_parts)
    keyword = prefix + suffix

    return (keyword, tk_val)


def _split_byte_args(s: str) -> list[str]:
    """Split comma-separated .byte arguments, respecting quoted strings."""
    parts = []
    current = []
    in_quote = False
    for c in s:
        if c == '"':
            in_quote = not in_quote
            current.append(c)
        elif c == ',' and not in_quote:
            parts.append(''.join(current).strip())
            current = []
        else:
            current.append(c)
    if current:
        parts.append(''.join(current).strip())
    return parts


def parse_extended_tokens(lines: list[str], equates: dict[str, int]) -> dict[int, str]:
    """Parse TAB_XTKSTR @s_ labels to get extended token keywords."""
    xtk_keywords: dict[int, str] = {}

    # Build ordered list of XTK names and values
    xtk_items = sorted(
        [(v, k) for k, v in equates.items() if k.startswith('XTK_')],
        key=lambda x: x[0]
    )

    # Parse @s_ string labels
    s_label_re = re.compile(r'^@s_\w+:\s*\.byte\s+"([^"]*)",0\s*(?:;.*)?$')
    s_label_placeholder_re = re.compile(r'^@s_\w+:\s*\.byte\s+\$FF,0')

    # Collect string labels in order from TAB_XTKSTR
    # The word table gives the order, but we need to match @s_ labels to XTK_ ids
    # Strategy: parse the .word entries to get label order, then match to strings

    # First, collect all @s_ label -> string mappings
    label_strings: dict[str, str] = {}
    for line in lines:
        stripped = line.strip()
        m = s_label_re.match(stripped)
        if m:
            label = stripped.split(':')[0].strip()
            label_strings[label] = m.group(1)
        elif s_label_placeholder_re.match(stripped):
            label = stripped.split(':')[0].strip()
            label_strings[label] = None  # reserved/placeholder

    # Parse the .word table to get label order
    in_xtkstr = False
    word_labels: list[str] = []
    repeat_count = 0
    repeat_label = None

    for line in lines:
        stripped = line.strip()
        if stripped.startswith('TAB_XTKSTR'):
            in_xtkstr = True
            continue
        if in_xtkstr:
            if stripped.startswith('.repeat'):
                m2 = re.match(r'\.repeat\s+\$?([0-9A-Fa-f]+)', stripped)
                if m2:
                    val_str = m2.group(1)
                    if '$' in stripped:
                        repeat_count = int(val_str, 16)
                    else:
                        repeat_count = int(val_str)
            elif stripped.startswith('.endrepeat'):
                if repeat_label:
                    for _ in range(repeat_count):
                        word_labels.append(repeat_label)
                repeat_count = 0
                repeat_label = None
            elif stripped.startswith('.word'):
                words_part = _strip_comment(stripped[5:])
                labels_on_line = []
                for w in words_part.split(','):
                    w = w.strip()
                    if w.startswith('@'):
                        labels_on_line.append(w)
                if repeat_count > 0:
                    # Inside .repeat block — set the repeat label
                    if labels_on_line:
                        repeat_label = labels_on_line[0]
                else:
                    word_labels.extend(labels_on_line)
            elif stripped.startswith('@s_') or stripped == '':
                # Reached the string definitions, done with word table
                in_xtkstr = False

    # Now map: word_labels[i] corresponds to XTK id i+1
    for i, label in enumerate(word_labels):
        xtk_id = i + 1
        kw = label_strings.get(label)
        if kw and kw != '\xff':
            # Check if this XTK id actually has an equate
            xtk_name = None
            for v, k in xtk_items:
                if v == xtk_id:
                    xtk_name = k
                    break
            if xtk_name:
                xtk_keywords[xtk_id] = kw

    return xtk_keywords


def generate(asm_path: str, output_path: str):
    """Generate tokens.json from basic.asm."""
    lines = Path(asm_path).read_text().splitlines()

    equates = resolve_equates(lines)
    primary = parse_keyword_tables(lines, equates)
    extended = parse_extended_tokens(lines, equates)

    result = {
        "version": 1,
        "prefix_byte": equates.get('TKX_PREFIX', 1),
        "primary": {str(k): v for k, v in sorted(primary.items())},
        "extended": {str(k): v for k, v in sorted(extended.items())},
    }

    Path(output_path).write_text(json.dumps(result, indent=2) + '\n')
    print(f"Generated {output_path}: {len(primary)} primary + {len(extended)} extended tokens")


def main():
    p = argparse.ArgumentParser(description="Extract EhBASIC token tables from basic.asm")
    p.add_argument("asm_file", help="Path to basic.asm")
    p.add_argument("-o", "--output", default="tokens.json", help="Output path")
    args = p.parse_args()
    generate(args.asm_file, args.output)


if __name__ == "__main__":
    main()
