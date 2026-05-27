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
OSS_CAD_SUITE_BIN ?= /opt/homebrew/oss-cad-suite/bin
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
EXTRA_DEFINES ?= -DVIDEO_720X480 -DGPDI_P_ONLY -DLATTICE_ECP5
SV = read_verilog -sv -DSYNTHESIS -DYOSYS -I$(RTLDIR)/vendor/redip_sid $(EXTRA_DEFINES)
VL = read_verilog -DSYNTHESIS $(EXTRA_DEFINES)
SYNTH_ARGS = $(BUILDDIR)/synth_args.txt
PNR_ARGS = $(BUILDDIR)/pnr_args.txt
# hdl-util HDMI generates very deep temporary names. Yosys' AUTONAME pass can
# spend tens of GB on those names after ABC9 even though nextpnr does not need
# them, so the production flow runs the ECP5 recipe through mapped cells and
# then performs the useful checks/output explicitly.
SYNTH_ECP5_JSON = synth_ecp5 -top fpga_top -run begin:map_cells; techmap -map +/lattice/cells_map_trellis.v; opt_lut_ins -tech lattice; clean; hierarchy -check; stat; check -noinit; blackbox =A:whitebox

NEXTPNR_SEED ?= 1
NEXTPNR_SEEDS ?= 1 2 3 4 5
NEXTPNR_THREADS ?=
NEXTPNR_PRE_PLACE ?= place_hdmi_serializer.py
NEXTPNR_PLACER_CELL_TIMEOUT ?= 8
NEXTPNR_EXTRA_FLAGS ?=
NEXTPNR_REPORT = $(BUILDDIR)/nextpnr-report.json
NEXTPNR_PLACE_REPORT = $(BUILDDIR)/nextpnr-place-report.json
NEXTPNR_PLACE_LOG = $(BUILDDIR)/nextpnr-place.log
NEXTPNR_PLACE_CONFIG = $(BUILDDIR)/$(PROJ).place.config
NEXTPNR_PLACE_MARGIN_MHZ ?= 0.5
TIMING_CHECKER ?= ../../tools/check-fpga-timing.py
NOVA_FLOORPLAN_MODE ?= critical
NOVA_FLOORPLAN_ENFORCE ?=
NEXTPNR_THREAD_FLAGS = $(if $(strip $(NEXTPNR_THREADS)),--threads $(NEXTPNR_THREADS),)
NEXTPNR_PRE_PLACE_FLAGS = $(if $(strip $(NEXTPNR_PRE_PLACE)),--pre-place $(NEXTPNR_PRE_PLACE),)
NEXTPNR_PRE_PLACE_DEPS = $(if $(strip $(NEXTPNR_PRE_PLACE)),$(NEXTPNR_PRE_PLACE),)
NEXTPNR_PLACER_TIMEOUT_FLAGS = --placer-heap-cell-placement-timeout $(NEXTPNR_PLACER_CELL_TIMEOUT)
NEXTPNR_BASE_FLAGS = --$(DEVICE) --package $(PACKAGE) --freq 25 --seed $(NEXTPNR_SEED) $(NEXTPNR_THREAD_FLAGS) $(NEXTPNR_PRE_PLACE_FLAGS) $(NEXTPNR_PLACER_TIMEOUT_FLAGS) $(NEXTPNR_EXTRA_FLAGS)
NEXTPNR_FLAGS = $(NEXTPNR_BASE_FLAGS) --report $(NEXTPNR_REPORT)
NEXTPNR_PLACE_FLAGS = $(NEXTPNR_BASE_FLAGS) --no-route --log $(NEXTPNR_PLACE_LOG) --report $(NEXTPNR_PLACE_REPORT)
NEXTPNR_ENV = NOVA_FLOORPLAN_MODE='$(NOVA_FLOORPLAN_MODE)' NOVA_FLOORPLAN_ENFORCE='$(NOVA_FLOORPLAN_ENFORCE)'

.PHONY: all synth placecheck seed-sweep pnr bitstream prog prog_flash clean fast ehbasic diag diag-vgc diag-sprites FORCE

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
          $(RTLDIR)/vgc_copper.sv $(RTLDIR)/blitter.sv \
          $(RTLDIR)/dma.sv $(RTLDIR)/fio.sv \
          $(RTLDIR)/thirdparty/sfifo.v
RTL_BRIDGE = $(RTLDIR)/debug_byte_fifo.sv \
             $(RTLDIR)/debug_async_byte_fifo.sv \
             $(RTLDIR)/debug_spi_slave.sv
# Production keeps the FPGA-side USB keyboard path enabled. Set
# ENABLE_USB_HID=0 only for diagnostics when isolating routing pressure.
ENABLE_USB_HID ?= 1
ifeq ($(ENABLE_USB_HID),1)
override EXTRA_DEFINES += -DENABLE_USB_HID
RTL_USB = $(RTLDIR)/usb_hid_keyboard.sv \
          $(RTLDIR)/thirdparty/usb_hid_host_fs/usb_hid_host.v \
          $(RTLDIR)/thirdparty/usb_hid_host_fs/usb_hid_host_rom.v
else
RTL_USB =
endif
USB_HID_ROM_HEX = $(RTLDIR)/thirdparty/usb_hid_host_fs/usb_hid_host_rom.mem
ifeq ($(ENABLE_USB_HID),1)
USB_HID_DEPS = $(USB_HID_ROM_HEX)
else
USB_HID_DEPS =
endif
RTL_TOP  = $(RTLDIR)/top.sv $(RTLDIR)/ecp5pll.sv $(RTLDIR)/debug_bridge.sv \
           $(RTLDIR)/sid_hdmi_audio.sv $(RTLDIR)/audio_pcm_fifo.sv \
           $(RTLDIR)/audio_led_meter.sv \
           $(RTLDIR)/wts_chip.sv \
           $(RTLDIR)/nic.sv $(RTLDIR)/math_copro.sv \
           fpga_top.sv \
           uart_rx.sv $(RTL_BRIDGE) $(RTL_USB)
RTL_CPU  = $(RTLDIR)/arlet_6502/cpu.v $(RTLDIR)/arlet_6502/ALU.v
RTL_REDIP = $(RTLDIR)/vendor/redip_sid/sid_pkg.sv \
            $(RTLDIR)/vendor/redip_sid/muladd.sv \
            $(RTLDIR)/vendor/redip_sid/sid_dac.sv \
            $(RTLDIR)/vendor/redip_sid/sid_control.sv \
            $(RTLDIR)/vendor/redip_sid/sid_waveform.sv \
            $(RTLDIR)/vendor/redip_sid/sid_envelope.sv \
            $(RTLDIR)/vendor/redip_sid/sid_voice.sv \
            $(RTLDIR)/vendor/redip_sid/sid_filter.sv \
            $(RTLDIR)/vendor/redip_sid/sid_pot.sv
RTL_SID  = $(RTL_REDIP) $(RTLDIR)/sid/nova_redip_sid_api.sv $(RTLDIR)/sid/sid_chip.sv \
           $(RTLDIR)/sid/sid_curve_reader.sv
RTL_HDMI = $(RTLDIR)/thirdparty/hdl-util-hdmi/audio_clock_regeneration_packet.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/audio_info_frame.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/audio_sample_packet.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/auxiliary_video_information_info_frame.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/packet_assembler.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/packet_picker.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/source_product_description_info_frame.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/tmds_channel.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/serializer.sv \
           $(RTLDIR)/thirdparty/hdl-util-hdmi/hdmi.sv
RTL_SDRAM_V  = $(RTLDIR)/sdram/sdram.v
RTL_SDRAM_SV = $(RTLDIR)/sdram/xram_sdram.sv $(RTLDIR)/sdram/debug_sdram_port_b_cdc.sv

# =========================================================================
# Synthesis — rebuilds only when source files change (don't 'make clean'!)
# =========================================================================
$(SYNTH_ARGS): FORCE
	@mkdir -p $(BUILDDIR)
	@printf '%s\n' \
		'EXTRA_DEFINES=$(EXTRA_DEFINES)' \
		> $(BUILDDIR)/build_args.new
	@if ! cmp -s $(BUILDDIR)/build_args.new $@; then \
		mv $(BUILDDIR)/build_args.new $@; \
	else \
		rm -f $(BUILDDIR)/build_args.new; \
	fi

$(PNR_ARGS): FORCE
	@mkdir -p $(BUILDDIR)
	@printf '%s\n' \
		'DEVICE=$(DEVICE)' \
		'PACKAGE=$(PACKAGE)' \
		'PIN_DEF=$(PIN_DEF)' \
		'NEXTPNR_FLAGS=$(NEXTPNR_FLAGS)' \
		'NEXTPNR_SEED=$(NEXTPNR_SEED)' \
		'NEXTPNR_SEEDS=$(NEXTPNR_SEEDS)' \
		'NEXTPNR_PRE_PLACE=$(NEXTPNR_PRE_PLACE)' \
		'NEXTPNR_PLACE_MARGIN_MHZ=$(NEXTPNR_PLACE_MARGIN_MHZ)' \
		'NOVA_FLOORPLAN_MODE=$(NOVA_FLOORPLAN_MODE)' \
		'NOVA_FLOORPLAN_ENFORCE=$(NOVA_FLOORPLAN_ENFORCE)' \
		> $(BUILDDIR)/build_args.new
	@if ! cmp -s $(BUILDDIR)/build_args.new $@; then \
		mv $(BUILDDIR)/build_args.new $@; \
	else \
		rm -f $(BUILDDIR)/build_args.new; \
	fi

$(BUILDDIR)/$(PROJ).json: $(SYNTH_ARGS) $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex $(USB_HID_DEPS)
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_SDRAM_V); \
		$(SYNTH_ECP5_JSON); \
		write_json $@"

# =========================================================================
# Diagnostic — run yosys only (no nextpnr/ecppack) to dump post-synth
# verilog + ilang + stats for inspection. Much faster than full synth
# (~3-5 min vs ~15). Use when debugging what yosys transformed.
# =========================================================================
diag: $(BUILDDIR)/post_synth.v

diag-vgc: $(BUILDDIR)/post_synth_vgc.v

diag-sprites: $(BUILDDIR)/post_synth_sprites.v

$(BUILDDIR)/post_synth.v: $(SYNTH_ARGS) $(RTL_VGC) $(RTL_TOP) $(RTL_CPU) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_V) $(RTL_SDRAM_SV) ../rom/ehbasic.hex ../rom/extension.hex $(USB_HID_DEPS)
	@mkdir -p $(BUILDDIR)
	$(YOSYS) -p "\
		verilog_defaults -add -DSYNTHESIS; \
		$(SV) $(RTL_VGC) $(RTL_TOP) $(RTL_SID) $(RTL_HDMI) $(RTL_SDRAM_SV); \
		$(VL) $(RTL_CPU) $(RTL_SDRAM_V); \
		$(SYNTH_ECP5_JSON); \
		stat; \
		write_verilog $@; \
		write_rtlil $(BUILDDIR)/post_synth.il"
	@echo "Post-synth artifacts:"
	@echo "  Verilog: $@"
	@echo "  ILANG:   $(BUILDDIR)/post_synth.il"

$(BUILDDIR)/post_synth_vgc.v: $(SYNTH_ARGS) $(RTL_VGC)
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

$(BUILDDIR)/post_synth_sprites.v: $(SYNTH_ARGS) $(DPRAM) $(RTLDIR)/vgc_sprites.sv
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
placecheck: ehbasic $(BUILDDIR)/$(PROJ).json $(PIN_DEF) $(NEXTPNR_PRE_PLACE_DEPS)
	@mkdir -p $(BUILDDIR)
	$(NEXTPNR_ENV) $(NEXTPNR) $(NEXTPNR_PLACE_FLAGS) --textcfg $(NEXTPNR_PLACE_CONFIG) --json $(BUILDDIR)/$(PROJ).json --lpf $(PIN_DEF)
	python3 $(TIMING_CHECKER) $(NEXTPNR_PLACE_REPORT) --log $(NEXTPNR_PLACE_LOG) --margin-mhz $(NEXTPNR_PLACE_MARGIN_MHZ)

seed-sweep: ehbasic $(BUILDDIR)/$(PROJ).json $(PIN_DEF) $(NEXTPNR_PRE_PLACE_DEPS)
	@mkdir -p $(BUILDDIR)
	@failed=0; \
	for seed in $(NEXTPNR_SEEDS); do \
		echo "=== placement seed $$seed ==="; \
		$(MAKE) --no-print-directory -o ehbasic NEXTPNR_SEED=$$seed placecheck; \
		rc=$$?; \
		if [ -f "$(NEXTPNR_PLACE_REPORT)" ]; then \
			cp "$(NEXTPNR_PLACE_REPORT)" "$(BUILDDIR)/nextpnr-place-report.seed$$seed.json"; \
		fi; \
		if [ $$rc -ne 0 ]; then \
			failed=1; \
		fi; \
	done; \
	exit $$failed

$(BUILDDIR)/$(PROJ).config: $(PNR_ARGS) $(BUILDDIR)/$(PROJ).json $(PIN_DEF) $(NEXTPNR_PRE_PLACE_DEPS)
	$(NEXTPNR_ENV) $(NEXTPNR) $(NEXTPNR_FLAGS) --textcfg $@ --json $(BUILDDIR)/$(PROJ).json --lpf $(PIN_DEF)

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
