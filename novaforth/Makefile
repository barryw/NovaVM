NOVA_ASM = ../runtime/asm
RESOURCES = ../e6502.Avalonia/Resources

all: novaforth.bin

novaforth.bin: novaforth.o novaforth.cfg
	ld65 -C novaforth.cfg -m novaforth.map -o $@ novaforth.o

novaforth.o: novaforth.s novaforth.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/libeditor.inc $(NOVA_ASM)/replline.s
	ca65 --cpu 65c02 -I $(NOVA_ASM) -l novaforth.lst -o $@ novaforth.s

install: novaforth.bin
	cp novaforth.bin $(RESOURCES)/novaforth.bin

clean:
	rm -f *.o *.bin *.lst *.map

.PHONY: clean all install
