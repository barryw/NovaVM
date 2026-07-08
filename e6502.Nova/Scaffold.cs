// Templates for `nova new` — the starter files written into a scaffolded native
// Nova 6502 project. @@NAME@@/@@LABEL@@ are substituted; @TAB@ becomes a real tab
// (Makefile recipe lines require tabs).

static class Scaffold
{
    public const string MainS = @"; @@NAME@@ — a native Nova program (65C02). Loads at $7200 and runs on boot
; (packed as AUTOBOOT.BIN in the disk image). Edit freely; `make ndi` rebuilds.

.include ""nova.inc""

.segment ""HEADER""
        .byte   $00, $72                ; load address = $7200

.segment ""CODE""
start:
        LDA     #$0C                    ; clear the screen
        STA     VGC_CHAROUT
        LDX     #0
@print:
        LDA     banner,X
        BEQ     @done
        STA     VGC_CHAROUT             ; write one character to the screen
        INX
        BNE     @print
@done:
        RTS                             ; hand control back to the system

.segment ""RODATA""
banner:
        .byte   ""HELLO FROM @@NAME@@!"", $0D, $00
";

    public const string LinkCfg = @"# ld65 config for @@NAME@@: a 2-byte load-address prefix ($00 $72) then code
# at $7200. The Nova loader reads the prefix and jumps to $7200.
MEMORY {
    ZP:   start = $0040, size = $40,    type = rw;
    HDR:  start = $0000, size = 2,      fill = yes;
    MAIN: start = $7200, size = $6000;
}
SEGMENTS {
    ZEROPAGE: load = ZP,   type = zp;
    HEADER:   load = HDR,  type = ro;
    CODE:     load = MAIN, type = ro;
    RODATA:   load = MAIN, type = ro;
    BSS:      load = MAIN, type = bss, define = yes;
}
";

    public const string Makefile = @"# @@NAME@@ — native Nova 6502 project.
# Requires: cc65 (brew install cc65) and the `nova` CLI on PATH.
NAME   := @@NAME@@
LABEL  := @@LABEL@@
NOVA   ?= nova
CPU    := --cpu w65c02
SDK    := sdk
BUILD  := build
REMOTE ?=

SRCS := $(wildcard src/*.s)
OBJS := $(patsubst src/%.s,$(BUILD)/%.o,$(SRCS))

all: $(BUILD)/$(NAME).bin

$(BUILD)/%.o: src/%.s | $(BUILD)
@TAB@ca65 $(CPU) -I $(SDK) $< -o $@

$(BUILD)/$(NAME).bin: $(OBJS) link.cfg
@TAB@ld65 -C link.cfg -o $@ $(OBJS) $(SDK)/nova.lib

$(BUILD):
@TAB@@mkdir -p $(BUILD)

# Pack the program as AUTOBOOT.BIN into a bootable Nova disk image.
ndi: all
@TAB@@cp $(BUILD)/$(NAME).bin $(BUILD)/AUTOBOOT.bin
@TAB@@rm -f $(NAME).ndi
@TAB@$(NOVA) create $(NAME).ndi --size 800 --label $(LABEL)
@TAB@$(NOVA) import $(NAME).ndi $(BUILD)/AUTOBOOT.bin /
@TAB@@echo "">> $(NAME).ndi ready — mount it on a Nova and cold-start to run.""

# Copy the image to a Nova device, mount it on fd0, and cold-start (autoboots).
#   make deploy REMOTE=192.168.1.188
deploy: ndi
@TAB@@test -n ""$(REMOTE)"" || { echo ""usage: make deploy REMOTE=<device-ip>""; exit 1; }
@TAB@$(NOVA) put $(NAME).ndi disks/$(NAME).ndi --remote $(REMOTE)
@TAB@$(NOVA) drive mount fd0 /disks/$(NAME).ndi --remote $(REMOTE)
@TAB@$(NOVA) vm cold-start --remote $(REMOTE)

clean:
@TAB@rm -rf $(BUILD) $(NAME).ndi

.PHONY: all ndi deploy clean
";

    public const string Readme = @"# @@NAME@@

A native Nova 6502 program you build on macOS/Linux and run as a bootable disk
image (`.ndi`).

## Prerequisites
- **cc65** — the 6502 assembler/linker: `brew install cc65` (macOS) or your package manager.
- **nova** — the Nova CLI on your `PATH` (used to pack the `.ndi`).

## Build
```
make          # assemble + link  -> build/@@NAME@@.bin
make ndi      # build + pack      -> @@NAME@@.ndi   (bootable: AUTOBOOT.BIN)
make clean
```

## Run it on a Nova
```
make deploy REMOTE=<device-ip>    # copies the image, mounts fd0, cold-starts (autoboots)
```
Or copy `@@NAME@@.ndi` to the device's `disks/` yourself and pick it in the OSD.

## Layout
- `src/main.s` — your program. Loads at `$7200`; the starter clears the screen and
  prints a banner via `VGC_CHAROUT`.
- `link.cfg` — ld65 memory/segment layout (load-address prefix + code at `$7200`).
- `sdk/` — the vendored Nova SDK: every runtime `*.inc` (register + command
  definitions) and `nova.lib`. Add more `.s` files under `src/` — the Makefile
  assembles every `src/*.s` and links `nova.lib`, so you can call runtime helpers
  (sprites, graphics, sound) declared in the `sdk/*.inc` headers.
";
}
