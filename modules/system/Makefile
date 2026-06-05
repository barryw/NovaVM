CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: system.bin system.nmod

system.o: system.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libsystem.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vgc.inc \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editui.s \
            $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editbuf.s \
            $(NOVA_ASM)/vgc.s
	$(CA65) system.s -o system.o

system.bin: system.o system.cfg
	$(LD65) -C system.cfg -m system.map -o system.bin system.o

# Self-documenting module: image + NDOC doc (from system.s ;@ comments, ids
# resolved from libsystem.inc) -> system.nmod.
system.nmod: system.bin system.s ../../tools/nmod_pack.py $(NOVA_ASM)/libsystem.inc
	python3 ../../tools/nmod_pack.py --src system.s --bin system.bin \
	        --out system.nmod --syms $(NOVA_ASM)/libsystem.inc

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: system.bin
	cp system.bin ../../e6502.Avalonia/Resources/system.bin

clean:
	rm -f system.o system.bin system.map system.nmod

.PHONY: all clean install
