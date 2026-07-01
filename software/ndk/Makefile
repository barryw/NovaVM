# Nova Developer Kit packager.

REPO_ROOT := ../..
SOFTWARE_ROOT := ..
DIST_DIR ?= $(REPO_ROOT)/dist
PACKAGE_NAME ?= nova-ndk
PACKAGE_DIR := $(DIST_DIR)/$(PACKAGE_NAME)
ARCHIVE := $(DIST_DIR)/$(PACKAGE_NAME).tar.gz

all: package

package:
	$(RM) -r $(PACKAGE_DIR)
	mkdir -p $(PACKAGE_DIR)
	cp README.md $(PACKAGE_DIR)/README.md
	cp $(REPO_ROOT)/License.txt $(PACKAGE_DIR)/LICENSE.txt
	rsync -a --delete $(SOFTWARE_ROOT)/runtime/asm/ $(PACKAGE_DIR)/asm/
	mkdir -p $(PACKAGE_DIR)/docs
	rsync -a --delete $(REPO_ROOT)/docs/assembly/ $(PACKAGE_DIR)/docs/assembly/
	mkdir -p $(PACKAGE_DIR)/docs/help/guides
	cp $(REPO_ROOT)/docs/help/guides/assembly.md $(PACKAGE_DIR)/docs/help/guides/
	cp $(REPO_ROOT)/docs/help/guides/graphics-and-display.md $(PACKAGE_DIR)/docs/help/guides/
	cp $(REPO_ROOT)/docs/help/guides/memory-map.md $(PACKAGE_DIR)/docs/help/guides/
	cp $(REPO_ROOT)/docs/help/guides/networking.md $(PACKAGE_DIR)/docs/help/guides/
	cp $(REPO_ROOT)/docs/help/guides/sound-and-music.md $(PACKAGE_DIR)/docs/help/guides/
	mkdir -p $(PACKAGE_DIR)/examples/programs
	rsync -a --include='*/' --include='*.s' --include='*.cfg' --include='*.inc' --include='README.md' --exclude='*' $(REPO_ROOT)/docs/programs/ $(PACKAGE_DIR)/examples/programs/
	rsync -a --delete --exclude='*.o' --exclude='*.dbg' $(SOFTWARE_ROOT)/assembly/apps/ $(PACKAGE_DIR)/apps/

archive: package
	tar -C $(DIST_DIR) -czf $(ARCHIVE) $(PACKAGE_NAME)

clean:
	$(RM) -r $(PACKAGE_DIR) $(ARCHIVE)

.PHONY: all package archive clean
