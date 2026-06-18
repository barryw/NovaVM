CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib

all: files.bin files.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

files.o: files.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libfiles.inc \
            $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s \
            $(NOVA_ASM)/pager.inc $(NOVA_ASM)/pager.s
	$(CA65) files.s -o files.o

files.bin: files.o files.cfg $(NOVA_LIB_FILE)
	$(LD65) -C files.cfg -m files.map -o files.bin files.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> files.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
files.nmod: files.bin files.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/fio.s $(NOVA_ASM)/pager.s
	python3 ../../tools/nmod_pack.py --src files.s --bin files.bin \
	        --out files.nmod --syms $(NOVA_ASM)/libfiles.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: files.bin
	cp files.bin ../../e6502.Avalonia/Resources/files.bin

clean:
	rm -f files.o files.bin files.map files.nmod

FORCE:

.PHONY: all clean install FORCE
