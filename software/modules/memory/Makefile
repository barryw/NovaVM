CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib

all: memory.bin memory.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

memory.o: memory.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libmemory.inc \
            $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/xram.inc $(NOVA_ASM)/xram.s \
            $(NOVA_ASM)/xmc.s \
            $(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s
	$(CA65) memory.s -o memory.o

memory.bin: memory.o memory.cfg $(NOVA_LIB_FILE)
	$(LD65) -C memory.cfg -m memory.map -o memory.bin memory.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> memory.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
memory.nmod: memory.bin memory.s ../../../tools/nmod_pack.py ../../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/xram.s $(NOVA_ASM)/xmc.s
	python3 ../../../tools/nmod_pack.py --src memory.s --bin memory.bin \
	        --out memory.nmod --syms $(NOVA_ASM)/libmemory.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: memory.bin
	cp memory.bin ../../../e6502.Avalonia/Resources/memory.bin

clean:
	rm -f memory.o memory.bin memory.map memory.nmod

FORCE:

.PHONY: all clean install FORCE
