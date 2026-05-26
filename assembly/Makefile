# NovaVM standalone assembly applications
# Requires ca65 and ld65 on PATH.

NOVA_ASM := ../runtime/asm

KEYBOARD_DIR := apps/keyboard
DEMO_DIR     := apps/demo
VGC_WAIT_OBJ := $(KEYBOARD_DIR)/vgc_wait.o

all: keyboard keyboard-test keyboard-harness demo

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

clean:
	$(RM) $(KEYBOARD_DIR)/keyboard.o $(KEYBOARD_DIR)/keyboard.bin
	$(RM) $(KEYBOARD_DIR)/kbdtest.o $(KEYBOARD_DIR)/kbdtest.bin
	$(RM) $(KEYBOARD_DIR)/test_loop.o $(KEYBOARD_DIR)/test_loop.bin
	$(RM) $(KEYBOARD_DIR)/test_music_status.o $(KEYBOARD_DIR)/test_music_status.bin
	$(RM) $(VGC_WAIT_OBJ)
	$(RM) $(DEMO_DIR)/demo.o $(DEMO_DIR)/demo.bin $(DEMO_DIR)/demo.dbg

.PHONY: all keyboard keyboard-test install-keyboard demo install-demo clean
