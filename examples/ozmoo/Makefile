CA65 ?= ca65
LD65 ?= ld65
DOTNET ?= dotnet

BUILD_DIR := build
DIST_DIR := dist
NOVA_LIB := ../../ehbasic/lib
PACKER := tools/Nova.Ozmoo.Packer/Nova.Ozmoo.Packer.csproj
SMOKE := tools/Nova.Ozmoo.Smoke/Nova.Ozmoo.Smoke.csproj
SMOKE_DLL := tools/Nova.Ozmoo.Smoke/bin/Release/net10.0/Nova.Ozmoo.Smoke.dll
PACKER_SRC := $(PACKER) tools/Nova.Ozmoo.Packer/Program.cs
SMOKE_SRC := $(SMOKE) tools/Nova.Ozmoo.Smoke/Program.cs
SMOKE_BUILD_ARGS ?= --no-restore
SMOKE_ARGS ?=
PROJECT_SMOKE_ARGS ?= --generic-boot
AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin
RUNTIME := $(BUILD_DIR)/ozmoo.bin
PROJECT ?=
STORY_NAME ?= story.bin
RUNTIME_NAME ?= ozmoo.bin

ifneq ($(strip $(PROJECT)),)
PROJECT_DIR := projects/$(PROJECT)
include $(PROJECT_DIR)/project.mk

IMAGE ?= $(PROJECT_IMAGE)
LABEL ?= $(PROJECT_LABEL)
SIZE_KB ?= $(PROJECT_SIZE_KB)
STORY ?= $(PROJECT_STORY)
PROJECT_SMOKE ?= $(PROJECT_DIR)/smoke.txt
else
IMAGE ?= $(DIST_DIR)/fd0.ndi
LABEL ?= OZMOO
SIZE_KB ?= 1440
endif

PACK_ARGS := --output "$(IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "$(LABEL)" --size-kb "$(SIZE_KB)" --story-name "$(STORY_NAME)"
ifneq ($(strip $(STORY)),)
PACK_ARGS += --story "$(STORY)"
endif

all: $(AUTOBOOT) $(RUNTIME)

$(AUTOBOOT): src/autoboot.s src/autoboot.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/autoboot.o src/autoboot.s
	$(LD65) -C src/autoboot.cfg -m $(BUILD_DIR)/autoboot.map -Ln $(BUILD_DIR)/autoboot.sym -o $@ $(BUILD_DIR)/autoboot.o

$(RUNTIME): src/runtime.s src/runtime.cfg src/zstory.inc src/zstory.s src/ztext.inc src/ztext.s src/zobject.inc src/zobject.s src/zvm.inc src/zvm.s $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/dma.inc $(NOVA_LIB)/dma.s $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/blitter.s $(NOVA_LIB)/xram.inc $(NOVA_LIB)/xram.s $(NOVA_LIB)/vtext.inc $(NOVA_LIB)/vtext.s | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/runtime.o src/runtime.s
	$(LD65) -C src/runtime.cfg -m $(BUILD_DIR)/runtime.map -Ln $(BUILD_DIR)/runtime.sym -o $@ $(BUILD_DIR)/runtime.o

ndi: $(IMAGE)

$(SMOKE_DLL): $(SMOKE_SRC)
	$(DOTNET) build $(SMOKE) -c Release $(SMOKE_BUILD_ARGS)

smoke: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(SMOKE_ARGS)

test-zork-state: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-state-smoke.txt

test-zork-house: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-house-scroll-smoke.txt

test-project: $(IMAGE) $(SMOKE_DLL)
	@if [ -z "$(PROJECT)" ]; then echo "PROJECT is required, e.g. PROJECT=zork-iii" >&2; exit 2; fi
	@if [ ! -f "$(PROJECT_SMOKE)" ]; then echo "project smoke script not found: $(PROJECT_SMOKE)" >&2; exit 2; fi
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(PROJECT_SMOKE_ARGS) --script "$(PROJECT_SMOKE)" $(SMOKE_ARGS)

$(IMAGE): $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) | $(DIST_DIR)
	$(DOTNET) run --project $(PACKER) -- $(PACK_ARGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

.PHONY: all ndi smoke test-zork-state test-zork-house test-project clean
