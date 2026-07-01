PROJECT_TITLE := Arthur
PROJECT_LABEL := ARTHUR
PROJECT_SIZE_KB := 1440
PROJECT_STORY := $(PROJECT_DIR)/STORY.BIN
PROJECT_PICTURES := $(PROJECT_DIR)/PICS.BLB
PROJECT_IMAGE := $(DIST_DIR)/$(PROJECT)/fd0.ndi

# V6 graphical story. Start with a light boot smoke; once the first stable
# screen is known, pin Arthur-specific text and graphics probes here.
PROJECT_SMOKE_ARGS := --generic-boot --boot-only --no-status-line --skip-manifest-check --auto-read-char-space
NOVAZ_SMOKE_MAX_STEPS ?= 120000000
export NOVAZ_SMOKE_MAX_STEPS
