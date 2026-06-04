; libgraphics.inc — GRAPHICS module (MODULE_ID_GRAPHICS = $01) fn-id contract.
;
; The canonical paged graphics library: the superset of every NDK graphics
; driver, callable identically by every runtime through the Phase-3 lib_call ABI.
; Built once (modules/graphics/graphics.s), staged to XRAM $060000, paged into
; bank 1 on first lib_call.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_GRAPHICS
;   LIB_FN_ID  ($0301) = one of the GFN_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;       - signed-16 X/Y values occupy the LOW word of a cell (high word ignored).
;       - BYTES args pack ptr16 in the low word, len16 in the high word.
;   JSR lib_call
;   -> LIB_RESULT ($0313) = 32-bit LE result (for reporter-style fns)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; Each fn is a thin leaf wrapper: read the arg cells -> load the driver's
; VGC_P*/register/struct inputs -> JSR the NDK driver entry -> write
; LIB_RESULT/LIB_STATUS. fn-ids are grouped per domain with room to grow.

.ifndef LIBGRAPHICS_INC
LIBGRAPHICS_INC = 1

.include "libabi.inc"            ; MODULE_ID_GRAPHICS, LIB_* mailbox cells, LERR_*

; =====================================================================
; $00-$0F  draw  (driver: vgc.s — vgc_gcls/gcolor/plot/unplot/line/circle/
;                 rect/fill/paint/gtext; primitives issue VCMD_* via vgc_cmd
;                 and the wrapper waits via vgc_wait_cmd)
; =====================================================================
GFN_GCLS      = $00   ; ()                                  -> vgc_gcls
GFN_GCOLOR    = $01   ; (color:ARG0)                        -> VGC_P0; vgc_gcolor
GFN_PLOT      = $02   ; (x:ARG0 s16, y:ARG1 s16)            -> VGC_P0..P3; vgc_plot
GFN_UNPLOT    = $03   ; (x:ARG0, y:ARG1)                    -> VGC_P0..P3; vgc_unplot
GFN_LINE      = $04   ; (x0:ARG0,y0:ARG1,x1:ARG2,y1:ARG3)   -> VGC_P0..P7; vgc_line
GFN_CIRCLE    = $05   ; (cx:ARG0,cy:ARG1,rx:ARG2,ry:ARG3)   -> VGC_P0..P7; vgc_circle (ry=rx if 0 -> ellipse superset)
GFN_RECT      = $06   ; (x0,y0,x1,y1:ARG0..3)               -> VGC_P0..P7; vgc_rect
GFN_FILL      = $07   ; (x0,y0,x1,y1:ARG0..3)               -> VGC_P0..P7; vgc_fill
GFN_PAINT     = $08   ; (x:ARG0, y:ARG1)                    -> VGC_P0..P3; vgc_paint
GFN_GTEXT     = $09   ; (x:ARG0, y:ARG1, BYTES str:ARG2)    -> VGC_P0..P5 + FIO_NAME; vgc_gtext
;             $0A-$0F reserved

; =====================================================================
; $10-$1F  text / mode  (driver: vgc.s text-side)
;   COLOR(fg,bg,border) FONT MODE REVERSE/off FLASH/off LOCATE CLS CLSWIN
;   DISPLAYon/off  -> vgc_set_fg/bg/border, vgc_set_font, vgc_set_mode,
;   vgc_reverse_default/off, vgc_flash_on/off, vgc_locate, vgc_cls,
;   vgc_display_on/off.
;
; Arg-register conventions, verified against runtime/asm/vgc.s:
;   - vgc_set_fg/bg/border take X (STX VGC_FGCOL/BGCOL/BORDER) — pure register
;     stores, no VGC command issued, no wait needed.
;   - vgc_set_font / vgc_set_mode take X (STX VGC_FONT / VGC_MODE) — register
;     stores, no wait.
;   - vgc_locate reads VGC_P0 (col) -> VGC_CURSX and VGC_P1 (row) -> VGC_CURSY,
;     so the wrapper loads ARG0->VGC_P0, ARG1->VGC_P1 — register stores, no wait.
;   - vgc_reverse_default/off, vgc_flash_on/off mutate VGC_TXTFLAGS only — no wait.
;   - vgc_display_on/off store $0F/$00 to VGC_DIMMER — register store, no wait.
;   - vgc_cls issues a char-out ($0C form-feed) command and waits internally.
;
; NOTE: vtext.s (region/cursor/puts, 16 ops) does NOT fit alongside these in
; $10-$1F; it gets its own fn-id range in a later batch. CLSWIN has no driver
; entry in vgc.s, so GFN_CLSWIN is reserved here and stubbed to gfn_unimpl
; (-> LERR_NO_FN) until a window-clear driver routine exists.
GFN_COLOR      = $10   ; (fg:ARG0, bg:ARG1, border:ARG2) -> vgc_set_fg/bg/border (X each)
GFN_FONT       = $11   ; (slot:ARG0)                      -> X; vgc_set_font
GFN_MODE       = $12   ; (mode:ARG0)                      -> X; vgc_set_mode
GFN_REVERSE    = $13   ; ()  reverse-on                   -> vgc_reverse_default
GFN_REVERSEOFF = $14   ; ()                               -> vgc_reverse_off
GFN_FLASH      = $15   ; ()                               -> vgc_flash_on
GFN_FLASHOFF   = $16   ; ()                               -> vgc_flash_off
GFN_LOCATE     = $17   ; (col:ARG0, row:ARG1)             -> VGC_P0/P1; vgc_locate
GFN_CLS        = $18   ; ()                               -> vgc_cls (issues cmd, waits)
GFN_CLSWIN     = $19   ; ()  clear active window          -> STUB (no vgc entry) -> gfn_unimpl
GFN_DISPLAYON  = $1A   ; ()                               -> vgc_display_on  (VGC_DIMMER=$0F)
GFN_DISPLAYOFF = $1B   ; ()                               -> vgc_display_off (VGC_DIMMER=$00)
;             $1C-$1F reserved

; =====================================================================
; $20-$3F  hw-sprite  (driver: sprite.s)
;   define/row/clear/copy/pos/enable/disable/flip/priority/shape/setreg/
;   getx/gety + collision API (status/mask/clear/read-clear/irq)
; (constants filled in batch 4b.4)

; =====================================================================
; $40-$4F  copper  (drivers: copper.s + copper_split.s)
;   list/add/clear/on/off/use/end/split/set_reg/set_sprite_reg
; (constants filled in batch 4b.5)

; =====================================================================
; $50-$5F  blit / dma  (drivers: blitter.s + dma.s)
;   blitcopy/blitfill/blit-start/wait/dmacopy/dmafill + status/err/count
; (constants filled in batch 4b.6)

; =====================================================================
; $60-$7F  vsprite  (driver: vsprite.s)
;   blit/fill/rotate/save-restore-bg/gfx_* + scene compositor
; (constants filled in batch 4b.7)

; =====================================================================
; $80-$9F  msprite  (driver: msprite.s)
;   spawn/destroy/show/hide/pos/frame/anim/priority/transcolor/tick/commit
; (constants filled in batch 4b.8)

; =====================================================================
; $A0-$AF  image / mem  (drivers: nvg.s + vgc mem)
;   GSAVE/GLOAD/NVGload x4/MEMREAD/MEMWRITE/VPOKE/VPEEK
; (constants filled in batch 4b.9)

; =====================================================================
; $B0-$BF  turtle-render  (the rendering ops the resident turtle calls in 4c)
;   draw-line / draw+rotate turtle-icon sprite / erase-icon / ensure-gfx-mode
; (constants filled in batch 4b.10)

; =====================================================================
; $C0-$CF  anim  (driver: anim.s)   — superset extension beyond design §2
; $D0-$DF  tween (driver: tween.s)  — superset extension beyond design §2
; (constants filled in batch 4b.11)

.endif
