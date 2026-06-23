# Arty Z7 boot-splash handoff

## What we want (exact spec from the board owner)
On power-on (JP4 = SD, no host/JTAG), the Arty Z7-20 should:
1. **Fade the NovaVM logo IN over ~1 second** (dark → full bright)
2. **Hold it for 3 seconds** at full brightness
3. **Fade it OUT over ~1 second** (bright → dark)
4. **Boot to NovaBASIC**: banner + `Ready` + flashing cursor, full brightness
5. **Never reboot/loop.**

The logo asset is `/sd/assets/boot/novavm_logo.nvg` (NVG2: 320×200, 4-bit packed + 16-colour palette; 32064 bytes). It IS present and valid on the SD.

## Current broken behaviour (after many attempts)
- **No visible fade-in** — logo just appears.
- **Hold is ~1 second**, not 3.
- **Reboots/loops** after reaching `Ready` (or during the splash).

## ⚠️ THE #1 SUSPECT — verify this FIRST
**Radically different builds all produce the *identical* symptom** (no fade-in / 1s hold / reboot): busy-loop timing vs `usleep` timing, different durations, inverted dim polarity, watchdog disabled vs not. That is not how code changes behave. It strongly implies **the board is NOT running the `BOOT.bin` we push to the SD** — it's booting something stale/different.

Before changing ANY more splash code, prove which binary is actually executing:
1. Add a unique marker near the top of `main()` in `ps_fio/src/main.c`, e.g.
   `xil_printf("[boot] BUILD MARKER 0xCAFE0007\r\n");` (bump the number each build).
2. Rebuild + deploy (below), power-cycle, watch the serial (`/dev/ttyUSB1`, 115200).
3. If the marker does NOT appear / is the old number, the SD boot is loading a different image. Then investigate:
   - Is there a **second `BOOT.bin`** the BootROM prefers (different SD partition, FAT16 boot partition vs the FatFs partition `nova put` writes to)? `nova put ... BOOT.bin` writes to FatFs `0:/BOOT.bin`; confirm that's the partition the Zynq BootROM actually reads.
   - Is the board actually booting from **QSPI** or a cached image rather than SD? Check JP4/MIO boot-mode strap.
   - Does the FSBL load the **PL bitstream and app from a fixed offset** baked at `bootgen` time rather than re-reading `BOOT.bin` each boot?
   - Pull the SD on a PC and confirm `BOOT.bin`'s timestamp/size matches `build/BOOT.bin` after a push.

Everything below (timing, dim, watchdog) is almost certainly already correct in source; it just may never have executed on hardware.

## Hardware / boot architecture
- Zynq-7020: ARM Cortex-A9 PS (bare-metal C, `ps_fio/src/`) + Artix PL (6502 SoC, shares `e6502.FPGA/rtl/` with the ULX3S).
- Boot chain: BootROM → FSBL → PL bitstream → `ps_fio.elf`, all packed in `BOOT.bin` via `bootgen`.
- `ps_fio.elf` `main()`: holds the 6502 in reset (`cpu_hold(1)`), loads the BASIC ROM, mounts SD, stages the loader, runs `boot_splash()`, releases the 6502 (`cpu_hold(0)`), then brings up net/usb/audio and enters the FIO service loop.

## Relevant files
- `boards/arty_z7/ps_fio/src/main.c` — `boot_splash()` (the splash), `main()` (boot sequence), `swdt_disable()`/`swdt_kick()` helpers. **(uncommitted changes live here)**
- `boards/arty_z7/fio_bridge.sv` — PS↔FPGA AXI bridge. Added the VGC-memory **write** port (`dbg_vmem_we`/`dbg_vmem_wdata`) at AXI offset `0x2C` write: `{space[2:0]@25, addr[16:0]@8, data[7:0]}`. **(committed `4aef325`)**
- `boards/arty_z7/arty_z7_full.sv` — wires `fb_vmem_we`/`fb_vmem_wdata` into `top.sv`'s `dbg_vmem_we`/`dbg_vmem_data`. **(committed `4aef325`)**
- `rtl/vgc.sv` — `DISPLAY_DIM` applied at line ~3439 as `color × dim / 16` (smooth 16-level brightness on the final composited image). `dbg_vmem` write port (`dbg_vmem_gfx_we`, highest priority in the gfx port-A mux, ~line 3103).
- `boards/arty_z7/make_boot_bin.sh` — packs FSBL + bitstream + `ps_fio.elf` → `build/BOOT.bin`. **(committed `3411097`)**
- `test/test_vgc_dbg_gfx_write.sv` + `test/Makefile` — Verilator test proving the dbg_vmem gfx write. **(committed `4aef325`)**

## What was verified to actually WORK (over JTAG, where the elf definitely runs)
- **dbg_vmem gfx write**: PS self-check logged `[splash] selfcheck gfx[0]: wrote 0A, read 0A`; logo pixels confirmed resident in the gfx plane (`vm read-vram 3 …` non-zero at rows 40–120). Verilator `test_vgc_dbg_gfx_write` passes.
- **NovaBASIC reaches `Ready`**: char plane (`vm read-vram 1 …`) shows the banner + `Ready`.
- **DISPLAY_DIM polarity**: poking `$A0E5` (41189) = `0x0F` made the text VISIBLE; `0x00` = dark/black. So **0x00 = dark, 0x0F = full bright** (NOT "dimming"). Register read-backs of a known-good state: `$A000`(MODE)=0, `$A0E5`(DIM)=0x0F bright, `$A001`(BGCOL)=0, `$A00D`(BORDER)=0x0B gray, color plane=0x0F (white text index), palette[15]=0xB0B0B0 (light gray).
- **Reboot cause**: `REBOOT_STATUS` (`0xF8000258`) read `0x00400000` = **bit 22 = SWDT (system watchdog) reset**. `XPAR_XWDTPS_0_BASEADDR` is **undefined** in the BSP, so the FSBL's `XWdtPs` watchdog-disable code is compiled out — the BootROM-armed SWDT is never turned off. `swdt_disable()` (write `0xF8005000` = `0x00ABC000`, ZKEY 0xABC + WDEN=0) was verified over JTAG to change `SWDT_ZMR` 0x1C0 → 0x00000000.

## Splash logic currently in `boot_splash()` (uncommitted)
1. `poke(VGC_DIM=0x00)` (dark), set mode `$A000`=3 (gfx-only), bg/border/cursor, load `.nvg` palette via `$A0F4`/`$A0F5`.
2. Render: 320×200 pixels into gfx space (3) via `R_VMEM_WRITE` (AXI `0x2C`), `swdt_kick()` per row.
3. Fade in `dim 0→15`, hold, fade out `dim 15→0` using `usleep()` (`sleep.h`), `swdt_kick()` each step.
4. Restore: mode `$A000`=0 (text), `$A0E5`=0x0F (full bright), cursor on, `R_VMEM_ADDR` back to char/0.
5. `main()` early: `swdt_disable()`. `main()` service loop top: `swdt_kick()`.

Register map used (all on the 6502 bus via `poke()` / `dbg_poke_vgc`):
`$A000` MODE, `$A001` BGCOL, `$A00A` CURSOR_EN, `$A00D` BORDER, `$A0E5` DISPLAY_DIM,
`$A0F4`/`$A0F5` PALETTE_INDEX/DATA. Gfx pixels go via the dbg_vmem write port, NOT poke.

PS AXI registers (`FIO_BASE 0x40000000`): `R_POKE 0x00`, `R_VMEM_ADDR 0x28` (`{space[2:0]@17,addr[16:0]}`), `R_VMEM_DATA 0x2C` (read), `R_VMEM_WRITE 0x2C` (write `{space@25,addr@8,data}`). SWDT: ZMR `0xF8005000` (disable = write `0x00ABC000`), RESTART `0xF8005008` (kick = write `0x1999`). `REBOOT_STATUS 0xF8000258`.

## Build + deploy (this is what we've been doing)
```bash
cd e6502.FPGA/boards/arty_z7
# 1. Rebuild the PS app (regenerates platform incl FSBL):
rm -rf /tmp/nova_fio_ws
/tools/Xilinx/Vitis/2024.2/bin/vitis -s vitis/build_ps_fio.py     # ~3 min -> /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf
# 2. Pack BOOT.bin (FSBL + build/ps_full/.../arty_z7_full.bit + ps_fio.elf):
./make_boot_bin.sh                                                # -> build/BOOT.bin (~4.77 MB)
# 3. Push to the SD over the network (board running, on 192.168.1.213):
cd /home/barry/NovaVM
dotnet run --project e6502.Nova -c Release -- put e6502.FPGA/boards/arty_z7/build/BOOT.bin BOOT.bin --remote 192.168.1.213
# -> /sd/BOOT.bin. Then power-cycle (JP4 = SD).
```
The bitstream itself rarely needs rebuilding (`vivado -mode batch -source build_full.tcl`, ~15 min); only `arty_z7_full.sv`/`fio_bridge.sv`/`rtl/` changes need it.

## How to run/verify WITHOUT power-cycling (JTAG)
```bash
cd e6502.FPGA/boards/arty_z7
/tools/Xilinx/Vitis/2024.2/bin/xsct boot_fio_noreset.tcl   # connect; rst -processor; ps7_init; fpga; dow ps_fio.elf; con
```
**OPEN QUESTION (suspect):** whether a JTAG `dow` actually overrides the SD boot when JP4=SD. If it does NOT, all JTAG "verification" was reading the SD's image, not the freshly-flashed elf. **Use the BUILD MARKER to settle this too.** If JTAG `dow` is being ignored, set JP4=JTAG to verify, OR rely on the SD path + the marker.

## Diagnostics
- Serial: `stty -F /dev/ttyUSB1 115200 raw -echo; cat /dev/ttyUSB1`. `ps_fio` prints `[boot] REBOOT_STATUS=… SWDT_ZMR=…`, `[boot] SWDT_ZMR now=…`, `[splash] …`, `[fio] 6502 released`, net/DHCP. (FTDI sometimes re-enumerates; only `/dev/ttyUSB1` is the console UART.)
- Screen/VGC memory: `nova vm read-vram <space> <addr> <len>` (space 1=char, 2=color, 3=gfx, 7=textattr). `nova vm peek <addr>` reads a 6502-bus/VGC register.
- The `nova` CLI = `dotnet run --project e6502.Nova -c Release -- …` (or build it).

## Gotchas hit during this work
- **Repeated JTAG flashing wedges the PS Ethernet/PHY** → DHCP stops completing (`No route to host`). A clean power-cycle restores it. The SD boot's network is fine; it's the JTAG `rst` churn that degrades it.
- **`rst -srst`** clears the PL without rebooting (board goes silent); **`rst -por`** is "not supported for target". So a real power-on can't be triggered from JTAG — only the physical power button gives a true POR / SD boot.
- Busy-loop delays are unreliable (speed depends on what else is in the loop body) → use `usleep`/`sleep` (`sleep.h`).
- Leftover fd0 `.ndi` mounts will auto-boot a game and clobber the splash (`nova drive unmount fd0`).

## Recommended order of attack for whoever picks this up
1. **Prove what's booting** with the BUILD MARKER (above). Do not touch splash logic until the marker on the SD image is confirmed to appear in serial after a power-cycle. This is very likely the entire bug.
2. Once you can change behaviour by pushing: the splash source (timing via `usleep`, dim polarity 0=dark/0xF=bright, `swdt_disable()`+`swdt_kick()`) is believed correct — adjust durations to taste (`usleep(62500)`×16 ≈ 1s fade; `usleep(500000)`×6 ≈ 3s hold).
3. If it still reboots with the watchdog disabled+kicked, re-read `REBOOT_STATUS` on the actual SD boot to see if it's still SWDT (bit 22) or something else, and consider adding `XWDTPS` to the PS7 block design so the FSBL disables it natively.
