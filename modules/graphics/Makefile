CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: graphics.bin

graphics.o: graphics.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libgraphics.inc \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/sprite.inc $(NOVA_ASM)/sprite.s
	$(CA65) graphics.s -o graphics.o

graphics.bin: graphics.o graphics.cfg
	$(LD65) -C graphics.cfg -m graphics.map -o graphics.bin graphics.o

clean:
	rm -f graphics.o graphics.bin graphics.map

.PHONY: all clean
