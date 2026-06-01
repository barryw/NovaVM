#!/usr/bin/env python3
"""Command-line wrapper around the shared NovaHost client."""

from __future__ import annotations

import argparse
import json
import sys
from typing import Any

from novahost_client import (
    DEFAULT_DEBUG_PORT,
    DEFAULT_HOST,
    DEFAULT_HTTP_PORT,
    DEFAULT_TIMEOUT,
    NovaHostClient,
    NovaHostError,
    parse_int,
)


def print_json(value: dict[str, Any], pretty: bool) -> None:
    if pretty:
        print(json.dumps(value, indent=2, sort_keys=True))
    else:
        print(json.dumps(value, separators=(",", ":"), sort_keys=True))


CPU_STATE_NAMES = {
    0: "ABS0",
    1: "ABS1",
    2: "ABSX0",
    3: "ABSX1",
    4: "ABSX2",
    12: "DECODE",
    13: "FETCH",
    35: "READ",
    36: "REG",
    46: "WRITE",
    47: "ZP0",
    48: "ZPX0",
    49: "ZPX1",
    62: "WAI",
    63: "STP",
}


def normalize_trace_response(response: dict[str, Any]) -> dict[str, Any]:
    if "records" in response:
        return response

    hex_text = str(response.get("hex", ""))
    if not hex_text:
        return {**response, "records": []}

    data = bytes.fromhex(hex_text)
    record_bytes = int(response.get("record_bytes", 12))
    records: list[dict[str, Any]] = []
    for base in range(0, len(data) - record_bytes + 1, record_bytes):
        ctrl = data[base + 11]
        records.append(
            {
                "pc": (data[base] << 8) | data[base + 1],
                "addr": (data[base + 2] << 8) | data[base + 3],
                "din": data[base + 4],
                "dout": data[base + 5],
                "a": data[base + 6],
                "sp": data[base + 7],
                "flags": data[base + 8],
                "state": data[base + 9] & 0x3F,
                "ir": data[base + 10],
                "ctrl": ctrl,
                "rdy": bool(ctrl & 0x01),
                "we": bool(ctrl & 0x02),
                "irq": bool(ctrl & 0x04),
                "nmi": bool(ctrl & 0x08),
                "decode": bool(ctrl & 0x10),
            }
        )
    out = dict(response)
    out["records"] = records
    return out


def print_trace(response: dict[str, Any], args: argparse.Namespace) -> None:
    normalized = normalize_trace_response(response)
    records = normalized.get("records", [])
    pc_filter = parse_int(args.pc) if args.pc else None
    addr_filter = parse_int(args.addr) if args.addr else None

    for index, record in enumerate(records):
        if args.writes and not record.get("we"):
            continue
        if args.decode and not record.get("decode"):
            continue
        if pc_filter is not None and int(record.get("pc", -1)) != pc_filter:
            continue
        if addr_filter is not None and int(record.get("addr", -1)) != addr_filter:
            continue

        state = int(record.get("state", 0))
        state_name = CPU_STATE_NAMES.get(state, str(state))
        print(
            f"{index:02d} pc=${int(record['pc']):04X} "
            f"state={state_name:<6} ir=${int(record['ir']):02X} "
            f"addr=${int(record['addr']):04X} din=${int(record['din']):02X} "
            f"dout=${int(record['dout']):02X} a=${int(record['a']):02X} "
            f"sp=${int(record['sp']):02X} flags=${int(record['flags']):02X} "
            f"rdy={int(bool(record.get('rdy')))} we={int(bool(record.get('we')))} "
            f"decode={int(bool(record.get('decode')))}"
        )


def add_common(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_DEBUG_PORT)
    parser.add_argument("--http-port", type=int, default=DEFAULT_HTTP_PORT)
    parser.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT)
    parser.add_argument("--pretty", action="store_true", help="pretty-print JSON responses")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Talk to NovaHost over HTTP/debug TCP.")
    add_common(parser)
    sub = parser.add_subparsers(dest="cmd", required=True)

    raw = sub.add_parser("raw", help="send a raw debug JSON payload")
    raw.add_argument("payload", help='JSON object, for example {"command":"dbg_state"}')
    raw.add_argument("--allow-error", action="store_true", help="exit 0 even when ok=false")

    sub.add_parser("health", help="GET /health")
    sub.add_parser("sd-status", help="GET /sd-status")
    sub.add_parser("boot-status", help="debug boot_status")
    sub.add_parser("state", help="debug CPU state")
    sub.add_parser("pause", help="pause the CPU")
    sub.add_parser("resume", help="resume the CPU")
    sub.add_parser("step", help="single-step one CPU instruction")
    sub.add_parser("break-list", help="list hardware breakpoints")
    sub.add_parser("break-clear-all", help="clear all hardware breakpoints")
    sub.add_parser("reload-rom", help="reload ROM images from SD")
    sub.add_parser("cursor", help="read text cursor position")

    break_set = sub.add_parser("break-set", help="set a source-PC breakpoint")
    break_set.add_argument("address")
    break_set.add_argument("--slot", type=int)
    break_set.add_argument("--disable", action="store_true", help="set the slot disabled")

    break_clear = sub.add_parser("break-clear", help="clear a hardware breakpoint")
    break_clear.add_argument("--slot", type=int)
    break_clear.add_argument("--address")

    trace = sub.add_parser("trace", help="read the CPU trace buffer")
    trace.add_argument("--count", type=int, default=16)
    trace.add_argument("--json", action="store_true", help="emit decoded JSON")
    trace.add_argument("--raw-json", action="store_true", help="emit NovaHost's raw response")
    trace.add_argument("--writes", action="store_true", help="show only write cycles")
    trace.add_argument("--decode", action="store_true", help="show only opcode decode cycles")
    trace.add_argument("--pc", help="show only records at this source PC")
    trace.add_argument("--addr", help="show only records for this bus address")

    cold = sub.add_parser("cold-start", help="reload default ROMs and reset the VM")
    cold.add_argument("--no-wait", action="store_true", help="do not wait for Ready")
    cold.add_argument("--text", help="screen text to wait for instead of Ready")

    vm_reset = sub.add_parser("vm-reset", help="POST /vm-reset without reloading ROMs")
    vm_reset.add_argument("--no-wait", action="store_true", help=argparse.SUPPRESS)
    vm_reset.add_argument("--text", help=argparse.SUPPRESS)

    wait = sub.add_parser("wait-ready", help="wait for screen text")
    wait.add_argument("--text", default="Ready")
    wait.add_argument("--timeout-ms", type=int, default=5000)

    key = sub.add_parser("key", help="send one key")
    key.add_argument("key")

    type_text = sub.add_parser("type-text", help="inject literal text")
    type_text.add_argument("text")

    basic = sub.add_parser("basic", help="type one BASIC line and press return")
    basic.add_argument("line")

    sub.add_parser("mute-sid", help="type a BASIC line that zeros both SID register ranges")

    screen = sub.add_parser("screen", help="read visible text screen")
    screen.add_argument("--json", action="store_true", help="emit raw JSON instead of text lines")

    line = sub.add_parser("line", help="read one text row")
    line.add_argument("row", type=int)

    peek = sub.add_parser("peek", help="read one 6502 address")
    peek.add_argument("address")

    poke = sub.add_parser("poke", help="write one 6502 address")
    poke.add_argument("address")
    poke.add_argument("value")

    read_vram = sub.add_parser("read-vram", help="read VGC memory")
    read_vram.add_argument("space", type=int)
    read_vram.add_argument("address")
    read_vram.add_argument("length", type=int)

    fill_vram = sub.add_parser("fill-vram", help="fill VGC memory")
    fill_vram.add_argument("space", type=int)
    fill_vram.add_argument("address")
    fill_vram.add_argument("value")
    fill_vram.add_argument("length", type=int)

    return parser


def dispatch(args: argparse.Namespace) -> int:
    client = NovaHostClient(args.host, args.port, args.http_port, args.timeout)
    try:
        if args.cmd == "raw":
            payload = json.loads(args.payload)
            response = client.raw(payload, require_ok=not args.allow_error)
            print_json(response, args.pretty)
            return 0 if args.allow_error or response.get("ok", False) else 1
        if args.cmd == "health":
            print_json(client.health(), args.pretty)
        elif args.cmd == "sd-status":
            print_json(client.sd_status(), args.pretty)
        elif args.cmd == "boot-status":
            print_json(client.boot_status(), args.pretty)
        elif args.cmd == "state":
            print_json(client.state(), args.pretty)
        elif args.cmd == "pause":
            print_json(client.pause(), args.pretty)
        elif args.cmd == "resume":
            print_json(client.resume(), args.pretty)
        elif args.cmd == "step":
            print_json(client.step(), args.pretty)
        elif args.cmd == "break-set":
            print_json(
                client.break_set(parse_int(args.address), slot=args.slot, enabled=not args.disable),
                args.pretty,
            )
        elif args.cmd == "break-clear":
            if args.slot is None and args.address is None:
                raise NovaHostError("break-clear requires --slot or --address")
            print_json(
                client.break_clear(
                    slot=args.slot,
                    address=parse_int(args.address) if args.address else None,
                ),
                args.pretty,
            )
        elif args.cmd == "break-clear-all":
            print_json(client.break_clear_all(), args.pretty)
        elif args.cmd == "break-list":
            print_json(client.break_list(), args.pretty)
        elif args.cmd == "trace":
            response = client.trace(args.count, hex_mode=not args.raw_json)
            if args.raw_json:
                print_json(response, args.pretty)
            elif args.json:
                print_json(normalize_trace_response(response), args.pretty)
            else:
                print_trace(response, args)
        elif args.cmd == "cold-start":
            print_json(client.cold_start(wait_ready=not args.no_wait, text=args.text), args.pretty)
        elif args.cmd == "vm-reset":
            if args.text:
                raise NovaHostError("vm-reset is REST-only; run wait-ready separately if needed")
            print_json(client.vm_reset(), args.pretty)
        elif args.cmd == "wait-ready":
            print_json(client.wait_ready(text=args.text, timeout_ms=args.timeout_ms), args.pretty)
        elif args.cmd == "reload-rom":
            print_json(client.reload_rom(), args.pretty)
        elif args.cmd == "cursor":
            print_json(client.get_cursor(), args.pretty)
        elif args.cmd == "key":
            print_json(client.send_key(args.key), args.pretty)
        elif args.cmd == "type-text":
            print_json(client.type_text(args.text), args.pretty)
        elif args.cmd == "basic":
            print_json(client.basic_line(args.line), args.pretty)
        elif args.cmd == "mute-sid":
            print_json(client.mute_sid(), args.pretty)
        elif args.cmd == "screen":
            response = client.read_screen()
            if args.json:
                print_json(response, args.pretty)
            else:
                for line in response.get("lines", []):
                    print(line)
        elif args.cmd == "line":
            print_json(client.read_line(args.row), args.pretty)
        elif args.cmd == "peek":
            value = client.peek(parse_int(args.address))
            print(f"${value:02X}")
        elif args.cmd == "poke":
            print_json(client.poke(parse_int(args.address), parse_int(args.value)), args.pretty)
        elif args.cmd == "read-vram":
            print_json(client.read_vram(args.space, parse_int(args.address), args.length), args.pretty)
        elif args.cmd == "fill-vram":
            print_json(
                client.fill_vram(
                    args.space,
                    parse_int(args.address),
                    parse_int(args.value),
                    args.length,
                ),
                args.pretty,
            )
        else:
            raise NovaHostError(f"unknown command: {args.cmd}")
        return 0
    finally:
        client.close()


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    try:
        return dispatch(args)
    except Exception as exc:  # noqa: BLE001 - CLI should render concise failures.
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
