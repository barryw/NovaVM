CA65 = ca65 --cpu w65c02 -I ../../runtime/asm -I ../../languages/novapascal
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: langrt.bin langrt.nmod

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

langrt.o: langrt.s langrt.cfg ../../runtime/asm/libabi.inc \
          ../../runtime/asm/libmod.inc ../../runtime/asm/liblangrt.inc \
          ../../runtime/asm/nova.inc ../../runtime/asm/math_copro.inc \
          ../../languages/novapascal/pascal_real_runtime.inc
	$(CA65) langrt.s -o langrt.o

langrt.bin: langrt.o langrt.cfg
	$(LD65) -C langrt.cfg -m langrt.map -o langrt.bin langrt.o

langrt.nmod: langrt.bin langrt.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
              $(NOVA_ASM)/liblangrt.inc
	$(NOVA_CLI) module pack --src langrt.s --bin langrt.bin \
	        --out langrt.nmod --syms $(NOVA_ASM)/liblangrt.inc --ndk-dir $(NOVA_ASM)

install: langrt.bin langrt.nmod
	cp langrt.bin ../../../e6502.Avalonia/Resources/langrt.bin

clean:
	rm -f langrt.o langrt.bin langrt.map langrt.nmod

FORCE:

.PHONY: all install clean FORCE
