CA65=ca65 --cpu 65c02 -I ../../runtime/asm
LD65=ld65
all: testmod.bin
testmod.o: testmod.s ../../runtime/asm/libabi.inc ../../runtime/asm/libmod.inc
	$(CA65) testmod.s -o testmod.o
testmod.bin: testmod.o testmod.cfg
	$(LD65) -C testmod.cfg -o testmod.bin testmod.o
clean:
	rm -f testmod.o testmod.bin
