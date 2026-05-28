; ext_iface.inc — shared interface between NovaLogo base ROM and extension ROM
;
; These equates define the ZP mailbox used to pass arguments and results
; between the base ROM and the extension ROM via the RAM trampoline.
; Both ROMs include this file so addresses are always consistent.
;
; Addresses $80-$91 are outside the linker-managed ZEROPAGE segment
; but inside true zero page RAM, accessible to both ROM banks.

; --- Command and argument count ---
EXT_CMD         = $80           ; command ID (set by base ROM before trampoline call)
EXT_ARGC        = $81           ; argument count

; --- Argument 0 ---
EXT_ARG0_TYPE   = $82           ; type: VAL_NUMBER=$00, VAL_WORD=$01, VAL_LIST=$02
EXT_ARG0_HI     = $83           ; value high byte
EXT_ARG0_LO     = $84           ; value low byte
EXT_ARG0_FRAC   = $85           ; fractional byte (16.8 fixed point)

; --- Argument 1 ---
EXT_ARG1_TYPE   = $86
EXT_ARG1_HI     = $87
EXT_ARG1_LO     = $88
EXT_ARG1_FRAC   = $89

; --- Argument 2 ---
EXT_ARG2_TYPE   = $8A
EXT_ARG2_HI     = $8B
EXT_ARG2_LO     = $8C
EXT_ARG2_FRAC   = $8D

; --- Result ---
EXT_RESULT_TYPE = $8E           ; return value type
EXT_RESULT_HI   = $8F           ; return value high byte
EXT_RESULT_LO   = $90           ; return value low byte
EXT_RESULT_FRAC = $91           ; return value fractional byte

; --- Command IDs ---
EXT_CMD_TEST    = $01           ; test command: returns arg0 + 1
EXT_CMD_FD      = $10           ; FD distance — move forward
EXT_CMD_BK      = $11           ; BK distance — move backward
EXT_CMD_RT      = $12           ; RT degrees — turn right
EXT_CMD_LT      = $13           ; LT degrees — turn left
EXT_CMD_CS      = $14           ; CS — clear screen, init turtle
EXT_CMD_PU      = $15           ; PU — pen up
EXT_CMD_PD      = $16           ; PD — pen down
EXT_CMD_ST      = $17           ; ST — show turtle
EXT_CMD_HT      = $18           ; HT — hide turtle
EXT_CMD_HOME    = $19           ; HOME — move to center, heading 0
EXT_CMD_TS      = $1A           ; TEXTSCREEN — full text mode
EXT_CMD_SS      = $1B           ; SPLITSCREEN — copper split
EXT_CMD_FS      = $1C           ; FULLSCREEN — full graphics mode
EXT_CMD_SETXY   = $1D           ; SETXY x y — move to position
EXT_CMD_SETX    = $1E           ; SETX x — set X only
EXT_CMD_SETY    = $1F           ; SETY y — set Y only
EXT_CMD_SETH    = $20           ; SETHEADING — set heading degrees
EXT_CMD_XCOR    = $21           ; XCOR — report turtle X
EXT_CMD_YCOR    = $22           ; YCOR — report turtle Y
EXT_CMD_HEADING = $23           ; HEADING — report heading degrees
EXT_CMD_PENDOWNP = $24          ; PENDOWN? — report pen state
EXT_CMD_SHOWNP  = $25           ; SHOWN? — report visibility
EXT_CMD_SETPC   = $26           ; SETPC color — set pen color
EXT_CMD_SETBG   = $27           ; SETBG color — set background color
EXT_CMD_TOWARDS = $28           ; TOWARDS x y — heading towards point

; --- VGC graphics commands ---
EXT_CMD_SETCOLOR = $30          ; SETCOLOR color — set VGC draw color
EXT_CMD_PLOT     = $31          ; PLOT x y — plot pixel
EXT_CMD_UNPLOT   = $32          ; UNPLOT x y — clear pixel
EXT_CMD_LINE     = $33          ; LINE x1 y1 x2 y2 — draw line
EXT_CMD_CIRCLE   = $34          ; CIRCLE x y r — draw circle
EXT_CMD_RECT     = $35          ; RECT x1 y1 x2 y2 — draw rectangle
EXT_CMD_FILLRECT = $36          ; FILL x1 y1 x2 y2 — filled rectangle
EXT_CMD_PAINT    = $37          ; PAINT x y — flood fill

; --- Sprite commands ---
EXT_CMD_SPRITE   = $40          ; SPRITE n x y — position and enable
EXT_CMD_SPRITEPOS = $41         ; SPRITEPOS n x y — reposition
EXT_CMD_SPRITEON = $42          ; SPRITEON n — enable sprite
EXT_CMD_SPRITEOFF = $43         ; SPRITEOFF n — disable sprite
EXT_CMD_SPRCOLLP = $44          ; SPRITECOLLISION? n — collision query

; --- Sound commands ---
EXT_CMD_TONE     = $45          ; TONE freq dur — play tone
EXT_CMD_NOISE    = $46          ; NOISE dur — noise burst
EXT_CMD_VOLUME   = $47          ; VOLUME vol — set SID master volume

; --- Timing commands ---
EXT_CMD_WAIT     = $48          ; WAIT n — wait N frames
EXT_CMD_WAITVBL  = $49          ; WAITVBL — wait for next VBL
EXT_CMD_TIMER    = $4A          ; TIMER — return frame counter

; --- Value types (match eval_type in eval.s) ---
.ifndef VAL_NUMBER
VAL_NUMBER      = $00
.endif
.ifndef VAL_WORD
VAL_WORD        = $01
.endif
.ifndef VAL_LIST
VAL_LIST        = $02
.endif

; --- Turtle state — fixed ZP addresses ---
; 24-bit position (hi:lo:frac) for sub-pixel precision
TURTLE_X_FRAC   = $92
TURTLE_X_LO     = $93
TURTLE_X_HI     = $94
TURTLE_Y_FRAC   = $95
TURTLE_Y_LO     = $96
TURTLE_Y_HI     = $97
TURTLE_HEADING_LO = $98         ; degrees 0-359 (16-bit)
TURTLE_HEADING_HI = $99
TURTLE_PEN      = $9A           ; $00 = down, $01 = up
TURTLE_SHOWN    = $9B           ; $00 = hidden, $01 = shown
TURTLE_COLOR    = $9C           ; pen color (VGC palette index)
TURTLE_SPRITE   = $9D           ; sprite slot (default 15)
TURTLE_INITED   = $9E           ; $00 = not initialized, $01 = initialized

; --- Argument 3 (for 4+ arg commands like LINE, RECT, FILL) ---
EXT_ARG3_TYPE   = $9F
EXT_ARG3_HI     = $A0
EXT_ARG3_LO     = $A1
EXT_ARG3_FRAC   = $A2

; --- Trampoline location in RAM ---
EXT_TRAMPOLINE  = $0270         ; RAM address where trampoline code is copied
EXT_TRAMP_SIZE  = 14            ; size of trampoline code in bytes
