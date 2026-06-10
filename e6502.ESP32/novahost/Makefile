# NovaHost build — stages SD-resident boot assets, then invokes arduino-cli to
# compile + upload. ROMs/assets are deliberately not embedded in the ESP image.
#
# Usage:
#   make           — stage assets, compile, upload via direct OTA
#   make compile   — stage assets, compile only
#   make compact   — stage assets, compile serial-flashable firmware without OTA
#   make serial    — stage assets, compile, upload via USB serial
#   make sd-assets — build SD card asset tree under build/sd/
#
# Env overrides:
#   NOVAHOST   — OTA target IP (default: 192.168.1.65)
#   OTA_PORT   — ArduinoOTA port (default: 3232)
#   ESPOTA     — path to espota.py; auto-detected from Arduino15 if unset
#   SERIAL     — USB serial port for fallback flashing
#   FQBN       — Arduino board FQBN; default keeps OTA and minimizes SPIFFS
#   BUILD_FLAGS — extra Arduino build flags, for diagnostics/tuning

NOVAHOST ?= 192.168.1.65
OTA_PORT ?= 3232
SERIAL   ?= /dev/cu.usbserial-D01457
FQBN     ?= esp32:esp32:lolin32:PartitionScheme=min_spiffs
OTA_BUILD ?= build/ota-direct
BUILD_FLAGS ?=
ESPOTA   ?= $(shell find "$$HOME/Library/Arduino15/packages/esp32/hardware/esp32" "$$HOME/.arduino15/packages/esp32/hardware/esp32" -path '*/tools/espota.py' -type f 2>/dev/null | sort | tail -n 1)

REPO_ROOT := $(abspath ../..)
EHBASIC   := $(REPO_ROOT)/ehbasic
NOVALOGO  := $(REPO_ROOT)/novalogo
NOVAFORTH := $(REPO_ROOT)/novaforth
MODGFX    := $(REPO_ROOT)/modules/graphics
RTASM     := $(REPO_ROOT)/runtime/asm
FPGA_ROM  := $(REPO_ROOT)/e6502.FPGA/rom
HEX16TOBIN:= python3 $(REPO_ROOT)/tools/hex16_to_bin.py
SD_ASSETS := build/sd

COMPACT_BUILD ?= build/compact

.PHONY: all compile compact rom sd-assets serial upload upload-cli ota-direct clean ehbasic novalogo novaforth graphics libcall

all: upload

ehbasic:
	@$(MAKE) --no-print-directory -C $(EHBASIC)

novalogo:
	@$(MAKE) --no-print-directory -C $(NOVALOGO)

novaforth:
	@$(MAKE) --no-print-directory -C $(NOVAFORTH)

graphics:
	@$(MAKE) --no-print-directory -C $(MODGFX)

# Resident lib_call loader (157 B, ORG $0320). NovaHost pokes it into RAM $0320
# at boot so module lib_calls work (see stageResidentLoader in novahost.ino).
libcall:
	@$(MAKE) --no-print-directory -C $(RTASM)

# 6581 filter curve — 4096 × 16-bit entries. Streamed to SDRAM at boot so
# the FPGA's sid_curve_reader can fetch Fc→f0 lookups.
build/f6581_curve.bin: $(FPGA_ROM)/f6581_curve.hex
	@mkdir -p build
	@$(HEX16TOBIN) $< $@

sd-assets: ehbasic novalogo novaforth graphics libcall build/f6581_curve.bin
	@mkdir -p $(SD_ASSETS)/roms $(SD_ASSETS)/assets/sid $(SD_ASSETS)/assets/boot $(SD_ASSETS)/config $(SD_ASSETS)/lib
	@cp $(EHBASIC)/basic.bin $(SD_ASSETS)/roms/novabasic.bin
	@cp $(EHBASIC)/extension.bin $(SD_ASSETS)/roms/extension.bin
	@cp $(NOVALOGO)/novalogo.bin $(SD_ASSETS)/roms/novalogo.bin
	@cp $(NOVAFORTH)/novaforth.bin $(SD_ASSETS)/roms/novaforth.bin
	@cp build/f6581_curve.bin $(SD_ASSETS)/assets/sid/f6581_curve.bin
	@cp assets/boot/novavm_logo.nvg $(SD_ASSETS)/assets/boot/novavm_logo.nvg
	@cp assets/config/boot.json $(SD_ASSETS)/config/boot.json
	@cp $(REPO_ROOT)/tests/asm/testmod.bin $(SD_ASSETS)/lib/testmod.mod
	@cp $(MODGFX)/graphics.nmod $(SD_ASSETS)/lib/graphics.nmod
	@cp $(RTASM)/libcall.bin $(SD_ASSETS)/roms/libcall.bin
	@rm -f $(SD_ASSETS)/soundfonts/GeneralUser_GS.nsfb
	@echo "SD assets staged under $(CURDIR)/$(SD_ASSETS)"
	@find $(SD_ASSETS) -type f -print

rom: sd-assets

compile: sd-assets
	arduino-cli compile --fqbn $(FQBN) --build-property build.extra_flags="$(BUILD_FLAGS)" .

compact: sd-assets
	arduino-cli compile --fqbn $(FQBN) --build-path $(COMPACT_BUILD) --build-property build.extra_flags="-DNOVA_ENABLE_OTA=0" .

upload: ota-direct

ota-direct: sd-assets
	@test -n "$(ESPOTA)" || (echo "error: espota.py not found; set ESPOTA=/path/to/espota.py" >&2; exit 1)
	arduino-cli compile --fqbn $(FQBN) --build-path $(OTA_BUILD) --build-property build.extra_flags="$(BUILD_FLAGS)" .
	python3 "$(ESPOTA)" -i "$(NOVAHOST)" -p "$(OTA_PORT)" -f "$(OTA_BUILD)/novahost.ino.bin"

upload-cli: sd-assets
	arduino-cli compile --fqbn $(FQBN) .
	arduino-cli upload --fqbn $(FQBN) --protocol network --port $(NOVAHOST) .

serial: sd-assets
	arduino-cli compile --fqbn $(FQBN) --build-property build.extra_flags="$(BUILD_FLAGS)" .
	arduino-cli upload --fqbn $(FQBN) --protocol serial --port $(SERIAL) .

clean:
	rm -rf build/
