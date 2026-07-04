CA65=ca65 --cpu w65c02 -I ../../runtime/asm
LD65=ld65
NOVA_CLI=dotnet run --project ../../../e6502.Nova -c Release --
NOVA_CLI_SOURCES=../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs
all: testmod.bin testmod.nmod libcall.bin
testmod.o: testmod.s ../../runtime/asm/libabi.inc ../../runtime/asm/libmod.inc
	$(CA65) testmod.s -o testmod.o
testmod.bin: testmod.o testmod.cfg
	$(LD65) -C testmod.cfg -o testmod.bin testmod.o
# Self-documenting module: testmod.bin + ;@ doc-comments -> testmod.nmod
testmod.nmod: testmod.bin testmod.s $(NOVA_CLI_SOURCES)
	$(NOVA_CLI) module pack --src testmod.s --bin testmod.bin --out testmod.nmod
libcall.o: ../../runtime/asm/libcall.s ../../runtime/asm/libabi.inc ../../runtime/asm/nova.inc
	$(CA65) ../../runtime/asm/libcall.s -o libcall.o
libcall.bin: libcall.o libcall_blob.cfg
	$(LD65) -C libcall_blob.cfg -o libcall.bin libcall.o
clean:
	rm -f testmod.o testmod.bin testmod.nmod libcall.o libcall.bin
