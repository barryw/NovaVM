# NovaVM standalone assembly applications
# Requires ca65 and ld65 on PATH.

NOVA_ASM := ../runtime/asm

KEYBOARD_DIR := apps/keyboard
MUSIC_DIR    := apps/music
ROTATE_DIR   := apps/vsprite_rotate
TURTLE_DIR   := apps/turtle
EDITUI_SHELL_DIR := apps/editui_shell
EDITBUF_TEST_DIR := apps/editbuf_test
EDITBUF_DEMO_DIR := apps/editbuf_demo
MOUSEDEMO_DIR := apps/mousedemo
SPRITEBANK_DEMO_DIR := apps/spritebank_demo
VGC_WAIT_OBJ := $(KEYBOARD_DIR)/vgc_wait.o
VGC_VSYNC_OBJ := $(KEYBOARD_DIR)/vgc_vsync.o
VGC_KEYBOARD_OBJS := $(VGC_WAIT_OBJ) $(VGC_VSYNC_OBJ)
# Runtime feature archive: ld65 pulls only the referenced members (resolving
# transitive deps like blitter/vtext) instead of hand-listing individual .o files.
NOVA_LIB := $(NOVA_ASM)/build/nova.lib

$(NOVA_LIB):
	$(MAKE) -C $(NOVA_ASM)

all: keyboard keyboard-test keyboard-harness music vsprite-rotate turtle editui-shell editbuf-test editbuf-demo mousedemo spritebank-demo xramcheck

keyboard: $(KEYBOARD_DIR)/keyboard.bin
keyboard-test: $(KEYBOARD_DIR)/kbdtest.bin
keyboard-harness: $(KEYBOARD_DIR)/test_loop.bin $(KEYBOARD_DIR)/test_music_status.bin

$(KEYBOARD_DIR)/keyboard.bin: $(KEYBOARD_DIR)/keyboard.o $(VGC_KEYBOARD_OBJS) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/keyboard.o $(VGC_KEYBOARD_OBJS)

$(KEYBOARD_DIR)/keyboard.o: $(KEYBOARD_DIR)/keyboard.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/kbdtest.bin: $(KEYBOARD_DIR)/kbdtest.o $(VGC_KEYBOARD_OBJS) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/kbdtest.o $(VGC_KEYBOARD_OBJS)

$(KEYBOARD_DIR)/kbdtest.o: $(KEYBOARD_DIR)/keyboard.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -D KEYBOARD_TEST=1 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/test_loop.bin: $(KEYBOARD_DIR)/test_loop.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/test_loop.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/test_loop.o: $(KEYBOARD_DIR)/test_loop.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/test_music_status.bin: $(KEYBOARD_DIR)/test_music_status.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/test_music_status.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/test_music_status.o: $(KEYBOARD_DIR)/test_music_status.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(VGC_WAIT_OBJ): $(NOVA_ASM)/vgc_wait.s $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(VGC_VSYNC_OBJ): $(NOVA_ASM)/vgc_vsync.s $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

install-keyboard: keyboard
	@echo "install-keyboard no longer writes to a host fallback; use nova create/import to place keyboard.bin on an NDI image." >&2
	@false

music: $(MUSIC_DIR)/music.bin

# nova.lib (vgc.s) already provides vgc_wait_cmd + vgc_vsync, so don't also link
# the standalone vgc_*.o here (that's only for the nova.lib-less keyboard.bin).
$(MUSIC_DIR)/music.bin: $(MUSIC_DIR)/music.o $(MUSIC_DIR)/keyboard_lib.o $(MUSIC_DIR)/music.cfg $(NOVA_LIB)
	ld65 -C $(MUSIC_DIR)/music.cfg -o $@ $(MUSIC_DIR)/music.o $(MUSIC_DIR)/keyboard_lib.o $(NOVA_LIB)

$(MUSIC_DIR)/music.o: $(MUSIC_DIR)/music.s $(MUSIC_DIR)/music_notes.inc $(MUSIC_DIR)/music_tracks.inc $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

# keyboard visualizer as a linkable library (no HEADER/main; kbdviz_run only)
$(MUSIC_DIR)/keyboard_lib.o: $(KEYBOARD_DIR)/keyboard.s $(NOVA_ASM)/novavm.inc $(NOVA_ASM)/audio.inc
	ca65 --cpu w65c02 -DKBD_LIB -I $(NOVA_ASM) $(KEYBOARD_DIR)/keyboard.s -o $@

vsprite-rotate: $(ROTATE_DIR)/vsprite_rotate.bin

$(ROTATE_DIR)/vsprite_rotate.bin: $(ROTATE_DIR)/vsprite_rotate.o $(NOVA_LIB) $(ROTATE_DIR)/vsprite_rotate.cfg
	ld65 -C $(ROTATE_DIR)/vsprite_rotate.cfg -o $@ $(ROTATE_DIR)/vsprite_rotate.o $(NOVA_LIB)

$(ROTATE_DIR)/vsprite_rotate.o: $(ROTATE_DIR)/vsprite_rotate.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(ROTATE_DIR)/vsprite.o: $(NOVA_ASM)/vsprite.s $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

install-vsprite-rotate: vsprite-rotate
	@echo "install-vsprite-rotate no longer writes to a host fallback; use nova create/import to place vsprite_rotate.bin on an NDI image." >&2
	@false

turtle: $(TURTLE_DIR)/turtle.bin

$(TURTLE_DIR)/turtle.bin: $(TURTLE_DIR)/turtle.o $(NOVA_LIB) $(TURTLE_DIR)/turtle.cfg
	ld65 -C $(TURTLE_DIR)/turtle.cfg -o $@ $(TURTLE_DIR)/turtle.o $(NOVA_LIB)

$(TURTLE_DIR)/turtle.o: $(TURTLE_DIR)/turtle.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(TURTLE_DIR)/vsprite.o: $(NOVA_ASM)/vsprite.s $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

install-turtle: turtle
	@echo "install-turtle no longer writes to a host fallback; use nova create/import to place turtle.bin on an NDI image." >&2
	@false

mousedemo: $(MOUSEDEMO_DIR)/mousedemo.bin

$(MOUSEDEMO_DIR)/mousedemo.bin: $(MOUSEDEMO_DIR)/mousedemo.o $(NOVA_LIB) $(MOUSEDEMO_DIR)/mousedemo.cfg
	ld65 -C $(MOUSEDEMO_DIR)/mousedemo.cfg -o $@ $(MOUSEDEMO_DIR)/mousedemo.o $(NOVA_LIB)

$(MOUSEDEMO_DIR)/mousedemo.o: $(MOUSEDEMO_DIR)/mousedemo.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/novavm.inc $(NOVA_ASM)/mouse.inc $(NOVA_ASM)/mouse_events.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

spritebank-demo: $(SPRITEBANK_DEMO_DIR)/spritebank_demo.bin

$(SPRITEBANK_DEMO_DIR)/spritebank_demo.bin: $(SPRITEBANK_DEMO_DIR)/spritebank_demo.o $(NOVA_LIB) $(SPRITEBANK_DEMO_DIR)/spritebank_demo.cfg
	ld65 -C $(SPRITEBANK_DEMO_DIR)/spritebank_demo.cfg -o $@ $(SPRITEBANK_DEMO_DIR)/spritebank_demo.o $(NOVA_LIB)

# Bootable ball-bounce disk: the demo .bin becomes AUTOBOOT.BIN, which the
# novavm boot-time probe LOADs + runs at cold start. Mount on fd0 + cold-start
# (or boot with it) and the ball bounces with sound, no SYS needed.
NOVA_BIN  := ../../e6502.Nova/bin/Release/net10.0/linux-x64/nova
BALL_NDI  := ../../docs/programs/ball-bounce.ndi

ball-bounce-ndi: spritebank-demo
	cp $(SPRITEBANK_DEMO_DIR)/spritebank_demo.bin $(SPRITEBANK_DEMO_DIR)/AUTOBOOT.bin
	rm -f $(BALL_NDI)
	$(NOVA_BIN) create $(BALL_NDI) --size 800 --label BALLBNC
	$(NOVA_BIN) import $(BALL_NDI) $(SPRITEBANK_DEMO_DIR)/AUTOBOOT.bin /
	rm -f $(SPRITEBANK_DEMO_DIR)/AUTOBOOT.bin
	@echo "built $(BALL_NDI) -- scp to /data/nova/disks/floppy, mount fd0, cold-start to autoboot"

$(SPRITEBANK_DEMO_DIR)/spritebank_demo.o: $(SPRITEBANK_DEMO_DIR)/spritebank_demo.s $(SPRITEBANK_DEMO_DIR)/demo.nsp $(NOVA_ASM)/nova.inc $(NOVA_ASM)/spritebank.inc $(NOVA_ASM)/msprite.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) --bin-include-dir $(SPRITEBANK_DEMO_DIR) $< -o $@

editui-shell: $(EDITUI_SHELL_DIR)/editui_shell.bin

$(EDITUI_SHELL_DIR)/editui_shell.bin: $(EDITUI_SHELL_DIR)/editui_shell.o $(NOVA_LIB) $(EDITUI_SHELL_DIR)/editui_shell.cfg
	ld65 -C $(EDITUI_SHELL_DIR)/editui_shell.cfg -o $@ $(EDITUI_SHELL_DIR)/editui_shell.o $(NOVA_LIB)

$(EDITUI_SHELL_DIR)/editui_shell.o: $(EDITUI_SHELL_DIR)/editui_shell.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/editui.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(EDITUI_SHELL_DIR)/editui.o: $(NOVA_ASM)/editui.s $(NOVA_ASM)/editui.inc $(NOVA_ASM)/vtext.s $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

install-editui-shell: editui-shell
	@echo "install-editui-shell no longer writes to a host fallback; use nova create/import to place editui_shell.bin on an NDI image." >&2
	@false

xramcheck: apps/xramcheck/xramcheck.bin

# Hardware smoke test for the XRAM/file path the whole toolchain stages through.
apps/xramcheck/xramcheck.bin: apps/xramcheck/xramcheck.o apps/xramcheck/xramcheck.cfg
	ld65 -C apps/xramcheck/xramcheck.cfg -o $@ apps/xramcheck/xramcheck.o

apps/xramcheck/xramcheck.o: apps/xramcheck/xramcheck.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/libabi.inc $(NOVA_ASM)/libmemory.inc $(NOVA_ASM)/libfiles.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) -o $@ $<

editbuf-test: $(EDITBUF_TEST_DIR)/editbuf_test.bin

$(EDITBUF_TEST_DIR)/editbuf_test.bin: $(EDITBUF_TEST_DIR)/editbuf_test.o $(NOVA_LIB) $(EDITBUF_TEST_DIR)/editbuf_test.cfg
	ld65 -C $(EDITBUF_TEST_DIR)/editbuf_test.cfg -o $@ $(EDITBUF_TEST_DIR)/editbuf_test.o $(NOVA_LIB)

$(EDITBUF_TEST_DIR)/editbuf_test.o: $(EDITBUF_TEST_DIR)/editbuf_test.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/docbuf.inc $(NOVA_ASM)/nui.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

$(EDITBUF_TEST_DIR)/editui.o: $(NOVA_ASM)/editui.s $(NOVA_ASM)/editui.inc $(NOVA_ASM)/vtext.s $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $(NOVA_ASM)/editui.s -o $@

$(EDITBUF_TEST_DIR)/editbuf.o: $(NOVA_ASM)/editbuf.s $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/editui.inc $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $(NOVA_ASM)/editbuf.s -o $@

editbuf-demo: $(EDITBUF_DEMO_DIR)/editbuf_demo.bin

$(EDITBUF_DEMO_DIR)/editbuf_demo.bin: $(EDITBUF_DEMO_DIR)/editbuf_demo.o $(NOVA_LIB) $(EDITBUF_DEMO_DIR)/editbuf_demo.cfg
	ld65 -C $(EDITBUF_DEMO_DIR)/editbuf_demo.cfg -o $@ $(EDITBUF_DEMO_DIR)/editbuf_demo.o $(NOVA_LIB)

$(EDITBUF_DEMO_DIR)/editbuf_demo.o: $(EDITBUF_DEMO_DIR)/editbuf_demo.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/editui.inc $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/nui.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

clean:
	$(RM) $(KEYBOARD_DIR)/keyboard.o $(KEYBOARD_DIR)/keyboard.bin
	$(RM) $(KEYBOARD_DIR)/kbdtest.o $(KEYBOARD_DIR)/kbdtest.bin
	$(RM) $(KEYBOARD_DIR)/test_loop.o $(KEYBOARD_DIR)/test_loop.bin
	$(RM) $(KEYBOARD_DIR)/test_music_status.o $(KEYBOARD_DIR)/test_music_status.bin
	$(RM) $(VGC_WAIT_OBJ)
	$(RM) $(VGC_VSYNC_OBJ)
	$(RM) $(MUSIC_DIR)/music.o $(MUSIC_DIR)/keyboard_lib.o $(MUSIC_DIR)/music.bin
	$(RM) $(ROTATE_DIR)/vsprite_rotate.o $(ROTATE_DIR)/vsprite_rotate.bin
	$(RM) $(ROTATE_DIR)/vsprite.o
	$(RM) $(TURTLE_DIR)/turtle.o $(TURTLE_DIR)/turtle.bin
	$(RM) $(TURTLE_DIR)/vsprite.o
	$(RM) $(EDITUI_SHELL_DIR)/editui_shell.o $(EDITUI_SHELL_DIR)/editui_shell.bin
	$(RM) $(EDITUI_SHELL_DIR)/editui.o
	$(RM) $(EDITBUF_TEST_DIR)/editbuf_test.o $(EDITBUF_TEST_DIR)/editbuf_test.bin
	$(RM) $(EDITBUF_DEMO_DIR)/editbuf_demo.o $(EDITBUF_DEMO_DIR)/editbuf_demo.bin
	$(RM) $(MOUSEDEMO_DIR)/mousedemo.o $(MOUSEDEMO_DIR)/mousedemo.bin
	$(RM) $(SPRITEBANK_DEMO_DIR)/spritebank_demo.o $(SPRITEBANK_DEMO_DIR)/spritebank_demo.bin

.PHONY: all xramcheck keyboard keyboard-test keyboard-harness install-keyboard music vsprite-rotate install-vsprite-rotate turtle install-turtle editui-shell install-editui-shell editbuf-test editbuf-demo spritebank-demo ball-bounce-ndi clean
