# NovaVM top-level build.
#
# Produces every *deployable* binary from source so a deploy is always a matched
# set (no stale/mismatched artifacts) and the binaries need not live in git.
#
#   make binaries       build all ROMs/modules/apps from source + stage them into
#                       the Avalonia/CLI Resources, the FPGA rom dir, and the web copies
#   make clean-binaries remove the generated binaries (git-tracked fixtures are kept)
#
# Dependency order: software/runtime/asm (nova.lib) -> ehbasic (generates novavm.inc) ->
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
	$(MAKE) -C software/runtime/asm install          # nova.lib + libcall.bin -> Resources
	$(MAKE) -C software/languages/ehbasic            # basic.bin, extension.bin, novavm.inc
	$(MAKE) -C software/languages/novaforth install
	$(MAKE) -C software/languages/novalogo install
	@for m in $(MODULES); do $(MAKE) -C software/modules/$$m install || exit 1; done
	$(MAKE) -C software/assembly                       # apps (keyboard, demo, turtle, ...)
	python3 tools/make_petscii_font.py $(AVRES)
	$(MAKE) stage-resources
	$(MAKE) stage-web
	@echo "=== binaries: all deployable artifacts built + staged from source ==="

# ehbasic has no install target; stage it (renamed) + the FPGA/CLI copies here.
stage-resources:
	cp software/languages/ehbasic/basic.bin     $(AVRES)/ehbasic.bin
	cp software/languages/ehbasic/extension.bin $(AVRES)/extension.bin
	cp software/languages/ehbasic/basic.bin     $(CLIRES)/ehbasic.bin
	cp software/languages/ehbasic/extension.bin $(CLIRES)/extension.bin
	cp software/languages/ehbasic/basic.bin     $(FPGAROM)/ehbasic.bin
	cp software/languages/ehbasic/extension.bin $(FPGAROM)/extension.bin

# Mirror the built Avalonia Resources .bin into the static-site emulator copies.
stage-web:
	@for d in $(WEBDIRS); do mkdir -p $$d; \
	  for f in $(WEBBINS); do cp $(AVRES)/$$f.bin $$d/$$f.bin; done; done

clean-binaries:
	$(MAKE) -C software/runtime/asm clean || true
	$(MAKE) -C software/languages/ehbasic clean || true
	$(MAKE) -C software/languages/novaforth clean || true
	$(MAKE) -C software/languages/novalogo clean || true
	@for m in $(MODULES); do $(MAKE) -C software/modules/$$m clean || true; done
	$(MAKE) -C software/assembly clean || true
