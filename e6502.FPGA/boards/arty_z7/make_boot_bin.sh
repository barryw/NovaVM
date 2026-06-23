#!/usr/bin/env bash
# Build BOOT.bin for non-volatile (SD/QSPI) boot: FSBL + arty_z7_full.bit + ps_fio.elf.
# This makes the Arty boot NovaVM straight from power-on (JP4 = SD) instead of needing
# a JTAG download every time -- the Zynq equivalent of flashing the ULX3S SPI flash.
#
# Prereqs (all produced by the normal build flow):
#   1. bitstream:  vivado -mode batch -source build_full.tcl   -> build/ps_full/.../arty_z7_full.bit
#   2. PS app + FSBL: vitis -s vitis/build_ps_fio.py           -> /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf
#                                                                  + nova_fio_plat/.../sw/boot/fsbl.elf
# Deploy:
#   nova put build/BOOT.bin BOOT.bin --remote <ip>   # -> /sd/BOOT.bin (FAT root the BootROM reads)
#   set JP4 = SD, then power-cycle.                  # boots FSBL -> bitstream -> ps_fio -> splash -> BASIC
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
WS="${NOVA_FIO_WS:-/tmp/nova_fio_ws}"
FSBL="$WS/nova_fio_plat/export/nova_fio_plat/sw/boot/fsbl.elf"
BIT="$HERE/build/ps_full/ps_full.runs/impl_1/arty_z7_full.bit"
APP="$WS/ps_fio/build/ps_fio.elf"
BOOTGEN="${BOOTGEN:-/tools/Xilinx/Vitis/2024.2/bin/bootgen}"

for f in "$FSBL" "$BIT" "$APP"; do
    [ -f "$f" ] || { echo "ERROR: missing $f (run the build flow above)"; exit 1; }
done

cat > "$HERE/build/boot.bif" <<EOF
the_ROM_image:
{
	[bootloader]$FSBL
	$BIT
	$APP
}
EOF

"$BOOTGEN" -arch zynq -image "$HERE/build/boot.bif" -w on -o "$HERE/build/BOOT.bin"
echo "BOOT.bin -> $HERE/build/BOOT.bin"
ls -la "$HERE/build/BOOT.bin"
