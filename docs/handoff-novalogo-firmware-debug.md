# NovaLogo Firmware Boot Debug — Handoff

## Goal

Boot NovaLogo on ULX3S hardware by having the ESP32 firmware read `/config/boot.json` from SD, select the `novalogo` runtime, and stream `/roms/novalogo.bin` + `/roms/novalogo_ext.bin` to the FPGA instead of the hardcoded BASIC ROMs.

## What Works

- NovaLogo ROM compiles correctly (16KB base + 16KB extension)
- Staged SD assets now include `/roms/novalogo.bin` and
  `/roms/novalogo_ext.bin`
- Staged `/config/boot.json` defines both `novabasic` and `novalogo`:
  ```json
  {"vm":{"defaultRuntime":"novalogo"},"languages":{"novabasic":{"rom":"/roms/novabasic.bin","extensionRom":"/roms/extension.bin"},"novalogo":{"rom":"/roms/novalogo.bin","extensionRom":"/roms/novalogo_ext.bin"}}}
  ```
- `nova runtime list` reads the config correctly via HTTP
- OTA uploads report "Success" and the firmware DOES change (verified by diagnostic markers)
- The `send_file` HTTP bug is fixed: bulk file responses use bounded
  `write_all(..., allow_probe_write=false)`
- The Avalonia emulator runs Logo correctly against the current ROM resources:
  `DRAW` produces only the centered turtle, and `PU FD 20 PD RT 45 FD 20`
  ends at `x=174 y=45 heading=45`

## What Doesn't Work

Hardware verification is still pending. The current board is not reachable from
this machine, and the latest successful bitstream backup predates the FPGA
`ROMSWAP_LOGO` fix. Do not claim hardware is fixed until a new bitstream is
built/flashed and `tools/run-novalogo-hardware-smoke.py` passes against the
board.

## Root Cause Found

`deserializeJson(doc, f)` returns `NoMemory` when called from inside `loadRomsToFPGA()`. ArduinoJson v7's `JsonDocument` allocates from the ESP32 heap. By the time `loadRomsToFPGA` runs, the heap is exhausted by earlier boot operations (WiFi stack, boot splash image loading, etc.).

Diagnostic marker confirmed: `diag=2 err=NoMemory fsize=209 fpos=0 favail=209` — file opens fine (209 bytes, correct position), but JSON parse fails due to heap pressure.

## What Was Tried

1. **Original approach**: Separate `readRuntimeConfig()` called from `loadRomsToFPGA()` — opens file, parses with ArduinoJson, returns struct. **Failed: NoMemory.**

2. **Reuse existing parse**: `mountConfiguredDrives()` already parses boot.json with ArduinoJson during SD mount. Moved `extractRuntimeConfig(doc)` call inside that function to piggyback on the already-allocated JsonDocument. **Failed: `mountConfiguredDrives` itself also returns early when boot.json has no `mounts` key, AND `deserializeJson` may also fail there with NoMemory** (the sd-status shows `fd_mounts=0 hd_mounts=0` consistent with parse failure).

3. **Current source fix**: `parseRuntimeConfigFromFile()` reads `boot.json` as
   a bounded stack buffer and delegates to `parseBootRuntimeConfigText()`.
   **Zero heap allocation. No ArduinoJson.** Called from `loadRomsToFPGA()`
   where the ROM paths are selected.

## Current State of the Code

File: `e6502.ESP32/novahost/novahost.ino`

- `parseRuntimeConfigFromFile()` is implemented and covered by host tests
- `g_runtime_config` global struct stores the result
- `loadRomsToFPGA()` calls `parseRuntimeConfigFromFile()` then uses `g_runtime_config`
- The stale `extractRuntimeConfig(JsonDocument&)` forward decl has been replaced

## Next Step

1. Wait for board access to return (`/dev/cu.usbserial-*` or NovaHost health).
2. Build/flash a new bitstream containing the `ROMSWAP_LOGO` `$05` source fix.
3. Deploy SD/Firmware assets and run:
   `NOVALOGO_SMOKE=1 NOVALOGO_SMOKE_LEAVE_BOOT_CONFIG=1 tools/flash-ulx3s-stack.sh`
4. If hardware still boots BASIC or corrupts `DRAW`, inspect NovaHost boot logs
   and run `tools/run-novalogo-hardware-smoke.py --host 192.168.1.65 --timeout 60`
   directly after the board is reachable.

## Key Learnings

- ESP32 heap is severely constrained during boot (WiFi + MDNS + boot splash consume most of it)
- ArduinoJson v7 heap-allocates internally — can't be used in low-heap contexts
- The `mountConfiguredDrives` ArduinoJson parse was also likely failing silently
- String-based config parsing (strstr) avoids the heap entirely — 1KB stack buffer is fine
- OTA builds MUST use `make` (not `make compile` + manual espota) due to build-path differences

## 2026-05-29 Update — LOGO DRAW Hardware Corruption

NovaHost now boots NovaLogo far enough to reach the LOGO prompt, but hardware
`DRAW` corrupted the display: the turtle ran/drew a large moire-like shape while
Avalonia showed the correct centered turtle.

Root cause: NovaLogo extension calls write `ROMSWAP_EXTENSION` (`$04`) before
`JSR $C000`, then write `ROMSWAP_LOGO` (`$05`) before returning. Avalonia handled
`$05`, but FPGA `top.sv` only treated `$02` as "return to primary ROM", so
`ext_rom_active` stayed asserted after the extension returned. The CPU then
returned to LOGO code while still fetching extension ROM bytes.

Fix in source:

- `e6502.FPGA/rtl/top.sv` now defines all runtime ROM swap values.
- `$04` selects the extension bank.
- `$01`, `$02`, `$03`, and `$05` all return to the primary runtime bank.

Regression added:

- `make -C e6502.FPGA/test test_novalogo_draw_top`
- Loads real `novalogo.bin` and `novalogo_ext.bin`, boots LOGO, types `DRAW`,
  checks `ext_rom_active == 0`, verifies turtle state at `$9F00`, and asserts
  the graphics plane contains only the small centered turtle. The same
  top-level test now also types `PU`, `FD 20`, `PD`, `RT 45`, `FD 20`, waiting
  for the cursor/prompt after each line, then verifies the turtle ends at
  `x=174 y=45 heading=45`, a diagonal segment was drawn, and the old center
  was not redrawn.
- `make -C e6502.FPGA/test test_romswap_values_top`
- Fast CPU-driven invariant test that writes `$04`, then `$01/$02/$03/$05`
  through the real top-level CPU bus and checks only `$04` selects extension
  while every runtime value returns to primary.
- `dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter "FullyQualifiedName~NovaLogoTests|FullyQualifiedName~RomSwapTests"`
- Emulator coverage includes `DRAW` as a `CS` alias, split-screen prompt
  visibility, cursor off/on during command execution, ROM swap behavior, and
  the reported `FD`/`RT 45`/`FD` behavior. It also checks the banner's displayed
  `BYTES FREE` value against the live LOGO heap pointer instead of accepting a
  hard-coded string.
- `tools/run-novalogo-hardware-smoke.py`
- Smoke now targets either hardware or Avalonia. Hardware remains the default;
  use `--target avalonia --debug-port 6504` for a local emulator instance.
- The smoke runs the same post-`DRAW` turn/forward sequence:
  `PU`, `FD 20`, `PD`, `RT 45`, `FD 20`. It checks turtle memory, verifies the
  diagonal graphics pixels, and verifies the original center was cleared. The
  smoke message includes `turn_fd=x...,y...,h...,diag...,old...`.
- The smoke now waits for each command's state effect and then confirms the
  cursor is enabled on a real prompt row. This avoids stale-prompt false
  positives while still allowing the prompt row to advance after echoed LOGO
  commands.
- Debug `get_cursor` now includes `enabled` on Avalonia and NovaHost so smoke
  tools can pace LOGO commands on "cursor is back at prompt" when firmware has
  the new endpoint. The smoke script remains compatible with older x/y-only
  firmware by falling back to prompt-row checks and state predicates.
- NovaHost staged SD assets now include both NovaLogo ROMs and advertise both
  `novabasic` and `novalogo` in `/config/boot.json`. This prevents a normal
  stack deploy from silently reverting the SD card to a BASIC-only runtime
  config.
- `tools/flash-ulx3s-stack.sh` now has an optional `NOVALOGO_SMOKE=1`
  post-deploy gate. It runs the same hardware smoke after SD sync/ROM reload,
  with `NOVALOGO_SMOKE_LEAVE_BOOT_CONFIG=1` available when the board should be
  left booting Logo after verification.

Current evidence:

- ROMSWAP value test passes: `$05` returns primary and final bank is primary.
- Verilator top-level test passes: centered turtle, `center=39`, `total=39`.
- Logo extension movement now rounds signed `distance * sin/cos / 128`
  products before the integer shift. This preserves the hardware math ABI
  (`cos(0) == $7F`) while making cardinal moves land on the requested pixel
  distance instead of truncating one pixel.
- Verilator turn/forward interaction passes: `x=174 y=45 heading=45`,
  `diagonal=7`, `old_center=0`.
- Emulator LOGO/RomSwap subset passes: 81 passed, 0 failed, 0 skipped.
- `python3 -m py_compile tools/run-novalogo-hardware-smoke.py tools/novahost_client.py` passes.
- `dotnet build e6502.Nova/e6502.Nova.csproj -c Release` passes.
- `dotnet build e6502.Avalonia/e6502.Avalonia.csproj -c Release` passes.
- Live Avalonia smoke passes against a fresh port-6504 instance with current
  ROM resources:
  `PASS NovaLogo DRAW: center_pixels=39 total_pixels=39; turn_fd=x174,y45,h45,diag7,old0`.
- The default Avalonia debug target on port 6502 has also been relaunched with
  current ROM resources and passes the same smoke command:
  `tools/run-novalogo-hardware-smoke.py --target avalonia --timeout 30`.
- `make -C e6502.ESP32/novahost/test test` passes. The NDI host test now
  matches the checked-in `SHOWCASE` fixture and verifies case-insensitive
  lookup against a real `AUTOBOOT.bin` entry.
- `python3 e6502.ESP32/novahost/test/test_wifi_contract.py` passes. Bulk HTTP
  file responses use the non-probe bounded write path, the active-high LED
  contract matches the current user/music/operator LED mux, the staged SD
  runtime config/ROM asset contract covers NovaLogo, and the stack deploy path
  exposes the optional NovaLogo post-deploy smoke gate.
- CPU/core regression sweep passes without synthesis:
  - `make -C e6502.FPGA/test test_cpu_dormann_6502 test_cpu_dormann_65c02 test_cpu_stz test_cpu_memory_write_opcodes`
  - 6502 functional pass loop reached after 92,606,025 cycles.
  - 65C02 extended opcode pass loop reached after 66,062,425 cycles.
  - STZ mode regression passed 5/0.
  - CPU memory-write opcode regression passed 66/0.
- MMIO/VGC/blitter/math regressions pass without synthesis:
  - `make -C e6502.FPGA/test test_mmio_bus_cycles_top test_vgc_cmd_readback_hazard_top test_blitter_cmd_single_pulse_top`
  - `make -C e6502.FPGA/test test_blitter test_vgc_blitter_scroll test_math_copro`
  - Standalone blitter passed 122/0, VGC/blitter scroll passed 19/0,
    math coprocessor passed 78/0, and the top-level MMIO/blitter command
    hazard group exited cleanly.
- Full e6502 unit run passes with hardware/debug integration skips:
  `dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --no-build`
  reported 1279 passed, 0 failed, 12 skipped.
- `make -C e6502.ESP32/novahost sd-assets` stages
  `/roms/novalogo.bin`, `/roms/novalogo_ext.bin`, `/roms/novabasic.bin`, and
  `/roms/extension.bin`.
- The latest successful bitstream backup
  `e6502.20260529_155306.logo_mmio_readaddr_split.bit` was built before this
  ROMSWAP_LOGO source fix, so it does not prove the hardware fix.
- Current board access check failed: no `/dev/cu.usbserial-*` or
  `/dev/cu.usbmodem*` device was present, and NovaHost health at
  `192.168.1.65` timed out.
- Do not claim the hardware is fixed until a new bitstream containing the
  ROMSWAP_LOGO `top.sv` change is built, flashed, and LOGO `DRAW` is verified
  on the board.
