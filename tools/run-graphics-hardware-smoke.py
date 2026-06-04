#!/usr/bin/env python3
"""Phase 4b: prove the REAL GRAPHICS paged-library module loads + dispatches on silicon.

End-to-end on the physical ULX3S, no RTL / firmware / ROM / flash change — the
same protected-RAM design as tools/run-libcall-hardware-smoke.py (the Phase-3
TEST-module proof), retargeted at the GRAPHICS module (id=$01):

  1. STAGE  modules/graphics/graphics.bin (the 16 KB GRAPHICS module, id=$01)
     into XRAM at --base ($060000 = shelf slot 0, the loader's compile-time map)
     via write_sdram in 256-byte chunks; read_sdram spot-checks first+last chunk.
  2. INSTALL (CPU paused): cap BASIC's EMEM/SSTOR to $9BFF, POKE
     tests/asm/libcall.bin -> $9C00 (the real loader, whose modtab now resolves
     MODULE_ID_GRAPHICS -> slot 0), POKE tools/graphics_smoke_driver.bin -> $9F80,
     peek-verify, point USR (JMP $0A) at $9F80, resume.
  3. TRIGGER  "A=USR(0)" — the driver fills the mailbox FRESH (MOD_ID=$01,
     FN=$00 GCLS), JSRs lib_call ($9C00) which pages the module in, validates its
     "NL"/$01 header, flips ROMSWAP to bank 1, dispatches gfn_gcls (issues
     VCMD_GCLS, waits, STATUS=LERR_OK), then stashes STATUS/RESIDENT to $9FE0.
  4. READ  the outcome with peek_block($9FE0, 6) (plain high RAM, host read valid).
  5. ASSERT STATUS==0x00 (LERR_OK) and RESIDENT==0x01 (MODULE_ID_GRAPHICS),
     proving the GRAPHICS module became resident and a real VGC op dispatched.

GCLS uses VGC_P/VGC_CMD MMIO + LIB_SCRATCH only (NOT the module BSS band at
$0320+), so this is safe under a live BASIC program without the deferred
cross-runtime band reservation. The board's s3 bitstream already carries the
page-in hardware ($BA76) + ROMSWAP ($A03F); nothing is flashed.
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

GRAPHICS_BIN = os.path.join(REPO, "modules", "graphics", "graphics.bin")
LIBCALL_BIN = os.path.join(REPO, "tests", "asm", "libcall.bin")
DRIVER_BIN = os.path.join(HERE, "graphics_smoke_driver.bin")

LIBCALL_ADDR = 0x9C00
DRIVER_ADDR = 0x9F80
STASH_ADDR = 0x9FE0
STASH_LEN = 6               # RESULT(4) + STATUS(1) + RESIDENT(1)

MEM_CAP = 0x9BFF
EMEM = (0x85, 0x86)
SSTOR = (0x81, 0x82)
USRJMP = (0x0A, 0x0B, 0x0C)

EXPECT_STATUS = 0x00        # LERR_OK
EXPECT_RESIDENT = 0x01      # MODULE_ID_GRAPHICS

MODULE_BYTES = 16384


def read_bin(path: str, expect_len: int | None = None) -> bytes:
    data = open(path, "rb").read()
    if expect_len is not None and len(data) != expect_len:
        raise SystemExit(f"FAIL: {path} is {len(data)}B, expected {expect_len}B")
    return data


def stage_module(client: NovaHostClient, base: int, module: bytes) -> None:
    for off in range(0, len(module), 256):
        chunk = list(module[off:off + 256])
        client.command("write_sdram", require_ok=True, address=base + off, values=chunk)


def verify_staged(client: NovaHostClient, base: int, module: bytes) -> None:
    for off in (0, len(module) - 256):
        resp = client.command("read_sdram", require_ok=True, address=base + off, count=256)
        got = list(resp.get("values", []))
        want = list(module[off:off + 256])
        if got != want:
            raise SystemExit(f"FAIL: XRAM staging mismatch at +{off:#x}")


def poke_bytes(client: NovaHostClient, addr: int, data: bytes) -> None:
    for i, b in enumerate(data):
        client.poke(addr + i, b)


def install(client: NovaHostClient, loader: bytes, driver: bytes) -> None:
    if LIBCALL_ADDR + len(loader) > DRIVER_ADDR:
        raise SystemExit(f"FAIL: loader ({len(loader)}B) overruns driver at {DRIVER_ADDR:#x}")
    if DRIVER_ADDR + len(driver) > STASH_ADDR:
        raise SystemExit(f"FAIL: driver ({len(driver)}B) overruns stash at {STASH_ADDR:#x}")

    client.pause()
    try:
        client.poke(EMEM[0], MEM_CAP & 0xFF)
        client.poke(EMEM[1], (MEM_CAP >> 8) & 0xFF)
        client.poke(SSTOR[0], MEM_CAP & 0xFF)
        client.poke(SSTOR[1], (MEM_CAP >> 8) & 0xFF)

        poke_bytes(client, LIBCALL_ADDR, loader)
        poke_bytes(client, DRIVER_ADDR, driver)

        if list(client.peek_block(LIBCALL_ADDR, len(loader))) != list(loader):
            raise SystemExit("FAIL: loader did not land in RAM (poke/peek mismatch)")
        if list(client.peek_block(DRIVER_ADDR, len(driver))) != list(driver):
            raise SystemExit("FAIL: driver did not land in RAM (poke/peek mismatch)")

        client.poke(USRJMP[0], 0x4C)
        client.poke(USRJMP[1], DRIVER_ADDR & 0xFF)
        client.poke(USRJMP[2], (DRIVER_ADDR >> 8) & 0xFF)
    finally:
        client.resume()
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
                    help="XRAM byte base for the staged GRAPHICS module "
                         "(default 0x060000, shelf slot 0 — must match lib_call's map)")
    ap.add_argument("--cold-start", action="store_true")
    args = ap.parse_args()

    module = read_bin(GRAPHICS_BIN, MODULE_BYTES)
    loader = read_bin(LIBCALL_BIN)
    driver = read_bin(DRIVER_BIN)
    print(f"host: {args.host}  base: {args.base:#08x}")
    print(f"loader: {len(loader)}B @ {LIBCALL_ADDR:#06x}   "
          f"driver: {len(driver)}B @ {DRIVER_ADDR:#06x}   module: {len(module)}B")

    with NovaHostClient(host=args.host) as client:
        if args.cold_start:
            client.cold_start(wait_ready=True)
        else:
            client.wait_ready()

        print("staging 16 KB GRAPHICS module into XRAM ...")
        stage_module(client, args.base, module)
        verify_staged(client, args.base, module)
        print("  staged + spot-checked in XRAM")

        print("installing loader + graphics driver stub + USR vector ...")
        install(client, loader, driver)

        print("triggering A=USR(0) (driver -> lib_call -> GCLS) ...")
        basic_line(client, "A=USR(0)")

        stash = client.peek_block(STASH_ADDR, STASH_LEN)

    status = stash[4]
    resident = stash[5]
    print("  stashed bytes @ {:#06x}: {}".format(
        STASH_ADDR, " ".join(f"{b:02X}" for b in stash)))
    print(f"  STATUS   = 0x{status:02X}  (expect 0x{EXPECT_STATUS:02X} LERR_OK)")
    print(f"  RESIDENT = 0x{resident:02X}  (expect 0x{EXPECT_RESIDENT:02X} MODULE_ID_GRAPHICS)")

    ok = (status == EXPECT_STATUS and resident == EXPECT_RESIDENT)
    print("PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (NovaHostError, OSError) as exc:
        print(f"FAIL: {exc}")
        sys.exit(2)
