PROJ     = e6502
DEVICE   = 85k
PACKAGE  = CABGA381
PIN_DEF  = ulx3s.lpf
BUILDDIR = build
RTLDIR   = ../rtl

# Common dependency
DPRAM = $(RTLDIR)/dpram.sv

# All RTL sources
RTL_CORE = $(DPRAM) $(RTLDIR)/top.sv $(RTLDIR)/vgc.sv \
           $(RTLDIR)/vgc_timing.sv $(RTLDIR)/vgc_text.sv $(RTLDIR)/vgc_gfx.sv \
           $(RTLDIR)/artist.sv $(RTLDIR)/vgc_sprites.sv $(RTLDIR)/vgc_copper.sv \
           $(RTLDIR)/tile_engine.sv $(RTLDIR)/blitter.sv

RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v

RTL_SID  = $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv $(RTLDIR)/sid/sid_envelope.sv \
           $(RTLDIR)/sid/sid_filter.sv $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv

RTL_HDMI = $(RTLDIR)/hdmi/tmds_encoder.v $(RTLDIR)/hdmi/vga2dvid.v $(RTLDIR)/hdmi/fake_differential.v

RTL_PLL  = $(RTLDIR)/ecp5pll.sv

RTL_FPGA = fpga_top.sv uart_rx.sv

# ROM hex files
ROM_HEX = ../rom/ehbasic.hex ../rom/extension.hex

NEXTPNR_FLAGS = --$(DEVICE) --package $(PACKAGE) --freq 25 --timing-allow-fail

YOSYS = yosys
SV = read_verilog -sv -DSYNTHESIS
VL = read_verilog -DSYNTHESIS

.PHONY: all synth pnr bitstream prog prog_flash clean

all: bitstream

# =========================================================================
# Synthesis: all modules in one Yosys invocation with -noflatten
# -noflatten preserves module hierarchy so ABC processes each module
# individually — prevents ABC memory explosion on the combined design.
# Large memories in top.sv use dpram (BRAM) under SYNTHESIS define,
# so they don't expand into LUT fabric.
# =========================================================================
$(BUILDDIR)/$(PROJ).json: $(RTL_FPGA) $(RTL_PLL) $(RTL_CORE) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(ROM_HEX)
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_FPGA) $(RTL_PLL) $(RTL_CORE) $(RTL_SID); \
		$(VL) $(RTL_CPU) $(RTL_HDMI); \
		synth_ecp5 -noflatten -top fpga_top -json $@"

synth: $(BUILDDIR)/$(PROJ).json

# Place & Route
$(BUILDDIR)/$(PROJ).config: $(BUILDDIR)/$(PROJ).json $(PIN_DEF)
	nextpnr-ecp5 $(NEXTPNR_FLAGS) --textcfg $@ --json $< --lpf $(PIN_DEF)

pnr: $(BUILDDIR)/$(PROJ).config

# Bitstream
$(BUILDDIR)/$(PROJ).bit: $(BUILDDIR)/$(PROJ).config
	ecppack --compress $< $@

bitstream: $(BUILDDIR)/$(PROJ).bit

# Program to SRAM (volatile)
prog: $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s $<

# Program to flash (persistent)
prog_flash: $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s -f $<

clean:
	rm -rf $(BUILDDIR)
