# ESP ROM / NovaHost Audit

Date: 2026-04-29

Scope: NovaHost ESP32 firmware, staged SD assets, FPGA debug UART protocol, hardware test/MCP command compatibility, and size/asset posture.

## Executive Summary

NovaHost is on the right architectural path: ROMs, the splash asset, and SID curve data are SD-resident instead of embedded in the ESP image. The firmware is now aligned with the current FPGA/VGC debug contract for the screen-read path, CPU state status byte, and 80x50 text mode assumptions.

The original blocker was screen reading. The current FPGA intentionally rejects the old `CMD_READ_SCREEN` and `CMD_READ_COLOR` commands because direct text/color windows were removed. NovaHost now uses debug-only VGC memory reads instead, so hardware tests that need `read_screen`, `read_line`, or `wait_ready` have a current path once the matching bitstream and ESP firmware are flashed.

Status after this audit pass:

- Added debug-only FPGA `CMD_READ_VGC_BLK` for VGC memory-space reads.
- Updated NovaHost to read text/color via VGC char/color memory blocks instead of obsolete screen/color commands.
- Updated NovaHost screen handling to 80x50 / 4000 bytes.
- Updated NovaHost `dbg_state` parsing to expose `paused`, `waiting`, and `stopped`.
- Updated hardware-test row range to use `VgcConstants.ScreenRows`.
- Hardened SD REST JSON output so SD diagnostics and directory entry names are escaped.
- Removed one duplicate 16KB FIO transfer buffer.
- Updated FIO directory iteration to use the mounted NDI header's directory size instead of a hardcoded 192-entry directory.
- Verified focused FPGA/ESP/.NET tests locally.
- Hardened FPGA production build defaults so synth uses `-DVIDEO_720X480 -DGPDI_P_ONLY` unless explicitly overridden.
- Fresh 65C02 FPGA bitstream produced with passing timing; still needs FPGA + ESP flash before hardware tests are meaningful.

## Must Fix Before Hardware Baseline

1. Replace obsolete ESP screen reads.

   Original state:

   - `FpgaBridge::readScreen()` sends `CMD_READ_SCREEN`.
   - `debug_bridge.sv` now returns an error for `CMD_READ_SCREEN` and `CMD_READ_COLOR`.
   - `DebugServer::cmdReadScreen()`, `cmdReadLine()`, and `findTextOnScreen()` depend on `readScreen()`.

   Implemented direction:

   - Do not restore CPU-visible direct VRAM windows.
   - Add a debug-only VGC memory read path, `CMD_READ_VGC_BLK`.
   - Update ESP to read 4000 text bytes and format 50 rows.

2. Update all 80x50 assumptions in NovaHost.

   Original stale assumptions:

   - `_screenBuf[2000]`
   - `for row < 25`
   - `row >= 25`
   - comments saying 25 lines

   Implemented constants:

   - `SCREEN_COLS = 80`
   - `SCREEN_ROWS = 50`
   - `SCREEN_BYTES = 4000`

3. Update hardware tests for 50 rows.

   Fixed: `NovaHostHardwareTests.GetCursor_ReturnsCoordinatesInRange()` now uses `VgcConstants.ScreenRows`.

4. Keep ESP and FPGA protocol definitions synchronized.

   Original protocol mismatch:

   - FPGA CPU state now returns an extra status byte containing paused/stopped/waiting state.
   - ESP reads the byte but ignores it.

   Implemented direction:

   - Extend `FpgaBridge::CpuState` with `waiting`, `stopped`, and `paused`.
   - Report those fields from `dbg_state`.
   - This is needed for WAI/STP debug behavior.

## Hardware Test Contract

The hardware test gate should cover only commands NovaHost is expected to support on real hardware:

- `peek`
- `poke`
- `send_key`
- `type_text`
- `read_screen`
- `read_line`
- `get_cursor`
- `wait_ready`
- `watch`
- `run_cycles`
- `dbg_state`
- `dbg_pause`
- `dbg_resume`
- `cold_start`
- `reload_rom`

Avalonia supports many richer TCP commands such as graphics, sprites, tiles, SID, WTS, debugger breakpoints, disassembly, and screenshots. NovaHost does not currently implement those. That is acceptable if intentional, but MCP tooling should either:

- route those commands only to Avalonia/sim, or
- implement hardware-safe equivalents through BASIC/ROM routines or official device registers.

The hardware server should not pretend to support Avalonia-only commands.

## SD / Asset Management

Implemented and useful:

- ROMs are staged under `/roms/`.
- Boot splash is staged under `/assets/boot/`.
- SID 6581 curve is staged under `/assets/sid/`.
- `GET /sd/path`, `PUT /sd/path`, `DELETE /sd/path`, and directory listing are implemented.
- Whole-file upload/download is the model, which matches the requirement for disk images and soundfonts.
- `PUT` creates parent directories automatically.
- Mounted `.ndi` files are protected from overwrite/delete while mounted.
- SD REST JSON now escapes status strings and directory names.

Gaps / follow-up:

- `boot.json` is staged but not parsed. Boot splash timing, runtime selection, theme, and autoboot are still hardcoded in firmware/BASIC.
- There is no typed REST API layer. The CLI must know that `.ndi`, ROMs, soundfonts, and assets belong in specific directories.
- There is no auth. That is fine for a lab LAN, but it should be explicit before this is treated as a general network appliance.
- HTTP requests are handled synchronously. A large upload/download will monopolize the ESP loop while it runs; OTA/FIO/debug progress should be considered best-effort during a transfer.

## FIO / NDI

Good:

- ESP-side NDI parser uses format version 2 and 32-bit sector counts.
- Host-side NDI tests pass: 46 passed, 0 failed.
- File metadata uses 32-bit size and sector counts internally.
- FIO directory iteration now follows `directory_sector_count` from the mounted image header.

Constraints:

- FIO `LOAD`/`SAVE` are capped at 16KB today.
- FIO status only reports size low/high, so the 6502-facing FIO API is still 16-bit for transferred byte count.
- `NdiImage::write_file()` requires contiguous free sectors, so fragmented images may fail writes even if total free space is sufficient.
- BASIC `MOUNT` still maps the supplied image name to `/<name>.ndi`; if the CLI standardizes on `/disks/`, ROM routines or FIO mount handling should be updated to match.

This is acceptable for small BASIC file I/O right now, but not a general large-file streaming API.

## UART / Concurrency Risk

NovaHost has two consumers of the same FPGA UART:

- `FioEventReader`, which watches for FPGA async event marker bytes.
- `FpgaBridge`, which sends request/response commands and calls `drain()` before operations.

Because `drain()` drops stale bytes, a debug/API operation can theoretically discard an async FIO event if one arrives at the wrong time. The loop polls FIO before debug server work, which reduces the risk, but the design is not robust.

Long-term fix: one serial broker owns the UART, understands both command responses and async events, and dispatches events without blind draining.

## ESP Size / Memory

Current `make compile` result after this audit pass:

- Sketch uses 1,061,721 bytes, 54% of the 1,966,080-byte `min_spiffs` app partition.
- Global variables use 73,040 bytes, 22% of ESP32 dynamic memory.
- The 4KB 80x50 screen buffer is included in that global RAM total.

Largest Nova-owned RAM items:

- `FioDispatcher` shared LOAD/SAVE transfer buffer: 16KB
- `DebugServer` screen buffer: 4KB for 80x50 text.

Diet opportunities:

- Stream FIO load/save in chunks instead of full-buffering.
- Keep ROMs/assets on SD; do not re-embed them.
- Keep OTA/WiFi/SD because those are product features, but expect Arduino/ESP-IDF networking stacks to dominate flash usage.

## Verification

- `make -C e6502.FPGA/test test_debug_bridge test_vgc_regs`: pass.
- `make -C e6502.FPGA/test test_rom_load test_debug_peek test_key_inject test_rom_over_sid test_fio_top`: pass.
- `make -C e6502.ESP32/novahost/test test`: 46 passed, 0 failed.
- `make -C e6502.ESP32/novahost compile`: pass.
- `dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release -p:SkipRomBuild=true --filter FullyQualifiedName~NovaHostHardwareTests`: 12 skipped because `NOVAHOST` is not set.
- `git diff --check`: pass.

## Recommended Fix Order

1. Rebuild a fresh bitstream that includes the debug VGC read path.
2. Flash FPGA + ESP together using the stack deploy script.
3. Run hardware tests as the 65C02 baseline gate.
4. Only after those pass, start BASIC ROM CMOS optimizations.

Fresh bitstream from this pass:

- `/Users/barry/Git/e6502/e6502.FPGA/fpga/bit_backups/e6502.cmos65c02_espread_20260429_222626.bit`
- Timing gate passes: pixel 31.45 MHz / 27.00 MHz, shift 201.25 MHz / 135.01 MHz, SDRAM 186.57 MHz / 100.00 MHz.
