CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: sound.bin sound.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

sound.o: sound.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libsound.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/audio.inc $(NOVA_ASM)/audio.s \
            $(NOVA_ASM)/fio.inc $(NOVA_ASM)/fio.s \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/vgc.s
	$(CA65) sound.s -o sound.o

sound.bin: sound.o sound.cfg $(NOVA_LIB_FILE)
	$(LD65) -C sound.cfg -m sound.map -o sound.bin sound.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc -> sound.nmod. --ndk-dir single-sources
# each ;@ndk fn's summary from the NDK routine and enforces the wrapper-calls-NDK
# drift guard. Depends on the wrapped NDK sources so a summary edit rebuilds the doc.
sound.nmod: sound.bin sound.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libsound.inc $(NOVA_ASM)/audio.s $(NOVA_ASM)/vgc.s
	$(NOVA_CLI) module pack --src sound.s --bin sound.bin \
	        --out sound.nmod --syms $(NOVA_ASM)/libsound.inc --ndk-dir $(NOVA_ASM)

# Stage the 16K image into Avalonia Resources (CompositeBusDevice boot-stages it).
install: sound.bin
	cp sound.bin ../../../e6502.Avalonia/Resources/sound.bin

clean:
	rm -f sound.o sound.bin sound.map sound.nmod

FORCE:

.PHONY: all clean install FORCE
