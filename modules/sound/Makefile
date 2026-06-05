CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: sound.bin sound.nmod

sound.o: sound.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libsound.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/audio.inc $(NOVA_ASM)/audio.s \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s
	$(CA65) sound.s -o sound.o

sound.bin: sound.o sound.cfg
	$(LD65) -C sound.cfg -m sound.map -o sound.bin sound.o

# Self-documenting module: image + NDOC doc -> sound.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
sound.nmod: sound.bin sound.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libsound.inc $(NOVA_ASM)/audio.s $(NOVA_ASM)/vgc.s
	python3 ../../tools/nmod_pack.py --src sound.s --bin sound.bin \
	        --out sound.nmod --syms $(NOVA_ASM)/libsound.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: sound.bin
	cp sound.bin ../../e6502.Avalonia/Resources/sound.bin

clean:
	rm -f sound.o sound.bin sound.map sound.nmod

.PHONY: all clean install
