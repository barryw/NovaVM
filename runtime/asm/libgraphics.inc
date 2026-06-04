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
; $20-$3F  hw-sprite  (driver: runtime/asm/sprite.s)
;   define/row/clear/copy/pos/enable/disable/flip/priority/shape/setreg/
;   getx/gety + collision API (status/mask/clear/read-clear/irq)
;
; Arg-register conventions, VERIFIED against runtime/asm/sprite.s and the VGC
; ExecuteSpriteCommand mapping (VirtualGraphicsController.cs): VGC_P<n> is the
; command register $A011+n; the sprite engine reads P0=sprite-index for every
; command. Sprite X is a full 16-bit (signed) value (low byte VGC_SPR_XL, high
; byte VGC_SPR_XH); sprite Y is a single unsigned byte (VGC_SPR_YL; the YH field
; is reserved/no-op). So only X carries the s16 contract for sprites.
;
; Command-issuing ops (define/row/clear/copy/pos/enable/disable/flip/priority)
; go through sprite_command -> STA VGC_CMD; the wrapper waits via vgc_wait_cmd
; (busy bit reads 0 in the emulator -> no-op wait) and sets STATUS=OK.
; Register-store ops (shape/setreg) write the $A040 sprite attribute block
; directly -> no command, no wait. Reporter ops (getx/gety + every collision
; read) copy the driver's A/Y return into LIB_RESULT (4-byte LE, high bytes
; zeroed) then STATUS=OK.
;
; Layouts (ARGn low word unless noted; index is 0-15):
GFN_SPR_DEFINE   = $20  ; (idx:ARG0, x:ARG1 0-15, y:ARG2 0-15, color:ARG3) -> P0/P1/P2/P3; VCMD_SPRDEF
GFN_SPR_ROW      = $21  ; (idx:ARG0, row:ARG1, bytes0_3:ARG2, bytes4_7:ARG3) -> P0/P1, P2..P9; sprite_row
GFN_SPR_CLEAR    = $22  ; (idx:ARG0)                       -> P0; sprite_clear
GFN_SPR_COPY     = $23  ; (src:ARG0, dst:ARG1)             -> P0/P1; sprite_copy
GFN_SPR_POS      = $24  ; (idx:ARG0, x:ARG1 s16, y:ARG2)   -> P0, P1/P2(=x), P3(=y); sprite_pos
GFN_SPR_ENABLE   = $25  ; (idx:ARG0)                       -> P0; sprite_enable
GFN_SPR_DISABLE  = $26  ; (idx:ARG0)                       -> P0; sprite_disable
GFN_SPR_FLIP     = $27  ; (idx:ARG0, flags:ARG1)           -> P0/P1; sprite_flip
GFN_SPR_PRIORITY = $28  ; (idx:ARG0, pri:ARG1)             -> P0/P1; sprite_priority
GFN_SPR_SHAPE    = $29  ; (idx:ARG0, shape:ARG1)           -> X/A; sprite_set_shape (reg store)
GFN_SPR_SETREG   = $2A  ; (idx:ARG0, field:ARG1, val:ARG2) -> X/Y/A; sprite_set_reg8 (reg store)
GFN_SPR_SETREG16 = $2B  ; (idx:ARG0, field:ARG1, val16:ARG2) -> X/Y/NVR0L,NVR0H; sprite_set_reg16
GFN_SPR_GETX     = $2C  ; (idx:ARG0) reporter -> RESULT = 16-bit X (A=hi,Y=lo); sprite_get_x
GFN_SPR_GETY     = $2D  ; (idx:ARG0) reporter -> RESULT = Y byte (in Y); sprite_get_y
;             $2E-$2F reserved
; --- sprite-sprite collision API ($30-$35) ---
GFN_SPR_COLL_STATUS    = $30  ; () reporter -> RESULT = VGC_COLLST low byte (A); sprite_collision_status
GFN_SPR_COLL_MASK      = $31  ; () reporter -> RESULT = 16-bit mask (A=hi,Y=lo); sprite_collision_mask
GFN_SPR_COLL_CLEAR     = $32  ; () clear mask + ack IRQ; sprite_collision_clear
GFN_SPR_COLL_READCLEAR = $33  ; () reporter -> RESULT = 16-bit mask, then clear; sprite_collision_read_clear
GFN_SPR_COLL_IRQON     = $34  ; () enable sprite-sprite collision IRQ (+CLI); sprite_collision_irq_enable
GFN_SPR_COLL_IRQOFF    = $35  ; () disable sprite-sprite collision IRQ; sprite_collision_irq_disable
; --- sprite-background collision API ($36-$3B) ---
GFN_SPR_BG_STATUS      = $36  ; () reporter -> RESULT = VGC_COLLBG low byte (A); sprite_background_collision_status
GFN_SPR_BG_MASK        = $37  ; () reporter -> RESULT = 16-bit mask (A=hi,Y=lo); sprite_background_collision_mask
GFN_SPR_BG_CLEAR       = $38  ; () clear bg mask + ack IRQ; sprite_background_clear
GFN_SPR_BG_READCLEAR   = $39  ; () reporter -> RESULT = 16-bit mask, then clear; sprite_background_read_clear
GFN_SPR_BG_IRQON       = $3A  ; () enable sprite-bg collision IRQ (+CLI); sprite_background_irq_enable
GFN_SPR_BG_IRQOFF      = $3B  ; () disable sprite-bg collision IRQ; sprite_background_irq_disable
;             $3C-$3F reserved

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
