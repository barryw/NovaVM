CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: system.bin system.nmod

system.o: system.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libsystem.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vgc.inc \
            $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/vgc.s \
            $(NOVA_ASM)/rng.inc $(NOVA_ASM)/rng.s \
            $(NOVA_ASM)/nui.inc $(NOVA_ASM)/nui.s \
            $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s \
            $(NOVA_ASM)/overlay.inc $(NOVA_ASM)/overlay.s \
            $(NOVA_ASM)/pager.inc $(NOVA_ASM)/pager.s \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s \
            $(NOVA_ASM)/runtime_labels.inc
	$(CA65) system.s -o system.o

system.bin: system.o system.cfg
	$(LD65) -C system.cfg -m system.map -o system.bin system.o

# The ADDR("LABEL") table is generated from the EhBASIC build (basic.sym). Build it
# there if missing so `make -C modules/system` works cold (the Avalonia/root build
# already runs `make -C ehbasic` first, making this a no-op in that path).
$(NOVA_ASM)/runtime_labels.inc:
	$(MAKE) -C ../../ehbasic runtime-labels

# Self-documenting module: image + NDOC doc (from system.s ;@ comments, ids
# resolved from libsystem.inc) -> system.nmod. --ndk-dir single-sources each ;@ndk
# fn's summary from the NDK routine and enforces the wrapper-calls-NDK drift guard.
system.nmod: system.bin system.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/vgc.s \
            $(NOVA_ASM)/rng.s $(NOVA_ASM)/nui.s $(NOVA_ASM)/overlay.s
	python3 ../../tools/nmod_pack.py --src system.s --bin system.bin \
	        --out system.nmod --syms $(NOVA_ASM)/libsystem.inc --ndk-dir $(NOVA_ASM)

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: system.bin
	cp system.bin ../../e6502.Avalonia/Resources/system.bin

clean:
	rm -f system.o system.bin system.map system.nmod

.PHONY: all clean install
