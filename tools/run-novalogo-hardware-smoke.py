#!/usr/bin/env python3
"""Run NovaLogo DRAW/debug smoke checks on live Nova hardware or Avalonia."""

from __future__ import annotations

import argparse
import copy
import http.client
import json
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path
from collections.abc import Callable
from typing import Any

from novahost_client import DEFAULT_DEBUG_PORT, DEFAULT_HOST, NovaHostClient, NovaHostError


REPO_ROOT = Path(__file__).resolve().parents[1]
LOGO_ROM = REPO_ROOT / "software/languages/novalogo/novalogo.bin"

TURTLE_STATE = 0x9F00
TURTLE_X_FRAC = 0
TURTLE_X_LO = 1
TURTLE_X_HI = 2
TURTLE_Y_FRAC = 3
TURTLE_Y_LO = 4
TURTLE_Y_HI = 5
TURTLE_HEADING_LO = 6
TURTLE_HEADING_HI = 7
TURTLE_PEN = 8
TURTLE_SHOWN = 9
TURTLE_COLOR = 10
TURTLE_INITED = 12

SPRITE_BASE = 0xA040
SPRITE_STRIDE = 8
SPRITE_FLAGS = 5
SPRITE_ENABLE = 0x80

VGC_GFX_SPACE = 3
VGC_MODE = 0xA000
GFX_WIDTH = 320
SPLIT_GFX_ROWS = 160


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--target", choices=("hardware", "avalonia"), default="hardware")
    parser.add_argument("--host", default=None)
    parser.add_argument("--debug-port", "--port", type=int, default=None)
    parser.add_argument("--timeout", type=float, default=30.0)
    parser.add_argument("--review-delay", type=float, default=0.0)
    parser.add_argument("--skip-rom-upload", action="store_true")
    parser.add_argument("--leave-boot-config", action="store_true")
    parser.add_argument("--skip-debug-breakpoint", action="store_true")
    args = parser.parse_args()
    if args.host is None:
        args.host = "127.0.0.1" if args.target == "avalonia" else DEFAULT_HOST
    if args.debug_port is None:
        args.debug_port = 6502 if args.target == "avalonia" else DEFAULT_DEBUG_PORT
    return args


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


def get_json(host: str, path: str) -> dict[str, Any]:
    return json.loads(http_request(host, "GET", path).decode("utf-8"))


def put_json(host: str, path: str, value: dict[str, Any]) -> None:
    data = json.dumps(value, indent=2).encode("utf-8") + b"\n"
    http_request(host, "PUT", path, data, content_type="application/json")


def put_file(host: str, src: Path, dest: str, timeout: float) -> None:
    if not src.exists():
        raise FileNotFoundError(f"{src} does not exist; build novalogo first")
    http_request(host, "PUT", f"/sd{dest}", src.read_bytes(), timeout=timeout)


def retry_sd_operation(description: str, timeout: float, operation: Callable[[], Any]) -> Any:
    deadline = time.monotonic() + timeout
    last_error: BaseException | None = None
    while time.monotonic() < deadline:
        try:
            return operation()
        except (RuntimeError, OSError, TimeoutError, urllib.error.URLError, http.client.IncompleteRead) as exc:
            last_error = exc
            time.sleep(0.5)
    raise TimeoutError(f"{description} did not complete before timeout: {last_error}") from last_error


def configure_novalogo_runtime(config: dict[str, Any]) -> dict[str, Any]:
    updated = copy.deepcopy(config)
    vm = updated.setdefault("vm", {})
    if not isinstance(vm, dict):
        vm = {}
        updated["vm"] = vm
    vm["defaultRuntime"] = "novalogo"

    languages = updated.setdefault("languages", {})
    if not isinstance(languages, dict):
        languages = {}
        updated["languages"] = languages
    logo = languages.setdefault("novalogo", {})
    if not isinstance(logo, dict):
        logo = {}
        languages["novalogo"] = logo
    logo["rom"] = "/roms/novalogo.bin"
    return updated


def screen_text(client: NovaHostClient) -> str:
    response = client.read_screen()
    return "\n".join(str(line) for line in response.get("lines", []))


def wait_screen_contains(client: NovaHostClient, text: str, timeout: float) -> str:
    deadline = time.monotonic() + timeout
    last_error = ""
    while time.monotonic() < deadline:
        try:
            screen = screen_text(client)
            if text in screen:
                return screen
        except (NovaHostError, OSError, TimeoutError) as exc:
            last_error = str(exc)
            client.close()
        time.sleep(0.25)
    raise TimeoutError(f"screen did not contain {text!r}: {last_error}\n{screen_text(client)}")


def wait_prompt_row(client: NovaHostClient, row: int | None, timeout: float) -> int:
    return wait_logo_ready(client, timeout, row=row)


def wait_for_condition(description: str, timeout: float, predicate: Callable[[], bool]) -> None:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if predicate():
            return
        time.sleep(0.05)
    raise TimeoutError(f"timed out waiting for {description}")


def wait_logo_ready(client: NovaHostClient, timeout: float, row: int | None = None) -> int:
    """Wait until LOGO is plausibly back at the prompt.

    Newer debug servers expose cursor enabled state. Older firmware only
    exposes x/y, so keep the row-40 prompt fallback for compatibility.
    """
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        cursor = client.get_cursor()
        cursor_x = int(cursor.get("x", -1))
        cursor_y = int(cursor.get("y", -1))
        screen = client.read_screen()
        lines = [str(line) for line in screen.get("lines", [])]
        cursor_line = lines[cursor_y] if 0 <= cursor_y < len(lines) else ""
        cursor_on_prompt = cursor_x == 2 and cursor_line.strip() == "?"
        if "enabled" in cursor:
            if cursor.get("enabled") and cursor_on_prompt and (row is None or cursor_y == row):
                return cursor_y
        else:
            if row is not None:
                if len(lines) > row and lines[row].strip() == "?" and cursor_y == row:
                    return row
            elif cursor_on_prompt:
                return cursor_y
        time.sleep(0.1)
    expected = f"row {row}" if row is not None else "the cursor row"
    raise TimeoutError(f"Logo prompt did not become ready on {expected}\n{screen_text(client)}")


def type_logo_line(client: NovaHostClient, line: str, timeout: float, row: int | None = None) -> int:
    client.type_text(f"{line}\r")
    # Fast commands can complete between network polls. Waiting on the prompt is
    # still useful for pacing and for catching a hung interpreter, but state
    # predicates below are the authoritative command-specific checks.
    return wait_logo_ready(client, min(timeout, 5.0), row=row)


def run_logo_line_until(
    client: NovaHostClient,
    line: str,
    timeout: float,
    description: str,
    predicate: Callable[[], bool],
    row: int | None = None,
) -> int:
    client.type_text(f"{line}\r")
    wait_for_condition(description, timeout, predicate)
    return wait_logo_ready(client, timeout, row=row)


def gfx_row(client: NovaHostClient, y: int, x: int = 0, width: int = GFX_WIDTH) -> list[int]:
    row: list[int] = []
    offset = 0
    while offset < width:
        chunk = min(256, width - offset)
        response = client.read_vram(VGC_GFX_SPACE, y * GFX_WIDTH + x + offset, chunk)
        values = response.get("values")
        if not isinstance(values, list):
            raise NovaHostError(f"read_vram returned no values for graphics row {y}")
        row.extend(int(value) & 0xFF for value in values)
        offset += chunk
    return row


def count_gfx_nonzero(client: NovaHostClient, x: int, y: int, width: int, height: int) -> int:
    total = 0
    for row in range(y, y + height):
        total += sum(1 for value in gfx_row(client, row, x, width) if value != 0)
    return total


def read_turtle_state(client: NovaHostClient) -> list[int]:
    return client.peek_block(TURTLE_STATE, 17)


def turtle_word(state: list[int], lo_offset: int, hi_offset: int) -> int:
    return state[lo_offset] | (state[hi_offset] << 8)


def assert_turtle_state(client: NovaHostClient) -> None:
    state = read_turtle_state(client)
    expected = {
        TURTLE_X_FRAC: 0,
        TURTLE_X_LO: 160,
        TURTLE_X_HI: 0,
        TURTLE_Y_FRAC: 0,
        TURTLE_Y_LO: 80,
        TURTLE_Y_HI: 0,
        TURTLE_HEADING_LO: 0,
        TURTLE_HEADING_HI: 0,
        TURTLE_PEN: 0,
        TURTLE_SHOWN: 1,
        TURTLE_COLOR: 1,
        TURTLE_INITED: 1,
    }
    for offset, want in expected.items():
        got = state[offset]
        if got != want:
            raise RuntimeError(
                f"turtle state ${TURTLE_STATE + offset:04X} = ${got:02X}, expected ${want:02X}; "
                f"state={state}"
            )


def run_turn_forward_smoke(
    client: NovaHostClient,
    timeout: float,
    prompt_row: int | None = None,
    pen_up_cmd: str = "PU",
    forward_cmd: str = "FD 20",
    pen_down_cmd: str = "PD",
    right_cmd: str = "RT 45",
) -> tuple[int, int, int, int, int]:
    run_logo_line_until(
        client,
        pen_up_cmd,
        timeout,
        f"{pen_up_cmd} to set pen up",
        lambda: read_turtle_state(client)[TURTLE_PEN] == 1,
    )

    prompt_row = run_logo_line_until(
        client,
        forward_cmd,
        timeout,
        f"pen-up {forward_cmd} to move north from center",
        lambda: turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 60,
    )

    prompt_row = run_logo_line_until(
        client,
        pen_down_cmd,
        timeout,
        f"{pen_down_cmd} to set pen down",
        lambda: read_turtle_state(client)[TURTLE_PEN] == 0,
    )

    prompt_row = run_logo_line_until(
        client,
        right_cmd,
        timeout,
        f"{right_cmd} to update heading",
        lambda: turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 45,
    )

    run_logo_line_until(
        client,
        forward_cmd,
        timeout,
        f"{forward_cmd} after {right_cmd} to update position",
        lambda: 172 <= turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) <= 175
        and 44 <= turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) <= 47,
    )

    client.pause()
    try:
        state = read_turtle_state(client)
        x = turtle_word(state, TURTLE_X_LO, TURTLE_X_HI)
        y = turtle_word(state, TURTLE_Y_LO, TURTLE_Y_HI)
        heading = turtle_word(state, TURTLE_HEADING_LO, TURTLE_HEADING_HI)
        if heading != 45:
            raise RuntimeError(f"{right_cmd} left heading={heading}, expected 45; state={state}")
        if not 172 <= x <= 175:
            raise RuntimeError(f"{forward_cmd} after {right_cmd} ended at x={x}, expected 172..175; state={state}")
        if not 44 <= y <= 47:
            raise RuntimeError(f"{forward_cmd} after {right_cmd} ended at y={y}, expected 44..47; state={state}")

        diagonal_pixels = count_gfx_nonzero(client, 161, 53, 7, 8)
        if diagonal_pixels < 4:
            raise RuntimeError(
                f"{forward_cmd} after {right_cmd} did not draw the expected diagonal, saw {diagonal_pixels} pixels"
            )

        old_center_pixels = count_gfx_nonzero(client, 152, 72, 16, 16)
        if old_center_pixels != 0:
            raise RuntimeError(f"isolated pen-down move redrew from old center: {old_center_pixels} pixels")

        return x, y, heading, diagonal_pixels, old_center_pixels
    finally:
        client.resume()


def run_cardinal_relative_turn_smoke(client: NovaHostClient, timeout: float) -> tuple[int, int, int]:
    run_logo_line_until(
        client,
        "CLEARSCREEN",
        timeout,
        "CLEARSCREEN to reset before cardinal relative-turn check",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80
        and turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 0,
    )
    run_logo_line_until(
        client,
        "PENUP",
        timeout,
        "PENUP before cardinal relative-turn check",
        lambda: read_turtle_state(client)[TURTLE_PEN] == 1,
    )
    run_logo_line_until(
        client,
        "FD 50",
        timeout,
        "FD 50 north with no x drift",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 30,
    )
    run_logo_line_until(
        client,
        "RT 90",
        timeout,
        "RT 90 to face east",
        lambda: turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 90,
    )
    run_logo_line_until(
        client,
        "FD 50",
        timeout,
        "FD 50 east with no y drift",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 210
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 30,
    )
    run_logo_line_until(
        client,
        "RT 90",
        timeout,
        "second RT 90 should be relative to east",
        lambda: turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 180,
    )
    run_logo_line_until(
        client,
        "FD 50",
        timeout,
        "FD 50 south after the second relative RT 90",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 210
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80,
    )

    state = read_turtle_state(client)
    return (
        turtle_word(state, TURTLE_X_LO, TURTLE_X_HI),
        turtle_word(state, TURTLE_Y_LO, TURTLE_Y_HI),
        turtle_word(state, TURTLE_HEADING_LO, TURTLE_HEADING_HI),
    )


def run_repeat_square_smoke(client: NovaHostClient, timeout: float) -> tuple[int, int, int, int, int]:
    prompt_row = run_logo_line_until(
        client,
        "CLEARSCREEN",
        timeout,
        "CLEARSCREEN to reset before REPEAT square check",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80
        and turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 0,
    )
    run_logo_line_until(
        client,
        "REPEAT 4 [FD 50 RT 90]",
        timeout,
        "REPEAT 4 [FD 50 RT 90] to draw a closed box",
        lambda: count_gfx_nonzero(client, 158, 28, 56, 5) >= 30,
    )

    client.pause()
    try:
        state = read_turtle_state(client)
        x = turtle_word(state, TURTLE_X_LO, TURTLE_X_HI)
        y = turtle_word(state, TURTLE_Y_LO, TURTLE_Y_HI)
        heading = turtle_word(state, TURTLE_HEADING_LO, TURTLE_HEADING_HI)
        top_pixels = count_gfx_nonzero(client, 158, 28, 56, 5)
        right_pixels = count_gfx_nonzero(client, 208, 28, 5, 56)
    finally:
        client.resume()

    if x != 160 or y != 80 or heading != 0:
        raise RuntimeError(f"REPEAT square did not close at the origin: x={x} y={y} heading={heading}")
    if top_pixels < 30 or right_pixels < 30:
        raise RuntimeError(f"REPEAT square did not draw expected box edges: top={top_pixels} right={right_pixels}")
    return x, y, heading, top_pixels, right_pixels


def assert_turtle_pixels(client: NovaHostClient) -> tuple[int, int]:
    center_pixels = count_gfx_nonzero(client, 152, 72, 16, 16)
    if not 20 <= center_pixels <= 80:
        raise RuntimeError(f"expected a small centered turtle, saw {center_pixels} nonzero center pixels")

    total_pixels = count_gfx_nonzero(client, 0, 0, GFX_WIDTH, SPLIT_GFX_ROWS)
    if not 20 <= total_pixels <= 140:
        raise RuntimeError(
            f"expected only the turtle in graphics plane after DRAW, saw {total_pixels} nonzero pixels"
        )
    return center_pixels, total_pixels


def run_setpos_smoke(client: NovaHostClient, timeout: float) -> tuple[int, int]:
    run_logo_line_until(
        client,
        "CLEARSCREEN",
        timeout,
        "CLEARSCREEN to reset before SETPOS",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80,
    )
    run_logo_line_until(
        client,
        "PENUP",
        timeout,
        "PENUP before SETPOS",
        lambda: read_turtle_state(client)[TURTLE_PEN] == 1,
    )
    run_logo_line_until(
        client,
        "SETPOS [100 50]",
        timeout,
        "SETPOS [100 50] to update position from a Logo list",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 100
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 50,
    )

    state = read_turtle_state(client)
    return (
        turtle_word(state, TURTLE_X_LO, TURTLE_X_HI),
        turtle_word(state, TURTLE_Y_LO, TURTLE_Y_HI),
    )


def run_procedure_editor_smoke(client: NovaHostClient, timeout: float) -> tuple[int, int, int]:
    run_logo_line_until(
        client,
        "CLEARSCREEN",
        timeout,
        "CLEARSCREEN to reset before TO editor smoke",
        lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
        and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80,
    )

    mode_before = client.peek(VGC_MODE)
    turtle_pixels_before = count_gfx_nonzero(client, 152, 72, 16, 16)
    client.type_text("TO HPROC\rPRINT 6060\r\x13\x11")
    wait_screen_contains(client, "HPROC DEFINED", timeout)
    wait_logo_ready(client, timeout)

    mode_after = client.peek(VGC_MODE)
    if mode_after != mode_before:
        raise RuntimeError(f"TO editor did not restore VGC mode: before=${mode_before:02X} after=${mode_after:02X}")

    turtle_pixels_after = count_gfx_nonzero(client, 152, 72, 16, 16)
    if turtle_pixels_after < turtle_pixels_before:
        raise RuntimeError(
            f"TO editor cleared graphics/turtle pixels: before={turtle_pixels_before} after={turtle_pixels_after}"
        )

    type_logo_line(client, "HPROC", timeout)
    wait_screen_contains(client, "6060", timeout)
    return mode_after, turtle_pixels_before, turtle_pixels_after


def decode_trace(response: dict[str, Any]) -> list[dict[str, Any]]:
    hex_text = str(response.get("hex", ""))
    if not hex_text:
        return list(response.get("records", []))
    data = bytes.fromhex(hex_text)
    record_bytes = int(response.get("record_bytes", 12))
    records = []
    for base in range(0, len(data) - record_bytes + 1, record_bytes):
        ctrl = data[base + 11]
        records.append(
            {
                "pc": (data[base] << 8) | data[base + 1],
                "state": data[base + 9] & 0x3F,
                "ir": data[base + 10],
                "decode": bool(ctrl & 0x10),
            }
        )
    return records


def wait_breakpoint_hit(client: NovaHostClient, timeout: float) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        state = client.state()
        if state.get("paused") and state.get("breakpoint_hit"):
            return state
        time.sleep(0.05)
    raise TimeoutError("debug breakpoint did not hit before timeout")


def run_debug_breakpoint_smoke(client: NovaHostClient, timeout: float) -> int:
    client.break_clear_all()
    paused = client.pause()
    trace = client.trace(64, hex_mode=True)
    decode_records = [record for record in decode_trace(trace) if record.get("decode")]
    if not decode_records:
        raise RuntimeError("trace buffer did not contain any decode records")

    pc = int(decode_records[-1]["pc"]) & 0xFFFF
    client.break_set(pc)
    client.resume()
    try:
        hit = wait_breakpoint_hit(client, min(timeout, 5.0))
        if (int(hit.get("pc", -1)) & 0xFFFF) != pc:
            raise RuntimeError(f"breakpoint hit at ${int(hit.get('pc', -1)) & 0xFFFF:04X}, expected ${pc:04X}")
        return pc
    finally:
        client.break_clear_all()
        client.resume()


def run_smoke(args: argparse.Namespace) -> None:
    original_config = None
    if args.target == "hardware":
        original_config = retry_sd_operation(
            "read original boot config",
            args.timeout,
            lambda: get_json(args.host, "/sd/config/boot.json"),
        )

    client = NovaHostClient(host=args.host, port=args.debug_port, timeout=args.timeout)
    try:
        if args.target == "hardware" and not args.skip_rom_upload:
            print("upload /roms/novalogo.bin", flush=True)
            retry_sd_operation(
                "upload /roms/novalogo.bin",
                args.timeout,
                lambda: put_file(args.host, LOGO_ROM, "/roms/novalogo.bin", args.timeout),
            )
        if args.target == "hardware":
            assert original_config is not None
            retry_sd_operation(
                "write NovaLogo boot config",
                args.timeout,
                lambda: put_json(args.host, "/sd/config/boot.json", configure_novalogo_runtime(original_config)),
            )
        print(f"cold-start NovaLogo on {args.target}", flush=True)
        try:
            if args.target == "avalonia":
                client.command("cold_start", require_ok=True, wait_ready=0, runtime="logo")
            else:
                client.cold_start(wait_ready=False)
        except OSError:
            # cold_start can reset the debug TCP connection before a response is
            # flushed. The following screen poll is the real boot assertion.
            pass
        finally:
            client.close()
        wait_screen_contains(client, "Nova LOGO v1.0", args.timeout)
        screen = wait_screen_contains(client, "BYTES FREE", args.timeout)
        if "BYTES FREE" not in screen or "-" in screen:
            raise RuntimeError(f"Logo banner/free-byte line is wrong:\n{screen}")
        wait_screen_contains(client, "?", args.timeout)
        prompt_row = wait_logo_ready(client, args.timeout)

        prompt_row = run_logo_line_until(
            client,
            "DRAW",
            args.timeout,
            "DRAW to initialize the centered turtle",
            lambda: read_turtle_state(client)[TURTLE_INITED] == 1
            and read_turtle_state(client)[TURTLE_SHOWN] == 1,
        )
        screen = screen_text(client)
        if "I DON'T KNOW HOW TO DRAW" in screen:
            raise RuntimeError(f"DRAW was not recognized:\n{screen}")

        client.pause()
        try:
            assert_turtle_state(client)
            center_pixels, total_pixels = assert_turtle_pixels(client)
            slot15_flags = client.peek(SPRITE_BASE + 15 * SPRITE_STRIDE + SPRITE_FLAGS)
            if slot15_flags & SPRITE_ENABLE:
                raise RuntimeError(f"Logo turtle consumed hardware sprite slot 15: flags=${slot15_flags:02X}")
        finally:
            client.resume()

        turn_x, turn_y, turn_heading, diagonal_pixels, old_center_pixels = run_turn_forward_smoke(
            client,
            args.timeout,
            prompt_row,
        )
        prompt_row = run_logo_line_until(
            client,
            "CLEARSCREEN",
            args.timeout,
            "CLEARSCREEN to reset before long-form turtle aliases",
            lambda: turtle_word(read_turtle_state(client), TURTLE_X_LO, TURTLE_X_HI) == 160
            and turtle_word(read_turtle_state(client), TURTLE_Y_LO, TURTLE_Y_HI) == 80
            and turtle_word(read_turtle_state(client), TURTLE_HEADING_LO, TURTLE_HEADING_HI) == 0,
        )
        long_x, long_y, long_heading, long_diagonal_pixels, long_old_center_pixels = run_turn_forward_smoke(
            client,
            args.timeout,
            prompt_row,
            pen_up_cmd="PENUP",
            forward_cmd="FORWARD 20",
            pen_down_cmd="PENDOWN",
            right_cmd="RIGHT 45",
        )
        cardinal_x, cardinal_y, cardinal_heading = run_cardinal_relative_turn_smoke(client, args.timeout)
        square_x, square_y, square_heading, square_top, square_right = run_repeat_square_smoke(client, args.timeout)
        setpos_x, setpos_y = run_setpos_smoke(client, args.timeout)
        editor_mode, editor_pixels_before, editor_pixels_after = run_procedure_editor_smoke(client, args.timeout)

        breakpoint_pc = None
        if args.target == "hardware" and not args.skip_debug_breakpoint:
            breakpoint_pc = run_debug_breakpoint_smoke(client, args.timeout)

        if args.review_delay > 0:
            time.sleep(args.review_delay)
        msg = (
            f"PASS NovaLogo DRAW: center_pixels={center_pixels} total_pixels={total_pixels}; "
            f"turn_fd=x{turn_x},y{turn_y},h{turn_heading},diag{diagonal_pixels},old{old_center_pixels}; "
            f"long_fd=x{long_x},y{long_y},h{long_heading},diag{long_diagonal_pixels},old{long_old_center_pixels}; "
            f"cardinal=x{cardinal_x},y{cardinal_y},h{cardinal_heading}; "
            f"square=x{square_x},y{square_y},h{square_heading},top{square_top},right{square_right}; "
            f"setpos=x{setpos_x},y{setpos_y}; "
            f"editor=mode${editor_mode:02X},pixels{editor_pixels_before}->{editor_pixels_after}"
        )
        if breakpoint_pc is not None:
            msg += f" breakpoint=${breakpoint_pc:04X}"
        print(msg, flush=True)
    finally:
        client.close()
        if args.target == "hardware" and not args.leave_boot_config:
            assert original_config is not None
            retry_sd_operation(
                "restore original boot config",
                args.timeout,
                lambda: put_json(args.host, "/sd/config/boot.json", original_config),
            )


def main() -> int:
    args = parse_args()
    run_smoke(args)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001 - hardware smoke should report concise failure.
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
