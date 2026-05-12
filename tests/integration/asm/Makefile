CA65 ?= ca65
LD65 ?= ld65
BUILD_DIR ?= /tmp/e6502-asm-fixtures
FIXTURE_DIR := ../fixtures
NOVA_LIB := ../../../ehbasic/lib

all: $(FIXTURE_DIR)/xram_runtime.bin $(FIXTURE_DIR)/vtext_runtime.bin $(FIXTURE_DIR)/audio_runtime.bin $(FIXTURE_DIR)/nic_runtime.bin $(FIXTURE_DIR)/gameserver_runtime.bin $(FIXTURE_DIR)/gameserver_live_runtime.bin $(FIXTURE_DIR)/overlay_runtime.bin

$(FIXTURE_DIR)/xram_runtime.bin: xram_runtime.s xram_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/dma.inc $(NOVA_LIB)/dma.s $(NOVA_LIB)/xram.s $(NOVA_LIB)/xram.inc | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/xram_runtime.o xram_runtime.s
	$(LD65) -C xram_runtime.cfg -m $(BUILD_DIR)/xram_runtime.map -Ln $(BUILD_DIR)/xram_runtime.sym -o $@ $(BUILD_DIR)/xram_runtime.o

$(FIXTURE_DIR)/vtext_runtime.bin: vtext_runtime.s vtext_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/blitter.s $(NOVA_LIB)/vtext.s $(NOVA_LIB)/vtext.inc | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/vtext_runtime.o vtext_runtime.s
	$(LD65) -C vtext_runtime.cfg -m $(BUILD_DIR)/vtext_runtime.map -Ln $(BUILD_DIR)/vtext_runtime.sym -o $@ $(BUILD_DIR)/vtext_runtime.o

$(FIXTURE_DIR)/audio_runtime.bin: audio_runtime.s audio_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/audio.inc $(NOVA_LIB)/audio.s | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/audio_runtime.o audio_runtime.s
	$(LD65) -C audio_runtime.cfg -m $(BUILD_DIR)/audio_runtime.map -Ln $(BUILD_DIR)/audio_runtime.sym -o $@ $(BUILD_DIR)/audio_runtime.o

$(FIXTURE_DIR)/nic_runtime.bin: nic_runtime.s nic_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/nic.inc $(NOVA_LIB)/nic.s | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/nic_runtime.o nic_runtime.s
	$(LD65) -C nic_runtime.cfg -m $(BUILD_DIR)/nic_runtime.map -Ln $(BUILD_DIR)/nic_runtime.sym -o $@ $(BUILD_DIR)/nic_runtime.o

$(FIXTURE_DIR)/gameserver_runtime.bin: gameserver_runtime.s gameserver_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/nic.inc $(NOVA_LIB)/nic.s $(NOVA_LIB)/gameserver.inc $(NOVA_LIB)/gameserver.s | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/gameserver_runtime.o gameserver_runtime.s
	$(LD65) -C gameserver_runtime.cfg -m $(BUILD_DIR)/gameserver_runtime.map -Ln $(BUILD_DIR)/gameserver_runtime.sym -o $@ $(BUILD_DIR)/gameserver_runtime.o

$(FIXTURE_DIR)/gameserver_live_runtime.bin: gameserver_live_runtime.s gameserver_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/nic.inc $(NOVA_LIB)/nic.s $(NOVA_LIB)/gameserver.inc $(NOVA_LIB)/gameserver.s | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/gameserver_live_runtime.o gameserver_live_runtime.s
	$(LD65) -C gameserver_runtime.cfg -m $(BUILD_DIR)/gameserver_live_runtime.map -Ln $(BUILD_DIR)/gameserver_live_runtime.sym -o $@ $(BUILD_DIR)/gameserver_live_runtime.o

$(FIXTURE_DIR)/overlay_runtime.bin: overlay_runtime.s gameserver_runtime.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/pager.inc $(NOVA_LIB)/pager.s $(NOVA_LIB)/overlay.inc $(NOVA_LIB)/overlay.s | $(BUILD_DIR) $(FIXTURE_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/overlay_runtime.o overlay_runtime.s
	$(LD65) -C gameserver_runtime.cfg -m $(BUILD_DIR)/overlay_runtime.map -Ln $(BUILD_DIR)/overlay_runtime.sym -o $@ $(BUILD_DIR)/overlay_runtime.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(FIXTURE_DIR):
	mkdir -p $(FIXTURE_DIR)

.PHONY: all
