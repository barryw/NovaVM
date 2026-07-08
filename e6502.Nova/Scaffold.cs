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

    // --template sprite : a bouncing ball with async bounce sounds, built entirely
    // on the NDK (sprite_*, audio_*, vgc_vsync). The loop NEVER blocks on audio.
    public const string SpriteMainS = @"; @@NAME@@ — a bouncing ball, built on the Nova NDK.
;
; A single hardware sprite bounces around the screen; each time it hits a wall a
; short ""blip"" plays. Everything runs through the NDK runtime (linked from
; nova.lib), so this doubles as a tour of it:
;
;   sprite_clear / sprite_row  — define sprite 0's 16x16 shape (one row at a time)
;   sprite_priority / sprite_enable / sprite_pos — place and show it
;   vgc_vsync                  — pace the loop to the 60 Hz video frame
;   audio_init                 — SID + default instruments, once at startup
;   audio_play_sound           — fire a one-shot note and RETURN IMMEDIATELY
;   audio_tick                 — advance those notes one frame (gates them off)
;
; ASYNC RULE: audio is fire-and-forget. `audio_play_sound` steals a SID voice,
; records a frame countdown, and returns at once — it never waits for the note to
; finish. `audio_tick`, called once per frame, counts the note down and releases
; it. The bounce never stalls the ball. (Contrast the blocking `audio_tone`, which
; waits N frames — don't call that from an animation loop.)
;
; Loads at $7200 and runs on boot. Edit freely; `make ndi` rebuilds.

.include ""nova.inc""        ; register + control-code definitions
.include ""sprite.inc""      ; sprite_* command helpers
.include ""audio.inc""       ; audio_* sound helpers
.include ""vgc.inc""         ; vgc_vsync / vgc_wait_cmd

XLEFT   = 0                  ; sprite X is a byte here (0..239), so no 16-bit math
XRIGHT  = 239                ; right wall for a 16-wide sprite
YTOP    = 16                 ; leave the top text row for the title
YBOT    = 183                ; bottom wall (200 - 16 - a little)
XVEL0   = 3                  ; starting velocity (pixels/frame)
YVEL0   = 2
NOTE_X  = 43                 ; MIDI note for a side-wall bounce (low)
NOTE_Y  = 55                 ; MIDI note for a top/bottom bounce (higher)
BLIPLEN = 6                  ; bounce note length in frames

.segment ""ZEROPAGE""
xpos:   .res 1
ypos:   .res 1
xvel:   .res 1               ; signed
yvel:   .res 1               ; signed
row:    .res 1

.segment ""HEADER""
        .byte   $00, $72                ; load address = $7200

.segment ""CODE""
start:
        SEI
        CLD
        LDX     #$FF
        TXS

        JSR     audio_init              ; SID ready + a default instrument in slot 0

        ; --- clean black screen with a title -------------------------------
        STZ     VGC_BGCOL
        STZ     VGC_BORDER
        STZ     VGC_MODE                ; text layer (sprites draw over it)
        LDA     #$0C
        STA     VGC_CHAROUT             ; clear screen
        LDX     #0
@title: LDA     title,X
        BEQ     @haveball
        STA     VGC_CHAROUT
        INX
        BNE     @title

        ; --- define hardware sprite 0 = the ball ---------------------------
@haveball:
        STZ     VGC_P0                  ; P0 = sprite index 0
        JSR     sprite_clear            ; wipe its 16x16 shape RAM
        JSR     vgc_wait_cmd            ; let the command finish before the next
        STZ     row
@shape:
        LDA     row
        STA     VGC_P1                  ; P1 = row number
        ASL     A
        ASL     A
        ASL     A                       ; A = row * 8 = offset into ball_shape
        TAX
        LDY     #0
@cpy:   LDA     ball_shape,X            ; copy this row's 8 bytes into P2..P9
        STA     VGC_P2,Y
        INX
        INY
        CPY     #8
        BNE     @cpy
        STZ     VGC_P0                  ; P0 = sprite 0
        JSR     sprite_row              ; write one 16-pixel row of the shape
        JSR     vgc_wait_cmd
        INC     row
        LDA     row
        CMP     #16
        BNE     @shape

        ; --- priority (front), then show it --------------------------------
        STZ     VGC_P0
        LDA     #2                      ; 2 = draw in front of text/gfx
        STA     VGC_P1
        JSR     sprite_priority
        STZ     VGC_P0
        JSR     sprite_enable

        ; --- initial position + velocity -----------------------------------
        LDA     #100
        STA     xpos
        LDA     #80
        STA     ypos
        LDA     #XVEL0
        STA     xvel
        LDA     #YVEL0
        STA     yvel

; ----------------------------------------------------------------------------
; Main loop: one iteration per video frame. vgc_vsync is the only wait.
; ----------------------------------------------------------------------------
loop:
        JSR     vgc_vsync               ; block until the next frame (60 Hz)
        JSR     audio_tick              ; advance async notes — returns at once

        ; --- horizontal move + wall bounce ---------------------------------
        CLC
        LDA     xpos
        ADC     xvel                    ; signed add (xvel may be negative)
        STA     xpos
        CMP     #XRIGHT+1
        BCC     @xok                    ; 0..XRIGHT: still on screen
        LDA     xvel                    ; off screen — which wall?
        BMI     @xleft                  ; moving left  -> underflowed past 0
        LDA     #XRIGHT                 ; moving right -> clamp to right wall
        STA     xpos
        BRA     @xhit
@xleft: STZ     xpos                    ; clamp to left wall
@xhit:  JSR     neg_xvel                ; reverse horizontal direction
        LDA     #NOTE_X
        JSR     bounce                  ; async blip
@xok:

        ; --- vertical move + wall bounce -----------------------------------
        CLC
        LDA     ypos
        ADC     yvel
        STA     ypos
        CMP     #YTOP
        BCC     @ytop                   ; above the top wall
        CMP     #YBOT+1
        BCC     @yok                    ; YTOP..YBOT: on screen
        LDA     #YBOT                   ; hit the bottom
        STA     ypos
        BRA     @yhit
@ytop:  LDA     #YTOP                   ; hit the top
        STA     ypos
@yhit:  JSR     neg_yvel
        LDA     #NOTE_Y
        JSR     bounce
@yok:

        ; --- push the new position to hardware sprite 0 --------------------
        STZ     VGC_P0                  ; P0 = sprite 0
        LDA     xpos
        STA     VGC_P1                  ; P1 = X low
        STZ     VGC_P2                  ; P2 = X high (X < 256)
        LDA     ypos
        STA     VGC_P3                  ; P3 = Y
        JSR     sprite_pos
        JMP     loop

; --- helpers ----------------------------------------------------------------
; A = MIDI note -> play a short async bounce blip. Tail-calls audio_play_sound,
; which returns immediately (X = length in frames, Y = instrument slot 0).
bounce:
        LDX     #BLIPLEN
        LDY     #0
        JMP     audio_play_sound

neg_xvel:
        LDA     #0
        SEC
        SBC     xvel
        STA     xvel
        RTS

neg_yvel:
        LDA     #0
        SEC
        SBC     yvel
        STA     yvel
        RTS

.segment ""RODATA""
title:
        .byte   ""@@NAME@@ - bouncing ball (edit src/main.s)"", $00

; 16x16 ball, 4bpp, 2 pixels per byte (high nibble = left pixel). Colours:
; 7 body, 8 rim shade, 1 highlight, 0 transparent. 16 rows x 8 bytes.
ball_shape:
        .byte $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $08, $88, $88, $80, $00, $00
        .byte $00, $00, $88, $88, $88, $88, $00, $00
        .byte $00, $08, $87, $77, $77, $78, $80, $00
        .byte $00, $88, $11, $11, $77, $77, $88, $00
        .byte $08, $87, $11, $11, $77, $77, $78, $80
        .byte $08, $87, $11, $11, $77, $77, $78, $80
        .byte $08, $87, $11, $17, $77, $77, $78, $80
        .byte $08, $87, $77, $77, $77, $77, $78, $80
        .byte $08, $87, $77, $77, $77, $77, $78, $80
        .byte $08, $87, $77, $77, $77, $77, $78, $80
        .byte $00, $88, $77, $77, $77, $77, $88, $00
        .byte $00, $08, $87, $77, $77, $78, $80, $00
        .byte $00, $00, $88, $88, $88, $88, $00, $00
        .byte $00, $00, $08, $88, $88, $80, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00
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
