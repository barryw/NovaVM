# EhBASIC 2.22 + NovaVM tools — cc65 toolchain
# Requires ca65 and ld65 on PATH

CC65_CFG ?= $(shell dirname $(shell which ld65))/../cfg
NOVA_ASM := ../../runtime/asm
NOVAVM_INC := $(NOVA_ASM)/novavm.inc
NOVA_LIB_FILE := $(NOVA_ASM)/build/nova.lib

all: basic.bin extension.bin ../../runtime/asm/runtime_labels.inc ../../assembly/apps/keyboard/keyboard.bin ../../assembly/apps/demo/demo.bin tokens.json

ABI_SOURCES = \
	$(NOVA_ASM)/nova.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio_clear_error.s $(NOVA_ASM)/fio.s \
	$(NOVA_ASM)/audio.inc $(NOVA_ASM)/audio.s \
	$(NOVA_ASM)/nic.inc $(NOVA_ASM)/nic.s \
	$(NOVA_ASM)/gameserver.inc $(NOVA_ASM)/gameserver.s \
	$(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s \
	$(NOVA_ASM)/vgc_palette.inc $(NOVA_ASM)/vgc_palette.s \
	$(NOVA_ASM)/sprite.inc $(NOVA_ASM)/sprite.s \
	$(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
	$(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
	$(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s \
	$(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
	$(NOVA_ASM)/xram.inc $(NOVA_ASM)/xram.s \
	$(NOVA_ASM)/pager.inc $(NOVA_ASM)/pager.s \
	$(NOVA_ASM)/rng.inc $(NOVA_ASM)/rng.s \
	$(NOVA_ASM)/nvg.inc $(NOVA_ASM)/nvg.s \
	$(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s $(NOVA_ASM)/vtext_mixed.s \
	$(NOVA_ASM)/nui.inc $(NOVA_ASM)/nui.s \
	$(NOVA_ASM)/replline.inc $(NOVA_ASM)/replline.s \
	$(NOVA_ASM)/tween.inc $(NOVA_ASM)/tween.s \
	$(NOVA_ASM)/xmc.inc $(NOVA_ASM)/xmc.s

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

# ROM build
basic.bin basic.sym: basic.o ehbasic.cfg $(NOVA_LIB_FILE)
	ld65 -C ehbasic.cfg -vm -m basic.map -Ln basic.sym -o basic.bin basic.o $(NOVA_LIB_FILE)

basic.o: basic.asm min_mon.asm $(NOVA_ASM)/nova.inc $(NOVA_ASM)/copper.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/audio.inc $(NOVA_ASM)/nic.inc $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/sprite.inc $(NOVA_ASM)/rng.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -g -l min_mon.lst --feature labels_without_colons -o basic.o min_mon.asm

# Extension ROM build
extension.bin: extension.o extension.cfg $(NOVA_LIB_FILE)
	ld65 -C extension.cfg -o extension.bin extension.o $(NOVA_LIB_FILE)

extension.o: extension.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/audio.inc $(NOVA_ASM)/dma.inc $(NOVA_ASM)/xram.inc $(NOVA_ASM)/rng.inc $(NOVA_ASM)/xmc.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) extension.s -o extension.o

# Token table extraction
tokens.json: basic.asm ../../../tools/gen_tokens.py
	python3 ../../../tools/gen_tokens.py basic.asm -o tokens.json

runtime_labels.json ../../../docs/assembly/runtime-labels.md ../../runtime/asm/runtime_labels.inc: basic.sym $(ABI_SOURCES) ../../../tools/gen_runtime_abi_docs.py
	python3 ../../../tools/gen_runtime_abi_docs.py $(ABI_SOURCES) --sym basic.sym --json runtime_labels.json --md ../../../docs/assembly/runtime-labels.md --asm ../../runtime/asm/runtime_labels.inc

runtime-labels: runtime_labels.json ../../../docs/assembly/runtime-labels.md ../../runtime/asm/runtime_labels.inc

# Auto-generate hardware constants include
$(NOVAVM_INC): ../../../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym ../../../tools/gen_novavm_inc.py
	python3 ../../../tools/gen_novavm_inc.py ../../../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym -o $(NOVAVM_INC)

novavm.inc: $(NOVAVM_INC)
	@echo "novavm.inc is generated at $(NOVAVM_INC)"

# Standalone assembly applications
keyboard: keyboard.bin

keyboard.bin: ../../assembly/apps/keyboard/keyboard.bin

../../assembly/apps/keyboard/keyboard.bin: $(NOVAVM_INC)
	$(MAKE) -C ../../assembly keyboard

install-keyboard: keyboard.bin
	cp ../../assembly/apps/keyboard/keyboard.bin ~/e6502-programs/keyboard.bin

demo: demo.bin

demo.bin: ../../assembly/apps/demo/demo.bin

../../assembly/apps/demo/demo.bin: $(NOVAVM_INC)
	$(MAKE) -C ../../assembly demo

install-demo: demo.bin
	cp ../../assembly/apps/demo/demo.bin ~/e6502-programs/demo.bin

clean:
	$(RM) *.o *.lst *.map *.bin *.sym
	$(MAKE) -C ../../assembly clean

FORCE:

.PHONY: all runtime-labels keyboard install-keyboard demo install-demo clean FORCE
