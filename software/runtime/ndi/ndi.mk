# ndi.mk — build a bootable NovaVM language floppy (.ndi) from a runtime ROM.
#
# A language Makefile includes this after defining:
#   NDI_RUNTIME  the 16KB runtime ROM to boot   (e.g. novaforth.bin)
#   NDI_OUTPUT   the .ndi image to produce       (e.g. novaforth.ndi)
#   NDI_LABEL    disk label, <=8 chars           (e.g. NFORTH)
# and providing a rule that builds $(NDI_RUNTIME). Then:
#   make ndi    -> a clean image from scratch (runtime + AUTOBOOT.bin + pack)
#
# The runtime is always stored inside the image as RUNTIME.BIN, which the shared
# autoboot launcher (runtime/ndi/autoboot.s) LOADRUNTIMEs into $C000 — so one
# launcher serves every language. No caching: `make ndi` wipes all artifacts and
# rebuilds completely.

CA65   ?= ca65
LD65   ?= ld65
DOTNET ?= dotnet

NDI_MK_DIR       := $(dir $(lastword $(MAKEFILE_LIST)))
NDI_NOVA_ASM     := $(NDI_MK_DIR)../asm
NDI_NOVA_LIB     := $(NDI_NOVA_ASM)/build/nova.lib
NDI_PACKER       := $(NDI_MK_DIR)../../examples/novaz/tools/Nova.NovaZ.Packer/Nova.NovaZ.Packer.csproj
NDI_AUTOBOOT_S   := $(NDI_MK_DIR)autoboot.s
NDI_AUTOBOOT_CFG := $(NDI_MK_DIR)autoboot.cfg
NDI_SIZE_KB      ?= 1440

ndi-autoboot.o: $(NDI_AUTOBOOT_S) $(NDI_AUTOBOOT_CFG)
	$(CA65) --cpu w65c02 -I $(NDI_NOVA_ASM) -o $@ $(NDI_AUTOBOOT_S)

AUTOBOOT.bin: ndi-autoboot.o $(NDI_AUTOBOOT_CFG)
	$(MAKE) -C $(NDI_NOVA_ASM) build/nova.lib
	$(LD65) -C $(NDI_AUTOBOOT_CFG) -o $@ ndi-autoboot.o $(NDI_NOVA_LIB)

# A language may set NDI_ASSETS (a list of --asset <path>:<name> flags, e.g. the
# forth library tree it autoloads at boot) + NDI_ASSET_DEPS (their files, for the
# rebuild trigger). Both default empty -> a plain runtime+autoboot image.
$(NDI_OUTPUT): $(NDI_RUNTIME) AUTOBOOT.bin $(NDI_PACKER) $(NDI_ASSET_DEPS)
	$(DOTNET) run --project $(NDI_PACKER) -- \
	    --output "$(NDI_OUTPUT)" \
	    --autoboot AUTOBOOT.bin \
	    --runtime "$(NDI_RUNTIME)" --runtime-name "RUNTIME.BIN" \
	    --label "$(NDI_LABEL)" --size-kb "$(NDI_SIZE_KB)" \
	    $(NDI_ASSETS)

# Build a completely clean image from scratch (no stale runtime/autoboot/.ndi).
ndi:
	$(MAKE) clean
	rm -f AUTOBOOT.bin ndi-autoboot.o "$(NDI_OUTPUT)"
	$(MAKE) "$(NDI_OUTPUT)"

.PHONY: ndi
