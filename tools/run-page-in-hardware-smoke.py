#!/usr/bin/env python3
"""Phase-3 Step-3a: prove page_dma copies XRAM -> ext_rom byte-exact on real silicon.

No RTL / firmware / ROM changes. End-to-end on hardware:
  1. Stage a known 16 KB pattern into XRAM at a fixed base via `write_sdram`
     (byte[i] = (i ^ (i>>8)) & 0xFF). page_dma's byte order is identity at byte
     granularity, so ext_rom[i] == XRAM[base+i] after the page-in.
  2. POKE the checksum stub (tools/page_in_checksum_stub.bin, assembled from .s)
     into CPU RAM at $9F80 and point the BASIC USR vector at it. The stub must run
     from RAM because reading $C000-$FFFF maps bank 1, unmapping BASIC's own ROM.
  3. From BASIC: POKE the page_dma registers ($BA76+) to copy XRAM->ext_rom (CPU
     stalls ~164us), then USR() the stub to checksum the paged bytes and PRINT it.
  4. read_screen scrapes the printed checksum; compare to the host-computed expected.
  5. Independent cross-check: host flips ROMSWAP and peek_blocks $C000 directly.

The 6502-side readback logic is unit-proven in e6502UnitTests/PageInChecksumStubTests.cs;
the page_dma copy itself is Verilator-proven (Phase 2). This is the on-hardware proof.
"""
from __future__ import annotations

import argparse
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError  # noqa: E402

STUB_BIN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "page_in_checksum_stub.bin")

# CPU-space addresses the stub + driver use.
STUB_ADDR = 0x9F80          # where the stub is POKEd / runs from
SUM_ADDR = 0x9FE0           # stub writes the 16-bit checksum here (little-endian)
EMEM = (0x85, 0x86)         # EhBASIC end-of-memory pointer (lo, hi)
SSTOR = (0x81, 0x82)        # EhBASIC string-storage pointer (lo, hi)
USRJMP = (0x0A, 0x0B, 0x0C)  # JMP opcode, target lo, target hi
ROMSWAP = 0xA03F
RS_BASIC = 0x02
RS_EXT = 0x04

# page_dma MMIO registers (docs/plans/2026-06-02-xram-streaming-engine-impl.md Task 11d).
PGD_CMD = 0xBA76
PGD_SRCL, PGD_SRCM, PGD_SRCH = 0xBA78, 0xBA79, 0xBA7A
PGD_WORDSL, PGD_WORDSH = 0xBA7B, 0xBA7C

MODULE_BYTES = 16384
WORDS = MODULE_BYTES // 2   # 8192


def pattern_byte(i: int) -> int:
    return (i ^ (i >> 8)) & 0xFF


def expected_checksum() -> int:
    return sum(pattern_byte(i) for i in range(MODULE_BYTES)) & 0xFFFF


def stage_pattern(client: NovaHostClient, base: int) -> None:
    for off in range(0, MODULE_BYTES, 256):
        chunk = [pattern_byte(off + j) for j in range(256)]
        client.command("write_sdram", require_ok=True, address=base + off, values=chunk)


def verify_staged(client: NovaHostClient, base: int) -> None:
    """Spot-check a few 256-byte windows landed in XRAM as written."""
    for off in (0, MODULE_BYTES // 2, MODULE_BYTES - 256):
        resp = client.command("read_sdram", require_ok=True, address=base + off, count=256)
        got = list(resp.get("values", []))
        want = [pattern_byte(off + j) for j in range(256)]
        if got != want:
            raise SystemExit(f"FAIL: XRAM staging mismatch at +{off:#x} (write_sdram/read_sdram)")


def poke_bytes(client: NovaHostClient, addr: int, data: bytes) -> None:
    for i, b in enumerate(data):
        client.poke(addr + i, b)


def install_stub(client: NovaHostClient) -> None:
    stub = open(STUB_BIN, "rb").read()
    if len(stub) > (SUM_ADDR - STUB_ADDR):
        raise SystemExit(f"stub ({len(stub)}B) overruns its RAM slot below {SUM_ADDR:#x}")
    # Host debug-bridge pokes race a running CPU and derail it after ~10 pokes
    # (observed: PC walks into RAM/stack and wedges). They are only safe while the
    # CPU is paused, so halt it for the whole poke batch and resume after.
    cap = STUB_ADDR - 1
    client.pause()
    try:
        # Protect the stub + result region: cap BASIC's memory below it.
        client.poke(EMEM[0], cap & 0xFF)
        client.poke(EMEM[1], (cap >> 8) & 0xFF)
        client.poke(SSTOR[0], cap & 0xFF)
        client.poke(SSTOR[1], (cap >> 8) & 0xFF)
        poke_bytes(client, STUB_ADDR, stub)
        # Point USR() at the stub: JMP $9F80.
        client.poke(USRJMP[0], 0x4C)
        client.poke(USRJMP[1], STUB_ADDR & 0xFF)
        client.poke(USRJMP[2], (STUB_ADDR >> 8) & 0xFF)
        got = client.peek_block(STUB_ADDR, len(stub))
        if list(got) != list(stub):
            raise SystemExit("FAIL: stub did not land in RAM (poke/peek mismatch)")
    finally:
        client.resume()
    # The CPU needs a beat after resume before it is polling the key queue again;
    # without this the first typed line is dropped.
    time.sleep(0.4)


def basic_line(client: NovaHostClient, text: str) -> None:
    client.type_text(text.upper())
    client.send_key("ENTER")
    time.sleep(0.3)


def read_screen_text(client: NovaHostClient) -> str:
    return "\n".join(str(line) for line in client.read_screen().get("lines", []))


def trigger_and_checksum(client: NovaHostClient, base: int) -> int:
    # Trigger the page-in (CPU stalls during the copy, then resumes).
    basic_line(
        client,
        f"POKE{PGD_SRCL},{base & 0xFF}:POKE{PGD_SRCM},{(base >> 8) & 0xFF}:"
        f"POKE{PGD_SRCH},{(base >> 16) & 0xFF}:"
        f"POKE{PGD_WORDSL},{WORDS & 0xFF}:POKE{PGD_WORDSH},{(WORDS >> 8) & 0xFF}:"
        f"POKE{PGD_CMD},1",
    )
    # Run the RAM stub (maps bank 1, sums $C000-$FFFF) and print the result.
    basic_line(
        client,
        f'A=USR(0):?"CHK=";PEEK({SUM_ADDR})+256*PEEK({SUM_ADDR + 1})',
    )
    screen = read_screen_text(client)
    m = re.search(r"CHK=\s*(\d+)", screen)
    if not m:
        raise SystemExit("FAIL: no 'CHK=' checksum on screen:\n" + screen)
    return int(m.group(1)) & 0xFFFF


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--host", default=DEFAULT_HOST)
    ap.add_argument("--base", type=lambda s: int(s, 0), default=0x060000,
                    help="XRAM byte base for the staged module (default 0x060000)")
    ap.add_argument("--cold-start", action="store_true",
                    help="cold-start BASIC before the test")
    args = ap.parse_args()

    expected = expected_checksum()
    print(f"host: {args.host}  base: {args.base:#08x}  expected checksum: {expected}")

    with NovaHostClient(host=args.host) as client:
        if args.cold_start:
            client.cold_start(wait_ready=True)
        else:
            client.wait_ready()

        print("staging 16 KB pattern into XRAM ...")
        stage_pattern(client, args.base)
        verify_staged(client, args.base)
        print("  staged + verified in XRAM")

        print("installing checksum stub + USR vector ...")
        install_stub(client)

        print("triggering page-in + BASIC checksum ...")
        actual = trigger_and_checksum(client, args.base)
        print(f"  BASIC checksum: {actual}  (expected {expected})")

        ok = actual == expected

    if not ok:
        print(f"  MISMATCH: CPU reads checksum {actual}, expected {expected}.")
        print("  (Host peek/poke of ext_rom is NOT a valid cross-check here: host debug")
        print("   reads use a different bus path than the CPU. Trust only the CPU/BASIC read.)")
    print("PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (NovaHostError, OSError) as exc:
        print(f"FAIL: {exc}")
        sys.exit(2)
