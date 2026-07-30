NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

all: novaforth.bin

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) build/nova.lib

novaforth.bin: novaforth.o novaforth.cfg $(NOVA_LIB_FILE)
	ld65 -C novaforth.cfg -m novaforth.map -o $@ novaforth.o $(NOVA_LIB_FILE)

novaforth.o: novaforth.s novaforth.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/libeditor.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -l novaforth.lst -o $@ novaforth.s

install: novaforth.bin
	cp novaforth.bin $(RESOURCES)/novaforth.bin

clean:
	rm -f *.o *.bin *.lst *.map

FORCE:

.PHONY: clean all install FORCE

# --- bootable language floppy: novaforth.ndi (autoboot -> LOADRUNTIME -> $C000) ---
NDI_RUNTIME := novaforth.bin
NDI_OUTPUT  := novaforth.ndi
NDI_LABEL   := NFORTH
# Bake the forth library tree into the image: novaforth INCLUDEs forth/lib/core.4th
# + forth/autoexec.4th (and their includes) at boot, so the libs must travel with
# the disk. The packer turns each path into a real .ndi subdirectory (forth/lib/...).
# demos.4th is a top-level show-off file: INCLUDE "demos.4th" then TUNNEL / etc.
NDI_FORTH_FILES := forth/autoexec.4th demos.4th musicviz.4th $(shell find forth/lib -name '*.4th' 2>/dev/null)
# Data a packed program needs travels in the same NDI (musicviz.4th plays this MIDI
# via SND-MIDPLAY-FILE, which now reads it from the mounted image, not a host dir).
NDI_DATA_FILES  := mountainking.mid
NDI_ASSETS      := $(foreach f,$(NDI_FORTH_FILES),--asset $(f):$(f)) $(foreach f,$(NDI_DATA_FILES),--asset $(f):$(f))
NDI_ASSET_DEPS  := $(NDI_FORTH_FILES) $(NDI_DATA_FILES)
include ../../runtime/ndi/ndi.mk
