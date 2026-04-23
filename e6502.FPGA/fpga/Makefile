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

.PHONY: all synth pnr bitstream prog prog_flash clean fast ehbasic diag

all: ehbasic bitstream

# Refresh ROMs from ehbasic source before any synthesis run. Delegates to the
# parent Makefile's hex target, which rebuilds ehbasic and regenerates
# rom/*.hex only when content changed (so synth skips unchanged builds).
ehbasic:
	@$(MAKE) --no-print-directory -C .. hex

# =========================================================================
# Source files
# =========================================================================
RTL_VGC = $(DPRAM) $(RTLDIR)/vgc.sv $(RTLDIR)/vgc_timing.sv $(RTLDIR)/vgc_text.sv \
          $(RTLDIR)/vgc_gfx.sv $(RTLDIR)/artist.sv $(RTLDIR)/vgc_sprites.sv \
          $(RTLDIR)/vgc_copper.sv $(RTLDIR)/tile_engine.sv $(RTLDIR)/blitter.sv \
          $(RTLDIR)/thirdparty/sfifo.v
RTL_TOP  = $(RTLDIR)/top.sv $(RTLDIR)/ecp5pll.sv $(RTLDIR)/debug_bridge.sv fpga_top.sv uart_rx.sv uart_tx.sv
RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
RTL_SID  = $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv $(RTLDIR)/sid/sid_envelope.sv \
           $(RTLDIR)/sid/sid_filter.sv $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv \
           $(RTLDIR)/sid/sid_curve_reader.sv
RTL_HDMI = $(RTLDIR)/hdmi/vga2dvid.v $(RTLDIR)/hdmi/tmds_encoder.v $(RTLDIR)/hdmi/fake_differential.v
RTL_SDRAM_V  = $(RTLDIR)/sdram/sdram.v
RTL_SDRAM_SV = $(RTLDIR)/sdram/xram_sdram.sv

# =========================================================================
# Synthesis — rebuilds only when source files change (don't 'make clean'!)
# =========================================================================
$(BUILDDIR)/$(PROJ).json: $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_HDMI) $(RTL_SDRAM_V); \
		synth_ecp5 -noflatten -top fpga_top -json $@; \
		write_verilog $(BUILDDIR)/post_synth.v; \
		write_rtlil $(BUILDDIR)/post_synth.il"

# =========================================================================
# Diagnostic — run yosys only (no nextpnr/ecppack) to dump post-synth
# verilog + ilang + stats for inspection. Much faster than full synth
# (~3-5 min vs ~15). Use when debugging what yosys transformed.
# =========================================================================
diag: $(BUILDDIR)/post_synth.v

$(BUILDDIR)/post_synth.v: $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_HDMI) $(RTL_SDRAM_V); \
		synth_ecp5 -noflatten -top fpga_top; \
		stat; \
		write_verilog $@; \
		write_rtlil $(BUILDDIR)/post_synth.il"
	@echo "Post-synth artifacts:"
	@echo "  Verilog: $@"
	@echo "  ILANG:   $(BUILDDIR)/post_synth.il"

synth: ehbasic $(BUILDDIR)/$(PROJ).json

# =========================================================================
# Place & Route + Bitstream
# =========================================================================
$(BUILDDIR)/$(PROJ).config: $(BUILDDIR)/$(PROJ).json $(PIN_DEF)
	nextpnr-ecp5 $(NEXTPNR_FLAGS) --textcfg $@ --json $< --lpf $(PIN_DEF)

pnr: ehbasic $(BUILDDIR)/$(PROJ).config

$(BUILDDIR)/$(PROJ).bit: $(BUILDDIR)/$(PROJ).config
	ecppack --compress $< $@

bitstream: ehbasic $(BUILDDIR)/$(PROJ).bit

# Program
prog: ehbasic $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s $(BUILDDIR)/$(PROJ).bit

prog_flash: ehbasic $(BUILDDIR)/$(PROJ).bit
	openFPGALoader --board ulx3s -f $(BUILDDIR)/$(PROJ).bit

# Fast: build + flash to SRAM in one command
fast: ehbasic bitstream
	openFPGALoader --board ulx3s $(BUILDDIR)/$(PROJ).bit

clean:
	rm -rf $(BUILDDIR)
