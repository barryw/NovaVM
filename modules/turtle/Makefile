CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: turtle.bin turtle.nmod

turtle.o: turtle.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libturtle.inc \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/sprite.inc $(NOVA_ASM)/sprite.s \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/copper_split.inc $(NOVA_ASM)/copper_split.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
            $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s
	$(CA65) turtle.s -o turtle.o

turtle.bin: turtle.o turtle.cfg
	$(LD65) -C turtle.cfg -m turtle.map -o turtle.bin turtle.o

# Self-documenting module: image + NDOC doc -> turtle.nmod. --ndk-dir keeps the
# drift guard available (the turtle has no ;@ndk mappings, so nothing to check —
# it owns its own move-math + render rather than wrapping NDK leaf routines).
turtle.nmod: turtle.bin turtle.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libturtle.inc
	python3 ../../tools/nmod_pack.py --src turtle.s --bin turtle.bin \
	        --out turtle.nmod --syms $(NOVA_ASM)/libturtle.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice demand-loads it).
install: turtle.bin
	cp turtle.bin ../../e6502.Avalonia/Resources/turtle.bin

clean:
	rm -f turtle.o turtle.bin turtle.map turtle.nmod

.PHONY: all clean install
