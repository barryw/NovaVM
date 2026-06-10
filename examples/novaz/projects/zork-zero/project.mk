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
# M3 interim pins (Task 4: picture_data answers from PICS.PAK): the banner
# now lays out from real picture metrics — "Banquet Hall ... Flatheadia" on
# row 2, "Moves:  0 ... Score:   0" on row 3, the M2 "Moves:Sc1" garble is
# gone — and the boot CLEAR-CRCNT newline storm self-limited (the countdown
# is armed with a real picture height instead of -1), so boot needs no MORE
# answers at all. The prompt sits at (18,41): the drop-cap margin is still
# armed because the CR interrupt never fires — Task 6 releases it; Task 7
# re-pins the final layout with border art.
# Task 6: the CR interrupt fires and RESET-MARGIN releases the drop-cap
# margins — the prompt lands at (11,34) inside the real 40x58 playfield the
# game sizes from border-art metrics (move_window 0,11,12 in the trace).
PROJECT_SMOKE_ARGS := --generic-boot --no-status-line --skip-manifest-check \
	--expect-at "9,2=>Banquet Hall" \
	--expect-at "61,2=>Flatheadia" \
	--expect-at "9,3=>Moves:  0" \
	--expect-at "61,3=>Score:   0" \
	--expect-at "11,34=>>"
# The 65K-newline boot storm died with real picture metrics; 80M steps is
# plenty for boot + three turns now.
NOVAZ_SMOKE_MAX_STEPS ?= 80000000
export NOVAZ_SMOKE_MAX_STEPS
