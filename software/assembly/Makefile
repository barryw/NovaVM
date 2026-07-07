# NovaVM standalone assembly applications
# Requires ca65 and ld65 on PATH.

NOVA_ASM := ../runtime/asm

KEYBOARD_DIR := apps/keyboard
DEMO_DIR     := apps/demo
ROTATE_DIR   := apps/vsprite_rotate
TURTLE_DIR   := apps/turtle
EDITUI_SHELL_DIR := apps/editui_shell
EDITBUF_TEST_DIR := apps/editbuf_test
EDITBUF_DEMO_DIR := apps/editbuf_demo
MOUSEDEMO_DIR := apps/mousedemo
VGC_WAIT_OBJ := $(KEYBOARD_DIR)/vgc_wait.o
VGC_VSYNC_OBJ := $(KEYBOARD_DIR)/vgc_vsync.o
VGC_KEYBOARD_OBJS := $(VGC_WAIT_OBJ) $(VGC_VSYNC_OBJ)
# Runtime feature archive: ld65 pulls only the referenced members (resolving
# transitive deps like blitter/vtext) instead of hand-listing individual .o files.
NOVA_LIB := $(NOVA_ASM)/build/nova.lib

$(NOVA_LIB):
	$(MAKE) -C $(NOVA_ASM)

all: keyboard keyboard-test keyboard-harness demo vsprite-rotate turtle editui-shell editbuf-test editbuf-demo mousedemo

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

demo: $(DEMO_DIR)/demo.bin

$(DEMO_DIR)/demo.bin: $(DEMO_DIR)/demo.o $(DEMO_DIR)/demo.cfg
	ld65 -C $(DEMO_DIR)/demo.cfg -o $@ $(DEMO_DIR)/demo.o

$(DEMO_DIR)/demo.o: $(DEMO_DIR)/demo.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu w65c02 -I $(NOVA_ASM) $< -o $@

install-demo: demo
	@echo "install-demo no longer writes to a host fallback; use nova create/import to place demo.bin on an NDI image." >&2
	@false

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
	$(RM) $(DEMO_DIR)/demo.o $(DEMO_DIR)/demo.bin $(DEMO_DIR)/demo.dbg
	$(RM) $(ROTATE_DIR)/vsprite_rotate.o $(ROTATE_DIR)/vsprite_rotate.bin
	$(RM) $(ROTATE_DIR)/vsprite.o
	$(RM) $(TURTLE_DIR)/turtle.o $(TURTLE_DIR)/turtle.bin
	$(RM) $(TURTLE_DIR)/vsprite.o
	$(RM) $(EDITUI_SHELL_DIR)/editui_shell.o $(EDITUI_SHELL_DIR)/editui_shell.bin
	$(RM) $(EDITUI_SHELL_DIR)/editui.o
	$(RM) $(EDITBUF_TEST_DIR)/editbuf_test.o $(EDITBUF_TEST_DIR)/editbuf_test.bin
	$(RM) $(EDITBUF_DEMO_DIR)/editbuf_demo.o $(EDITBUF_DEMO_DIR)/editbuf_demo.bin

.PHONY: all keyboard keyboard-test install-keyboard demo install-demo vsprite-rotate install-vsprite-rotate turtle install-turtle editui-shell install-editui-shell editbuf-test editbuf-demo clean
