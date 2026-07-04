CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: turtle.bin turtle.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

turtle.o: turtle.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libturtle.inc \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/sprite.inc $(NOVA_ASM)/sprite.s \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/copper_split.inc $(NOVA_ASM)/copper_split.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
            $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/vsprite.s
	$(CA65) turtle.s -o turtle.o

turtle.bin: turtle.o turtle.cfg $(NOVA_LIB_FILE)
	$(LD65) -C turtle.cfg -m turtle.map -o turtle.bin turtle.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> turtle.nmod. --ndk-dir keeps the
# drift guard available (the turtle has no ;@ndk mappings, so nothing to check —
# it owns its own move-math + render rather than wrapping NDK leaf routines).
turtle.nmod: turtle.bin turtle.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libturtle.inc
	$(NOVA_CLI) module pack --src turtle.s --bin turtle.bin \
	        --out turtle.nmod --syms $(NOVA_ASM)/libturtle.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice demand-loads it).
install: turtle.bin
	cp turtle.bin ../../../e6502.Avalonia/Resources/turtle.bin

clean:
	rm -f turtle.o turtle.bin turtle.map turtle.nmod

FORCE:

.PHONY: all clean install FORCE
