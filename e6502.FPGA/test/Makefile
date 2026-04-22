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
        $(RTL)/sid/sid_filter.sv $(RTL)/sid/sid_tables.sv $(RTL)/sid/sid_dac.sv

TESTS = test_copper test_copper_lists test_gtext test_vgc_text test_vgc_gfx test_vgc_sprites test_vgc_regs test_tile_engine test_blitter test_sid test_rom_load test_debug_bridge test_debug_peek test_key_inject

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

test_tile_engine: test_tile_engine.sv $(RTL)/dpram.sv $(RTL)/tile_engine.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/dpram.sv $(RTL)/tile_engine.sv $< && $(RUN) test/$(BD)/$@/$@

test_blitter: test_blitter.sv $(RTL)/blitter.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/blitter.sv $< && $(RUN) test/$(BD)/$@/$@

test_sid: test_sid.sv $(SID)
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) -Wno-IMPLICITSTATIC -Wno-SELRANGE --top-module $@ --Mdir $(BD)/$@ -o $@ $(SID) $< && $(RUN) test/$(BD)/$@/$@

# Debug-bridge protocol unit test — drives rx_valid/rx_data directly, verifies
# CMD_POKE_ROM, CMD_POKE_ROM_BLK, CMD_RESET_HOLD/REL decode + outputs.
test_debug_bridge: test_debug_bridge.sv $(RTL)/debug_bridge.sv
	@rm -rf $(BD)/$@ && mkdir -p $(BD)/$@ && $(VERILATOR) $(VFLAGS) --top-module $@ --Mdir $(BD)/$@ -o $@ $(RTL)/debug_bridge.sv $< && $(RUN) test/$(BD)/$@/$@

# End-to-end test: instantiates full top.sv (CPU + VGC + SID + blitter + RAM/ROM),
# streams a tiny 6502 program into basic_rom via dbg_rom_we, verifies execution.
ROM_LOAD_RTL = $(VGC) $(SID) $(RTL)/blitter.sv $(RTL)/arlet_6502/cpu.v $(RTL)/arlet_6502/ALU.v $(RTL)/top.sv
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

clean:
	rm -rf $(BD)
