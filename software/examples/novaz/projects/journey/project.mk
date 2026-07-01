PROJECT_TITLE := Journey
PROJECT_LABEL := JOURNEY
PROJECT_SIZE_KB := 1440
PROJECT_STORY := $(PROJECT_DIR)/STORY.BIN
PROJECT_PICTURES := $(PROJECT_DIR)/PICS.BLB
PROJECT_IMAGE := $(DIST_DIR)/$(PROJECT)/fd0.ndi

# Journey is a V6 graphical/menu-driven story: its first stable UI is the
# menu, not a bare parser prompt.
PROJECT_SMOKE_ARGS := --generic-boot --screen-only --no-status-line --skip-manifest-check --auto-read-char-space \
	--expect-screen "The Party" \
	--expect-screen "Individual Commands" \
	--expect-screen "Start"
NOVAZ_SMOKE_MAX_STEPS ?= 120000000
export NOVAZ_SMOKE_MAX_STEPS
