CA65 ?= ca65
LD65 ?= ld65
AR65 ?= ar65
DOTNET ?= dotnet
PYTHON ?= python3
CA65FLAGS ?=
SMOKE_EXTRA ?=
SMOKE_SCREEN_EXPECTATIONS ?= --expect-screen "SELECT A MODE" --expect-screen "4  NETWORK GAME"
SMOKE_GFX_EXPECTATIONS ?= --expect-gfx-color "90,80=>0E" --expect-gfx-color "150,80=>03" --expect-gfx-color "170,75=>0D" --expect-gfx-color "220,85=>07"
RENDER_STEP_PAUSE ?= 1
RENDER_FINAL_PAUSE ?= 120
RENDER_PIXEL_STEP ?= 1
RENDER_REPEAT ?= 1
TEST_RENDER_STEP_PAUSE ?= 0
TEST_RENDER_FINAL_PAUSE ?= 0
TEST_RENDER_PIXEL_STEP ?= 11
TEST_RENDER_REPEAT ?= 0
RENDER_BUILD_DIR ?= build-render
RENDER_IMAGE ?= dist/render-test.ndi
MENU_BUILD_DIR ?= build-menu
MENU_IMAGE ?= dist/menu-test.ndi
MODE_BUILD_DIR ?= build-mode
MODE_IMAGE ?= dist/mode-test.ndi
MODE_GFX_EXPECTATIONS ?= --expect-gfx-color "64,162=>01"
NETWORK_GAME_SERVER_HOST ?= 127.0.0.1
NETWORK_GAME_SERVER_PORT ?= 16504
CASTLE_BUILD_DIR ?= build-castle
CASTLE_IMAGE ?= dist/castle-test.ndi

BUILD_DIR := build
DIST_DIR := dist
NOVA_LIB := ../../runtime/asm
PACKER := ../novaz/tools/Nova.NovaZ.Packer/Nova.NovaZ.Packer.csproj
OVERLAY_PACKER := ../../../e6502.Tools/e6502.Tools.csproj
OVERLAY_PACKER_DLL := ../../../e6502.Tools/bin/Release/net10.0/e6502.Tools.dll
OVERLAY_PACKER_BUILD_ARGS ?= -c Release -m:1 /p:SkipRomBuild=true /nr:false
SMOKE := ../novaz/tools/Nova.NovaZ.Smoke/Nova.NovaZ.Smoke.csproj
SMOKE_DLL := ../novaz/tools/Nova.NovaZ.Smoke/bin/Release/net10.0/Nova.NovaZ.Smoke.dll
SMOKE_SRC := $(SMOKE) ../novaz/tools/Nova.NovaZ.Smoke/Program.cs
SMOKE_BUILD_ARGS ?= --no-restore /nr:false /p:UseSharedCompilation=false
ENGINE_CA65FLAGS ?=

AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin
RUNTIME := $(BUILD_DIR)/novachess.bin
RUNTIME_SYM := $(BUILD_DIR)/runtime.sym
NOVA_LIB_FILE := $(NOVA_LIB)/build/nova.lib
ENGINE := $(BUILD_DIR)/CHESSENG.BIN
RULES_ENGINE := $(BUILD_DIR)/CHESSRUL.BIN
SPLASH_NVG := $(BUILD_DIR)/SPLASH.NVG
NETWORK_OVERLAY_PAYLOAD := $(BUILD_DIR)/netgame.bin
NETWORK_OVERLAY := $(BUILD_DIR)/NETGAME.OVL
NETWORK_OVERLAY_SYM := $(BUILD_DIR)/netgame.sym
NETWORK_KERNEL_LABELS := $(BUILD_DIR)/network_kernel_labels.inc
ENGINE_SYM := $(BUILD_DIR)/engine_nova.sym
RULES_ENGINE_SYM := $(BUILD_DIR)/rules_nova.sym
ENGINE_LABELS := $(BUILD_DIR)/engine_labels.inc
RULES_ENGINE_LABELS := $(BUILD_DIR)/rules_engine_labels.inc
IMAGE ?= $(DIST_DIR)/fd0.ndi
LABEL ?= NCHESS
SIZE_KB ?= 1440
RUNTIME_NAME ?= novachess.bin
ENGINE_NAME ?= CHESSENG.BIN
RULES_ENGINE_NAME ?= CHESSRUL.BIN
SPLASH_NVG_NAME ?= SPLASH.NVG
NETWORK_OVERLAY_NAME ?= NETGAME.OVL
SPLASH_SOURCE := assets/NovaChessSplashSource.png
SPLASH_SCRIPT := tools/make_splash_nvg.py
SPLASH_PREVIEW := $(BUILD_DIR)/SPLASH.png
SPLASH_PROJECT := $(BUILD_DIR)/NovaChessSplash.novadraw
ENGINE_VENDOR := vendor/chess6502-engine
ENGINE_VENDOR_SOURCES := $(shell find $(ENGINE_VENDOR)/src -name '*.s' -print)
ENGINE_SOURCES := src/engine_nova.s src/engine_platform_nova.s src/engine_nova.cfg $(ENGINE_VENDOR_SOURCES)
RULES_ENGINE_SOURCES := src/rules_nova.s src/engine_nova.cfg $(ENGINE_VENDOR_SOURCES)

all: $(AUTOBOOT) $(ENGINE_LABELS) $(RULES_ENGINE_LABELS) check-engine-abi $(RUNTIME) $(ENGINE) $(RULES_ENGINE) $(SPLASH_NVG) $(NETWORK_OVERLAY)

$(NOVA_LIB_FILE): FORCE
	$(MAKE) -C $(NOVA_LIB) clean-nova-lib build/nova.lib

$(AUTOBOOT): src/autoboot.s src/autoboot.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB_FILE) | $(BUILD_DIR)
	$(CA65) $(CA65FLAGS) --cpu w65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/autoboot.o src/autoboot.s
	$(LD65) -C src/autoboot.cfg -m $(BUILD_DIR)/autoboot.map -Ln $(BUILD_DIR)/autoboot.sym -o $@ $(BUILD_DIR)/autoboot.o $(NOVA_LIB_FILE)

$(RUNTIME): src/runtime.s src/runtime.cfg Pieces.inc $(ENGINE_LABELS) $(RULES_ENGINE_LABELS) $(NOVA_LIB)/nova.inc $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/vsprite.inc $(NOVA_LIB)/vtext.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/xram.inc $(NOVA_LIB)/pager.inc $(NOVA_LIB)/overlay.inc $(NOVA_LIB)/nvg.inc $(NOVA_LIB)/vgc_palette.inc $(NOVA_LIB)/tween.inc $(NOVA_LIB)/nui.inc $(NOVA_LIB_FILE) | $(BUILD_DIR)
	$(CA65) $(CA65FLAGS) --cpu w65c02 -I $(NOVA_LIB) -I $(BUILD_DIR) -o $(BUILD_DIR)/runtime.o src/runtime.s
	$(LD65) -C src/runtime.cfg -m $(BUILD_DIR)/runtime.map -Ln $(RUNTIME_SYM) -o $@ $(BUILD_DIR)/runtime.o $(NOVA_LIB_FILE)

$(NETWORK_KERNEL_LABELS): $(RUNTIME) | $(BUILD_DIR)
	awk 'BEGIN { map["start_game_screen"]="KERNEL_START_GAME_SCREEN"; map["update_clock"]="KERNEL_UPDATE_CLOCK"; map["set_status_network"]="KERNEL_SET_STATUS_NETWORK"; map["set_status_thinking"]="KERNEL_SET_STATUS_THINKING"; map["set_status_moving"]="KERNEL_SET_STATUS_MOVING"; map["set_status_invalid"]="KERNEL_SET_STATUS_INVALID"; map["wait_demo_frames"]="KERNEL_WAIT_DEMO_FRAMES"; map["prompt_human_move"]="KERNEL_PROMPT_HUMAN_MOVE"; map["commit_current_move"]="KERNEL_COMMIT_CURRENT_MOVE"; map["show_game_result"]="KERNEL_SHOW_GAME_RESULT"; map["input_set_idle_hook"]="KERNEL_INPUT_SET_IDLE_HOOK"; map["input_clear_idle_hook"]="KERNEL_INPUT_CLEAR_IDLE_HOOK"; map["self_from"]="KERNEL_SELF_FROM"; map["self_to"]="KERNEL_SELF_TO"; map["game_state"]="KERNEL_GAME_STATE"; } $$3 ~ /^\./ { sym = substr($$3, 2); if (sym in map) printf "%-36s = $$%s\n", map[sym], $$2; }' "$(RUNTIME_SYM)" > $@

$(NETWORK_OVERLAY_PAYLOAD): src/network_overlay.s src/network_overlay.cfg src/nchess_net.inc src/nchess_net.s $(NETWORK_KERNEL_LABELS) $(ENGINE_LABELS) $(NOVA_LIB)/nova.inc $(NOVA_LIB)/nic.inc $(NOVA_LIB)/gameserver.inc $(NOVA_LIB_FILE) | $(BUILD_DIR)
	$(CA65) $(CA65FLAGS) --cpu w65c02 -I $(NOVA_LIB) -I $(BUILD_DIR) -I src -o $(BUILD_DIR)/network_overlay.o src/network_overlay.s
	$(LD65) -C src/network_overlay.cfg -m $(BUILD_DIR)/netgame.map -Ln $(NETWORK_OVERLAY_SYM) -o $@ $(BUILD_DIR)/network_overlay.o $(NOVA_LIB_FILE)

$(OVERLAY_PACKER_DLL): FORCE
	$(DOTNET) build $(OVERLAY_PACKER) $(OVERLAY_PACKER_BUILD_ARGS)

$(NETWORK_OVERLAY): $(NETWORK_OVERLAY_PAYLOAD) $(OVERLAY_PACKER_DLL)
	$(DOTNET) "$(OVERLAY_PACKER_DLL)" pack-overlay --input "$(NETWORK_OVERLAY_PAYLOAD)" --output "$@" --load 0x6000 --max-size 0x3000 --sym "$(NETWORK_OVERLAY_SYM)" --main network_overlay_main --unload network_overlay_unload --bss-size "$$(awk '$$3 == ".__BSS_SIZE__" { print "0x" $$2 }' "$(NETWORK_OVERLAY_SYM)")" --module-id 2 --module-version 1

$(ENGINE): $(ENGINE_SOURCES) | $(BUILD_DIR)
	$(CA65) $(ENGINE_CA65FLAGS) --cpu w65c02 -I $(NOVA_LIB) -I src -I $(ENGINE_VENDOR)/src -I $(ENGINE_VENDOR)/src/engine -I $(ENGINE_VENDOR)/src/ai -o $(BUILD_DIR)/engine_nova.o src/engine_nova.s
	$(LD65) -C src/engine_nova.cfg -m $(BUILD_DIR)/engine_nova.map -Ln $(BUILD_DIR)/engine_nova.sym -o $@ $(BUILD_DIR)/engine_nova.o

$(RULES_ENGINE): $(RULES_ENGINE_SOURCES) | $(BUILD_DIR)
	$(CA65) $(ENGINE_CA65FLAGS) --cpu w65c02 -I $(NOVA_LIB) -I src -I $(ENGINE_VENDOR)/src -I $(ENGINE_VENDOR)/src/engine -I $(ENGINE_VENDOR)/src/ai -o $(BUILD_DIR)/rules_nova.o src/rules_nova.s
	$(LD65) -C src/engine_nova.cfg -m $(BUILD_DIR)/rules_nova.map -Ln $(BUILD_DIR)/rules_nova.sym -o $@ $(BUILD_DIR)/rules_nova.o

$(SPLASH_NVG): $(SPLASH_SOURCE) $(SPLASH_SCRIPT) | $(BUILD_DIR)
	$(PYTHON) "$(SPLASH_SCRIPT)" "$(SPLASH_SOURCE)" "$@" --format nvg --forbid-color 9 --normalize-family 5,13 --erase-rect 48,174,224,20 --preview "$(SPLASH_PREVIEW)" --novadraw "$(SPLASH_PROJECT)"

$(ENGINE_LABELS): $(ENGINE) | $(BUILD_DIR)
	awk 'BEGIN { map["nova_chess_engine_start"]="ENGINE_START"; map["nova_chess_init_piece_lists"]="ENGINE_CHESS_INIT_PIECE_LISTS"; map["nova_chess_generate_legal_moves"]="ENGINE_CHESS_GENERATE_LEGAL_MOVES"; map["nova_chess_find_best_move"]="ENGINE_CHESS_FIND_BEST_MOVE"; map["nova_chess_make_move"]="ENGINE_CHESS_MAKE_MOVE"; map["nova_chess_begin_game"]="ENGINE_CHESS_BEGIN_GAME"; map["nova_chess_commit_move"]="ENGINE_CHESS_COMMIT_MOVE"; map["nova_chess_unmake_move"]="ENGINE_CHESS_UNMAKE_MOVE"; map["nova_chess_is_square_attacked"]="ENGINE_CHESS_IS_SQUARE_ATTACKED"; map["nova_chess_check_king_in_check"]="ENGINE_CHESS_CHECK_KING_IN_CHECK"; map["nova_chess_record_position"]="ENGINE_CHESS_RECORD_POSITION"; map["nova_chess_clear_position_history"]="ENGINE_CHESS_CLEAR_POSITION_HISTORY"; map["nova_chess_check_repetition"]="ENGINE_CHESS_CHECK_REPETITION"; map["nova_chess_check_game_state"]="ENGINE_CHESS_CHECK_GAME_STATE"; map["nova_chess_tt_clear"]="ENGINE_TT_CLEAR"; map["Board88"]="ENGINE_BOARD88"; map["BestMoveFrom"]="ENGINE_BEST_MOVE_FROM"; map["BestMoveTo"]="ENGINE_BEST_MOVE_TO"; map["MoveCount"]="ENGINE_MOVE_COUNT"; map["MoveListFrom"]="ENGINE_MOVE_LIST_FROM"; map["MoveListTo"]="ENGINE_MOVE_LIST_TO"; map["currentplayer"]="ENGINE_CURRENTPLAYER"; map["difficulty"]="ENGINE_DIFFICULTY"; } $$3 ~ /^\./ { sym = substr($$3, 2); if (sym in map) printf "%-36s = $$%s\n", map[sym], $$2; }' "$(ENGINE_SYM)" > $@

$(RULES_ENGINE_LABELS): $(RULES_ENGINE) | $(BUILD_DIR)
	awk 'BEGIN { map["nova_chess_engine_start"]="ENGINE_START"; map["nova_chess_init_piece_lists"]="ENGINE_CHESS_INIT_PIECE_LISTS"; map["nova_chess_generate_legal_moves"]="ENGINE_CHESS_GENERATE_LEGAL_MOVES"; map["nova_chess_find_best_move"]="ENGINE_CHESS_FIND_BEST_MOVE"; map["nova_chess_make_move"]="ENGINE_CHESS_MAKE_MOVE"; map["nova_chess_begin_game"]="ENGINE_CHESS_BEGIN_GAME"; map["nova_chess_commit_move"]="ENGINE_CHESS_COMMIT_MOVE"; map["nova_chess_unmake_move"]="ENGINE_CHESS_UNMAKE_MOVE"; map["nova_chess_is_square_attacked"]="ENGINE_CHESS_IS_SQUARE_ATTACKED"; map["nova_chess_check_king_in_check"]="ENGINE_CHESS_CHECK_KING_IN_CHECK"; map["nova_chess_record_position"]="ENGINE_CHESS_RECORD_POSITION"; map["nova_chess_clear_position_history"]="ENGINE_CHESS_CLEAR_POSITION_HISTORY"; map["nova_chess_check_repetition"]="ENGINE_CHESS_CHECK_REPETITION"; map["nova_chess_check_game_state"]="ENGINE_CHESS_CHECK_GAME_STATE"; map["nova_chess_tt_clear"]="ENGINE_TT_CLEAR"; map["Board88"]="ENGINE_BOARD88"; map["MoveCount"]="ENGINE_MOVE_COUNT"; map["MoveListFrom"]="ENGINE_MOVE_LIST_FROM"; map["MoveListTo"]="ENGINE_MOVE_LIST_TO"; map["currentplayer"]="ENGINE_CURRENTPLAYER"; map["difficulty"]="ENGINE_DIFFICULTY"; } $$3 ~ /^\./ { sym = substr($$3, 2); if (sym in map) printf "%-36s = $$%s\n", map[sym], $$2; }' "$(RULES_ENGINE_SYM)" > $@

check-engine-abi: $(ENGINE_LABELS) $(RULES_ENGINE_LABELS)
	awk '$$1 ~ /^(ENGINE_START|ENGINE_CHESS_INIT_PIECE_LISTS|ENGINE_CHESS_GENERATE_LEGAL_MOVES|ENGINE_CHESS_FIND_BEST_MOVE|ENGINE_CHESS_MAKE_MOVE|ENGINE_CHESS_BEGIN_GAME|ENGINE_CHESS_COMMIT_MOVE|ENGINE_CHESS_UNMAKE_MOVE|ENGINE_CHESS_IS_SQUARE_ATTACKED|ENGINE_CHESS_CHECK_KING_IN_CHECK|ENGINE_CHESS_RECORD_POSITION|ENGINE_CHESS_CLEAR_POSITION_HISTORY|ENGINE_CHESS_CHECK_REPETITION|ENGINE_CHESS_CHECK_GAME_STATE|ENGINE_TT_CLEAR|ENGINE_BOARD88|ENGINE_MOVE_COUNT|ENGINE_MOVE_LIST_FROM|ENGINE_MOVE_LIST_TO|ENGINE_CURRENTPLAYER|ENGINE_DIFFICULTY)$$/' "$(ENGINE_LABELS)" > $(BUILD_DIR)/engine_shared_abi.inc
	awk '$$1 ~ /^(ENGINE_START|ENGINE_CHESS_INIT_PIECE_LISTS|ENGINE_CHESS_GENERATE_LEGAL_MOVES|ENGINE_CHESS_FIND_BEST_MOVE|ENGINE_CHESS_MAKE_MOVE|ENGINE_CHESS_BEGIN_GAME|ENGINE_CHESS_COMMIT_MOVE|ENGINE_CHESS_UNMAKE_MOVE|ENGINE_CHESS_IS_SQUARE_ATTACKED|ENGINE_CHESS_CHECK_KING_IN_CHECK|ENGINE_CHESS_RECORD_POSITION|ENGINE_CHESS_CLEAR_POSITION_HISTORY|ENGINE_CHESS_CHECK_REPETITION|ENGINE_CHESS_CHECK_GAME_STATE|ENGINE_TT_CLEAR|ENGINE_BOARD88|ENGINE_MOVE_COUNT|ENGINE_MOVE_LIST_FROM|ENGINE_MOVE_LIST_TO|ENGINE_CURRENTPLAYER|ENGINE_DIFFICULTY)$$/' "$(RULES_ENGINE_LABELS)" > $(BUILD_DIR)/rules_shared_abi.inc
	diff -u $(BUILD_DIR)/engine_shared_abi.inc $(BUILD_DIR)/rules_shared_abi.inc

ndi: $(IMAGE)

$(SMOKE_DLL): $(SMOKE_SRC)
	$(DOTNET) build $(SMOKE) -c Release $(SMOKE_BUILD_ARGS)

smoke: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --generic-boot --screen-only --skip-manifest-check $(SMOKE_SCREEN_EXPECTATIONS) $(SMOKE_GFX_EXPECTATIONS) $(SMOKE_EXTRA)

render-test:
	$(MAKE) smoke BUILD_DIR=$(RENDER_BUILD_DIR) IMAGE=$(RENDER_IMAGE) CA65FLAGS="-DCHESS_RENDER_TEST=1 -DCHESS_SELFPLAY_DEMO=0 -DDEMO_STEP_PAUSE=$(RENDER_STEP_PAUSE) -DDEMO_FINAL_PAUSE=$(RENDER_FINAL_PAUSE) -DDEMO_PIXEL_STEP=$(RENDER_PIXEL_STEP) -DDEMO_REPEAT=$(RENDER_REPEAT)" SMOKE_SCREEN_EXPECTATIONS='--expect-screen "NOVA CHESS" --expect-screen "STATUS" --expect-screen "MOVES"' SMOKE_GFX_EXPECTATIONS='--expect-gfx-color "10,6=>0F" --expect-gfx-color "32,6=>0C" --expect-gfx-color "19,188=>01" --expect-gfx-color "195,13=>01" --expect-gfx-color "107,101=>01" --expect-gfx-color "128,119=>01" --expect-gfx-color "132,120=>0F" --expect-gfx-color "152,74=>01" --expect-gfx-color "16,98=>01" --expect-gfx-color "174,74=>01" --expect-gfx-color "108,140=>01" --expect-gfx-color "107,79=>0B" --expect-gfx-color "110,79=>0C" --expect-gfx-color "128,53=>0B" --expect-gfx-color "132,54=>0C" --expect-gfx-color "152,96=>0B" --expect-gfx-color "16,76=>0B" --expect-gfx-color "174,96=>0B" --expect-gfx-color "108,30=>0B"'

menu-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(MENU_BUILD_DIR) IMAGE=$(MENU_IMAGE) CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"
	$(DOTNET) $(SMOKE_DLL) "$(MENU_IMAGE)" --generic-boot --screen-only --skip-manifest-check $(SMOKE_SCREEN_EXPECTATIONS) $(SMOKE_GFX_EXPECTATIONS)

mode-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(MODE_BUILD_DIR) IMAGE=$(MODE_IMAGE) CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"
	env NOVAZ_SMOKE_MAX_STEPS=160000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "1E" --skip-manifest-check --expect-screen "01.W" --expect-screen "01.B" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)
	env NOVAZ_SMOKE_MAX_STEPS=160000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "2EWE2E4\r" --skip-manifest-check --expect-screen "01.W E2-E4" --expect-screen "01.B" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)
	env NOVAZ_SMOKE_MAX_STEPS=120000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "3E2E4\rE7E5\r" --skip-manifest-check --expect-screen "01.W E2-E4" --expect-screen "01.B E7-E5" --expect-screen "TURN WHITE" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)
	env NOVAZ_SMOKE_MAX_STEPS=120000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "3A3" --skip-manifest-check --expect-screen "MOVE:A3" --expect-screen "BAD FROM SQUARE" --expect-gfx-color "10,116=>0A"
	env NOVAZ_SMOKE_MAX_STEPS=120000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "3E2E5" --skip-manifest-check --expect-screen "MOVE:E2-E5" --expect-screen "ILLEGAL MOVE" --expect-gfx-color "98,72=>0A"
	env NOVAZ_SMOKE_MAX_STEPS=160000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "3E2\x082E4\r" --skip-manifest-check --expect-screen "01.W E2-E4" --expect-screen "TURN BLACK" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)
	env NOVAZ_SMOKE_MAX_STEPS=120000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "1\x083" --skip-manifest-check --expect-screen "MOVE:" --expect-screen "TURN WHITE" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)
	env NOVAZ_SMOKE_MAX_STEPS=120000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "2E\x083" --skip-manifest-check --expect-screen "MOVE:" --expect-screen "TURN WHITE" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)

network-mode-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(MODE_BUILD_DIR) IMAGE=$(MODE_IMAGE) CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"
	env NOVA_GAME_SERVER_HOST="$(NETWORK_GAME_SERVER_HOST)" NOVA_GAME_SERVER_PORT="$(NETWORK_GAME_SERVER_PORT)" NOVAZ_SMOKE_MAX_STEPS=160000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "4E2E4\r" --skip-manifest-check --expect-screen "01.W E2-E4" --expect-screen "01.B" --expect-screen "STATUS" --expect-screen "MOVES" $(MODE_GFX_EXPECTATIONS)

network-error-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(MODE_BUILD_DIR) IMAGE=$(MODE_IMAGE) CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"
	env NOVA_GAME_SERVER_HOST="127.0.0.1" NOVA_GAME_SERVER_PORT="1" NOVAZ_SMOKE_MAX_STEPS=200000000 $(DOTNET) $(SMOKE_DLL) "$(MODE_IMAGE)" --generic-boot --screen-only --screen-input "4" --skip-manifest-check --expect-screen "NETWORK ERROR" --expect-screen "game server unavailable"

castle-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(CASTLE_BUILD_DIR) IMAGE=$(CASTLE_IMAGE) CA65FLAGS="-DCHESS_CASTLE_TEST=1 -DCHESS_SELFPLAY_DEMO=0 -DDEMO_STEP_PAUSE=0 -DDEMO_FINAL_PAUSE=0 -DDEMO_REPEAT=0"
	$(DOTNET) $(SMOKE_DLL) "$(CASTLE_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "NOVA CHESS" --expect-screen "STATUS" --expect-screen "MOVES" --expect-gfx-color "64,162=>01" --expect-gfx-color "82,164=>01"

test: smoke menu-test mode-test castle-test
	$(MAKE) render-test RENDER_STEP_PAUSE=$(TEST_RENDER_STEP_PAUSE) RENDER_FINAL_PAUSE=$(TEST_RENDER_FINAL_PAUSE) RENDER_PIXEL_STEP=$(TEST_RENDER_PIXEL_STEP) RENDER_REPEAT=$(TEST_RENDER_REPEAT)

$(IMAGE): $(AUTOBOOT) $(RUNTIME) $(ENGINE) $(RULES_ENGINE) $(SPLASH_NVG) $(NETWORK_OVERLAY) check-engine-abi $(PACKER)
	mkdir -p "$(dir $(IMAGE))"
	$(DOTNET) run --project $(PACKER) -- --output "$(IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --asset "$(ENGINE):$(ENGINE_NAME)" --asset "$(RULES_ENGINE):$(RULES_ENGINE_NAME)" --asset "$(SPLASH_NVG):$(SPLASH_NVG_NAME)" --asset "$(NETWORK_OVERLAY):$(NETWORK_OVERLAY_NAME)" --label "$(LABEL)" --size-kb "$(SIZE_KB)"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

FORCE:

clean:
	rm -rf $(BUILD_DIR) build-render $(DIST_DIR)

.PHONY: all ndi check-engine-abi smoke render-test menu-test mode-test network-mode-test network-error-test castle-test test clean FORCE
