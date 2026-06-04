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
; $10-$1F  text / mode  (driver: vgc.s text-side + vtext.s)
;   COLOR(fg,bg,border) FONT MODE REVERSE/off FLASH/off LOCATE CLS CLSWIN
;   DISPLAYon/off  -> vgc_set_fg/bg/border, vgc_set_font, vgc_set_mode,
;   vgc_reverse_*, vgc_flash_on/off, vgc_locate, vgc_cls, vgc_display_on/off
;   (vtext region/cursor/puts land here too)
; (constants filled in batch 4b.3)

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
