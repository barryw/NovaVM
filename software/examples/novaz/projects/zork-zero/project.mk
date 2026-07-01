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
# The --expect-at args pin the V6 pixel-unit boot layout (cells are 0-based
# col,row): the story sees a 320x200-unit screen with a 4x4 font, lays out the
# banner from real picture metrics, and the text layer snaps those units back
# to 80x50 cells at the VTEXT boundary. Small transparent inline pictures align
# their visible art to the 4px text grid, so the story's own CR counts stay
# intact; boot needs no MORE answers.
PROJECT_SMOKE_ARGS := --generic-boot --no-status-line --skip-manifest-check \
	--expect-zork0-boot-gfx-replay \
	--expect-at "8,1=>Banquet Hall" \
	--expect-at "61,1=>Flatheadia" \
	--expect-at "8,2=>Moves:  0" \
	--expect-at "61,2=>Score:   0" \
	--expect-at "11,33=>>" \
	--expect-gfx-color "0,0=>06" \
	--expect-gfx-color "12,34=>01" \
	--expect-gfx-color "43,43=>0C" \
	--expect-gfx-color "65,61=>0C" \
	--expect-gfx-color "43,92=>0B" \
	--expect-gfx-color "300,34=>01"
# The gfx probes read picture pixels straight from the plane: title art
# pic 5 at (0,0), border art pic 2 at pixel (43,43), and the per-refresh
# border pic \$D8 at px (43,92) — values derived from the blorb PNGs through
# the packer's generated custom-palette quantizer.
# The 65K-newline boot storm died with real picture metrics; 80M steps is
# plenty for boot + three turns now.
NOVAZ_SMOKE_MAX_STEPS ?= 80000000
export NOVAZ_SMOKE_MAX_STEPS
