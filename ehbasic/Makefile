# EhBASIC 2.22 + NovaVM tools — cc65 toolchain
# Requires ca65 and ld65 on PATH

CC65_CFG ?= $(shell dirname $(shell which ld65))/../cfg

all: basic.bin keyboard.bin tokens.json

# ROM build
basic.bin basic.sym: basic.o ehbasic.cfg
	ld65 -C ehbasic.cfg -vm -m basic.map -Ln basic.sym -o basic.bin basic.o

basic.o: basic.asm min_mon.asm
	ca65 -g -l min_mon.lst --feature labels_without_colons -o basic.o min_mon.asm

# Token table extraction
tokens.json: basic.asm ../tools/gen_tokens.py
	python3 ../tools/gen_tokens.py basic.asm -o tokens.json

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

clean:
	$(RM) *.o *.lst *.map *.bin *.sym

.PHONY: all keyboard install-keyboard clean
