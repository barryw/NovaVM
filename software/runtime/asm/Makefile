# runtime/asm/Makefile — canonical resident library loader build (Task 4c.0a).
#
# Builds the ONE canonical libcall.bin from libcall.s at ORG $0320 (the
# cross-runtime resident loader band, just above the $0300-$031F mailbox) and
# installs it into the Avalonia Resources, mirroring how novalogo.bin lives
# there. This is distinct from tests/asm/libcall.bin (ORG $9C00), which is
# isolated $9C00 scaffolding for LibCallTests/GraphicsModuleTests/HW smoke.
#
# Requires ca65 and ld65 on PATH (cc65 toolchain).

CA65      := ca65 --cpu w65c02 -I .
LD65      := ld65
AR65      := ar65
RESOURCES := ../../../e6502.Avalonia/Resources
BUILD_DIR := build
NOVA_LIB  := $(BUILD_DIR)/nova.lib

NOVA_LIB_SRCS := \
	array.s \
	audio.s \
	wts.s \
	anim.s \
	blitter.s \
	copper.s \
	copper_split.s \
	dma.s \
	docbuf.s \
	editbuf.s \
	editui.s \
	editui_box.s \
	editui_shutdown.s \
	fio_clear_error.s \
	fio.s \
	fio_inline.s \
	gameserver.s \
	mouse.s \
	mouse_events.s \
	msprite.s \
	nic.s \
	nui_state.s \
	nui_dialog.s \
	nui_input.s \
	nui_list.s \
	nui_file.s \
	nui_text.s \
	nui_uisave.s \
	nui_wait.s \
	nvg.s \
	nvg_named.s \
	overlay.s \
	pager.s \
	ramheap.s \
	replline.s \
	rng.s \
	sprite.s \
	spritebank.s \
	textsvc.s \
	tween.s \
	vgc.s \
	vgc_palette.s \
	vsprite.s \
	vtext.s \
	vtext_inline.s \
	vtext_mixed.s \
	xmc.s \
	xram.s

NOVA_LIB_OBJS := $(addprefix $(BUILD_DIR)/,$(NOVA_LIB_SRCS:.s=.o))

all: libcall.bin $(NOVA_LIB)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.s *.inc *.body | $(BUILD_DIR)
	$(CA65) -DNOVA_EMIT_ALL_RUNTIME=1 -o $@ $<

$(NOVA_LIB): $(NOVA_LIB_OBJS)
	rm -f $@
	$(AR65) r $@ $(NOVA_LIB_OBJS)

libcall.o: libcall.s libabi.inc nova.inc
	$(CA65) libcall.s -o libcall.o

libcall.bin: libcall.o libcall_resident.cfg
	$(LD65) -C libcall_resident.cfg -m libcall.map -o libcall.bin libcall.o

install: libcall.bin
	cp libcall.bin $(RESOURCES)/libcall.bin

clean:
	$(MAKE) clean-nova-lib
	rm -f libcall.o libcall.bin libcall.map

clean-nova-lib:
	rm -rf $(BUILD_DIR)

.PHONY: all install clean clean-nova-lib
