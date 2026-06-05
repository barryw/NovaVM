CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: graphics.bin graphics.nmod

graphics.o: graphics.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libgraphics.inc \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/sprite.inc $(NOVA_ASM)/sprite.s \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/copper_split.inc $(NOVA_ASM)/copper_split.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
            $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s \
            $(NOVA_ASM)/msprite.inc $(NOVA_ASM)/msprite.s \
            $(NOVA_ASM)/nvg.inc $(NOVA_ASM)/nvg.s \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s \
            $(NOVA_ASM)/anim.inc $(NOVA_ASM)/anim.s \
            $(NOVA_ASM)/tween.inc $(NOVA_ASM)/tween.s \
            $(NOVA_ASM)/pager.inc $(NOVA_ASM)/pager.s
	$(CA65) graphics.s -o graphics.o

graphics.bin: graphics.o graphics.cfg
	$(LD65) -C graphics.cfg -m graphics.map -o graphics.bin graphics.o

# Self-documenting module: image + NDOC doc (from graphics.s ;@ comments, ids
# resolved from libgraphics.inc) -> graphics.nmod.
graphics.nmod: graphics.bin graphics.s ../../tools/nmod_pack.py $(NOVA_ASM)/libgraphics.inc
	python3 ../../tools/nmod_pack.py --src graphics.s --bin graphics.bin \
	        --out graphics.nmod --syms $(NOVA_ASM)/libgraphics.inc

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: graphics.bin
	cp graphics.bin ../../e6502.Avalonia/Resources/graphics.bin

clean:
	rm -f graphics.o graphics.bin graphics.map graphics.nmod

.PHONY: all clean install
