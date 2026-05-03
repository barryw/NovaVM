PROJ     = e6502
DEVICE   = 85k
PACKAGE  = CABGA381
PIN_DEF  = ulx3s.lpf
BUILDDIR = build
RTLDIR   = ../rtl

# OSS CAD Suite (yosys, nextpnr-ecp5, ecppack, fujprog) — use absolute
# paths so the build works regardless of the user's shell configuration.
# `export PATH := ...` is unreliable across Make versions on macOS — some
# recipes exec directly and bypass the export. Absolute paths always work.
OSS_CAD_SUITE_BIN := /opt/homebrew/oss-cad-suite/bin
export PATH := $(OSS_CAD_SUITE_BIN):$(PATH)

DPRAM = $(RTLDIR)/dpram.sv

YOSYS    = $(OSS_CAD_SUITE_BIN)/yosys
NEXTPNR  = $(OSS_CAD_SUITE_BIN)/nextpnr-ecp5
ECPPACK  = $(OSS_CAD_SUITE_BIN)/ecppack
FPGALOAD = openFPGALoader
# Production ULX3S build defaults:
# - VIDEO_720X480 gives the 720x480 HDMI timing + 640x400 Nova canvas.
# - GPDI_P_ONLY uses ECP5 LVCMOS33D on the positive GPDI pins and lets the
#   IO buffer generate the complementary pins. Without this, nextpnr rejects
#   gpdi_dn[*] because the LPF's LVCMOS33D pins must be constrained to the
#   A side of a pseudo-differential pair.
EXTRA_DEFINES ?= -DVIDEO_720X480 -DGPDI_P_ONLY
SV = read_verilog -sv -DSYNTHESIS $(EXTRA_DEFINES)
VL = read_verilog -DSYNTHESIS $(EXTRA_DEFINES)
BUILD_ARGS = $(BUILDDIR)/build_args.txt

NEXTPNR_SEED ?= 1
NEXTPNR_THREADS ?=
NEXTPNR_REPORT = $(BUILDDIR)/nextpnr-report.json
NEXTPNR_THREAD_FLAGS = $(if $(strip $(NEXTPNR_THREADS)),--threads $(NEXTPNR_THREADS),)
NEXTPNR_FLAGS = --$(DEVICE) --package $(PACKAGE) --freq 25 --seed $(NEXTPNR_SEED) $(NEXTPNR_THREAD_FLAGS) --report $(NEXTPNR_REPORT) --timing-allow-fail

.PHONY: all synth pnr bitstream prog prog_flash clean fast ehbasic diag diag-vgc diag-sprites FORCE

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
          $(RTLDIR)/dma.sv $(RTLDIR)/fio.sv $(RTLDIR)/thirdparty/sfifo.v
RTL_UART = $(RTLDIR)/debug_uart_rx.sv \
           $(RTLDIR)/debug_uart_tx.sv \
           $(RTLDIR)/debug_byte_fifo.sv
RTL_TOP  = $(RTLDIR)/top.sv $(RTLDIR)/ecp5pll.sv $(RTLDIR)/debug_bridge.sv fpga_top.sv uart_rx.sv uart_tx.sv $(RTL_UART)
RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
RTL_SID  = $(RTLDIR)/sid/sid_chip.sv $(RTLDIR)/sid/sid_voice.sv $(RTLDIR)/sid/sid_envelope.sv \
           $(RTLDIR)/sid/sid_filter.sv $(RTLDIR)/sid/sid_tables.sv $(RTLDIR)/sid/sid_dac.sv \
           $(RTLDIR)/sid/sid_curve_reader.sv
RTL_HDMI = $(RTLDIR)/hdmi/vga2dvid.v $(RTLDIR)/hdmi/tmds_encoder.v \
           $(RTLDIR)/hdmi/fake_differential.v $(RTLDIR)/hdmi/hdmi_device_ref.v
RTL_SDRAM_V  = $(RTLDIR)/sdram/sdram.v
RTL_SDRAM_SV = $(RTLDIR)/sdram/xram_sdram.sv $(RTLDIR)/sdram/debug_sdram_port_b_cdc.sv

# =========================================================================
# Synthesis — rebuilds only when source files change (don't 'make clean'!)
# =========================================================================
$(BUILD_ARGS): FORCE
	@mkdir -p $(BUILDDIR)
	@printf '%s\n' \
		'DEVICE=$(DEVICE)' \
		'PACKAGE=$(PACKAGE)' \
		'PIN_DEF=$(PIN_DEF)' \
		'NEXTPNR_FLAGS=$(NEXTPNR_FLAGS)' \
		'NEXTPNR_SEED=$(NEXTPNR_SEED)' \
		'EXTRA_DEFINES=$(EXTRA_DEFINES)' \
		> $(BUILDDIR)/build_args.new
	@if ! cmp -s $(BUILDDIR)/build_args.new $@; then \
		mv $(BUILDDIR)/build_args.new $@; \
	else \
		rm -f $(BUILDDIR)/build_args.new; \
	fi

$(BUILDDIR)/$(PROJ).json: $(BUILD_ARGS) $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_HDMI) $(RTL_SDRAM_V); \
		synth_ecp5 -top fpga_top; \
		write_json $@"

# =========================================================================
# Diagnostic — run yosys only (no nextpnr/ecppack) to dump post-synth
# verilog + ilang + stats for inspection. Much faster than full synth
# (~3-5 min vs ~15). Use when debugging what yosys transformed.
# =========================================================================
diag: $(BUILDDIR)/post_synth.v

diag-vgc: $(BUILDDIR)/post_synth_vgc.v

diag-sprites: $(BUILDDIR)/post_synth_sprites.v

$(BUILDDIR)/post_synth.v: $(BUILD_ARGS) $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_HDMI) $(RTL_SDRAM_V); \
		synth_ecp5 -top fpga_top; \
		stat; \
		write_verilog $@; \
		write_rtlil $(BUILDDIR)/post_synth.il"
	@echo "Post-synth artifacts:"
	@echo "  Verilog: $@"
	@echo "  ILANG:   $(BUILDDIR)/post_synth.il"

$(BUILDDIR)/post_synth_vgc.v: $(BUILD_ARGS) $(RTL_VGC)
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC); \
		synth_ecp5 -top vgc; \
		stat; \
		write_verilog $@; \
		write_rtlil $(BUILDDIR)/post_synth_vgc.il"
	@echo "VGC post-synth artifacts:"
	@echo "  Verilog: $@"
	@echo "  ILANG:   $(BUILDDIR)/post_synth_vgc.il"

$(BUILDDIR)/post_synth_sprites.v: $(BUILD_ARGS) $(DPRAM) $(RTLDIR)/vgc_sprites.sv
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(DPRAM) $(RTLDIR)/vgc_sprites.sv; \
		synth_ecp5 -top vgc_sprites; \
		stat; \
		write_verilog $@; \
		write_rtlil $(BUILDDIR)/post_synth_sprites.il"
	@echo "Sprite post-synth artifacts:"
	@echo "  Verilog: $@"
	@echo "  ILANG:   $(BUILDDIR)/post_synth_sprites.il"

synth: ehbasic $(BUILDDIR)/$(PROJ).json

# =========================================================================
# Place & Route + Bitstream
# =========================================================================
$(BUILDDIR)/$(PROJ).config: $(BUILD_ARGS) $(BUILDDIR)/$(PROJ).json $(PIN_DEF)
	$(NEXTPNR) $(NEXTPNR_FLAGS) --textcfg $@ --json $(BUILDDIR)/$(PROJ).json --lpf $(PIN_DEF)

pnr: ehbasic $(BUILDDIR)/$(PROJ).config

$(BUILDDIR)/$(PROJ).bit: $(BUILDDIR)/$(PROJ).config
	$(ECPPACK) --compress $< $@

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
