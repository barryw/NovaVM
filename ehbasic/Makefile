# EhBASIC 2.22 + NovaVM tools — cc65 toolchain
# Requires ca65 and ld65 on PATH

CC65_CFG ?= $(shell dirname $(shell which ld65))/../cfg

all: basic.bin extension.bin keyboard.bin demo.bin tokens.json

ABI_SOURCES = \
	lib/nova.inc lib/fio.inc lib/fio.s \
	lib/audio.inc lib/audio.s \
	lib/nic.inc lib/nic.s \
	lib/vgc.inc lib/vgc.s \
	lib/sprite.inc lib/sprite.s \
	lib/dma.inc lib/dma.s \
	lib/blitter.inc lib/blitter.s \
	lib/copper.inc lib/copper.s \
	lib/tile.inc lib/tile.s \
	lib/xram.inc lib/xram.s lib/xmc.s

# ROM build
basic.bin basic.sym: basic.o ehbasic.cfg
	ld65 -C ehbasic.cfg -vm -m basic.map -Ln basic.sym -o basic.bin basic.o

basic.o: basic.asm min_mon.asm lib/nova.inc lib/fio.inc lib/fio.s lib/audio.inc lib/audio.s lib/nic.inc lib/nic.s lib/vgc.inc lib/vgc.s lib/sprite.inc lib/sprite.s
	ca65 --cpu 65c02 -g -l min_mon.lst --feature labels_without_colons -o basic.o min_mon.asm

# Extension ROM build
extension.bin: extension.o extension.cfg
	ld65 -C extension.cfg -o extension.bin extension.o

extension.o: extension.s runtime_labels.inc lib/nova.inc lib/fio.inc lib/fio.s lib/audio.inc lib/audio.s lib/dma.inc lib/dma.s lib/blitter.inc lib/blitter.s lib/copper.inc lib/copper.s lib/tile.inc lib/tile.s lib/xram.inc lib/xram.s lib/xmc.s
	ca65 --cpu 65c02 extension.s -o extension.o

# Token table extraction
tokens.json: basic.asm ../tools/gen_tokens.py
	python3 ../tools/gen_tokens.py basic.asm -o tokens.json

runtime_labels.json ../docs/assembly/runtime-labels.md runtime_labels.inc: basic.sym $(ABI_SOURCES) ../tools/gen_runtime_abi_docs.py
	python3 ../tools/gen_runtime_abi_docs.py $(ABI_SOURCES) --sym basic.sym --json runtime_labels.json --md ../docs/assembly/runtime-labels.md --asm runtime_labels.inc

runtime-labels: runtime_labels.json ../docs/assembly/runtime-labels.md runtime_labels.inc

# Auto-generate hardware constants include
novavm.inc: ../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym ../tools/gen_novavm_inc.py
	python3 ../tools/gen_novavm_inc.py ../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym -o novavm.inc

# Keyboard visualizer
keyboard: keyboard.bin

keyboard.bin: keyboard.o keyboard.cfg
	ld65 -C keyboard.cfg -o keyboard.bin keyboard.o

keyboard.o: keyboard.s novavm.inc
	ca65 --cpu 65c02 keyboard.s -o keyboard.o

install-keyboard: keyboard.bin
	cp keyboard.bin ~/e6502-programs/keyboard.bin

# Demo disk player
demo: demo.bin

demo.bin: demo.o demo.cfg
	ld65 -C demo.cfg -o demo.bin demo.o

demo.o: demo.s novavm.inc
	ca65 --cpu 65c02 demo.s -o demo.o

install-demo: demo.bin
	cp demo.bin ~/e6502-programs/demo.bin

clean:
	$(RM) *.o *.lst *.map *.bin *.sym

.PHONY: all runtime-labels keyboard install-keyboard demo install-demo clean
