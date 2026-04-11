PROJ     = e6502_sim
RTL      = rtl/top.sv rtl/vgc.sv rtl/tile_engine.sv rtl/blitter.sv \
           rtl/sid/sid_chip.sv rtl/sid/sid_voice.sv rtl/sid/sid_envelope.sv \
           rtl/sid/sid_filter.sv rtl/sid/sid_tables.sv rtl/sid/sid_dac.sv \
           rtl/arlet_6502/cpu.v rtl/arlet_6502/ALU.v
SIM_SRC  = sim/sim_main.cpp
ROM_BIN  = rom/ehbasic.bin
ROM_HEX  = rom/ehbasic.hex
EXT_BIN  = rom/extension.bin
EXT_HEX  = rom/extension.hex
OBJ_DIR  = obj_dir

VERILATOR = verilator
VFLAGS    = --cc --exe --build -Wall \
            -Wno-WIDTHTRUNC -Wno-WIDTHEXPAND \
            -Wno-UNUSEDSIGNAL -Wno-UNUSEDPARAM -Wno-CASEINCOMPLETE \
            -Wno-PROCASSINIT -Wno-CASEX -Wno-SYNCASYNCNET -Wno-UNOPTFLAT -Wno-LATCH \
            -Wno-PINCONNECTEMPTY -Wno-IMPLICITSTATIC -Wno-SELRANGE \
            --top-module top \
            -CFLAGS "-O2 -DSIM $(shell sdl2-config --cflags)" \
            -LDFLAGS "$(shell sdl2-config --libs)" \
            --Mdir $(OBJ_DIR) -o ../$(PROJ)

.PHONY: all run clean hex

all: $(PROJ)

# Convert binary ROM to hex format for $readmemh
$(ROM_HEX): $(ROM_BIN)
	@echo "Converting ROM to hex..."
	@python3 -c "\
import sys; \
data = open('$(ROM_BIN)','rb').read(); \
f = open('$(ROM_HEX)','w'); \
[f.write('%02X\n' % b) for b in data]; \
f.close(); \
print(f'ROM: {len(data)} bytes converted')"

$(EXT_HEX): $(EXT_BIN)
	@echo "Converting extension ROM to hex..."
	@python3 -c "\
import sys; \
data = open('$(EXT_BIN)','rb').read(); \
f = open('$(EXT_HEX)','w'); \
[f.write('%02X\n' % b) for b in data]; \
f.close(); \
print(f'Extension ROM: {len(data)} bytes converted')"

hex: $(ROM_HEX) $(EXT_HEX)

# Build simulation executable
$(PROJ): $(RTL) $(SIM_SRC) $(ROM_HEX) $(EXT_HEX)
	$(VERILATOR) $(VFLAGS) $(RTL) $(SIM_SRC)

# Run the simulation
run: $(PROJ)
	@./$(PROJ)

clean:
	rm -rf $(OBJ_DIR) $(PROJ) $(ROM_HEX) $(EXT_HEX)
