NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

# NovaLogo is a single 16K ROM + shared modules. The extension ROM was deleted in
# Phase B (its handlers moved to the SYSTEM/SOUND modules).
all: novalogo.bin

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novalogo.bin: novalogo.o novalogo.cfg $(NOVA_LIB_FILE)
	ld65 -C novalogo.cfg -m novalogo.map -o $@ novalogo.o $(NOVA_LIB_FILE)

novalogo.o: novalogo.s heap.s gc.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s ext_iface.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -l novalogo.lst -o $@ novalogo.s

install: novalogo.bin
	cp novalogo.bin $(RESOURCES)/novalogo.bin

clean:
	rm -f *.o *.bin *.lst *.map

FORCE:

.PHONY: clean all install FORCE

# --- bootable language floppy: novalogo.ndi (autoboot -> LOADRUNTIME -> $C000) ---
NDI_RUNTIME := novalogo.bin
NDI_OUTPUT  := novalogo.ndi
NDI_LABEL   := NLOGO
include ../../runtime/ndi/ndi.mk
