PROJECT_TITLE := Zork Zero
PROJECT_LABEL := ZORK0
PROJECT_SIZE_KB := 1440
PROJECT_STORY := $(PROJECT_DIR)/STORY.BIN
PROJECT_IMAGE := $(DIST_DIR)/$(PROJECT)/fd0.ndi
# V6 has no v3-style status line, and the banner scrolls off before the first
# prompt, so release/serial never survive on screen for the manifest check.
PROJECT_SMOKE_ARGS := --generic-boot --no-status-line --skip-manifest-check
# 300KB V6 story: boot pages enough story to blow the default 80M-step budget.
NOVAZ_SMOKE_MAX_STEPS ?= 480000000
export NOVAZ_SMOKE_MAX_STEPS
