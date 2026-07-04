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
  with full Vitis install"). Use `nova arty build-ps-fio`, which invokes the
  Vitis Python CLI hook with a fresh workspace. The CLI rejects a workspace
  touched by classic xsct.
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

## PS FIO host: built, blocked on a Vivado tmds_channel synth heisenbug (2026-06-19)
The Zynq PS is now the NovaVM host (replacing manual xsct staging + the FIO NAK):
- PL: fio_bridge.sv (AXI4-Lite slave on M_AXI_GP0 @ 0x40000000) — PS pokes/peeks
  CPU RAM + the FIO bank, injects keys, holds/releases the 6502 reset (held at
  config), observes fio_event. XRAM is staged by the PS writing DDR directly.
- BD: M_AXI_GP0 -> axi_protocol_converter (AXI3->AXI4-Lite) -> external M_AXI_FIO,
  all on hp_aclk (=clk_pixel, no CDC); fio_aresetn exposed.
- arty_z7_full drives top's dbg surface from fio_bridge (staging FSM + NAK gone).
- PS app (`ps_fio/src/main.c`, built through `nova arty build-ps-fio`): standalone + xilffs FatFs;
  holds CPU reset, stages libcall.bin to $0320, mounts microSD, releases CPU,
  services FIO_CMD_LOAD_MODULE ($2C: id in FIO_SRC_LO, slot in FIO_END_LO) by
  reading /lib/<name>.nmod into the XRAM shelf slot (DDR 0x10000000+0x060000+
  slot*0x4000) + DCacheFlush, then FIO_STATUS=OK.

BLOCKER (open): with fio_bridge in the design, Vivado synth FAILS hdl-util-hdmi
tmds_channel (Synth 8-6156, NO error cause) -> cascades to hdmi -> arty_z7_full.
The IDENTICAL hdmi/tmds synthesizes fine WITHOUT fio_bridge (that's the working
READY build). Ruled out: genvar/CN constant-fold (unrolled the 3 tmds_channel
instances to literal CN), default_nettype (removed from fio_bridge), OOM (44G
free), project-state (multiple fresh rm -rf + BD + full), defines (runme confirms
-verilog_define SYNTHESIS/VIDEO_720X480 applied), multithreaded synth (maxThreads
1 still fails), unique-case (both are full; changed to plain case), debug ILA
(removed). All still fail tmds_channel identically.
Recommended next: synthesize hdmi out-of-context (synth_design -mode
out_of_context -top hdmi -> hdmi.dcp, reference as a black box) to isolate it from
the design; or try a different Vivado version. The PS app is ready to test the
moment a bitstream+XSA with fio_bridge exists. READY milestone (no fio_bridge)
is unaffected and remains the demoable state.

## MILESTONE: NovaBASIC reaches READY on the Arty (2026-06-19)
Full NovaVM boots to the BASIC **banner + READY** on the Arty Z7-20 over HDMI,
with XRAM backed by PS DDR3. The end-to-end chain that works:
- ROM + fonts load (after fixing the rom-wipe hook); cold-start draws the banner.
- autoboot lib_call(FILES): shelf HIT (SHELF_TAG[0]=FILES seeded in the loader
  image) -> PGD page-in from XRAM slot 0 (DDR 0x10060000, files.bin pre-staged by
  boot_full.tcl) -> this EXERCISES + VALIDATES the axi_xram STREAM path -> FILES
  MOD_ENTRY -> FILE_LOAD(AUTOBOOT) -> FIO NAK not-found -> READY.
Reproduce: build_full_bd.tcl then build_full.tcl (fresh project), then
boot_full.tcl (ps7_init + DDR + stage files.bin to 0x10060000 + program PL).
Remaining: NO CURSOR — the READY line-reader does lib_call(SYSTEM) (basic.asm
:1215) and SYSTEM isn't staged; the SYSTEM module turns the cursor ON at entry.
Fix: stage SYSTEM (id 3) too (slot 1 / shelf_tag[1]=3, system.bin -> 0x10064000).
Typing also needs a keyboard (key_valid tied 0 — USB HID later). Long-term: PS
host services FIO_CMD_LOAD_MODULE so modules stream from SD on demand.

## Root cause nailed: resident loader + staged module both required (2026-06-19)
ILA (d_pc/d_addr) + the NovaHost boot spec settled the READY blocker:
- The resident lib_call loader (libcall.bin, 248B) MUST live in CPU RAM $0320.
  Without it, lib_call's `JSR $0320` = $00 = BRK -> garbage. NovaHost pokes it at
  boot while holding dbg_cpu_reset. We now do this in a PL "staging FSM" in
  arty_z7_full (holds dbg_cpu_reset, pokes rom/libcall_loader.hex into $0320,
  releases). dbg_poke outranks the held CPU on RAM port A (top.sv:1131) — the
  NovaHost model. (A 64KB main_ram $readmemh bake also worked mechanically but
  is the wrong approach: large init'd byte-write BRAM misbehaved under Vivado.)
- Loader ALONE is NOT enough: lib_call(FILES) with no FILES module staged in
  XRAM -> PGD page-in maps a ZEROED bank-1 overlay at $C000, lc_validate sees
  bad magic, and control returns into the zero overlay -> BRK loop (ILA: 5 PCs,
  $0002 BRK -> vector $FFFE/$FFFF=$0000 -> $0000). ROMSWAP stuck on the empty
  bank. NovaHost never hits this because it always stages the module first.
- THEREFORE minimum READY = loader@$0320 (done) + FILES module (.nmod) staged in
  XRAM at its shelf slot ($06xxxx) + shelf dir @ $0418, then release CPU. The
  module page-in exercises the axi_xram STREAM path (untested) — next to verify.
  Build FILES.nmod with cc65 (installed) + `nova module pack`; stage to DDR
  (PS/xsct write XRAM=DDR directly) BEFORE releasing the CPU.

## READY blocker = lib_call dispatch, NOT the Zynq/XRAM work (2026-06-19, latest)
- Tried autoboot-skip in hardware: pause the CPU (dbg_pause) right after reset,
  poke AUTOBOOT_SKIP ($B9F0)=1, release. RESULT: WORSE — no banner at all.
  Pausing the arlet 6502 at/just-after reset corrupts its startup. Reverted.
  (Continuous dbg_poke to set $B9F0 is also unusable: it hijacks RAM port A
  from CPU reads.)
- ILA (d_pc/d_addr): in the banner-showing build the CPU runs (rdy toggling)
  but PC sits at ~$021D executing the input-buffer RAM region with d_addr in the
  stack ($010C) — i.e. the autoboot lib_call(FILES) DISPATCHED INTO GARBAGE
  before reaching the FIO (so the FIO NAK never fires). The lib_call/bank-switch
  (ROMSWAP) path appears broken under the Vivado build; same symptom on BRAM-only.
- This is a firmware/RTL co-debug thread, INDEPENDENT of the Zynq + axi_xram +
  DDR work (all of which build, time-close, and run). Clean ways to READY:
  (a) rebuild the ROM with autoboot disabled / AUTOBOOT_SKIP defaulted (needs
  cc65, not on this host); (b) PC-trace the lib_call dispatch from the banner to
  find the bad jump; (c) bring up the PS file backend so autoboot succeeds.

## XRAM bring-up — RESOLVED diagnosis (2026-06-19, later)
ILA on the CPU (d_pc/d_rdy/d_we/d_addr) + a PS-DAP DDR read settled it:
- The "no banner" in the integrated build was NOT axi_xram — it was the EhBASIC
  ROM BRAM loading all-zeros under **project-mode synthesis** ($readmemh
  "rom/..." resolves from the run dir, not boards/arty_z7). FIX: a pre-synth
  hook (STEPS.SYNTH_DESIGN.TCL.PRE in build_full.tcl) symlinks rom/ into the
  run dir. After the fix the CPU runs real code (129 distinct PCs vs 5 BRK-loop
  PCs) and the banner renders.
- axi_xram / XRAM is NOT the boot blocker: at boot LIB_RESIDENT=HOSTEXT so the
  ext ROM is resident in BRAM and ensure_ext_resident is a no-op (XRAM staging
  $07C000 reads 0, as expected). DDR confirmed working (PS DAP). d_rdy toggles
  (CPU running, not stalled) -> no XMC access pending.
- Remaining READY blocker (SAME as BRAM-only build): after the banner the CPU
  loops at ~$0200 (input-buffer RAM) accessing the stack -> it jumped into
  RAM/garbage in the autoboot lib_call(FILES) path. This is a firmware/bank-
  switch (ROMSWAP/lib_call) issue, independent of the Zynq/XRAM work.
  Next: AUTOBOOT_SKIP ($B9F0=1, gated on ~d_we so CPU writes aren't dropped) to
  confirm autoboot is the culprit, or PC-trace the lib_call path from the banner.

## XRAM bring-up status (2026-06-19)
Full integration (arty_z7_full) BUILDS with timing met; PS DDR3 confirmed
working via the PS DAP (mwr/mrd 0x10000000 reads back). Booted on hardware
(ps7_init -> DDR up -> program PL). Result: **gray border renders (MMCM/VGC/
HDMI/reset all good) but NO banner** — i.e. the 6502 now stalls on its FIRST
XMC access through axi_xram (previously, with the BRAM stub doneA=1, it ran far
enough to render the banner and only hung later at the lib page-in).

Diagnosis: xram_sdram holds we/oe through SD_ACTIVE until it sees doneA, then
captures doutA the next cycle (axi_xram holds doutA, so reads are fine). The
only way the CPU hangs here is axi_xram never pulsing doneA -> its AXI
transaction to S_AXI_HP0 never completes. The DDR test used the PS DAP, NOT the
PL->SmartConnect->HP0 path, so that path is unverified.

Next debug (needs visibility): add an ILA on axi_xram's AXI handshake
(aw/w/b/ar/r valid+ready, state) to see where it stalls. Prime suspects:
SmartConnect aresetn (rstgen/ FCLK_RESET0_N) not released; HP0 not routing;
boot order (try program PL before ps7_init, or assert a PL reset after). Minor
(not the hang): axi_xram re-triggers a redundant write because we/oe stay
asserted one cycle after doneA — harmless, but worth gating on we/oe deassert.

## Milestones
- [x] **PS smoke test**: PS7 BD + XSA ✓; Vitis platform + hello.elf ✓; JTAG
      boot CONFIRMED — "Hello World" on /dev/ttyUSB1. **JP4 boot-mode jumper
      MUST be set to JTAG** (that was the earlier APB/empty-target failure).
      Boot: `xsct vitis/boot_ps.tcl` (connect; ps7_init; dow hello.elf; con).
- [ ] **XRAM via DDR**: axi_xram bridge + PS7 block design wrapping nova_core;
      DDR init by FSBL; keep FIO NAK -> reach READY with libraries.
- [ ] **FIO/SD backend**: fio_bridge (AXI-GP) + FreeRTOS fio_svc task + FatFs
      on microSD -> real LOAD/SAVE/DIR; drop the NAK.
- [ ] **(later) PS audio**: DDR-backed SF2/WTS + high-priority FreeRTOS task.

## ✅ MILESTONE: interactive NovaBASIC on Arty Z7-20 (2026-06-20)

Full interactive machine confirmed on hardware over HDMI:
- READY + **flashing cursor**, console keyboard input works (`PRINT 3*7` -> `21`).
- LED0 (CPU-alive, gated on cpu_rdy) blinking = CPU running, no stall.
- PS (Zynq A9) is the NovaHost: mounts microSD, stages the lib_call loader, services
  FIO, and **writes library modules directly into bank-1 (ext_rom) via the fio_bridge
  ROM-load port** (idx=1). Reliable GP0 single-writes.
- The lib_call XRAM/HP0 page-in is BYPASSED: axi_xram's stream is a no-op
  (S_SDONE immediately), because the HP0 burst-read DMA was unreliable (flakily
  stalled in S_STR_R / never reached S_SDONE). PS-direct-write is the Arty-native fix.
- Console keys: PS reads UART RX (RX_EN enabled explicitly) -> fio_bridge KEY ->
  VGC key queue -> CHARIN.

Remaining: multi-module ALTERNATION (a loader shelf-HIT would no-op the page-in and
leave the wrong module resident -> needs a 1-slot/always-MISS loader tweak); real
file I/O FIO commands (LOAD/SAVE/DIR currently NAK'd); USB-HID keyboard; PS Ethernet
+ network LED (LED2/LD5).
