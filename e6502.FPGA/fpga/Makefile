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

.PHONY: all synth pnr bitstream prog prog_flash clean

all: bitstream

# =========================================================================
# Cached sub-module synthesis — only rebuilds when source changes
# =========================================================================

# CPU (Arlet 6502) — never changes
$(BUILDDIR)/cpu.json: $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -q -p "$(VL) $^; synth_ecp5 -top cpu -json $@"
	@echo "  [cached] cpu"

# SID sound chip — rarely changes
$(BUILDDIR)/sid.json: $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv \
                      $(RTLDIR)/sid/sid_envelope.sv $(RTLDIR)/sid/sid_filter.sv \
                      $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -q -p "$(SV) $^; synth_ecp5 -top sid_chip -json $@"
	@echo "  [cached] sid"

# HDMI output — never changes
$(BUILDDIR)/hdmi.json: $(RTLDIR)/hdmi/tmds_encoder.v $(RTLDIR)/hdmi/vga2dvid.v \
                       $(RTLDIR)/hdmi/fake_differential.v
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -q -p "$(VL) $^; synth_ecp5 -top fake_differential -json $@"
	@echo "  [cached] hdmi"

# Blitter — rarely changes
$(BUILDDIR)/blitter.json: $(RTLDIR)/blitter.sv
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -q -p "$(SV) $^; synth_ecp5 -top blitter -json $@"
	@echo "  [cached] blitter"

# Tile engine (MOSAIC) — slow to synth, rarely changes
$(BUILDDIR)/tile_engine.json: $(DPRAM) $(RTLDIR)/tile_engine.sv
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -q -p "$(SV) $^; synth_ecp5 -top tile_engine -json $@"
	@echo "  [cached] tile_engine"

CACHED_JSONS = $(BUILDDIR)/cpu.json $(BUILDDIR)/sid.json $(BUILDDIR)/hdmi.json \
               $(BUILDDIR)/blitter.json $(BUILDDIR)/tile_engine.json

# =========================================================================
# Top-level synthesis — loads cached modules, synthesizes the rest
# Only VGC chips + top + fpga wrapper are re-synthesized each time
# =========================================================================
RTL_VGC = $(DPRAM) $(RTLDIR)/vgc.sv $(RTLDIR)/vgc_timing.sv $(RTLDIR)/vgc_text.sv \
          $(RTLDIR)/vgc_gfx.sv $(RTLDIR)/artist.sv $(RTLDIR)/vgc_sprites.sv \
          $(RTLDIR)/vgc_copper.sv $(RTLDIR)/tile_engine.sv $(RTLDIR)/blitter.sv
RTL_TOP = $(RTLDIR)/top.sv $(RTLDIR)/ecp5pll.sv fpga_top.sv uart_rx.sv

RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
RTL_SID  = $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv $(RTLDIR)/sid/sid_envelope.sv \
           $(RTLDIR)/sid/sid_filter.sv $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv
RTL_HDMI = $(RTLDIR)/hdmi/tmds_encoder.v $(RTLDIR)/hdmi/vga2dvid.v $(RTLDIR)/hdmi/fake_differential.v

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

# Build all cached modules (parallel)
cache: $(CACHED_JSONS)
	@echo "All cached modules built"

clean:
	rm -rf $(BUILDDIR)
