#!/usr/bin/env python3
"""Generate the Nova NDK reference book API chapter from runtime comments."""

from __future__ import annotations

import argparse
import json
from collections import defaultdict
from pathlib import Path
from typing import Iterable

from gen_runtime_abi_docs import parse_equates, parse_tagged_sources, resolve_expr


LIBRARY_ORDER = [
    "nova",
    "fio",
    "audio",
    "vgc",
    "sprite",
    "msprite",
    "vsprite",
    "vtext",
    "nui",
    "copper",
    "dma",
    "blitter",
    "xram",
    "xmc",
    "pager",
    "rng",
    "nvg",
    "anim",
    "tween",
    "nic",
    "gameserver",
    "overlay",
    "math_copro",
    "math_fixed8",
    "strip",
    "vgc_wait",
]


LIBRARY_TITLES = {
    "anim": "Animation",
    "audio": "Audio and Music",
    "blitter": "Blitter",
    "copper": "Copper",
    "dma": "DMA",
    "fio": "File I/O",
    "gameserver": "Game Server",
    "math_copro": "Math Coprocessor",
    "math_fixed8": "Fixed-Point Math",
    "msprite": "Meta-Sprites",
    "nic": "Network Interface",
    "nova": "Nova Core",
    "nui": "Nova UI",
    "nvg": "Nova Graphics Images",
    "overlay": "Overlays",
    "pager": "File Pager",
    "rng": "Random Numbers",
    "sprite": "Sprites",
    "strip": "Runtime Stripping",
    "tween": "Tweening",
    "vgc": "Video and Graphics Controller",
    "vgc_wait": "VGC Wait Helpers",
    "vsprite": "Vector Sprites",
    "vtext": "Vector Text",
    "xmc": "Expansion Memory Controller",
    "xram": "Expansion RAM",
}


REQUIRED_KEYS = ("label", "kind", "symbol", "summary")


def collect_sources(runtime_dir: Path) -> list[Path]:
    return sorted([*runtime_dir.glob("*.inc"), *runtime_dir.glob("*.s")])


def library_name(entry: dict[str, object]) -> str:
    explicit = str(entry.get("library", "")).strip().lower()
    if explicit:
        return explicit
    return Path(str(entry["source"])).stem


def sort_key(name: str) -> tuple[int, str]:
    try:
        return (LIBRARY_ORDER.index(name), name)
    except ValueError:
        return (len(LIBRARY_ORDER), name)


def validate(entries: Iterable[dict[str, object]]) -> None:
    errors: list[str] = []
    for entry in entries:
        missing = [key for key in REQUIRED_KEYS if not str(entry.get(key, "")).strip()]
        if missing:
            location = f"{entry.get('source')}:{entry.get('line')}"
            label = entry.get("label", "<unknown>")
            errors.append(f"{location}: {label}: missing {', '.join(missing)}")
    if errors:
        raise SystemExit("Invalid NDK API docs:\n" + "\n".join(errors))


def enrich_values(entries: list[dict[str, object]], equates: dict[str, str]) -> None:
    for entry in entries:
        symbol = str(entry.get("symbol", "")).strip()
        kind = str(entry.get("kind", "")).strip()
        if kind == "routine" or not symbol:
            continue
        if symbol not in equates:
            continue
        entry["value"] = equates[symbol]
        resolved = resolve_expr(symbol, equates)
        if resolved is not None:
            entry["resolved"] = f"${resolved:04X}" if resolved > 0xFF else f"${resolved:02X}"


def make_json(entries: list[dict[str, object]]) -> dict[str, object]:
    libraries: dict[str, list[dict[str, object]]] = defaultdict(list)
    for entry in entries:
        libraries[library_name(entry)].append(entry)

    result = []
    for name in sorted(libraries, key=sort_key):
        sources = sorted({Path(str(entry["source"])).name for entry in libraries[name]})
        result.append(
            {
                "name": name,
                "title": LIBRARY_TITLES.get(name, name.replace("_", " ").title()),
                "sources": sources,
                "entries": sorted(libraries[name], key=lambda item: str(item.get("label", ""))),
            }
        )
    return {"version": 1, "libraries": result}


def tex_escape(value: object) -> str:
    text = str(value)
    replacements = {
        "\\": r"\textbackslash{}",
        "&": r"\&",
        "%": r"\%",
        "$": r"\$",
        "#": r"\#",
        "_": r"\_",
        "{": r"\{",
        "}": r"\}",
        "~": r"\textasciitilde{}",
        "^": r"\textasciicircum{}",
    }
    return "".join(replacements.get(ch, ch) for ch in text)


def tex_code(value: object) -> str:
    return rf"\texttt{{{tex_escape(value)}}}"


def tex_list(values: object) -> list[str]:
    if not isinstance(values, list):
        return []
    return [str(value) for value in values if str(value).strip()]


def tex_named_list(values: object) -> list[dict[str, str]]:
    if not isinstance(values, list):
        return []
    result = []
    for value in values:
        if isinstance(value, dict):
            name = str(value.get("name", "")).strip()
            description = str(value.get("description", "")).strip()
            if name:
                result.append({"name": name, "description": description})
    return result


def write_tex(api: dict[str, object], output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "% Generated by tools/gen_ndk_reference.py. Do not edit.",
        r"\chapter{Runtime Library Reference}",
        "",
        "This chapter is generated from structured comments in the Nova runtime",
        "assembly sources. It lists the callable routines, constants,",
        "pseudo-registers, and shared data blocks exposed to assembly programs.",
        "",
    ]

    for library in api["libraries"]:
        name = str(library["name"])
        title = str(library["title"])
        sources = library.get("sources", [])
        entries = library["entries"]
        if not entries:
            continue
        if isinstance(sources, list) and sources:
            source_text = ", ".join(tex_code(source) for source in sources)
        else:
            source_text = tex_code(name)

        lines.extend(
            [
                rf"\section{{{tex_escape(title)}}}",
                rf"\noindent\textit{{Generated from {source_text}.}}",
                "",
            ]
        )

        for entry in entries:
            label = str(entry.get("label", ""))
            kind = str(entry.get("kind", ""))
            symbol = str(entry.get("symbol", ""))
            summary = str(entry.get("summary", ""))
            lines.extend(
                [
                    rf"\begin{{ndkapibox}}{{{tex_escape(label)}}}{{{tex_escape(kind)}}}",
                    rf"\textbf{{Symbol:}} {tex_code(symbol)}\\",
                    rf"\textbf{{Summary:}} {tex_escape(summary)}",
                ]
            )
            if entry.get("abi"):
                lines.append(rf"\par\textbf{{ABI:}} {tex_escape(entry['abi'])}")
            if entry.get("value"):
                resolved = entry.get("resolved")
                if resolved:
                    lines.append(rf"\par\textbf{{Value:}} {tex_code(entry['value'])} ({tex_code(resolved)})")
                else:
                    lines.append(rf"\par\textbf{{Value:}} {tex_code(entry['value'])}")

            inputs = tex_named_list(entry.get("inputs"))
            outputs = tex_named_list(entry.get("outputs"))
            requires = tex_list(entry.get("requires"))

            if inputs:
                lines.append(r"\par\textbf{Inputs:}")
                lines.append(r"\begin{itemize}")
                for item in inputs:
                    lines.append(rf"\item {tex_code(item['name'])}: {tex_escape(item['description'])}")
                lines.append(r"\end{itemize}")

            if outputs:
                lines.append(r"\par\textbf{Outputs:}")
                lines.append(r"\begin{itemize}")
                for item in outputs:
                    lines.append(rf"\item {tex_code(item['name'])}: {tex_escape(item['description'])}")
                lines.append(r"\end{itemize}")

            if requires:
                lines.append(r"\par\textbf{Requires:}")
                lines.append(", ".join(tex_code(value) for value in requires))

            source = Path(str(entry.get("source", ""))).name
            line = entry.get("line", "")
            lines.append(rf"\par\textbf{{Source:}} {tex_code(source)} line {tex_escape(line)}")
            lines.append(r"\end{ndkapibox}")
            lines.append("")

    output.write_text("\n".join(lines).rstrip() + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runtime-dir", type=Path, required=True)
    parser.add_argument("--tex", type=Path, required=True)
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()

    sources = collect_sources(args.runtime_dir)
    entries = parse_tagged_sources(sources)
    validate(entries)
    enrich_values(entries, parse_equates(sources))
    api = make_json(entries)

    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(api, indent=2) + "\n")
    write_tex(api, args.tex)
    total = sum(len(library["entries"]) for library in api["libraries"])
    print(f"Generated {args.tex}, {args.json}: {len(api['libraries'])} libraries, {total} entries")


if __name__ == "__main__":
    main()
