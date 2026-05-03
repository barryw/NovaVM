# NovaHost build — stages SD-resident boot assets, then invokes arduino-cli to
# compile + upload. ROMs/assets are deliberately not embedded in the ESP image.
#
# Usage:
#   make           — stage assets, compile, upload via direct OTA
#   make compile   — stage assets, compile only
#   make serial    — stage assets, compile, upload via USB serial
#   make sd-assets — build SD card asset tree under build/sd/
#
# Env overrides:
#   NOVAHOST   — OTA target IP (default: 192.168.1.65)
#   OTA_PORT   — ArduinoOTA port (default: 3232)
#   ESPOTA     — path to espota.py; auto-detected from Arduino15 if unset
#   SERIAL     — USB serial port for fallback flashing
#   FQBN       — Arduino board FQBN; default keeps OTA and minimizes SPIFFS

NOVAHOST ?= 192.168.1.65
OTA_PORT ?= 3232
SERIAL   ?= /dev/cu.usbserial-D01457
FQBN     ?= esp32:esp32:lolin32:PartitionScheme=min_spiffs
OTA_BUILD ?= build/ota-direct
ESPOTA   ?= $(shell find "$$HOME/Library/Arduino15/packages/esp32/hardware/esp32" "$$HOME/.arduino15/packages/esp32/hardware/esp32" -path '*/tools/espota.py' -type f 2>/dev/null | sort | tail -n 1)

REPO_ROOT := $(abspath ../..)
EHBASIC   := $(REPO_ROOT)/ehbasic
FPGA_ROM  := $(REPO_ROOT)/e6502.FPGA/rom
HEX16TOBIN:= python3 $(REPO_ROOT)/tools/hex16_to_bin.py
SD_ASSETS := build/sd

.PHONY: all compile rom sd-assets serial upload upload-cli ota-direct clean ehbasic

all: upload

ehbasic:
	@$(MAKE) --no-print-directory -C $(EHBASIC)

# 6581 filter curve — 4096 × 16-bit entries. Streamed to SDRAM at boot so
# the FPGA's sid_curve_reader can fetch Fc→f0 lookups.
build/f6581_curve.bin: $(FPGA_ROM)/f6581_curve.hex
	@mkdir -p build
	@$(HEX16TOBIN) $< $@

sd-assets: ehbasic build/f6581_curve.bin
	@mkdir -p $(SD_ASSETS)/roms $(SD_ASSETS)/assets/sid $(SD_ASSETS)/assets/boot $(SD_ASSETS)/config
	@cp $(EHBASIC)/basic.bin $(SD_ASSETS)/roms/novabasic.bin
	@cp $(EHBASIC)/extension.bin $(SD_ASSETS)/roms/extension.bin
	@cp build/f6581_curve.bin $(SD_ASSETS)/assets/sid/f6581_curve.bin
	@cp assets/boot/novavm_logo.nvg $(SD_ASSETS)/assets/boot/novavm_logo.nvg
	@cp assets/config/boot.json $(SD_ASSETS)/config/boot.json
	@echo "SD assets staged under $(CURDIR)/$(SD_ASSETS)"
	@find $(SD_ASSETS) -type f -print

rom: sd-assets

compile: sd-assets
	arduino-cli compile --fqbn $(FQBN) .

upload: ota-direct

ota-direct: sd-assets
	@test -n "$(ESPOTA)" || (echo "error: espota.py not found; set ESPOTA=/path/to/espota.py" >&2; exit 1)
	arduino-cli compile --fqbn $(FQBN) --build-path $(OTA_BUILD) .
	python3 "$(ESPOTA)" -i "$(NOVAHOST)" -p "$(OTA_PORT)" -f "$(OTA_BUILD)/novahost.ino.bin"

upload-cli: sd-assets
	arduino-cli compile --fqbn $(FQBN) .
	arduino-cli upload --fqbn $(FQBN) --protocol network --port $(NOVAHOST) .

serial: sd-assets
	arduino-cli compile --fqbn $(FQBN) .
	arduino-cli upload --fqbn $(FQBN) --protocol serial --port $(SERIAL) .

clean:
	rm -rf build/
