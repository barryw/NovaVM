# NovaVM standalone assembly applications
# Requires ca65 and ld65 on PATH.

NOVA_ASM := ../runtime/asm

KEYBOARD_DIR := apps/keyboard
DEMO_DIR     := apps/demo
ROTATE_DIR   := apps/vsprite_rotate
TURTLE_DIR   := apps/turtle
EDITUI_SHELL_DIR := apps/editui_shell
VGC_WAIT_OBJ := $(KEYBOARD_DIR)/vgc_wait.o

all: keyboard keyboard-test keyboard-harness demo vsprite-rotate turtle editui-shell

keyboard: $(KEYBOARD_DIR)/keyboard.bin
keyboard-test: $(KEYBOARD_DIR)/kbdtest.bin
keyboard-harness: $(KEYBOARD_DIR)/test_loop.bin $(KEYBOARD_DIR)/test_music_status.bin

$(KEYBOARD_DIR)/keyboard.bin: $(KEYBOARD_DIR)/keyboard.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/keyboard.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/keyboard.o: $(KEYBOARD_DIR)/keyboard.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/kbdtest.bin: $(KEYBOARD_DIR)/kbdtest.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/kbdtest.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/kbdtest.o: $(KEYBOARD_DIR)/keyboard.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu 65c02 -D KEYBOARD_TEST=1 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/test_loop.bin: $(KEYBOARD_DIR)/test_loop.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/test_loop.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/test_loop.o: $(KEYBOARD_DIR)/test_loop.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(KEYBOARD_DIR)/test_music_status.bin: $(KEYBOARD_DIR)/test_music_status.o $(VGC_WAIT_OBJ) $(KEYBOARD_DIR)/keyboard.cfg
	ld65 -C $(KEYBOARD_DIR)/keyboard.cfg -o $@ $(KEYBOARD_DIR)/test_music_status.o $(VGC_WAIT_OBJ)

$(KEYBOARD_DIR)/test_music_status.o: $(KEYBOARD_DIR)/test_music_status.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(VGC_WAIT_OBJ): $(NOVA_ASM)/vgc_wait.s $(NOVA_ASM)/vgc.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

install-keyboard: keyboard
	cp $(KEYBOARD_DIR)/keyboard.bin ~/e6502-programs/keyboard.bin

demo: $(DEMO_DIR)/demo.bin

$(DEMO_DIR)/demo.bin: $(DEMO_DIR)/demo.o $(DEMO_DIR)/demo.cfg
	ld65 -C $(DEMO_DIR)/demo.cfg -o $@ $(DEMO_DIR)/demo.o

$(DEMO_DIR)/demo.o: $(DEMO_DIR)/demo.s $(NOVA_ASM)/novavm.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

install-demo: demo
	cp $(DEMO_DIR)/demo.bin ~/e6502-programs/demo.bin

vsprite-rotate: $(ROTATE_DIR)/vsprite_rotate.bin

$(ROTATE_DIR)/vsprite_rotate.bin: $(ROTATE_DIR)/vsprite_rotate.o $(ROTATE_DIR)/vsprite.o $(ROTATE_DIR)/vsprite_rotate.cfg
	ld65 -C $(ROTATE_DIR)/vsprite_rotate.cfg -o $@ $(ROTATE_DIR)/vsprite_rotate.o $(ROTATE_DIR)/vsprite.o

$(ROTATE_DIR)/vsprite_rotate.o: $(ROTATE_DIR)/vsprite_rotate.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(ROTATE_DIR)/vsprite.o: $(NOVA_ASM)/vsprite.s $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

install-vsprite-rotate: vsprite-rotate
	cp $(ROTATE_DIR)/vsprite_rotate.bin ~/e6502-programs/vsprite_rotate.bin

turtle: $(TURTLE_DIR)/turtle.bin

$(TURTLE_DIR)/turtle.bin: $(TURTLE_DIR)/turtle.o $(TURTLE_DIR)/vsprite.o $(TURTLE_DIR)/turtle.cfg
	ld65 -C $(TURTLE_DIR)/turtle.cfg -o $@ $(TURTLE_DIR)/turtle.o $(TURTLE_DIR)/vsprite.o

$(TURTLE_DIR)/turtle.o: $(TURTLE_DIR)/turtle.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(TURTLE_DIR)/vsprite.o: $(NOVA_ASM)/vsprite.s $(NOVA_ASM)/vsprite.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

install-turtle: turtle
	cp $(TURTLE_DIR)/turtle.bin ~/e6502-programs/turtle.bin

editui-shell: $(EDITUI_SHELL_DIR)/editui_shell.bin

$(EDITUI_SHELL_DIR)/editui_shell.bin: $(EDITUI_SHELL_DIR)/editui_shell.o $(EDITUI_SHELL_DIR)/editui.o $(EDITUI_SHELL_DIR)/editui_shell.cfg
	ld65 -C $(EDITUI_SHELL_DIR)/editui_shell.cfg -o $@ $(EDITUI_SHELL_DIR)/editui_shell.o $(EDITUI_SHELL_DIR)/editui.o

$(EDITUI_SHELL_DIR)/editui_shell.o: $(EDITUI_SHELL_DIR)/editui_shell.s $(NOVA_ASM)/nova.inc $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/editui.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

$(EDITUI_SHELL_DIR)/editui.o: $(NOVA_ASM)/editui.s $(NOVA_ASM)/editui.inc $(NOVA_ASM)/vtext.s $(NOVA_ASM)/vtext.inc $(NOVA_ASM)/blitter.s $(NOVA_ASM)/blitter.inc $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) $< -o $@

install-editui-shell: editui-shell
	cp $(EDITUI_SHELL_DIR)/editui_shell.bin ~/e6502-programs/editui_shell.bin

clean:
	$(RM) $(KEYBOARD_DIR)/keyboard.o $(KEYBOARD_DIR)/keyboard.bin
	$(RM) $(KEYBOARD_DIR)/kbdtest.o $(KEYBOARD_DIR)/kbdtest.bin
	$(RM) $(KEYBOARD_DIR)/test_loop.o $(KEYBOARD_DIR)/test_loop.bin
	$(RM) $(KEYBOARD_DIR)/test_music_status.o $(KEYBOARD_DIR)/test_music_status.bin
	$(RM) $(VGC_WAIT_OBJ)
	$(RM) $(DEMO_DIR)/demo.o $(DEMO_DIR)/demo.bin $(DEMO_DIR)/demo.dbg
	$(RM) $(ROTATE_DIR)/vsprite_rotate.o $(ROTATE_DIR)/vsprite_rotate.bin
	$(RM) $(ROTATE_DIR)/vsprite.o
	$(RM) $(TURTLE_DIR)/turtle.o $(TURTLE_DIR)/turtle.bin
	$(RM) $(TURTLE_DIR)/vsprite.o
	$(RM) $(EDITUI_SHELL_DIR)/editui_shell.o $(EDITUI_SHELL_DIR)/editui_shell.bin
	$(RM) $(EDITUI_SHELL_DIR)/editui.o

.PHONY: all keyboard keyboard-test install-keyboard demo install-demo vsprite-rotate install-vsprite-rotate turtle install-turtle editui-shell install-editui-shell clean
