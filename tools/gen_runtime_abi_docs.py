#!/usr/bin/env python3
"""Generate runtime label docs and lookup tables from @label comments."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Iterable


COMMENT_RE = re.compile(r"^\s*;\s*@(\w+)(?:\s+(.*))?$")
EQUATE_RE = re.compile(r"^\s*([A-Za-z_][\w]*)\s*=\s*([^;]+)")
SYM_RE = re.compile(r"^al\s+([0-9A-Fa-f]+)\s+\.(\S+)")


def parse_tagged_sources(paths: Iterable[Path]) -> list[dict[str, object]]:
    entries: list[dict[str, object]] = []
    pending: dict[str, object] | None = None

    for path in paths:
        for line_no, line in enumerate(path.read_text().splitlines(), start=1):
            match = COMMENT_RE.match(line)
            if match:
                key = match.group(1).lower()
                value = (match.group(2) or "").strip()
                if key == "label":
                    if pending:
                        entries.append(pending)
                    pending = {
                        "label": value,
                        "source": str(path),
                        "line": line_no,
                        "inputs": [],
                        "outputs": [],
                        "requires": [],
                    }
                elif pending is not None:
                    if key == "in":
                        cast_list(pending, "inputs").append(parse_named_text(value))
                    elif key == "out":
                        cast_list(pending, "outputs").append(parse_named_text(value))
                    elif key == "requires":
                        cast_list(pending, "requires").extend(split_words(value))
                    else:
                        pending[key] = value
                continue

            if pending and line.strip() and not line.lstrip().startswith(";"):
                entries.append(pending)
                pending = None

    if pending:
        entries.append(pending)

    return entries


def cast_list(entry: dict[str, object], key: str) -> list[object]:
    value = entry.setdefault(key, [])
    if not isinstance(value, list):
        raise TypeError(f"{entry.get('label')}: expected list for {key}")
    return value


def parse_named_text(value: str) -> dict[str, str]:
    if ":" in value:
        name, text = value.split(":", 1)
        return {"name": name.strip(), "description": text.strip()}
    parts = value.split(None, 1)
    if len(parts) == 2:
        return {"name": parts[0], "description": parts[1]}
    return {"name": value, "description": ""}


def split_words(value: str) -> list[str]:
    return [part.strip() for part in re.split(r"[,\s]+", value) if part.strip()]


def parse_equates(paths: Iterable[Path]) -> dict[str, str]:
    equates: dict[str, str] = {}
    for path in paths:
        for line in path.read_text().splitlines():
            match = EQUATE_RE.match(line)
            if match:
                equates[match.group(1)] = match.group(2).strip()
    return equates


def parse_symbols(path: Path | None) -> dict[str, int]:
    if path is None or not path.exists():
        return {}
    symbols: dict[str, int] = {}
    for line in path.read_text().splitlines():
        match = SYM_RE.match(line)
        if match:
            symbols[match.group(2)] = int(match.group(1), 16)
    return symbols


def resolve_expr(expr: str, equates: dict[str, str], seen: set[str] | None = None) -> int | None:
    expr = expr.strip()
    if not expr:
        return None
    if expr.startswith("$"):
        return int(expr[1:], 16)
    if expr.startswith("0x"):
        return int(expr, 16)
    if expr.isdigit():
        return int(expr)

    if seen is None:
        seen = set()
    if expr in seen or expr not in equates:
        return None
    seen.add(expr)
    return resolve_expr(equates[expr], equates, seen)


def enrich(entries: list[dict[str, object]], equates: dict[str, str], symbols: dict[str, int]) -> list[dict[str, object]]:
    for entry in entries:
        symbol = str(entry.get("symbol", ""))
        address = None
        if symbol in symbols:
            address = symbols[symbol]
        elif symbol in equates:
            address = resolve_expr(symbol, equates)
        if address is not None:
            entry["address"] = f"${address:04X}" if address > 0xFF else f"${address:02X}"
    return sorted(entries, key=lambda item: str(item.get("label", "")))


def write_json(entries: list[dict[str, object]], output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({"version": 1, "labels": entries}, indent=2) + "\n")


def address_to_int(value: object) -> int | None:
    if not isinstance(value, str):
        return None
    text = value.strip()
    if text.startswith("$"):
        return int(text[1:], 16)
    if text.startswith("0x"):
        return int(text, 16)
    if text.isdigit():
        return int(text)
    return None


def label_hash(label: str) -> int:
    h = 0
    for ch in label.upper().encode("ascii"):
        h = ((h << 5) + h + ch) & 0xFF
    return h


def write_asm(entries: list[dict[str, object]], output: Path, bucket_count: int = 64) -> None:
    if bucket_count <= 0 or bucket_count & (bucket_count - 1):
        raise ValueError("bucket_count must be a power of two")

    labels: list[tuple[str, int]] = []
    for entry in entries:
        label = str(entry.get("label", "")).strip().upper()
        address = address_to_int(entry.get("address"))
        if not label or address is None:
            continue
        try:
            label.encode("ascii")
        except UnicodeEncodeError as exc:
            raise ValueError(f"{label}: runtime labels must be ASCII") from exc
        if len(label) > 255:
            raise ValueError(f"{label}: runtime label is too long")
        labels.append((label, address))

    buckets: list[list[tuple[str, int]]] = [[] for _ in range(bucket_count)]
    for label, address in labels:
        buckets[label_hash(label) & (bucket_count - 1)].append((label, address))

    output.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "; Generated by tools/gen_runtime_abi_docs.py. Do not edit.",
        "; Runtime label hash table for ADDR(\"LABEL\") in the extension ROM.",
        "",
        f"RUNTIME_LABEL_BUCKET_COUNT = {bucket_count}",
        f"RUNTIME_LABEL_COUNT = {len(labels)}",
        "",
        "RuntimeLabelBuckets:",
    ]
    for index in range(bucket_count):
        lines.append(f"      .word @bucket_{index:02X}")

    for index, bucket in enumerate(buckets):
        lines.append("")
        lines.append(f"@bucket_{index:02X}:")
        for label, address in sorted(bucket):
            lines.append(f"      .byte {len(label)}")
            lines.append(f"      .word ${address:04X}")
            lines.append(f"      .byte {json.dumps(label)}")
        lines.append("      .byte 0")

    output.write_text("\n".join(lines).rstrip() + "\n")


def write_markdown(entries: list[dict[str, object]], output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Runtime Labels",
        "",
        "Generated from `@label` comments in the assembly runtime sources.",
        "",
    ]

    for entry in entries:
        label = str(entry.get("label", ""))
        lines.append(f"## {label}")
        summary = str(entry.get("summary", "")).strip()
        if summary:
            lines.append("")
            lines.append(summary)
        lines.append("")
        lines.append(f"- Kind: `{entry.get('kind', 'unknown')}`")
        if entry.get("symbol"):
            lines.append(f"- Symbol: `{entry['symbol']}`")
        if entry.get("address"):
            lines.append(f"- Address: `{entry['address']}`")
        if entry.get("abi"):
            lines.append(f"- ABI: `{entry['abi']}`")
        for key, title in (("inputs", "Inputs"), ("outputs", "Outputs")):
            values = entry.get(key)
            if isinstance(values, list) and values:
                lines.append("")
                lines.append(f"{title}:")
                for value in values:
                    if isinstance(value, dict):
                        lines.append(f"- `{value.get('name', '')}`: {value.get('description', '')}")
        requires = entry.get("requires")
        if isinstance(requires, list) and requires:
            lines.append("")
            lines.append("Requires:")
            for value in requires:
                lines.append(f"- `{value}`")
        lines.append("")

    output.write_text("\n".join(lines).rstrip() + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sources", nargs="+", type=Path)
    parser.add_argument("--sym", type=Path, default=None, help="ld65 -Ln symbol file for routine addresses")
    parser.add_argument("--json", type=Path, required=True, help="Output runtime label JSON")
    parser.add_argument("--md", type=Path, required=True, help="Output Markdown reference")
    parser.add_argument("--asm", type=Path, default=None, help="Output ca65 lookup table include")
    args = parser.parse_args()

    entries = parse_tagged_sources(args.sources)
    equates = parse_equates(args.sources)
    symbols = parse_symbols(args.sym)
    entries = enrich(entries, equates, symbols)
    write_json(entries, args.json)
    write_markdown(entries, args.md)
    if args.asm is not None:
        write_asm(entries, args.asm)
    outputs = [str(args.json), str(args.md)]
    if args.asm is not None:
        outputs.append(str(args.asm))
    print(f"Generated {', '.join(outputs)}: {len(entries)} labels")


if __name__ == "__main__":
    main()
