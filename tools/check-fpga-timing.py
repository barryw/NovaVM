#!/usr/bin/env python3
"""Fail when a nextpnr JSON timing report misses any declared fmax constraint."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


MAX_FREQUENCY_RE = re.compile(
    r"Max frequency for clock\s+(.+?):\s+([0-9.]+)\s+MHz\s+"
    r"\((PASS|FAIL) at\s+([0-9.]+)\s+MHz\)"
)


def load_fmax_from_log(path: Path) -> dict[str, dict[str, float]]:
    fmax: dict[str, dict[str, float]] = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = MAX_FREQUENCY_RE.search(line)
        if match is None:
            continue
        clock = match.group(1).strip().strip("'")
        fmax[clock] = {
            "achieved": float(match.group(2)),
            "constraint": float(match.group(4)),
        }
    return fmax


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path, help="nextpnr --report JSON file")
    parser.add_argument(
        "--log",
        type=Path,
        help="nextpnr log to parse when --no-route leaves the JSON fmax empty",
    )
    parser.add_argument(
        "--margin-mhz",
        type=float,
        default=0.0,
        help="extra required headroom above each clock constraint",
    )
    args = parser.parse_args()

    with args.report.open("r", encoding="utf-8") as f:
        report = json.load(f)

    fmax = report.get("fmax")
    if not isinstance(fmax, dict):
        print(f"error: {args.report} has no fmax object", file=sys.stderr)
        return 2
    if not fmax and args.log is not None:
        fmax = load_fmax_from_log(args.log)
    if not fmax:
        print(
            f"error: {args.report} has an empty fmax object"
            + (f" and {args.log} had no Max frequency lines" if args.log else ""),
            file=sys.stderr,
        )
        return 2

    failed = []
    for clock, data in sorted(fmax.items()):
        achieved = float(data["achieved"])
        constraint = float(data["constraint"])
        required = constraint + args.margin_mhz
        status = "PASS" if achieved >= required else "FAIL"
        print(
            f"{status}: {clock}: achieved {achieved:.2f} MHz, "
            f"required {required:.2f} MHz"
        )
        if achieved < required:
            failed.append((clock, achieved, required))

    if failed:
        print("", file=sys.stderr)
        print("Timing failed:", file=sys.stderr)
        for clock, achieved, required in failed:
            print(
                f"  {clock}: achieved {achieved:.2f} MHz, "
                f"required {required:.2f} MHz",
                file=sys.stderr,
            )
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
