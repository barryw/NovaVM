#!/usr/bin/env python3
"""Stress NovaHost's FPGA SPI bridge through SDRAM read/write round trips."""

from __future__ import annotations

import argparse
import random
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from novahost_client import (  # noqa: E402
    DEFAULT_DEBUG_PORT,
    DEFAULT_HOST,
    DEFAULT_HTTP_PORT,
    DEFAULT_TIMEOUT,
    NovaHostClient,
    NovaHostError,
    parse_int,
)


BLOCK_SIZE = 256


def pattern_for(block: int, iteration: int) -> list[int]:
    seed = 0x6502_0000 ^ (iteration << 12) ^ block
    rng = random.Random(seed)
    mode = (block + iteration) % 6
    if mode == 0:
        return [(i + block + iteration) & 0xFF for i in range(BLOCK_SIZE)]
    if mode == 1:
        return [0x00 if (i & 1) == 0 else 0xFF for i in range(BLOCK_SIZE)]
    if mode == 2:
        return [0xAA if (i & 1) == 0 else 0x55 for i in range(BLOCK_SIZE)]
    if mode == 3:
        return [((i * 17) + (block * 31) + iteration) & 0xFF for i in range(BLOCK_SIZE)]
    if mode == 4:
        return [0xFF - ((i + block) & 0xFF) for i in range(BLOCK_SIZE)]
    return [rng.randrange(0, 256) for _ in range(BLOCK_SIZE)]


def read_sdram(client: NovaHostClient, address: int) -> list[int]:
    response = client.raw(
        {"command": "read_sdram", "address": address, "count": BLOCK_SIZE},
        require_ok=True,
    )
    values = response.get("values")
    if not isinstance(values, list) or len(values) != BLOCK_SIZE:
        raise NovaHostError(f"read_sdram returned malformed values at 0x{address:06X}")
    return [int(value) & 0xFF for value in values]


def write_sdram(client: NovaHostClient, address: int, values: list[int]) -> None:
    client.raw(
        {"command": "write_sdram", "address": address, "values": values},
        require_ok=True,
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Round-trip patterned SDRAM blocks through the NovaHost FPGA SPI bridge."
    )
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_DEBUG_PORT)
    parser.add_argument("--http-port", type=int, default=DEFAULT_HTTP_PORT)
    parser.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT)
    parser.add_argument("--base", default="0x7F0000", help="scratch SDRAM byte address")
    parser.add_argument("--blocks", type=int, default=64, help="256-byte blocks per iteration")
    parser.add_argument("--iterations", type=int, default=2)
    parser.add_argument("--no-restore", action="store_true", help="leave test pattern in SDRAM")
    args = parser.parse_args()

    if args.blocks < 1:
        parser.error("--blocks must be >= 1")
    if args.iterations < 1:
        parser.error("--iterations must be >= 1")

    base = parse_int(args.base)
    total_bytes = args.blocks * BLOCK_SIZE * args.iterations
    originals: list[list[int]] = []

    started = time.monotonic()
    with NovaHostClient(args.host, args.port, args.http_port, args.timeout) as client:
        health = client.health()
        if not health.get("ok") or not health.get("fpgaBridgeAvailable", True):
            raise NovaHostError(f"NovaHost bridge is not ready: {health}")

        print(
            f"SPI bridge SDRAM stress: base=0x{base:06X} "
            f"blocks={args.blocks} iterations={args.iterations}"
        )

        for block in range(args.blocks):
            originals.append(read_sdram(client, base + block * BLOCK_SIZE))

        try:
            for iteration in range(args.iterations):
                for block in range(args.blocks):
                    address = base + block * BLOCK_SIZE
                    expected = pattern_for(block, iteration)
                    write_sdram(client, address, expected)
                    actual = read_sdram(client, address)
                    if actual != expected:
                        for offset, (a, e) in enumerate(zip(actual, expected, strict=True)):
                            if a != e:
                                raise NovaHostError(
                                    "SDRAM mismatch at "
                                    f"0x{address + offset:06X}: got 0x{a:02X}, expected 0x{e:02X}"
                                )
                        raise NovaHostError(f"SDRAM mismatch in block {block}")
                print(f"  iteration {iteration + 1}/{args.iterations}: ok")
        finally:
            if not args.no_restore:
                for block, original in enumerate(originals):
                    write_sdram(client, base + block * BLOCK_SIZE, original)

    elapsed = time.monotonic() - started
    print(f"PASS: {total_bytes} patterned bytes verified in {elapsed:.2f}s")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
