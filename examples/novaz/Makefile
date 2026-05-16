CA65 ?= ca65
LD65 ?= ld65
DOTNET ?= dotnet

BUILD_DIR := build
DIST_DIR := dist
NOVA_LIB := ../../runtime/asm
PACKER := tools/Nova.NovaZ.Packer/Nova.NovaZ.Packer.csproj
SMOKE := tools/Nova.NovaZ.Smoke/Nova.NovaZ.Smoke.csproj
Z3_SPEC := tools/Nova.NovaZ.Z3SpecStory/Nova.NovaZ.Z3SpecStory.csproj
Z4_STYLE := tools/Nova.NovaZ.Z4StyleStory/Nova.NovaZ.Z4StyleStory.csproj
Z5_SPEC := tools/Nova.NovaZ.Z5SpecStory/Nova.NovaZ.Z5SpecStory.csproj
SMOKE_DLL := tools/Nova.NovaZ.Smoke/bin/Release/net10.0/Nova.NovaZ.Smoke.dll
PACKER_SRC := $(PACKER) tools/Nova.NovaZ.Packer/Program.cs
SMOKE_SRC := $(SMOKE) tools/Nova.NovaZ.Smoke/Program.cs
SMOKE_HOST_SRC := ../../e6502.Avalonia/Hardware/FileIoController.cs ../../e6502.Avalonia/Hardware/VgcConstants.cs
Z3_SPEC_SRC := $(Z3_SPEC) tools/Nova.NovaZ.Z3SpecStory/Program.cs
Z4_STYLE_SRC := $(Z4_STYLE) tools/Nova.NovaZ.Z4StyleStory/Program.cs
Z5_SPEC_SRC := $(Z5_SPEC) tools/Nova.NovaZ.Z5SpecStory/Program.cs
SMOKE_BUILD_ARGS ?= --no-restore /nr:false
SMOKE_ARGS ?=
PROJECT_SMOKE_ARGS ?= --generic-boot
AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin
RUNTIME := $(BUILD_DIR)/novaz.bin
Z3_SPEC_STORY := $(BUILD_DIR)/z3-spec.z3
Z3_SPEC_IMAGE := $(DIST_DIR)/z3-spec/fd0.ndi
Z4_STYLE_STORY := $(BUILD_DIR)/z4-styles.z4
Z4_STYLE_IMAGE := $(DIST_DIR)/z4-styles/fd0.ndi
Z5_SPEC_STORY := $(BUILD_DIR)/z5-spec.z5
Z5_SPEC_IMAGE := $(DIST_DIR)/z5-spec/fd0.ndi
PROJECT ?=
STORY_NAME ?= story.bin
RUNTIME_NAME ?= novaz.bin
INFOCOM_ROOT ?= /Volumes/Software/Emulation/Infocom
ZORK_I_STORY ?= $(INFOCOM_ROOT)/Zork 1 - The Great Underground Empire/Zork1.z3
ZORK_II_STORY ?= $(INFOCOM_ROOT)/Zork 2 - The Wizard of Frobozz/Zork2.z3
ZORK_III_STORY ?= $(INFOCOM_ROOT)/Zork 3 - The Dungeon Master/Zork3.z3
DEADLINE_STORY ?= $(INFOCOM_ROOT)/Deadline/Deadline.z3
AMFV_STORY ?= $(INFOCOM_ROOT)/A Mind Forever Voyaging/Amfv.z4
TRINITY_STORY ?= $(INFOCOM_ROOT)/Trinity/Trinity.z4
HHGG_STORY ?= $(INFOCOM_ROOT)/The Hitch Hikers Guide To The Galaxy/Hitchhik.z5
BORDER_ZONE_STORY ?= $(INFOCOM_ROOT)/Border Zone/Borderzo.z5
BEYOND_ZORK_STORY ?= $(INFOCOM_ROOT)/Beyond Zork - The Coconut of Quendor/BeyondZo.z5
SHERLOCK_STORY ?= $(INFOCOM_ROOT)/Sherlock - The Riddle of the Crown Jewels/Sherlock.z5
ZTUU_STORY ?= $(INFOCOM_ROOT)/Zork_ The Undiscovered Underground/ZTUU.Z5

ifneq ($(strip $(PROJECT)),)
PROJECT_DIR := projects/$(PROJECT)
include $(PROJECT_DIR)/project.mk

IMAGE ?= $(PROJECT_IMAGE)
LABEL ?= $(PROJECT_LABEL)
SIZE_KB ?= $(PROJECT_SIZE_KB)
STORY ?= $(PROJECT_STORY)
PROJECT_SMOKE ?= $(PROJECT_DIR)/smoke.txt
PROJECT_LONGPLAY ?= $(PROJECT_DIR)/longplay.txt
else
IMAGE ?= $(DIST_DIR)/fd0.ndi
LABEL ?= NOVAZ
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

$(RUNTIME): src/runtime.s src/runtime.cfg src/zstory.inc src/zstory.s src/ztext.inc src/ztext.s src/zobject.inc src/zobject.s src/zvm.inc src/zvm.s $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/dma.inc $(NOVA_LIB)/dma.s $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/blitter.s $(NOVA_LIB)/xram.inc $(NOVA_LIB)/xram.s $(NOVA_LIB)/pager.inc $(NOVA_LIB)/pager.s $(NOVA_LIB)/rng.inc $(NOVA_LIB)/rng.s $(NOVA_LIB)/vtext.inc $(NOVA_LIB)/vtext.s | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/runtime.o src/runtime.s
	$(LD65) -C src/runtime.cfg -m $(BUILD_DIR)/runtime.map -Ln $(BUILD_DIR)/runtime.sym -o $@ $(BUILD_DIR)/runtime.o

ndi: $(IMAGE)

$(SMOKE_DLL): $(SMOKE_SRC) $(SMOKE_HOST_SRC)
	$(DOTNET) build $(SMOKE) -c Release $(SMOKE_BUILD_ARGS)

smoke: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(SMOKE_ARGS)

test-zork-state: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-state-smoke.txt

test-zork-house: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-house-scroll-smoke.txt

test-zork-save: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-save-restore-smoke.txt

test-zork-torture: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --script tests/zork-i-torture-smoke.txt

$(Z3_SPEC_STORY): $(Z3_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z3_SPEC) -- --output "$@"

$(Z4_STYLE_STORY): $(Z4_STYLE_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z4_STYLE) -- --output "$@"

$(Z5_SPEC_STORY): $(Z5_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z5_SPEC) -- --output "$@"

test-z3-spec: $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) $(Z3_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z3_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z3SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z3_SPEC_STORY)"
	$(DOTNET) $(SMOKE_DLL) "$(Z3_SPEC_IMAGE)" --generic-boot --expect-screen "addrpaddrA-12" --expect-screen "abbr@" --expect-screen "upper window ok" --expect-screen "buffer off ok" --expect-screen "z3 window ok" --expect-screen "z3 spec ok" --skip-manifest-check "take red,key=>z3 input ok"

test-z4-styles: $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) $(Z4_STYLE_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z4_STYLE_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z4STYLE" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z4_STYLE_STORY)"
	$(DOTNET) $(SMOKE_DLL) "$(Z4_STYLE_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "style fixture done" --expect-text-color "normal text uses color 12=>0C" --expect-text-color "bold text uses white=>0F" --expect-text-color "bold phrase survives reset=>0F" --expect-text-color "reverse text uses color c0=>C0" --expect-text-color "bold reverse uses color f0=>F0" --expect-text-color "italic request is ignored=>0C" --expect-text-color "fixed request is ignored=>0C"

test-z5-spec: $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) $(Z5_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z5_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z5SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z5_SPEC_STORY)"
	$(DOTNET) $(SMOKE_DLL) "$(Z5_SPEC_IMAGE)" --generic-boot --no-status-line --skip-manifest-check --expect-screen "z5 spec ok" "take silver,key=>z5 input ok"

test-project: $(IMAGE) $(SMOKE_DLL)
	@if [ -z "$(PROJECT)" ]; then echo "PROJECT is required, e.g. PROJECT=zork-iii" >&2; exit 2; fi
	@if [ ! -f "$(PROJECT_SMOKE)" ]; then echo "project smoke script not found: $(PROJECT_SMOKE)" >&2; exit 2; fi
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(PROJECT_SMOKE_ARGS) --script "$(PROJECT_SMOKE)" $(SMOKE_ARGS)

test-project-longplay: $(IMAGE) $(SMOKE_DLL)
	@if [ -z "$(PROJECT)" ]; then echo "PROJECT is required, e.g. PROJECT=zork-iii" >&2; exit 2; fi
	@if [ ! -f "$(PROJECT_LONGPLAY)" ]; then echo "project longplay script not found: $(PROJECT_LONGPLAY)" >&2; exit 2; fi
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(PROJECT_SMOKE_ARGS) --script "$(PROJECT_LONGPLAY)" $(SMOKE_ARGS)

test-infocom-z3-smokes:
	$(MAKE) test-project PROJECT=zork-i STORY="$(ZORK_I_STORY)"
	$(MAKE) test-project PROJECT=zork-ii STORY="$(ZORK_II_STORY)"
	$(MAKE) test-project PROJECT=zork-iii STORY="$(ZORK_III_STORY)"
	$(MAKE) test-project PROJECT=deadline STORY="$(DEADLINE_STORY)"

test-infocom-v45-smokes:
	$(MAKE) test-project PROJECT=amfv STORY="$(AMFV_STORY)"
	$(MAKE) test-project PROJECT=trinity STORY="$(TRINITY_STORY)"
	$(MAKE) test-project PROJECT=hhgg STORY="$(HHGG_STORY)"
	$(MAKE) test-project PROJECT=beyond-zork STORY="$(BEYOND_ZORK_STORY)"
	$(MAKE) test-project PROJECT=border-zone STORY="$(BORDER_ZONE_STORY)"
	$(MAKE) test-project PROJECT=sherlock STORY="$(SHERLOCK_STORY)"
	$(MAKE) test-project PROJECT=ztuu STORY="$(ZTUU_STORY)"

test-infocom-smokes: test-infocom-z3-smokes test-infocom-v45-smokes

test-infocom-z3-longplays:
	$(MAKE) test-project-longplay PROJECT=zork-i STORY="$(ZORK_I_STORY)"
	$(MAKE) test-project-longplay PROJECT=deadline STORY="$(DEADLINE_STORY)"

test-infocom-v45-longplays:
	$(MAKE) test-project-longplay PROJECT=amfv STORY="$(AMFV_STORY)"
	$(MAKE) test-project-longplay PROJECT=trinity STORY="$(TRINITY_STORY)"
	$(MAKE) test-project-longplay PROJECT=hhgg STORY="$(HHGG_STORY)"
	$(MAKE) test-project-longplay PROJECT=border-zone STORY="$(BORDER_ZONE_STORY)"
	$(MAKE) test-project-longplay PROJECT=sherlock STORY="$(SHERLOCK_STORY)"
	$(MAKE) test-project-longplay PROJECT=ztuu STORY="$(ZTUU_STORY)"

test-infocom-longplays: test-infocom-z3-longplays test-infocom-v45-longplays

test-infocom-z5-longplays:
	$(MAKE) test-project-longplay PROJECT=hhgg STORY="$(HHGG_STORY)"
	$(MAKE) test-project-longplay PROJECT=border-zone STORY="$(BORDER_ZONE_STORY)"
	$(MAKE) test-project-longplay PROJECT=sherlock STORY="$(SHERLOCK_STORY)"
	$(MAKE) test-project-longplay PROJECT=ztuu STORY="$(ZTUU_STORY)"

$(IMAGE): FORCE $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) | $(DIST_DIR)
	mkdir -p "$(dir $(IMAGE))"
	$(DOTNET) run --project $(PACKER) -- $(PACK_ARGS)

FORCE:

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

.PHONY: all ndi smoke test-zork-state test-zork-house test-zork-save test-zork-torture test-z3-spec test-z4-styles test-z5-spec test-project test-project-longplay test-infocom-z3-smokes test-infocom-v45-smokes test-infocom-smokes test-infocom-z3-longplays test-infocom-v45-longplays test-infocom-longplays test-infocom-z5-longplays clean FORCE
