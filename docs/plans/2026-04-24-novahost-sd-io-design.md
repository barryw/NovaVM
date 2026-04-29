# NovaHost SD I/O Design

*Date: 2026-04-24*

## Summary

Give NovaBASIC programs on FPGA hardware the same file I/O they already
have on Avalonia — `LOAD`, `SAVE`, `DIR`, `MOUNT`, `CD`, `SFLOAD` — backed
by an SD card. ESP32 (NovaHost) owns the SD card and understands the `.ndi`
container format. FPGA gains one small register-file RTL module
(`fio.sv`) and one async debug-bridge event. The 6502-visible wire
contract ($B9A0-$B9EF) is identical to Avalonia.

## Non-goals

- Wavetable / soundfont streaming (`SFLOAD` specifically). Deferred until
  WTS architecture is chosen (ESP-side synth vs FPGA-side). Shape of this
  design does not preclude either; see "Future" below.
- Creating `.ndi` images from within Nova. All images created offline via
  the `e6502.NDI` CLI.
- Loose `.bas` / `.gfx` / `.mid` files on SD. Only `.ndi` containers and
  `.sf2` soundfonts live at the FAT32 layer.
- Hot-swap SD detection, card encryption, or any form of tamper resistance.

## Architecture overview

```
  ┌──────────┐  MMIO writes       ┌─────────┐  async event  ┌──────────┐
  │  6502    │ ─────────────────▶ │ fio.sv  │ ─────────────▶│          │
  │ (EhBASIC │    $B9A0-$B9EF     │ 80-byte │   FIO_EVENT   │  ESP32   │
  │  tokens) │ ◀───────────────── │ regfile │ ◀─────────────│ NovaHost │
  └──────────┘    FioStatus       └─────────┘   debug-      │  + SD    │
                                                 bridge      └──────────┘
                                                 UART              │
                                                                   │ SPI
                                                                   ▼
                                                              ┌────────┐
                                                              │ FAT32  │
                                                              │   SD   │
                                                              └────────┘
```

1. CPU writes cmd register $B9A0 (or any other FIO reg).
2. `fio.sv` latches the write; on non-zero write to $B9A0, asserts
   `fio_event` for one cycle.
3. Debug bridge packetizes `fio_event` as a new async `FIO_EVENT` UART
   message. FPGA→ESP direction.
4. ESP receives `FIO_EVENT`, uses `CMD_PEEK_FIO_BANK` to read the full
   80-byte bank in one shot. Parses `FioCmd` and arguments.
5. ESP does SD work: resolves mount device, traverses `.ndi`, reads/writes
   bytes.
6. ESP streams payload into CPU RAM via `CMD_POKE_RAM_BLK` (for `LOAD`,
   `GLOAD`), or reads from CPU RAM via `CMD_PEEK_RAM_BLK` (for `SAVE`,
   `GSAVE`).
7. ESP writes result fields (`FioStatus`, `FioSizeL/H`, etc) back via
   `CMD_POKE_FIO_BANK`. `FioStatus` flipping from 0 → 2 (ok) or 3 (err) is
   how Nova knows the op completed.
8. BASIC program busy-waits on `FioStatus` — same pattern as Avalonia.

## Why ESP parses `.ndi` (and not FPGA/6502)

Avalonia's `e6502.Avalonia/Hardware/FileIoController.cs` already plays the
"translate Nova FIO commands ↔ .ndi" role. Moving that logic to ESP keeps
the translator on the host side of the system. The 6502 stays dumb
(pokes registers), the FPGA stays simple (RTL regfile + event),
`NdiImage.cs`'s proven parser gets ported to C++ once.

Alternative B (FPGA/6502 parses .ndi) would require porting a non-trivial
BAM+directory parser to 6502 assembly inside 40KB program space, with no
host-side code reuse. Alternative C (hybrid) adds two places where state
can desync. A is the clear winner; this decision was locked in Q1 of the
brainstorm.

## Why FPGA pushes (not ESP polls)

MMIO semantics demand synchronous peripheral notification. When 6502
writes $B9A0, the peripheral must react — not "eventually notice when it
gets around to polling." Polling breaks the contract, wastes UART
bandwidth at idle, creates races between CPU writes and peek samples, and
places a permanent latency floor (~10ms) under all future work. The FPGA
is on the bus; the only correct answer is to extend MMIO across the chip
boundary via async event.

The cost is small: ~80 bytes of BRAM + a `cmd_written` pulse + one new
debug-bridge packet type.

## FPGA RTL: `rtl/fio.sv`

New module, ~150 lines. Responsibilities:

- Decode CPU bus writes to $B9A0-$B9EF (80 bytes). Back with a small BRAM
  or dist-RAM register file.
- Decode CPU reads of the same range, returning the register file
  contents. No latency surprises (use a clocked readback latch like VGC).
- On write to $B9A0 (cmd register) with data != 0, assert `fio_event` for
  one clock.
- Expose `fio_event` output to the top-level debug bridge.
- Expose full 80-byte bank to debug bridge as readable/writable window
  (via `dbg_addr`/`dbg_rdata`/`dbg_we` extension).

The module does not interpret commands. It is pure storage + write
notification. All interpretation lives in the ESP.

Synthesis cost estimate: 80 bytes = 1 DP16KD if we pack 8-bit wide, or use
distributed RAM (more LUTs but no BRAM). Diag before committing.

## Debug-bridge protocol extensions

Existing commands (ESP→FPGA): `CMD_PEEK`, `CMD_POKE`, `CMD_POKE_ROM`,
`CMD_POKE_ROM_BLK`, `CMD_RESET_HOLD/REL`, `CMD_POKE_SDRAM_BLK`.

Additions:

| Cmd | Direction | Payload | Purpose |
|---|---|---|---|
| `CMD_PEEK_FIO_BANK` | ESP→FPGA | (none) | Returns all 80 bytes in one packet |
| `CMD_POKE_FIO_BANK` | ESP→FPGA | 80 bytes | Writes all 80 bytes in one packet |
| `CMD_POKE_RAM_BLK` | ESP→FPGA | addr(2) + len(2) + data | Writes block to CPU RAM |
| `CMD_PEEK_RAM_BLK` | ESP→FPGA | addr(2) + len(2) | Reads block from CPU RAM |
| `EVENT_FIO` | FPGA→ESP | (none) | Async: cmd register was written |

`_RAM_BLK` variants are needed because `LOAD` can write up to 40KB of
payload into CPU RAM; poking one byte at a time over UART would take
minutes. `_ROM_BLK` precedent exists — generalize the block-write path.

## Drive model

- **HDs**: ESP scans SD root for glob `hd*.ndi` at boot. Auto-mounts each
  match to HD0, HD1, HD2... in sorted order. All use the NDI v2 format
  with 32-bit sector fields; the initial hard-drive target is 64MB.
  Persistent across reboots.
- **FDs**: Four user-mountable slots (FD0-FD3), via `MOUNT
  "FD0:","path/to/image"`. Argument is SD-relative path, `.ndi` extension
  implicit. Can be anywhere on SD including nested subdirs for
  organization.
- **Creation**: only via `e6502.NDI` CLI. Nova itself can
  `LOAD`/`SAVE`/`MKDIR`/`RMDIR` inside existing images but cannot create
  new ones.
- **Case**: SD filesystem is case-preserving but comparisons are
  case-insensitive (FAT32 behavior). Nova-side filenames inside `.ndi` are
  case-preserving ASCII.

## SD layout

- Root `/`: any number of subdirs for user organization + `hd*.ndi` at
  root for auto-mount. Mixed:
  ```
  /hd0.ndi            # auto-mounts HD0:
  /hd1.ndi            # auto-mounts HD1:
  /games/spaceinv.ndi # user mounts via MOUNT "FD0:","games/spaceinv"
  /soundfonts/fluidsynth.sf2  # via SFLOAD "soundfonts/fluidsynth"
  /demos/*.ndi
  ```
- No loose `.bas`/`.gfx`/`.mid` files at the FAT32 layer. All
  Nova-visible files live inside `.ndi` containers.
- `.sf2` soundfonts are the one FAT32-layer exception because the synth
  streams them raw (too big to fit a `.ndi` practically and no benefit to
  containerizing).

## Autoboot flow

1. FPGA bitstream loads from SPI flash.
2. ESP boots, mounts SD, scans for `hd*.ndi` and auto-mounts each.
3. ESP streams ROM to FPGA via `CMD_POKE_ROM_BLK` (existing flow).
4. ESP releases CPU reset via `CMD_RESET_REL` (existing flow).
5. 6502 boots EhBASIC. "Ready" prints to screen.
6. ESP observes Ready via serial console / scroll buffer.
7. ESP checks key FIFO via `CMD_PEEK` at the key-FIFO address: if SPACE
   was held during boot, skip autoboot. Drop to Ready prompt.
8. Otherwise, ESP searches mounted drives in order HD0→HD1→…→FD0→…→FD3
   for a directory entry named exactly `autoboot` (case-insensitive). The
   first match wins.
9. If found, ESP injects keystrokes via the existing `type_text` +
   `send_key` mechanism: `LOAD"<winning-drive>:autoboot":RUN<ENTER>`.
10. If no drive has an autoboot file, user lands at the bare Ready prompt.

The `autoboot` filename has no extension. It is a tokenized BASIC program
stored inside the `.ndi` with file type BAS (FioDirType=0). Extension
would be decorative — the directory entry already carries type info and
the filename itself is our container-level convention marker.

## BASIC-visible contract

Unchanged from Avalonia. All existing tokens work identically:

- `LOAD "drive:path"` — load program, optionally prefixed with
  device. Bare `LOAD "file"` uses the current drive set by last `CD`.
- `SAVE "drive:path"` — save current program.
- `DIR "pattern"` — list files matching pattern on current drive.
- `DIROPEN / DIRNEXT / DIRNAM$ / DIRSIZ / DIRTYP` — iterate directory
  stream.
- `MOUNT "FDx:","image"` — mount `image.ndi` from SD path (subdirs
  allowed).
- `UNMOUNT "FDx:"` — unmount.
- `CD "drive:/path"` — change directory within current mount.
- `MKDIR / RMDIR` — manage directories inside mounted `.ndi`.
- `GSAVE / GLOAD / TSAVE / TLOAD` — graphics and tile I/O, same register
  bank.
- `SFLOAD "name"` — soundfont load. Stubbed in Phase 1, full impl tied to
  WTS phase.

## Error handling

`FioStatus` values after an op completes:
- 0 = idle
- 2 = ok
- 3 = error (check `FioErrCode`)

`FioErrCode` values (matches Avalonia):
- 0 = none
- 1 = file not found
- 2 = I/O error

New error codes needed:
- 3 = drive not mounted (MOUNT missing)
- 4 = drive full (SAVE exceeds image capacity)
- 5 = image corrupt (bad NDI magic / version / checksum)
- 6 = SD absent or FAT32 unmounted

Error reporting is by-value only — no error strings. Matches the existing
thin-register contract.

## CLI extensions (`e6502.NDI`)

Current verbs (no changes needed): create, dir, info, validate, label,
import, export, delete, mkdir, rmdir, tokenize, detokenize.

Add:
- `--size` accepts values up to at least 65536 (64 MB). Default floppy
  size stays at 800 KB. `--hd` is shorthand for 64MB:
  `ndi create hd0.ndi --hd --label HOME`.
- `autoboot` helper: `ndi set-autoboot image.ndi program.bas` — tokenizes
  `program.bas` and imports it as file `autoboot` in the image. Saves a
  common two-step dance.
- Nothing else; the existing verbs cover the rest.

## Concurrency model

ESP serializes all FIO operations. Each `EVENT_FIO` is processed to
completion (including all SD reads, RAM pokes, and status writeback)
before the next is accepted. Nova's BASIC busy-waits on `FioStatus`, so
it can't overlap commands anyway — this is the natural model.

When WTS comes later and starts requesting soundfont bytes in real-time,
we'll introduce a second priority lane. Out of scope for this phase.

## Boot ordering

Power-on sequence must be:
1. FPGA bitstream loaded (from SPI flash — automatic)
2. ESP boots (runs firmware out of its own flash)
3. ESP mounts SD (retry on failure; display LED pattern for absent card)
4. ESP streams ROM to FPGA BRAM (`CMD_POKE_ROM_BLK` loop)
5. ESP auto-mounts all `hd*.ndi` and caches each BAM + directory
6. ESP releases CPU reset
7. EhBASIC boots, prints Ready
8. ESP injects autoboot if found

If SD fails to mount, ESP still runs (step 4+6 still work — ROM is
currently in ESP flash) but all FIO ops return FioErrCode=6. BASIC
programs see "I/O error" — same behavior as running Avalonia without
`~/e6502-programs`.

Future: move ROM from ESP flash to SD, so SD becomes load-bearing. One
commit, lots of win (easier to update ROM, ROM per-SD-card). Out of scope
for Phase 1.

## Phased implementation

**Phase 1: Infrastructure** (this design).
- `rtl/fio.sv` + debug-bridge packet extension + `CMD_*_RAM_BLK` +
  `CMD_*_FIO_BANK` + `EVENT_FIO`.
- Verilator testbench for `fio.sv` (register latching, event pulse).
- Verilator testbench for the debug-bridge packet extensions.

**Phase 2: ESP firmware.**
- Port `NdiHeader.cs`, `NdiBam.cs`, `NdiDirectory.cs`, `NdiFloppyDevice.cs`
  to C++ (header-only is fine).
- FAT32 SD driver (SPI mode — use Arduino `SD` library or LittleFS; SPI
  mode is easy on ULX3S if we wire it).
- FIO command dispatcher: on `EVENT_FIO`, read bank, dispatch by `FioCmd`.
- Mount manager: HDs auto-scan, FDs user-controlled.
- Autoboot injector.

**Phase 3: ULX3S SD wiring.** (RESOLVED)

Verified against `emard/ulx3s/doc/constraints/ulx3s_v20.lpf`: SD card is
**physically shared** between FPGA and ESP32. Each SD line maps to both
an FPGA pin and an ESP32 GPIO:

| SD line | FPGA pin | ESP32 GPIO |
|---|---|---|
| sd_clk  | H2 | 14 |
| sd_cmd (MOSI) | J1 | 15 |
| sd_d[0] (MISO) | J3 | 2  |
| sd_d[1] (IRQ) | H1 | 4  |
| sd_d[2] | K1 | 12 |
| sd_d[3] (CS) | K2 | 13 |

ESP has direct access. No FPGA SD controller needed. Task list:

- ESP: use Arduino `SD.h` (SPI mode, CS=GPIO13) or `SD_MMC.h` (4-bit
  mode, higher bandwidth). Start with SPI for simplicity; switch to MMC
  if soundfont streaming needs the extra bandwidth later.
- FPGA: do **NOT** declare `sd_*` in `ulx3s.lpf`. Keep those pins
  tri-stated (default for undeclared pins under Yosys+nextpnr-ecp5). Any
  FPGA drive on those pins would contend with ESP and corrupt SD access.
- ESP32 GPIO12 is a strapping pin (flash voltage select). SD's d2 line
  can hold it high/low at boot and affect flash voltage. Standard
  mitigation: set eFuse `FLASH_VOLTAGE_SEL` via `espefuse.py`, or use a
  4.7kΩ pulldown on GPIO12. See memory `reference_esp32_dev.md`.

**Phase 4: BASIC integration + test.**
- sim6502 `fileio.6502` suite must pass on `--backend novavm`.
- End-to-end tests: MOUNT + LOAD + RUN of a real .ndi on hardware.
- Autoboot test: HD with `autoboot` file boots into it.

**Phase 5: CLI extensions.**
- `--size` up to 64MB, `--hd` shorthand, `ndi set-autoboot`.

**Phase 6 (later): soundfont / SFLOAD / WTS.**
Separate design doc.

## Testing strategy

- **Verilator**: `test_fio.sv` — regfile latching, event pulse, bank
  peek/poke. Expect ~20 assertions.
- **ESP firmware unit tests**: mock SD layer + mock FPGA bridge; test
  NDI parser against known-good `.ndi` fixtures; test command dispatcher.
- **sim6502 `fileio` suite**: backend=novavm, already exists for
  Avalonia. Aim for parity.
- **Hardware integration**: real SD card with pre-loaded `hd0.ndi`
  containing autoboot + sample programs; boot board, observe splash +
  program run.

## SD driver (resolved)

**Arduino `SD_MMC.h`** — ESP32 Arduino core, 4-bit SDIO mode, FAT32 via
`esp_vfs_fat` (ESP-IDF reference stack). No custom code, no rolled
filesystem. ULX3S has all four data lines wired (d0–d3), so we use the
full-bandwidth path from day one. `SD.h` (SPI, 1-bit) is a fallback if
SD_MMC proves problematic on this board, but there's no known reason to
expect that.

Standard API — `SD_MMC.begin()`, `SD_MMC.open("/path")`, `file.read()`,
`file.write()`, `file.close()`, directory iteration via `openNextFile()`.
Same `fs::FS` interface used everywhere in the ESP32 Arduino ecosystem.

## Open decisions (parking lot)
- **autoboot skip key** — SPACE or ESC. Preference: SPACE. Keyboard is
  easy to hold unambiguously.
- **FioErrCode range** — 2 codes today → grow to 6. Check if any existing
  BASIC code hard-codes these values; may need audit before adding new
  codes.

## Summary table

| Component | Location | Status |
|---|---|---|
| FIO register bank | FPGA `rtl/fio.sv` | NEW |
| `.ndi` parser | ESP firmware (C++ port) | NEW |
| FAT32 SD driver | ESP firmware (Arduino SD) | NEW |
| FIO command dispatcher | ESP firmware | NEW |
| Mount manager | ESP firmware | NEW |
| Autoboot injector | ESP firmware | NEW |
| Debug bridge | FPGA+ESP | EXTEND (add 5 packet types) |
| BASIC tokens | `ehbasic/basic.asm` | UNCHANGED |
| CLI | `e6502.NDI` | EXTEND (--size, set-autoboot) |
