#!/usr/bin/env bash
# build_uboot_oracle.sh — build U-Boot for the Arty Z7-20 as a JTAG-bootable USB
# diagnostic "oracle". This was used to prove that the USB-host keyboard
# enumeration failure is HARDWARE/DEVICE-level (U-Boot's proven ehci-zynq driver
# fails identically to our bare-metal usb.c), not a bug in our driver.
#
# Boot it with boot_uboot.tcl (ps7_init + SLCR-unlock + dow u-boot.elf + con),
# read the console on /dev/ttyUSB1 @115200, then at the "Zynq>" prompt run
# `usb reset`, `usb tree`, `usb info` to inspect USB enumeration.
#
# Key Arty-specific fixes (vs the stock zynq-zybo-z7 DTB + xilinx_zynq_virt cfg):
#   * console UART0 (Arty USB-UART), not UART1 (Zybo)
#   * DRAM 512 MB (Arty), not 1 GB (Zybo)
#   * ps-clk-frequency = 50 MHz (Arty crystal!), not 33.33 MHz -> else every
#     clock incl. the UART baud is off by 1.5x (garbage console)
#   * OF_EMBED + DEFAULT_DEVICE_TREE=zynq-zybo-z7, OF_BOARD off -> DTB is in the
#     ELF (JTAG dow has no prior-stage DTB handoff; else fdtdec_setup -> hang())
#   * SPL off, SKIP_LOWLEVEL_INIT on, ZYNQ_DDRC_INIT off -> ps7_init already did
#     the low-level bring-up; U-Boot must not redo it
set -euo pipefail
UB=/tmp/u-boot-xlnx
export PATH=/tools/Xilinx/Vitis/2024.2/gnu/aarch32/lin/gcc-arm-none-eabi/bin:$PATH
export CROSS_COMPILE=arm-none-eabi-

[ -d "$UB" ] || git clone --depth 1 https://github.com/Xilinx/u-boot-xlnx.git "$UB"
cd "$UB"
make xilinx_zynq_virt_defconfig

DTS=arch/arm/dts/zynq-zybo-z7.dts
sed -i 's/serial0 = &uart1;/serial0 = \&uart0;/'           $DTS
sed -i 's/^&uart1 {/\&uart0 {/'                            $DTS
sed -i 's/reg = <0x0 0x40000000>;/reg = <0x0 0x20000000>;/' $DTS   # 512MB
sed -i 's/ps-clk-frequency = <33333333>;/ps-clk-frequency = <50000000>;/' $DTS

./scripts/config --enable OF_EMBED --disable OF_BOARD \
                 --set-str DEFAULT_DEVICE_TREE "zynq-zybo-z7" \
                 --disable SPL --disable SPL_FRAMEWORK \
                 --enable SKIP_LOWLEVEL_INIT --disable ZYNQ_DDRC_INIT \
                 --enable USB_KEYBOARD --enable CMD_USB
make olddefconfig
make DEVICE_TREE=zynq-zybo-z7 -j"$(nproc)"
echo "Built $UB/u-boot.elf — boot with: xsct boot_uboot.tcl"
