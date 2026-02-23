#!/usr/bin/env python3
"""Interactive client for the e6502 NIC (length-prefixed framing).

The NIC wire protocol prefixes every message with a 1-byte length
(0 means 256).  This script handles the framing so you can type
plain text and see replies.

Usage:
    python3 docs/programs/nic_client.py [host] [port]

Defaults to localhost:8080 (matching echo_server.bas).
"""

import socket
import sys
import threading

def recv_loop(sock: socket.socket):
    """Read length-prefixed messages and print them."""
    try:
        while True:
            hdr = sock.recv(1)
            if not hdr:
                print("\n[server disconnected]")
                break
            length = hdr[0] if hdr[0] != 0 else 256
            data = b""
            while len(data) < length:
                chunk = sock.recv(length - len(data))
                if not chunk:
                    print("\n[server disconnected mid-message]")
                    return
                data += chunk
            print(f"< {data.decode('latin-1', errors='replace')}")
    except OSError:
        pass

def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "localhost"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8080

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    print(f"Connected to {host}:{port}  (Ctrl-C to quit)")

    t = threading.Thread(target=recv_loop, args=(sock,), daemon=True)
    t.start()

    try:
        while True:
            line = input("> ")
            if not line:
                continue
            data = line.encode("latin-1")
            if len(data) > 256:
                data = data[:256]
            length_byte = len(data) if len(data) < 256 else 0
            sock.sendall(bytes([length_byte]) + data)
    except (KeyboardInterrupt, EOFError):
        print("\n[disconnecting]")
    finally:
        sock.close()

if __name__ == "__main__":
    main()
