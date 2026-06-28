#!/bin/bash
# Reusable VGC render sim: Verilate the real rtl/vgc.sv + submodules, run the
# C++ tb, render one active frame to vgc.ppm. Iterate on RTL without a bitstream.
set -e
cd /home/barry/NovaVM/e6502.FPGA/boards/arty_z7/sim/vgc
RTL=/home/barry/NovaVM/e6502.FPGA/rtl
rm -rf obj_dir vgc.ppm
verilator --cc --exe --build -j 0 \
  -DSYNTHESIS=1 -DVIDEO_720X480=1 --top-module vgc \
  -Wno-WIDTH -Wno-UNOPTFLAT -Wno-CASEINCOMPLETE -Wno-WIDTHCONCAT -Wno-UNSIGNED \
  -Wno-CMPCONST -Wno-LATCH -Wno-BLKANDNBLK -Wno-MULTIDRIVEN -Wno-SELRANGE \
  -Wno-IMPLICIT -Wno-PINMISSING -Wno-WIDTHTRUNC -Wno-WIDTHEXPAND -Wno-fatal \
  -I$RTL \
  $RTL/vgc.sv $RTL/vgc_timing.sv $RTL/vgc_text.sv $RTL/vgc_gfx.sv \
  $RTL/artist.sv $RTL/vgc_sprites.sv $RTL/vgc_copper.sv $RTL/dpram.sv $RTL/dpram_dc.sv $RTL/thirdparty/sfifo.v \
  vgc_render_tb.cpp 2>&1 | grep -iE '%error|%warning.*(module|cannot|not found|undefined)|error:' | head -30 || true
echo "=== run ==="
[ -x obj_dir/Vvgc ] && ./obj_dir/Vvgc || echo "BUILD FAILED (no obj_dir/Vvgc)"
