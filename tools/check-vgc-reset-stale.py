#!/usr/bin/env python3
"""Verify system reset clears stale VGC memory on live Nova hardware."""

from __future__ import annotations

import argparse
import json
import os
import socket
import sys
import time
from dataclasses import dataclass


DEFAULT_HOST = os.environ.get("NOVAHOST", "192.168.1.65")
DEFAULT_PORT = int(os.environ.get("NOVAHOST_PORT", "6503"))
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
    Plane("tile patterns", 6, 32768, 0xC3, True),
)


class DebugClient:
    def __init__(self, host: str, port: int, timeout: float):
        self._sock = socket.create_connection((host, port), timeout=timeout)
        self._sock.settimeout(timeout)

    def close(self) -> None:
        self._sock.close()

    def command(self, command: str, **kwargs: int | str) -> dict:
        payload = {"command": command, **kwargs}
        self._sock.sendall(json.dumps(payload, separators=(",", ":")).encode("utf-8") + b"\n")
        raw = b""
        while not raw.endswith(b"\n"):
            chunk = self._sock.recv(65536)
            if not chunk:
                break
            raw += chunk
        if not raw:
            raise RuntimeError(f"{command}: no response")
        try:
            response = json.loads(raw.decode("utf-8", errors="replace"))
        except json.JSONDecodeError as exc:
            raise RuntimeError(f"{command}: invalid JSON response: {raw!r}") from exc
        if not response.get("ok", False):
            raise RuntimeError(f"{command}: {response.get('error', response)}")
        return response


def fill_plane(client: DebugClient, plane: Plane) -> None:
    client.command(
        "fill_vram",
        space=plane.space,
        address=0,
        value=plane.dirty_value,
        length=plane.length,
    )


def read_block(client: DebugClient, space: int, address: int, length: int) -> list[int]:
    response = client.command("read_vram", space=space, address=address, length=length)
    if length == 1:
        return [int(response["value"])]
    return [int(v) for v in response["data"]]


def scan_plane(client: DebugClient, plane: Plane) -> tuple[int, str | None]:
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
        description="Dirty VGC memories, cold-reset the VM, and verify stale data is gone."
    )
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument("--settle", type=float, default=0.5, help="seconds to wait after cold_start")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    client = DebugClient(args.host, args.port, args.timeout)

    try:
        status = client.command("boot_status")
        if not status.get("fpgaBridgeAvailable", True):
            raise RuntimeError(f"FPGA bridge unavailable: {status}")

        client.command("dbg_pause")

        print("Dirtying VGC memory planes...")
        for plane in PLANES:
            fill_plane(client, plane)
            print(f"  dirtied {plane.name}")

        print("Issuing cold_start reset...")
        client.command("cold_start", wait_ready=0)
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
    finally:
        client.close()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
