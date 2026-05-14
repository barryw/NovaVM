#!/usr/bin/env python3
"""Build and run the assembly sprite multiplex smoke test on live hardware."""

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
SOURCE = REPO_ROOT / "docs/programs/sprite_multiplex_asm.s"
CFG = REPO_ROOT / "docs/programs/diag9000.cfg"
LOAD_ADDR = 0x9000
VGC_PLANE_SPRITE = 4
SPRITE_TRANSPARENT_COLOR = 0x6
VISIBLE_SPRITE_COLORS = set(range(16)) - {SPRITE_TRANSPARENT_COLOR}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--timeout", type=float, default=20.0)
    parser.add_argument("--review-delay", type=float, default=5.0)
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


def build_demo() -> bytes:
    with tempfile.TemporaryDirectory(prefix="nova-sprite-mux-") as tmp:
        obj = Path(tmp) / "sprite_multiplex_asm.o"
        out = Path(tmp) / "sprite_multiplex_asm.bin"
        subprocess.run(
            [
                "ca65",
                "--cpu",
                "65c02",
                "-I",
                str(REPO_ROOT / "ehbasic/lib"),
                "-I",
                str(REPO_ROOT / "ehbasic"),
                "-o",
                str(obj),
                str(SOURCE),
            ],
            check=True,
        )
        subprocess.run(["ld65", "-C", str(CFG), "-o", str(out), str(obj)], check=True)
        return out.read_bytes()


def parse_byte_token(token: str) -> int:
    token = token.strip()
    if token.startswith("$"):
        return int(token[1:], 16)
    return int(token, 0)


def expected_demo_shape_data() -> list[int]:
    in_shape_data = False
    data: list[int] = []

    for line in SOURCE.read_text(encoding="utf-8").splitlines():
        code = line.split(";", 1)[0].strip()
        if not in_shape_data:
            in_shape_data = code == "shape_data:"
            continue
        if not code.startswith(".byte"):
            continue
        for token in code.removeprefix(".byte").split(","):
            data.append(parse_byte_token(token) & 0xFF)

    if len(data) == 0 or len(data) % 128 != 0:
        raise RuntimeError(f"expected sprite shape data to be non-empty 128-byte shapes, saw {len(data)} bytes")
    return data


def verify_palette_coverage(data: list[int]) -> None:
    seen = set()
    for value in data:
        seen.add((value >> 4) & 0x0F)
        seen.add(value & 0x0F)

    missing = sorted(VISIBLE_SPRITE_COLORS - seen)
    if missing:
        missing_text = ", ".join(f"${value:X}" for value in missing)
        raise RuntimeError(f"sprite demo palette coverage missing visible colors: {missing_text}")


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


def verify_shape_vram(client: NovaHostClient) -> None:
    expected = expected_demo_shape_data()
    verify_palette_coverage(expected)
    actual: list[int] = []
    for offset in range(0, len(expected), 256):
        count = min(256, len(expected) - offset)
        response = client.read_vram(VGC_PLANE_SPRITE, offset, count)
        values = response.get("data")
        if values is None:
            values = [response.get("value")]
        actual.extend(int(value) & 0xFF for value in values)
    if actual != expected:
        for index, (want, got) in enumerate(zip(expected, actual)):
            if want != got:
                raise RuntimeError(
                    f"shape RAM mismatch at ${index:04X}: saw ${got:02X}, expected ${want:02X}"
                )
        raise RuntimeError("shape RAM mismatch")


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


def enabled_sprite_count(client: NovaHostClient) -> int:
    count = 0
    for sprite in range(16):
        flags = client.peek(0xA040 + sprite * 8 + 5)
        if flags & 0x80:
            count += 1
    return count


def sprite0_x(client: NovaHostClient) -> int:
    return client.peek(0xA040) | (client.peek(0xA041) << 8)


def sample_motion_paused(client: NovaHostClient) -> tuple[int, int]:
    client.pause()
    try:
        return enabled_sprite_count(client), sprite0_x(client)
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
        verify_shape_vram(client)

        count, x0 = sample_motion_paused(client)
        time.sleep(0.5)
        _, x1 = sample_motion_paused(client)

        if count != 16:
            raise RuntimeError(f"expected 16 enabled sprites, saw {count}")
        if x0 == x1:
            raise RuntimeError(f"sprite 0 X did not move: {x0}")

        print(f"PASS sprite multiplex demo running: {len(data)} bytes at ${LOAD_ADDR:04X}, x0 {x0}->{x1}")
    finally:
        client.close()
        if not args.leave_boot_config:
            put_json(args.host, "/sd/config/boot.json", original_config)


def main() -> int:
    args = parse_args()
    data = build_demo()
    if len(data) > 0x0800:
        raise RuntimeError(f"demo is {len(data)} bytes, exceeds diag9000.cfg 2 KiB budget")
    run_demo(args, data)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001 - hardware smoke should report concise failure.
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
