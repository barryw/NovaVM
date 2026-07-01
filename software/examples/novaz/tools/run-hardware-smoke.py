#!/usr/bin/env python3
"""Run NovaZ smoke scripts against live Nova hardware through NovaHost."""

from __future__ import annotations

import argparse
import json
import socket
import sys
import time
from pathlib import Path


DEFAULT_HOST = "192.168.1.65"
DEFAULT_PORT = 6503


class DebugClient:
    def __init__(self, host: str, port: int, timeout: float) -> None:
        self._host = host
        self._port = port
        self._timeout = timeout

    def command(self, command: str, **kwargs: object) -> dict[str, object]:
        payload = {"command": command}
        payload.update(kwargs)
        data = json.dumps(payload, separators=(",", ":")).encode("utf-8") + b"\n"

        with socket.create_connection((self._host, self._port), timeout=self._timeout) as sock:
            sock.settimeout(self._timeout)
            sock.sendall(data)
            response = b""
            while not response.endswith(b"\n"):
                chunk = sock.recv(65536)
                if not chunk:
                    break
                response += chunk

        if not response:
            raise RuntimeError(f"no response for debug command {command!r}")
        text = response.decode("utf-8", errors="replace").strip()
        result = json.loads(text)
        if not result.get("ok", False):
            raise RuntimeError(f"debug command {command!r} failed: {text}")
        return result


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--timeout", type=float, default=10.0)
    parser.add_argument("--key-delay", type=float, default=0.05)
    parser.add_argument("--turn-delay", type=float, default=1.25)
    parser.add_argument("--prompt-timeout", type=float, default=30.0)
    parser.add_argument("--script", action="append", type=Path, required=True)
    parser.add_argument("--expect-more", action="store_true")
    return parser.parse_args()


def load_script(path: Path) -> list[tuple[str, list[str]]]:
    commands: list[tuple[str, list[str]]] = []
    for line_number, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        command, _, expected_text = line.partition("=>")
        if not command.strip():
            raise ValueError(f"{path}:{line_number}: empty command")
        expected = [part.strip() for part in expected_text.split("&&") if part.strip()]
        commands.append((command.strip(), expected))
    return commands


def read_screen(client: DebugClient) -> tuple[list[str], int, int]:
    response = client.command("read_screen")
    lines = response.get("lines")
    if not isinstance(lines, list):
        raise RuntimeError(f"read_screen returned no lines: {response}")
    return [str(line) for line in lines], int(response["cursor_x"]), int(response["cursor_y"])


def is_ready_prompt(lines: list[str], cursor_x: int, cursor_y: int) -> bool:
    if cursor_y < 0 or cursor_y >= len(lines):
        return False
    line = lines[cursor_y]
    return cursor_x == 1 and len(line) > 0 and line[0] == ">" and all(ch == " " for ch in line[1:])


def wait_ready_prompt(client: DebugClient, timeout: float) -> tuple[list[str], int, int, int]:
    deadline = time.monotonic() + timeout
    last: tuple[list[str], int, int] | None = None
    more_prompts = 0
    while time.monotonic() < deadline:
        last = read_screen(client)
        if any("[ MORE ]" in line for line in last[0]):
            more_prompts += 1
            client.command("send_key", key="ENTER")
            time.sleep(0.3)
            continue
        if is_ready_prompt(*last):
            lines, cursor_x, cursor_y = last
            return lines, cursor_x, cursor_y, more_prompts
        time.sleep(0.2)
    if last is None:
        raise TimeoutError("timed out before first screen read")
    lines, cursor_x, cursor_y = last
    raise TimeoutError(
        f"timed out waiting for Zork prompt at cursor {cursor_x},{cursor_y}\n"
        + "\n".join(lines)
    )


def send_line(client: DebugClient, text: str, key_delay: float) -> None:
    for ch in text:
        client.command("send_key", key=ch)
        time.sleep(key_delay)
    client.command("send_key", key="ENTER")


def extract_command_transcript(lines: list[str], command: str) -> str:
    echo = ">" + command
    for index in range(len(lines) - 1, -1, -1):
        if lines[index].rstrip().lower() == echo.lower():
            return "\n".join(lines[index:])
    return "\n".join(lines)


def visible(lines: list[str]) -> str:
    return "\n".join(line.rstrip() for line in lines if line.strip())


def normalize_whitespace(text: str) -> str:
    return " ".join(text.split())


def main() -> int:
    args = parse_args()
    client = DebugClient(args.host, args.port, args.timeout)

    total_more_prompts = 0
    _, _, _, seen_more = wait_ready_prompt(client, args.prompt_timeout)
    total_more_prompts += seen_more
    for script in args.script:
        print(f"== {script} ==")
        for command, expected in load_script(script):
            print(f"> {command}", flush=True)
            send_line(client, command, args.key_delay)
            lines, cursor_x, cursor_y, seen_more = wait_ready_prompt(client, args.prompt_timeout)
            total_more_prompts += seen_more
            transcript = extract_command_transcript(lines, command)
            for marker in expected:
                if marker not in transcript and normalize_whitespace(marker) not in normalize_whitespace(transcript):
                    print(transcript, file=sys.stderr)
                    raise RuntimeError(f"expected output after {command!r} to contain {marker!r}")
            print(f"ok cursor={cursor_x},{cursor_y}")
            time.sleep(args.turn_delay)

    if args.expect_more and total_more_prompts == 0:
        lines, _, _ = read_screen(client)
        print(visible(lines), file=sys.stderr)
        raise RuntimeError("expected at least one [ MORE ] prompt")

    lines, _, _ = read_screen(client)
    print(f"Hardware NovaZ smoke passed. morePrompts={total_more_prompts}")
    print(visible(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
