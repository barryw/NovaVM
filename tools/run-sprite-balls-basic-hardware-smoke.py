#!/usr/bin/env python3
"""Run a NovaBASIC sprite demo on live hardware."""

from __future__ import annotations

import argparse
import json
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PROGRAM = REPO_ROOT / "docs/programs/sprite_balls_basic.bas"
BASIC_ROM = REPO_ROOT / "ehbasic/basic.bin"
EXTENSION_ROM = REPO_ROOT / "ehbasic/extension.bin"
SPRITE_BASE = 0xA040
SPRITE_STRIDE = 8
SPR_FLAG_ENABLE = 0x80


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--timeout", type=float, default=20.0)
    parser.add_argument("--line-delay", type=float, default=0.02)
    parser.add_argument("--review-delay", type=float, default=8.0)
    parser.add_argument("--program", type=Path, default=DEFAULT_PROGRAM)
    parser.add_argument("--expected-sprites", type=int, default=8)
    parser.add_argument("--expected-y", type=int)
    parser.add_argument("--expected-shape", type=int)
    parser.add_argument(
        "--startup-timeout",
        type=float,
        default=30.0,
        help="seconds to wait after RUN for all ball sprites to become enabled",
    )
    parser.add_argument(
        "--skip-rom-upload",
        action="store_true",
        help="assume /roms/novabasic.bin and /roms/extension.bin are already current",
    )
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


def put_file(host: str, src: Path, dest: str, timeout: float) -> None:
    http_request(host, "PUT", f"/sd{dest}", src.read_bytes(), timeout=timeout)


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


def upload_runtime_roms(args: argparse.Namespace, client: NovaHostClient) -> None:
    if args.skip_rom_upload:
        return
    print("upload /roms/novabasic.bin", flush=True)
    put_file(args.host, BASIC_ROM, "/roms/novabasic.bin", args.timeout)
    print("upload /roms/extension.bin", flush=True)
    put_file(args.host, EXTENSION_ROM, "/roms/extension.bin", args.timeout)
    print("reload ROMs from SD", flush=True)
    client.reload_rom()


def program_lines(program: Path) -> list[str]:
    lines = []
    for line in program.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if stripped:
            lines.append(stripped)
    return lines


def type_program(client: NovaHostClient, lines: list[str], line_delay: float) -> None:
    print(f"type {len(lines)} BASIC lines", flush=True)
    for index, line in enumerate(lines, 1):
        client.basic_line(line)
        if line_delay > 0:
            time.sleep(line_delay)
        if index % 20 == 0:
            print(f"  typed {index}/{len(lines)}", flush=True)


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


def wait_sprites_enabled(client: NovaHostClient, timeout: float, expected: int) -> float:
    start = time.monotonic()
    deadline = start + timeout
    while time.monotonic() < deadline:
        if enabled_sprite_count(client) == expected:
            return time.monotonic() - start
        time.sleep(0.25)
    count = enabled_sprite_count(client)
    raise TimeoutError(f"expected {expected} enabled sprites, saw {count} after {timeout:.1f}s")


def assert_no_basic_error(client: NovaHostClient) -> None:
    screen = "\n".join(str(line) for line in client.read_screen().get("lines", []))
    if " Error in line " in screen or "Syntax Error" in screen:
        raise RuntimeError(f"BASIC reported an error while running sprite balls demo:\n{screen}")


def run_demo(args: argparse.Namespace) -> None:
    original_config = get_json(args.host, "/sd/config/boot.json")
    client = NovaHostClient(host=args.host, timeout=args.timeout)
    try:
        upload_runtime_roms(args, client)
        post_json(args.host, "/drives/fd0/unmount")
        client.cold_start(wait_ready=False)
        wait_ready(client, args.timeout)

        type_program(client, program_lines(args.program), args.line_delay)
        client.basic_line("RUN")
        print("RUN sent; waiting for sprites to enable", flush=True)
        startup_seconds = wait_sprites_enabled(client, args.startup_timeout, args.expected_sprites)
        print(f"all sprites enabled after {startup_seconds:.2f}s; leaving demo on screen for review", flush=True)
        time.sleep(args.review_delay)
        assert_no_basic_error(client)

        count, before, _ = sample_sprites_paused(client, args.expected_sprites)
        time.sleep(0.5)
        _, after, shapes = sample_sprites_paused(client, args.expected_sprites)

        if count != args.expected_sprites:
            raise RuntimeError(f"expected {args.expected_sprites} enabled sprites, saw {count}")
        if before == after:
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

        print(f"PASS BASIC sprite balls demo running: ball0 {before[0]}->{after[0]}", flush=True)
    finally:
        client.close()
        if not args.leave_boot_config:
            put_json(args.host, "/sd/config/boot.json", original_config)


def main() -> int:
    args = parse_args()
    run_demo(args)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001 - hardware smoke should report concise failure.
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
