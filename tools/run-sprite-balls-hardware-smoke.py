#!/usr/bin/env python3
"""Build and run an assembly sprite demo on live hardware."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import tempfile
import time
import urllib.error
import urllib.request
from pathlib import Path

from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = REPO_ROOT / "docs/programs/sprite_balls_asm.s"
CFG = REPO_ROOT / "docs/programs/diag9000_4k.cfg"
LOAD_ADDR = 0x9000
LOAD_LIMIT = 0x1000
SPRITE_BASE = 0xA040
SPRITE_STRIDE = 8
SPR_FLAG_ENABLE = 0x80


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--timeout", type=float, default=20.0)
    parser.add_argument("--review-delay", type=float, default=8.0)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE)
    parser.add_argument("--expected-sprites", type=int, default=8)
    parser.add_argument("--no-movement-check", action="store_true")
    parser.add_argument("--expected-y", type=int)
    parser.add_argument("--expected-shape", type=int)
    parser.add_argument(
        "--leave-boot-config",
        action="store_true",
        help="do not restore /config/boot.json after temporarily unmounting fd0",
    )
    return parser.parse_args()


def http_url(host: str, path: str) -> str:
    return f"http://{host}{path}"


def http_request(
    host: str,
    method: str,
    path: str,
    data: bytes | None = None,
    content_type: str = "application/octet-stream",
    timeout: float = 30.0,
) -> bytes:
    headers = {}
    if data is not None:
        headers["Content-Type"] = content_type
    request = urllib.request.Request(http_url(host, path), data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            return response.read()
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"{method} {http_url(host, path)} failed: HTTP {exc.code}: {body}") from exc


def get_json(host: str, path: str) -> dict[str, object]:
    return json.loads(http_request(host, "GET", path).decode("utf-8"))


def put_json(host: str, path: str, value: dict[str, object]) -> None:
    data = json.dumps(value, indent=2).encode("utf-8") + b"\n"
    http_request(host, "PUT", path, data, content_type="application/json")


def post_json(host: str, path: str, payload: dict[str, object] | None = None) -> dict[str, object]:
    data = json.dumps(payload or {}, separators=(",", ":")).encode("utf-8")
    return json.loads(http_request(host, "POST", path, data, content_type="application/json").decode("utf-8"))


def build_demo(source: Path) -> bytes:
    with tempfile.TemporaryDirectory(prefix="nova-sprite-balls-") as tmp:
        obj = Path(tmp) / "sprite_demo.o"
        out = Path(tmp) / "sprite_demo.bin"
        subprocess.run(
            [
                "ca65",
                "--cpu",
                "65c02",
                "-I",
                str(REPO_ROOT / "runtime/asm"),
                "-I",
                str(REPO_ROOT / "ehbasic"),
                "-o",
                str(obj),
                str(source),
            ],
            check=True,
        )
        subprocess.run(["ld65", "-C", str(CFG), "-o", str(out), str(obj)], check=True)
        return out.read_bytes()


def peek_block(client: NovaHostClient, address: int, count: int) -> list[int]:
    response = client.raw({"command": "peek_block", "address": address, "count": count}, require_ok=True)
    return [int(v) & 0xFF for v in response["values"]]


def poke_block_bytewise(client: NovaHostClient, address: int, data: bytes) -> None:
    for offset, value in enumerate(data):
        client.poke(address + offset, value)


def verify_loaded(client: NovaHostClient, data: bytes) -> None:
    for offset in range(0, len(data), 256):
        chunk = data[offset : offset + 256]
        actual = bytes(peek_block(client, LOAD_ADDR + offset, len(chunk)))
        if actual != chunk:
            for index, (want, got) in enumerate(zip(chunk, actual)):
                if want != got:
                    addr = LOAD_ADDR + offset + index
                    raise RuntimeError(
                        f"load verify failed at ${addr:04X}: saw ${got:02X}, expected ${want:02X}"
                    )
            raise RuntimeError("load verify failed")


def wait_ready(client: NovaHostClient, timeout: float) -> None:
    deadline = time.monotonic() + timeout
    last_error = ""
    while time.monotonic() < deadline:
        try:
            client.wait_ready(timeout_ms=1000)
            return
        except (NovaHostError, OSError, TimeoutError) as exc:
            last_error = str(exc)
            client.close()
            time.sleep(0.25)
    raise TimeoutError(f"BASIC did not reach Ready: {last_error}")


def type_basic_line(client: NovaHostClient, line: str) -> None:
    client.basic_line(line)


def flush_basic_input(client: NovaHostClient, timeout: float) -> None:
    client.type_text("\r")
    time.sleep(0.2)
    wait_ready(client, timeout)


def sprite_position(client: NovaHostClient, sprite: int) -> tuple[int, int]:
    base = SPRITE_BASE + sprite * SPRITE_STRIDE
    x = client.peek(base) | (client.peek(base + 1) << 8)
    y = client.peek(base + 2) | (client.peek(base + 3) << 8)
    return x, y


def sprite_shape(client: NovaHostClient, sprite: int) -> int:
    return client.peek(SPRITE_BASE + sprite * SPRITE_STRIDE + 4)


def enabled_sprite_count(client: NovaHostClient) -> int:
    count = 0
    for sprite in range(16):
        flags = client.peek(SPRITE_BASE + sprite * SPRITE_STRIDE + 5)
        if flags & SPR_FLAG_ENABLE:
            count += 1
    return count


def sample_sprites_paused(
    client: NovaHostClient,
    expected_sprites: int,
) -> tuple[int, list[tuple[int, int]], list[int]]:
    client.pause()
    try:
        count = enabled_sprite_count(client)
        positions = [sprite_position(client, i) for i in range(expected_sprites)]
        shapes = [sprite_shape(client, i) for i in range(expected_sprites)]
        return count, positions, shapes
    finally:
        client.resume()


def run_demo(args: argparse.Namespace, data: bytes) -> None:
    original_config = get_json(args.host, "/sd/config/boot.json")
    client = NovaHostClient(host=args.host, timeout=args.timeout)
    try:
        post_json(args.host, "/drives/fd0/unmount")
        client.cold_start(wait_ready=False)
        wait_ready(client, args.timeout)

        client.pause()
        poke_block_bytewise(client, LOAD_ADDR, data)
        verify_loaded(client, data)
        client.resume()

        flush_basic_input(client, args.timeout)
        type_basic_line(client, f"SYS {LOAD_ADDR}")
        time.sleep(args.review_delay)

        count, before, _ = sample_sprites_paused(client, args.expected_sprites)
        time.sleep(0.5)
        _, after, shapes = sample_sprites_paused(client, args.expected_sprites)

        if count != args.expected_sprites:
            raise RuntimeError(f"expected {args.expected_sprites} enabled sprites, saw {count}")
        if not args.no_movement_check and before == after:
            raise RuntimeError(f"ball sprite positions did not change: {before}")
        if args.expected_y is not None:
            bad = [(index, y) for index, (_, y) in enumerate(after) if y != args.expected_y]
            if bad:
                raise RuntimeError(f"sprites not at expected y={args.expected_y}: {bad}")
        if args.expected_shape is not None:
            bad = [(index, shape) for index, shape in enumerate(shapes) if shape != args.expected_shape]
            if bad:
                raise RuntimeError(f"sprites not at expected shape={args.expected_shape}: {bad}")
        for sample_name, positions in (("before", before), ("after", after)):
            for index, (x, y) in enumerate(positions):
                if not (0 <= x <= 304 and 0 <= y <= 184):
                    raise RuntimeError(
                        f"ball {index} out of bounds in {sample_name} smoke sample: x={x}, y={y}"
                    )

        print(
            f"PASS assembly sprite demo running: {len(data)} bytes at ${LOAD_ADDR:04X}, "
            f"sprite0 {before[0]}->{after[0]}"
        )
    finally:
        client.close()
        if not args.leave_boot_config:
            put_json(args.host, "/sd/config/boot.json", original_config)


def main() -> int:
    args = parse_args()
    data = build_demo(args.source)
    if len(data) > LOAD_LIMIT:
        raise RuntimeError(f"demo is {len(data)} bytes, exceeds $9000 4K window")
    run_demo(args, data)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001 - hardware smoke should report concise failure.
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
