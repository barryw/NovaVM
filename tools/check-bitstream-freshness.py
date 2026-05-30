#!/usr/bin/env python3
"""Verify that a synthesized bitstream metadata file matches this checkout."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def metadata_path_for(bitstream: Path) -> Path:
    return bitstream.with_suffix(".metadata.json")


def iter_inputs(metadata: dict[str, Any], include_roms: bool) -> list[dict[str, Any]]:
    source = metadata.get("source")
    if not isinstance(source, dict):
        return []

    inputs: list[dict[str, Any]] = []
    rtl_inputs = source.get("rtl_inputs")
    if isinstance(rtl_inputs, list):
        inputs.extend(item for item in rtl_inputs if isinstance(item, dict))

    if include_roms:
        rom_inputs = source.get("rom_inputs")
        if isinstance(rom_inputs, list):
            inputs.extend(item for item in rom_inputs if isinstance(item, dict))

    return inputs


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Fail if bitstream metadata hashes do not match the current checkout."
    )
    parser.add_argument("bitstream", type=Path)
    parser.add_argument("--repo-root", type=Path, default=Path.cwd())
    parser.add_argument(
        "--include-roms",
        action="store_true",
        help="also compare ROM hex/bin inputs recorded in metadata",
    )
    args = parser.parse_args()

    bitstream = args.bitstream
    metadata_path = metadata_path_for(bitstream)
    if not metadata_path.is_file():
        print(f"error: bitstream metadata not found: {metadata_path}", file=sys.stderr)
        return 2

    try:
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        print(f"error: invalid bitstream metadata {metadata_path}: {exc}", file=sys.stderr)
        return 2

    repo_root = args.repo_root.resolve()
    mismatches: list[str] = []
    checked = 0
    for item in iter_inputs(metadata, include_roms=args.include_roms):
        rel = item.get("path")
        expected = item.get("sha256")
        if not isinstance(rel, str) or not isinstance(expected, str):
            continue

        path = repo_root / rel
        if not path.is_file():
            mismatches.append(f"{rel}: missing")
            continue

        actual = sha256(path)
        checked += 1
        if actual != expected:
            mismatches.append(f"{rel}: current {actual}, bitstream {expected}")

    if checked == 0:
        print(f"error: no comparable RTL inputs in metadata: {metadata_path}", file=sys.stderr)
        return 2

    if mismatches:
        print(f"error: stale bitstream: {bitstream}", file=sys.stderr)
        print(f"metadata: {metadata_path}", file=sys.stderr)
        for mismatch in mismatches[:20]:
            print(f"  {mismatch}", file=sys.stderr)
        if len(mismatches) > 20:
            print(f"  ... {len(mismatches) - 20} more mismatches", file=sys.stderr)
        return 1

    print(f"PASS: bitstream metadata matches {checked} current RTL input files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
