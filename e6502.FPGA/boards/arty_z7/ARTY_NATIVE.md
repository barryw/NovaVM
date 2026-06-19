# Arty Z7-20 — native optimization opportunities

Principle: keep the **6502-visible ABI identical** to the ULX3S (memory map, FIO
protocol, lib_call ABI, register layouts) so the same ROM/runtime/programs run
on both. Optimize the **implementation behind** those interfaces for the Arty's
better hardware (512MB DDR3, dual A9 PS, more LUT/BRAM/DSP, Ethernet, USB host,
HDMI-in). ULX3S corner-cuts → Arty wins:

## Memory / runtime
- **Module shelf paging is a corner-cut.** ULX3S pages library modules through a
  4×16KB LRU "shelf" (libcall.s PGD page-in, page_in_ctrl/page_dma + a CDC) because
  fast RAM is scarce — every lib_call can stall the CPU on a page-in. Arty has
  512MB DDR3: make **all modules resident in DDR** (one fixed slot per module,
  no LRU, no eviction) → lib_calls never page-in/stall. Same lib_call ABI, faster.
- **XRAM is tiny (512KB) by necessity.** Arty can expose much more XRAM (many MB)
  for huge programs / RAM disks / asset banks — behind the same XMC window ABI.
- **Boot staging is slow.** ULX3S streams runtime+modules from the ESP32 over a
  slow SPI/serial bridge. Arty: PS stages straight into shared DDR at high speed
  (or PL DMA from QSPI/BRAM) → much faster boot, fully-preloaded runtime.

## Host / I/O (PS replaces ESP32 NovaHost)
- microSD on PS (FatFs) + **native Gigabit Ethernet** + **USB host** — faster &
  richer than the ESP32. Web IDE / admin served from the board; zero-copy file
  I/O into shared DDR (PS writes XRAM/CPU-RAM directly, no slow bridge).
- **USB keyboards/mice/gamepads via the PS hard USB host** instead of the ULX3S
  PL soft-USB (usb_hid_host) — more robust, frees PL LUTs.

## Audio
- DSP slices + DDR: more SID voices, **large SF2 soundfonts resident in DDR**,
  PS-side mixing / MOD-XM-MIDI, real reverb/chorus. (Also sidesteps the reDIP-SID
  Vivado port: a PS or rebuilt-PL SID path can be cleaner.)

## Video
- DDR framebuffers: hi-res, **24-bit color**, page-flip, hardware scroll over a
  huge virtual canvas, more/bigger sprites with DDR-backed sheets, up to 1080p.
- **HDMI-IN** (the board has it): live external video as a layer/texture, capture,
  genlock — net-new capability the ULX3S can't do.

## Compute
- PS as coprocessor (float/string/heavy math, accelerate BASIC); PL has room for
  multiple 6502s. Faster 6502 clock possible (separate clock domain).

## Status
First applies: the XRAM-via-DDR bridge (axi_xram) is in. Next high-value native
win once booting: make library modules **resident in DDR** (drop the shelf
paging) — simpler, faster, and removes the page-in stall path entirely.
