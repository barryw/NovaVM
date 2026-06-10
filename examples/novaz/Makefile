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
Z4_SPEC := tools/Nova.NovaZ.Z4SpecStory/Nova.NovaZ.Z4SpecStory.csproj
Z5_SPEC := tools/Nova.NovaZ.Z5SpecStory/Nova.NovaZ.Z5SpecStory.csproj
Z6_SPEC := tools/Nova.NovaZ.Z6SpecStory/Nova.NovaZ.Z6SpecStory.csproj
JUKEBOX := tools/Nova.NovaZ.Jukebox/Nova.NovaZ.Jukebox.csproj
SMOKE_DLL := tools/Nova.NovaZ.Smoke/bin/Release/net10.0/Nova.NovaZ.Smoke.dll
PACKER_SRC := $(PACKER) tools/Nova.NovaZ.Packer/Program.cs tools/Nova.NovaZ.Packer/BlorbSounds.cs tools/Nova.NovaZ.Packer/BlorbPictures.cs
SMOKE_SRC := $(SMOKE) tools/Nova.NovaZ.Smoke/Program.cs
SMOKE_HOST_SRC := ../../e6502.Avalonia/Hardware/FileIoController.cs ../../e6502.Avalonia/Hardware/VgcConstants.cs
Z3_SPEC_SRC := $(Z3_SPEC) tools/Nova.NovaZ.Z3SpecStory/Program.cs
Z4_STYLE_SRC := $(Z4_STYLE) tools/Nova.NovaZ.Z4StyleStory/Program.cs
Z4_SPEC_SRC := $(Z4_SPEC) tools/Nova.NovaZ.Z4SpecStory/Program.cs
Z5_SPEC_SRC := $(Z5_SPEC) tools/Nova.NovaZ.Z5SpecStory/Program.cs
Z6_SPEC_SRC := $(Z6_SPEC) tools/Nova.NovaZ.Z6SpecStory/Program.cs
JUKEBOX_SRC := $(JUKEBOX) tools/Nova.NovaZ.Jukebox/Program.cs
SMOKE_BUILD_ARGS ?= --no-restore /nr:false
SMOKE_ARGS ?=
PROJECT_SMOKE_ARGS ?= --generic-boot
AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin
RUNTIME := $(BUILD_DIR)/novaz.bin
NOVAZ6_BIN := $(BUILD_DIR)/novaz6.bin
RUNTIME_ABI := $(BUILD_DIR)/runtime_abi.inc
# Explicit ROM ABI surface exported to the RAM-resident segment. Task 6+ grows
# this list; the awk rule fails the build if a symbol vanishes from the ROM.
RUNTIME_ABI_SYMS := zvm_operand_lo zvm_operand_hi zvm_operand_count zvm_fetch zvm_set_var zvm_value_lo zvm_value_hi zvm_branch_decode zvm_branch_apply zvm_branch_true zvm_branch_false zvm_unsupported zstory_version \
	nz_screen_flush_word zvm_window_current zvm_clear_whole_screen \
	vtext_clear_region vtext_set_cursor vtext_scroll_up VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS \
	zstory_addr_l zstory_addr_m zstory_addr_h zstory_word_lo zstory_word_hi zstory_write16 zstory_read16 zvm_stack_pop zvm_text_style pager_load_file_page xram_read8 zstory_read8 zstory_write8
Z3_SPEC_STORY := $(BUILD_DIR)/z3-spec.z3
Z3_SPEC_IMAGE := $(DIST_DIR)/z3-spec/fd0.ndi
Z4_STYLE_STORY := $(BUILD_DIR)/z4-styles.z4
Z4_STYLE_IMAGE := $(DIST_DIR)/z4-styles/fd0.ndi
Z4_SPEC_STORY := $(BUILD_DIR)/z4-spec.z4
Z4_SPEC_IMAGE := $(DIST_DIR)/z4-spec/fd0.ndi
Z5_SPEC_STORY := $(BUILD_DIR)/z5-spec.z5
Z5_SPEC_SOUND := $(BUILD_DIR)/z5-spec-sound.pak
Z5_SPEC_IMAGE := $(DIST_DIR)/z5-spec/fd0.ndi
Z6_SPEC_STORY := $(BUILD_DIR)/z6-spec.z6
Z6_SPEC_PICS := $(BUILD_DIR)/z6-spec-pics.pak
Z6_COLOURS_STORY := $(BUILD_DIR)/z6-colours.z6
Z6_SPEC_V7_STORY := $(BUILD_DIR)/z6-spec-v7.z6
Z6_MAIN_RET_STORY := $(BUILD_DIR)/z6-main-returns.z6
Z6_SPEC_IMAGE := $(DIST_DIR)/z6-spec/fd0.ndi
Z6_COLOURS_IMAGE := $(DIST_DIR)/z6-colours/fd0.ndi
Z6_SPEC_V7_IMAGE := $(DIST_DIR)/z6-spec-v7/fd0.ndi
Z6_SPEC_NOSEG_IMAGE := $(DIST_DIR)/z6-nosegment/fd0.ndi
Z6_MAIN_RET_IMAGE := $(DIST_DIR)/z6-main-returns/fd0.ndi
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
LURKING_STORY ?= $(INFOCOM_ROOT)/The Lurking Horror/Lurking.z3

# Sampled-sound Blorb archives (IF Archive: infocom/media/sound/, also shipped
# alongside the story files here). Override if yours live elsewhere; download
# missing ones with `curl -A Mozilla`.
LURKING_SOUNDS ?= $(INFOCOM_ROOT)/The Lurking Horror/Lurking.blb
SHERLOCK_SOUNDS ?= $(INFOCOM_ROOT)/Sherlock - The Riddle of the Crown Jewels/Sherlock.blb

ifneq ($(strip $(PROJECT)),)
PROJECT_DIR := projects/$(PROJECT)
include $(PROJECT_DIR)/project.mk

IMAGE ?= $(PROJECT_IMAGE)
LABEL ?= $(PROJECT_LABEL)
SIZE_KB ?= $(PROJECT_SIZE_KB)
STORY ?= $(PROJECT_STORY)
SOUNDS ?= $(PROJECT_SOUNDS)
PICTURES ?= $(PROJECT_PICTURES)
PROJECT_SMOKE ?= $(PROJECT_DIR)/smoke.txt
PROJECT_LONGPLAY ?= $(PROJECT_DIR)/longplay.txt
else
IMAGE ?= $(DIST_DIR)/fd0.ndi
LABEL ?= NOVAZ
SIZE_KB ?= 1440
endif

PACK_ARGS := --output "$(IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "$(LABEL)" --size-kb "$(SIZE_KB)" --story-name "$(STORY_NAME)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
ifneq ($(strip $(STORY)),)
PACK_ARGS += --story "$(STORY)"
endif
ifneq ($(strip $(SOUNDS)),)
PACK_ARGS += --sounds "$(SOUNDS)"
endif
ifneq ($(strip $(PICTURES)),)
PACK_ARGS += --pictures "$(PICTURES)"
endif

all: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN)

$(AUTOBOOT): src/autoboot.s src/autoboot.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/autoboot.o src/autoboot.s
	$(LD65) -C src/autoboot.cfg -m $(BUILD_DIR)/autoboot.map -Ln $(BUILD_DIR)/autoboot.sym -o $@ $(BUILD_DIR)/autoboot.o

$(RUNTIME): src/runtime.s src/runtime.cfg src/zstory.inc src/zstory.s src/ztext.inc src/ztext.s src/zobject.inc src/zobject.s src/zvm.inc src/zvm.s src/zvm6.inc $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/dma.inc $(NOVA_LIB)/dma.s $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/blitter.s $(NOVA_LIB)/xram.inc $(NOVA_LIB)/xram.s $(NOVA_LIB)/pager.inc $(NOVA_LIB)/pager.s $(NOVA_LIB)/rng.inc $(NOVA_LIB)/rng.s $(NOVA_LIB)/vtext.inc $(NOVA_LIB)/vtext.s | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -g -I $(NOVA_LIB) -o $(BUILD_DIR)/runtime.o src/runtime.s
	$(LD65) -C src/runtime.cfg -m $(BUILD_DIR)/runtime.map -Ln $(BUILD_DIR)/runtime.sym -o $@ $(BUILD_DIR)/runtime.o

# ROM ABI for the RAM-resident segment: pin each needed runtime label to its
# linked address. Requires ca65 -g on the runtime so unexported labels appear
# in the -Ln symbol file (lines: "al 00XXXX .symname", exported ones twice).
$(RUNTIME_ABI): $(RUNTIME) Makefile
	awk -v syms='$(RUNTIME_ABI_SYMS)' 'BEGIN { n = split(syms, want, " "); for (i = 1; i <= n; i++) need[want[i]] = 1 } \
	    $$1 == "al" { name = substr($$3, 2); if (name in need && !(name in done)) { done[name] = 1; printf "%s := $$%s\n", name, substr($$2, 3) } } \
	    END { for (i = 1; i <= n; i++) if (!(want[i] in done)) { printf "runtime_abi: symbol %s missing from runtime.sym\n", want[i] > "/dev/stderr"; exit 1 } }' \
	    $(BUILD_DIR)/runtime.sym > $@ || { rm -f $@; exit 1; }

$(NOVAZ6_BIN): src/zvm6.s src/zvm6.inc src/novaz6.cfg src/zstory.inc $(NOVA_LIB)/nova.inc $(NOVA_LIB)/xram.inc $(NOVA_LIB)/pager.inc $(RUNTIME_ABI) | $(BUILD_DIR)
	$(CA65) --cpu 65c02 -g -I $(NOVA_LIB) -I $(BUILD_DIR) -o $(BUILD_DIR)/zvm6.o src/zvm6.s
	$(LD65) -C src/novaz6.cfg -m $(BUILD_DIR)/novaz6.map -Ln $(BUILD_DIR)/novaz6.sym -o $@ $(BUILD_DIR)/zvm6.o

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

$(Z4_SPEC_STORY): $(Z4_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z4_SPEC) -- --output "$@"

$(Z5_SPEC_STORY): $(Z5_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z5_SPEC) -- --output "$@" --sound-pak "$(Z5_SPEC_SOUND)"

$(Z6_SPEC_STORY): $(Z6_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z6_SPEC) -- --output "$@" --pictures-pak "$(Z6_SPEC_PICS)"

$(Z6_COLOURS_STORY): $(Z6_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z6_SPEC) -- --output "$@" --fixture colours

$(Z6_SPEC_V7_STORY): $(Z6_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z6_SPEC) -- --output "$@" --version 7

$(Z6_MAIN_RET_STORY): $(Z6_SPEC_SRC) | $(BUILD_DIR)
	$(DOTNET) run --project $(Z6_SPEC) -- --output "$@" --main-returns

test-z3-spec: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z3_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z3_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z3SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z3_SPEC_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z3_SPEC_IMAGE)" --generic-boot --expect-screen "addrpaddrA-12" --expect-screen "abbr@" --expect-screen "upper window ok" --expect-screen "buffer off ok" --expect-screen "z3 window ok" --expect-screen "z3 spec ok" --skip-manifest-check "take red,key=>z3 input ok"

test-z4-styles: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z4_STYLE_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z4_STYLE_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z4STYLE" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z4_STYLE_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z4_STYLE_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "style fixture done" --expect-text-color "normal text uses color 12=>0C" --expect-text-color "bold text uses white=>0F" --expect-text-color "bold phrase survives reset=>0F" --expect-text-color "reverse text uses color c0=>C0" --expect-text-color "bold reverse uses color f0=>F0" --expect-text-color "italic request is ignored=>0C" --expect-text-color "fixed request is ignored=>0C" --expect-text-color "stacked styles use color f0=>F0"

test-z4-spec: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z4_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z4_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z4SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z4_SPEC_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z4_SPEC_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "z4 caps ok" --expect-screen "z4 timed ok" --expect-screen "z4 spec ok"

test-z5-spec: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z5_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z5_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z5SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z5_SPEC_STORY)" --asset "$(Z5_SPEC_SOUND):SOUND.PAK" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z5_SPEC_IMAGE)" --generic-boot --no-status-line --skip-manifest-check --expect-screen "z5 sound ok" --expect-screen "z5 spec ok" "take silver,key=>z5 input ok"

test-z6-spec: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z6_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z6_SPEC_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z6SPEC" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z6_SPEC_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN" --asset "$(Z6_SPEC_PICS):PICS.PAK"
	$(DOTNET) $(SMOKE_DLL) "$(Z6_SPEC_IMAGE)" --generic-boot --no-status-line --skip-manifest-check --expect-at "0,0=>BANNER" --expect-at "4,4=>SENT" --expect-at "5,5=>F06" --expect-at "5,43=>LONGTAILXYZ" --expect-at "5,44=>Z2 " --expect-at "5,45=>ZLAST ok" --expect-at "5,46=>    " --expect-at "5,47=>    " --expect-at "5,48=>    " --expect-at "5,49=>>" --expect-at "75,9=>BB" --expect-at "75,12=>EE" --expect-at "75,13=>  " "look=>z6 input ok"

test-z6-colours: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z6_COLOURS_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z6_COLOURS_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z6COLOUR" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z6_COLOURS_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z6_COLOURS_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "colour fixture done" \
	    --expect-text-color "egadef=>0F" --expect-text-color "egagrnblu=>12" --expect-text-color "egawhite=>0F" \
	    --expect-text-color "egaparch=>F0" --expect-text-color "egarev=>0F" --expect-text-color "egaroman=>F0" \
	    --expect-text-color "egabold=>F8" --expect-text-color "egakeep=>F0" --expect-text-color "egaredkeep=>F4" \
	    --expect-text-color "egadflt=>0F"

test-z6-v7-rejected: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z6_SPEC_V7_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z6_SPEC_V7_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z6SPECV7" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z6_SPEC_V7_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z6_SPEC_V7_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "UNSUPPORTED Z-MACHINE VERSION: 7"

# V6 frame-0 return: a main routine that returns must halt the machine
# cleanly with ZVM_STOP_QUIT (3) — not wedge through the bad-target path
# (which would leave ZVM_STOP_UNSUPPORTED, 2).
test-z6-main-returns: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(Z6_MAIN_RET_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z6_MAIN_RET_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z6MAINRT" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z6_MAIN_RET_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(Z6_MAIN_RET_IMAGE)" --generic-boot --skip-manifest-check --expect-stop 3

# Negative test: a v6 story packed WITHOUT the NOVAZ6.BIN asset must abort the
# boot with the runtime's missing-segment message. Deliberately the only
# packer invocation with no --asset "$(NOVAZ6_BIN):NOVAZ6.BIN".
test-z6-no-segment: $(AUTOBOOT) $(RUNTIME) $(PACKER_SRC) $(Z6_SPEC_STORY) $(SMOKE_DLL)
	$(DOTNET) run --project $(PACKER) -- --output "$(Z6_SPEC_NOSEG_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "Z6NOSEG" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(Z6_SPEC_STORY)"
	$(DOTNET) $(SMOKE_DLL) "$(Z6_SPEC_NOSEG_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "NOVAZ6.BIN MISSING OR INVALID"

# Aggregate V6 suite: positive spec story + the three negative/edge stories.
test-z6-suite: test-z6-spec test-z6-colours test-z6-v7-rejected test-z6-no-segment test-z6-main-returns

# Umbrella for every synthetic spec-story suite (no external story files needed).
test-spec-suites: test-z3-spec test-z4-styles test-z4-spec test-z5-spec test-z6-suite

# --- Sound jukebox (mount-and-run "Infocom sound museum" showcase images) ----
LURKING_SOUND_NUMS  := 3,4,6,7,8,9,10,11,12,13,15,16,17,18
SHERLOCK_SOUND_NUMS := 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17

# Headless jukebox check: boots, plays sound #3, then RETURN advances through
# the list and wraps. No external .blb needed (unknown numbers play silently),
# so this is CI-safe; it proves the jukebox loop and the sound_effect trigger.
JUKEBOX_TEST_STORY := $(BUILD_DIR)/jukebox-test.z3
JUKEBOX_TEST_IMAGE := $(DIST_DIR)/jukebox-test/fd0.ndi
test-jukebox: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(JUKEBOX_SRC) $(SMOKE_DLL)
	$(DOTNET) run --project $(JUKEBOX) -- --output "$(JUKEBOX_TEST_STORY)" --title "Test Jukebox" --numbers "3,4,5"
	$(DOTNET) run --project $(PACKER) -- --output "$(JUKEBOX_TEST_IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "JUKETEST" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(JUKEBOX_TEST_STORY)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"
	$(DOTNET) $(SMOKE_DLL) "$(JUKEBOX_TEST_IMAGE)" --generic-boot --no-status-line --skip-manifest-check --expect-screen "Sound #3" "go=>Sound #4" "go=>Sound #5" "go=>Sound #3"

jukebox-lurking: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(JUKEBOX_SRC)
	$(DOTNET) run --project $(JUKEBOX) -- --output "$(BUILD_DIR)/jukebox-lurking.z3" --title "Lurking Horror Sounds" --numbers "$(LURKING_SOUND_NUMS)"
	$(DOTNET) run --project $(PACKER) -- --output "$(DIST_DIR)/jukebox-lurking/fd0.ndi" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "LURKJUKE" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(BUILD_DIR)/jukebox-lurking.z3" --sounds "$(LURKING_SOUNDS)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"

jukebox-sherlock: $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) $(JUKEBOX_SRC)
	$(DOTNET) run --project $(JUKEBOX) -- --output "$(BUILD_DIR)/jukebox-sherlock.z3" --title "Sherlock Sounds" --numbers "$(SHERLOCK_SOUND_NUMS)"
	$(DOTNET) run --project $(PACKER) -- --output "$(DIST_DIR)/jukebox-sherlock/fd0.ndi" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --label "SHERJUKE" --size-kb "1440" --story-name "$(STORY_NAME)" --story "$(BUILD_DIR)/jukebox-sherlock.z3" --sounds "$(SHERLOCK_SOUNDS)" --asset "$(NOVAZ6_BIN):NOVAZ6.BIN"

# Build the full clearly-named Infocom collection (one .ndi per game) into
# dist/collection/, auto-discovering supported games under $(INFOCOM_ROOT),
# bundling sound where present, and smoke-testing each image.
collection:
	INFOCOM_ROOT="$(INFOCOM_ROOT)" tools/build-infocom-collection.py $(COLLECTION_ARGS)

test-project: $(IMAGE) $(SMOKE_DLL)
	@if [ -z "$(PROJECT)" ]; then echo "PROJECT is required, e.g. PROJECT=zork-iii" >&2; exit 2; fi
	@if [ ! -f "$(PROJECT_SMOKE)" ]; then echo "project smoke script not found: $(PROJECT_SMOKE)" >&2; exit 2; fi
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(PROJECT_SMOKE_ARGS) --script "$(PROJECT_SMOKE)" $(SMOKE_ARGS)

# One-off NZ6 dispatch capture (M2 window-model research): runs the project's
# capture.txt with the dispatch trace enabled. Per-instruction sampling makes
# this slower than test-project; it is not a test target. Output log:
# build/nz6-trace-$(PROJECT).log (see docs/plans/2026-06-10-zork-zero-window-ops.md).
capture-z6-trace: $(IMAGE) $(SMOKE_DLL)
	@if [ -z "$(PROJECT)" ]; then echo "PROJECT is required, e.g. PROJECT=zork-zero" >&2; exit 2; fi
	@if [ ! -f "$(PROJECT_DIR)/capture.txt" ]; then echo "capture script not found: $(PROJECT_DIR)/capture.txt" >&2; exit 2; fi
	NOVAZ_SMOKE_TRACE_NZ6="$(BUILD_DIR)/nz6-trace-$(PROJECT).log" $(DOTNET) $(SMOKE_DLL) "$(IMAGE)" $(PROJECT_SMOKE_ARGS) --script "$(PROJECT_DIR)/capture.txt" $(SMOKE_ARGS)

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

$(IMAGE): FORCE $(AUTOBOOT) $(RUNTIME) $(NOVAZ6_BIN) $(PACKER_SRC) | $(DIST_DIR)
	mkdir -p "$(dir $(IMAGE))"
	$(DOTNET) run --project $(PACKER) -- $(PACK_ARGS)

FORCE:

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

.PHONY: all ndi smoke collection test-zork-state test-zork-house test-zork-save test-zork-torture test-z3-spec test-z4-styles test-z4-spec test-z5-spec test-z6-spec test-z6-v7-rejected test-z6-no-segment test-z6-main-returns test-z6-suite test-spec-suites test-jukebox jukebox-lurking jukebox-sherlock test-project test-project-longplay test-infocom-z3-smokes test-infocom-v45-smokes test-infocom-smokes test-infocom-z3-longplays test-infocom-v45-longplays test-infocom-longplays test-infocom-z5-longplays clean FORCE
