#!/usr/bin/env python3
"""Verify the live FPGA/NovaHost NIC path on hardware.

Default mode is a non-UI hardware smoke:
  - checks the $A100-$A13F NIC register bank
  - verifies debug-side IRQ latching
  - verifies the NIC IRQ reaches the CPU/debug trace line

Pass --exercise-novachess-network to cold-start Nova Chess, select
"N NEW GAME" then "4 NETWORK GAME", and verify a CPU write to NIC_CMD is
serviced by NovaHost. That mode changes the running VM state.
"""

from __future__ import annotations

import argparse
import queue
import socket
import sys
import time
from dataclasses import dataclass
from threading import Thread
from typing import Any

from novahost_client import (
    DEFAULT_DEBUG_PORT,
    DEFAULT_HOST,
    DEFAULT_HTTP_PORT,
    DEFAULT_TIMEOUT,
    NovaHostClient,
)


NIC_BASE = 0xA100
NIC_CMD = NIC_BASE + 0x00
NIC_STATUS = NIC_BASE + 0x01
NIC_SLOT = NIC_BASE + 0x02
NIC_IRQCTRL = NIC_BASE + 0x03
NIC_IRQSTATUS = NIC_BASE + 0x04
NIC_CMDSEQ = NIC_BASE + 0x05
NIC_CMDSHADOW = NIC_BASE + 0x06
NIC_RPORTL = NIC_BASE + 0x08
NIC_RPORTH = NIC_BASE + 0x09
NIC_DMAL = NIC_BASE + 0x10
NIC_DMAH = NIC_BASE + 0x11
NIC_DMALEN = NIC_BASE + 0x12
NIC_MSGLEN = NIC_BASE + 0x13
NIC_DMASTATUS = NIC_BASE + 0x14
NIC_DMAERR = NIC_BASE + 0x15
NIC_SLOTST0 = NIC_BASE + 0x18
NIC_NAMEBUF = NIC_BASE + 0x20

NIC_CMD_CONNECT = 0x01
NIC_CMD_DISCONNECT = 0x02
NIC_CMD_SEND = 0x03
NIC_CMD_RECV = 0x04

NIC_STATUS_READY = 0x01
NIC_STATUS_ANYDATA = 0x02
NIC_STATUS_ANYERROR = 0x80

NIC_ST_CONNECTED = 0x01
NIC_ST_DATAREADY = 0x02
NIC_ST_SENDREADY = 0x04
NIC_ST_ERROR = 0x08
NIC_ST_REMOTE_CLOSED = 0x10

NIC_DMAST_TX_READY = 0x01
NIC_DMAST_RX_DONE = 0x02
NIC_DMAST_ERROR = 0x40
NIC_DMAST_BUSY = 0x80

DMA_TX_ADDR = 0x2200
DMA_RX_ADDR = 0x2230
AUTOBOOT_SKIP = 0xB9F0
COMMAND_HELPER_ADDR = 0x7000
COMMAND_MAILBOX_ADDR = NIC_BASE + 0x0A
COMMAND_LOOP_TEMPLATE = bytes(
    [
        0xAD, 0x0A, 0xA1,  # LDA COMMAND_MAILBOX_ADDR
        0xF0, 0xFB,        # BEQ COMMAND_HELPER_ADDR
        0x8D, 0x06, 0xA1,  # STA NIC_CMDSHADOW
        0x9C, 0x00, 0xA1,  # STZ NIC_CMD
        0xEE, 0x05, 0xA1,  # INC NIC_CMDSEQ
        0xD0, 0x03,        # BNE :+
        0xEE, 0x05, 0xA1,  # INC NIC_CMDSEQ
        0x8D, 0x00, 0xA1,  # STA NIC_CMD
        0x9C, 0x0A, 0xA1,  # STZ COMMAND_MAILBOX_ADDR
        0x80, 0xE5,        # BRA COMMAND_HELPER_ADDR
    ]
)


@dataclass
class CheckState:
    failures: int = 0

    def pass_(self, message: str) -> None:
        print(f"PASS {message}", flush=True)

    def fail(self, message: str) -> None:
        self.failures += 1
        print(f"FAIL {message}", flush=True)

    def expect(self, condition: bool, message: str) -> None:
        if condition:
            self.pass_(message)
        else:
            self.fail(message)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run live hardware NIC smoke checks.")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_DEBUG_PORT)
    parser.add_argument("--http-port", type=int, default=DEFAULT_HTTP_PORT)
    parser.add_argument("--timeout", type=float, default=max(DEFAULT_TIMEOUT, 20.0))
    parser.add_argument(
        "--exercise-novachess-network",
        action="store_true",
        help="cold-start Nova Chess and select the network game menu to prove CPU NIC_CMD events",
    )
    parser.add_argument(
        "--exercise-basic-dma",
        action="store_true",
        help="cold-start BASIC and verify CPU-driven NIC CONNECT/SEND/RECV DMA against a local TCP peer",
    )
    parser.add_argument(
        "--network-timeout",
        type=float,
        default=12.0,
        help="seconds to wait for NovaHost to service the Nova Chess network command",
    )
    return parser.parse_args()


def peek_block(client: NovaHostClient, address: int, count: int) -> list[int]:
    response = client.raw(
        {"command": "peek_block", "address": address, "count": count},
        require_ok=True,
    )
    return [int(v) & 0xFF for v in response["values"]]


def poke_block_bytewise(client: NovaHostClient, address: int, values: bytes | list[int]) -> None:
    for offset, value in enumerate(values):
        client.poke((address + offset) & 0xFFFF, int(value) & 0xFF)


def wait_until(predicate: Any, timeout: float, interval: float = 0.05) -> bool:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if predicate():
            return True
        time.sleep(interval)
    return False


def trace_irq_seen(client: NovaHostClient, count: int = 16) -> bool:
    response = client.raw({"command": "dbg_trace", "count": count}, require_ok=True)
    return any(bool(record.get("irq", False)) for record in response["records"])


def read_screen_text(client: NovaHostClient) -> str:
    response = client.read_screen()
    return "\n".join(str(line) for line in response.get("lines", []))


def wait_screen_contains(client: NovaHostClient, text: str, timeout: float) -> bool:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if text in read_screen_text(client):
            return True
        time.sleep(0.1)
    return False


def nic_name(client: NovaHostClient) -> str:
    values = peek_block(client, NIC_NAMEBUF, 32)
    chars: list[str] = []
    for value in values:
        if value == 0:
            break
        if 32 <= value <= 126:
            chars.append(chr(value))
        else:
            chars.append(".")
    return "".join(chars)


def nic_snapshot(client: NovaHostClient) -> dict[str, Any]:
    rport = client.peek(NIC_RPORTL) | (client.peek(NIC_RPORTH) << 8)
    return {
        "cmd": client.peek(NIC_CMD),
        "status": client.peek(NIC_STATUS),
        "slot": client.peek(NIC_SLOT),
        "irqctrl": client.peek(NIC_IRQCTRL),
        "irqstatus": client.peek(NIC_IRQSTATUS),
        "slot0": client.peek(NIC_SLOTST0),
        "rport": rport,
        "name": nic_name(client),
    }


def print_nic_debug(client: NovaHostClient, label: str) -> None:
    try:
        status = client.raw({"command": "nic_status"}, require_ok=True)
        bank = peek_block(client, NIC_BASE, 32)
    except Exception as exc:  # noqa: BLE001 - best-effort diagnostics only.
        print(f"{label}: failed to read NIC diagnostics: {exc}", flush=True)
        return

    print(f"{label}: nic_status={status}", flush=True)
    print(f"{label}: bank[0..31]={[hex(v) for v in bank]}", flush=True)


def cleanup_nic_probe_registers(client: NovaHostClient) -> None:
    client.poke(NIC_STATUS, NIC_STATUS_READY)
    for slot in range(4):
        client.poke(NIC_SLOTST0 + slot, NIC_ST_SENDREADY)
    client.poke(NIC_IRQSTATUS, 0)
    client.poke(NIC_IRQCTRL, 0)
    client.poke(NIC_DMASTATUS, 0)
    client.poke(NIC_DMAERR, 0)
    client.poke(NIC_CMD, 0)


def check_register_bank(client: NovaHostClient, state: CheckState) -> None:
    cleanup_nic_probe_registers(client)
    bank = peek_block(client, NIC_BASE, 64)
    state.expect(len(bank) == 64, "NIC bank peek_block returned 64 bytes")
    state.expect(bank[0x01] == NIC_STATUS_READY, "NIC_STATUS is READY ($01)")
    state.expect(
        bank[0x18:0x1C] == [NIC_ST_SENDREADY] * 4,
        "NIC slot statuses are SENDREADY ($04)",
    )


def check_irq_latch(client: NovaHostClient, state: CheckState) -> None:
    cleanup_nic_probe_registers(client)
    client.poke(NIC_IRQCTRL, 0x01)

    state.expect(client.peek(NIC_IRQSTATUS) == 0x00, "IRQSTATUS starts clear")
    client.poke(NIC_SLOTST0, NIC_ST_SENDREADY | NIC_ST_DATAREADY)
    time.sleep(0.02)
    state.expect(client.peek(NIC_IRQSTATUS) == 0x01, "slot0 data-ready latches IRQSTATUS bit 0")

    client.poke(NIC_SLOTST0, NIC_ST_SENDREADY)
    time.sleep(0.02)
    state.expect(client.peek(NIC_IRQSTATUS) == 0x01, "IRQSTATUS remains latched after slot clears")

    client.poke(NIC_IRQSTATUS, 0x00)
    time.sleep(0.02)
    state.expect(client.peek(NIC_IRQSTATUS) == 0x00, "IRQSTATUS debug clear drops the latch")


def check_cpu_irq_line(client: NovaHostClient, state: CheckState) -> None:
    cleanup_nic_probe_registers(client)
    client.poke(NIC_IRQCTRL, 0x01)
    client.poke(NIC_SLOTST0, NIC_ST_SENDREADY | NIC_ST_DATAREADY)
    time.sleep(0.05)
    state.expect(trace_irq_seen(client), "NIC IRQ reaches CPU/debug trace line")

    cleanup_nic_probe_registers(client)
    time.sleep(0.05)
    state.expect(not trace_irq_seen(client, count=8), "NIC IRQ line drops after cleanup")


def local_ip_for_novahost(host: str, http_port: int, timeout: float) -> str:
    with socket.create_connection((host, http_port), timeout=timeout) as sock:
        return str(sock.getsockname()[0])


def read_exact(sock: socket.socket, count: int, timeout: float) -> bytes:
    sock.settimeout(timeout)
    data = bytearray()
    while len(data) < count:
        chunk = sock.recv(count - len(data))
        if not chunk:
            raise RuntimeError(f"socket closed with {len(data)}/{count} bytes read")
        data.extend(chunk)
    return bytes(data)


def send_basic_line_slow(client: NovaHostClient, line: str, key_delay: float = 0.035) -> None:
    for ch in line:
        client.send_key(ch)
        time.sleep(key_delay)
    client.send_key("ENTER")
    time.sleep(key_delay)


def install_command_loop(client: NovaHostClient) -> None:
    helper = bytes(COMMAND_LOOP_TEMPLATE)
    last_actual = b""
    for attempt in range(3):
        poke_block_bytewise(client, COMMAND_HELPER_ADDR, helper)
        actual = bytes(peek_block(client, COMMAND_HELPER_ADDR, len(helper)))
        if actual == helper:
            break
        last_actual = actual
        time.sleep(0.1)
    else:
        raise RuntimeError(
            "failed to install CPU NIC command loop "
            f"expected={list(helper)} actual={list(last_actual)}"
        )


def start_command_loop_from_basic(client: NovaHostClient) -> None:
    client.pause()
    try:
        install_command_loop(client)
        client.poke(COMMAND_MAILBOX_ADDR, 0)
    finally:
        client.resume()

    loop_end = COMMAND_HELPER_ADDR + len(COMMAND_LOOP_TEMPLATE)
    for _attempt in range(3):
        client.basic_line(f"SYS {COMMAND_HELPER_ADDR}")
        if wait_until(
            lambda: COMMAND_HELPER_ADDR <= int(client.state().get("pc", 0)) <= loop_end,
            8.0,
        ):
            return
        send_basic_line_slow(client, f"SYS {COMMAND_HELPER_ADDR}")
        if wait_until(
            lambda: COMMAND_HELPER_ADDR <= int(client.state().get("pc", 0)) <= loop_end,
            8.0,
        ):
            return
        time.sleep(0.5)

    print(read_screen_text(client), flush=True)
    print_nic_debug(client, "Command loop did not start")
    raise TimeoutError("BASIC did not start CPU NIC command loop")


def issue_nic_command_from_cpu_loop(client: NovaHostClient, cmd: int) -> None:
    client.poke(COMMAND_MAILBOX_ADDR, 0)
    previous_seq = client.peek(NIC_CMDSEQ)
    client.poke(COMMAND_MAILBOX_ADDR, cmd & 0xFF)
    if not wait_until(lambda: client.peek(NIC_CMDSEQ) != previous_seq, 8.0):
        print_nic_debug(client, "Command loop did not advance CMDSEQ")
        raise TimeoutError("CPU NIC command loop did not execute")
    if not wait_until(lambda: client.peek(COMMAND_MAILBOX_ADDR) == 0, 2.0):
        print_nic_debug(client, "Command loop did not clear mailbox")
        raise TimeoutError("CPU NIC command loop did not clear mailbox")


def issue_debug_nic_command(client: NovaHostClient, slot: int, cmd: int, timeout: float = 2.0) -> None:
    next_seq = (client.peek(NIC_CMDSEQ) + 1) & 0xFF
    client.poke(NIC_SLOT, slot & 0x03)
    client.poke(NIC_CMD, 0)
    client.poke(NIC_CMDSHADOW, cmd & 0xFF)
    client.poke(NIC_CMDSEQ, next_seq)
    client.poke(NIC_CMD, cmd & 0xFF)
    wait_command_cleared(client, timeout)


def reset_nic_dispatcher_slots(client: NovaHostClient) -> None:
    for slot in range(4):
        issue_debug_nic_command(client, slot, NIC_CMD_DISCONNECT)


def wait_command_cleared(client: NovaHostClient, timeout: float) -> bool:
    return wait_until(lambda: client.peek(NIC_CMD) == 0, timeout)


def accept_in_background(server: socket.socket) -> queue.Queue[Any]:
    results: queue.Queue[Any] = queue.Queue(maxsize=1)

    def run() -> None:
        try:
            conn, addr = server.accept()
            results.put((conn, addr))
        except Exception as exc:  # noqa: BLE001 - surfaced by caller.
            results.put(exc)

    thread = Thread(target=run, daemon=True)
    thread.start()
    return results


def get_accept_result(results: queue.Queue[Any], timeout: float) -> tuple[socket.socket, Any]:
    try:
        result = results.get(timeout=timeout)
    except queue.Empty as exc:
        raise TimeoutError("NovaHost did not connect to local TCP peer") from exc
    if isinstance(result, Exception):
        raise result
    return result


def check_basic_dma(client: NovaHostClient, state: CheckState, timeout: float) -> None:
    print("Cold-starting BASIC for CPU-driven NIC DMA smoke...", flush=True)
    reset_nic_dispatcher_slots(client)
    client.fill_vram(1, 0, 32, 4000)
    client.poke(AUTOBOOT_SKIP, 0xFF)
    client.cold_start(wait_ready=True, text="Ready")
    time.sleep(1.0)
    cleanup_nic_probe_registers(client)
    server: socket.socket | None = None

    try:
        print("Starting CPU NIC command loop...", flush=True)
        start_command_loop_from_basic(client)

        local_ip = local_ip_for_novahost(client.host, client.http_port, client.timeout)
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((local_ip, 0))
        server.listen(1)
        server.settimeout(timeout)
        port = int(server.getsockname()[1])
        accepted = accept_in_background(server)

        poke_block_bytewise(client, NIC_NAMEBUF, local_ip.encode("ascii") + b"\x00")
        client.poke(NIC_SLOT, 0)
        client.poke(NIC_RPORTL, port & 0xFF)
        client.poke(NIC_RPORTH, (port >> 8) & 0xFF)

        issue_nic_command_from_cpu_loop(client, NIC_CMD_CONNECT)
        conn, _addr = get_accept_result(accepted, timeout)
        with conn:
            connected = wait_until(
                lambda: (client.peek(NIC_SLOTST0) & NIC_ST_CONNECTED) != 0,
                timeout,
            )
            connect_status = client.raw({"command": "nic_status"}, require_ok=True)
            print(
                "NIC connect status: "
                f"lastConnectOk={connect_status.get('lastConnectOk')} "
                f"slotStatus={connect_status.get('slotStatus')} "
                f"lastError={connect_status.get('lastError')!r}",
                flush=True,
            )
            state.expect(connected, "CPU-issued NIC CONNECT reaches NovaHost and local TCP peer")
            state.expect(
                bool(connect_status.get("lastConnectOk")),
                "NovaHost reports CONNECT success",
            )
            state.expect(wait_command_cleared(client, timeout), "CONNECT command is cleared")

            tx_payload = b"DMA-TX!"
            print("Staging TX payload in CPU RAM...", flush=True)
            poke_block_bytewise(client, DMA_TX_ADDR, tx_payload)
            client.poke(NIC_DMAL, DMA_TX_ADDR & 0xFF)
            client.poke(NIC_DMAH, (DMA_TX_ADDR >> 8) & 0xFF)
            client.poke(NIC_DMALEN, len(tx_payload))
            client.poke(NIC_DMASTATUS, 0)
            client.poke(NIC_DMAERR, 0)

            print("Issuing CPU SEND command...", flush=True)
            issue_nic_command_from_cpu_loop(client, NIC_CMD_SEND)
            print("Waiting for TCP SEND payload...", flush=True)
            try:
                wire_len = read_exact(conn, 1, timeout)[0]
                received = read_exact(conn, wire_len or 256, timeout)
            except Exception:
                print_nic_debug(client, "SEND failure")
                raise
            state.expect(received == tx_payload, "NIC SEND DMA copies CPU RAM into TCP payload")
            state.expect(
                wait_until(lambda: (client.peek(NIC_DMASTATUS) & NIC_DMAST_TX_READY) != 0, timeout),
                "SEND reports TX_READY after hardware DMA",
            )
            state.expect(client.peek(NIC_DMAERR) == 0, "SEND DMAERR remains clear")
            state.expect(wait_command_cleared(client, timeout), "SEND command is cleared")

            rx_payload = b"RX-DMA"
            conn.sendall(bytes([len(rx_payload)]) + rx_payload)
            data_ready = wait_until(
                lambda: (client.peek(NIC_SLOTST0) & NIC_ST_DATAREADY) != 0,
                timeout,
            )
            state.expect(data_ready, "NovaHost queues inbound TCP payload for RECV")

            poke_block_bytewise(client, DMA_RX_ADDR, b"\x00" * len(rx_payload))
            client.poke(NIC_DMAL, DMA_RX_ADDR & 0xFF)
            client.poke(NIC_DMAH, (DMA_RX_ADDR >> 8) & 0xFF)
            client.poke(NIC_DMASTATUS, 0)
            client.poke(NIC_DMAERR, 0)

            issue_nic_command_from_cpu_loop(client, NIC_CMD_RECV)
            rx_done = wait_until(
                lambda: (client.peek(NIC_DMASTATUS) & NIC_DMAST_RX_DONE) != 0,
                timeout,
            )
            rx_ram = bytes(peek_block(client, DMA_RX_ADDR, len(rx_payload)))
            if not rx_done:
                print_nic_debug(client, "RECV failure")
            msg_len_ok = wait_until(lambda: client.peek(NIC_MSGLEN) == len(rx_payload), timeout)
            actual_msg_len = client.peek(NIC_MSGLEN)
            if not msg_len_ok:
                print(
                    f"RECV length mismatch: got {actual_msg_len}, want {len(rx_payload)}",
                    flush=True,
                )
            state.expect(rx_done, "RECV reports RX_DONE after hardware DMA")
            state.expect(client.peek(NIC_DMAERR) == 0, "RECV DMAERR remains clear")
            state.expect(msg_len_ok, "RECV exposes received message length")
            state.expect(rx_ram == rx_payload, "NIC RECV DMA copies TCP payload into CPU RAM")
            state.expect(wait_command_cleared(client, timeout), "RECV command is cleared")

            issue_nic_command_from_cpu_loop(client, NIC_CMD_DISCONNECT)
            state.expect(wait_command_cleared(client, timeout), "DISCONNECT command is cleared")
    finally:
        client.poke(AUTOBOOT_SKIP, 0x00)
        if server is not None:
            server.close()
        cleanup_nic_probe_registers(client)
        try:
            client.cold_start(wait_ready=True, text="Ready")
        except Exception:
            pass


def exercise_novachess_network(client: NovaHostClient, state: CheckState, timeout: float) -> None:
    cleanup_nic_probe_registers(client)
    client.poke(NIC_CMD, 0)

    print("Cold-starting Nova Chess and selecting network game...", flush=True)
    client.cold_start(wait_ready=True, text="N NEW GAME")
    if not wait_screen_contains(client, "N NEW GAME", 5.0):
        state.fail("Nova Chess title menu did not appear")
        return

    client.send_key("N")
    if not wait_screen_contains(client, "NEW GAME", 5.0):
        state.fail("Nova Chess new-game menu did not appear")
        return

    menu_text = read_screen_text(client)
    if "4 NETWORK GAME" not in menu_text:
        if "NETWORK PLAY WILL USE NOVAHOST" in menu_text:
            state.fail(
                "live Nova Chess image lacks the current network menu option "
                "('4 NETWORK GAME'); refresh the SD/ROM image before this event test"
            )
        else:
            state.fail("Nova Chess new-game menu does not expose network game option")
        return

    client.send_key("4")

    deadline = time.monotonic() + timeout
    saw_command = False
    final = nic_snapshot(client)
    while time.monotonic() < deadline:
        snap = nic_snapshot(client)
        final = snap
        if snap["cmd"] != 0:
            saw_command = True

        host_serviced = (
            snap["cmd"] == 0
            and (
                (snap["slot0"] & (NIC_ST_CONNECTED | NIC_ST_ERROR | NIC_ST_REMOTE_CLOSED)) != 0
                or (snap["status"] & (NIC_STATUS_ANYDATA | NIC_STATUS_ANYERROR)) != 0
            )
        )
        if host_serviced:
            break
        time.sleep(0.1)

    if final["cmd"] == 0 and (
        (final["slot0"] & (NIC_ST_CONNECTED | NIC_ST_ERROR | NIC_ST_REMOTE_CLOSED)) != 0
        or (final["status"] & (NIC_STATUS_ANYDATA | NIC_STATUS_ANYERROR)) != 0
    ):
        state.pass_(
            "NovaHost serviced CPU NIC_CMD event "
            f"(status=${final['status']:02X}, slot0=${final['slot0']:02X}, "
            f"host={final['name']!r}, port={final['rport']})"
        )
    elif saw_command or final["cmd"] != 0:
        state.fail(
            "CPU wrote NIC_CMD but NovaHost did not service it "
            f"(cmd=${final['cmd']:02X}, status=${final['status']:02X}, "
            f"slot0=${final['slot0']:02X}, host={final['name']!r}, port={final['rport']})"
        )
    else:
        state.fail(
            "Nova Chess network path did not issue NIC_CMD "
            f"(cmd=${final['cmd']:02X}, status=${final['status']:02X}, "
            f"slot0=${final['slot0']:02X})"
        )


def main() -> int:
    args = parse_args()
    state = CheckState()

    with NovaHostClient(args.host, args.port, args.http_port, args.timeout) as client:
        health = client.health()
        state.expect(bool(health.get("ok")), "NovaHost /health returned ok")
        state.expect(
            bool(health.get("fpgaBridgeAvailable")),
            "NovaHost reports FPGA bridge available",
        )
        state.expect(
            bool(health.get("capabilities", {}).get("nicDispatcher")),
            "NovaHost reports NIC dispatcher capability",
        )

        reset_nic_dispatcher_slots(client)
        check_register_bank(client, state)
        check_irq_latch(client, state)
        check_cpu_irq_line(client, state)

        if args.exercise_novachess_network:
            exercise_novachess_network(client, state, args.network_timeout)

        if args.exercise_basic_dma:
            check_basic_dma(client, state, args.network_timeout)

        cleanup_nic_probe_registers(client)

    return 1 if state.failures else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
