CA65=ca65 --cpu 65c02 -I ../../runtime/asm
LD65=ld65
all: testmod.bin libcall.bin
testmod.o: testmod.s ../../runtime/asm/libabi.inc ../../runtime/asm/libmod.inc
	$(CA65) testmod.s -o testmod.o
testmod.bin: testmod.o testmod.cfg
	$(LD65) -C testmod.cfg -o testmod.bin testmod.o
libcall.o: ../../runtime/asm/libcall.s ../../runtime/asm/libabi.inc ../../runtime/asm/nova.inc
	$(CA65) ../../runtime/asm/libcall.s -o libcall.o
libcall.bin: libcall.o libcall_blob.cfg
	$(LD65) -C libcall_blob.cfg -o libcall.bin libcall.o
clean:
	rm -f testmod.o testmod.bin libcall.o libcall.bin
