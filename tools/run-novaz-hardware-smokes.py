#!/usr/bin/env python3
"""Run NovaZ smoke scripts against live Nova hardware."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
import urllib.error
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO_ROOT / "tools"))

from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError  # noqa: E402


SCREEN_COLS = 80
SCREEN_ROWS = 50
VGC_PLANE_COLOR = 2


@dataclass(frozen=True)
class SmokeCommand:
    text: str
    expected: tuple[str, ...] = ()
    raw: bool = False
    wait_seconds: float | None = None
    expect_stop: int | None = None
    wait_for_prompt: bool = True


@dataclass(frozen=True)
class TextColorExpectation:
    text: str
    color: int


@dataclass(frozen=True)
class ScreenRowExpectation:
    row: int
    text: str


@dataclass(frozen=True)
class ForbiddenTextBelowExpectation:
    first_row: int
    text: str


@dataclass(frozen=True)
class HardwareSmoke:
    name: str
    image: Path
    script: Path | None = None
    commands: tuple[SmokeCommand, ...] = ()
    expected_screens: tuple[str, ...] = ()
    expected_text_colors: tuple[TextColorExpectation, ...] = ()
    screen_only: bool = False
    expect_more: bool = False
    expect_time_status: bool = False
    no_status_line: bool = False
    header_expected_any: tuple[str, ...] = ()
    expected_rows: tuple[ScreenRowExpectation, ...] = ()
    forbidden_text_below: tuple[ForbiddenTextBelowExpectation, ...] = ()
    mount_path: str = field(init=False)

    def __post_init__(self) -> None:
        object.__setattr__(self, "mount_path", f"/novaz-smoke/{self.name}.ndi")


def repo_path(path: str) -> Path:
    return REPO_ROOT / path


SMOKES: tuple[HardwareSmoke, ...] = (
    HardwareSmoke(
        name="z3-spec",
        image=repo_path("examples/novaz/dist/z3-spec/fd0.ndi"),
        commands=(SmokeCommand("take red,key", ("z3 input ok",)),),
        expected_screens=(
            "addrpaddrA-12",
            "abbr@",
            "upper window ok",
            "buffer off ok",
            "z3 window ok",
            "z3 spec ok",
        ),
    ),
    HardwareSmoke(
        name="z4-styles",
        image=repo_path("examples/novaz/dist/z4-styles/fd0.ndi"),
        screen_only=True,
        expected_screens=("style fixture done",),
        expected_text_colors=(
            TextColorExpectation("normal text uses color 12", 0x0C),
            TextColorExpectation("bold text uses white", 0x0F),
            TextColorExpectation("bold phrase survives reset", 0x0F),
            TextColorExpectation("reverse text uses color c0", 0xC0),
            TextColorExpectation("bold reverse uses color f0", 0xF0),
            TextColorExpectation("italic request is ignored", 0x0C),
            TextColorExpectation("fixed request is ignored", 0x0C),
        ),
    ),
    HardwareSmoke(
        name="z5-spec",
        image=repo_path("examples/novaz/dist/z5-spec/fd0.ndi"),
        commands=(SmokeCommand("take silver,key", ("z5 input ok",)),),
        expected_screens=("z5 spec ok",),
        no_status_line=True,
    ),
    HardwareSmoke(
        name="zork-i",
        image=repo_path("examples/novaz/dist/zork-i/fd0.ndi"),
        script=repo_path("examples/novaz/projects/zork-i/smoke.txt"),
        expect_more=True,
    ),
    HardwareSmoke(
        name="zork-i-torture",
        image=repo_path("examples/novaz/dist/zork-i/fd0.ndi"),
        script=repo_path("examples/novaz/tests/zork-i-torture-smoke.txt"),
    ),
    HardwareSmoke(
        name="zork-i-longplay",
        image=repo_path("examples/novaz/dist/zork-i/fd0.ndi"),
        script=repo_path("examples/novaz/projects/zork-i/longplay.txt"),
    ),
    HardwareSmoke(
        name="zork-ii",
        image=repo_path("examples/novaz/dist/zork-ii/fd0.ndi"),
        script=repo_path("examples/novaz/projects/zork-ii/smoke.txt"),
    ),
    HardwareSmoke(
        name="zork-iii",
        image=repo_path("examples/novaz/dist/zork-iii/fd0.ndi"),
        script=repo_path("examples/novaz/projects/zork-iii/smoke.txt"),
        expect_more=True,
    ),
    HardwareSmoke(
        name="deadline",
        image=repo_path("examples/novaz/dist/deadline/fd0.ndi"),
        script=repo_path("examples/novaz/projects/deadline/smoke.txt"),
        expect_time_status=True,
    ),
    HardwareSmoke(
        name="deadline-longplay",
        image=repo_path("examples/novaz/dist/deadline/fd0.ndi"),
        script=repo_path("examples/novaz/projects/deadline/longplay.txt"),
        expect_time_status=True,
    ),
    HardwareSmoke(
        name="amfv",
        image=repo_path("examples/novaz/dist/amfv/fd0.ndi"),
        script=repo_path("examples/novaz/projects/amfv/smoke.txt"),
        no_status_line=True,
        header_expected_any=("Mode:",),
        expected_rows=(
            ScreenRowExpectation(0, "Mode:"),
            ScreenRowExpectation(1, "Location:"),
        ),
        forbidden_text_below=(
            ForbiddenTextBelowExpectation(2, "Mode:"),
            ForbiddenTextBelowExpectation(2, "Location:"),
        ),
    ),
    HardwareSmoke(
        name="amfv-longplay",
        image=repo_path("examples/novaz/dist/amfv/fd0.ndi"),
        script=repo_path("examples/novaz/projects/amfv/longplay.txt"),
        no_status_line=True,
        header_expected_any=("Mode:",),
        expected_rows=(
            ScreenRowExpectation(0, "Mode:"),
            ScreenRowExpectation(1, "Location:"),
        ),
        forbidden_text_below=(
            ForbiddenTextBelowExpectation(2, "Mode:"),
            ForbiddenTextBelowExpectation(2, "Location:"),
        ),
    ),
    HardwareSmoke(
        name="trinity",
        image=repo_path("examples/novaz/dist/trinity/fd0.ndi"),
        script=repo_path("examples/novaz/projects/trinity/smoke.txt"),
        no_status_line=True,
        header_expected_any=("Palace Gate", "Broad Walk"),
    ),
    HardwareSmoke(
        name="trinity-longplay",
        image=repo_path("examples/novaz/dist/trinity/fd0.ndi"),
        script=repo_path("examples/novaz/projects/trinity/longplay.txt"),
        no_status_line=True,
        header_expected_any=("Palace Gate", "Broad Walk", "Inverness Terrace"),
    ),
    HardwareSmoke(
        name="hhgg",
        image=repo_path("examples/novaz/dist/hhgg/fd0.ndi"),
        script=repo_path("examples/novaz/projects/hhgg/smoke.txt"),
        no_status_line=True,
        header_expected_any=("Darkness", "Bedroom"),
    ),
    HardwareSmoke(
        name="hhgg-longplay",
        image=repo_path("examples/novaz/dist/hhgg/fd0.ndi"),
        script=repo_path("examples/novaz/projects/hhgg/longplay.txt"),
        no_status_line=True,
        header_expected_any=("Darkness", "Bedroom"),
    ),
    HardwareSmoke(
        name="beyond-zork",
        image=repo_path("examples/novaz/dist/beyond-zork/fd0.ndi"),
        script=repo_path("examples/novaz/projects/beyond-zork/smoke.txt"),
        no_status_line=True,
        header_expected_any=("Hilltop",),
    ),
    HardwareSmoke(
        name="border-zone",
        image=repo_path("examples/novaz/dist/border-zone/fd0.ndi"),
        script=repo_path("examples/novaz/projects/border-zone/smoke.txt"),
        no_status_line=True,
        header_expected_any=("Your Compartment", "Outside Your Compartment"),
    ),
    HardwareSmoke(
        name="border-zone-longplay",
        image=repo_path("examples/novaz/dist/border-zone/fd0.ndi"),
        script=repo_path("examples/novaz/projects/border-zone/longplay.txt"),
        no_status_line=True,
        header_expected_any=("Your Compartment", "Outside Your Compartment"),
    ),
    HardwareSmoke(
        name="sherlock",
        image=repo_path("examples/novaz/dist/sherlock/fd0.ndi"),
        script=repo_path("examples/novaz/projects/sherlock/smoke.txt"),
        no_status_line=True,
        header_expected_any=("221-B Baker Street",),
    ),
    HardwareSmoke(
        name="sherlock-longplay",
        image=repo_path("examples/novaz/dist/sherlock/fd0.ndi"),
        script=repo_path("examples/novaz/projects/sherlock/longplay.txt"),
        no_status_line=True,
        header_expected_any=("221-B Baker Street",),
    ),
    HardwareSmoke(
        name="ztuu",
        image=repo_path("examples/novaz/dist/ztuu/fd0.ndi"),
        script=repo_path("examples/novaz/projects/ztuu/smoke.txt"),
    ),
    HardwareSmoke(
        name="ztuu-longplay",
        image=repo_path("examples/novaz/dist/ztuu/fd0.ndi"),
        script=repo_path("examples/novaz/projects/ztuu/longplay.txt"),
    ),
)


def parse_command_spec(spec: str) -> SmokeCommand:
    command, sep, expected = spec.partition("=>")
    command = command.strip()
    if not command:
        raise ValueError(f"empty command in smoke spec: {spec!r}")
    wait_for_prompt = True
    if command.startswith("!"):
        wait_for_prompt = False
        command = command[1:].lstrip()
    raw = False
    raw_prefix = ".raw "
    if command.lower().startswith(raw_prefix):
        raw = True
        command = command[len(raw_prefix):]
        if not command:
            raise ValueError(f"empty raw input in smoke spec: {spec!r}")
    wait_prefix = ".wait "
    wait_seconds = None
    if command.lower().startswith(wait_prefix):
        wait_seconds = float(command[len(wait_prefix):].strip())
        wait_for_prompt = False
        command = command[len(wait_prefix):].strip()
    expect_stop = None
    expect_stop_prefix = ".expect-stop "
    if command.lower().startswith(expect_stop_prefix):
        expect_stop = int(command[len(expect_stop_prefix):].strip(), 0)
        wait_for_prompt = False
        command = command[len(expect_stop_prefix):].strip()
    if not sep:
        return SmokeCommand(
            command,
            raw=raw,
            wait_seconds=wait_seconds,
            expect_stop=expect_stop,
            wait_for_prompt=wait_for_prompt,
        )
    return SmokeCommand(
        command,
        tuple(part.strip() for part in expected.split("&&") if part.strip()),
        raw=raw,
        wait_seconds=wait_seconds,
        expect_stop=expect_stop,
        wait_for_prompt=wait_for_prompt,
    )


def load_commands(smoke: HardwareSmoke) -> list[SmokeCommand]:
    commands = list(smoke.commands)
    if smoke.script is None:
        return commands
    for line in smoke.script.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        commands.append(parse_command_spec(line))
    return commands


def try_read_runtime_symbol(symbol_name: str) -> int | None:
    candidates = [
        REPO_ROOT / "examples/novaz/build/runtime.sym",
        REPO_ROOT / "build/runtime.sym",
    ]
    for path in candidates:
        if not path.is_file():
            continue
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            parts = line.split()
            if len(parts) < 3:
                continue
            if parts[2].lstrip(".") != symbol_name:
                continue
            return int(parts[1], 16)
    return None


def normalize_whitespace(text: str) -> str:
    return " ".join(text.split())


def contains_normalized(screen: str, expected: str) -> bool:
    return expected in screen or normalize_whitespace(expected) in normalize_whitespace(screen)


def require_contains(screen: str, expected: str, context: str) -> None:
    if contains_normalized(screen, expected):
        return
    raise RuntimeError(f"expected {context} to contain {expected!r}\n{screen}")


def extract_command_transcript(screen: str, command: str) -> str:
    lines = screen.splitlines()
    echo = ">" + command
    for index in range(len(lines) - 1, -1, -1):
        if lines[index].rstrip().lower() == echo.lower():
            return "\n".join(lines[index:])
    return screen


def screen_text(snapshot: dict[str, object]) -> str:
    return "\n".join(str(line) for line in snapshot.get("lines", []))


def has_trailing_bare_prompt(screen: str) -> bool:
    for line in reversed(screen.splitlines()[:SCREEN_ROWS]):
        stripped = line.rstrip()
        if not stripped:
            continue
        return stripped.startswith(">") and stripped[1:].strip() == ""
    return False


def has_status_line(screen: str) -> bool:
    first_line = screen.splitlines()[0] if screen.splitlines() else ""
    score_at = first_line.find("Score:")
    moves_at = first_line.find("Moves:")
    time_at = first_line.find("Time:")
    has_score = score_at > 0 and moves_at > score_at and first_line[:score_at].strip()
    has_time = time_at > 0 and first_line[:time_at].strip()
    return bool(has_score or has_time)


def require_status_line(screen: str) -> None:
    if not has_status_line(screen):
        raise RuntimeError(f"expected status line with location plus score/moves or time\n{screen}")


def has_time_status_line(screen: str) -> bool:
    first_line = screen.splitlines()[0] if screen.splitlines() else ""
    time_at = first_line.find("Time:")
    return time_at > 0 and bool(first_line[:time_at].strip())


def require_time_status_line(screen: str) -> None:
    if not has_time_status_line(screen):
        raise RuntimeError(f"expected time-game status line\n{screen}")


def require_header(screen: str, expected_values: tuple[str, ...]) -> None:
    if not expected_values:
        return
    first_line = screen.splitlines()[0] if screen.splitlines() else ""
    if any(expected in first_line for expected in expected_values):
        return
    expected = " or ".join(repr(value) for value in expected_values)
    raise RuntimeError(f"expected first screen row to contain {expected}; got {first_line!r}\n{screen}")


def require_layout(screen: str, smoke: HardwareSmoke) -> None:
    lines = screen.splitlines()
    for expected in smoke.expected_rows:
        if expected.row >= len(lines):
            raise RuntimeError(
                f"expected row {expected.row} to contain {expected.text!r}; "
                f"screen has only {len(lines)} rows\n{screen}"
            )
        if contains_normalized(lines[expected.row], expected.text):
            continue
        raise RuntimeError(
            f"expected row {expected.row} to contain {expected.text!r}; "
            f"got {lines[expected.row]!r}\n{screen}"
        )

    for forbidden in smoke.forbidden_text_below:
        for row, line in enumerate(lines[forbidden.first_row:], start=forbidden.first_row):
            if not contains_normalized(line, forbidden.text):
                continue
            raise RuntimeError(
                f"found forbidden header/status text {forbidden.text!r} on row {row}; "
                f"expected it to stay above row {forbidden.first_row}\n{screen}"
            )


def find_text(screen: str, text: str) -> tuple[int, int] | None:
    for row, line in enumerate(screen.splitlines()):
        col = line.find(text)
        if col >= 0:
            return row, col
    return None


def http_request(host: str, method: str, path: str, data: bytes | None = None,
                 content_type: str = "application/octet-stream",
                 timeout: float = 60.0) -> bytes:
    url = f"http://{host}{path}"
    headers = {}
    if data is not None:
        headers["Content-Type"] = content_type
    request = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            return response.read()
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"{method} {url} failed: HTTP {exc.code}: {body}") from exc


def put_file(host: str, src: Path, dest: str) -> None:
    http_request(host, "PUT", f"/sd{dest}", src.read_bytes(), timeout=120.0)


def post_json(host: str, path: str, payload: dict[str, object] | None = None) -> dict[str, object]:
    data = json.dumps(payload or {}, separators=(",", ":")).encode("utf-8")
    body = http_request(host, "POST", path, data, content_type="application/json")
    return json.loads(body.decode("utf-8"))


def wait_for_health(host: str, timeout: float) -> None:
    deadline = time.monotonic() + timeout
    last_error = ""
    while time.monotonic() < deadline:
        try:
            body = http_request(host, "GET", "/health", timeout=2.0)
            health = json.loads(body.decode("utf-8"))
            if health.get("ok") and health.get("fpgaBridgeAvailable", True):
                return
            last_error = str(health)
        except Exception as exc:  # noqa: BLE001 - polling should tolerate reboot transport churn.
            last_error = str(exc)
        time.sleep(0.5)
    raise TimeoutError(f"NovaHost health did not recover within {timeout}s: {last_error}")


def build_images(selected: list[HardwareSmoke]) -> None:
    targets = []
    if any(s.name in {"z3-spec", "z4-styles", "z5-spec"} for s in selected):
        targets.extend(["test-z3-spec", "test-z4-styles", "test-z5-spec"])
    smoke_names = {s.name for s in selected}
    if any(name in {"zork-i", "zork-i-torture", "zork-ii", "zork-iii", "deadline", "amfv", "trinity", "hhgg", "beyond-zork", "border-zone", "sherlock", "ztuu"} for name in smoke_names):
        targets.append("test-infocom-smokes")
    if any(name in {"zork-i-longplay", "deadline-longplay", "amfv-longplay", "trinity-longplay", "hhgg-longplay", "border-zone-longplay", "sherlock-longplay", "ztuu-longplay"} for name in smoke_names):
        targets.append("test-infocom-longplays")
    if not targets:
        return
    subprocess.run(["make", "-C", str(REPO_ROOT / "examples/novaz"), *targets], check=True)


def wait_for_screen(
    client: NovaHostClient,
    expected: tuple[str, ...],
    *,
    timeout: float,
    more_counter: list[int],
    want_prompt: bool = False,
) -> dict[str, object]:
    deadline = time.monotonic() + timeout
    last_screen = ""
    while time.monotonic() < deadline:
        try:
            snapshot = client.read_screen()
        except (NovaHostError, OSError, TimeoutError):
            client.close()
            time.sleep(0.25)
            continue

        last_screen = screen_text(snapshot)
        if "UNSUPPORTED Z-OPCODE" in last_screen:
            raise RuntimeError(f"NovaZ hit an unsupported opcode\n{last_screen}")
        if "[ MORE ]" in last_screen:
            more_counter[0] += 1
            client.send_key("\r")
            time.sleep(0.2)
            continue
        has_expected = all(contains_normalized(last_screen, item) for item in expected)
        has_prompt = not want_prompt or has_trailing_bare_prompt(last_screen)
        if has_expected and has_prompt:
            time.sleep(0.25)
            try:
                settled = client.read_screen()
            except (NovaHostError, OSError, TimeoutError):
                client.close()
                time.sleep(0.25)
                continue
            settled_screen = screen_text(settled)
            if "[ MORE ]" in settled_screen:
                more_counter[0] += 1
                client.send_key("\r")
                time.sleep(0.2)
                continue
            settled_expected = all(contains_normalized(settled_screen, item) for item in expected)
            settled_prompt = not want_prompt or has_trailing_bare_prompt(settled_screen)
            if settled_expected and settled_prompt:
                return settled
            if handle_startup_prompt(client, settled_screen):
                time.sleep(0.5)
                continue
        if handle_startup_prompt(client, last_screen):
            time.sleep(0.5)
            continue
        time.sleep(0.25)

    raise TimeoutError(f"timed out waiting for screen content {expected}\n{last_screen}")


def handle_startup_prompt(client: NovaHostClient, screen: str) -> bool:
    lower = screen.lower()
    if has_trailing_bare_prompt(screen):
        return False
    if "hit any key" in lower or "press any key" in lower:
        client.send_key(" ")
        time.sleep(0.5)
        return True
    if "is this a vt220" in lower or "please type yes or no" in lower:
        client.type_text("NO\r")
        return True
    if "type restore" in lower and "start the game" in lower:
        client.type_text("N\r")
        return True
    if "type begin, restore or quit" in lower:
        client.type_text("BEGIN\r")
        return True
    if "which chapter would you like" in lower:
        client.type_text("1\r")
        return True
    if "beyond science" in lower or "zork is a registered trademark" in lower:
        client.send_key(" ")
        time.sleep(0.5)
        return True
    if "type [return] to continue" in lower:
        client.send_key("\r")
        time.sleep(0.5)
        return True
    return False


def wait_for_status_snapshot(
    client: NovaHostClient,
    screen: str,
    *,
    expect_time_status: bool,
    timeout: float = 2.0,
) -> str:
    deadline = time.monotonic() + timeout
    last_screen = screen
    while time.monotonic() < deadline:
        if has_status_line(last_screen) and (not expect_time_status or has_time_status_line(last_screen)):
            return last_screen
        time.sleep(0.1)
        last_screen = screen_text(client.read_screen())
    return last_screen


def require_text_color(client: NovaHostClient, screen: str, expected: TextColorExpectation) -> None:
    pos = find_text(screen, expected.text)
    if pos is None:
        raise RuntimeError(f"expected screen to contain text for color check {expected.text!r}\n{screen}")

    row, col = pos
    response = client.read_vram(VGC_PLANE_COLOR, row * SCREEN_COLS + col, len(expected.text))
    values = response.get("data")
    if values is None:
        values = [response.get("value")]

    for offset, ch in enumerate(expected.text):
        if ch == " ":
            continue
        actual = int(values[offset]) & 0xFF
        if actual == expected.color:
            continue
        raise RuntimeError(
            f"expected {expected.text!r} cell {col + offset},{row} "
            f"to use color ${expected.color:02X}; saw ${actual:02X}\n{screen}"
        )


def wait_for_game_prompt(
    client: NovaHostClient,
    smoke: HardwareSmoke,
    *,
    timeout: float,
    more_counter: list[int],
) -> str:
    snapshot = wait_for_screen(
        client,
        smoke.expected_screens,
        timeout=timeout,
        more_counter=more_counter,
        want_prompt=True,
    )
    screen = screen_text(snapshot)
    require_header(screen, smoke.header_expected_any)
    require_layout(screen, smoke)
    if not smoke.no_status_line:
        screen = wait_for_status_snapshot(client, screen, expect_time_status=smoke.expect_time_status)
        require_status_line(screen)
    if smoke.expect_time_status:
        require_time_status_line(screen)
    return screen


def run_smoke(
    host: str,
    smoke: HardwareSmoke,
    boot_timeout: float,
    command_timeout: float,
    reboot_after_mount: bool,
) -> None:
    if not smoke.image.is_file():
        raise FileNotFoundError(f"image not found for {smoke.name}: {smoke.image}")
    if smoke.script is not None and not smoke.script.is_file():
        raise FileNotFoundError(f"script not found for {smoke.name}: {smoke.script}")

    print(f"=== {smoke.name}: upload {smoke.image.name} -> {smoke.mount_path}", flush=True)
    post_json(host, "/drives/fd0/unmount")
    put_file(host, smoke.image, smoke.mount_path)
    post_json(host, "/drives/fd0/mount", {"path": smoke.mount_path})

    client = NovaHostClient(host=host, timeout=15)
    more_counter = [0]
    try:
        if reboot_after_mount:
            client.close()
            post_json(host, "/reboot")
            wait_for_health(host, boot_timeout)
        else:
            client.cold_start(wait_ready=False)
            client.close()
            time.sleep(0.5)

        if smoke.screen_only:
            snapshot = wait_for_screen(
                client,
                smoke.expected_screens,
                timeout=boot_timeout,
                more_counter=more_counter,
                want_prompt=False,
            )
            screen = screen_text(snapshot)
            for expected in smoke.expected_screens:
                require_contains(screen, expected, f"{smoke.name} screen")
            for expected in smoke.expected_text_colors:
                require_text_color(client, screen, expected)
            if smoke.expect_more and more_counter[0] == 0:
                raise RuntimeError(f"expected at least one [ MORE ] prompt for {smoke.name}\n{screen}")
            print(f"=== {smoke.name}: passed morePrompts={more_counter[0]}", flush=True)
            return

        screen = wait_for_game_prompt(
            client,
            smoke,
            timeout=boot_timeout,
            more_counter=more_counter,
        )

        for command in load_commands(smoke):
            if command.text.lower() == ".reboot":
                print("> .reboot", flush=True)
                client.cold_start(wait_ready=False)
                client.close()
                time.sleep(0.5)
                screen = wait_for_game_prompt(
                    client,
                    smoke,
                    timeout=boot_timeout,
                    more_counter=more_counter,
                )
                continue
            if command.wait_seconds is not None:
                print(f".wait {command.wait_seconds:g}", flush=True)
                time.sleep(command.wait_seconds)
                continue
            if command.expect_stop is not None:
                print(f".expect-stop {command.expect_stop}", flush=True)
                stop_address = try_read_runtime_symbol("zvm_stop_reason")
                if stop_address is None:
                    raise RuntimeError("runtime symbol zvm_stop_reason not found")
                actual_stop = client.peek(stop_address)
                if actual_stop != command.expect_stop:
                    screen = screen_text(client.read_screen())
                    raise RuntimeError(
                        f"expected zvm_stop_reason={command.expect_stop}, saw {actual_stop}\n{screen}"
                    )
                continue
            marker = ".raw " if command.raw else "> "
            print(f"{marker}{command.text}", flush=True)
            if command.raw:
                if len(command.text) == 1:
                    client.send_key(command.text)
                else:
                    client.type_text(command.text)
            else:
                client.type_text(command.text + "\r")
            time.sleep(0.35)
            if not command.wait_for_prompt and not command.expected:
                raise RuntimeError(f"no-prompt command {command.text!r} requires expected screen text")
            snapshot = wait_for_screen(
                client,
                command.expected if not command.wait_for_prompt else (),
                timeout=command_timeout,
                more_counter=more_counter,
                want_prompt=command.wait_for_prompt,
            )
            screen = screen_text(snapshot)
            if command.wait_for_prompt:
                require_header(screen, smoke.header_expected_any)
                require_layout(screen, smoke)
                if not smoke.no_status_line:
                    screen = wait_for_status_snapshot(client, screen, expect_time_status=smoke.expect_time_status)
                    require_status_line(screen)
                if smoke.expect_time_status:
                    require_time_status_line(screen)
            transcript = extract_command_transcript(screen, command.text) if command.wait_for_prompt and not command.raw else screen
            for expected in command.expected:
                require_contains(transcript, expected, f"{smoke.name} output after {command.text!r}")

        if smoke.expect_more and more_counter[0] == 0:
            raise RuntimeError(f"expected at least one [ MORE ] prompt for {smoke.name}\n{screen}")
        print(f"=== {smoke.name}: passed morePrompts={more_counter[0]}", flush=True)
    finally:
        client.close()


def select_smokes(names: str | None) -> list[HardwareSmoke]:
    by_name = {smoke.name: smoke for smoke in SMOKES}
    if not names:
        return list(SMOKES)
    selected = []
    for name in names.split(","):
        key = name.strip()
        if not key:
            continue
        if key not in by_name:
            raise ValueError(f"unknown smoke {key!r}; choices: {', '.join(by_name)}")
        selected.append(by_name[key])
    return selected


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--only", help="comma-separated smoke names to run")
    parser.add_argument("--build", action="store_true", help="rebuild local images before uploading")
    parser.add_argument("--leave-mounted", action="store_true", help="leave final FD0 image mounted")
    parser.add_argument(
        "--fast-cold-start",
        action="store_true",
        help="skip NovaHost reboot after mounting and use only VM cold_start",
    )
    parser.add_argument("--boot-timeout", type=float, default=60.0)
    parser.add_argument("--command-timeout", type=float, default=45.0)
    args = parser.parse_args()

    selected = select_smokes(args.only)
    if args.build:
        build_images(selected)

    failed = False
    for smoke in selected:
        try:
            run_smoke(
                args.host,
                smoke,
                args.boot_timeout,
                args.command_timeout,
                reboot_after_mount=not args.fast_cold_start,
            )
        except Exception as exc:  # noqa: BLE001 - hardware test runner should continue.
            failed = True
            print(f"=== {smoke.name}: FAILED: {exc}", file=sys.stderr, flush=True)
            break

    if not args.leave_mounted:
        try:
            post_json(args.host, "/drives/fd0/unmount")
            if args.fast_cold_start:
                client = NovaHostClient(host=args.host, timeout=15)
                try:
                    client.cold_start(wait_ready=False)
                finally:
                    client.close()
            else:
                post_json(args.host, "/reboot")
                wait_for_health(args.host, args.boot_timeout)
        except Exception as exc:  # noqa: BLE001 - cleanup failure is diagnostic only.
            print(f"cleanup warning: {exc}", file=sys.stderr)

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
