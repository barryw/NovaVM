# EhBASIC 2.22 build using cc65 toolchain
# Requires ca65 and ld65 on PATH

# Find cc65 config directory
CC65_CFG ?= $(shell dirname $(shell which ld65))/../cfg

all: basic.bin

basic.bin: basic.o
	ld65 -C $(CC65_CFG)/none.cfg -vm -m basic.map -o basic.bin basic.o

basic.o: basic.asm min_mon.asm
	ca65 -g -l min_mon.lst --feature labels_without_colons -o basic.o min_mon.asm

clean:
	$(RM) *.o *.lst *.map *.bin

.PHONY: all clean
