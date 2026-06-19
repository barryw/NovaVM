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

## Tooling notes (this build host)
- Vivado/Vitis 2024.2. Arty Z7-20 board files installed (`digilentinc.com:
  arty-z7-20:part0:1.1`).
- **Vitis classic xsct platform/app flow is NOT available** ("--classic only
  with full Vitis install"). Use the **new `vitis -s <script.py>` Python CLI**
  (`vitis/build_ps_hello.py` — confirmed working: builds platform + FSBL BSP +
  hello.elf). Use a FRESH workspace dir; the CLI rejects a workspace touched by
  classic xsct.
- JTAG boot of the PS app is still pending: `xsct connect; targets` returned an
  EMPTY list even though Vivado's hw_manager sees `arm_dap_0` + `xc7z020`. So
  the cable/DAP are fine — it's an xsct/hw_server contention or boot-mode issue.
  To resolve next: ensure no Vivado hw_manager session holds the FT2232 cable,
  confirm `JP4` boot-mode = JTAG, then `connect` / `targets` / `ps7_init` /
  `dow` / `con` (see `vitis/boot_ps.tcl`). PS UART0 -> /dev/ttyUSB1 @115200.

## Future capabilities the Zynq unlocks (beyond ULX3S parity)
Confirmed Arty Z7-20 PS resources (board preset): 512 MB DDR3, ENET0
(Ethernet), USB0 (host), SD0 (microSD), QSPI, UART0; dual A9 @667MHz + NEON;
PL has 220 DSP + 4.9 Mb BRAM.
- **Memory**: XRAM 512 KB -> many MB; DDR framebuffers (hi-res/24-bit, page
  flip, huge virtual scroll); instant whole-machine save-states to DDR/SD.
- **PS as super-NovaHost**: FAT/ext4 on microSD; USB-host keyboards/mice/
  gamepads/storage; **Ethernet** -> telnet-to-BASIC, board-hosted web IDE/
  panel, network load/save, multiplayer.
- **Audio**: more SID voices; large SF2 soundfonts in DDR; PS software synth
  (FreeRTOS high-prio task) for MOD/XM/MIDI/mp3; DSP reverb/chorus.
- **Video**: 1080p HDMI out; HDMI **IN** as a live layer/texture/capture;
  DSP-accelerated blit (scale/rotate/alpha), more+bigger sprites/layers.
- **Compute**: PS as coprocessor (math/float/strings, accelerate BASIC);
  room in PL for multiple 6502s.

## Milestones
- [~] **PS smoke test**: PS7 BD + XSA build ✓; Vitis platform + hello.elf build
      ✓ (vitis/build_ps_hello.py). JTAG boot to confirm on /dev/ttyUSB1 PENDING
      (xsct target enumeration, see Tooling notes).
- [ ] **XRAM via DDR**: axi_xram bridge + PS7 block design wrapping nova_core;
      DDR init by FSBL; keep FIO NAK -> reach READY with libraries.
- [ ] **FIO/SD backend**: fio_bridge (AXI-GP) + FreeRTOS fio_svc task + FatFs
      on microSD -> real LOAD/SAVE/DIR; drop the NAK.
- [ ] **(later) PS audio**: DDR-backed SF2/WTS + high-priority FreeRTOS task.
