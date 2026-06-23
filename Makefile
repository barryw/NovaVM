# NovaVM top-level build.
#
# Produces every *deployable* binary from source so a deploy is always a matched
# set (no stale/mismatched artifacts) and the binaries need not live in git.
#
#   make binaries       build all ROMs/modules/apps from source + stage them into
#                       the Avalonia/CLI Resources, the FPGA rom dir, and the web copies
#   make clean-binaries remove the generated binaries (git-tracked fixtures are kept)
#
# Dependency order: runtime/asm (nova.lib) -> ehbasic (generates novavm.inc) ->
# everything else. Toolchain: cc65 (ca65/ld65), python3.

AVRES   := e6502.Avalonia/Resources
CLIRES  := e6502.CLI/Resources
FPGAROM := e6502.FPGA/rom
WEBDIRS := e6502.Browser/wwwroot/rust website/emulator/rust
MODULES := editor files graphics memory net sound system turtle
# Resource .bin set mirrored to the web copies (cp437 is a tracked font fixture).
WEBBINS := cp437 ehbasic extension novaforth novalogo libcall $(MODULES)

.PHONY: binaries clean-binaries stage-resources stage-web

binaries:
	$(MAKE) -C runtime/asm install          # nova.lib + libcall.bin -> Resources
	$(MAKE) -C ehbasic                       # basic.bin, extension.bin, novavm.inc
	$(MAKE) -C novaforth install
	$(MAKE) -C novalogo install
	@for m in $(MODULES); do $(MAKE) -C modules/$$m install || exit 1; done
	$(MAKE) -C assembly                       # apps (keyboard, demo, turtle, ...)
	python3 tools/make_petscii_font.py $(AVRES)
	$(MAKE) stage-resources
	$(MAKE) stage-web
	@echo "=== binaries: all deployable artifacts built + staged from source ==="

# ehbasic has no install target; stage it (renamed) + the FPGA/CLI copies here.
stage-resources:
	cp ehbasic/basic.bin     $(AVRES)/ehbasic.bin
	cp ehbasic/extension.bin $(AVRES)/extension.bin
	cp ehbasic/basic.bin     $(CLIRES)/ehbasic.bin
	cp ehbasic/extension.bin $(CLIRES)/extension.bin
	cp ehbasic/basic.bin     $(FPGAROM)/ehbasic.bin
	cp ehbasic/extension.bin $(FPGAROM)/extension.bin

# Mirror the built Avalonia Resources .bin into the static-site emulator copies.
stage-web:
	@for d in $(WEBDIRS); do mkdir -p $$d; \
	  for f in $(WEBBINS); do cp $(AVRES)/$$f.bin $$d/$$f.bin; done; done

clean-binaries:
	$(MAKE) -C runtime/asm clean || true
	$(MAKE) -C ehbasic clean || true
	$(MAKE) -C novaforth clean || true
	$(MAKE) -C novalogo clean || true
	@for m in $(MODULES); do $(MAKE) -C modules/$$m clean || true; done
	$(MAKE) -C assembly clean || true
