# Arty Z7-20 — Zynq PS integration plan

Goal: use the Zynq **PS** (dual Cortex-A9 + DDR3 controller + MIO microSD) to
unblock the full NovaVM on the Arty:

1. **XRAM via PS DDR3** — the PL NovaVM core needs ~512 KB XRAM (expansion
   memory). NovaBASIC stages its libraries at XRAM `$07C000` and `lib_call`
   pages them in; with no XRAM the page-in stalls the CPU (`rdy=0`) right after
   the banner. Backing XRAM with PS DDR3 lets page-ins complete → with the
   existing FIO NAK this reaches `READY` with working libraries.
2. **File I/O / SD via PS** — replace the ESP32 NovaHost (ULX3S) and the
   temporary "fake FIO host" NAK with a PS task that mounts the microSD and
   services `fio_event`. This is "the SD card works like the ULX3S" — better,
   because it's the PS's own SD on a 1.3 GHz-class core.

## PS software: FreeRTOS

Chosen over bare-metal and Linux:
- XRAM-via-DDR needs **no** PS software at runtime (pure AXI-HP hardware once
  DDR is init'd by the FSBL). The RTOS earns its keep on file/SD and **future
  PS-side audio** (DDR-backed SF2/WTS + a high-priority audio task that
  preempts file I/O — the reason to pick an RTOS now).
- Xilinx ships a FreeRTOS BSP for Zynq in Vitis, so setup ≈ bare-metal.
- Tasks: `fio_svc` (service FIO over AXI-GP + FatFs on SD), later `audio`
  (high prio), `idle`.

## Hardware architecture (Vivado block design)

```
  PS7 (processing_system7, Arty Z7-20 preset)
   ├─ DDR3            (PS hard controller, board preset pins)
   ├─ UART0 -> MIO    (the FTDI USB-UART, /dev/ttyUSB1) — PS console
   ├─ SD0  -> MIO     (microSD)
   ├─ M_AXI_GP0       PS master -> PL: control/FIO bridge regs
   ├─ S_AXI_HP0       PL master -> PS DDR3: XRAM data path
   └─ FCLK_CLK0       PS->PL clock (optional; PL still has its own 125 MHz)
  PL
   ├─ nova_core  (rtl/top.sv, unchanged interface)
   ├─ axi_xram   (NEW: presents sdram.v's portA/portB/stream contract to
   │              top, backs it with AXI-HP reads/writes into a PS DDR region)
   ├─ fio_bridge (NEW: AXI-GP slave exposing fio_event + the FIO reg bank to
   │              the PS so the fio_svc task can peek/poke like NovaHost)
   └─ hdmi/MMCM/OBUFDS (as today)
```

### axi_xram bridge — the long pole
Implements the `sdram.v` interface `top` expects (rtl/sdram/sdram.v):
- Port A: `addrA[24:0]/weA/dinA[7:0]/oeA -> doutA[7:0]/doneA` (1-clk done pulse).
- Port B: same.
- Stream: `stream_req/stream_addr/stream_words/stream_ready -> stream_dout[15:0]/
  stream_valid/stream_busy/stream_done` (burst read).
Backed by AXI-HP (64-bit AXI3 into PS DDR): map XRAM byte addr -> `XRAM_DDR_BASE +
addr`. Read-modify-write or WSTRB for byte writes; burst reads for the stream
port. Arbitrate A/B/stream (stream is mutually exclusive per sdram.v).

### FIO bridge + protocol (port the ESP32 NovaHost)
NovaHost reference: `e6502.ESP32/novahost/{fio_event_reader,fio_dispatcher,
fpga_bridge}.cpp`. Protocol (rtl/fio.sv): CPU writes non-zero FioCmd -> single
`fio_event` pulse; host reads the FIO reg bank ($B9A0-$B9EF), executes, writes
results + flips FioStatus to 2 (ok) / 3 (err). On the Arty the PS does this over
AXI-GP via the fio_bridge instead of the ESP SPI/serial link.

## Boot flow
Zynq boots the PS first (not pure-PL JTAG): FSBL inits PS+DDR, loads the PL
bitstream, then the FreeRTOS app. Package `BOOT.bin` (FSBL + bit + app) on the
microSD FAT partition, or JTAG-download via Vitis/XSDB for dev. PS console on
UART0 -> /dev/ttyUSB1 (verifiable on the build host).

## Milestones
- [ ] **PS smoke test**: minimal PS7 (DDR + UART0), "hello" app over JTAG ->
      confirm on /dev/ttyUSB1. Proves PS config + DDR + boot + Vitis flow.
- [ ] **XRAM via DDR**: axi_xram bridge + PS7 block design wrapping nova_core;
      DDR init by FSBL; keep FIO NAK -> reach READY with libraries.
- [ ] **FIO/SD backend**: fio_bridge (AXI-GP) + FreeRTOS fio_svc task + FatFs
      on microSD -> real LOAD/SAVE/DIR; drop the NAK.
- [ ] **(later) PS audio**: DDR-backed SF2/WTS + high-priority FreeRTOS task.
