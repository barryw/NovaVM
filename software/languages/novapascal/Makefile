NOVA_ASM = ../../runtime/asm
TOOLCHAIN = ../../toolchain
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

BUILD_DIR = tools/build
TOOLCHAIN_BUILD = $(TOOLCHAIN)/build

TOOL_CFG = $(TOOLCHAIN)/tool.cfg
TOOL_IO = $(TOOLCHAIN_BUILD)/nptool_io.o
NPEDIT = $(BUILD_DIR)/NPEDIT.BIN
NAS = $(TOOLCHAIN_BUILD)/NAS.BIN
NL = $(TOOLCHAIN_BUILD)/NL.BIN
TOOL_BINS = $(NPEDIT) $(NAS) $(NL)
PASCAL_LIBRARY = $(BUILD_DIR)/PASCAL.NLIB
NASPP_OVERLAY = $(TOOLCHAIN_BUILD)/NASPP.OVL
NASBE_OVERLAY = $(TOOLCHAIN_BUILD)/NASBE.OVL
NLWORK_OVERLAY = $(TOOLCHAIN_BUILD)/NLWORK.OVL
TOOLCHAIN_ASSETS = $(NAS) $(NL) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY) $(TOOL_IO)

all: novapascal.bin $(TOOL_BINS) $(PASCAL_LIBRARY) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novapascal.bin: novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o novapascal.cfg $(NOVA_LIB_FILE)
	ld65 -C novapascal.cfg -m novapascal.map -o $@ novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o $(NOVA_LIB_FILE)

novapascal.o: novapascal.s shell.s $(TOOLCHAIN)/nptool.inc novapascal.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -l novapascal.lst -o $@ novapascal.s

$(BUILD_DIR):
	mkdir -p $@

$(TOOLCHAIN_ASSETS): toolchain

toolchain:
	$(MAKE) -C $(TOOLCHAIN) all

$(BUILD_DIR)/resident_io.o: $(TOOLCHAIN)/nptool_io.s $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ $(TOOLCHAIN)/nptool_io.s

$(BUILD_DIR)/npc_resident.o: tools/npc.s $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/xram.inc $(NOVA_ASM)/docbuf.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npc.s

$(BUILD_DIR)/npedit.o: tools/npedit.s $(TOOLCHAIN)/nptool.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libeditor.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ tools/npedit.s

$(NPEDIT): $(BUILD_DIR)/npedit.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npedit.map -o $@ $(BUILD_DIR)/npedit.o $(TOOL_IO)

$(BUILD_DIR)/pascal_runtime.o: pascal_runtime.s $(TOOLCHAIN)/nobj.inc $(NOVA_ASM)/nova.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -I $(NOVA_ASM) -o $@ pascal_runtime.s

$(PASCAL_LIBRARY): $(BUILD_DIR)/pascal_runtime.o pascal_runtime.cfg
	ld65 -C pascal_runtime.cfg -o $@ $(BUILD_DIR)/pascal_runtime.o

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
NDI_ASSET_DEPS := hello.pas hello.npp $(TOOL_BINS) $(PASCAL_LIBRARY) $(NASPP_OVERLAY) $(NASBE_OVERLAY) $(NLWORK_OVERLAY)
NDI_ASSETS := --asset hello.pas:HELLO.PAS --asset hello.npp:HELLO.NPP \
	--asset $(BUILD_DIR)/NPEDIT.BIN:NPEDIT.BIN \
	--asset $(NAS):NAS.BIN \
	--asset $(NL):NL.BIN \
	--asset $(NASPP_OVERLAY):NASPP.OVL \
	--asset $(NASBE_OVERLAY):NASBE.OVL \
	--asset $(NLWORK_OVERLAY):NLWORK.OVL \
	--asset $(PASCAL_LIBRARY):PASCAL.NLIB
include ../../runtime/ndi/ndi.mk
