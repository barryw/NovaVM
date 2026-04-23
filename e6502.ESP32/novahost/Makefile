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
FPGA_ROM  := $(REPO_ROOT)/e6502.FPGA/rom
BIN2HDR   := python3 $(REPO_ROOT)/tools/bin2header.py
HEX16TOBIN:= python3 $(REPO_ROOT)/tools/hex16_to_bin.py

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

# 6581 filter curve — 4096 × 16-bit entries. Streamed to SDRAM at boot so
# the FPGA's sid_curve_reader can fetch Fc→f0 lookups.
build/f6581_curve.bin: $(FPGA_ROM)/f6581_curve.hex
	@mkdir -p build
	@$(HEX16TOBIN) $< $@

sid_curve_rom.h: build/f6581_curve.bin
	@$(BIN2HDR) $< sid_curve_rom.h SID_CURVE_ROM

rom: ehbasic_rom.h extension_rom.h sid_curve_rom.h

compile: rom
	arduino-cli compile --fqbn $(FQBN) .

upload: rom
	arduino-cli compile --fqbn $(FQBN) --upload --port $(NOVAHOST) .

serial: rom
	arduino-cli compile --fqbn $(FQBN) --upload --port $(SERIAL) .

clean:
	rm -rf build/
