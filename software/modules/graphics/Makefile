CA65 = ca65 --cpu w65c02 -I ../../runtime/asm
LD65 = ld65
NOVA_ASM = ../../runtime/asm
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
NOVA_CLI_PROJECT = ../../../e6502.Nova/e6502.Nova.csproj
NOVA_CLI_RID ?= linux-x64
NOVA_CLI_BIN ?= ../../../e6502.Nova/bin/Release/net10.0/$(NOVA_CLI_RID)/nova
NOVA_CLI = $(NOVA_CLI_BIN)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

all: graphics.bin graphics.nmod

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

$(NOVA_CLI_BIN): FORCE
	dotnet build $(NOVA_CLI_PROJECT) -c Release -r $(NOVA_CLI_RID) -p:NuGetAudit=false -p:UseSharedCompilation=false -p:BuildInParallel=false -v:minimal

graphics.o: graphics.s \
            $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmod.inc $(NOVA_ASM)/libgraphics.inc \
            $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/nova.inc \
            $(NOVA_ASM)/sprite.inc \
            $(NOVA_ASM)/copper.inc \
            $(NOVA_ASM)/copper_split.inc \
            $(NOVA_ASM)/blitter.inc \
            $(NOVA_ASM)/dma.inc \
            $(NOVA_ASM)/vsprite.inc \
            $(NOVA_ASM)/msprite.inc \
            $(NOVA_ASM)/nvg.inc \
            $(NOVA_ASM)/fio.inc \
            $(NOVA_ASM)/anim.inc \
            $(NOVA_ASM)/tween.inc \
            $(NOVA_ASM)/pager.inc \
            $(NOVA_LIB_FILE)
	$(CA65) graphics.s -o graphics.o

graphics.bin: graphics.o graphics.cfg $(NOVA_LIB_FILE)
	$(LD65) -C graphics.cfg -m graphics.map -o graphics.bin graphics.o $(NOVA_LIB_FILE)

# Self-documenting module: image + NDOC doc (from graphics.s ;@ comments, ids
# resolved from libgraphics.inc) -> graphics.nmod. --ndk-dir single-sources each
# ;@ndk fn's summary from the wrapped NDK routine and enforces the drift guard
# (wrapper must JSR the routine it maps to).
graphics.nmod: graphics.bin graphics.s $(NOVA_CLI_BIN) $(NOVA_CLI_SOURCES) \
            $(NOVA_ASM)/libgraphics.inc $(wildcard $(NOVA_ASM)/*.s)
	$(NOVA_CLI) module pack --src graphics.s --bin graphics.bin \
	        --out graphics.nmod --syms $(NOVA_ASM)/libgraphics.inc --ndk-dir $(NOVA_ASM)

# Copy the raw 16K image into the Avalonia Resources dir, where CompositeBusDevice
# pre-stages it into the XRAM shelf at boot (mirrors firmware boot.json staging).
install: graphics.bin
	cp graphics.bin ../../../e6502.Avalonia/Resources/graphics.bin

clean:
	rm -f graphics.o graphics.bin graphics.map graphics.nmod

FORCE:

.PHONY: all clean install FORCE
