CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib

all: net.bin net.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

net.o: net.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libnet.inc \
            $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/nic.inc $(NOVA_ASM)/nic.s \
            $(NOVA_ASM)/gameserver.inc $(NOVA_ASM)/gameserver.s
	$(CA65) net.s -o net.o

net.bin: net.o net.cfg $(NOVA_LIB_FILE)
	$(LD65) -C net.cfg -m net.map -o net.bin net.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> net.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
net.nmod: net.bin net.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libnet.inc $(NOVA_ASM)/nic.s $(NOVA_ASM)/gameserver.s
	python3 ../../tools/nmod_pack.py --src net.s --bin net.bin \
	        --out net.nmod --syms $(NOVA_ASM)/libnet.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: net.bin
	cp net.bin ../../e6502.Avalonia/Resources/net.bin

clean:
	rm -f net.o net.bin net.map net.nmod

FORCE:

.PHONY: all clean install FORCE
