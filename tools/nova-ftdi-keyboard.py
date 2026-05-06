#!/usr/bin/env python3
"""Interactive keyboard bridge for Nova's FTDI UART input.

The FPGA currently accepts keyboard bytes on the FTDI RX line at 9600 8N1.
This tool makes the host computer act as the USB keyboard host: it reads keys
from the local terminal and writes Nova-compatible bytes to the ULX3S FTDI
serial port.
"""

from __future__ import annotations

import argparse
import glob
import os
import select
import sys
import termios
import time
import tty
from dataclasses import dataclass

try:
    import serial
    from serial.tools import list_ports
except ImportError as exc:  # pragma: no cover - rendered by CLI.
    serial = None
    list_ports = None
    SERIAL_IMPORT_ERROR = exc
else:
    SERIAL_IMPORT_ERROR = None


DEFAULT_BAUD = int(os.environ.get("NOVA_FTDI_BAUD", "9600"))
DEFAULT_PORT = os.environ.get("NOVA_FTDI_PORT")
QUIT_BYTE = b"\x1d"  # Ctrl-]


@dataclass(frozen=True)
class PortInfo:
    device: str
    description: str = ""
    hwid: str = ""


def discover_ports() -> list[PortInfo]:
    ports: dict[str, PortInfo] = {}

    if list_ports is not None:
        for port in list_ports.comports():
            ports[port.device] = PortInfo(
                device=port.device,
                description=port.description or "",
                hwid=port.hwid or "",
            )

    for pattern in (
        "/dev/cu.usbserial*",
        "/dev/cu.usbmodem*",
        "/dev/ttyUSB*",
        "/dev/ttyACM*",
    ):
        for device in glob.glob(pattern):
            ports.setdefault(device, PortInfo(device=device))

    return sorted(ports.values(), key=lambda p: p.device)


def choose_port(requested: str | None) -> str:
    if requested:
        return requested
    if DEFAULT_PORT:
        return DEFAULT_PORT

    ports = discover_ports()
    likely = [
        port
        for port in ports
        if "usbserial" in port.device.lower()
        or "ftdi" in port.description.lower()
        or "ftdi" in port.hwid.lower()
        or "0403:" in port.hwid.lower()
    ]

    if len(likely) == 1:
        return likely[0].device
    if len(ports) == 1:
        return ports[0].device

    lines = ["could not choose FTDI serial port automatically"]
    if ports:
        lines.append("available serial ports:")
        for port in ports:
            detail = " ".join(x for x in (port.description, port.hwid) if x)
            lines.append(f"  {port.device}" + (f"  {detail}" if detail else ""))
    else:
        lines.append("no serial ports found")
    lines.append("pass --port /dev/cu.usbserial-... or set NOVA_FTDI_PORT")
    raise RuntimeError("\n".join(lines))


def open_serial(port: str, baud: int) -> serial.Serial:
    if serial is None:
        raise RuntimeError(f"pyserial is required: {SERIAL_IMPORT_ERROR}")

    return serial.Serial(
        port=port,
        baudrate=baud,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=0,
        write_timeout=1,
        xonxoff=False,
        rtscts=False,
        dsrdtr=False,
    )


def render_byte(byte: int) -> str:
    if byte == 0x0D:
        return "\n"
    if byte == 0x7F:
        return "\b \b"
    if byte == 0x09:
        return "\t"
    if 0x20 <= byte <= 0x7E:
        return chr(byte)
    return ""


def normalize_input(data: bytes) -> bytes:
    out = bytearray()
    for byte in data:
        if byte == 0x0A:
            out.append(0x0D)
        elif byte == 0x08:
            out.append(0x7F)
        else:
            out.append(byte)
    return bytes(out)


def forward_keys(
    port: str,
    baud: int,
    echo: bool,
    inter_byte_delay: float,
    ctrl_c_quits: bool,
) -> None:
    ser = open_serial(port, baud)
    old_term = termios.tcgetattr(sys.stdin.fileno())

    print(f"Nova FTDI keyboard bridge: {port} @ {baud} 8N1", file=sys.stderr)
    print("Type normally. Ctrl-] quits. Ctrl-C is sent to Nova.", file=sys.stderr)
    sys.stderr.flush()

    try:
        tty.setraw(sys.stdin.fileno())
        while True:
            readable, _, _ = select.select([sys.stdin], [], [], 0.1)
            if not readable:
                continue

            data = os.read(sys.stdin.fileno(), 1024)
            if not data:
                break

            if ctrl_c_quits and b"\x03" in data:
                break

            if QUIT_BYTE in data:
                before, _, _ = data.partition(QUIT_BYTE)
                data = before
                should_quit = True
            else:
                should_quit = False

            data = normalize_input(data)
            for byte in data:
                ser.write(bytes((byte,)))
                if echo:
                    sys.stdout.write(render_byte(byte))
                    sys.stdout.flush()
                if inter_byte_delay > 0:
                    time.sleep(inter_byte_delay)

            ser.flush()
            if should_quit:
                break
    finally:
        termios.tcsetattr(sys.stdin.fileno(), termios.TCSADRAIN, old_term)
        ser.close()
        print("\nNova FTDI keyboard bridge closed.", file=sys.stderr)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Send local keyboard input to Nova through the ULX3S FTDI UART."
    )
    parser.add_argument("--port", help="serial port, for example /dev/cu.usbserial-D01457")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="serial baud rate")
    parser.add_argument("--list", action="store_true", help="list candidate serial ports and exit")
    parser.add_argument("--echo", action="store_true", help="locally echo forwarded characters")
    parser.add_argument(
        "--delay-ms",
        type=float,
        default=0.0,
        help="optional delay between bytes for slow paste operations",
    )
    parser.add_argument(
        "--ctrl-c-quits",
        action="store_true",
        help="make Ctrl-C quit instead of sending byte 0x03 to Nova",
    )
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    if args.list:
        ports = discover_ports()
        if not ports:
            print("no serial ports found")
            return 1
        for port in ports:
            detail = " ".join(x for x in (port.description, port.hwid) if x)
            print(port.device + (f"\t{detail}" if detail else ""))
        return 0

    try:
        port = choose_port(args.port)
        forward_keys(
            port=port,
            baud=args.baud,
            echo=args.echo,
            inter_byte_delay=max(args.delay_ms, 0.0) / 1000.0,
            ctrl_c_quits=args.ctrl_c_quits,
        )
    except Exception as exc:  # noqa: BLE001 - CLI should render concise failures.
        print(f"error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
