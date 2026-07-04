NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

# NovaLogo is a single 16K ROM + shared modules. The extension ROM was deleted in
# Phase B (its handlers moved to the SYSTEM/SOUND modules).
all: novalogo.bin LOGOHK.BIN

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novalogo.bin: novalogo.o novalogo.cfg $(NOVA_LIB_FILE)
	ld65 -C novalogo.cfg -m novalogo.map -o $@ novalogo.o $(NOVA_LIB_FILE)

novalogo.o: novalogo.s heap.s gc.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s ext_iface.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -l novalogo.lst -o $@ novalogo.s

novalogo_hooks.o: novalogo_hooks.s novalogo_hooks.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libeditor.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -l novalogo_hooks.lst -o $@ novalogo_hooks.s

LOGOHK.BIN: novalogo_hooks.o novalogo_hooks.cfg
	ld65 -C novalogo_hooks.cfg -m novalogo_hooks.map -o $@ novalogo_hooks.o

install: novalogo.bin LOGOHK.BIN
	cp novalogo.bin $(RESOURCES)/novalogo.bin
	cp LOGOHK.BIN $(RESOURCES)/LOGOHK.BIN

clean:
	rm -f *.o *.bin *.BIN *.lst *.map

FORCE:

.PHONY: clean all install FORCE

# --- bootable language floppy: novalogo.ndi (autoboot -> LOADRUNTIME -> $C000) ---
NDI_RUNTIME := novalogo.bin
NDI_OUTPUT  := novalogo.ndi
NDI_LABEL   := NLOGO
NDI_ASSET_DEPS := LOGOHK.BIN
NDI_ASSETS := --asset LOGOHK.BIN:LOGOHK.BIN
include ../../runtime/ndi/ndi.mk
