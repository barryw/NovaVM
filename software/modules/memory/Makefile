CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: memory.bin memory.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

memory.o: memory.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libmemory.inc \
            $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/novaexec.inc \
            $(NOVA_ASM)/xram.inc $(NOVA_ASM)/xram.s \
            $(NOVA_ASM)/xmc.s \
            $(NOVA_ASM)/dma.inc $(NOVA_ASM)/dma.s \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s
	$(CA65) memory.s -o memory.o

memory.bin: memory.o memory.cfg $(NOVA_LIB_FILE)
	$(LD65) -C memory.cfg -m memory.map -o memory.bin memory.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> memory.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
memory.nmod: memory.bin memory.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/xram.s $(NOVA_ASM)/xmc.s
	$(NOVA_CLI) module pack --src memory.s --bin memory.bin \
	        --out memory.nmod --syms $(NOVA_ASM)/libmemory.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: memory.bin
	cp memory.bin ../../../e6502.Avalonia/Resources/memory.bin

clean:
	rm -f memory.o memory.bin memory.map memory.nmod

FORCE:

.PHONY: all clean install FORCE
