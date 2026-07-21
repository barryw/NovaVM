NOVA_ASM = ../../runtime/asm
TOOLCHAIN = ../../toolchain
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources
NOVA_CLI_DLL = ../../../e6502.Nova/bin/Release/net10.0/linux-x64/nova.dll
NOVA_CLI = dotnet $(NOVA_CLI_DLL)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs

BUILD_DIR = tools/build
TOOLCHAIN_BUILD = $(TOOLCHAIN)/build

TOOL_CFG = $(TOOLCHAIN)/tool.cfg
TOOL_IO = $(TOOLCHAIN_BUILD)/nptool_io.o
NPEDIT = $(BUILD_DIR)/NPEDIT.BIN
NPO2 = $(BUILD_DIR)/NPO2.BIN
NPPROJ = $(BUILD_DIR)/NPPROJ.BIN
NAS = $(TOOLCHAIN_BUILD)/NAS.BIN
NL = $(TOOLCHAIN_BUILD)/NL.BIN
TOOL_BINS = $(NPEDIT) $(NPO2) $(NPPROJ) $(NAS) $(NL)
PASCAL_LIBRARY = $(BUILD_DIR)/PASCAL.NLIB
PASCAL_BINDING_DIR = $(BUILD_DIR)/ndk
NDK_REFERENCE_SOURCES = $(wildcard $(NOVA_ASM)/*.inc) $(wildcard $(NOVA_ASM)/*.s)
NASPP_OVERLAY = $(TOOLCHAIN_BUILD)/NASPP.OVL
NASBE_OVERLAY = $(TOOLCHAIN_BUILD)/NASBE.OVL
NLWORK_OVERLAY = $(TOOLCHAIN_BUILD)/NLWORK.OVL
TOOLCHAIN_ASSETS = $(NAS) $(NL) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY) $(TOOL_IO)

all: novapascal.bin $(TOOL_BINS) $(PASCAL_LIBRARY) pascal-bindings $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novapascal.bin: novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o novapascal.cfg $(NOVA_LIB_FILE)
	ld65 -C novapascal.cfg -m novapascal.map -o $@ novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o $(NOVA_LIB_FILE)

novapascal.o: novapascal.s shell.s tools/npproj.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc novapascal.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -l novapascal.lst -o $@ novapascal.s

$(BUILD_DIR):
	mkdir -p $@

$(TOOLCHAIN_ASSETS): toolchain

toolchain:
	$(MAKE) -C $(TOOLCHAIN) all

$(BUILD_DIR)/resident_io.o: $(TOOLCHAIN)/nptool_io.s $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ $(TOOLCHAIN)/nptool_io.s

$(BUILD_DIR)/npc_resident.o: tools/npc.s $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/xram.inc $(NOVA_ASM)/docbuf.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npc.s

$(BUILD_DIR)/npedit.o: tools/npedit.s $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libeditor.inc $(NOVA_ASM)/libmemory.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npedit.s

$(NPEDIT): $(BUILD_DIR)/npedit.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npedit.map -o $@ $(BUILD_DIR)/npedit.o $(TOOL_IO)

$(BUILD_DIR)/npo2.o: tools/npo2.s $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/xramstream.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/xram.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npo2.s

$(NPO2): $(BUILD_DIR)/npo2.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npo2.map -o $@ $(BUILD_DIR)/npo2.o $(TOOL_IO)

$(BUILD_DIR)/npproj.o: tools/npproj.s tools/npproj.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I tools -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npproj.s

$(NPPROJ): $(BUILD_DIR)/npproj.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npproj.map -o $@ $(BUILD_DIR)/npproj.o $(TOOL_IO)

$(BUILD_DIR)/pascal_runtime.o: pascal_runtime.s $(TOOLCHAIN)/nobj.inc $(NOVA_ASM)/nova.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ pascal_runtime.s

$(PASCAL_LIBRARY): $(BUILD_DIR)/pascal_runtime.o pascal_runtime.cfg
	ld65 -C pascal_runtime.cfg -o $@ $(BUILD_DIR)/pascal_runtime.o

$(NOVA_CLI_DLL): $(NOVA_CLI_SOURCES)
	dotnet build ../../../e6502.Nova/e6502.Nova.csproj -c Release -m:1 /p:SkipRomBuild=true /nr:false

pascal-bindings: $(NDK_REFERENCE_SOURCES) $(NOVA_CLI_SOURCES) $(NOVA_CLI_DLL) | $(BUILD_DIR)
	rm -rf $(PASCAL_BINDING_DIR)
	mkdir -p $(PASCAL_BINDING_DIR)
	$(NOVA_CLI) codegen ndk-reference --runtime-dir $(NOVA_ASM) \
	    --tex $(BUILD_DIR)/ndk-reference.tex \
	    --json $(BUILD_DIR)/ndk-reference.json \
	    --pascal-dir $(PASCAL_BINDING_DIR)

install: novapascal.bin
	cp novapascal.bin $(RESOURCES)/novapascal.bin

clean:
	rm -f *.o *.bin *.lst *.map AUTOBOOT.bin ndi-autoboot.o
	rm -rf $(BUILD_DIR)

FORCE:

.PHONY: clean all install toolchain FORCE

# Bootable language floppy: AUTOBOOT loads the shell ROM into the primary bank.
NDI_RUNTIME := novapascal.bin
NDI_OUTPUT  := novapascal.ndi
NDI_LABEL   := NPASCAL
NDI_DIRECTORY_SECTORS := 96
PASCAL_UNIT_SOURCES := pascal.inc units/novagfx.pas units/graphics.inc units/graphics.npi units/graphics.s \
	units/novainput.pas units/input.inc units/input.npi units/input.s \
	units/novarand.pas units/random.inc units/random.npi units/random.s
NDI_ASSET_DEPS := hello.pas hello.npp fizzbuzz.pas fizzbuzz.npp life.pas life.npp $(PASCAL_UNIT_SOURCES) $(NOVA_ASM)/nvr.inc $(TOOL_BINS) $(PASCAL_LIBRARY) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)
NDI_ASSETS := --asset hello.pas:HELLO/MAIN.PAS --asset hello.npp:HELLO/HELLO.NPP \
	--asset fizzbuzz.pas:FIZZBUZZ/MAIN.PAS --asset fizzbuzz.npp:FIZZBUZZ/FIZZBUZZ.NPP \
	--asset life.pas:LIFE/MAIN.PAS --asset life.npp:LIFE/LIFE.NPP \
	--asset pascal.inc:PASCAL.INC \
	--asset $(NOVA_ASM)/nvr.inc:NVR.INC \
	--asset units/novagfx.pas:NOVAGFX.PAS \
	--asset units/graphics.inc:GRAPHICS.INC \
	--asset units/graphics.npi:GRAPHICS.NPI \
	--asset units/graphics.s:GRAPHICS.S \
	--asset units/novainput.pas:NOVAINPUT.PAS \
	--asset units/input.inc:INPUT.INC \
	--asset units/input.npi:INPUT.NPI \
	--asset units/input.s:INPUT.S \
	--asset units/novarand.pas:NOVARAND.PAS \
	--asset units/random.inc:RANDOM.INC \
	--asset units/random.npi:RANDOM.NPI \
	--asset units/random.s:RANDOM.S \
	--asset $(BUILD_DIR)/NPEDIT.BIN:NPEDIT.BIN \
	--asset $(BUILD_DIR)/NPO2.BIN:NPO2.BIN \
	--asset $(BUILD_DIR)/NPPROJ.BIN:NPPROJ.BIN \
	--asset $(NAS):NAS.BIN \
	--asset $(NL):NL.BIN \
	--asset $(NASPP_OVERLAY):NASPP.OVL \
	--asset $(NASBE_OVERLAY):NASBE.OVL \
	--asset $(NLWORK_OVERLAY):NLWORK.OVL \
	--asset $(PASCAL_LIBRARY):PASCAL.NLIB \
	--asset-dir $(PASCAL_BINDING_DIR)
include ../../runtime/ndi/ndi.mk

$(NDI_OUTPUT): pascal-bindings

.PHONY: pascal-bindings
