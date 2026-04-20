# NovaHost build — regenerates embedded ROM headers from ehbasic source,
# then invokes arduino-cli to compile + upload.
#
# Usage:
#   make          — regen headers, compile, upload via OTA (novahost.local)
#   make compile  — regen headers, compile only
#   make serial   — regen headers, compile, upload via USB serial
#   make rom      — regen headers only (useful after editing ehbasic/basic.s)
#
# Env overrides:
#   NOVAHOST   — OTA hostname (default: novahost.local)
#   SERIAL     — USB serial port for fallback flashing
#   FQBN       — Arduino board FQBN (default: esp32:esp32:lolin32)

NOVAHOST ?= novahost.local
SERIAL   ?= /dev/cu.usbserial-D01457
FQBN     ?= esp32:esp32:lolin32

REPO_ROOT := $(abspath ../..)
EHBASIC   := $(REPO_ROOT)/ehbasic
BIN2HDR   := python3 $(REPO_ROOT)/tools/bin2header.py

# ROM headers — regenerated every build so they can never go stale relative
# to ehbasic source. Depend on the built .bin files; ehbasic's own Makefile
# handles asm→bin.
.PHONY: all compile rom serial upload clean ehbasic

all: upload

ehbasic:
	@$(MAKE) --no-print-directory -C $(EHBASIC)

ehbasic_rom.h: ehbasic
	@$(BIN2HDR) $(EHBASIC)/basic.bin ehbasic_rom.h EHBASIC_ROM

extension_rom.h: ehbasic
	@$(BIN2HDR) $(EHBASIC)/extension.bin extension_rom.h EXTENSION_ROM

rom: ehbasic_rom.h extension_rom.h

compile: rom
	arduino-cli compile --fqbn $(FQBN) .

upload: rom
	arduino-cli compile --fqbn $(FQBN) --upload --port $(NOVAHOST) .

serial: rom
	arduino-cli compile --fqbn $(FQBN) --upload --port $(SERIAL) .

clean:
	rm -rf build/
