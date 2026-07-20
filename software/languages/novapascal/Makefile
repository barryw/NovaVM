NOVA_ASM = ../../runtime/asm
TOOLCHAIN = ../../toolchain
NOVA_LIB_FILE = $(NOVA_ASM)/build/nova.lib
RESOURCES = ../../../e6502.Avalonia/Resources

BUILD_DIR = tools/build

TOOL_CFG = tools/tool.cfg
TOOL_IO = $(BUILD_DIR)/nptool_io.o
TOOL_BINS = $(BUILD_DIR)/NPEDIT.BIN $(BUILD_DIR)/NAS.BIN $(BUILD_DIR)/NL.BIN
PASCAL_LIBRARY = $(BUILD_DIR)/PASCAL.NLIB

all: novapascal.bin $(TOOL_BINS) $(PASCAL_LIBRARY)

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_ASM) clean-nova-lib build/nova.lib

novapascal.bin: novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o novapascal.cfg $(NOVA_LIB_FILE)
	ld65 -C novapascal.cfg -m novapascal.map -o $@ novapascal.o $(BUILD_DIR)/npc_resident.o $(BUILD_DIR)/resident_io.o $(NOVA_LIB_FILE)

novapascal.o: novapascal.s shell.s nptool.inc novapascal.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc $(NOVA_ASM)/fio.inc $(NOVA_ASM)/replline.inc
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -l novapascal.lst -o $@ novapascal.s

$(BUILD_DIR):
	mkdir -p $@

$(TOOL_IO): tools/nptool_io.s nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -D NPTOOL_BIN -I . -I $(NOVA_ASM) -o $@ tools/nptool_io.s

$(BUILD_DIR)/resident_io.o: tools/nptool_io.s nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $@ tools/nptool_io.s

$(BUILD_DIR)/npc_resident.o: tools/npc.s nptool.inc $(NOVA_ASM)/xram.inc $(NOVA_ASM)/docbuf.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $@ tools/npc.s

$(BUILD_DIR)/npedit.o: tools/npedit.s nptool.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libeditor.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $@ tools/npedit.s

$(BUILD_DIR)/novaasm_tool.o: tools/novaasm_tool.s nptool.inc $(TOOLCHAIN)/novaasm.inc $(TOOLCHAIN)/nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -o $@ tools/novaasm_tool.s

$(BUILD_DIR)/novaasm_core.o: $(TOOLCHAIN)/novaasm.s $(TOOLCHAIN)/novaasm.inc $(TOOLCHAIN)/nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -o $@ $(TOOLCHAIN)/novaasm.s

$(BUILD_DIR)/novalink_tool.o: tools/novalink_tool.s nptool.inc $(TOOLCHAIN)/novalink.inc $(TOOLCHAIN)/nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(TOOLCHAIN) -o $@ tools/novalink_tool.s

$(BUILD_DIR)/novalink_core.o: $(TOOLCHAIN)/novalink.s $(TOOLCHAIN)/novalink.inc $(TOOLCHAIN)/nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I $(TOOLCHAIN) -o $@ $(TOOLCHAIN)/novalink.s

$(BUILD_DIR)/NPEDIT.BIN: $(BUILD_DIR)/npedit.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/npedit.map -o $@ $(BUILD_DIR)/npedit.o $(TOOL_IO)

$(BUILD_DIR)/NAS.BIN: $(BUILD_DIR)/novaasm_tool.o $(BUILD_DIR)/novaasm_core.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/novaasm.map -o $@ $(BUILD_DIR)/novaasm_tool.o $(BUILD_DIR)/novaasm_core.o $(TOOL_IO)

$(BUILD_DIR)/NL.BIN: $(BUILD_DIR)/novalink_tool.o $(BUILD_DIR)/novalink_core.o $(TOOL_IO) $(TOOL_CFG)
	ld65 -C $(TOOL_CFG) -m $(BUILD_DIR)/novalink.map -o $@ $(BUILD_DIR)/novalink_tool.o $(BUILD_DIR)/novalink_core.o $(TOOL_IO)

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

.PHONY: clean all install FORCE

# Bootable language floppy: AUTOBOOT loads the shell ROM into the primary bank.
NDI_RUNTIME := novapascal.bin
NDI_OUTPUT  := novapascal.ndi
NDI_LABEL   := NPASCAL
NDI_ASSET_DEPS := hello.pas hello.npp $(TOOL_BINS) $(PASCAL_LIBRARY)
NDI_ASSETS := --asset hello.pas:HELLO.PAS --asset hello.npp:HELLO.NPP \
	--asset $(BUILD_DIR)/NPEDIT.BIN:NPEDIT.BIN \
	--asset $(BUILD_DIR)/NAS.BIN:NAS.BIN \
	--asset $(BUILD_DIR)/NL.BIN:NL.BIN \
	--asset $(PASCAL_LIBRARY):PASCAL.NLIB
include ../../runtime/ndi/ndi.mk
