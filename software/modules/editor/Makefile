CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: editor.bin editor.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

editor.o: editor.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libeditor.inc \
            $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vgc.inc \
            $(NOVA_ASM)/copper.inc $(NOVA_ASM)/copper.s \
            $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editui.s \
            $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/vtext.s \
            $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/blitter.s \
            $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editbuf.s $(NOVA_ASM)/editpager.s
	$(CA65) editor.s -o editor.o

editor.bin: editor.o editor.cfg $(NOVA_LIB_FILE)
	$(LD65) -C editor.cfg -m editor.map -o editor.bin editor.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc (from editor.s ;@ comments, ids
# resolved from libeditor.inc) -> editor.nmod.
editor.nmod: editor.bin editor.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libeditor.inc
	$(NOVA_CLI) module pack --src editor.s --bin editor.bin \
	        --out editor.nmod --syms $(NOVA_ASM)/libeditor.inc --ndk-dir $(NOVA_ASM)

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: editor.bin editor.nmod
	cp editor.bin ../../../e6502.Avalonia/Resources/editor.bin

clean:
	rm -f editor.o editor.bin editor.map editor.nmod

FORCE:

.PHONY: all clean install FORCE
