#!/usr/bin/env python3
"""Shared NovaHost HTTP/debug client utilities.

This module is intentionally small and dependency-free so hardware tools can
all use the same JSON framing, defaults, timeout behavior, and error handling.
"""

from __future__ import annotations

import json
import os
import shutil
import socket
import subprocess
import tempfile
import time
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any


DEFAULT_HOST = os.environ.get("NOVAHOST", "192.168.1.65")
DEFAULT_DEBUG_PORT = int(os.environ.get("NOVAHOST_PORT", "6503"))
DEFAULT_HTTP_PORT = int(os.environ.get("HTTP_PORT", "80"))
DEFAULT_TIMEOUT = float(os.environ.get("DEBUG_RESPONSE_TIMEOUT", "15"))


class NovaHostError(RuntimeError):
    """Raised when NovaHost transport or protocol handling fails."""


# ---------------------------------------------------------------------------
# NovaHost management over TCP via the `nova` CLI.
#
# NovaHost is TCP-only: the old HTTP file/drive/device endpoints (/sd, /drives,
# /reboot, /health) were removed. File/drive/device-management operations now go
# through the `nova` CLI (management TCP), the same path used interactively.
# These helpers shell out to that binary so the hardware test runners do not
# depend on the dead HTTP API.
# ---------------------------------------------------------------------------

_NOVA_CLI_CACHE: str | None = None


def find_nova_cli() -> str:
    """Locate the published `nova` CLI binary (TCP management client)."""
    global _NOVA_CLI_CACHE
    if _NOVA_CLI_CACHE:
        return _NOVA_CLI_CACHE
    override = os.environ.get("NOVA_CLI")
    if override and Path(override).is_file():
        _NOVA_CLI_CACHE = override
        return override
    repo = Path(__file__).resolve().parents[1]
    patterns = (
        "e6502.Nova/bin/Release/net*/*/native/nova",
        "e6502.Nova/bin/*/net*/*/native/nova",
        "e6502.Nova/bin/*/net*/nova",
    )
    for pattern in patterns:
        for cand in sorted(repo.glob(pattern)):
            if cand.is_file() and os.access(cand, os.X_OK):
                _NOVA_CLI_CACHE = str(cand)
                return _NOVA_CLI_CACHE
    found = shutil.which("nova")
    if found:
        _NOVA_CLI_CACHE = found
        return found
    raise NovaHostError(
        "nova CLI binary not found; build it "
        "(`dotnet publish e6502.Nova -c Release`) or set NOVA_CLI / add `nova` to PATH"
    )


def nova_cli(host: str, *args: str, timeout: float = 180.0, check: bool = True) -> str:
    """Run the nova CLI against `host` over TCP and return stdout."""
    cmd = [find_nova_cli(), "--remote", host, *args]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired as exc:
        raise NovaHostError(f"nova {' '.join(args)} timed out after {timeout}s") from exc
    if check and proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"rc={proc.returncode}"
        raise NovaHostError(f"nova {' '.join(args)} failed: {detail}")
    return proc.stdout


def _sd_path(path: str) -> str:
    """Normalize an HTTP-era `/sd/...` path to an SD-root-relative nova path."""
    if path.startswith("/sd/"):
        path = path[3:]
    elif path == "/sd":
        path = "/"
    if not path.startswith("/"):
        path = "/" + path
    return path


def cli_upload(host: str, src: str | os.PathLike[str], dest: str, timeout: float = 180.0) -> None:
    nova_cli(host, "put", str(src), _sd_path(dest), timeout=timeout)


def cli_download_bytes(host: str, remote: str, timeout: float = 120.0) -> bytes:
    with tempfile.TemporaryDirectory() as tmp:
        local = Path(tmp) / "download.bin"
        nova_cli(host, "get", _sd_path(remote), str(local), timeout=timeout)
        return local.read_bytes()


def cli_drive_mount(host: str, slot: str, path: str, timeout: float = 60.0) -> None:
    nova_cli(host, "drive", "mount", slot, _sd_path(path), timeout=timeout)


def cli_drive_unmount(host: str, slot: str, timeout: float = 60.0) -> None:
    # Unmounting an already-empty slot must not fail the run.
    nova_cli(host, "drive", "unmount", slot, timeout=timeout, check=False)


def cli_reboot(host: str, timeout: float = 60.0) -> None:
    nova_cli(host, "device", "reboot", timeout=timeout, check=False)


def cli_health(host: str, timeout: float = 8.0) -> dict[str, Any]:
    data = json.loads(nova_cli(host, "device", "status", timeout=timeout))
    health = data.get("health", data)
    return health if isinstance(health, dict) else data


def cli_wait_for_health(host: str, timeout: float) -> None:
    deadline = time.monotonic() + timeout
    last = ""
    while time.monotonic() < deadline:
        try:
            health = cli_health(host, timeout=6.0)
            if health.get("ok") and health.get("fpgaBridgeAvailable", True):
                return
            last = str(health)
        except Exception as exc:  # noqa: BLE001 - polling tolerates reboot churn.
            last = str(exc)
        time.sleep(1.0)
    raise TimeoutError(f"NovaHost health did not recover within {timeout}s: {last}")


class NovaHostClient:
    def __init__(
        self,
        host: str | None = None,
        port: int | None = None,
        http_port: int | None = None,
        timeout: float | None = None,
    ) -> None:
        self.host = host or DEFAULT_HOST
        self.port = int(port if port is not None else DEFAULT_DEBUG_PORT)
        self.http_port = int(http_port if http_port is not None else DEFAULT_HTTP_PORT)
        self.timeout = float(timeout if timeout is not None else DEFAULT_TIMEOUT)
        self._sock: socket.socket | None = None
        self._rx = b""

    def __enter__(self) -> "NovaHostClient":
        self.connect()
        return self

    def __exit__(self, exc_type: object, exc: object, tb: object) -> None:
        self.close()

    def connect(self) -> None:
        if self._sock is not None:
            return
        self._sock = socket.create_connection((self.host, self.port), timeout=self.timeout)
        self._sock.settimeout(self.timeout)

    def close(self) -> None:
        if self._sock is None:
            return
        self._sock.close()
        self._sock = None
        self._rx = b""

    def command(self, command: str, require_ok: bool = False, **kwargs: Any) -> dict[str, Any]:
        payload: dict[str, Any] = {"command": command}
        payload.update(kwargs)
        return self.raw(payload, require_ok=require_ok)

    def raw(self, payload: dict[str, Any], require_ok: bool = False) -> dict[str, Any]:
        if "command" not in payload:
            raise NovaHostError("debug payload is missing 'command'")

        self.connect()
        assert self._sock is not None
        line = json.dumps(payload, separators=(",", ":")).encode("utf-8") + b"\n"
        self._sock.sendall(line)

        while b"\n" not in self._rx:
            chunk = self._sock.recv(65536)
            if not chunk:
                break
            self._rx += chunk

        if b"\n" not in self._rx:
            raise NovaHostError(f"no newline response for debug command {payload['command']!r}")

        raw_line, self._rx = self._rx.split(b"\n", 1)
        text = raw_line.decode("utf-8", errors="replace").strip()
        if not text:
            raise NovaHostError(f"empty response for debug command {payload['command']!r}")

        try:
            response = json.loads(text)
        except json.JSONDecodeError as exc:
            raise NovaHostError(f"invalid JSON from {payload['command']!r}: {text}") from exc

        if require_ok and not response.get("ok", False):
            raise NovaHostError(f"{payload['command']}: {response.get('error', response)}")
        return response

    def http_get_text(self, path: str, timeout: float | None = None) -> str:
        url = self._http_url(path)
        with urllib.request.urlopen(url, timeout=timeout or self.timeout) as resp:
            return resp.read().decode("utf-8", errors="replace")

    def http_get_json(self, path: str, timeout: float | None = None) -> dict[str, Any]:
        text = self.http_get_text(path, timeout=timeout)
        try:
            return json.loads(text)
        except json.JSONDecodeError as exc:
            raise NovaHostError(f"invalid JSON from {path}: {text}") from exc

    def http_post_json(self, path: str, payload: dict[str, Any] | None = None,
                       timeout: float | None = None) -> dict[str, Any]:
        url = self._http_url(path)
        data = b"" if payload is None else json.dumps(payload, separators=(",", ":")).encode("utf-8")
        headers = {"Content-Type": "application/json"} if payload is not None else {}
        req = urllib.request.Request(url, data=data, headers=headers, method="POST")
        try:
            with urllib.request.urlopen(req, timeout=timeout or self.timeout) as resp:
                text = resp.read().decode("utf-8", errors="replace")
        except urllib.error.HTTPError as exc:
            text = exc.read().decode("utf-8", errors="replace")
            detail = text.strip() or exc.reason
            raise NovaHostError(f"POST {path}: HTTP {exc.code} {detail}") from exc
        try:
            return json.loads(text) if text.strip() else {"ok": True}
        except json.JSONDecodeError as exc:
            raise NovaHostError(f"invalid JSON from {path}: {text}") from exc

    def try_http_get_json(self, path: str, timeout: float | None = None) -> dict[str, Any]:
        url = self._http_url(path)
        try:
            return {"ok": True, "url": url, "json": self.http_get_json(path, timeout=timeout)}
        except (OSError, urllib.error.URLError, NovaHostError) as exc:
            return {"ok": False, "url": url, "error": str(exc)}

    def health(self) -> dict[str, Any]:
        return self.http_get_json("/health")

    def sd_status(self) -> dict[str, Any]:
        return self.http_get_json("/sd-status")

    def boot_status(self) -> dict[str, Any]:
        return self.command("boot_status", require_ok=True)

    def state(self) -> dict[str, Any]:
        return self.command("dbg_state", require_ok=True)

    def pause(self) -> dict[str, Any]:
        return self.command("dbg_pause", require_ok=True)

    def resume(self) -> dict[str, Any]:
        return self.command("dbg_resume", require_ok=True)

    def step(self) -> dict[str, Any]:
        return self.command("dbg_step", require_ok=True)

    def break_set(self, address: int, slot: int | None = None, enabled: bool = True) -> dict[str, Any]:
        kwargs: dict[str, Any] = {"address": address & 0xFFFF, "enabled": 1 if enabled else 0}
        if slot is not None:
            kwargs["slot"] = slot
        return self.command("dbg_break_set", require_ok=True, **kwargs)

    def break_clear(self, slot: int | None = None, address: int | None = None) -> dict[str, Any]:
        kwargs: dict[str, Any] = {}
        if slot is not None:
            kwargs["slot"] = slot
        if address is not None:
            kwargs["address"] = address & 0xFFFF
        return self.command("dbg_break_clear", require_ok=True, **kwargs)

    def break_clear_all(self) -> dict[str, Any]:
        return self.command("dbg_break_clear_all", require_ok=True)

    def break_list(self) -> dict[str, Any]:
        return self.command("dbg_break_list", require_ok=True)

    def trace(self, count: int = 16, hex_mode: bool = True) -> dict[str, Any]:
        kwargs: dict[str, Any] = {"count": count}
        if hex_mode:
            kwargs["format"] = "hex"
        return self.command("dbg_trace", require_ok=True, **kwargs)

    def cold_start(self, wait_ready: bool = True, text: str | None = None) -> dict[str, Any]:
        kwargs: dict[str, Any] = {"wait_ready": 1 if wait_ready else 0}
        if text:
            kwargs["text"] = text
        return self.command("cold_start", require_ok=True, **kwargs)

    def vm_reset(self, wait_ready: bool = False, text: str | None = None) -> dict[str, Any]:
        if wait_ready or text:
            raise NovaHostError("vm_reset is REST-only; call wait_ready separately if needed")
        return self.http_post_json("/vm-reset")

    def reload_rom(self) -> dict[str, Any]:
        return self.command("reload_rom", require_ok=True)

    def wait_ready(self, text: str = "Ready", timeout_ms: int = 5000) -> dict[str, Any]:
        return self.command("wait_ready", require_ok=True, text=text, timeout_ms=timeout_ms)

    def send_key(self, key: str) -> dict[str, Any]:
        return self.command("send_key", require_ok=True, key=key)

    def type_text(self, text: str) -> dict[str, Any]:
        return self.command("type_text", require_ok=True, text=text)

    def basic_line(self, line: str) -> dict[str, Any]:
        if not line.endswith("\r"):
            line += "\r"
        return self.type_text(line)

    def mute_sid(self) -> dict[str, Any]:
        return self.basic_line("FOR I=54272 TO 54335:POKE I,0:NEXT I")

    def read_screen(self) -> dict[str, Any]:
        return self.command("read_screen", require_ok=True)

    def read_line(self, row: int) -> dict[str, Any]:
        return self.command("read_line", require_ok=True, row=row)

    def get_cursor(self) -> dict[str, Any]:
        return self.command("get_cursor", require_ok=True)

    def peek(self, address: int) -> int:
        response = self.command("peek", require_ok=True, address=address)
        return int(response.get("value", 0)) & 0xFF

    def poke(self, address: int, value: int) -> dict[str, Any]:
        return self.command("poke", require_ok=True, address=address, value=value & 0xFF)

    def peek_block(self, address: int, count: int) -> list[int]:
        response = self.command("peek_block", require_ok=True, address=address & 0xFFFF, count=count)
        values = response.get("values")
        if not isinstance(values, list):
            raise NovaHostError(f"peek_block returned no values for 0x{address & 0xFFFF:04X}")
        return [int(value) & 0xFF for value in values]

    def read_range_bytewise(self, start: int, length: int) -> dict[str, Any]:
        values: list[int | None] = []
        errors: list[dict[str, Any]] = []
        for offset in range(length):
            address = (start + offset) & 0xFFFF
            try:
                values.append(self.peek(address))
            except Exception as exc:  # noqa: BLE001 - diagnostics should keep going.
                values.append(None)
                errors.append({"address": address, "error": str(exc)})
        return {"start": start & 0xFFFF, "length": length, "values": values, "errors": errors}

    def read_vram(self, space: int, address: int, length: int) -> dict[str, Any]:
        response = self.command(
            "read_vram",
            require_ok=True,
            space=space,
            address=address,
            length=length,
        )
        if "values" not in response and "data" in response:
            response["values"] = response["data"]
        return response

    def fill_vram(self, space: int, address: int, value: int, length: int) -> dict[str, Any]:
        return self.command(
            "fill_vram",
            require_ok=True,
            space=space,
            address=address,
            value=value & 0xFF,
            length=length,
        )

    def _http_url(self, path: str) -> str:
        if not path.startswith("/"):
            path = "/" + path
        return f"http://{self.host}:{self.http_port}{path}"


def parse_int(value: str) -> int:
    text = value.strip()
    if text.startswith("$"):
        return int(text[1:], 16)
    return int(text, 0)
