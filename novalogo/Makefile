NOVA_ASM = ../runtime/asm
RESOURCES = ../e6502.Avalonia/Resources

all: novalogo.bin novalogo_ext.bin

novalogo.bin: novalogo.o novalogo.cfg
	ld65 -C novalogo.cfg -m novalogo.map -o $@ novalogo.o

novalogo.o: novalogo.s heap.s gc.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s ext_iface.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -o $@ novalogo.s

novalogo_ext.bin: extension.o extension.cfg
	ld65 -C extension.cfg -o $@ extension.o

extension.o: extension.s ext_iface.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -o $@ extension.s

install: novalogo.bin novalogo_ext.bin
	cp novalogo.bin $(RESOURCES)/novalogo.bin
	cp novalogo_ext.bin $(RESOURCES)/novalogo_ext.bin

clean:
	rm -f *.o *.bin

.PHONY: clean all install
