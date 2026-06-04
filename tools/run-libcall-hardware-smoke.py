#!/usr/bin/env python3
"""Task T10: prove the REAL paged-library loader (lib_call) works on silicon.

End-to-end on the physical ULX3S, no RTL / firmware / ROM / flash change. The
board's flashed s3 bitstream already carries the page-in hardware ($BA76) and
ROMSWAP ($A03F) — the exact same RTL the Verilator suite passed 23/23 against.
This drives that hardware through the production code path:

  1. STAGE  tests/asm/testmod.bin (the 16 KB TEST module, id=$7F) into XRAM at
     --base ($060000 = shelf slot 0, the loader's compile-time map) via
     write_sdram in 256-byte chunks; read_sdram spot-checks first+last chunk.
  2. INSTALL (CPU paused — host pokes are only safe while halted):
       - cap BASIC's end-of-mem (EMEM) + string-storage (SSTOR) to $9BFF so
         BASIC never touches $9C00+ where the loader + driver + stash live,
       - POKE tests/asm/libcall.bin  -> $9C00 (the real loader),
       - POKE tools/libcall_driver_stub.bin -> $9F80 (the driver),
       - peek-verify both landed byte-exact,
       - point the BASIC USR vector (JMP $0A) at $9F80,
       - resume, settle.
     The driver sets the mailbox itself, so $0300+ need NOT be protected.
  3. TRIGGER  "?USR(0)" in immediate mode — runs the driver via the USR vector.
     The driver fills the mailbox FRESH (MOD_ID=$7F, FN=ECHO, ARG0=$DEADBEEF),
     JSRs lib_call ($9C00) which pages the module in, validates its header,
     flips ROMSWAP to bank 1, dispatches FN 0 (ECHO copies ARG0->RESULT), then
     stashes RESULT/STATUS/RESIDENT to protected high RAM at $9FE0.
  4. READ  the outcome with host peek_block($9FE0, 6). This is plain low/high
     RAM (NOT ROM / NOT MMIO), so the host debug-bridge read path is valid here
     — unlike reading $C000+ ext_rom, which would map bank 1 on the host bus.
  5. ASSERT RESULT==0xDEADBEEF, STATUS==0x00 (LERR_OK), RESIDENT==0x7F
     (MODULE_ID_TEST, proving the load actually became resident).

Protected-RAM / no-flash design: nothing is flashed and no ROM is rebuilt. The
loader, driver, and result stash all live in RAM above the BASIC memory cap, so
a normal interactive BASIC session can run "?USR(0)" without clobbering them,
and the 6-byte outcome survives until the host reads it back.

The 6502 logic is unit-proven (MSTest) and Verilator-proven (23/23, same RTL);
this is the silicon proof. DO NOT point this at a bitstream lacking the page-in
hardware — the board's s3 bitstream already has it, so no new flash is needed.
"""
from __future__ import annotations

import argparse
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from novahost_client import DEFAULT_HOST, NovaHostClient, NovaHostError  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)

# Binaries staged / POKEd onto the board.
TESTMOD_BIN = os.path.join(REPO, "tests", "asm", "testmod.bin")
LIBCALL_BIN = os.path.join(REPO, "tests", "asm", "libcall.bin")
DRIVER_BIN = os.path.join(HERE, "libcall_driver_stub.bin")

# CPU-space layout.
LIBCALL_ADDR = 0x9C00       # real loader entry (lib_call)
DRIVER_ADDR = 0x9F80        # driver stub; runs via USR vector
STASH_ADDR = 0x9FE0         # driver stashes 6 outcome bytes here (protected RAM)
STASH_LEN = 6               # RESULT(4) + STATUS(1) + RESIDENT(1)

# Cap BASIC below $9C00 so it never touches the loader/driver/stash region.
MEM_CAP = 0x9BFF
EMEM = (0x85, 0x86)         # EhBASIC end-of-memory pointer (lo, hi)
SSTOR = (0x81, 0x82)        # EhBASIC string-storage pointer (lo, hi)
USRJMP = (0x0A, 0x0B, 0x0C)  # JMP opcode, target lo, target hi

# Expected outcome of FN 0 ECHO on ARG0 = $DEADBEEF, with the TEST module loaded.
EXPECT_RESULT = 0xDEADBEEF
EXPECT_STATUS = 0x00        # LERR_OK
EXPECT_RESIDENT = 0x7F      # MODULE_ID_TEST

MODULE_BYTES = 16384


def read_bin(path: str, expect_len: int | None = None) -> bytes:
    data = open(path, "rb").read()
    if expect_len is not None and len(data) != expect_len:
        raise SystemExit(f"FAIL: {path} is {len(data)}B, expected {expect_len}B")
    return data


def stage_module(client: NovaHostClient, base: int, module: bytes) -> None:
    """Write the 16 KB module into XRAM at `base` in 256-byte chunks."""
    for off in range(0, len(module), 256):
        chunk = list(module[off:off + 256])
        client.command("write_sdram", require_ok=True, address=base + off, values=chunk)


def verify_staged(client: NovaHostClient, base: int, module: bytes) -> None:
    """Spot-check the first and last 256-byte windows round-tripped via XRAM."""
    for off in (0, len(module) - 256):
        resp = client.command("read_sdram", require_ok=True, address=base + off, count=256)
        got = list(resp.get("values", []))
        want = list(module[off:off + 256])
        if got != want:
            raise SystemExit(f"FAIL: XRAM staging mismatch at +{off:#x} (write_sdram/read_sdram)")


def poke_bytes(client: NovaHostClient, addr: int, data: bytes) -> None:
    for i, b in enumerate(data):
        client.poke(addr + i, b)


def install(client: NovaHostClient, loader: bytes, driver: bytes) -> None:
    """Place the loader + driver in RAM and wire the USR vector (CPU paused)."""
    if LIBCALL_ADDR + len(loader) > DRIVER_ADDR:
        raise SystemExit(
            f"FAIL: loader ({len(loader)}B at {LIBCALL_ADDR:#x}) overruns driver at {DRIVER_ADDR:#x}")
    if DRIVER_ADDR + len(driver) > STASH_ADDR:
        raise SystemExit(
            f"FAIL: driver ({len(driver)}B at {DRIVER_ADDR:#x}) overruns stash at {STASH_ADDR:#x}")

    # Host debug-bridge pokes race a running CPU and derail it; only safe while
    # paused. Halt for the whole batch and resume after.
    client.pause()
    try:
        # Protect $9C00+ : cap BASIC's memory below the loader.
        client.poke(EMEM[0], MEM_CAP & 0xFF)
        client.poke(EMEM[1], (MEM_CAP >> 8) & 0xFF)
        client.poke(SSTOR[0], MEM_CAP & 0xFF)
        client.poke(SSTOR[1], (MEM_CAP >> 8) & 0xFF)

        poke_bytes(client, LIBCALL_ADDR, loader)
        poke_bytes(client, DRIVER_ADDR, driver)

        got_loader = client.peek_block(LIBCALL_ADDR, len(loader))
        if list(got_loader) != list(loader):
            raise SystemExit("FAIL: loader did not land in RAM (poke/peek mismatch)")
        got_driver = client.peek_block(DRIVER_ADDR, len(driver))
        if list(got_driver) != list(driver):
            raise SystemExit("FAIL: driver did not land in RAM (poke/peek mismatch)")

        # Point USR() at the driver: JMP $9F80.
        client.poke(USRJMP[0], 0x4C)
        client.poke(USRJMP[1], DRIVER_ADDR & 0xFF)
        client.poke(USRJMP[2], (DRIVER_ADDR >> 8) & 0xFF)
    finally:
        client.resume()
    # The CPU needs a beat after resume before it polls the key queue again;
    # without this the first typed line is dropped.
    time.sleep(0.4)


def basic_line(client: NovaHostClient, text: str) -> None:
    client.type_text(text.upper())
    client.send_key("ENTER")
    time.sleep(0.3)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", default=DEFAULT_HOST)
    ap.add_argument("--base", type=lambda s: int(s, 0), default=0x060000,
                    help="XRAM byte base for the staged TEST module (default 0x060000, "
                         "shelf slot 0 — must match lib_call's compile-time map)")
    ap.add_argument("--cold-start", action="store_true",
                    help="cold-start BASIC before the test")
    args = ap.parse_args()

    module = read_bin(TESTMOD_BIN, MODULE_BYTES)
    loader = read_bin(LIBCALL_BIN)
    driver = read_bin(DRIVER_BIN)
    print(f"host: {args.host}  base: {args.base:#08x}")
    print(f"loader: {len(loader)}B @ {LIBCALL_ADDR:#06x}   "
          f"driver: {len(driver)}B @ {DRIVER_ADDR:#06x}   "
          f"module: {len(module)}B")

    with NovaHostClient(host=args.host) as client:
        if args.cold_start:
            client.cold_start(wait_ready=True)
        else:
            client.wait_ready()

        print("staging 16 KB TEST module into XRAM ...")
        stage_module(client, args.base, module)
        verify_staged(client, args.base, module)
        print("  staged + spot-checked in XRAM")

        print("installing loader + driver stub + USR vector ...")
        install(client, loader, driver)

        print("triggering ?USR(0) (driver -> lib_call -> ECHO) ...")
        basic_line(client, "?USR(0)")

        stash = client.peek_block(STASH_ADDR, STASH_LEN)

    result = stash[0] | (stash[1] << 8) | (stash[2] << 16) | (stash[3] << 24)
    status = stash[4]
    resident = stash[5]

    print("  stashed bytes @ {:#06x}: {}".format(
        STASH_ADDR, " ".join(f"{b:02X}" for b in stash)))
    print(f"  RESULT   = 0x{result:08X}  (expect 0x{EXPECT_RESULT:08X})")
    print(f"  STATUS   = 0x{status:02X}        (expect 0x{EXPECT_STATUS:02X})")
    print(f"  RESIDENT = 0x{resident:02X}        (expect 0x{EXPECT_RESIDENT:02X})")

    ok = (result == EXPECT_RESULT and status == EXPECT_STATUS and resident == EXPECT_RESIDENT)
    print("PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (NovaHostError, OSError) as exc:
        print(f"FAIL: {exc}")
        sys.exit(2)
