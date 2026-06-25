#!/bin/bash
set -e
source /tools/Xilinx/Vivado/2024.2/settings64.sh >/dev/null 2>&1
cd /home/barry/NovaVM/e6502.FPGA/boards/arty_z7/sim
RTL=/home/barry/NovaVM/e6502.FPGA/rtl
BRD=/home/barry/NovaVM/e6502.FPGA/boards/arty_z7
REDIP=$RTL/vendor/redip_sid
rm -rf xsim.dir *.jou *.log *.pb .Xil 2>/dev/null || true
echo "=== xvlog (SystemVerilog) ==="
xvlog -sv -d SYNTHESIS=1 -d VIDEO_720X480=1 -i $REDIP \
  $BRD/sid_stubs.sv $RTL/dpram.sv $RTL/dpram_dc.sv \
  vgc_sim.sv $RTL/vgc_timing.sv $RTL/vgc_text.sv $RTL/vgc_gfx.sv \
  $RTL/artist.sv $RTL/vgc_sprites.sv $RTL/vgc_copper.sv $RTL/blitter.sv $RTL/dma.sv \
  $RTL/page_in_ctrl.sv $RTL/page_dma.sv $RTL/fio.sv $RTL/rom_load_cdc_fifo.sv \
  $RTL/math_copro.sv $RTL/wts_chip.sv $RTL/nic.sv \
  $RTL/sdram/xram_sdram.sv $RTL/sdram/debug_sdram_port_b_cdc.sv \
  $RTL/top.sv tb_top.sv 2>&1 | grep -iE "error|fail|cannot|unknown" | head -30 || echo "  (sv ok)"
echo "=== xvlog (Verilog) ==="
xvlog -d SYNTHESIS=1 $RTL/arlet_6502/cpu.v $RTL/arlet_6502/ALU.v $RTL/thirdparty/sfifo.v 2>&1 | grep -iE "error|fail" | head || echo "  (v ok)"
echo "=== xelab ==="
xelab tb_top -s tbsim --timescale 1ns/1ps -relax 2>&1 | grep -iE "error|fail|unresolved|cannot" | head -30 || echo "  (elab ok)"
echo "=== xsim ==="
xsim tbsim -runall 2>&1 | grep -vE "^$" | tail -120
