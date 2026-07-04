# NovaVM on Arty Z7 — Linux host (migration from bare-metal `ps_fio`)

Replaces the bare-metal PS host (`../ps_fio/`) with **Linux on the Zynq ARM Cortex-A9 PS**.
The PL (6502 SoC) RTL is unchanged. Goal: deterministic boot → NovaBASIC `Ready` (or
autoboot), audio via a prioritized kernel driver. See memory note
"Arty Z7 NovaVM — migrate host from bare-metal to Linux" for the why.

## Hardware facts (from build/arty_z7_full.xsa — authoritative)
- **`fio_bridge` register block: `0x40000000`–`0x4000FFFF` (64 KB)** on M_AXI_GP0
  (`MASTERBUSINTERFACE="M_AXI_GP0"`, `SLAVEBUSINTERFACE="M_AXI_FIO"`). Userspace `novavm`
  mmaps this for peek/poke/ctrl/vmem/rom/audio — all the `R_*` regs (FIO_BASE+offset) from
  `ps_fio/src/main.c`. **Access via a UIO node** (not `/dev/mem`). `fio_event` is the host IRQ.
- **PS↔PL shared XRAM in PS DDR: base `0x10000000`** (`XRAM_DDR_BASE`), reached by the PL via
  **S_AXI_HP0** (`HP0_DDR_LOWOCM`, PL sees PS DDR `0x0`–`0x3FFFFFFF`). Size: 512 KB XRAM +
  SID curve at +`0x80000` + module shelf — reserve a generous aligned block (e.g. 16 MB).
  **Must be a `reserved-memory` (no-map) node** so Linux never allocates it; `novavm` maps it
  coherently. (Bare-metal also did `Xil_DCacheFlushRange` on this — Linux mapping must be
  non-cached / explicitly synced.)
- **PS config (Arty-specific, already proven on U-Boot):**
  console **UART0** (Arty USB-UART; Zybo is UART1), **DRAM 512 MB**, **ps-clk-frequency =
  50 MHz** (Arty crystal — NOT 33.33 MHz or every clock incl. UART baud is 1.5× off).
  Low-level bring-up done by `ps7_init` (XSA ps7_init.tcl) — SPL/low-level-init OFF in U-Boot.

## Boot chain (FIT, deterministic)
BootROM → FSBL (or U-Boot SPL) → **U-Boot** → loads a **FIT image** (kernel + DT + bitstream)
→ U-Boot `fpga load` the PL bitstream + boot kernel → **Linux** → init → `novavm` service.
The 6502 stays held (CTRL bit0) until `novavm` stages the ROM + releases it.

## Layout (BR2_EXTERNAL — keep custom board out of the Buildroot tree)
```
linux/
  README.md                 (this)
  br2-external/
    external.desc           name: NOVAVM_ARTY
    external.mk
    Config.in
    configs/novavm_arty_z7_defconfig   (zynq, U-Boot+kernel+rootfs, novavm pkg)
    board/novavm_arty_z7/
      dts/novavm-arty-z7.dts           (base zynq + Arty fixes + PL nodes; see dts/ here)
      linux.config / linux.fragment    (UIO, the PL drivers, no bloat)
      uboot fragment                   (UART0, 512MB, 50MHz, SPL off)
      rootfs-overlay/                  (the novavm systemd/init service)
      genimage.cfg + post-image.sh     (SD image: boot FAT + ext4 rootfs)
  dts/novavm-arty-z7-pl.dtsi           (the PL additions — fio_bridge UIO + reserved-mem)
  novavm/                              (the userspace host port of ps_fio — Phase 2)
  audio-drv/                           (the PL-audio-FIFO kernel module — Phase 3)
```

## Phases
1. **Bootstrap**: Buildroot zynq image (U-Boot reusing the Arty fixes + linux-xlnx + our DT +
   minimal rootfs) → serial login, PL bitstream loaded, UIO maps `0x40000000`, reserved-mem ok.
2. **Port `ps_fio` → `novavm` userspace**: mmap fio_bridge (UIO) + the XRAM reserved-mem; FIO
   service loop (thread); file I/O → Linux FS (drop FatFs); mgmt(6504)/debug(6503)/upload(6502)
   → sockets (drop lwIP); boot seq hold→ROM→splash→release→service; autoboot AUTOBOOT.bin/bas.
3. **Audio kernel driver**: platform driver for the PL audio FIFO, fed by the userspace
   WTS/MIDI mixer via a char dev/mmap, with an hrtimer + high RT priority (no playback distortion).
4. **Boot-time + splash**: logo early (PL-config default image, or early userspace); trim init
   for determinism; ≤ ~10 s acceptable.
5. **`nova` CLI** unchanged (TCP); deploy = `scp novavm + restart service`.

## Build (TBD as Phase 1 lands — Buildroot at /tmp/buildroot)
`make BR2_EXTERNAL=$(pwd)/br2-external novavm_arty_z7_defconfig && make` → SD image.
