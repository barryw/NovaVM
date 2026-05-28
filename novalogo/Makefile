NOVA_ASM = ../runtime/asm

novalogo.bin: novalogo.o novalogo.cfg
	ld65 -C novalogo.cfg -o $@ novalogo.o

novalogo.o: novalogo.s heap.s tokens.s tokenizer.s eval.s builtins.s lists.s vars.s procedures.s $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -o $@ novalogo.s

clean:
	rm -f *.o *.bin

.PHONY: clean
