NOVA_ASM = ../../runtime/asm
TOOLCHAIN = ../../toolchain
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

OBJS = novapascal.o novaasm.o novalink.o

all: novapascal.bin

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novapascal.bin: $(OBJS) novapascal.cfg $(NOVA_LIB_FILE)
	ld65 -C novapascal.cfg -m novapascal.map -o $@ $(OBJS) $(NOVA_LIB_FILE)

novapascal.o: novapascal.s novapascal.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libeditor.inc $(TOOLCHAIN)/novaasm.inc $(TOOLCHAIN)/novalink.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -I $(TOOLCHAIN) -l novapascal.lst -o $@ novapascal.s

novaasm.o: $(TOOLCHAIN)/novaasm.s $(TOOLCHAIN)/novaasm.inc $(TOOLCHAIN)/nobj.inc
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -l novaasm.lst -o $@ $(TOOLCHAIN)/novaasm.s

novalink.o: $(TOOLCHAIN)/novalink.s $(TOOLCHAIN)/novalink.inc $(TOOLCHAIN)/nobj.inc
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -l novalink.lst -o $@ $(TOOLCHAIN)/novalink.s

install: novapascal.bin
	cp novapascal.bin $(RESOURCES)/novapascal.bin

clean:
	rm -f *.o *.bin *.lst *.map AUTOBOOT.bin ndi-autoboot.o

FORCE:

.PHONY: clean all install FORCE

# Bootable language floppy: AUTOBOOT loads this runtime into the primary bank.
NDI_RUNTIME := novapascal.bin
NDI_OUTPUT  := novapascal.ndi
NDI_LABEL   := NPASCAL
NDI_ASSET_DEPS := hello.pas
NDI_ASSETS := --asset hello.pas:HELLO.PAS
include ../../runtime/ndi/ndi.mk
