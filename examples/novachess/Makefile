CA65 ?= ca65
LD65 ?= ld65
DOTNET ?= dotnet
CA65FLAGS ?=
SMOKE_EXTRA ?=
SMOKE_GFX_EXPECTATIONS ?= --expect-gfx-color "10,6=>0F" --expect-gfx-color "32,6=>0C" --expect-gfx-color "6,2=>0F" --expect-gfx-color "8,4=>0B" --expect-gfx-color "19,188=>01" --expect-gfx-color "195,13=>01"
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
CASTLE_BUILD_DIR ?= build-castle
CASTLE_IMAGE ?= dist/castle-test.ndi

BUILD_DIR := build
DIST_DIR := dist
NOVA_LIB := ../../ehbasic/lib
PACKER := ../novaz/tools/Nova.NovaZ.Packer/Nova.NovaZ.Packer.csproj
SMOKE := ../novaz/tools/Nova.NovaZ.Smoke/Nova.NovaZ.Smoke.csproj
SMOKE_DLL := ../novaz/tools/Nova.NovaZ.Smoke/bin/Release/net10.0/Nova.NovaZ.Smoke.dll
SMOKE_SRC := $(SMOKE) ../novaz/tools/Nova.NovaZ.Smoke/Program.cs
SMOKE_BUILD_ARGS ?= --no-restore /nr:false /p:UseSharedCompilation=false
ENGINE_CA65FLAGS ?=

AUTOBOOT := $(BUILD_DIR)/AUTOBOOT.bin
RUNTIME := $(BUILD_DIR)/novachess.bin
ENGINE := $(BUILD_DIR)/CHESSENG.BIN
ENGINE_SYM := $(BUILD_DIR)/engine_nova.sym
ENGINE_LABELS := $(BUILD_DIR)/engine_labels.inc
IMAGE ?= $(DIST_DIR)/fd0.ndi
LABEL ?= NCHESS
SIZE_KB ?= 1440
RUNTIME_NAME ?= novachess.bin
ENGINE_NAME ?= CHESSENG.BIN
ENGINE_VENDOR := vendor/chess6502-engine
ENGINE_VENDOR_SOURCES := $(shell find $(ENGINE_VENDOR)/src -name '*.s' -print)
ENGINE_SOURCES := src/engine_nova.s src/engine_platform_nova.s src/engine_nova.cfg $(ENGINE_VENDOR_SOURCES)

all: $(AUTOBOOT) $(ENGINE_LABELS) $(RUNTIME) $(ENGINE)

$(AUTOBOOT): src/autoboot.s src/autoboot.cfg $(NOVA_LIB)/nova.inc $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s | $(BUILD_DIR)
	$(CA65) $(CA65FLAGS) --cpu 65c02 -I $(NOVA_LIB) -o $(BUILD_DIR)/autoboot.o src/autoboot.s
	$(LD65) -C src/autoboot.cfg -m $(BUILD_DIR)/autoboot.map -Ln $(BUILD_DIR)/autoboot.sym -o $@ $(BUILD_DIR)/autoboot.o

$(RUNTIME): src/runtime.s src/runtime.cfg $(ENGINE_LABELS) $(NOVA_LIB)/nova.inc $(NOVA_LIB)/blitter.inc $(NOVA_LIB)/blitter.s $(NOVA_LIB)/vsprite.inc $(NOVA_LIB)/vsprite.s $(NOVA_LIB)/vtext.inc $(NOVA_LIB)/vtext.s $(NOVA_LIB)/fio.inc $(NOVA_LIB)/fio.s $(NOVA_LIB)/tween.inc $(NOVA_LIB)/tween.s | $(BUILD_DIR)
	$(CA65) $(CA65FLAGS) --cpu 65c02 -I $(NOVA_LIB) -I $(BUILD_DIR) -o $(BUILD_DIR)/runtime.o src/runtime.s
	$(LD65) -C src/runtime.cfg -m $(BUILD_DIR)/runtime.map -Ln $(BUILD_DIR)/runtime.sym -o $@ $(BUILD_DIR)/runtime.o

$(ENGINE): $(ENGINE_SOURCES) | $(BUILD_DIR)
	$(CA65) $(ENGINE_CA65FLAGS) --cpu 65c02 -I $(NOVA_LIB) -I src -I $(ENGINE_VENDOR)/src -I $(ENGINE_VENDOR)/src/engine -I $(ENGINE_VENDOR)/src/ai -o $(BUILD_DIR)/engine_nova.o src/engine_nova.s
	$(LD65) -C src/engine_nova.cfg -m $(BUILD_DIR)/engine_nova.map -Ln $(BUILD_DIR)/engine_nova.sym -o $@ $(BUILD_DIR)/engine_nova.o

$(ENGINE_LABELS): $(ENGINE) | $(BUILD_DIR)
	awk 'BEGIN { map["chess_engine_start"]="ENGINE_START"; map["ChessInitPieceLists"]="ENGINE_CHESS_INIT_PIECE_LISTS"; map["ChessGenerateLegalMoves"]="ENGINE_CHESS_GENERATE_LEGAL_MOVES"; map["ChessFindBestMove"]="ENGINE_CHESS_FIND_BEST_MOVE"; map["ChessMakeMove"]="ENGINE_CHESS_MAKE_MOVE"; map["ChessBeginGame"]="ENGINE_CHESS_BEGIN_GAME"; map["ChessCommitMove"]="ENGINE_CHESS_COMMIT_MOVE"; map["ChessRecordPosition"]="ENGINE_CHESS_RECORD_POSITION"; map["ChessClearPositionHistory"]="ENGINE_CHESS_CLEAR_POSITION_HISTORY"; map["ChessCheckRepetition"]="ENGINE_CHESS_CHECK_REPETITION"; map["ChessCheckGameState"]="ENGINE_CHESS_CHECK_GAME_STATE"; map["Board88"]="ENGINE_BOARD88"; map["BestMoveFrom"]="ENGINE_BEST_MOVE_FROM"; map["BestMoveTo"]="ENGINE_BEST_MOVE_TO"; map["MoveCount"]="ENGINE_MOVE_COUNT"; map["MoveListFrom"]="ENGINE_MOVE_LIST_FROM"; map["MoveListTo"]="ENGINE_MOVE_LIST_TO"; map["currentplayer"]="ENGINE_CURRENTPLAYER"; map["difficulty"]="ENGINE_DIFFICULTY"; map["TTClear"]="ENGINE_TT_CLEAR"; } $$3 ~ /^\./ { sym = substr($$3, 2); if (sym in map) printf "%-36s = $$%s\n", map[sym], $$2; }' "$(ENGINE_SYM)" > $@

ndi: $(IMAGE)

$(SMOKE_DLL): $(SMOKE_SRC)
	$(DOTNET) build $(SMOKE) -c Release $(SMOKE_BUILD_ARGS)

smoke: $(IMAGE) $(SMOKE_DLL)
	$(DOTNET) $(SMOKE_DLL) "$(IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "NOVA CHESS" --expect-screen "STATUS" --expect-screen "MOVES" $(SMOKE_GFX_EXPECTATIONS) $(SMOKE_EXTRA)

render-test:
	$(MAKE) smoke BUILD_DIR=$(RENDER_BUILD_DIR) IMAGE=$(RENDER_IMAGE) CA65FLAGS="-DCHESS_RENDER_TEST=1 -DDEMO_STEP_PAUSE=$(RENDER_STEP_PAUSE) -DDEMO_FINAL_PAUSE=$(RENDER_FINAL_PAUSE) -DDEMO_PIXEL_STEP=$(RENDER_PIXEL_STEP) -DDEMO_REPEAT=$(RENDER_REPEAT)" SMOKE_GFX_EXPECTATIONS='--expect-gfx-color "10,6=>0F" --expect-gfx-color "32,6=>0C" --expect-gfx-color "19,188=>01" --expect-gfx-color "195,13=>01" --expect-gfx-color "107,101=>01" --expect-gfx-color "128,119=>01" --expect-gfx-color "132,120=>0F" --expect-gfx-color "152,74=>01" --expect-gfx-color "16,98=>01" --expect-gfx-color "174,74=>01" --expect-gfx-color "108,140=>01" --expect-gfx-color "107,79=>0B" --expect-gfx-color "110,79=>0C" --expect-gfx-color "128,53=>0B" --expect-gfx-color "132,54=>0C" --expect-gfx-color "152,96=>0B" --expect-gfx-color "16,76=>0B" --expect-gfx-color "174,96=>0B" --expect-gfx-color "108,30=>0B"'

menu-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(MENU_BUILD_DIR) IMAGE=$(MENU_IMAGE) CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"
	$(DOTNET) $(SMOKE_DLL) "$(MENU_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "NOVA CHESS" --expect-screen "N NEW GAME" --expect-screen "D DEMO"

castle-test: $(SMOKE_DLL)
	$(MAKE) ndi BUILD_DIR=$(CASTLE_BUILD_DIR) IMAGE=$(CASTLE_IMAGE) CA65FLAGS="-DCHESS_CASTLE_TEST=1 -DDEMO_STEP_PAUSE=0 -DDEMO_FINAL_PAUSE=0 -DDEMO_REPEAT=0"
	$(DOTNET) $(SMOKE_DLL) "$(CASTLE_IMAGE)" --generic-boot --screen-only --skip-manifest-check --expect-screen "NOVA CHESS" --expect-screen "STATUS" --expect-screen "MOVES" --expect-gfx-color "64,162=>01" --expect-gfx-color "82,164=>01"

test: smoke menu-test castle-test
	$(MAKE) render-test RENDER_STEP_PAUSE=$(TEST_RENDER_STEP_PAUSE) RENDER_FINAL_PAUSE=$(TEST_RENDER_FINAL_PAUSE) RENDER_PIXEL_STEP=$(TEST_RENDER_PIXEL_STEP) RENDER_REPEAT=$(TEST_RENDER_REPEAT)

$(IMAGE): $(AUTOBOOT) $(RUNTIME) $(ENGINE) $(PACKER)
	mkdir -p "$(dir $(IMAGE))"
	$(DOTNET) run --project $(PACKER) -- --output "$(IMAGE)" --autoboot "$(AUTOBOOT)" --runtime "$(RUNTIME)" --runtime-name "$(RUNTIME_NAME)" --asset "$(ENGINE):$(ENGINE_NAME)" --label "$(LABEL)" --size-kb "$(SIZE_KB)"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) build-render $(DIST_DIR)

.PHONY: all ndi smoke render-test menu-test castle-test test clean
