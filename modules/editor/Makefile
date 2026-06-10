CA65 = ca65 --cpu 65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm

all: editor.bin editor.nmod

editor.o: editor.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libeditor.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vgc.inc \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editui.s \
            $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editbuf.s
	$(CA65) editor.s -o editor.o

editor.bin: editor.o editor.cfg
	$(LD65) -C editor.cfg -m editor.map -o editor.bin editor.o

# Self-documenting module: image + NDOC doc (from editor.s ;@ comments, ids
# resolved from libeditor.inc) -> editor.nmod.
editor.nmod: editor.bin editor.s ../../tools/nmod_pack.py ../../tools/gen_runtime_abi_docs.py \
            $(NOVA_ASM)/libeditor.inc
	python3 ../../tools/nmod_pack.py --src editor.s --bin editor.bin \
	        --out editor.nmod --syms $(NOVA_ASM)/libeditor.inc --ndk-dir $(NOVA_ASM)

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: editor.bin
	cp editor.bin ../../e6502.Avalonia/Resources/editor.bin

clean:
	rm -f editor.o editor.bin editor.map editor.nmod

.PHONY: all clean install
