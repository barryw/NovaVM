# FPGA Debug Bridge — ESP32 Test Infrastructure

**Goal:** Run the existing Verilator test suite against real FPGA hardware by using the ESP32 (NovaHost) as a TCP↔UART bridge. Tests connect to `novahost.local:6503` and speak the same JSON protocol as `sim_main.cpp`.

## Architecture

```
┌─────────────┐     TCP/JSON      ┌──────────────┐    UART binary    ┌──────────┐
│  Test Suite  │ ◄──────────────► │   NovaHost   │ ◄──────────────► │   FPGA   │
│  (sim6502)   │   WiFi:6503      │   (ESP32)    │   GPIO16/17      │  top.sv  │
└─────────────┘                   └──────────────┘                   └──────────┘
```

## Existing Infrastructure

- **top.sv** already has debug ports: `dbg_peek_en/addr/data`, `dbg_poke_en/addr/data`, `dbg_pause`, `dbg_cpu_pc/a/x/y/sp/flags`
- **sim_main.cpp** (port 6503) implements the JSON protocol: `peek`, `poke`, `read_screen`, `send_key`, `cold_start`, `wait_ready`, `dbg_state`, etc.
- **NovaHost** (ESP32) already has WiFi TCP server on port 23, FPGA UART on GPIO16/17 (Serial2 at 115200)
- **fpga_top.sv** has wifi_gpio0 driven high, but wifi_rxd/wifi_txd pins are not yet used

## Phase 1: FPGA Debug UART (RTL changes)

### Files to modify:
- `e6502.FPGA/fpga/fpga_top.sv` — add wifi_rxd/wifi_txd ports, instantiate debug UART
- `e6502.FPGA/fpga/ulx3s.lpf` — add K3/K4 pin constraints
- `e6502.FPGA/rtl/top.sv` — wire debug UART to existing dbg_peek/poke ports

### New files:
- `e6502.FPGA/rtl/uart_tx.sv` — UART transmitter (8N1)
- `e6502.FPGA/rtl/debug_bridge.sv` — binary protocol decoder/encoder

### Binary protocol (FPGA ↔ ESP32 UART):

Request format (ESP32 → FPGA):
```
Byte 0: Command
  0x01 = PEEK (read 1 byte)
  0x02 = POKE (write 1 byte)
  0x03 = SEND_KEY (inject keystroke)
  0x04 = READ_SCREEN (read 2000 chars)
  0x05 = READ_COLOR (read 2000 color bytes)
  0x06 = CPU_STATE (get PC, A, X, Y, SP, flags)
  0x07 = PAUSE
  0x08 = RESUME
  0x09 = PEEK_BLOCK (read N bytes from address)
Byte 1-2: Address (big-endian, for PEEK/POKE/PEEK_BLOCK)
Byte 3: Data (for POKE) or Count (for PEEK_BLOCK) or Key (for SEND_KEY)
```

Response format (FPGA → ESP32):
```
Byte 0: Status (0x00 = OK, 0xFF = error)
Byte 1+: Data
  PEEK: 1 byte
  PEEK_BLOCK: N bytes
  READ_SCREEN: 2000 bytes
  READ_COLOR: 2000 bytes
  CPU_STATE: 8 bytes (PChi, PClo, A, X, Y, SP, flags, 0)
  Others: no data (just status)
```

UART settings: 115200 baud, 8N1 (ESP32 Serial2 already configured for this)

### Implementation notes:
- debug_bridge.sv is a state machine that reads commands from UART RX, drives dbg_peek/poke signals, collects responses, sends via UART TX
- READ_SCREEN uses dbg_peek to read char RAM ($AA00-$B1CF) sequentially — 2000 reads at CPU speed
- CPU_STATE reads from the dbg_cpu_* output ports (directly wired, no peek needed)
- SEND_KEY pulses key_valid with the provided key byte
- PAUSE/RESUME drive dbg_pause

## Phase 2: NovaHost Protocol Handler (ESP32 firmware)

### Files to modify:
- `e6502.ESP32/novahost/novahost.ino` — add test server on port 6503

### New files (split novahost into multiple files):
- `e6502.ESP32/novahost/debug_server.cpp/.h` — TCP server + JSON protocol handler
- `e6502.ESP32/novahost/fpga_bridge.cpp/.h` — binary protocol over Serial2

### JSON commands to implement (matching sim_main.cpp):

Priority 1 (needed for basic tests):
- `peek` → PEEK binary command
- `poke` → POKE binary command  
- `send_key` / `type_text` → SEND_KEY binary command (one per char)
- `read_screen` → READ_SCREEN binary command → format as JSON array
- `read_line` → READ_SCREEN then extract row
- `get_cursor` → PEEK $A003 and $A004
- `dbg_state` → CPU_STATE binary command
- `dbg_pause` / `dbg_resume` → PAUSE/RESUME

Priority 2 (needed for full test compat):
- `cold_start` → POKE reset vector, pulse key
- `wait_ready` → poll read_screen for "Ready" text (ESP32-side loop)
- `watch` → poll peek for condition (ESP32-side loop)
- `run_cycles` → not feasible over UART, approximate with timed pause/resume

### ESP32 implementation notes:
- ArduinoJson library for JSON parsing/generation (already in ESP32 Arduino framework)
- TCP server on port 6503, single client at a time
- Serial2 at 115200 for FPGA communication
- Request-response: send binary command, wait for response, format JSON, send TCP
- Timeout: 1 second per command (UART response should be < 1ms for single peek)
- READ_SCREEN is the slowest: 2000 bytes at 115200 = ~170ms

## Phase 3: Test Runner Integration

### No code changes needed if tests use TCP:
- Tests that connect to `localhost:6503` can be redirected to `novahost.local:6503`
- Environment variable: `EMULATOR_HOST=novahost.local` or `--host` flag

### Verify with existing integration tests:
```bash
# Against Verilator sim
sim6502 --host localhost --port 6503

# Against FPGA hardware  
sim6502 --host novahost.local --port 6503
```

## Build & Test Workflow

```bash
# 1. Build FPGA with debug bridge
cd e6502.FPGA/fpga && make fast

# 2. Flash to FPGA
make prog  # or make prog_flash for persistent

# 3. Flash NovaHost with debug server
cd e6502.ESP32/novahost
arduino-cli compile --fqbn esp32:esp32:lolin32 .
arduino-cli upload --fqbn esp32:esp32:lolin32 --port /dev/cu.usbserial-D01457 .

# 4. Run tests against FPGA
cd tests/integration
sim6502 --host novahost.local --port 6503 *.6502
```

## Risks & Mitigations

1. **UART bandwidth:** READ_SCREEN (2000 bytes) takes ~170ms at 115200. Tests with many screen reads will be slower than sim. Mitigation: batch reads, only read what's needed.

2. **Timing sensitivity:** Some tests assume instant execution. FPGA runs at 12.5 MHz CPU, much faster than tests expect. Mitigation: tests already use `wait_ready` for synchronization.

3. **ESP32 memory:** JSON parsing + 2000-byte screen buffer + TCP buffer. ESP32 has 520KB SRAM, should be fine. Monitor with `ESP.getFreeHeap()`.

4. **Serial collision:** FTDI serial (keyboard) and ESP32 serial (debug) are independent paths. No conflict.
