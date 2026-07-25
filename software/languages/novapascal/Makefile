NOVA_ASM = ../../runtime/asm
TOOLCHAIN = ../../toolchain
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources
NOVA_CLI_DLL = ../../../e6502.Nova/bin/Release/net10.0/linux-x64/nova.dll
NOVA_CLI = dotnet $(NOVA_CLI_DLL)
NOVA_CLI_SOURCES = ../../../e6502.Nova/Program.cs ../../../e6502.Nova/BuildTools.cs
OVERLAY_PACKER = ../../../e6502.Tools/e6502.Tools.csproj
OVERLAY_PACKER_DLL = ../../../e6502.Tools/bin/Release/net10.0/e6502.Tools.dll

BUILD_DIR = tools/build
TOOLCHAIN_BUILD = $(TOOLCHAIN)/build

TOOL_CFG = $(TOOLCHAIN)/tool.cfg
TOOL_IO = $(TOOLCHAIN_BUILD)/nptool_io.o
NPEDIT = $(BUILD_DIR)/NPEDIT.BIN
NPO2 = $(BUILD_DIR)/NPO2.BIN
NPPROJ = $(BUILD_DIR)/NPPROJ.BIN
NPCFE_PAYLOAD = $(BUILD_DIR)/npcfe.bin
NPCFE = $(BUILD_DIR)/NPCFE.OVL
NPCFE_LOAD = 0x1E00
NPCFE_MAX = 0x8200
NAS = $(TOOLCHAIN_BUILD)/NAS.BIN
NL = $(TOOLCHAIN_BUILD)/NL.BIN
NBUILD = $(TOOLCHAIN_BUILD)/NBUILD.BIN
TOOL_BINS = $(NPEDIT) $(NPO2) $(NPPROJ) $(NAS) $(NL) $(NBUILD)
PASCAL_LIBRARY = $(BUILD_DIR)/PASCAL.NLIB
PASCAL_BINDING_DIR = $(BUILD_DIR)/ndk
NDK_REFERENCE_SOURCES = $(wildcard $(NOVA_ASM)/*.inc) $(wildcard $(NOVA_ASM)/*.s)
NASPP_OVERLAY = $(TOOLCHAIN_BUILD)/NASPP.OVL
NASBE_OVERLAY = $(TOOLCHAIN_BUILD)/NASBE.OVL
NLWORK_OVERLAY = $(TOOLCHAIN_BUILD)/NLWORK.OVL
TOOLCHAIN_ASSETS = $(NAS) $(NL) $(NBUILD) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY) $(TOOL_IO)

all: novapascal.bin $(NPCFE) $(TOOL_BINS) $(PASCAL_LIBRARY) pascal-bindings $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novapascal.bin: novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/npc_frontend.o $(BUILD_DIR)/resident_io.o novapascal.cfg $(NOVA_LIB_FILE)
	ld65 -C novapascal.cfg -m novapascal.map -Ln $(BUILD_DIR)/novapascal.sym -o $@ novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/npc_frontend.o $(BUILD_DIR)/resident_io.o $(NOVA_LIB_FILE)

novapascal.o: novapascal.s shell.s tools/npproj.inc tools/npc_frontend.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nbuild.inc $(TOOLCHAIN)/nptool.inc novapascal.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -l novapascal.lst -o $@ novapascal.s

$(BUILD_DIR):
	mkdir -p $@

$(TOOLCHAIN_ASSETS): toolchain

toolchain:
	$(MAKE) -C $(TOOLCHAIN) all

$(BUILD_DIR)/resident_io.o: $(TOOLCHAIN)/nptool_io.s $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ $(TOOLCHAIN)/nptool_io.s

$(BUILD_DIR)/npc_resident.o: tools/npc_resident.s tools/npc_frontend.inc $(TOOLCHAIN)/nptool.inc $(TOOLCHAIN)/nptool_xram.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/xram.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I tools -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npc_resident.s

$(BUILD_DIR)/npc_frontend.o: tools/npc.s tools/npc_frontend.inc tools/npc_lowering.inc tools/npproj.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/xram.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I tools -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npc.s

$(NPCFE_PAYLOAD): novapascal.bin

$(OVERLAY_PACKER_DLL): FORCE
	dotnet build $(OVERLAY_PACKER) -c Release -m:1 /p:SkipRomBuild=true /nr:false

$(NPCFE): $(NPCFE_PAYLOAD) $(OVERLAY_PACKER_DLL)
	dotnet "$(OVERLAY_PACKER_DLL)" pack-overlay --input "$(NPCFE_PAYLOAD)" --output "$@" --load $(NPCFE_LOAD) --max-size $(NPCFE_MAX) --sym "$(BUILD_DIR)/novapascal.sym" --main npcfe_main --bss-size 0 --module-id 0x4E43 --module-version 1

$(BUILD_DIR)/npedit.o: tools/npedit.s $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editui.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libeditor.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/nui.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npedit.s

$(NPEDIT): $(BUILD_DIR)/npedit.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npedit.map -o $@ $(BUILD_DIR)/npedit.o $(TOOL_IO)

$(BUILD_DIR)/npo2.o: tools/npo2.s tools/npc_lowering.inc tools/npproj.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/xramstream.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/pager.inc $(NOVA_ASM)/xram.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I tools -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npo2.s

$(NPO2): $(BUILD_DIR)/npo2.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npo2.map -o $@ $(BUILD_DIR)/npo2.o $(TOOL_IO)

$(BUILD_DIR)/npproj.o: tools/npproj.s tools/npproj.inc $(TOOLCHAIN)/longbranch.inc $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I tools -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npproj.s

$(NPPROJ): $(BUILD_DIR)/npproj.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npproj.map -o $@ $(BUILD_DIR)/npproj.o $(TOOL_IO)

$(BUILD_DIR)/pascal_runtime.o: pascal_runtime.s pascal_real_runtime.inc pascal_real_thunks.inc pascal_longint_thunks.inc $(TOOLCHAIN)/nobj.inc $(NOVA_ASM)/array_index_core.body $(NOVA_ASM)/array_index_byte_stack.body $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/libmemory.inc | $(BUILD_DIR)
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
NDI_SIZE_KB := 2048
NDI_DIRECTORY_SECTORS := 128
PASCAL_UNIT_SOURCES := pascal.inc pascalrt.inc units/novagfx.pas units/graphics.inc units/graphics.npi units/graphics.s \
	units/novainput.pas units/input.inc units/input.npi units/input.s \
	units/novarand.pas units/random.inc units/random.npi units/random.s \
	units/novamemory.pas units/memory.inc units/memory.npi units/memory.s \
	units/crt.pas units/crt.asm units/crt.npi units/crtface.s \
	units/dos.pas units/dos.asm units/dos.inc units/dos.npi units/dos.s \
	units/graph.pas units/graph.asm units/graph.inc units/graph.npi units/graph.s \
	units/overlay.pas units/overlay.asm units/overlayface.npi units/overlayface.s \
	units/printer.pas units/printer.asm units/printer.npi units/printer.s \
	units/strings.pas units/strings.asm units/strings.npi units/strings.s
MICROCALC_SOURCES := microcalc/MC.PAS microcalc/MC-MOD00.INC microcalc/MC-MOD01.INC \
	microcalc/MC-MOD02.INC microcalc/MC-MOD03.INC microcalc/MC-MOD04.INC \
	microcalc/MC-MOD05.INC microcalc/MC.HLP microcalc/MCDEMO.MCS \
	microcalc/MICROCALC.NPP microcalc/README.md
NDI_ASSET_DEPS := hello.pas hello.npp fizzbuzz.pas fizzbuzz.npp life.pas life.npp novapascal-help.txt user-readme.txt $(MICROCALC_SOURCES) $(PASCAL_UNIT_SOURCES) $(NOVA_ASM)/nvr.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/libsound.inc $(NOVA_ASM)/libsystem.inc $(NPCFE) $(TOOL_BINS) $(PASCAL_LIBRARY) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)
NDI_ASSETS := --asset hello.pas:HELLO/MAIN.PAS --asset hello.npp:HELLO/HELLO.NPP \
	--asset fizzbuzz.pas:FIZZBUZZ/MAIN.PAS --asset fizzbuzz.npp:FIZZBUZZ/FIZZBUZZ.NPP \
	--asset life.pas:LIFE/MAIN.PAS --asset life.npp:LIFE/LIFE.NPP \
	--asset microcalc/MC.PAS:MICROCALC/MC.PAS \
	--asset microcalc/MC-MOD00.INC:MICROCALC/MC-MOD00.INC \
	--asset microcalc/MC-MOD01.INC:MICROCALC/MC-MOD01.INC \
	--asset microcalc/MC-MOD02.INC:MICROCALC/MC-MOD02.INC \
	--asset microcalc/MC-MOD03.INC:MICROCALC/MC-MOD03.INC \
	--asset microcalc/MC-MOD04.INC:MICROCALC/MC-MOD04.INC \
	--asset microcalc/MC-MOD05.INC:MICROCALC/MC-MOD05.INC \
	--asset microcalc/MC.HLP:MICROCALC/MC.HLP \
	--asset microcalc/MCDEMO.MCS:MICROCALC/MCDEMO.MCS \
	--asset microcalc/MICROCALC.NPP:MICROCALC/MICROCALC.NPP \
	--asset microcalc/README.md:MICROCALC/README.TXT \
	--asset pascal.inc:SYSTEM/PASCAL.INC \
	--asset pascalrt.inc:SYSTEM/PASCALRT.INC \
	--asset $(NOVA_ASM)/nvr.inc:SYSTEM/NVR.INC \
	--asset units/novagfx.pas:SYSTEM/NOVAGFX.PAS \
	--asset units/graphics.inc:SYSTEM/GRAPHICS.INC \
	--asset units/graphics.npi:SYSTEM/GRAPHICS.NPI \
	--asset units/graphics.s:SYSTEM/GRAPHICS.S \
	--asset units/novainput.pas:SYSTEM/NOVAINPUT.PAS \
	--asset units/input.inc:SYSTEM/INPUT.INC \
	--asset units/input.npi:SYSTEM/INPUT.NPI \
	--asset units/input.s:SYSTEM/INPUT.S \
	--asset units/novarand.pas:SYSTEM/NOVARAND.PAS \
	--asset units/random.inc:SYSTEM/RANDOM.INC \
	--asset units/random.npi:SYSTEM/RANDOM.NPI \
	--asset units/random.s:SYSTEM/RANDOM.S \
	--asset units/novamemory.pas:SYSTEM/NOVAMEMORY.PAS \
	--asset units/memory.inc:SYSTEM/MEMORY.INC \
	--asset units/memory.npi:SYSTEM/MEMORY.NPI \
	--asset units/memory.s:SYSTEM/MEMORY.S \
	--asset units/crt.pas:SYSTEM/CRT.PAS \
	--asset units/crt.asm:SYSTEM/CRT.ASM \
	--asset units/crt.npi:SYSTEM/CRT.NPI \
	--asset units/crtface.s:SYSTEM/CRTFACE.S \
	--asset units/dos.pas:SYSTEM/DOS.PAS \
	--asset units/dos.asm:SYSTEM/DOS.ASM \
	--asset units/dos.inc:SYSTEM/DOS.INC \
	--asset units/dos.npi:SYSTEM/DOS.NPI \
	--asset units/dos.s:SYSTEM/DOS.S \
	--asset units/graph.pas:SYSTEM/GRAPH.PAS \
	--asset units/graph.asm:SYSTEM/GRAPH.ASM \
	--asset units/graph.inc:SYSTEM/GRAPH.INC \
	--asset units/graph.npi:SYSTEM/GRAPH.NPI \
	--asset units/graph.s:SYSTEM/GRAPH.S \
	--asset units/overlay.pas:SYSTEM/OVERLAY.PAS \
	--asset units/overlay.asm:SYSTEM/OVERLAY.ASM \
	--asset units/overlayface.npi:SYSTEM/OVERLAYFACE.NPI \
	--asset units/overlayface.s:SYSTEM/OVERLAYFACE.S \
	--asset units/printer.pas:SYSTEM/PRINTER.PAS \
	--asset units/printer.asm:SYSTEM/PRINTER.ASM \
	--asset units/printer.npi:SYSTEM/PRINTER.NPI \
	--asset units/printer.s:SYSTEM/PRINTER.S \
	--asset units/strings.pas:SYSTEM/STRINGS.PAS \
	--asset units/strings.asm:SYSTEM/STRINGS.ASM \
	--asset units/strings.npi:SYSTEM/STRINGS.NPI \
	--asset units/strings.s:SYSTEM/STRINGS.S \
	--asset novapascal-help.txt:NPHELP.TXT \
	--asset $(NOVA_ASM)/libabi.inc:SYSTEM/LIBABI.INC \
	--asset $(NOVA_ASM)/libfiles.inc:SYSTEM/LIBFILES.INC \
	--asset $(NOVA_ASM)/libmemory.inc:SYSTEM/LIBMEMORY.INC \
	--asset $(NOVA_ASM)/libsound.inc:SYSTEM/LIBSOUND.INC \
	--asset $(NOVA_ASM)/libsystem.inc:SYSTEM/LIBSYSTEM.INC \
	--asset user-readme.txt:USER/README.TXT \
	--asset $(NPCFE):NPCFE.OVL \
	--asset $(BUILD_DIR)/NPEDIT.BIN:NPEDIT.BIN \
	--asset $(BUILD_DIR)/NPO2.BIN:NPO2.BIN \
	--asset $(BUILD_DIR)/NPPROJ.BIN:NPPROJ.BIN \
	--asset $(NAS):NAS.BIN \
	--asset $(NL):NL.BIN \
	--asset $(NBUILD):NBUILD.BIN \
	--asset $(NASPP_OVERLAY):NASPP.OVL \
	--asset $(NASBE_OVERLAY):NASBE.OVL \
	--asset $(NLWORK_OVERLAY):NLWORK.OVL \
	--asset $(PASCAL_LIBRARY):SYSTEM/PASCAL.NLIB \
	--asset-dir $(PASCAL_BINDING_DIR):SYSTEM
include ../../runtime/ndi/ndi.mk

$(NDI_OUTPUT): pascal-bindings

.PHONY: pascal-bindings
