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

; Highest implemented fn-id + 1. Grows per domain batch. The draw domain $00-$09
; is live; ids $0A-$0F sit above GFX_FN_COUNT and resolve to LERR_NO_FN via the
; dispatch bounds-check (the jtable is dense only up to GFX_FN_COUNT-1).
GFX_FN_COUNT = $0A

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
      ; $0A.. grow here per domain; ids >= GFX_FN_COUNT -> LERR_NO_FN (bounds check)

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
finish_ok:
      jsr     vgc_wait_cmd
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

; Shared NDK driver bodies. vgc.s sets its own `.segment "CODE"` and pulls nova.inc
; (VGC_CMD/VCMD_GCLS) via vgc.inc; co-assembles cleanly under its .ifndef guards.
      .include "vgc.s"

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
