#!/usr/bin/env python3
"""Verify system reset clears stale VGC memory on live Nova hardware."""

from __future__ import annotations

import argparse
import sys
import time
from dataclasses import dataclass

from novahost_client import DEFAULT_DEBUG_PORT, DEFAULT_HOST, NovaHostClient

BLOCK = 256


@dataclass(frozen=True)
class Plane:
    name: str
    space: int
    length: int
    dirty_value: int
    expect_zero: bool


PLANES = (
    Plane("text chars", 1, 4000, 0x01, False),
    Plane("text colors", 2, 4000, 0x06, False),
    Plane("text attrs", 7, 4000, 0x80, True),
    Plane("graphics bitmap", 3, 64000, 0x0D, True),
    Plane("sprite shapes", 4, 2048, 0xA5, True),
)


def fill_plane(client: NovaHostClient, plane: Plane) -> None:
    client.fill_vram(plane.space, 0, plane.dirty_value, plane.length)


def read_block(client: NovaHostClient, space: int, address: int, length: int) -> list[int]:
    response = client.read_vram(space, address, length)
    if length == 1:
        return [int(response["value"])]
    return [int(v) for v in response["data"]]


def scan_plane(client: NovaHostClient, plane: Plane) -> tuple[int, str | None]:
    failures = 0
    first: str | None = None

    for offset in range(0, plane.length, BLOCK):
        length = min(BLOCK, plane.length - offset)
        data = read_block(client, plane.space, offset, length)
        for index, value in enumerate(data):
            bad = value != 0 if plane.expect_zero else value == plane.dirty_value
            if not bad:
                continue
            failures += 1
            if first is None:
                expected = "0" if plane.expect_zero else f"not {plane.dirty_value}"
                first = f"offset {offset + index}: got {value}, expected {expected}"

    return failures, first


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Dirty VGC memories, reset the current VM runtime, and verify stale data is gone."
    )
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_DEBUG_PORT)
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument("--settle", type=float, default=0.5, help="seconds to wait after vm_reset")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    with NovaHostClient(args.host, args.port, timeout=args.timeout) as client:
        status = client.boot_status()
        if not status.get("fpgaBridgeAvailable", True):
            raise RuntimeError(f"FPGA bridge unavailable: {status}")

        client.pause()

        print("Dirtying VGC memory planes...")
        for plane in PLANES:
            fill_plane(client, plane)
            print(f"  dirtied {plane.name}")

        print("Issuing vm_reset...")
        client.vm_reset(wait_ready=False)
        time.sleep(args.settle)

        print("Scanning for stale data...")
        failed = False
        for plane in PLANES:
            failures, first = scan_plane(client, plane)
            if failures:
                failed = True
                print(f"FAIL {plane.name}: {failures} stale bytes; first {first}")
            else:
                expectation = "zeroed" if plane.expect_zero else "dirty sentinel removed"
                print(f"PASS {plane.name}: {expectation}")

        return 1 if failed else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
