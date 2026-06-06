; libturtle.inc — TURTLE module (MODULE_ID_TURTLE = $07) fn-id contract.
;
; The turtle is a reusable cross-language graphics service: FD/RT/pen/heading
; move-math + Amiga-BOB rotate-blit render. It was lifted whole out of the
; GRAPHICS module (it does not fit the Logo base ROM, and it is not Logo-specific
; — any runtime can drive it via lib_call). Semantics + render moved together; no
; splitting. Turtle state stays at $9F00 (shared RAM, visible while the module is
; paged), so callers and tests read X/Y/HEADING there unchanged.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_TURTLE
;   LIB_FN_ID  ($0301) = one of the TUR_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result (reporters only)
;   -> LIB_STATUS ($0302) = LERR_OK ($00)
;
; TUR_INIT/DRAW/ERASE are the low-level render fns (built-in icon install, the
; erase-old + save-bg + rotate-blit-keyed stamp, and the saved-bg restore). TUR_OP
; is the op-dispatched command engine: ARG2 byte0 = a turtle command id (TOP_*,
; mirroring the legacy EXT_CMD_* values); value args ride in ARG0/ARG1 as Logo
; 16.8 fixed point (cell byte0 = FRAC, byte1 = LO, byte2 = HI). Commands set
; LIB_STATUS = LERR_OK; reporters write a Logo 16.8 value to LIB_RESULT low word.
;
; DRAW move ops frame-wait inside the rotate-blit, so the caller (or test) must
; PUMP the bus while the fn runs.

.ifndef LIBTURTLE_INC
LIBTURTLE_INC = 1

.include "libabi.inc"            ; MODULE_ID_TURTLE, LIB_* mailbox cells, LERR_*

; =====================================================================
; Turtle module function ids.
; =====================================================================
TUR_INIT  = $00   ; () install built-in icon into module BSS source buffer; reset bg bookkeeping
TUR_DRAW  = $01   ; (x:ARG0 s16 center, y:ARG1 s16 center, angle:ARG2 u8) erase-old + save-bg + rotate-blit-keyed
TUR_ERASE = $02   ; () restore saved bg
TUR_OP    = $03   ; (op:ARG2 byte0, args:ARG0/ARG1 Logo 16.8) turtle command engine

; Highest implemented fn-id + 1. ids >= TUR_FN_COUNT -> LERR_NO_FN.
TUR_FN_COUNT = $04

; Turtle op-codes carried in ARG2 byte0 (mirror novalogo/ext_iface.inc EXT_CMD_*).
TOP_FD       = $10   ; FD distance
TOP_BK       = $11   ; BK distance
TOP_RT       = $12   ; RT degrees
TOP_LT       = $13   ; LT degrees
TOP_CS       = $14   ; CLEARSCREEN / DRAW
TOP_PU       = $15   ; PENUP
TOP_PD       = $16   ; PENDOWN
TOP_ST       = $17   ; SHOWTURTLE
TOP_HT       = $18   ; HIDETURTLE
TOP_HOME     = $19   ; HOME
TOP_TS       = $1A   ; TEXTSCREEN
TOP_SS       = $1B   ; SPLITSCREEN
TOP_FS       = $1C   ; FULLSCREEN
TOP_SETXY    = $1D   ; SETXY x y
TOP_SETX     = $1E   ; SETX x
TOP_SETY     = $1F   ; SETY y
TOP_SETH     = $20   ; SETHEADING degrees
TOP_XCOR     = $21   ; XCOR (reporter)
TOP_YCOR     = $22   ; YCOR (reporter)
TOP_HEADING  = $23   ; HEADING (reporter)
TOP_PENDOWNP = $24   ; PENDOWN? (reporter)
TOP_SHOWNP   = $25   ; SHOWN? (reporter)
TOP_SETPC    = $26   ; SETPENCOLOR color
TOP_SETBG    = $27   ; SETBACKGROUND color
TOP_TOWARDS  = $28   ; TOWARDS x y (reporter)
; $29 was SETPOS [x y]. The module is 100% numeric ops now; the [x y] cons-list
; unpack moved to the Logo adapter (novalogo/eval.s logo_adapt_turtle @setpos),
; which drives the generic SETXY ($1D) op. The module no longer implements $29.

.endif
