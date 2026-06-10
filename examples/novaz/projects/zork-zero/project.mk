PROJECT_TITLE := Zork Zero
PROJECT_LABEL := ZORK0
PROJECT_SIZE_KB := 1440
PROJECT_STORY := $(PROJECT_DIR)/STORY.BIN
# User-supplied local copy of the Zork Zero graphics blorb (gitignored, like
# STORY.BIN): cp ".../Zork Zero - The Revenge of Megaboz/Zork0.blb" PICS.BLB
PROJECT_PICTURES := $(PROJECT_DIR)/PICS.BLB
PROJECT_IMAGE := $(DIST_DIR)/$(PROJECT)/fd0.ndi
# V6 has no v3-style status line, and the banner scrolls off before the first
# prompt, so release/serial never survive on screen for the manifest check.
# The --expect-at args pin the M2 boot layout (cells are 0-based col,row):
# banner room/region on row 0, the game's text-only "Moves:" banner artifact
# on row 1, a blank row in the banner/playfield gap, the room title inside the
# 45x70 playfield inset at origin (5,5) + 1-col left margin (text col 6, with
# blank gutter cols 0-5), and the prompt at the window bottom row 49.
PROJECT_SMOKE_ARGS := --generic-boot --no-status-line --skip-manifest-check \
	--expect-at "0,0=>Banquet Hall                   Flatheadia" \
	--expect-at "0,1=>Moves:" \
	--expect-at "0,4=>      " \
	--expect-at "0,37=>      Banquet Hall" \
	--expect-at "6,49=>>"
# 300KB V6 story: the boot prologue ends in the game's own ~65K-newline
# CLEAR-CRCNT storm (see README), which dominates the step budget.
NOVAZ_SMOKE_MAX_STEPS ?= 480000000
export NOVAZ_SMOKE_MAX_STEPS
