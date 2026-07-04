# Arty Z7-20 (XC7Z020) — bring-up

This board directory targets the Digilent **Arty Z7-20** (Zynq-7000
`xc7z020-1clg400c`) using the AMD/Vivado toolchain. It currently holds a
**pure-PL smoke test** that proves the toolchain, constraints, and JTAG
programming path before the NovaVM SoC core is ported over.

The shared, device-independent RTL lives in `../../rtl/`. Board-specific files
(top level, constraints, build/program scripts) live here.

For live-board deployment and debug, start with
[`HARDWARE_RUNBOOK.md`](HARDWARE_RUNBOOK.md). It records the current target IP,
the editor-demo deploy command, SSH host-key handling, and the stale-payload
guard that must run before hardware testing.

## Smoke test

`arty_z7_smoke.sv` — no Zynq PS block design required:

| Signal | Behavior |
|--------|----------|
| `led4_b` | ~1 Hz heartbeat — unmistakable "alive" blink |
| `led[3:0]` | free-running binary counter (`sw[0]` freezes it) |
| `led4_r` / `led4_g` | mirror `sw[0]` / `sw[1]` (input proof) |
| `btn[0]` | reset |
| Pmod JA | 115200-8N1 UART **echo** (`led5_r` flashes on RX, `led5_g` = TX busy) |
| `sw[1]` high | show the low nibble of the last received byte on `led[3:0]` |

## Build & program

```bash
make            # build build/arty_z7_smoke.bit (Vivado non-project flow)
make prog       # program the PL over JTAG (volatile / SRAM config)
```

`make prog` auto-starts a `hw_server` if one isn't already running. Vivado and
hw_server are expected under `/tools/Xilinx/.../2024.2` — override with
`make VIVADO=/path/to/vivado`.

## UART wiring (important)

The Arty Z7's **onboard USB-UART is connected to the Zynq PS (MIO), not the
PL** — so a PL-only design cannot use `/dev/ttyUSB1`. To exercise the echo,
connect a 3.3 V USB-UART adapter to **Pmod JA**:

| Adapter | Pmod JA pin | FPGA pin |
|---------|-------------|----------|
| TX  | JA1 | Y18 (`uart_rx_in`) |
| RX  | JA2 | Y19 (`uart_tx_out`) |
| GND | JA5 | — |
| 3V3 | JA6 (only if the adapter needs board power) | — |

Then: `screen /dev/ttyUSB<adapter> 115200` (or `picocom`) — typed characters
echo back, and `led5_r` flashes on each received byte.

Routing UART through the PS (so the onboard USB-UART works) is a later step
once the SoC port begins.

## Board facts for the port (authoritative)

Sources: Digilent Arty Z7 Reference Manual + `Arty-Z7-20-Master.xdc`.

- **HDMI: two ports, both wired directly to the Zynq PL** (TMDS_33):
  - **Source / OUT (J11):** clk `L16/L17`, d0 `K17/K18`, d1 `K19/J19`,
    d2 `J18/H18`; `hdmi_tx_hpdn` R19, CEC G15, DDC I2C `M17/M18`.
  - **Sink / IN (J10):** clk `N18/P19`, d0 `V20/W20`, d1 `T20/U20`,
    d2 `N20/P20`; HPD T19, CEC H17, DDC I2C `U14/U15`.
  - Driving the HDMI **source** needs a **wall adapter** (USB 5 V can sag to
    4.75 V, below the 4.8 V HDMI-spec minimum).
  - Xilinx TMDS path = `OSERDESE2` (10:1) + `OBUFDS`/`TMDS_33` + `MMCM`
    (pixel clock + 5× serial clock), replacing the ECP5 `ODDRX`/PLL serializer
    the current `hdl-util-hdmi` uses.
- **UART:** onboard USB-UART is **PS-only (MIO)**; PL designs use a Pmod.
- **RAM:** DDR3 is on the **PS**, not PL — no PL SDRAM like the ULX3S has.
- 125 MHz PL clock on **H16**.

## Builds in this directory

| Target | Top | What it does |
|--------|-----|--------------|
| `make` / `make prog` | `arty_z7_smoke` | LED/UART smoke test |
| `make hdmi` / `make prog-hdmi` | `arty_z7_hdmi_test` | 640x480 HDMI test pattern (color bars) |
| `make vgc` / `make prog-vgc` | `arty_z7_vgc` | **VGC text layer over HDMI at 720x480p** |
| `make vgcgfx` / `make prog-vgcgfx` | `arty_z7_vgc_gfx` | **VGC graphics layer (mode 3, animated framebuffer)** |

The `vgc` build instantiates the real `rtl/vgc.sv` standalone (no 6502), writes
a text banner into char RAM via the VGC debug-vmem port, feeds VGC RGB into
`hdl-util/hdmi` (VIDEO_ID_CODE 2), and serializes via OSERDESE2 -> OBUFDS to J11.

## Vivado gotchas found during bring-up (vs the ULX3S yosys flow)

1. **Serializer**: `serializer.sv` auto-selects the OSERDESE2 path when
   `SYNTHESIS` is defined and `LATTICE_ECP5` is **not** — so `build_*.tcl` pass
   `-verilog_define SYNTHESIS=1`. Wrap its single-ended `tmds`/`tmds_clock` in
   `OBUFDS` for the differential pins.
2. **Clocking**: MMCME2_BASE 125 MHz -> CLKFBOUT_MULT_F=8.625 -> 134.77 MHz (x5)
   + 26.95 MHz (pixel), exact 5:1. ~720x480p59.83 — fine for TVs.
3. **Packed-string part-selects return blanks under Vivado synth.** Indexing a
   string literal like `MSG[(LEN-1-i)*8 +: 8]` with a variable base produced
   null chars (invisible text). Use explicit byte arrays / `case` instead.
4. **VGC forward-referenced nets** (`artist_*`, `gfx_*`, `sprite_*`, `gfx_color`,
   `fio_name_len`) are used before declaration in `vgc.sv`; Vivado makes them
   implicit 1-bit nets -> graphics/sprite modes broken (text path unaffected).
   Must reorder declarations before graphics/sprite modes work on Arty.
5. The VGC clears its char RAM during post-reset; inject screen content
   *continuously* (per-frame repaint) or after the clear completes.

## Status / next steps

- [x] LED + button + switch smoke test (toolchain + JTAG proof)
- [x] PL UART echo on Pmod JA (shared `rtl/uart_rx.sv` + `rtl/uart_tx.sv`)
- [x] HDMI OUT test pattern (MMCM + OSERDESE2 + OBUFDS + TMDS_33), 640x480
- [x] **VGC text layer over HDMI at 720x480p (J11)** — confirmed on hardware
- [x] **VGC graphics layer over HDMI** (animated framebuffer) — confirmed on hardware
- [x] Fixed `vgc.sv` forward-ref declarations (gotcha #4) — validated on Vivado (0
      implicit nets) AND yosys; graphics path confirmed live
- [ ] Drive the VGC from the 6502 CPU (boot EhBASIC) instead of the debug loader
- [ ] Decide RAM strategy (PL BRAM-only first; DDR3 via PS later)
- [ ] Decide UART strategy for the real SoC (Pmod vs PS EMIO bridge)
