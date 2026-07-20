NOVA_ASM = ../runtime/asm
BUILD_DIR = build

NAS = $(BUILD_DIR)/NAS.BIN
NL = $(BUILD_DIR)/NL.BIN
NASPP_PAYLOAD = $(BUILD_DIR)/naspp.bin
NASPP = $(BUILD_DIR)/NASPP.OVL
NASBE_PAYLOAD = $(BUILD_DIR)/nasbe.bin
NASBE = $(BUILD_DIR)/NASBE.OVL
NLWORK_PAYLOAD = $(BUILD_DIR)/nlwork.bin
NLWORK = $(BUILD_DIR)/NLWORK.OVL
OVERLAY_PACKER = ../../e6502.Tools/e6502.Tools.csproj
OVERLAY_PACKER_DLL = ../../e6502.Tools/bin/Release/net10.0/e6502.Tools.dll

all: $(NAS) $(NL) $(NASPP) $(NASBE) $(NLWORK)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/nptool_io.o: nptool_io.s nptool.inc $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libfiles.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -D NPTOOL_BIN -I . -I $(NOVA_ASM) -o $@ nptool_io.s

$(BUILD_DIR)/novaasm_tool.o: novaasm_tool.s nas_preprocessor.inc nas_project.inc nas_backend.inc nas_core.inc nptool.inc novaasm.inc nobj.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/libsystem.inc $(NOVA_ASM)/xram.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $@ novaasm_tool.s

$(BUILD_DIR)/novaasm_core.o: novaasm.s novaasm.inc nas_backend.inc nas_core.inc nobj.inc $(NOVA_ASM)/nova.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -D NASM_CORE_OVERLAY -I . -I $(NOVA_ASM) -o $@ novaasm.s

$(BUILD_DIR)/novalink_tool.o: novalink_tool.s nptool.inc novalink.inc nl_worker.inc nobj.inc $(NOVA_ASM)/libsystem.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $@ novalink_tool.s

$(BUILD_DIR)/novalink_core.o: novalink.s novalink.inc nl_worker.inc nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -o $@ novalink.s

$(NAS): $(BUILD_DIR)/novaasm_tool.o $(BUILD_DIR)/nptool_io.o tool.cfg
	ld65 -C tool.cfg -m $(BUILD_DIR)/novaasm.map -o $@ $(BUILD_DIR)/novaasm_tool.o $(BUILD_DIR)/nptool_io.o

$(NL): $(BUILD_DIR)/novalink_tool.o $(BUILD_DIR)/novalink_core.o $(BUILD_DIR)/nptool_io.o tool.cfg
	ld65 -C tool.cfg -m $(BUILD_DIR)/novalink.map -o $@ $(BUILD_DIR)/novalink_tool.o $(BUILD_DIR)/novalink_core.o $(BUILD_DIR)/nptool_io.o

$(NASPP_PAYLOAD): nas_preprocessor.s nas_preprocessor.inc overlay.cfg $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -I $(NOVA_ASM) -o $(BUILD_DIR)/nas_preprocessor.o nas_preprocessor.s
	ld65 -C overlay.cfg -m $(BUILD_DIR)/naspp.map -Ln $(BUILD_DIR)/naspp.sym -o $@ $(BUILD_DIR)/nas_preprocessor.o

$(BUILD_DIR)/nas_backend.o: nas_backend.s nas_backend.inc nas_core.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -D NASBE_INTEGRATED -I . -I $(NOVA_ASM) -o $@ nas_backend.s

$(NASBE_PAYLOAD): $(BUILD_DIR)/novaasm_core.o $(BUILD_DIR)/nas_backend.o overlay.cfg
	ld65 -C overlay.cfg -m $(BUILD_DIR)/nasbe.map -Ln $(BUILD_DIR)/nasbe.sym -o $@ $(BUILD_DIR)/novaasm_core.o $(BUILD_DIR)/nas_backend.o

$(BUILD_DIR)/nl_worker.o: nl_worker.s nl_worker.inc novalink.inc nobj.inc | $(BUILD_DIR)
	ca65 --cpu w65c02 -I . -o $@ nl_worker.s

$(NLWORK_PAYLOAD): $(BUILD_DIR)/nl_worker.o overlay.cfg
	ld65 -C overlay.cfg -m $(BUILD_DIR)/nlwork.map -Ln $(BUILD_DIR)/nlwork.sym -o $@ $(BUILD_DIR)/nl_worker.o

$(OVERLAY_PACKER_DLL): FORCE
	dotnet build $(OVERLAY_PACKER) -c Release -m:1 /p:SkipRomBuild=true /nr:false

$(NASPP): $(NASPP_PAYLOAD) $(OVERLAY_PACKER_DLL)
	dotnet "$(OVERLAY_PACKER_DLL)" pack-overlay --input "$(NASPP_PAYLOAD)" --output "$@" --load 0x7000 --max-size 0x3000 --sym "$(BUILD_DIR)/naspp.sym" --main naspp_main --bss-size "$$(awk '$$3 == ".__BSS_SIZE__" { print "0x" $$2 }' "$(BUILD_DIR)/naspp.sym")" --module-id 0x4E50 --module-version 1

$(NASBE): $(NASBE_PAYLOAD) $(OVERLAY_PACKER_DLL)
	dotnet "$(OVERLAY_PACKER_DLL)" pack-overlay --input "$(NASBE_PAYLOAD)" --output "$@" --load 0x7000 --max-size 0x3000 --sym "$(BUILD_DIR)/nasbe.sym" --main nasm_assemble --bss-size "$$(awk '$$3 == ".__BSS_SIZE__" { print "0x" $$2 }' "$(BUILD_DIR)/nasbe.sym")" --module-id 0x4E42 --module-version 1

$(NLWORK): $(NLWORK_PAYLOAD) $(OVERLAY_PACKER_DLL)
	dotnet "$(OVERLAY_PACKER_DLL)" pack-overlay --input "$(NLWORK_PAYLOAD)" --output "$@" --load 0x7000 --max-size 0x3000 --sym "$(BUILD_DIR)/nlwork.sym" --main nlw_main --bss-size "$$(awk '$$3 == ".__BSS_SIZE__" { print "0x" $$2 }' "$(BUILD_DIR)/nlwork.sym")" --module-id 0x4E4C --module-version 1

clean:
	rm -rf $(BUILD_DIR)

FORCE:

.PHONY: all clean FORCE
