# Arty Z7 boot-splash handoff

## What we want (exact spec from the board owner)
On power-on (JP4 = SD, no host/JTAG), the Arty Z7-20 should:
1. **Fade the NovaVM logo IN over ~1 second** (dark → full bright)
2. **Hold it for 3 seconds** at full brightness
3. **Fade it OUT over ~1 second** (bright → dark)
4. **Boot to NovaBASIC**: banner + `Ready` + flashing cursor, full brightness
5. **Never reboot/loop.**

The logo asset is `/sd/assets/boot/novavm_logo.nvg` (NVG2: 320×200, 4-bit packed + 16-colour palette; 32064 bytes). It IS present and valid on the SD.

## Fix applied in source
- `ps_fio/src/main.c` now prints a unique boot marker at the top of `main()`:
  `[boot] BUILD MARKER 0xCAFE0013 <date> <time>`.
- `ps_fio/src/main.c` disables the BootROM-armed SWDT early, kicks it during the
  splash render/fades, and keeps kicking it in the service loop.
- `build_full_bd.tcl` enables the PS WDT peripheral so regenerated BSP/FSBL code
  includes the XWdtPs watchdog path instead of compiling it out.
- `boot_splash()` uses the verified dim polarity (`0x00` dark, `0x0F` bright)
  and `usleep()` timing for ~1 s fade-in, 3 s hold, and ~1 s fade-out.

## Current hardware status (2026-06-24)
The watchdog theory is ruled out for the observed loop. AMD UG585 says `REBOOT_STATUS` bit 16 is SWDT; the board repeatedly showed `SWDT=0`, `SWDT_ZMR=00000000`, and `SCUWDT_CTL=00000000`. The raw values `0x00400000` and `0x00500000` are POR/debug-reset history, not watchdog resets.

Verified on the board:
- Full Vivado bitstream/XSA rebuild completed successfully.
- Vitis rebuilt the platform, patched FSBL, and `ps_fio.elf` successfully.
- `vitis/build_ps_fio.py` patches generated `zynq_fsbl/fsbl_hooks.c` so FSBL forces `SWDT_ZMR=0x00ABC000` before bitstream load and before app handoff.
- Marker `0xCAFE000D` SD image was uploaded to `/sd/BOOT.bin`, downloaded back to `/tmp/arty_BOOT.bin`, and verified byte-for-byte with `cmp_exit=0`. SHA-256 was `ac080126cf0422cb5d21e8afb6f09bf7baf9ee32cb42fcfd47efc2536a7e4a74`.
- Marker `0xCAFE000D` serial repeatedly showed `reset decode POR=1 DEBUG=0 SWDT=0 raw=00400000` and `watchdog decode SWDT_EN=0 SWDT_RSTEN=0 SCUWDT_EN=0 SCUWDT_RST=0`.
- Marker `0xCAFE000D` progressed through splash, net init, management server, and debug server: `NovaHost debug server up on TCP 6503`, then restarted before USB/audio/service-loop output. The app exception handlers did not print an undefined/prefetch/data abort banner before the restart.
- Marker `0xCAFE0012` was loaded over JTAG in forced visual diagnostic mode with a persistent full-bright logo. The board owner confirmed the logo faded in and stayed, proving the embedded logo, VGC gfx write path, DISPLAY_DIM polarity, HDMI output, and PS-to-PL splash path are working.

Current local boot candidate:
- Source is at marker `0xCAFE0013`. Normal splash behavior is restored: fade in, hold, fade out, restore text mode/full brightness/cursor, release the 6502.
- `usb_init()` remains disabled because marker D got through the debug server and restarted before USB/audio/service-loop output.
- JTAG visual diagnostic mode now only activates when `REBOOT_STATUS` has the debug-reset bit set. Normal POR/SD boot follows the SD mount/drive/audio/network path.
- Vitis rebuilt marker 13 successfully and `make_boot_bin.sh` packaged it into `build/BOOT.bin` at 2026-06-24 02:37 UTC.
- `build/BOOT.bin` SHA-256: `7b214eceeab72242ce66ad42a3316b62a80e019ea7dc86245cee383aee0d4e46`.
- Marker 13 has **not** been deployed to SD from this host. `lsblk` shows no removable/FAT Arty boot card mounted; the only new `sdp` device is a 10G ext4 virtual disk, so do not copy the image there.

Next action:
- Copy `e6502.FPGA/boards/arty_z7/build/BOOT.bin` to the Arty boot SD card as `/BOOT.bin`, set JP4 = SD, and physically power-cycle. If marker 13 reaches `[boot] entering service loop`, the loop is isolated to `usb_init()`/USB host setup. If it still restarts, skip `audio_timer_init()` next, one checkpoint at a time.

## Hardware / boot architecture
- Zynq-7020: ARM Cortex-A9 PS (bare-metal C, `ps_fio/src/`) + Artix PL (6502 SoC, shares `e6502.FPGA/rtl/` with the ULX3S).
- Boot chain: BootROM → FSBL → PL bitstream → `ps_fio.elf`, all packed in `BOOT.bin` via `bootgen`.
- `ps_fio.elf` `main()`: holds the 6502 in reset (`cpu_hold(1)`), loads the BASIC ROM, mounts SD, stages the loader, runs `boot_splash()`, releases the 6502 (`cpu_hold(0)`), then brings up net/usb/audio and enters the FIO service loop.

## Relevant files
- `boards/arty_z7/ps_fio/src/main.c` — `boot_splash()` (the splash), `main()` (boot sequence), build marker, `swdt_disable()`/`swdt_kick()`/`scuwdt_disable()` helpers.
- `boards/arty_z7/build_full_bd.tcl` — enables the PS WDT peripheral so regenerated BSP/FSBL code can see `XPAR_XWDTPS_0_BASEADDR`.
- `boards/arty_z7/vitis/build_ps_fio.py` — patches generated FSBL hooks to force the PS SWDT fully off before bitstream load and before handoff.
- `boards/arty_z7/fio_bridge.sv` — PS↔FPGA AXI bridge. Added the VGC-memory **write** port (`dbg_vmem_we`/`dbg_vmem_wdata`) at AXI offset `0x2C` write: `{space[2:0]@25, addr[16:0]@8, data[7:0]}`. **(committed `4aef325`)**
- `boards/arty_z7/arty_z7_full.sv` — wires `fb_vmem_we`/`fb_vmem_wdata` into `top.sv`'s `dbg_vmem_we`/`dbg_vmem_data`. **(committed `4aef325`)**
- `rtl/vgc.sv` — `DISPLAY_DIM` applied at line ~3439 as `color × dim / 16` (smooth 16-level brightness on the final composited image). `dbg_vmem` write port (`dbg_vmem_gfx_we`, highest priority in the gfx port-A mux, ~line 3103).
- `boards/arty_z7/make_boot_bin.sh` — packs FSBL + bitstream + `ps_fio.elf` → `build/BOOT.bin`. **(committed `3411097`)**
- `test/test_vgc_dbg_gfx_write.sv` + `test/Makefile` — Verilator test proving the dbg_vmem gfx write. **(committed `4aef325`)**

## What was verified to actually WORK (over JTAG, where the elf definitely runs)
- **dbg_vmem gfx write**: PS self-check logged `[splash] selfcheck gfx[0]: wrote 0A, read 0A`; logo pixels confirmed resident in the gfx plane (`vm read-vram 3 …` non-zero at rows 40–120). Verilator `test_vgc_dbg_gfx_write` passes.
- **NovaBASIC reaches `Ready`**: char plane (`vm read-vram 1 …`) shows the banner + `Ready`.
- **DISPLAY_DIM polarity**: poking `$A0E5` (41189) = `0x0F` made the text VISIBLE; `0x00` = dark/black. So **0x00 = dark, 0x0F = full bright** (NOT "dimming"). Register read-backs of a known-good state: `$A000`(MODE)=0, `$A0E5`(DIM)=0x0F bright, `$A001`(BGCOL)=0, `$A00D`(BORDER)=0x0B gray, color plane=0x0F (white text index), palette[15]=0xB0B0B0 (light gray).
- **Reset cause correction**: AMD UG585 says `REBOOT_STATUS` bit 22 is POR, bit 20 is debug reset, and bit 16 is SWDT. Therefore `0x00400000` is POR history and `0x00500000` is POR + debug reset history, **not** SWDT. A real SWDT reset would set `0x00010000`. `SWDT_ZMR=0x000001C0` is also not proof that SWDT is running because WDEN is bit 0 and is clear in `0x1C0`. Markers `0xCAFE000D` and `0xCAFE0013` print decoded reset bits and watchdog enable bits explicitly.

## Splash logic currently in `boot_splash()` (uncommitted)
1. `poke(VGC_DIM=0x00)` (dark), set mode `$A000`=3 (gfx-only), bg/border/cursor, load `.nvg` palette via `$A0F4`/`$A0F5`.
2. Render: 320×200 pixels into gfx space (3) via `R_VMEM_WRITE` (AXI `0x2C`), `swdt_kick()` per row.
3. Fade in `dim 0→15`, hold, fade out `dim 15→0` using `usleep()` (`sleep.h`), `swdt_kick()` each step.
4. Restore: mode `$A000`=0 (text), `$A0E5`=0x0F (full bright), cursor on, `R_VMEM_ADDR` back to char/0.
5. FSBL hooks force SWDT off before bitstream load and before handoff. `main()` early: `swdt_disable()` + `scuwdt_disable()`. Long init paths and the service loop call `swdt_kick()` as a backup.

Register map used (all on the 6502 bus via `poke()` / `dbg_poke_vgc`):
`$A000` MODE, `$A001` BGCOL, `$A00A` CURSOR_EN, `$A00D` BORDER, `$A0E5` DISPLAY_DIM,
`$A0F4`/`$A0F5` PALETTE_INDEX/DATA. Gfx pixels go via the dbg_vmem write port, NOT poke.

PS AXI registers (`FIO_BASE 0x40000000`): `R_POKE 0x00`, `R_VMEM_ADDR 0x28` (`{space[2:0]@17,addr[16:0]}`), `R_VMEM_DATA 0x2C` (read), `R_VMEM_WRITE 0x2C` (write `{space@25,addr@8,data}`). SWDT: ZMR `0xF8005000` (disable = write `0x00ABC000`), RESTART `0xF8005008` (kick = write `0x1999`). `REBOOT_STATUS 0xF8000258`.

Authoritative references checked:
- AMD UG585 `REBOOT_STATUS` / reset reason: bit 22 POR, bit 20 debug reset, bit 16 SWDT. https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Register-REBOOT_STATUS-Details
- AMD UG585 SWDT ZMR: WDEN is bit 0; reset value includes non-enable bits. https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Register-swdt-XWDTPS_ZMR_OFFSET
- AMD UG585 boot mode / BootROM SD boot behavior. https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/BootROM-Steps

## Build + deploy (this is what we've been doing)
```bash
cd e6502.FPGA/boards/arty_z7
# 0. One-time after build_full_bd.tcl WDT changes: rebuild the full bitstream/XSA:
# vivado -mode batch -source build_full.tcl                         # ~15 min
# 1. Rebuild the PS app (regenerates platform incl FSBL):
rm -rf /tmp/nova_fio_ws
/tools/Xilinx/Vitis/2024.2/bin/vitis -s vitis/build_ps_fio.py     # ~3 min -> /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf
# 2. Pack BOOT.bin (FSBL + build/ps_full/.../arty_z7_full.bit + ps_fio.elf):
./make_boot_bin.sh                                                # -> build/BOOT.bin (~4.77 MB)
# 3a. Push to the SD over the network (board running, on 192.168.1.213):
cd /home/barry/NovaVM
dotnet run --project e6502.Nova -c Release -- put e6502.FPGA/boards/arty_z7/build/BOOT.bin BOOT.bin --remote 192.168.1.213
# -> /sd/BOOT.bin. Then power-cycle (JP4 = SD).

# 3b. If the board is not reachable, mount the SD card on the host and copy:
cp e6502.FPGA/boards/arty_z7/build/BOOT.bin /path/to/boot-volume/BOOT.bin
sync
```
The bitstream itself rarely needs rebuilding (`vivado -mode batch -source build_full.tcl`, ~15 min); only `arty_z7_full.sv`/`fio_bridge.sv`/`rtl/` changes need it.

## How to run/verify WITHOUT power-cycling (JTAG)
```bash
cd e6502.FPGA/boards/arty_z7
/tools/Xilinx/Vitis/2024.2/bin/xsct boot_fio_noreset.tcl   # connect; rst -processor; ps7_init; fpga; dow ps_fio.elf; con
```
**OPEN QUESTION (suspect):** whether a JTAG `dow` actually overrides the SD boot when JP4=SD. If it does NOT, all JTAG "verification" was reading the SD's image, not the freshly-flashed elf. **Use the BUILD MARKER to settle this too.** If JTAG `dow` is being ignored, set JP4=JTAG to verify, OR rely on the SD path + the marker.

## Diagnostics
- Serial: `stty -F /dev/ttyUSB1 115200 raw -echo; cat /dev/ttyUSB1`. `ps_fio` prints raw `REBOOT_STATUS`, decoded `POR/DEBUG/SWDT`, decoded watchdog enable bits, `[splash] …`, `[fio] 6502 released`, and net/DHCP. (FTDI sometimes re-enumerates; only `/dev/ttyUSB1` is the console UART.)
- Screen/VGC memory: `nova vm read-vram <space> <addr> <len>` (space 1=char, 2=color, 3=gfx, 7=textattr). `nova vm peek <addr>` reads a 6502-bus/VGC register.
- The `nova` CLI = `dotnet run --project e6502.Nova -c Release -- …` (or build it).

## Gotchas hit during this work
- **Repeated JTAG flashing wedges the PS Ethernet/PHY** → DHCP stops completing (`No route to host`). A clean power-cycle restores it. The SD boot's network is fine; it's the JTAG `rst` churn that degrades it.
- **`rst -srst`** clears the PL without rebooting (board goes silent); **`rst -por`** is "not supported for target". So a real power-on can't be triggered from JTAG — only the physical power button gives a true POR / SD boot.
- Busy-loop delays are unreliable (speed depends on what else is in the loop body) → use `usleep`/`sleep` (`sleep.h`).
- Leftover fd0 `.ndi` mounts will auto-boot a game and clobber the splash (`nova drive unmount fd0`).

## Recommended order of attack for whoever picks this up
1. Deploy marker `0xCAFE0013`, then capture a fresh serial log after a real physical power cycle and after `rst -system`. Use the decoded `POR/DEBUG/SWDT` line, not raw hex guesses.
2. If marker 13 still resets with `SWDT=0`, skip `audio_timer_init()` next, one checkpoint at a time.
3. Once SD boot reliably reaches the service loop/network again, return to visual validation of the splash fade timing on HDMI.
