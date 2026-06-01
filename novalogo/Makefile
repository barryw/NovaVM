NOVA_ASM = ../runtime/asm
RESOURCES = ../e6502.Avalonia/Resources

all: novalogo.bin novalogo_ext.bin

novalogo.bin: novalogo.o novalogo.cfg
	ld65 -C novalogo.cfg -m novalogo.map -o $@ novalogo.o

novalogo.o: novalogo.s heap.s gc.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s ext_iface.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -l novalogo.lst -o $@ novalogo.s

novalogo_ext.bin: extension.o extension.cfg
	ld65 -C extension.cfg -m extension.map -o $@ extension.o

extension.o: extension.s ext_iface.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s $(NOVA_ASM)/copper_split.inc $(NOVA_ASM)/copper_split.s $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editui.s $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editbuf.s $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s
	ca65 --cpu 65c02 -I $(NOVA_ASM) -l extension.lst -o $@ extension.s

install: novalogo.bin novalogo_ext.bin
	cp novalogo.bin $(RESOURCES)/novalogo.bin
	cp novalogo_ext.bin $(RESOURCES)/novalogo_ext.bin

clean:
	rm -f *.o *.bin *.lst *.map

.PHONY: clean all install
