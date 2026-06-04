# runtime/asm/Makefile — canonical resident library loader build (Task 4c.0a).
#
# Builds the ONE canonical libcall.bin from libcall.s at ORG $0320 (the
# cross-runtime resident loader band, just above the $0300-$031F mailbox) and
# installs it into the Avalonia Resources, mirroring how novalogo.bin lives
# there. This is distinct from tests/asm/libcall.bin (ORG $9C00), which is
# isolated $9C00 scaffolding for LibCallTests/GraphicsModuleTests/HW smoke.
#
# Requires ca65 and ld65 on PATH (cc65 toolchain).

CA65      := ca65 --cpu 65c02 -I .
LD65      := ld65
RESOURCES := ../../e6502.Avalonia/Resources

all: libcall.bin

libcall.o: libcall.s libabi.inc nova.inc
	$(CA65) libcall.s -o libcall.o

libcall.bin: libcall.o libcall_resident.cfg
	$(LD65) -C libcall_resident.cfg -m libcall.map -o libcall.bin libcall.o

install: libcall.bin
	cp libcall.bin $(RESOURCES)/libcall.bin

clean:
	rm -f libcall.o libcall.bin libcall.map

.PHONY: all install clean
