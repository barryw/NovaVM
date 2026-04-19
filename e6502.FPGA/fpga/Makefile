PROJ     = e6502
DEVICE   = 85k
PACKAGE  = CABGA381
PIN_DEF  = ulx3s.lpf
BUILDDIR = build
RTLDIR   = ../rtl

DPRAM = $(RTLDIR)/dpram.sv

YOSYS = yosys
SV = read_verilog -sv -DSYNTHESIS
VL = read_verilog -DSYNTHESIS

NEXTPNR_FLAGS = --$(DEVICE) --package $(PACKAGE) --freq 25 --timing-allow-fail

.PHONY: all synth pnr bitstream prog prog_flash clean fast

all: bitstream

# =========================================================================
# Source files
# =========================================================================
RTL_VGC = $(DPRAM) $(RTLDIR)/vgc.sv $(RTLDIR)/vgc_timing.sv $(RTLDIR)/vgc_text.sv \
          $(RTLDIR)/vgc_gfx.sv $(RTLDIR)/artist.sv $(RTLDIR)/vgc_sprites.sv \
          $(RTLDIR)/vgc_copper.sv $(RTLDIR)/tile_engine.sv $(RTLDIR)/blitter.sv
RTL_TOP  = $(RTLDIR)/top.sv $(RTLDIR)/ecp5pll.sv $(RTLDIR)/debug_bridge.sv fpga_top.sv uart_rx.sv uart_tx.sv
RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
RTL_SID  = $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv $(RTLDIR)/sid/sid_envelope.sv \
           $(RTLDIR)/sid/sid_filter.sv $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv
RTL_HDMI = $(RTLDIR)/hdmi/vga2dvid.v $(RTLDIR)/hdmi/tmds_encoder.v $(RTLDIR)/hdmi/fake_differential.v

# =========================================================================
# Synthesis — rebuilds only when source files change (don't 'make clean'!)
# =========================================================================
$(BUILDDIR)/$(PROJ).json: $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) ../rom/ehbasic.hex ../rom/extension.hex
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID); \
		$(VL) $(RTL_CPU) $(RTL_HDMI); \
		synth_ecp5 -noflatten -top fpga_top -json $@"

synth: $(BUILDDIR)/$(PROJ).json

# =========================================================================
# Place & Route + Bitstream
# =========================================================================
$(BUILDDIR)/$(PROJ).config: $(BUILDDIR)/$(PROJ).json $(PIN_DEF)
	nextpnr-ecp5 $(NEXTPNR_FLAGS) --textcfg $@ --json $< --lpf $(PIN_DEF)

pnr: $(BUILDDIR)/$(PROJ).config

$(BUILDDIR)/$(PROJ).bit: $(BUILDDIR)/$(PROJ).config
	ecppack --compress $< $@

bitstream: $(BUILDDIR)/$(PROJ).bit

# Program
prog: $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s $<

prog_flash: $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s -f $<

# Fast: build + flash to SRAM in one command
fast: bitstream
	openFPGALoader --board ulx3s $(BUILDDIR)/$(PROJ).bit

clean:
	rm -rf $(BUILDDIR)
