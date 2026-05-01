#!/usr/bin/env python3
"""Capture a data-backed NovaVM hardware debug snapshot.

The goal is to preserve enough evidence to debug a hung or failing hardware
run without guessing: boot status, CPU state, visible screen text, HDMI frame,
and key memory ranges around the CPU.
"""

from __future__ import annotations

import argparse
import datetime as _dt
import json
import os
import socket
import subprocess
import sys
import urllib.error
import urllib.request
from bisect import bisect_right
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_HOST = "192.168.1.65"
DEFAULT_PORT = 6503
DEFAULT_HTTP_PORT = 80
MMIO_REGS = [
    ("RegMode", 0xA000),
    ("RegBgCol", 0xA001),
    ("RegFgCol", 0xA002),
    ("RegCursorX", 0xA003),
    ("RegCursorY", 0xA004),
    ("RegScrollX", 0xA005),
    ("RegScrollY", 0xA006),
    ("RegFont", 0xA007),
    ("RegStatus", 0xA008),
    ("RegSpriteCount", 0xA009),
    ("RegCursorEnable", 0xA00A),
    ("RegColSt", 0xA00B),
    ("RegColBg", 0xA00C),
    ("RegBorder", 0xA00D),
    ("RegCmd", 0xA010),
    ("RegP0", 0xA011),
    ("RegP1", 0xA012),
    ("RegP2", 0xA013),
    ("RegP3", 0xA014),
    ("RegP4", 0xA015),
    ("RegP5", 0xA016),
    ("RegP6", 0xA017),
    ("RegP7", 0xA018),
    ("RegP8", 0xA019),
    ("RegP9", 0xA01A),
    ("RegP10", 0xA01B),
    ("RegP11", 0xA01C),
    ("RegP12", 0xA01D),
    ("RegP13", 0xA01E),
    ("RegP14", 0xA01F),
    ("RegIrqEnable", 0xA0F0),
    ("RegIrqStatus", 0xA0F1),
    ("RegIrqValid", 0xA0F3),
]


class DebugClient:
    def __init__(self, host: str, port: int, timeout: float) -> None:
        self._sock = socket.create_connection((host, port), timeout=timeout)
        self._sock.settimeout(timeout)
        self._rx = b""

    def close(self) -> None:
        self._sock.close()

    def command(self, command: str, **kwargs: Any) -> dict[str, Any]:
        payload = {"command": command}
        payload.update(kwargs)
        self._sock.sendall(json.dumps(payload, separators=(",", ":")).encode("utf-8") + b"\n")

        while b"\n" not in self._rx:
            chunk = self._sock.recv(65536)
            if not chunk:
                break
            self._rx += chunk

        if b"\n" not in self._rx:
            raise RuntimeError(f"no newline response for debug command {command!r}")

        line, self._rx = self._rx.split(b"\n", 1)
        text = line.decode("utf-8", errors="replace").strip()
        if not text:
            raise RuntimeError(f"empty response for debug command {command!r}")
        try:
            return json.loads(text)
        except json.JSONDecodeError as exc:
            raise RuntimeError(f"invalid JSON from {command!r}: {text}") from exc


def parse_args() -> argparse.Namespace:
    stamp = _dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    default_out = Path("/tmp") / f"novavm-snapshot-{stamp}"

    parser = argparse.ArgumentParser(
        description="Capture a NovaVM hardware debug snapshot through NovaHost."
    )
    parser.add_argument("--host", default=os.environ.get("NOVAHOST", DEFAULT_HOST))
    parser.add_argument("--port", type=int, default=int(os.environ.get("NOVAHOST_PORT", DEFAULT_PORT)))
    parser.add_argument("--http-port", type=int, default=int(os.environ.get("HTTP_PORT", DEFAULT_HTTP_PORT)))
    parser.add_argument("--timeout", type=float, default=float(os.environ.get("DEBUG_RESPONSE_TIMEOUT", "15")))
    parser.add_argument("--out", type=Path, default=default_out)
    parser.add_argument("--label", default="")
    parser.add_argument("--resume", action="store_true", help="resume the CPU after capture")
    parser.add_argument("--no-pause", action="store_true", help="do not pause CPU before capture")
    parser.add_argument("--no-hdmi", action="store_true", help="skip HDMI frame capture")
    parser.add_argument(
        "--hdmi-tool",
        type=Path,
        default=Path(os.environ.get("HDMI_CAPTURE_TOOL", REPO_ROOT / "tools" / "capture-hdmi-frame.sh")),
    )
    parser.add_argument(
        "--symbols",
        type=Path,
        default=REPO_ROOT / "ehbasic" / "basic.sym",
        help="cc65 symbol file used to map PC to ROM labels",
    )
    return parser.parse_args()


def http_get_json(host: str, port: int, path: str, timeout: float) -> dict[str, Any]:
    url = f"http://{host}:{port}{path}"
    try:
        with urllib.request.urlopen(url, timeout=timeout) as resp:
            data = resp.read().decode("utf-8", errors="replace")
        try:
            return {"ok": True, "url": url, "json": json.loads(data), "raw": data}
        except json.JSONDecodeError:
            return {"ok": True, "url": url, "raw": data}
    except (OSError, urllib.error.URLError) as exc:
        return {"ok": False, "url": url, "error": str(exc)}


def load_symbols(path: Path) -> list[tuple[int, str]]:
    symbols: list[tuple[int, str]] = []
    if not path.exists():
        return symbols

    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        parts = line.split()
        if len(parts) < 3 or parts[0] != "al":
            continue
        try:
            addr = int(parts[1], 16) & 0xFFFF
        except ValueError:
            continue
        name = parts[2].lstrip(".")
        symbols.append((addr, name))

    symbols.sort(key=lambda item: item[0])
    return symbols


def nearest_symbol(symbols: list[tuple[int, str]], pc: int | None) -> dict[str, Any] | None:
    if pc is None or not symbols:
        return None

    addrs = [addr for addr, _ in symbols]
    idx = bisect_right(addrs, pc & 0xFFFF) - 1
    if idx < 0:
        return None

    addr, name = symbols[idx]
    return {"address": addr, "name": name, "offset": (pc & 0xFFFF) - addr}


def flags_byte(state: dict[str, Any]) -> int | None:
    keys = ("nf", "vf", "df", "if", "zf", "cf")
    if not all(key in state for key in keys):
        return None
    return (
        ((int(state["nf"]) & 1) << 7)
        | ((int(state["vf"]) & 1) << 6)
        | (1 << 5)
        | ((int(state["df"]) & 1) << 3)
        | ((int(state["if"]) & 1) << 2)
        | ((int(state["zf"]) & 1) << 1)
        | (int(state["cf"]) & 1)
    )


def format_cpu_state(state: dict[str, Any], symbol: dict[str, Any] | None) -> str:
    if not state or not state.get("ok"):
        return "CPU state unavailable"

    pc = int(state.get("pc", 0)) & 0xFFFF
    p = flags_byte(state)
    sym = ""
    if symbol:
        offset = int(symbol["offset"])
        sym = f" {symbol['name']}"
        if offset:
            sym += f"+${offset:04X}"

    flags = f"P=${p:02X}" if p is not None else "P=?"
    return (
        f"PC=${pc:04X}{sym} A=${int(state.get('a', 0)) & 0xFF:02X} "
        f"X=${int(state.get('x', 0)) & 0xFF:02X} Y=${int(state.get('y', 0)) & 0xFF:02X} "
        f"SP=${int(state.get('sp', 0)) & 0xFF:02X} {flags} "
        f"paused={state.get('paused')} waiting={state.get('waiting')} stopped={state.get('stopped')}"
    )


def hexdump(start: int, values: list[int | None], width: int = 16) -> str:
    lines: list[str] = []
    for offset in range(0, len(values), width):
        chunk = values[offset : offset + width]
        hex_bytes = " ".join("--" if val is None else f"{val:02X}" for val in chunk)
        ascii_bytes = "".join("." if val is None or val < 32 or val > 126 else chr(val) for val in chunk)
        lines.append(f"{(start + offset) & 0xFFFF:04X}: {hex_bytes:<{width * 3 - 1}}  {ascii_bytes}")
    return "\n".join(lines)


def read_range_bytewise(client: DebugClient, start: int, length: int) -> dict[str, Any]:
    values: list[int | None] = []
    errors: list[dict[str, Any]] = []
    for offset in range(length):
        addr = (start + offset) & 0xFFFF
        try:
            resp = client.command("peek", address=addr)
        except Exception as exc:  # noqa: BLE001 - snapshot should keep going.
            values.append(None)
            errors.append({"address": addr, "error": str(exc)})
            continue
        if resp.get("ok"):
            values.append(int(resp.get("value", 0)) & 0xFF)
        else:
            values.append(None)
            errors.append({"address": addr, "response": resp})
    return {"start": start & 0xFFFF, "length": length, "values": values, "errors": errors}


def read_range(client: DebugClient, start: int, length: int) -> dict[str, Any]:
    values: list[int | None] = []
    errors: list[dict[str, Any]] = []
    offset = 0

    while offset < length:
        chunk_len = min(256, length - offset)
        addr = (start + offset) & 0xFFFF
        try:
            resp = client.command("peek_block", address=addr, count=chunk_len)
        except Exception as exc:  # noqa: BLE001 - old firmware may not expose peek_block.
            fallback = read_range_bytewise(client, (start + offset) & 0xFFFF, length - offset)
            values.extend(fallback["values"])
            errors.append({"address": addr, "peek_block_error": str(exc), "fallback": "bytewise"})
            errors.extend(fallback["errors"])
            return {"start": start & 0xFFFF, "length": length, "values": values, "errors": errors}

        block_values = resp.get("values")
        if resp.get("ok") and isinstance(block_values, list) and len(block_values) == chunk_len:
            values.extend(int(value) & 0xFF for value in block_values)
            offset += chunk_len
            continue

        fallback = read_range_bytewise(client, (start + offset) & 0xFFFF, length - offset)
        values.extend(fallback["values"])
        errors.append({"address": addr, "peek_block_response": resp, "fallback": "bytewise"})
        errors.extend(fallback["errors"])
        return {"start": start & 0xFFFF, "length": length, "values": values, "errors": errors}

    return {"start": start & 0xFFFF, "length": length, "values": values, "errors": errors}


def read_mmio_regs(client: DebugClient) -> dict[str, Any]:
    regs: dict[str, Any] = {}
    errors: list[dict[str, Any]] = []
    for name, addr in MMIO_REGS:
        try:
            resp = client.command("peek", address=addr)
        except Exception as exc:  # noqa: BLE001
            regs[name] = None
            errors.append({"name": name, "address": addr, "error": str(exc)})
            continue
        if resp.get("ok"):
            regs[name] = {"address": addr, "value": int(resp.get("value", 0)) & 0xFF}
        else:
            regs[name] = {"address": addr, "error": resp}
            errors.append({"name": name, "address": addr, "response": resp})
    return {"registers": regs, "errors": errors}


def decode_vectors(vector_range: dict[str, Any] | None) -> dict[str, int] | None:
    if not vector_range:
        return None
    values = vector_range.get("values")
    if not isinstance(values, list) or len(values) < 6 or any(value is None for value in values[:6]):
        return None
    vals = [int(value) & 0xFF for value in values[:6]]
    return {
        "nmi": vals[0] | (vals[1] << 8),
        "reset": vals[2] | (vals[3] << 8),
        "irq": vals[4] | (vals[5] << 8),
    }


def decode_stack(state: dict[str, Any], stack_range: dict[str, Any] | None) -> dict[str, Any] | None:
    if not state.get("ok") or stack_range is None:
        return None
    values = stack_range.get("values")
    if not isinstance(values, list) or len(values) != 256:
        return None

    sp = int(state.get("sp", 0)) & 0xFF
    frames: list[dict[str, Any]] = []
    addr = sp + 1
    while addr + 2 <= 0xFF and len(frames) < 12:
        p = values[addr]
        pcl = values[addr + 1]
        pch = values[addr + 2]
        if p is None or pcl is None or pch is None:
            break
        frames.append(
            {
                "stack": 0x0100 + addr,
                "p": int(p) & 0xFF,
                "return_pc": ((int(pch) & 0xFF) << 8) | (int(pcl) & 0xFF),
                "break_flag": bool(int(p) & 0x10),
                "irq_disabled": bool(int(p) & 0x04),
            }
        )
        addr += 3

    used = 0xFF - sp
    return {"sp": sp, "used_bytes": used, "potential_interrupt_frames": frames}


def analyze_snapshot(snapshot: dict[str, Any]) -> dict[str, Any]:
    observations: list[str] = []
    debug = snapshot.get("debug", {})
    memory = snapshot.get("memory", {})
    state = debug.get("state", {}) if isinstance(debug, dict) else {}
    vectors = decode_vectors(memory.get("vectors") if isinstance(memory, dict) else None)
    stack = decode_stack(state if isinstance(state, dict) else {}, memory.get("stack_page"))

    if isinstance(state, dict) and state.get("ok"):
        pc = int(state.get("pc", 0)) & 0xFFFF
        sp = int(state.get("sp", 0)) & 0xFF
        if pc < 0x0200:
            observations.append(f"PC is in zero page/stack space (${pc:04X}); this is suspicious for BASIC/ROM tests.")
        if vectors:
            if pc == vectors["nmi"]:
                observations.append("PC is exactly the NMI vector target.")
            if vectors["irq"] <= pc <= ((vectors["irq"] + 10) & 0xFFFF):
                observations.append("PC is inside the RAM IRQ handler window.")
            if vectors["nmi"] <= pc <= ((vectors["nmi"] + 10) & 0xFFFF):
                observations.append("PC is inside the RAM NMI handler window.")
        stack_used = 0xFF - sp
        if stack_used > 64:
            observations.append(
                f"Stack is heavily consumed (SP=${sp:02X}, used={stack_used} bytes); "
                "suspect interrupt/BRK storm, runaway recursion, or bad return address."
            )

    mmio = snapshot.get("mmio", {})
    regs = mmio.get("registers", {}) if isinstance(mmio, dict) else {}
    irq_enable = regs.get("RegIrqEnable", {}).get("value") if isinstance(regs.get("RegIrqEnable"), dict) else None
    irq_status = regs.get("RegIrqStatus", {}).get("value") if isinstance(regs.get("RegIrqStatus"), dict) else None
    if irq_enable is not None and irq_status is not None:
        if int(irq_enable) & int(irq_status):
            observations.append(
                f"VGC IRQ output should be asserted: enable=${int(irq_enable):02X}, status=${int(irq_status):02X}."
            )
        elif int(irq_enable) or int(irq_status):
            observations.append(
                f"VGC IRQ registers are non-zero: enable=${int(irq_enable):02X}, status=${int(irq_status):02X}."
            )

    return {"vectors": vectors, "stack": stack, "observations": observations}


def write_json(path: Path, data: dict[str, Any]) -> None:
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def capture_hdmi(args: argparse.Namespace, out_dir: Path) -> dict[str, Any]:
    if args.no_hdmi:
        return {"ok": False, "skipped": True}
    if not args.hdmi_tool.exists():
        return {"ok": False, "error": f"HDMI capture tool not found: {args.hdmi_tool}"}

    out_path = out_dir / "hdmi.png"
    try:
        proc = subprocess.run(
            [str(args.hdmi_tool), str(out_path)],
            cwd=REPO_ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=args.timeout + 10,
            check=False,
        )
    except Exception as exc:  # noqa: BLE001 - snapshot should keep going.
        return {"ok": False, "path": str(out_path), "error": str(exc)}

    return {
        "ok": proc.returncode == 0,
        "path": str(out_path),
        "returncode": proc.returncode,
        "stdout": proc.stdout.strip(),
        "stderr": proc.stderr.strip(),
    }


def main() -> int:
    args = parse_args()
    out_dir = args.out
    out_dir.mkdir(parents=True, exist_ok=True)

    symbols = load_symbols(args.symbols)
    snapshot: dict[str, Any] = {
        "label": args.label,
        "timestamp": _dt.datetime.now().isoformat(timespec="seconds"),
        "host": args.host,
        "port": args.port,
        "http_port": args.http_port,
        "symbol_file": str(args.symbols),
        "symbols_loaded": len(symbols),
        "http": {
            "health": http_get_json(args.host, args.http_port, "/health", args.timeout),
            "sd_status": http_get_json(args.host, args.http_port, "/sd-status", args.timeout),
        },
        "debug": {},
        "memory": {},
        "artifacts": {},
        "errors": [],
    }

    client: DebugClient | None = None
    try:
        client = DebugClient(args.host, args.port, args.timeout)
        snapshot["debug"]["boot_status"] = client.command("boot_status")
        if not snapshot["debug"]["boot_status"].get("fpgaBridgeAvailable", True):
            snapshot["errors"].append("FPGA bridge not available; skipped CPU/memory snapshot")
        else:
            if not args.no_pause:
                snapshot["debug"]["pause"] = client.command("dbg_pause")
            snapshot["debug"]["state"] = client.command("dbg_state")
            state = snapshot["debug"]["state"]
            pc = int(state["pc"]) & 0xFFFF if state.get("ok") and "pc" in state else None
            snapshot["debug"]["pc_symbol"] = nearest_symbol(symbols, pc)
            snapshot["debug"]["cursor"] = client.command("get_cursor")
            snapshot["debug"]["screen"] = client.command("read_screen")

            if pc is not None:
                pc_start = (pc - 16) & 0xFFFF
                snapshot["memory"]["pc_window"] = read_range(client, pc_start, 80)
            snapshot["memory"]["zero_page"] = read_range(client, 0x0000, 0x0100)
            snapshot["memory"]["stack_page"] = read_range(client, 0x0100, 0x0100)
            snapshot["memory"]["basic_workspace"] = read_range(client, 0x0200, 0x0200)
            snapshot["memory"]["vectors"] = read_range(client, 0xFFFA, 6)
            snapshot["mmio"] = read_mmio_regs(client)
            try:
                snapshot["debug"]["breakpoints"] = client.command("dbg_break_list")
            except Exception as exc:  # noqa: BLE001 - older firmware may not expose this.
                snapshot["debug"]["breakpoints"] = {"ok": False, "error": str(exc)}
            try:
                snapshot["debug"]["trace"] = client.command("dbg_trace", count=64)
            except Exception as exc:  # noqa: BLE001 - older firmware may not expose this.
                snapshot["debug"]["trace"] = {"ok": False, "error": str(exc)}
            snapshot["analysis"] = analyze_snapshot(snapshot)
    except Exception as exc:  # noqa: BLE001 - snapshot should keep going.
        snapshot["errors"].append(f"debug capture failed: {exc}")
    finally:
        if client is not None:
            if args.resume:
                try:
                    snapshot["debug"]["resume"] = client.command("dbg_resume")
                except Exception as exc:  # noqa: BLE001
                    snapshot["errors"].append(f"resume failed: {exc}")
            client.close()

    snapshot["artifacts"]["hdmi"] = capture_hdmi(args, out_dir)

    screen = snapshot.get("debug", {}).get("screen", {})
    if isinstance(screen, dict) and isinstance(screen.get("lines"), list):
        (out_dir / "screen.txt").write_text("\n".join(screen["lines"]) + "\n", encoding="utf-8")

    memory_lines: list[str] = []
    for name, data in snapshot.get("memory", {}).items():
        if not isinstance(data, dict):
            continue
        values = data.get("values")
        if not isinstance(values, list):
            continue
        memory_lines.append(f"[{name}] ${int(data['start']) & 0xFFFF:04X} length={int(data['length'])}")
        memory_lines.append(hexdump(int(data["start"]), values))
        memory_lines.append("")
    (out_dir / "memory.txt").write_text("\n".join(memory_lines), encoding="utf-8")

    trace = snapshot.get("debug", {}).get("trace", {})
    if isinstance(trace, dict) and isinstance(trace.get("records"), list):
        trace_lines = []
        for idx, rec in enumerate(trace["records"]):
            if not isinstance(rec, dict):
                continue
            trace_lines.append(
                f"{idx:02d}: PC=${int(rec.get('pc', 0)) & 0xFFFF:04X} "
                f"AB=${int(rec.get('addr', 0)) & 0xFFFF:04X} "
                f"DI=${int(rec.get('din', 0)) & 0xFF:02X} "
                f"DO=${int(rec.get('dout', 0)) & 0xFF:02X} "
                f"A=${int(rec.get('a', 0)) & 0xFF:02X} "
                f"SP=${int(rec.get('sp', 0)) & 0xFF:02X} "
                f"P=${int(rec.get('flags', 0)) & 0xFF:02X} "
                f"ST={int(rec.get('state', 0)) & 0x3F:02d} "
                f"IR=${int(rec.get('ir', 0)) & 0xFF:02X} "
                f"RDY={int(bool(rec.get('rdy')))} WE={int(bool(rec.get('we')))} "
                f"IRQ={int(bool(rec.get('irq')))} NMI={int(bool(rec.get('nmi')))}"
            )
        (out_dir / "trace.txt").write_text("\n".join(trace_lines) + "\n", encoding="utf-8")

    state = snapshot.get("debug", {}).get("state", {})
    symbol = snapshot.get("debug", {}).get("pc_symbol")
    summary_lines = [
        f"Snapshot: {out_dir}",
        f"Label: {args.label or '<none>'}",
        format_cpu_state(state if isinstance(state, dict) else {}, symbol if isinstance(symbol, dict) else None),
    ]
    health_json = snapshot["http"]["health"].get("json") if isinstance(snapshot["http"]["health"], dict) else None
    if isinstance(health_json, dict):
        summary_lines.append(
            "Health: "
            f"bootPhase={health_json.get('bootPhase')} "
            f"sdMounted={health_json.get('sdMounted')} "
            f"fpgaBridgeAvailable={health_json.get('fpgaBridgeAvailable')}"
        )
    if snapshot["errors"]:
        summary_lines.append("Errors:")
        summary_lines.extend(f"  {error}" for error in snapshot["errors"])
    analysis = snapshot.get("analysis", {})
    observations = analysis.get("observations") if isinstance(analysis, dict) else None
    if isinstance(observations, list) and observations:
        summary_lines.append("Observations:")
        summary_lines.extend(f"  {observation}" for observation in observations)
    trace = snapshot.get("debug", {}).get("trace", {})
    if isinstance(trace, dict) and trace.get("ok"):
        summary_lines.append(f"Trace: {out_dir / 'trace.txt'}")
    if isinstance(screen, dict) and isinstance(screen.get("lines"), list):
        visible = [line.rstrip() for line in screen["lines"] if str(line).strip()]
        summary_lines.append("Visible screen:")
        summary_lines.extend(f"  {line}" for line in visible[:12])

    (out_dir / "summary.txt").write_text("\n".join(summary_lines) + "\n", encoding="utf-8")
    write_json(out_dir / "snapshot.json", snapshot)

    print("\n".join(summary_lines))
    if snapshot["artifacts"]["hdmi"].get("ok"):
        print(f"HDMI: {snapshot['artifacts']['hdmi']['path']}")
    print(f"JSON: {out_dir / 'snapshot.json'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
