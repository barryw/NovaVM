#!/usr/bin/env python3
"""Smoke test: FPGA debug bridge via NovaHost TCP:6503.

Exercises the full stack: Test script -> TCP JSON -> ESP32 -> UART binary
-> FPGA debug_bridge -> core CPU.
"""
import json
import socket
import sys
import time

HOST = "192.168.1.65"
PORT = 6503


def send(sock, cmd, **kwargs):
    payload = {"command": cmd, **kwargs}
    sock.sendall((json.dumps(payload) + "\n").encode())
    # Read one line response
    buf = b""
    deadline = time.time() + 5.0
    while time.time() < deadline and b"\n" not in buf:
        chunk = sock.recv(4096)
        if not chunk:
            break
        buf += chunk
    line = buf.split(b"\n", 1)[0]
    return json.loads(line.decode()) if line else None


def main():
    s = socket.create_connection((HOST, PORT), timeout=5)
    s.settimeout(5.0)
    print(f"Connected to {HOST}:{PORT}")

    # Basic synchronous commands
    print("\n-- dbg_state --")
    print(send(s, "dbg_state"))

    print("\n-- get_cursor --")
    print(send(s, "get_cursor"))

    print("\n-- peek $FFFC (reset vector lo) --")
    print(send(s, "peek", address=0xFFFC))

    print("\n-- peek $FFFD (reset vector hi) --")
    print(send(s, "peek", address=0xFFFD))

    print("\n-- read_line row 0 --")
    print(send(s, "read_line", row=0))

    s.close()
    print("\nAll commands completed without timeout.")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"FAIL: {e}", file=sys.stderr)
        sys.exit(1)
