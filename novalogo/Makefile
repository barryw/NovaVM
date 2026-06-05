NOVA_ASM = ../runtime/asm
RESOURCES = ../e6502.Avalonia/Resources

# NovaLogo is a single 16K ROM + shared modules. The extension ROM was deleted in
# Phase B (its handlers moved to the SYSTEM/SOUND modules).
all: novalogo.bin

novalogo.bin: novalogo.o novalogo.cfg
	ld65 -C novalogo.cfg -m novalogo.map -o $@ novalogo.o

novalogo.o: novalogo.s heap.s gc.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s ext_iface.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -l novalogo.lst -o $@ novalogo.s

install: novalogo.bin
	cp novalogo.bin $(RESOURCES)/novalogo.bin

clean:
	rm -f *.o *.bin *.lst *.map

.PHONY: clean all install
