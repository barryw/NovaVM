#!/usr/bin/env python3
"""Run Nova Chess smoke checks against live Nova hardware."""

from __future__ import annotations

import argparse
import json
import socket
import subprocess
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path
from typing import Iterable

from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_IMAGE = REPO_ROOT / "examples/novachess/dist/mode-test.ndi"
DEFAULT_MOUNT_PATH = "/novachess-smoke/mode-test.ndi"
VGC_PLANE_GFX = 3
VGC_PLANE_CHAR = 1
SCREEN_COLS = 80


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--image", type=Path, default=DEFAULT_IMAGE)
    parser.add_argument("--mount-path", default=DEFAULT_MOUNT_PATH)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--leave-mounted", action="store_true")
    parser.add_argument("--boot-timeout", type=float, default=90.0)
    parser.add_argument("--mode-timeout", type=float, default=45.0)
    parser.add_argument("--input-review-delay", type=float, default=10.0)
    parser.add_argument(
        "--only",
        default="all",
        help="comma-separated subset: menu,zero,one,two,input,network, or all",
    )
    parser.add_argument("--start-server", action="store_true")
    parser.add_argument("--game-server-host")
    parser.add_argument("--game-server-port", type=int, default=16504)
    return parser.parse_args()


def http_url(host: str, path: str) -> str:
    return f"http://{host}{path}"


def http_request(
    host: str,
    method: str,
    path: str,
    data: bytes | None = None,
    content_type: str = "application/octet-stream",
    timeout: float = 120.0,
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


def get_json(host: str, path: str, timeout: float = 120.0) -> dict[str, object]:
    return json.loads(http_request(host, "GET", path, timeout=timeout).decode("utf-8"))


def put_json(host: str, path: str, value: dict[str, object]) -> None:
    data = json.dumps(value, indent=2).encode("utf-8") + b"\n"
    http_request(host, "PUT", path, data, content_type="application/json")


def post_json(host: str, path: str, payload: dict[str, object] | None = None) -> dict[str, object]:
    data = json.dumps(payload or {}, separators=(",", ":")).encode("utf-8")
    return json.loads(http_request(host, "POST", path, data, content_type="application/json").decode("utf-8"))


def put_file(host: str, src: Path, dest: str) -> None:
    http_request(host, "PUT", f"/sd{dest}", src.read_bytes(), timeout=180.0)


def wait_for_health(host: str, timeout: float) -> None:
    deadline = time.monotonic() + timeout
    last_error = ""
    while time.monotonic() < deadline:
        try:
            health = get_json(host, "/health", timeout=2.0)
            if health.get("ok") and health.get("fpgaBridgeAvailable", True):
                return
            last_error = str(health)
        except Exception as exc:  # noqa: BLE001 - polling must tolerate reboot churn.
            last_error = str(exc)
        time.sleep(0.5)
    raise TimeoutError(f"NovaHost health did not recover within {timeout}s: {last_error}")


def build_image(image: Path) -> None:
    subprocess.run(
        [
            "make",
            "-C",
            str(REPO_ROOT / "examples/novachess"),
            "ndi",
            "BUILD_DIR=build-mode",
            f"IMAGE={image}",
            "CA65FLAGS=-DCHESS_AUTOSTART_DEMO=0",
        ],
        check=True,
    )


def screen_text(client: NovaHostClient) -> str:
    response = client.read_screen()
    return "\n".join(str(line) for line in response.get("lines", []))


def normalize(text: str) -> str:
    return " ".join(text.split())


def contains(screen: str, expected: str) -> bool:
    return expected in screen or normalize(expected) in normalize(screen)


def wait_screen(client: NovaHostClient, expected: Iterable[str], timeout: float) -> str:
    expected = tuple(expected)
    deadline = time.monotonic() + timeout
    last_screen = ""
    while time.monotonic() < deadline:
        try:
            last_screen = screen_text(client)
        except (NovaHostError, OSError, TimeoutError):
            client.close()
            time.sleep(0.25)
            continue

        if all(contains(last_screen, item) for item in expected):
            return last_screen
        time.sleep(0.25)
    raise TimeoutError(f"timed out waiting for {expected}\n{last_screen}")


def require_gfx_pixel(client: NovaHostClient, x: int, y: int, expected: int, label: str) -> None:
    response = client.read_vram(VGC_PLANE_GFX, y * 320 + x, 1)
    values = response.get("data")
    actual = int(values[0] if isinstance(values, list) else response.get("value", 0)) & 0xFF
    if actual != expected:
        raise RuntimeError(f"{label}: expected gfx {x},{y}=${expected:02X}, saw ${actual:02X}")


def cold_start_to_menu(client: NovaHostClient, timeout: float) -> str:
    try:
        client.fill_vram(VGC_PLANE_CHAR, 0, 0x20, SCREEN_COLS * 25)
    except Exception:
        pass
    client.cold_start(wait_ready=False)
    client.close()
    time.sleep(2.0)
    screen = wait_screen(client, ("SELECT A MODE", "4  NETWORK GAME"), timeout)
    time.sleep(0.5)
    settled = screen_text(client)
    if all(contains(settled, item) for item in ("SELECT A MODE", "4  NETWORK GAME")):
        screen = settled
    require_gfx_pixel(client, 90, 80, 0x0E, "menu splash yellow")
    require_gfx_pixel(client, 150, 80, 0x03, "menu splash cyan")
    require_gfx_pixel(client, 170, 75, 0x0D, "menu splash green")
    require_gfx_pixel(client, 220, 85, 0x07, "menu splash accent")
    return screen


def parse_modes(value: str) -> set[str]:
    if value == "all":
        return {"menu", "zero", "one", "two", "input", "network"}

    modes = {part.strip() for part in value.split(",") if part.strip()}
    valid = {"menu", "zero", "one", "two", "input", "network"}
    unknown = modes - valid
    if unknown:
        raise ValueError(f"unknown --only mode(s): {', '.join(sorted(unknown))}")
    return modes


def send_key(client: NovaHostClient, key: str) -> None:
    client.send_key(key)
    time.sleep(0.2)


def type_text(client: NovaHostClient, text: str) -> None:
    client.type_text(text)
    time.sleep(0.35)


def review_pause(label: str, seconds: float) -> None:
    if seconds <= 0:
        return
    print(f"review pause {label}: {seconds:g}s", flush=True)
    time.sleep(seconds)


def run_zero_player(client: NovaHostClient, boot_timeout: float, mode_timeout: float) -> None:
    cold_start_to_menu(client, boot_timeout)
    send_key(client, "1")
    wait_screen(client, ("DIFFICULTY",), mode_timeout)
    send_key(client, "E")
    wait_screen(client, ("01.W", "01.B", "STATUS", "MOVES"), mode_timeout)
    require_gfx_pixel(client, 64, 162, 0x01, "0-player board")
    print("PASS 0-player computer vs computer", flush=True)


def run_one_player(client: NovaHostClient, boot_timeout: float, mode_timeout: float) -> None:
    cold_start_to_menu(client, boot_timeout)
    send_key(client, "2")
    wait_screen(client, ("DIFFICULTY",), mode_timeout)
    send_key(client, "E")
    wait_screen(client, ("PLAY AS",), mode_timeout)
    send_key(client, "W")
    wait_screen(client, ("MOVE:", "TURN WHITE"), mode_timeout)
    type_text(client, "E2E4\r")
    wait_screen(client, ("01.W E2-E4", "01.B", "STATUS", "MOVES"), mode_timeout)
    require_gfx_pixel(client, 64, 162, 0x01, "1-player board")
    print("PASS 1-player human vs computer", flush=True)


def run_two_player(client: NovaHostClient, boot_timeout: float, mode_timeout: float) -> None:
    cold_start_to_menu(client, boot_timeout)
    send_key(client, "3")
    wait_screen(client, ("MOVE:", "TURN WHITE"), mode_timeout)
    type_text(client, "E2E4\r")
    wait_screen(client, ("01.W E2-E4", "MOVE:"), mode_timeout)
    type_text(client, "E7E5\r")
    wait_screen(client, ("01.W E2-E4", "01.B E7-E5", "TURN WHITE", "STATUS", "MOVES"), mode_timeout)
    require_gfx_pixel(client, 64, 162, 0x01, "2-player board")
    print("PASS 2-player human vs human", flush=True)


def run_input_validation(
    client: NovaHostClient,
    boot_timeout: float,
    mode_timeout: float,
    review_delay: float,
) -> None:
    cold_start_to_menu(client, boot_timeout)
    send_key(client, "3")
    wait_screen(client, ("MOVE:", "TURN WHITE"), mode_timeout)
    type_text(client, "A3")
    wait_screen(client, ("MOVE:A3", "BAD FROM SQUARE"), mode_timeout)
    require_gfx_pixel(client, 10, 116, 0x0A, "invalid from square outline")

    send_key(client, "BACKSPACE")
    send_key(client, "BACKSPACE")
    wait_screen(client, ("MOVE:", "TURN WHITE"), mode_timeout)
    type_text(client, "E2E5")
    wait_screen(client, ("MOVE:E2-E5", "ILLEGAL MOVE"), mode_timeout)
    require_gfx_pixel(client, 98, 72, 0x0A, "illegal to square outline")

    cold_start_to_menu(client, boot_timeout)
    send_key(client, "3")
    wait_screen(client, ("MOVE:", "TURN WHITE"), mode_timeout)
    type_text(client, "E2")
    wait_screen(client, ("MOVE:E2-"), mode_timeout)
    review_pause("from square E2", review_delay)
    send_key(client, "BACKSPACE")
    wait_screen(client, ("MOVE:E"), mode_timeout)
    type_text(client, "2E4")
    wait_screen(client, ("MOVE:E2-E4",), mode_timeout)
    review_pause("to square E4", review_delay)
    send_key(client, "ENTER")
    wait_screen(client, ("01.W E2-E4", "TURN BLACK", "STATUS", "MOVES"), mode_timeout)
    require_gfx_pixel(client, 64, 162, 0x01, "input backspace completed move")
    print("PASS move input validation and backspace", flush=True)


def run_network(client: NovaHostClient, boot_timeout: float, mode_timeout: float) -> None:
    cold_start_to_menu(client, boot_timeout)
    send_key(client, "4")
    wait_screen(client, ("MOVE:", "TURN WHITE", "STATUS", "MOVES"), mode_timeout)
    type_text(client, "E2E4\r")
    wait_screen(client, ("01.W E2-E4", "01.B", "STATUS", "MOVES"), mode_timeout)
    require_gfx_pixel(client, 64, 162, 0x01, "network board")
    print("PASS network game against configured server", flush=True)


def start_game_server(port: int) -> subprocess.Popen[str]:
    process = subprocess.Popen(
        ["dotnet", "run", "--project", str(REPO_ROOT / "e6502.GameServer"), "--", "--port", str(port)],
        cwd=REPO_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    deadline = time.monotonic() + 30
    while time.monotonic() < deadline:
        if process.poll() is not None:
            output = process.stdout.read() if process.stdout else ""
            raise RuntimeError(f"game server exited early with {process.returncode}\n{output}")
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.5):
                return process
        except OSError:
            time.sleep(0.25)
    process.terminate()
    raise TimeoutError(f"game server did not listen on port {port}")


def configure_game_server(host: str, server_host: str, server_port: int) -> dict[str, object]:
    config = get_json(host, "/sd/config/boot.json")
    network = config.setdefault("network", {})
    if not isinstance(network, dict):
        network = {}
        config["network"] = network
    game_server = network.setdefault("gameServer", {})
    if not isinstance(game_server, dict):
        game_server = {}
        network["gameServer"] = game_server
    game_server["host"] = server_host
    game_server["port"] = server_port
    put_json(host, "/sd/config/boot.json", config)
    return config


def upload_mount_and_reboot(host: str, image: Path, mount_path: str, timeout: float) -> None:
    if not image.is_file():
        raise FileNotFoundError(image)
    print(f"upload {image} -> {mount_path}", flush=True)
    post_json(host, "/drives/fd0/unmount")
    put_file(host, image, mount_path)
    post_json(host, "/drives/fd0/mount", {"path": mount_path})
    post_json(host, "/reboot")
    wait_for_health(host, timeout)


def cleanup(host: str, original_config: dict[str, object] | None, leave_mounted: bool) -> None:
    if original_config is not None:
        put_json(host, "/sd/config/boot.json", original_config)
    if not leave_mounted:
        post_json(host, "/drives/fd0/unmount")
        client = NovaHostClient(host=host, timeout=15)
        try:
            client.cold_start(wait_ready=False)
        finally:
            client.close()


def main() -> int:
    args = parse_args()
    image = args.image.resolve()
    original_config: dict[str, object] | None = None
    server_process: subprocess.Popen[str] | None = None

    try:
        if args.build:
            build_image(image)

        if args.start_server:
            server_process = start_game_server(args.game_server_port)

        original_config = get_json(args.host, "/sd/config/boot.json")
        if args.game_server_host:
            configure_game_server(args.host, args.game_server_host, args.game_server_port)

        upload_mount_and_reboot(args.host, image, args.mount_path, args.boot_timeout)

        client = NovaHostClient(host=args.host, timeout=20)
        try:
            modes = parse_modes(args.only)
            if "menu" in modes:
                cold_start_to_menu(client, args.boot_timeout)
                print("PASS menu splash and mode selector", flush=True)
            if "zero" in modes:
                run_zero_player(client, args.boot_timeout, args.mode_timeout)
            if "one" in modes:
                run_one_player(client, args.boot_timeout, args.mode_timeout)
            if "two" in modes:
                run_two_player(client, args.boot_timeout, args.mode_timeout)
            if "input" in modes:
                run_input_validation(client, args.boot_timeout, args.mode_timeout, args.input_review_delay)
            if "network" in modes:
                run_network(client, args.boot_timeout, args.mode_timeout)
        finally:
            client.close()

        print("PASS Nova Chess hardware smoke complete", flush=True)
        return 0
    finally:
        try:
            cleanup(args.host, original_config, args.leave_mounted)
        except Exception as exc:  # noqa: BLE001 - cleanup failure is diagnostic only.
            print(f"cleanup warning: {exc}", file=sys.stderr, flush=True)
        if server_process is not None and server_process.poll() is None:
            server_process.terminate()
            try:
                server_process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                server_process.kill()


if __name__ == "__main__":
    raise SystemExit(main())
