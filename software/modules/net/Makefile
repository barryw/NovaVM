CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: net.bin net.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

net.o: net.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libnet.inc \
            $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/nic.inc $(NOVA_ASM)/nic.s \
            $(NOVA_ASM)/gameserver.inc $(NOVA_ASM)/gameserver.s
	$(CA65) net.s -o net.o

net.bin: net.o net.cfg $(NOVA_LIB_FILE)
	$(LD65) -C net.cfg -m net.map -o net.bin net.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> net.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
net.nmod: net.bin net.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libnet.inc $(NOVA_ASM)/nic.s $(NOVA_ASM)/gameserver.s
	$(NOVA_CLI) module pack --src net.s --bin net.bin \
	        --out net.nmod --syms $(NOVA_ASM)/libnet.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: net.bin
	cp net.bin ../../../e6502.Avalonia/Resources/net.bin

clean:
	rm -f net.o net.bin net.map net.nmod

FORCE:

.PHONY: all clean install FORCE
