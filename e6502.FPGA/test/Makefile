# Test runner — uses Verilator for fast simulation (~200x faster than iverilog)
# Usage: make          — build and run all tests
#        make test_blitter  — build and run one test
#        make clean    — remove build artifacts

VERILATOR = verilator
VFLAGS    = --cc --exe --build --timing --main \
            -Wno-WIDTHTRUNC -Wno-WIDTHEXPAND -Wno-UNUSEDSIGNAL -Wno-UNUSEDPARAM \
            -Wno-CASEINCOMPLETE -Wno-PROCASSINIT -Wno-UNOPTFLAT -Wno-LATCH \
            -Wno-PINCONNECTEMPTY -Wno-SYNCASYNCNET -Wno-INITIALDLY -Wno-TIMESCALEMOD \
            -Wno-CASEX -Wno-PINMISSING \
            -CFLAGS "-O2"

RTL   = ../rtl
BD    = build

VGC   = $(RTL)/dpram.sv $(RTL)/vgc_timing.sv $(RTL)/vgc_text.sv $(RTL)/vgc_gfx.sv \
        $(RTL)/artist.sv $(RTL)/vgc_sprites.sv $(RTL)/vgc_copper.sv $(RTL)/vgc.sv $(RTL)/tile_engine.sv \
        $(RTL)/thirdparty/sfifo.v
SID   = $(RTL)/sid/sid_chip.sv $(RTL)/sid/sid_voice.sv $(RTL)/sid/sid_envelope.sv \
        $(RTL)/sid/sid_filter.sv $(RTL)/sid/sid_tables.sv $(RTL)/sid/sid_dac.sv \
        $(RTL)/sid/sid_curve_reader.sv

TESTS = test_copper test_copper_lists test_gtext test_vgc_text test_vgc_gfx test_vgc_sprites test_vgc_regs test_tile_engine test_blitter test_blitter_xram_backpressure test_dma test_sid test_sid_per_reg_readback test_sid_adsr test_sid_waveforms test_sid_pulse test_sid_sync test_sid_ringmod test_sid_combined test_sid_curve test_sid_filter test_sid_pervoice_vol test_sid_model test_fio test_fio_top test_rom_load test_debug_bridge test_debug_peek test_key_inject test_rom_over_sid test_vgc_frame_counter test_vgc_scroll_wrap test_vgc_por_determinism test_sdram_loopback test_xram_sdram

.PHONY: all clean $(TESTS) run

all: $(TESTS)
	@echo "" && echo "=== All tests completed ==="

# Run from parent dir so $readmemh("rom/cp437.hex") resolves correctly
RUN = cd .. &&

test_copper: test_copper.sv $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_copper_lists: test_copper_lists.sv $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_gtext: test_gtext.sv $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

test_vgc_text: test_vgc_text.sv vgc_tb.svh $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_text.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_gfx: test_vgc_gfx.sv vgc_tb.svh $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_gfx.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_sprites: test_vgc_sprites.sv vgc_tb.svh $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_sprites.sv && $(RUN) test/$(BD)/$@/$@

test_vgc_regs: test_vgc_regs.sv vgc_tb.svh $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_regs.sv && $(RUN) test/$(BD)/$@/$@

# POR-determinism regression. Built with --x-assign unique so every
# uninitialized reg starts with a non-zero value — models ECP5 power-on
# indeterminacy. Verifies vid_hsync/vsync/de stay at idle during rst and
# that no phantom sync pulse appears in the first 200 cycles after
# rst-release. Was the boot-artifact bug diagnosed 2026-04-26.
test_vgc_por_determinism: test_vgc_por_determinism.sv vgc_tb.svh $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --x-assign unique --x-initial unique --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) test_vgc_por_determinism.sv && $(RUN) test/$(BD)/$@/$@ +verilator+rand+reset+2

test_tile_engine: test_tile_engine.sv $(RTL)/dpram.sv $(RTL)/tile_engine.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/dpram.sv $(RTL)/tile_engine.sv $< && $(RUN) test/$(BD)/$@/$@

test_blitter: test_blitter.sv $(RTL)/blitter.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/blitter.sv $< && $(RUN) test/$(BD)/$@/$@

# RAM→XRAM back-pressure regression. Forces xram_busy pulses during
# the blitter copy to verify writes are held (not dropped) when the
# XRAM wrapper is busy. Reproduces the integration-level STASH/FETCH
# round-trip failure.
test_blitter_xram_backpressure: test_blitter_xram_backpressure.sv $(RTL)/blitter.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/blitter.sv $< && $(RUN) test/$(BD)/$@/$@

test_dma: test_dma.sv $(RTL)/dma.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/dma.sv $< && $(RUN) test/$(BD)/$@/$@

test_sid: test_sid.sv $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $< && $(RUN) test/$(BD)/$@/$@

# Per-register readback test. Writes a distinct value to every write-register
# then reads each back; validates that each slot shadows its own value
# rather than returning `last_written`.
test_sid_per_reg_readback: test_sid_per_reg_readback.sv $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $< && $(RUN) test/$(BD)/$@/$@

# ADSR rate-timing test. Gates voice 3 on/off and samples env3 at $1C
# across three attack rates and two release rates.
test_sid_adsr: test_sid_adsr.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_adsr.sv && $(RUN) test/$(BD)/$@/$@

# Waveform correctness test. Samples $1B (OSC3 readback) across triangle,
# sawtooth, pulse (50/25/75% duty) and noise. Verifies periodicity,
# distinct-sample count, and duty tracking.
test_sid_waveforms: test_sid_waveforms.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_waveforms.sv && $(RUN) test/$(BD)/$@/$@

# Pulse-width exhaustive sweep. Writes values across the 12-bit PW
# register ($02/$03) and verifies duty cycle tracks (4096-pw)/4096.
test_sid_pulse: test_sid_pulse.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_pulse.sv && $(RUN) test/$(BD)/$@/$@

# Oscillator sync test. V2 drives V3's sync; verify V3 period latches
# to V2 when sync bit is set.
test_sid_sync: test_sid_sync.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_sync.sv && $(RUN) test/$(BD)/$@/$@

# Ring modulation test. Triangle waveform with ringmod bit; V3 ringmods
# from V2. Verify waveform distortion increases markedly when ring is on.
test_sid_ringmod: test_sid_ringmod.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_ringmod.sv && $(RUN) test/$(BD)/$@/$@

# Combined waveforms test. Exercises tri+saw, pulse+tri, pulse+saw, and
# triple-combined outputs in both 6581/8580 modes. Verifies non-trivial
# output and mode-distinct table contents.
test_sid_combined: test_sid_combined.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_combined.sv && $(RUN) test/$(BD)/$@/$@

# Filter curve reader — stand-alone test with a mock SDRAM port B.
# Verifies Fc-change detection, fetch latency, and round-robin between
# SID1 and SID2.
test_sid_curve: test_sid_curve.sv $(RTL)/sid/sid_curve_reader.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sid/sid_curve_reader.sv test_sid_curve.sv && $(RUN) test/$(BD)/$@/$@

# Filter routing + resonance + mode test. Exercises $D417/$D418 bits
# and verifies audio path through direct vs LP/BP/HP filter output.
test_sid_filter: test_sid_filter.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_filter.sv && $(RUN) test/$(BD)/$@/$@

# Per-voice volume extension test. Validates the NovaBASIC $1D/$1E/$1F
# per-voice amplitude registers.
test_sid_pervoice_vol: test_sid_pervoice_vol.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_pervoice_vol.sv && $(RUN) test/$(BD)/$@/$@

# 6581 vs 8580 DAC distortion model test. Verifies mode pin switches
# DC offset, envelope DAC path, and waveform DAC distortion.
test_sid_model: test_sid_model.sv sid_tb.svh $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) test_sid_model.sv && $(RUN) test/$(BD)/$@/$@

# FIO register bank — 80-byte file at $B9A0-$B9EF with fio_event pulse
# on CPU writes to FioCmd. Stand-alone module test.
test_fio: test_fio.sv $(RTL)/fio.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/fio.sv test_fio.sv && $(RUN) test/$(BD)/$@/$@

# FIO integration in top.sv — proves fio.sv is wired correctly through
# the io_group cpu_din mux, dbg_poke routing, and fio_event output.
# Loads a tiny 6502 program that writes to $B9A0/$B9A1, counts
# fio_event pulses, exercises debug-bridge poke/peek of the bank.
test_fio_top: test_fio_top.sv $(ROM_LOAD_RTL)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Debug-bridge protocol unit test — drives rx_valid/rx_data directly, verifies
# CMD_POKE_ROM, CMD_POKE_ROM_BLK, CMD_RESET_HOLD/REL decode + outputs.
test_debug_bridge: test_debug_bridge.sv $(RTL)/debug_bridge.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_bridge.sv $< && $(RUN) test/$(BD)/$@/$@

# End-to-end test: instantiates full top.sv (CPU + VGC + SID + blitter + RAM/ROM),
# streams a tiny 6502 program into basic_rom via dbg_rom_we, verifies execution.
ROM_LOAD_RTL = $(VGC) $(SID) $(RTL)/blitter.sv $(RTL)/dma.sv $(RTL)/fio.sv $(RTL)/arlet_6502/cpu.v $(RTL)/arlet_6502/ALU.v $(RTL)/top.sv $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv
test_rom_load: test_rom_load.sv $(ROM_LOAD_RTL)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Debug peek regression test — exercises top.sv's debug-peek mux on the
# synthesis code path (`ifdef SYNTHESIS`). Reproduces the hardware bug where
# peek of VGC-owned addresses falls through to main RAM.
test_debug_peek: test_debug_peek.sv $(ROM_LOAD_RTL)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Key-injection drop-rate regression test — drives key_valid/key_data directly
# on top.sv, peeks $A00F to observe char_in_reg. Reproduces the hardware-observed
# behavior where rapid-fire keys overwrite each other (single-entry latch, not FIFO).
test_key_inject: test_key_inject.sv $(ROM_LOAD_RTL)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# ROM-over-SID regression — proves reads from $D400/$D420/$D440 return
# ROM bytes, not SID register shadows. Guards against reintroducing the
# cpu_din-mux SID intercept that silently broke array-handling code
# because EhBASIC places live ROM at those addresses. Replaces the old
# test_sid_readback which tested the (now removed) SID-over-ROM intercept.
test_rom_over_sid: test_rom_over_sid.sv $(ROM_LOAD_RTL)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -DSYNTHESIS -Wno-IMPLICITSTATIC -Wno-SELRANGE -Wno-MULTIDRIVEN -Wno-CMPCONST --top-module $@ --Mdir $(BD)/$@ -o $@ $(ROM_LOAD_RTL) $< && $(RUN) test/$(BD)/$@/$@

# Regression for VGC $A008 frame counter. Before the fix $A008 was mapped
# to gfx_color (static) so BASIC's SPRITESET vblank-wait hung the CPU.
# This test probes the internal signal to prove it increments.
test_vgc_frame_counter: test_vgc_frame_counter.sv $(VGC)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(VGC) $< && $(RUN) test/$(BD)/$@/$@

# Regression for the scroll-offset 7-row mirror bug. Stand-alone test
# against vgc_timing.sv — brute-forces the full 25x25 text_row x
# scroll_offset grid. Before the fix, scroll_offset >= 8 causes
# text_rows 18..24 to collide with rows 0..7 because the 5-bit sum
# wraps before the `>= ROWS` fixup runs.
test_vgc_scroll_wrap: test_vgc_scroll_wrap.sv $(RTL)/vgc_timing.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/vgc_timing.sv $< && $(RUN) test/$(BD)/$@/$@

# SDRAM controller loopback — stand-alone, no top.sv. Instantiates sdram.v
# against a behavioural MT48LC16M16 model and probes a write→read roundtrip.
# First exit gate for Phase 2.5 Step 1.
test_sdram_loopback: test_sdram_loopback.sv $(RTL)/sdram/sdram.v
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v $< && $(RUN) test/$(BD)/$@/$@

# xram_sdram wrapper — pixel-clock handshake around sdram.v port A.
# First exit gate for Phase 2.5 Step 2 (XRAM BRAM stub → SDRAM).
test_xram_sdram: test_xram_sdram.sv $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/sdram/sdram.v $(RTL)/sdram/xram_sdram.sv $< && $(RUN) test/$(BD)/$@/$@

clean:
	rm -rf $(BD)
