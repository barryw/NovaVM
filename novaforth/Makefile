NOVA_ASM = ../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../e6502.Avalonia/Resources

all: novaforth.bin

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

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
