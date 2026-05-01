CA65 ?= ca65
LD65 ?= ld65
BUILD_DIR ?= /tmp/e6502-asm-fixtures
FIXTURE_DIR := ../fixtures

all: $(FIXTURE_DIR)/xram_runtime.bin $(FIXTURE_DIR)/vtext_runtime.bin

$(FIXTURE_DIR)/xram_runtime.bin: xram_runtime.s xram_runtime.cfg ../../../ehbasic/lib/nova.inc ../../../ehbasic/lib/fio.inc ../../../ehbasic/lib/fio.s ../../../ehbasic/lib/dma.inc ../../../ehbasic/lib/dma.s ../../../ehbasic/lib/xram.s ../../../ehbasic/lib/xram.inc | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I ../../../ehbasic/lib -I ../../../ehbasic -o $(BUILD_DIR)/xram_runtime.o xram_runtime.s
	$(LD65) -C xram_runtime.cfg -m $(BUILD_DIR)/xram_runtime.map -Ln $(BUILD_DIR)/xram_runtime.sym -o $@ $(BUILD_DIR)/xram_runtime.o

$(FIXTURE_DIR)/vtext_runtime.bin: vtext_runtime.s vtext_runtime.cfg ../../../ehbasic/lib/nova.inc ../../../ehbasic/lib/blitter.inc ../../../ehbasic/lib/blitter.s ../../../ehbasic/lib/vtext.s ../../../ehbasic/lib/vtext.inc | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I ../../../ehbasic -o $(BUILD_DIR)/vtext_runtime.o vtext_runtime.s
	$(LD65) -C vtext_runtime.cfg -m $(BUILD_DIR)/vtext_runtime.map -Ln $(BUILD_DIR)/vtext_runtime.sym -o $@ $(BUILD_DIR)/vtext_runtime.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(FIXTURE_DIR):
	mkdir -p $(FIXTURE_DIR)

.PHONY: all
