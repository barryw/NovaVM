# Arty Z7-20 (XC7Z020) builds.
#
#   make             # build the LED/UART smoke-test bitstream
#   make prog        # program the smoke test over JTAG (volatile)
#   make hdmi        # build the HDMI-OUT test-pattern bitstream
#   make prog-hdmi   # program the HDMI test over JTAG (volatile)
#   make hw_server   # ensure a hw_server daemon is running
#   make clean

TOP       = arty_z7_smoke
BUILDDIR  = build
BIT       = $(BUILDDIR)/$(TOP).bit

HDMI_TOP  = arty_z7_hdmi_test
HDMI_BIT  = $(BUILDDIR)/$(HDMI_TOP).bit

VGC_TOP   = arty_z7_vgc
VGC_BIT   = $(BUILDDIR)/$(VGC_TOP).bit

# AMD/Xilinx toolchain. Override on the command line if installed elsewhere.
XILINX_DIR ?= /tools/Xilinx
VIVADO     ?= $(XILINX_DIR)/Vivado/2024.2/bin/vivado
HW_SERVER  ?= $(XILINX_DIR)/Vivado/2024.2/bin/hw_server

VIVADO_BATCH = $(VIVADO) -mode batch -nojournal -notrace

HDMI_SRC = $(wildcard ../../rtl/thirdparty/hdl-util-hdmi/*.sv)

.PHONY: all bitstream prog hdmi prog-hdmi vgc prog-vgc vgcgfx prog-vgcgfx cpu prog-cpu hw_server clean

all: bitstream

bitstream: $(BIT)

$(BIT): $(TOP).sv $(TOP).xdc build.tcl ../../rtl/uart_rx.sv ../../rtl/uart_tx.sv
	$(VIVADO_BATCH) -log $(BUILDDIR)/vivado.log -source build.tcl

hdmi: $(HDMI_BIT)

$(HDMI_BIT): $(HDMI_TOP).sv $(HDMI_TOP).xdc build_hdmi.tcl $(HDMI_SRC)
	$(VIVADO_BATCH) -log $(BUILDDIR)/vivado_hdmi.log -source build_hdmi.tcl

prog-hdmi: hw_server $(HDMI_BIT)
	$(VIVADO_BATCH) -source program.tcl -tclargs $(HDMI_BIT)

VGC_SRC = $(HDMI_SRC) $(wildcard ../../rtl/vgc*.sv ../../rtl/artist.sv ../../rtl/dpram.sv)

vgc: $(VGC_BIT)

$(VGC_BIT): $(VGC_TOP).sv $(VGC_TOP).xdc build_vgc.tcl $(VGC_SRC)
	$(VIVADO_BATCH) -log $(BUILDDIR)/vivado_vgc.log -source build_vgc.tcl

prog-vgc: hw_server $(VGC_BIT)
	$(VIVADO_BATCH) -source program.tcl -tclargs $(VGC_BIT)

VGCGFX_TOP = arty_z7_vgc_gfx
VGCGFX_BIT = $(BUILDDIR)/$(VGCGFX_TOP).bit

vgcgfx: $(VGCGFX_BIT)

$(VGCGFX_BIT): $(VGCGFX_TOP).sv arty_z7_vgc.xdc build_vgcgfx.tcl $(VGC_SRC)
	$(VIVADO_BATCH) -log $(BUILDDIR)/vivado_vgcgfx.log -source build_vgcgfx.tcl

prog-vgcgfx: hw_server $(VGCGFX_BIT)
	$(VIVADO_BATCH) -source program.tcl -tclargs $(VGCGFX_BIT)

CPU_TOP = arty_z7_cpu
CPU_BIT = $(BUILDDIR)/$(CPU_TOP).bit
CPU_SRC = $(wildcard ../../rtl/*.sv ../../rtl/sid/*.sv ../../rtl/sdram/*.sv \
            ../../rtl/vendor/redip_sid/*.sv ../../rtl/arlet_6502/*.v) $(HDMI_SRC)

cpu: $(CPU_BIT)

$(CPU_BIT): $(CPU_TOP).sv arty_z7_vgc.xdc build_cpu.tcl $(CPU_SRC)
	$(VIVADO_BATCH) -log $(BUILDDIR)/vivado_cpu.log -source build_cpu.tcl

prog-cpu: hw_server $(CPU_BIT)
	$(VIVADO_BATCH) -source program.tcl -tclargs $(CPU_BIT)

# Start a JTAG hardware server if one isn't already listening on 3121.
hw_server:
	@pgrep -f hw_server >/dev/null 2>&1 || { \
		echo "starting hw_server..."; \
		$(HW_SERVER) -d >/dev/null 2>&1 & \
		sleep 3; }

prog: hw_server $(BIT)
	$(VIVADO_BATCH) -source program.tcl

clean:
	rm -rf $(BUILDDIR) .Xil vivado*.jou vivado*.log
