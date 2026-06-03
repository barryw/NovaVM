# Test runner — uses Verilator for fast simulation (~200x faster than iverilog)
# Usage: make          — build and run all tests
#        make test_blitter  — build and run one test
#        make clean    — remove build artifacts
#        make CLEAN_BUILD=1 test_debug_bridge  — force a clean Verilator rebuild

VERILATOR = verilator
TRACE ?= 0
TRACE_DEPTH ?= 5
TRACE_MAX_ARRAY ?= 64
TRACE_MAX_WIDTH ?= 4096
CPU_DORMANN_ARGS ?=
VJOBS ?= 0
CLEAN_BUILD ?= 0
TRACE_FILE ?=
SID_MODE ?= 8580
SID_FRAME_TICKS ?= 20000
SID_METRICS ?=
SID_HDMI_SAMPLE_DIV ?= 21

ifeq ($(TRACE),1)
TRACE_FLAGS = --prefix Vtrace_top --trace-fst --trace-structs --trace-params \
              --trace-depth $(TRACE_DEPTH) \
              --trace-max-array $(TRACE_MAX_ARRAY) \
              --trace-max-width $(TRACE_MAX_WIDTH) \
              trace_main.cpp
else
TRACE_FLAGS = --main
endif

VFLAGS    = --cc --exe --build -j $(VJOBS) --timing $(TRACE_FLAGS) \
            -I$(RTL)/vendor/redip_sid \
            -Wno-WIDTHTRUNC -Wno-WIDTHEXPAND -Wno-UNUSEDSIGNAL -Wno-UNUSEDPARAM \
            -Wno-CASEINCOMPLETE -Wno-PROCASSINIT -Wno-UNOPTFLAT -Wno-LATCH \
            -Wno-PINCONNECTEMPTY -Wno-SYNCASYNCNET -Wno-INITIALDLY -Wno-TIMESCALEMOD \
            -Wno-CASEX -Wno-PINMISSING -Wno-UNSIGNED \
            -CFLAGS "-O2"

RTL     = ../rtl
BD_BASE ?= build
BD      = $(BD_BASE)$(if $(filter 1,$(TRACE)),-trace,)

VGC   = $(RTL)/dpram.sv $(RTL)/vgc_timing.sv $(RTL)/vgc_text.sv $(RTL)/vgc_gfx.sv \
        $(RTL)/artist.sv $(RTL)/vgc_sprites.sv $(RTL)/vgc_copper.sv $(RTL)/vgc.sv \
        $(RTL)/thirdparty/sfifo.v
REDIP = $(RTL)/vendor/redip_sid/sid_pkg.sv \
        $(RTL)/vendor/redip_sid/muladd.sv \
        $(RTL)/vendor/redip_sid/sid_dac.sv \
        $(RTL)/vendor/redip_sid/sid_control.sv \
        $(RTL)/vendor/redip_sid/sid_waveform.sv \
        $(RTL)/vendor/redip_sid/sid_envelope.sv \
        $(RTL)/vendor/redip_sid/sid_voice.sv \
        $(RTL)/vendor/redip_sid/sid_filter.sv \
        $(RTL)/vendor/redip_sid/sid_pot.sv
SID   = $(REDIP) $(RTL)/sid/nova_redip_sid_api.sv $(RTL)/sid/sid_chip.sv \
        $(RTL)/sid/sid_curve_reader.sv
WTS   = $(RTL)/wts_chip.sv
CPU   = $(RTL)/arlet_6502/cpu.v $(RTL)/arlet_6502/ALU.v
HDMI  = $(RTL)/thirdparty/hdl-util-hdmi/audio_clock_regeneration_packet.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/audio_info_frame.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/audio_sample_packet.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/auxiliary_video_information_info_frame.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/packet_assembler.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/packet_picker.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/source_product_description_info_frame.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/tmds_channel.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/serializer.sv \
        $(RTL)/thirdparty/hdl-util-hdmi/hdmi.sv

TESTS = test_copper test_copper_lists test_gtext test_vgc_text test_vgc_gfx test_vgc_sprites test_vgc_regs test_vgc_charin_read_timing test_vgc_blitter_scroll test_blitter test_blitter_xram_backpressure test_blitter_cmd_single_pulse_top test_vgc_cmd_readback_hazard_top test_dma test_math_copro test_wts_chip test_cpu_dormann_6502 test_cpu_dormann_interrupt test_cpu_dormann_65c02 test_cpu_stz test_cpu_memory_write_opcodes test_cpu_wai_stp test_hdmi_audio test_hdmi_start_offset test_sid_hdmi_audio test_audio_pcm_fifo test_audio_led_meter test_sid test_sid_per_reg_readback test_sid_adsr test_sid_waveforms test_sid_pulse test_sid_sync test_sid_ringmod test_sid_combined test_sid_curve test_sid_filter test_sid_pervoice_vol test_sid_model test_fio test_nic test_fio_top test_nic_top test_rom_load test_debug_uart test_debug_spi_slave test_usb_hid_keyboard test_debug_bridge test_debug_bridge_rx_fifo test_debug_sdram_port_b_cdc test_novaz_xram_roundtrip test_custom_reset_top test_debug_peek test_key_inject test_rom_over_sid test_romswap_values_top test_sid_cpu_audio_top test_wts_top test_xmc_window_decode_top test_mmio_bus_cycles_top test_novalogo_draw_top test_vgc_frame_counter test_vgc_scroll_wrap test_vgc_top_row test_vgc_screen_window test_vgc_por_determinism test_sdram_loopback test_sdram_model_selfcheck test_sdram_model_selfcheck_fatal test_sdram_model_vs_ctrl test_sdram_stream test_page_dma test_xram_sdram

.PHONY: all clean $(TESTS) run

all: $(TESTS)
	@echo "" && echo "=== All tests completed ==="

# Run from parent dir so $readmemh("rom/cp437.hex") resolves correctly
RUN = cd .. &&
PREP_BUILD = @$(if $(filter 1,$(CLEAN_BUILD)),rm -rf $(BD)/$@ && )mkdir -p $(BD)/$@ &&

test_copper: test_copper.sv $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_copper_lists: test_copper_lists.sv $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_gtext: test_gtext.sv $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_vgc_text: test_vgc_text.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_text.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_gfx: test_vgc_gfx.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_gfx.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_sprites: test_vgc_sprites.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_sprites.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_regs: test_vgc_regs.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_regs.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_top_row: test_vgc_top_row.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_top_row.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_screen_window: test_vgc_screen_window.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_screen_window.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_charin_read_timing: test_vgc_charin_read_timing.sv $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_charin_read_timing.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_blitter_scroll: test_vgc_blitter_scroll.sv $(VGC) $(RTL)/blitter.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $(RTL)/blitter.sv test_vgc_blitter_scroll.sv && $(RUN) test/$(BD)/$@/$@

# POR-determinism regression. Built with --x-assign unique so every
# uninitialized reg starts with a non-zero value — models ECP5 power-on
# indeterminacy. Verifies vid_hsync/vsync/de stay at idle during rst and
# that no phantom sync pulse appears in the first 200 cycles after
# rst-release. Was the boot-artifact bug diagnosed 2026-04-26.
test_vgc_por_determinism: test_vgc_por_determinism.sv vgc_tb.svh $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --x-assign unique --x-initial unique --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_por_determinism.sv && $(RUN) test/$(BD)/$@/$@ +verilator+rand+reset+2

test_blitter: test_blitter.sv $(RTL)/blitter.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/blitter.sv $< && $(RUN) test/$(BD)/$@/$@

# RAM→XRAM back-pressure regression. Forces xram_busy pulses during
# the blitter copy to verify writes are held (not dropped) when the
# XRAM wrapper is busy. Reproduces the integration-level STASH/FETCH
# round-trip failure.
test_blitter_xram_backpressure: test_blitter_xram_backpressure.sv $(RTL)/blitter.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/blitter.sv $< && $(RUN) test/$(BD)/$@/$@

test_blitter_cmd_single_pulse_top: test_blitter_cmd_single_pulse_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

test_vgc_cmd_readback_hazard_top: test_vgc_cmd_readback_hazard_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

test_dma: test_dma.sv $(RTL)/dma.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/dma.sv $< && $(RUN) test/$(BD)/$@/$@

test_math_copro: test_math_copro.sv $(RTL)/math_copro.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/math_copro.sv $< && $(RUN) test/$(BD)/$@/$@

test_wts_chip: test_wts_chip.sv $(WTS)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(WTS) $< && $(RUN) test/$(BD)/$@/$@

test_cpu_dormann_6502: test_cpu_dormann.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module test_cpu_dormann --Mdir $(BD)/$@ -o $@ $(CPU) test_cpu_dormann.sv && $(RUN) test/$(BD)/$@/$@ +NAME=6502_functional +BIN=../e6502UnitTests/Resources/6502_functional_test.bin +LOAD=0 +START=0400 +PASS=3399 +MAX_CYCLES=500000000 $(CPU_DORMANN_ARGS)

test_cpu_dormann_interrupt: test_cpu_dormann.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module test_cpu_dormann --Mdir $(BD)/$@ -o $@ $(CPU) test_cpu_dormann.sv && $(RUN) test/$(BD)/$@/$@ +NAME=65c02_interrupt +BIN=../e6502UnitTests/Resources/6502_interrupt_test.bin +LOAD=0400 +START=0400 +PASS=06EC +MAX_CYCLES=10000000 +INTERRUPTS=1 +CMOS_D_CLEAR=1 $(CPU_DORMANN_ARGS)

test_cpu_dormann_65c02: test_cpu_dormann.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module test_cpu_dormann --Mdir $(BD)/$@ -o $@ $(CPU) test_cpu_dormann.sv && $(RUN) test/$(BD)/$@/$@ +NAME=65c02_extended +BIN=../e6502UnitTests/Resources/65C02_extended_opcodes_test.bin +LOAD=0 +START=0400 +PASS=24A8 +MAX_CYCLES=500000000 $(CPU_DORMANN_ARGS)

test_cpu_wai_stp: test_cpu_wai_stp.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module $@ --Mdir $(BD)/$@ -o $@ $(CPU) $< && $(RUN) test/$(BD)/$@/$@

test_cpu_stz: test_cpu_stz.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module $@ --Mdir $(BD)/$@ -o $@ $(CPU) $< && $(RUN) test/$(BD)/$@/$@

test_cpu_memory_write_opcodes: test_cpu_memory_write_opcodes.sv $(CPU)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSIM --top-module $@ --Mdir $(BD)/$@ -o $@ $(CPU) $< && $(RUN) test/$(BD)/$@/$@

test_hdmi_audio: test_hdmi_audio.sv $(HDMI)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(HDMI) $< && $(RUN) test/$(BD)/$@/$@

test_hdmi_start_offset: test_hdmi_start_offset.sv $(HDMI)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(HDMI) $< && $(RUN) test/$(BD)/$@/$@

test_sid_hdmi_audio: test_sid_hdmi_audio.sv $(RTL)/sid_hdmi_audio.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sid_hdmi_audio.sv $< && $(RUN) test/$(BD)/$@/$@

test_audio_pcm_fifo: test_audio_pcm_fifo.sv $(RTL)/audio_pcm_fifo.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/audio_pcm_fifo.sv $< && $(RUN) test/$(BD)/$@/$@

test_audio_led_meter: test_audio_led_meter.sv $(RTL)/audio_led_meter.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/audio_led_meter.sv $< && $(RUN) test/$(BD)/$@/$@

test_sid: test_sid.sv $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $< && $(RUN) test/$(BD)/$@/$@

# Per-register readback test. Writes a distinct value to every write-register
# then reads each back; validates that each slot shadows its own value
# rather than returning `last_written`.
test_sid_per_reg_readback: test_sid_per_reg_readback.sv $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $< && $(RUN) test/$(BD)/$@/$@

# ADSR rate-timing test. Gates voice 3 on/off and samples env3 at $1C
# across three attack rates and two release rates.
test_sid_adsr: test_sid_adsr.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_adsr.sv && $(RUN) test/$(BD)/$@/$@

# Waveform correctness test. Samples $1B (OSC3 readback) across triangle,
# sawtooth, pulse (50/25/75% duty) and noise. Verifies periodicity,
# distinct-sample count, and duty tracking.
test_sid_waveforms: test_sid_waveforms.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_waveforms.sv && $(RUN) test/$(BD)/$@/$@

# Pulse-width exhaustive sweep. Writes values across the 12-bit PW
# register ($02/$03) and verifies duty cycle tracks (4096-pw)/4096.
test_sid_pulse: test_sid_pulse.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_pulse.sv && $(RUN) test/$(BD)/$@/$@

# Oscillator sync test. V2 drives V3's sync; verify V3 period latches
# to V2 when sync bit is set.
test_sid_sync: test_sid_sync.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_sync.sv && $(RUN) test/$(BD)/$@/$@

# Ring modulation test. Triangle waveform with ringmod bit; V3 ringmods
# from V2. Verify waveform distortion increases markedly when ring is on.
test_sid_ringmod: test_sid_ringmod.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_ringmod.sv && $(RUN) test/$(BD)/$@/$@

# Combined waveforms test. Exercises tri+saw, pulse+tri, pulse+saw, and
# triple-combined outputs in both 6581/8580 modes. Verifies non-trivial
# output and mode-distinct table contents.
test_sid_combined: test_sid_combined.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_combined.sv && $(RUN) test/$(BD)/$@/$@

# Filter curve reader — stand-alone test with a mock SDRAM port B.
# Verifies Fc-change detection, fetch latency, and round-robin between
# SID1 and SID2.
test_sid_curve: test_sid_curve.sv $(RTL)/sid/sid_curve_reader.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sid/sid_curve_reader.sv test_sid_curve.sv && $(RUN) test/$(BD)/$@/$@

# Filter routing + resonance + mode test. Exercises $D417/$D418 bits
# and verifies audio path through direct vs LP/BP/HP filter output.
test_sid_filter: test_sid_filter.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_filter.sv && $(RUN) test/$(BD)/$@/$@

# Per-voice volume extension test. Validates the NovaBASIC $1D/$1E/$1F
# per-voice amplitude registers.
test_sid_pervoice_vol: test_sid_pervoice_vol.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_pervoice_vol.sv && $(RUN) test/$(BD)/$@/$@

# 6581 vs 8580 DAC distortion model test. Verifies mode pin switches
# DC offset, envelope DAC path, and waveform DAC distortion.
test_sid_model: test_sid_model.sv sid_tb.svh $(SID)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_model.sv && $(RUN) test/$(BD)/$@/$@

test_sid_trace_replay: test_sid_trace_replay.sv sid_tb.svh $(SID) $(RTL)/sid_hdmi_audio.sv
	@test -n "$(TRACE_FILE)" || (echo "TRACE_FILE=/path/to/file.rtltrace is required" >&2; exit 1)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $(RTL)/sid_hdmi_audio.sv test_sid_trace_replay.sv && $(RUN) test/$(BD)/$@/$@ +TRACE=$(TRACE_FILE) +MODE=$(SID_MODE) +FRAME_TICKS=$(SID_FRAME_TICKS) +HDMI_SAMPLE_DIV=$(SID_HDMI_SAMPLE_DIV) $(if $(SID_METRICS),+METRICS=$(SID_METRICS),)

# FIO register bank — 80-byte file at $B9A0-$B9EF with fio_event pulse
# on CPU writes to FioCmd. Stand-alone module test.
test_fio: test_fio.sv $(RTL)/fio.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/fio.sv test_fio.sv && $(RUN) test/$(BD)/$@/$@

test_nic: test_nic.sv $(RTL)/nic.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/nic.sv test_nic.sv && $(RUN) test/$(BD)/$@/$@

# FIO integration in top.sv — proves fio.sv is wired correctly through
# the io_group cpu_din mux, dbg_poke routing, and fio_event output.
# Loads a tiny 6502 program that writes to $B9A0/$B9A1, counts
# fio_event pulses, exercises debug-bridge poke/peek of the bank.
test_fio_top: test_fio_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

test_nic_top: test_nic_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Debug-bridge protocol unit test — drives rx_valid/rx_data directly, verifies
# CMD_POKE_ROM, CMD_POKE_ROM_BLK, CMD_RESET_HOLD/REL decode + outputs.
test_debug_uart: test_debug_uart.sv $(RTL)/debug_uart_rx.sv $(RTL)/debug_uart_tx.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_uart_rx.sv $(RTL)/debug_uart_tx.sv $< && $(RUN) test/$(BD)/$@/$@

test_debug_spi_slave: test_debug_spi_slave.sv $(RTL)/debug_spi_slave.sv $(RTL)/debug_async_byte_fifo.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_async_byte_fifo.sv $(RTL)/debug_spi_slave.sv $< && $(RUN) test/$(BD)/$@/$@

test_usb_hid_keyboard: test_usb_hid_keyboard.sv $(RTL)/usb_hid_keyboard.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DUSB_HID_HOST_EXTERNAL_STUB --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/usb_hid_keyboard.sv $< && $(RUN) test/$(BD)/$@/$@

test_debug_bridge: test_debug_bridge.sv $(RTL)/debug_bridge.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_bridge.sv $< && $(RUN) test/$(BD)/$@/$@

# RX FIFO regression for host->bridge SDRAM writes. Proves a full 256-byte
# debug packet survives while the SDRAM side is backpressured.
test_debug_bridge_rx_fifo: test_debug_bridge_rx_fifo.sv $(RTL)/debug_bridge.sv $(RTL)/debug_byte_fifo.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_byte_fifo.sv $(RTL)/debug_bridge.sv $< && $(RUN) test/$(BD)/$@/$@

test_debug_sdram_port_b_cdc: test_debug_sdram_port_b_cdc.sv $(RTL)/sdram/debug_sdram_port_b_cdc.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/debug_sdram_port_b_cdc.sv $< && $(RUN) test/$(BD)/$@/$@

# NovaZ/XLOAD-shaped XRAM roundtrip. Loads a synthetic story blob through the
# same SDRAM debug commands NovaHost FIO uses, then reads each chunk back.
test_novaz_xram_roundtrip: test_novaz_xram_roundtrip.sv $(RTL)/debug_bridge.sv $(RTL)/debug_byte_fifo.sv $(RTL)/sdram/debug_sdram_port_b_cdc.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_byte_fifo.sv $(RTL)/debug_bridge.sv $(RTL)/sdram/debug_sdram_port_b_cdc.sv $< && $(RUN) test/$(BD)/$@/$@

# End-to-end test: instantiates full top.sv (CPU + VGC + SID + blitter + RAM/ROM),
# streams a tiny 6502 program into basic_rom via dbg_rom_we, verifies execution.
ROM_LOAD_RTL = $(VGC) $(RTL)/dpram_dc.sv $(SID) $(WTS) $(RTL)/blitter.sv $(RTL)/dma.sv $(RTL)/math_copro.sv $(RTL)/fio.sv $(RTL)/nic.sv $(RTL)/arlet_6502/cpu.v $(RTL)/arlet_6502/ALU.v $(RTL)/top.sv $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv
test_rom_load: test_rom_load.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level custom-chip reset regression. Proves dbg_system_reset resets
# custom-device state and VGC BRAM-backed display contents.
test_custom_reset_top: test_custom_reset_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Debug peek regression test — exercises top.sv's debug-peek mux on the
# synthesis code path (`ifdef SYNTHESIS`). Reproduces the hardware bug where
# peek of VGC-owned addresses falls through to main RAM.
test_debug_peek: test_debug_peek.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Key-injection drop-rate regression test — drives key_valid/key_data directly
# on top.sv, peeks $A00F to observe char_in_reg. Reproduces the hardware-observed
# behavior where rapid-fire keys overwrite each other (single-entry latch, not FIFO).
test_key_inject: test_key_inject.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# ROM-over-SID regression — proves reads from $D400/$D420/$D440 return
# ROM bytes, not SID register shadows. Guards against reintroducing the
# cpu_din-mux SID intercept that silently broke array-handling code
# because EhBASIC places live ROM at those addresses. Replaces the old
# test_sid_readback which tested the (now removed) SID-over-ROM intercept.
test_rom_over_sid: test_rom_over_sid.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level ROM bank-switch regression. Proves ROMSWAP_EXTENSION selects the
# extension bank and every runtime value returns to the primary runtime bank.
test_romswap_values_top: test_romswap_values_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level SID audio regression. Runs real CPU writes into both SID chips
# and verifies signed audio output changes.
test_sid_cpu_audio_top: test_sid_cpu_audio_top.sv $(ROM_LOAD_RTL) $(RTL)/sid_hdmi_audio.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $(RTL)/sid_hdmi_audio.sv $< && $(RUN) test/$(BD)/$@/$@

# Top-level WTS regression. Verifies the Nova WTS register range is routed
# through the synthesized CPU/debug bus and contributes stereo audio.
test_wts_top: test_wts_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level XMC mapped-window decode regression. Verifies the synthesis path
# keeps WnAL readable/writable but ignores it for 256-byte window translation.
test_xmc_window_decode_top: test_xmc_window_decode_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level XMC read-only regression. Verifies the NovaZ-shaped
# `LDA $BF00,X` path never pulses the XRAM write path.
test_xmc_read_no_write_top: test_xmc_read_no_write_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Top-level MMIO bus-cycle regression. Runs real CPU indirect/indexed/RMW
# writes into FIO, VGC, XMC, DMA, and blitter registers.
test_mmio_bus_cycles_top: test_mmio_bus_cycles_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

test_novalogo_draw_top: test_novalogo_draw_top.sv $(ROM_LOAD_RTL)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Regression for VGC $A008 frame counter. Before the fix $A008 was mapped
# to gfx_color (static) so BASIC's SPRITESET vblank-wait hung the CPU.
# This test probes the internal signal to prove it increments.
test_vgc_frame_counter: test_vgc_frame_counter.sv $(VGC)
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

# Regression for the scroll-offset 7-row mirror bug. Stand-alone test
# against vgc_timing.sv — brute-forces the full 25x25 text_row x
# scroll_offset grid. Before the fix, scroll_offset >= 8 causes
# text_rows 18..24 to collide with rows 0..7 because the 5-bit sum
# wraps before the `>= ROWS` fixup runs.
test_vgc_scroll_wrap: test_vgc_scroll_wrap.sv $(RTL)/vgc_timing.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/vgc_timing.sv $< && $(RUN) test/$(BD)/$@/$@

# SDRAM controller loopback — stand-alone, no top.sv. Instantiates sdram.v
# against a behavioural MT48LC16M16 model and probes a write→read roundtrip.
# First exit gate for Phase 2.5 Step 1.
test_sdram_loopback: test_sdram_loopback.sv $(RTL)/sdram/sdram.v
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v $< && $(RUN) test/$(BD)/$@/$@

# Page-mode MT48LC16M16 behavioral model self-check — self-contained, NO RTL deps
# (pokes sdram_model.svh directly). The model is an include file, so the test source
# and the .svh are the only inputs.
#
# LEGAL path: ACTIVATE -> tRCD -> READ -> CAS returns the stored word + byte lanes.
test_sdram_model_selfcheck: test_sdram_model_selfcheck.sv sdram_model.svh
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ test_sdram_model_selfcheck.sv && $(RUN) test/$(BD)/$@/$@
#
# ILLEGAL path: ACTIVATE -> READ before tRCD must $fatal. The model's $fatal exits
# non-zero, so we INVERT the exit code: a trap (non-zero) is PASS, a clean finish
# (zero) is FAIL. Built in its own Mdir; reuses the same source with +EXPECT_FATAL.
test_sdram_model_selfcheck_fatal: test_sdram_model_selfcheck.sv sdram_model.svh
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module test_sdram_model_selfcheck --Mdir $(BD)/$@ -o $@ test_sdram_model_selfcheck.sv && \
	  ( $(RUN) test/$(BD)/$@/$@ +EXPECT_FATAL=1 ; \
	    rc=$$? ; \
	    if [ $$rc -ne 0 ]; then echo "  PASS illegal path trapped (\$$fatal, exit $$rc)" ; \
	    else echo "  FAIL illegal path did NOT trap (exit 0)" ; exit 1 ; fi )

# Ground-truth CAS-latency check: wires the REAL sdram.v controller to the
# behavioral sdram_model.svh and writes/reads several distinct byte addresses.
# The controller is already proven by test_sdram_loopback, so this passes only
# if the model's read-data timing matches real silicon (CAS_LATENCY=3). Its only
# RTL dep is sdram.v (mirrors test_sdram_loopback).
test_sdram_model_vs_ctrl: test_sdram_model_vs_ctrl.sv sdram_model.svh $(RTL)/sdram/sdram.v
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v test_sdram_model_vs_ctrl.sv && $(RUN) test/$(BD)/$@/$@

# Byte-exact page-mode stream readback — wires real sdram.v to sdram_model.svh,
# writes a 512-byte ramp via port A, then streams 256 words and checks byte
# order. RED against the Task-2 stream stub; goes green when the Task-4 FSM
# lands. Only RTL dep is sdram.v (mirrors test_sdram_model_vs_ctrl).
test_sdram_stream: test_sdram_stream.sv sdram_model.svh $(RTL)/sdram/sdram.v
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v test_sdram_stream.sv && $(RUN) test/$(BD)/$@/$@

# page_dma consumer — wires real sdram.v stream port -> page_dma -> a dpram
# standing in for bank-1 ext_rom. Pre-loads a 16 KB pattern into sdram_model,
# triggers page_dma, then reads the dpram back byte-exact + byte order.
# RED against the Task-10 stub; goes green when the page_dma FSM lands.
test_page_dma: test_page_dma.sv sdram_model.svh $(RTL)/sdram/sdram.v $(RTL)/page_dma.sv $(RTL)/dpram.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v $(RTL)/page_dma.sv $(RTL)/dpram.sv test_page_dma.sv && $(RUN) test/$(BD)/$@/$@

# xram_sdram wrapper — pixel-clock handshake around sdram.v port A.
# First exit gate for Phase 2.5 Step 2 (XRAM BRAM stub → SDRAM).
test_xram_sdram: test_xram_sdram.sv $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv
	$(PREP_BUILD) $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv $< && $(RUN) test/$(BD)/$@/$@

clean:
	rm -rf $(BD)
