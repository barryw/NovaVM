; graphics.s — GRAPHICS module (MODULE_ID_GRAPHICS = $01), the canonical paged
; graphics library. Built once, staged to XRAM $060000, paged into bank 1 on the
; first lib_call. Header "NL"/$01 at $C000 (lib_module_header); RTS-trick dispatch
; on LIB_FN_ID; thin leaf wrappers over the shared NDK driver bodies (vgc.s).
;
; This skeleton (Stage 4b.1) implements one fn — GFN_GCLS — proven end-to-end on
; both test axes (loader path + hardware VGC effect). Remaining domains land in
; 4b.2..4b.11; their jtable slots resolve to gfn_unimpl until then.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libgraphics.inc"
      .include "vgc.inc"               ; nova.inc -> NVR0L/NVR0H ZP scratch + VGC_* regs
                                       ; (guarded; re-included by vgc.s below)

; Highest implemented fn-id + 1. Grows per domain batch. The draw domain $00-$09
; is live; the text/mode domain $10-$1B is live (batch 4b.3); the hw-sprite domain
; $20-$3B is live (batch 4b.4); the copper domain $40-$49 is live (batch 4b.5).
; The jtable is dense from $00..GFX_FN_COUNT-1: implemented ids point at their
; wrapper, every gap id ($0A-$0F, $19 CLSWIN, $1C-$1F, $2E-$2F, $3C-$3F) points
; at gfn_unimpl so it resolves to LERR_NO_FN. ids >= GFX_FN_COUNT ($4A+) resolve
; to LERR_NO_FN via the dispatch bounds-check.
GFX_FN_COUNT = $4A

; GTEXT copies its BYTES string into the VGC FIO_NAME buffer ($B9B0-$B9EF, 64
; bytes). Mirror fio.inc's FIO_NAME_LIMIT here (fio.inc isn't pulled into the
; module) to clamp the copy length to the buffer.
GFX_FIO_NAME_LIMIT = $3F

      .segment "CODE"
      lib_module_header MODULE_ID_GRAPHICS, LIB_ABI_VERSION, GFX_FN_COUNT

; dispatch — fn-id router. RTS-trick: push (target-1) hi/lo, RTS jumps to target.
dispatch:
      lda     LIB_FN_ID
      cmp     #GFX_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     gfx_jtable+1,x
      pha
      lda     gfx_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

gfx_jtable:
      .word   gfn_gcls-1               ; $00 GCLS
      .word   gfn_gcolor-1             ; $01 GCOLOR
      .word   gfn_plot-1               ; $02 PLOT
      .word   gfn_unplot-1             ; $03 UNPLOT
      .word   gfn_line-1               ; $04 LINE
      .word   gfn_circle-1             ; $05 CIRCLE
      .word   gfn_rect-1               ; $06 RECT
      .word   gfn_fill-1               ; $07 FILL
      .word   gfn_paint-1              ; $08 PAINT
      .word   gfn_gtext-1              ; $09 GTEXT
      .word   gfn_unimpl-1             ; $0A gap -> LERR_NO_FN
      .word   gfn_unimpl-1             ; $0B gap
      .word   gfn_unimpl-1             ; $0C gap
      .word   gfn_unimpl-1             ; $0D gap
      .word   gfn_unimpl-1             ; $0E gap
      .word   gfn_unimpl-1             ; $0F gap
      .word   gfn_color-1              ; $10 COLOR
      .word   gfn_font-1               ; $11 FONT
      .word   gfn_mode-1               ; $12 MODE
      .word   gfn_reverse-1            ; $13 REVERSE
      .word   gfn_reverseoff-1         ; $14 REVERSEOFF
      .word   gfn_flash-1              ; $15 FLASH
      .word   gfn_flashoff-1           ; $16 FLASHOFF
      .word   gfn_locate-1             ; $17 LOCATE
      .word   gfn_cls-1                ; $18 CLS
      .word   gfn_unimpl-1             ; $19 CLSWIN -> LERR_NO_FN (no vgc driver entry)
      .word   gfn_displayon-1          ; $1A DISPLAYON
      .word   gfn_displayoff-1         ; $1B DISPLAYOFF
      .word   gfn_unimpl-1             ; $1C gap
      .word   gfn_unimpl-1             ; $1D gap
      .word   gfn_unimpl-1             ; $1E gap
      .word   gfn_unimpl-1             ; $1F gap
      .word   gfn_spr_define-1         ; $20 SPRDEFINE
      .word   gfn_spr_row-1            ; $21 SPRROW
      .word   gfn_spr_clear-1          ; $22 SPRCLEAR
      .word   gfn_spr_copy-1           ; $23 SPRCOPY
      .word   gfn_spr_pos-1            ; $24 SPRPOS
      .word   gfn_spr_enable-1         ; $25 SPRENABLE
      .word   gfn_spr_disable-1        ; $26 SPRDISABLE
      .word   gfn_spr_flip-1           ; $27 SPRFLIP
      .word   gfn_spr_priority-1       ; $28 SPRPRIORITY
      .word   gfn_spr_shape-1          ; $29 SPRSHAPE
      .word   gfn_spr_setreg-1         ; $2A SPRSETREG  (reg8)
      .word   gfn_spr_setreg16-1       ; $2B SPRSETREG16
      .word   gfn_spr_getx-1           ; $2C SPRGETX  (reporter)
      .word   gfn_spr_gety-1           ; $2D SPRGETY  (reporter)
      .word   gfn_unimpl-1             ; $2E gap
      .word   gfn_unimpl-1             ; $2F gap
      .word   gfn_spr_coll_status-1    ; $30 SPRCOLLSTATUS (reporter)
      .word   gfn_spr_coll_mask-1      ; $31 SPRCOLLMASK   (reporter)
      .word   gfn_spr_coll_clear-1     ; $32 SPRCOLLCLEAR
      .word   gfn_spr_coll_readclear-1 ; $33 SPRCOLLREADCLEAR (reporter)
      .word   gfn_spr_coll_irqon-1     ; $34 SPRCOLLIRQON
      .word   gfn_spr_coll_irqoff-1    ; $35 SPRCOLLIRQOFF
      .word   gfn_spr_bg_status-1      ; $36 SPRBGSTATUS (reporter)
      .word   gfn_spr_bg_mask-1        ; $37 SPRBGMASK   (reporter)
      .word   gfn_spr_bg_clear-1       ; $38 SPRBGCLEAR
      .word   gfn_spr_bg_readclear-1   ; $39 SPRBGREADCLEAR (reporter)
      .word   gfn_spr_bg_irqon-1       ; $3A SPRBGIRQON
      .word   gfn_spr_bg_irqoff-1      ; $3B SPRBGIRQOFF
      .word   gfn_unimpl-1             ; $3C gap
      .word   gfn_unimpl-1             ; $3D gap
      .word   gfn_unimpl-1             ; $3E gap
      .word   gfn_unimpl-1             ; $3F gap
      .word   gfn_copper_list-1        ; $40 COPPER_LIST
      .word   gfn_copper_add-1         ; $41 COPPER_ADD
      .word   gfn_copper_clear-1       ; $42 COPPER_CLEAR
      .word   gfn_copper_on-1          ; $43 COPPER_ON
      .word   gfn_copper_off-1         ; $44 COPPER_OFF
      .word   gfn_copper_use-1         ; $45 COPPER_USE
      .word   gfn_copper_end-1         ; $46 COPPER_END
      .word   gfn_copper_split-1       ; $47 COPPER_SPLIT
      .word   gfn_copper_set_reg-1     ; $48 COPPER_SET_REG
      .word   gfn_copper_set_sprite_reg-1 ; $49 COPPER_SET_SPRITE_REG
      ; $4A.. grow here per domain; ids >= GFX_FN_COUNT -> LERR_NO_FN (bounds check)

; Any reachable-but-unimplemented fn-id: report LERR_NO_FN.
gfn_unimpl:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

; copy_args_to_p — copy A arg-words (A in 1..4) from LIB_ARG0.. into VGC_P0..
;   Each arg cell is 4 bytes (LE 32-bit); only its low word (2 bytes) is used and
;   lands in 2 consecutive VGC_P regs. arg cell i (i=0..A-1): LIB_ARG0+i*4 low
;   byte -> VGC_P0+i*2, +1 high byte -> VGC_P0+i*2+1. The module runs from the
;   write-protected ROM window, so loop state lives in RAM scratch (LIB_SCRATCH),
;   never in CODE space. X walks the 4-byte source stride, Y the 2-byte dest
;   stride; the word counter in LIB_SCRATCH drives the loop A times.
GFX_WORDCNT = LIB_SCRATCH            ; remaining arg-words to copy
copy_args_to_p:
      sta     GFX_WORDCNT              ; word count (1..4)
      ldx     #0                       ; X = source byte index (0,4,8,12)
      ldy     #0                       ; Y = dest byte index (0,2,4,6)
@loop:
      lda     LIB_ARG0,x               ; arg word low byte -> VGC_P[2i]
      sta     VGC_P0,y
      lda     LIB_ARG0+1,x             ; arg word high byte -> VGC_P[2i+1]
      sta     VGC_P0+1,y
      iny                              ; dest += 2
      iny
      inx                              ; source += 4
      inx
      inx
      inx
      dec     GFX_WORDCNT
      bne     @loop
      rts

; finish_ok — common tail: wait for the VGC command to complete, then STATUS=OK.
; For ops that issue a VCMD_* via vgc_cmd (the draw primitives).
finish_ok:
      jsr     vgc_wait_cmd
      ; fall through to finish_ok_nowait

; finish_ok_nowait — STATUS=OK with no command wait. For pure register-store ops
; (set_fg/bg/border, set_font/mode, locate, reverse/flash, display_on/off) that
; never issue a VGC command, so there is no busy bit to poll.
finish_ok_nowait:
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; --- $00 GCLS: clear the gfx plane ---
; Issues VCMD_GCLS via vgc_gcls (does not wait) then vgc_wait_cmd so the caller
; (and the test) observes the cleared plane before the fn returns.
gfn_gcls:
      jsr     vgc_gcls
      jmp     finish_ok

; --- $01 GCOLOR: set the gfx draw colour ---
; color = low byte of ARG0 -> VGC_P0 (1-byte special; VCMD_GCOLOR reads only P0).
gfn_gcolor:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     vgc_gcolor
      jmp     finish_ok

; --- $02 PLOT: set pixel (x,y) ---  ARG0->P0/P1 (x s16), ARG1->P2/P3 (y s16)
gfn_plot:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_plot
      jmp     finish_ok

; --- $03 UNPLOT: clear pixel (x,y) ---  ARG0->P0/P1, ARG1->P2/P3
gfn_unplot:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_unplot
      jmp     finish_ok

; --- $04 LINE: (x0,y0)-(x1,y1) ---  ARG0->P0/P1, ARG1->P2/P3, ARG2->P4/P5, ARG3->P6/P7
gfn_line:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_line
      jmp     finish_ok

; --- $05 CIRCLE: centre (cx,cy), radii (rx,ry) ---  ARG0..3 -> P0..P7
; ry=0 degenerates to a circle of radius rx (handled downstream in vgc/RTL).
gfn_circle:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_circle
      jmp     finish_ok

; --- $06 RECT: outline (x0,y0)-(x1,y1) ---  ARG0..3 -> P0..P7
gfn_rect:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_rect
      jmp     finish_ok

; --- $07 FILL: filled rect (x0,y0)-(x1,y1) ---  ARG0..3 -> P0..P7
gfn_fill:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_fill
      jmp     finish_ok

; --- $08 PAINT: flood fill from (x,y) ---  ARG0->P0/P1, ARG1->P2/P3
gfn_paint:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_paint
      jmp     finish_ok

; --- $09 GTEXT: draw BYTES string at (x,y) ---
; ARG0->P0/P1 (x), ARG1->P2/P3 (y). ARG2 packs ptr16 (low word) + len16 (high
; word) of the source string; the contract carries no font/scale args, so this
; wrapper uses the defaults the driver expects: P4 = font slot 0, P5 = scale 1.
; The string is copied into FIO_NAME (len bytes) with FIO_NAMELEN = len, matching
; the vgc_gtext @requires (VGC_P0..P5 + FIO_NAME/FIO_NAMELEN).
gfn_gtext:
      lda     #2                       ; x,y -> P0/P1, P2/P3
      jsr     copy_args_to_p
      lda     #0                       ; P4 = font slot 0 (default)
      sta     VGC_P4
      lda     #1                       ; P5 = scale 1 (default)
      sta     VGC_P5
      ; src ptr16 = ARG2 low word -> module-private ZP pointer LIB_ZP for (zp),Y
      lda     LIB_ARG2
      sta     LIB_ZP
      lda     LIB_ARG2+1
      sta     LIB_ZP+1
      ; len = ARG2 high word low byte; clamp to the FIO name buffer limit
      lda     LIB_ARG2+2
      cmp     #GFX_FIO_NAME_LIMIT+1
      bcc     @lenok
      lda     #GFX_FIO_NAME_LIMIT
@lenok:
      sta     FIO_NAMELEN
      cmp     #0
      beq     @copied                  ; empty string -> nothing to copy
      tax                              ; X = byte count
      ldy     #0                       ; Y = shared src/dest index
@copyloop:
      lda     (LIB_ZP),y
      sta     FIO_NAME,y
      iny
      dex
      bne     @copyloop
@copied:
      jsr     vgc_gtext
      jmp     finish_ok

; =====================================================================
; $10-$1F  text / mode domain (batch 4b.3). All register-store ops (no VGC
; command), so each tail is finish_ok_nowait — except CLS, which issues a
; char-out command and waits inside vgc_cls before we set OK.
; =====================================================================

; --- $10 COLOR: set text fg/bg/border ---  fg=ARG0, bg=ARG1, border=ARG2.
; Each driver entry takes the colour byte in X (STX VGC_FGCOL/BGCOL/BORDER).
gfn_color:
      ldx     LIB_ARG0
      jsr     vgc_set_fg
      ldx     LIB_ARG1
      jsr     vgc_set_bg
      ldx     LIB_ARG2
      jsr     vgc_set_border
      jmp     finish_ok_nowait

; --- $11 FONT: select text font slot ---  slot=ARG0 -> X; vgc_set_font.
gfn_font:
      ldx     LIB_ARG0
      jsr     vgc_set_font
      jmp     finish_ok_nowait

; --- $12 MODE: set graphics/text mode ---  mode=ARG0 -> X; vgc_set_mode.
gfn_mode:
      ldx     LIB_ARG0
      jsr     vgc_set_mode
      jmp     finish_ok_nowait

; --- $13 REVERSE: enable reverse text using current fg/bg ---
gfn_reverse:
      jsr     vgc_reverse_default
      jmp     finish_ok_nowait

; --- $14 REVERSEOFF: disable reverse text ---
gfn_reverseoff:
      jsr     vgc_reverse_off
      jmp     finish_ok_nowait

; --- $15 FLASH: enable flashing text ---
gfn_flash:
      jsr     vgc_flash_on
      jmp     finish_ok_nowait

; --- $16 FLASHOFF: disable flashing text ---
gfn_flashoff:
      jsr     vgc_flash_off
      jmp     finish_ok_nowait

; --- $17 LOCATE: set text cursor (col,row) ---  col=ARG0 -> VGC_P0, row=ARG1 -> VGC_P1.
; vgc_locate copies VGC_P0 -> VGC_CURSX and VGC_P1 -> VGC_CURSY.
gfn_locate:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     vgc_locate
      jmp     finish_ok_nowait

; --- $18 CLS: clear the text screen ---  vgc_cls issues the $0C form-feed via
; VGC_CHAROUT and waits internally; we just set OK afterwards (no extra wait).
gfn_cls:
      jsr     vgc_cls
      jmp     finish_ok_nowait

; --- $1A DISPLAYON: restore full brightness ---  VGC_DIMMER = $0F.
gfn_displayon:
      jsr     vgc_display_on
      jmp     finish_ok_nowait

; --- $1B DISPLAYOFF: blank output, keep timing ---  VGC_DIMMER = $00.
gfn_displayoff:
      jsr     vgc_display_off
      jmp     finish_ok_nowait

; =====================================================================
; $20-$3B  hw-sprite domain (batch 4b.4). Driver: sprite.s (included below).
;
; Command-issuing ops load VGC_P* then issue the command and wait (finish_ok).
; The sprite engine reads P0 = sprite index for every command. Register-store
; ops write the $A040 attribute block directly (finish_ok_nowait). Reporter ops
; copy the driver's A/Y return into LIB_RESULT then set OK (finish_result_*).
; =====================================================================

; --- reporter result helpers (RAM scratch only — they write the LIB_RESULT cell) ---
; finish_result16 — Y = result low byte, A = result high byte -> LIB_RESULT
;   (4-byte LE; bytes 2-3 zeroed), STATUS=OK.
finish_result16:
      sty     LIB_RESULT
      sta     LIB_RESULT+1
      lda     #0
      sta     LIB_RESULT+2
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; finish_result8 — A = result byte -> LIB_RESULT low byte (bytes 1-3 zeroed), OK.
finish_result8:
      sta     LIB_RESULT
      lda     #0
      sta     LIB_RESULT+1
      sta     LIB_RESULT+2
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; --- $20 SPRDEFINE: set one shape pixel of sprite to a colour ---
; idx:ARG0->P0, x(0-15):ARG1->P1, y(0-15):ARG2->P2, color:ARG3->P3. VCMD_SPRDEF.
gfn_spr_define:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      lda     LIB_ARG2
      sta     VGC_P2
      lda     LIB_ARG3
      sta     VGC_P3
      lda     #VCMD_SPRDEF
      jsr     sprite_command
      jmp     finish_ok

; --- $21 SPRROW: write one 16px shape row (8 nibble-packed bytes) ---
; idx:ARG0->P0, row:ARG1->P1, then 8 shape bytes ARG2[0..3]->P2..P5,
; ARG3[0..3]->P6..P9. sprite_row issues VCMD_SPRROW (reads P0..P9).
gfn_spr_row:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      ; ARG2 (4 bytes) -> P2..P5, ARG3 (4 bytes) -> P6..P9
      ldx     #0
@rowloop:
      lda     LIB_ARG2,x
      sta     VGC_P2,x
      inx
      cpx     #8
      bne     @rowloop
      jsr     sprite_row
      jmp     finish_ok

; --- $22 SPRCLEAR: clear sprite shape data ---  idx:ARG0->P0.
gfn_spr_clear:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_clear
      jmp     finish_ok

; --- $23 SPRCOPY: copy shape data src->dst ---  src:ARG0->P0, dst:ARG1->P1.
gfn_spr_copy:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_copy
      jmp     finish_ok

; --- $24 SPRPOS: move sprite ---  idx:ARG0->P0, x(s16):ARG1->P1/P2, y:ARG2->P3.
gfn_spr_pos:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1                 ; x low  -> P1
      sta     VGC_P1
      lda     LIB_ARG1+1               ; x high -> P2
      sta     VGC_P2
      lda     LIB_ARG2                 ; y      -> P3
      sta     VGC_P3
      jsr     sprite_pos
      jmp     finish_ok

; --- $25 SPRENABLE: enable sprite ---  idx:ARG0->P0.
gfn_spr_enable:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_enable
      jmp     finish_ok

; --- $26 SPRDISABLE: disable sprite ---  idx:ARG0->P0.
gfn_spr_disable:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_disable
      jmp     finish_ok

; --- $27 SPRFLIP: set flip flags ---  idx:ARG0->P0, flags:ARG1->P1.
gfn_spr_flip:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_flip
      jmp     finish_ok

; --- $28 SPRPRIORITY: set priority ---  idx:ARG0->P0, pri:ARG1->P1.
gfn_spr_priority:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_priority
      jmp     finish_ok

; --- $29 SPRSHAPE: set sprite shape-slot index ---  idx:ARG0->X, shape:ARG1->A.
; sprite_set_shape writes the VGC_SPR_SHAPE attribute register (no command).
gfn_spr_shape:
      ldx     LIB_ARG0
      lda     LIB_ARG1
      jsr     sprite_set_shape
      jmp     finish_ok_nowait

; --- $2A SPRSETREG: write one 8-bit sprite attribute field ---
; idx:ARG0->X, field:ARG1->Y, value:ARG2->A. sprite_set_reg8 (no command).
gfn_spr_setreg:
      ldx     LIB_ARG0
      ldy     LIB_ARG1
      lda     LIB_ARG2
      jsr     sprite_set_reg8
      jmp     finish_ok_nowait

; --- $2B SPRSETREG16: write one 16-bit sprite attribute field ---
; idx:ARG0->X, field:ARG1->Y, value16:ARG2->NVR0L/NVR0H. sprite_set_reg16.
gfn_spr_setreg16:
      ldx     LIB_ARG0
      ldy     LIB_ARG1
      lda     LIB_ARG2
      sta     NVR0L
      lda     LIB_ARG2+1
      sta     NVR0H
      jsr     sprite_set_reg16
      jmp     finish_ok_nowait

; --- $2C SPRGETX: read sprite 16-bit X (reporter) ---  idx:ARG0->X.
; sprite_get_x returns A=X high, Y=X low -> LIB_RESULT 16-bit.
gfn_spr_getx:
      ldx     LIB_ARG0
      jsr     sprite_get_x
      jmp     finish_result16

; --- $2D SPRGETY: read sprite Y byte (reporter) ---  idx:ARG0->X.
; sprite_get_y returns Y = Y byte -> move to A -> LIB_RESULT 8-bit.
gfn_spr_gety:
      ldx     LIB_ARG0
      jsr     sprite_get_y
      tya
      jmp     finish_result8

; --- $30 SPRCOLLSTATUS: sprite-sprite collision low byte (reporter) ---
gfn_spr_coll_status:
      jsr     sprite_collision_status
      jmp     finish_result8

; --- $31 SPRCOLLMASK: sprite-sprite 16-bit collision mask (reporter) ---
; sprite_collision_mask returns A=hi (sprites 8-15), Y=lo (sprites 0-7).
gfn_spr_coll_mask:
      jsr     sprite_collision_mask
      jmp     finish_result16

; --- $32 SPRCOLLCLEAR: clear sprite-sprite mask + ack IRQ ---
gfn_spr_coll_clear:
      jsr     sprite_collision_clear
      jmp     finish_ok_nowait

; --- $33 SPRCOLLREADCLEAR: read 16-bit mask then clear (reporter) ---
gfn_spr_coll_readclear:
      jsr     sprite_collision_read_clear
      jmp     finish_result16

; --- $34 SPRCOLLIRQON: enable sprite-sprite collision IRQ (+CLI) ---
gfn_spr_coll_irqon:
      jsr     sprite_collision_irq_enable
      jmp     finish_ok_nowait

; --- $35 SPRCOLLIRQOFF: disable sprite-sprite collision IRQ ---
gfn_spr_coll_irqoff:
      jsr     sprite_collision_irq_disable
      jmp     finish_ok_nowait

; --- $36 SPRBGSTATUS: sprite-background collision low byte (reporter) ---
gfn_spr_bg_status:
      jsr     sprite_background_collision_status
      jmp     finish_result8

; --- $37 SPRBGMASK: sprite-background 16-bit collision mask (reporter) ---
gfn_spr_bg_mask:
      jsr     sprite_background_collision_mask
      jmp     finish_result16

; --- $38 SPRBGCLEAR: clear sprite-background mask + ack IRQ ---
gfn_spr_bg_clear:
      jsr     sprite_background_clear
      jmp     finish_ok_nowait

; --- $39 SPRBGREADCLEAR: read 16-bit bg mask then clear (reporter) ---
gfn_spr_bg_readclear:
      jsr     sprite_background_read_clear
      jmp     finish_result16

; --- $3A SPRBGIRQON: enable sprite-bg collision IRQ (+CLI) ---
gfn_spr_bg_irqon:
      jsr     sprite_background_irq_enable
      jmp     finish_ok_nowait

; --- $3B SPRBGIRQOFF: disable sprite-bg collision IRQ ---
gfn_spr_bg_irqoff:
      jsr     sprite_background_irq_disable
      jmp     finish_ok_nowait

; =====================================================================
; $40-$49  copper domain (batch 4b.5). Drivers: copper.s + copper_split.s.
;
; The copper edits/plays raster display lists. Every command-issuing entry goes
; through copper_command -> STA VGC_CMD; the wrapper waits via vgc_wait_cmd and
; sets STATUS=OK (finish_ok). copper_add reads P0/P1=x, P2=y, P3/P4=register
; specifier, P5=value. List-select ops (list/use) read P0. set_reg/set_sprite_reg
; expose the COMPLETE add-a-register-write op: prep P3/P4 via the matching
; register helper, load x/y/value, then issue copper_add (see libgraphics.inc).
; =====================================================================

; --- $40 COPPER_LIST: select the list that ADD/CLEAR edit ---  idx:ARG0->P0.
gfn_copper_list:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     copper_list
      jmp     finish_ok

; --- $41 COPPER_ADD: add a register-write event to the target list ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, reg(16):ARG2->P3/P4, value:ARG3->P5.
gfn_copper_add:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG2                 ; reg low  -> P3
      sta     VGC_P3
      lda     LIB_ARG2+1               ; reg high -> P4
      sta     VGC_P4
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      jsr     copper_add
      jmp     finish_ok

; --- $42 COPPER_CLEAR: empty the target list ---  ().
gfn_copper_clear:
      jsr     copper_clear
      jmp     finish_ok

; --- $43 COPPER_ON: enable copper execution ---  ().
gfn_copper_on:
      jsr     copper_on
      jmp     finish_ok

; --- $44 COPPER_OFF: disable copper execution ---  ().
gfn_copper_off:
      jsr     copper_off
      jmp     finish_ok

; --- $45 COPPER_USE: make a list active at the next vblank ---  idx:ARG0->P0.
gfn_copper_use:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     copper_use
      jmp     finish_ok

; --- $46 COPPER_END: finish the current list (target := active) ---  ().
gfn_copper_end:
      jsr     copper_list_end
      jmp     finish_ok

; --- $47 COPPER_SPLIT: one list, two VGC_MODE rules at a scanline split ---
; idx:ARG0->P0, splitY:ARG1->P1, mode0:ARG2->P2, mode1:ARG3->P3.
; copper_split_mode runs the full off/list/clear/add/add/use/on sequence,
; waiting internally between commands; it leaves the copper enabled.
gfn_copper_split:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      lda     LIB_ARG2
      sta     VGC_P2
      lda     LIB_ARG3
      sta     VGC_P3
      jsr     copper_split_mode
      jmp     finish_ok

; --- $48 COPPER_SET_REG: add a copper write to a direct VGC register ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, regIndex:ARG2->A (copper_set_reg_index sets
; P3/P4), value:ARG3->P5; then copper_add. Observable CopperEvent.
gfn_copper_set_reg:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      lda     LIB_ARG2                 ; register index -> A
      jsr     copper_set_reg_index     ; -> P3/P4
      jsr     copper_add
      jmp     finish_ok

; --- $49 COPPER_SET_SPRITE_REG: add a copper write to a sprite attribute reg ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, sprIdx:ARG2 low->X, field:ARG2 byte1->A
; (copper_set_sprite_reg sets P3/P4 = absolute sprite reg addr), value:ARG3->P5;
; then copper_add. Observable CopperEvent.
gfn_copper_set_sprite_reg:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      ldx     LIB_ARG2                 ; sprite index -> X
      lda     LIB_ARG2+1               ; field offset -> A
      jsr     copper_set_sprite_reg    ; -> P3/P4
      jsr     copper_add
      jmp     finish_ok

; Shared NDK driver bodies. vgc.s sets its own `.segment "CODE"` and pulls nova.inc
; (VGC_CMD/VCMD_GCLS) via vgc.inc; co-assembles cleanly under its .ifndef guards.
; sprite.s provides the hw-sprite command/register/collision driver entries.
; copper.s adds the copper command/list/add/register-prep entries; copper_split.s
; adds copper_split_mode (it pulls vgc.s + copper.s, all dedup'd by .ifndef guards).
      .include "vgc.s"
      .include "sprite.s"
      .include "copper.s"
      .include "copper_split.s"

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
