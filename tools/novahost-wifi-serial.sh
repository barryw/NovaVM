#!/usr/bin/env bash
# Manage NovaHost WiFi over USB serial by temporarily loading the FPGA
# FTDI<->ESP32 passthrough image, sending a serial console command, then
# reloading the main FPGA image from config flash.
#
# Usage:
#   tools/novahost-wifi-serial.sh wifi status
#   tools/novahost-wifi-serial.sh wifi set ssid "Network Name"
#   tools/novahost-wifi-serial.sh wifi set password "secret"
#   tools/novahost-wifi-serial.sh wifi connect
#
# Environment:
#   SERIAL=/dev/cu.usbserial-D01457
#   BAUD=115200
#   LOAD_PASSTHRU=1
#   RELOAD_FPGA=1
#   SERIAL_OPEN_DELAY=3

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FPGA_DIR="$REPO_ROOT/e6502.FPGA/fpga"

SERIAL="${SERIAL:-/dev/cu.usbserial-D01457}"
BAUD="${BAUD:-115200}"
LOAD_PASSTHRU="${LOAD_PASSTHRU:-1}"
RELOAD_FPGA="${RELOAD_FPGA:-1}"
SERIAL_OPEN_DELAY="${SERIAL_OPEN_DELAY:-3}"
OSS_CAD_SUITE_BIN="${OSS_CAD_SUITE_BIN:-/opt/homebrew/oss-cad-suite/bin}"
PASSTHRU_BIT="${PASSTHRU_BIT:-$FPGA_DIR/build/passthru.bit}"

if [ "$#" -eq 0 ]; then
    set -- wifi status
fi

cmd="$*"
loaded_passthru=0

reload_fpga() {
    if [ "$loaded_passthru" = "1" ] && [ "$RELOAD_FPGA" = "1" ]; then
        echo "Reloading FPGA from config flash..."
        openFPGALoader --board ulx3s -r
    fi
}
trap reload_fpga EXIT

build_passthru_if_needed() {
    if [ -f "$PASSTHRU_BIT" ]; then
        return 0
    fi

    echo "Building FPGA ESP serial passthrough..."
    (
        cd "$FPGA_DIR"
        mkdir -p build
        "$OSS_CAD_SUITE_BIN/yosys" -q -p \
            "read_verilog -sv passthru.sv; synth_ecp5 -top passthru -json build/passthru.json"
        "$OSS_CAD_SUITE_BIN/nextpnr-ecp5" --85k --package CABGA381 \
            --textcfg build/passthru.config --json build/passthru.json --lpf passthru.lpf -q
        "$OSS_CAD_SUITE_BIN/ecppack" build/passthru.config build/passthru.bit
    )
}

if [ "$LOAD_PASSTHRU" = "1" ]; then
    build_passthru_if_needed
    echo "Loading FPGA ESP serial passthrough..."
    openFPGALoader --board ulx3s "$PASSTHRU_BIT"
    loaded_passthru=1
fi

SERIAL_PATH="$SERIAL" SERIAL_BAUD="$BAUD" SERIAL_CMD="$cmd" \
SERIAL_OPEN_DELAY="$SERIAL_OPEN_DELAY" python3 - <<'PY'
import os
import select
import sys
import termios
import time

path = os.environ["SERIAL_PATH"]
baud = int(os.environ["SERIAL_BAUD"])
cmd = os.environ["SERIAL_CMD"].encode("utf-8") + b"\n"
open_delay = float(os.environ["SERIAL_OPEN_DELAY"])

baud_attr = getattr(termios, f"B{baud}", None)
if baud_attr is None:
    raise SystemExit(f"unsupported baud: {baud}")

fd = os.open(path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
try:
    attrs = termios.tcgetattr(fd)
    attrs[0] &= ~(termios.IXON | termios.IXOFF | termios.IXANY |
                  termios.ICRNL | termios.INLCR)
    attrs[1] &= ~termios.OPOST
    attrs[2] &= ~(termios.CSIZE | termios.PARENB | termios.CSTOPB)
    attrs[2] |= termios.CS8 | termios.CREAD | termios.CLOCAL
    attrs[3] &= ~(termios.ECHO | termios.ECHONL | termios.ICANON |
                  termios.ISIG | termios.IEXTEN)
    attrs[4] = baud_attr
    attrs[5] = baud_attr
    termios.tcsetattr(fd, termios.TCSANOW, attrs)

    time.sleep(open_delay)
    try:
        while os.read(fd, 4096):
            pass
    except BlockingIOError:
        pass

    os.write(fd, cmd)

    data = b""
    deadline = time.time() + 8.0
    idle_deadline = None
    while time.time() < deadline:
        ready, _, _ = select.select([fd], [], [], 0.2)
        if not ready:
            if data and idle_deadline is not None and time.time() >= idle_deadline:
                break
            continue
        try:
            chunk = os.read(fd, 4096)
        except BlockingIOError:
            continue
        if not chunk:
            break
        data += chunk
        idle_deadline = time.time() + 0.75

    text = data.decode("utf-8", errors="replace").strip()
    if text:
        print(text)
    else:
        print("warning: no serial response captured", file=sys.stderr)
        sys.exit(1)
finally:
    os.close(fd)
PY
