; turtle.s — TURTLE module (MODULE_ID_TURTLE = $07). The whole NovaLogo turtle
; engine (move-math semantics + rotate-blit render together), lifted verbatim out
; of the GRAPHICS module. Turtle graphics is a reusable cross-language graphics
; service any runtime can drive via lib_call; it did not fit the Logo base ROM, so
; it lives here as its own demand-loadable module.
;
; The turtle is a 16x16 color-keyed virtual sprite (an upward-triangle icon) drawn
; on the gfx plane with Amiga-BOB save/restore of the background. Turtle state
; stays at $9F00 (shared RAM, visible while the module is paged — tests peek it).
; The render path calls the module's OWN vgc.s / vsprite.s helpers directly (no
; nested lib_call); the math copro drives sincos / 16x16 mul / atan2.
;
; Four fns (libturtle.inc): TUR_INIT/DRAW/ERASE are the low-level render subs;
; TUR_OP is the op-dispatched command engine (ARG2 byte0 = TOP_* op id). The NDK
; driver bodies (vgc/sprite/copper/copper_split/blitter/dma/vsprite) are included
; at the BOTTOM so selective-emit (.referenced) keeps only the routines the turtle
; actually calls.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libturtle.inc"
      .include "vgc.inc"               ; nova.inc -> VGC_*/MATH_* regs + NVR* ZP scratch
      .include "vsprite.inc"           ; VSPRITE_* state symbols + BLT_SPACE_CPU/flags
      .include "copper_split.inc"      ; copper_split_mode .global

      .segment "CODE"
      lib_module_header MODULE_ID_TURTLE, LIB_ABI_VERSION, TUR_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into turtle.nmod). The
; turtle is NOT a thin NDK wrapper (it owns the move-math + render), so no ;@ndk
; mappings — the drift guard (--ndk-dir) finds nothing to check, which is correct.
; ===========================================================================
;@module TURTLE
;@version 1.0
;@brief Turtle graphics service: FD/RT/pen/heading move-math + rotate-blit render.
;
;@fn TUR_INIT
;@brief Install the built-in turtle icon into the module source buffer; reset bg bookkeeping.
;@ret void
;@status LERR_OK
;
;@fn TUR_DRAW
;@brief Erase the old turtle, save the background, then rotate-and-keyed-blit the turtle at a new centre.
;@arg x s16 turtle centre x (ARG0)
;@arg y s16 turtle centre y (ARG1)
;@arg angle u8 NDK rotation-angle byte (ARG2 byte0)
;@ret void
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn TUR_ERASE
;@brief Restore the saved background, erasing the turtle (no-op if nothing saved).
;@ret void
;@status LERR_OK
;
;@fn TUR_OP
;@brief NovaLogo turtle command engine (op-dispatched: ARG2 byte0 = a TOP_* command id; value args in ARG0/ARG1 as Logo 16.8).
;@arg op u8 turtle command id (TOP_*) (ARG2 byte0)
;@effect PUMPED: turtle DRAW move ops frame-wait — pump the bus.
;@ret void
;@status LERR_OK

; ===========================================================================
; dispatch — fn-id router (RTS-trick). TUR_FN_COUNT is small so fn*2 < 256.
; ===========================================================================
dispatch:
      lda     LIB_FN_ID
      cmp     #TUR_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     tur_jtable+1,x
      pha
      lda     tur_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

tur_jtable:
      .word   gfn_turtle_init-1        ; $00 TUR_INIT
      .word   gfn_turtle_draw-1        ; $01 TUR_DRAW
      .word   gfn_turtle_erase-1       ; $02 TUR_ERASE
      .word   gfn_turtle_op-1          ; $03 TUR_OP — turtle command engine

; finish_ok_nowait — STATUS=OK with no command wait. The turtle handlers tail into
; this (commands always OK; reporters published their value first). finish_vs maps
; a VSPRITE_RESULT_* code into LIB_STATUS for the DRAW save-bg/rotate path.
finish_ok_nowait:
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; finish_vs — publish the vsprite driver's A result (raw -> RESULT byte0), then
; map VSPRITE_RESULT_OK -> LERR_OK, else LERR_VSPRITE_FAIL. Verbatim port of the
; GRAPHICS module's finish_vs (the DRAW save-bg/rotate-blit path uses it).
LERR_VSPRITE_FAIL = $84   ; vsprite driver returned A!=0 (blitter error / bad geometry)
finish_vs:
      sta     LIB_RESULT               ; raw driver A -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #VSPRITE_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_VSPRITE_FAIL
      sta     LIB_STATUS
      rts

; =====================================================================
; $00/$01/$02  turtle-render domain. The turtle is a 16x16 color-keyed virtual
; sprite (an upward triangle icon) drawn on the gfx plane with Amiga-BOB save/
; restore of the background. The module owns the source/rotated/saved-bg buffers +
; bg bookkeeping in its BSS (declared below). DRAW reads the center from the
; mailbox (ARG0=x, ARG1=y, ARG2=angle). The vsprite driver helpers
; (vsprite_gfx_save_bg/restore_bg/rotate_blit_keyed) are linked from vsprite.s
; (included below). All loop/temp state is RAM (module BSS) — the module runs from
; write-protected ROM.
TURTLE_SPR_HALF = 8
TURTLE_MAX_X    = 304            ; 320 - 16
TURTLE_MAX_Y    = 184            ; 200 - 16
TURTLE_COL_WHITE = 1             ; Nova palette white index

; cfg_turtle_vsprite — configure the VSPRITE_* cells for the 16x16 turtle.
; ORIG/ROT/BG all point at the module-BSS buffers (all CPU RAM, bank-independent
; — the PROVEN path). Mirrors configure_turtle_vsprite (extension.s:666-705).
cfg_turtle_vsprite:
      lda     #16
      sta     VSPRITE_WIDTHL
      stz     VSPRITE_WIDTHH
      sta     VSPRITE_HEIGHTL
      stz     VSPRITE_HEIGHTH
      sta     VSPRITE_ORIGSTRL
      stz     VSPRITE_ORIGSTRH
      sta     VSPRITE_ROTSTRL
      stz     VSPRITE_ROTSTRH
      sta     VSPRITE_BGSTRL
      stz     VSPRITE_BGSTRH

      lda     #BLT_SPACE_CPU
      sta     VSPRITE_ORIGSPACE
      sta     VSPRITE_ROTSPACE
      sta     VSPRITE_BGSPACE

      lda     #<turtle_source_shape
      sta     VSPRITE_ORIGADDRL
      lda     #>turtle_source_shape
      sta     VSPRITE_ORIGADDRM
      stz     VSPRITE_ORIGADDRH

      lda     #<turtle_rotated_shape
      sta     VSPRITE_ROTADDRL
      lda     #>turtle_rotated_shape
      sta     VSPRITE_ROTADDRM
      stz     VSPRITE_ROTADDRH

      lda     #<turtle_saved_bg
      sta     VSPRITE_BGADDRL
      lda     #>turtle_saved_bg
      sta     VSPRITE_BGADDRM
      stz     VSPRITE_BGADDRH

      stz     VSPRITE_COLORKEY
      lda     #VSPRITE_FLAG_COLORKEY
      sta     VSPRITE_FLAGS
      rts

; set_turtle_pos — center the 16x16 stamp on (x,y) read from ARG0/ARG1 and clamp
; to the plane. Verbatim port of set_turtle_vsprite_pos (extension.s:710-764)
; with the center taken from the mailbox (LIB_ARG0 = x lo/hi, LIB_ARG1 = y lo/hi)
; and the clamp scratch (tmp0/tmp1) in module BSS. Result -> VSPRITE_XL/XH/Y.
set_turtle_pos:
      sec
      lda     LIB_ARG0                 ; x low
      sbc     #TURTLE_SPR_HALF
      sta     turtle_tmp0
      lda     LIB_ARG0+1               ; x high
      sbc     #0
      sta     turtle_tmp1
      bmi     @x_zero
      cmp     #>TURTLE_MAX_X
      bcc     @x_ok
      bne     @x_max
      lda     turtle_tmp0
      cmp     #<TURTLE_MAX_X
      bcc     @x_ok
      beq     @x_ok
@x_max:
      lda     #<TURTLE_MAX_X
      sta     VSPRITE_XL
      lda     #>TURTLE_MAX_X
      sta     VSPRITE_XH
      bra     @y
@x_zero:
      stz     VSPRITE_XL
      stz     VSPRITE_XH
      bra     @y
@x_ok:
      lda     turtle_tmp0
      sta     VSPRITE_XL
      lda     turtle_tmp1
      sta     VSPRITE_XH
@y:
      ; Y max (184) is single-byte, so the high borrow stays in A and is tested
      ; in place (bmi/bne) rather than stashed like the two-byte X clamp above.
      sec
      lda     LIB_ARG1                 ; y low
      sbc     #TURTLE_SPR_HALF
      sta     turtle_tmp0
      lda     LIB_ARG1+1               ; y high
      sbc     #0
      bmi     @y_zero
      bne     @y_max
      lda     turtle_tmp0
      cmp     #(TURTLE_MAX_Y + 1)
      bcc     @y_ok
@y_max:
      lda     #TURTLE_MAX_Y
      sta     VSPRITE_Y
      rts
@y_zero:
      stz     VSPRITE_Y
      rts
@y_ok:
      lda     turtle_tmp0
      sta     VSPRITE_Y
      rts

; turtle_do_erase — restore the saved background if one is pending, then clear
; the flag. Shared by ERASE and the erase-old step of DRAW. Mirrors
; erase_turtle_sprite (extension.s:788-801).
turtle_do_erase:
      lda     turtle_bg_saved
      beq     @done
      jsr     cfg_turtle_vsprite
      lda     turtle_bg_x_lo
      sta     VSPRITE_XL
      lda     turtle_bg_x_hi
      sta     VSPRITE_XH
      lda     turtle_bg_y
      sta     VSPRITE_Y
      jsr     vsprite_gfx_restore_bg
      stz     turtle_bg_saved
@done:
      rts

; --- $00 TUR_INIT: install the built-in icon + reset bg bookkeeping. ---
; Copies the 256-byte turtle_icon ROM constant into turtle_source_shape and zeros
; turtle_bg_saved (so a stale saved-background is never restored onto a freshly
; cleared plane). One-time setup AND the CLEARSCREEN re-init hook. Always OK.
gfn_turtle_init:
      ldx     #0
@copy:
      lda     turtle_icon,x
      sta     turtle_source_shape,x
      inx
      bne     @copy                    ; exactly 256 bytes
      stz     turtle_bg_saved
      ; Arm the BSS canary: the icon + BOB buffers now hold valid data. This word
      ; lets a later op detect that the (volatile, shared) module BSS was clobbered
      ; by another paged module — see turtle_ensure_bss.
      lda     #TURTLE_BSS_MAGIC0
      sta     turtle_bss_magic
      lda     #TURTLE_BSS_MAGIC1
      sta     turtle_bss_magic+1
      jmp     finish_ok_nowait

; =====================================================================
; turtle_ensure_bss — re-install the volatile BSS state if it was clobbered.
; The icon buffer, BOB save/restore buffers, and bg bookkeeping live in the module
; BSS band ($0420+), which the paged-library loader REUSES for whichever module is
; resident. TURTLE_INITED lives in surviving RAM ($9F00), so after another module
; (e.g. the editor) runs between turtle ops, INITED still reads 1 but the icon
; buffer is garbage. The canary word in BSS only survives while the turtle stays
; resident; a mismatch means we were paged out -> re-install the icon (gfn_turtle_init
; also clears bg_saved, so no stale background is restored). Called at the top of
; every turtle command so the next op after a page-in always blits a clean icon.
; =====================================================================
TURTLE_BSS_MAGIC0 = $5A
TURTLE_BSS_MAGIC1 = $A5
turtle_ensure_bss:
      lda     turtle_bss_magic
      cmp     #TURTLE_BSS_MAGIC0
      bne     @reinstall
      lda     turtle_bss_magic+1
      cmp     #TURTLE_BSS_MAGIC1
      bne     @reinstall
      rts                              ; canary intact -> BSS valid
@reinstall:
      ; Re-copy the icon into the (clobbered) source buffer and re-arm the canary.
      ; NOTE: we do NOT clear turtle_bg_saved here — the BOB save/restore bookkeeping
      ; must stay intact so the next move still erases the previous stamp (clearing
      ; it strands the pre-page-out stamp as a ghost).
      ldx     #0
@copy:
      lda     turtle_icon,x
      sta     turtle_source_shape,x
      inx
      bne     @copy
      lda     #TURTLE_BSS_MAGIC0
      sta     turtle_bss_magic
      lda     #TURTLE_BSS_MAGIC1
      sta     turtle_bss_magic+1
      rts

; --- $01 TUR_DRAW: erase-old + save-bg + rotate-blit-keyed. ---
; x:ARG0 s16 center, y:ARG1 s16 center, angle:ARG2 byte0 u8. Faithful port of
; draw_turtle_sprite (extension.s:806-827): restore any pending bg, configure the
; vsprite cells, center+clamp the stamp, record the stamp position, save the new
; bg (bail to finish_vs on failure — no blit, no bg_saved), then stamp the angle
; and rotate-blit-keyed. PUMPED on the test side (rotate-blit waits on VGC_FRAME).
gfn_turtle_draw:
      jsr     turtle_do_erase          ; restore previously-saved bg (if any)
      jsr     cfg_turtle_vsprite
      jsr     set_turtle_pos           ; center+clamp -> VSPRITE_XL/XH/Y
      lda     VSPRITE_XL
      sta     turtle_bg_x_lo
      lda     VSPRITE_XH
      sta     turtle_bg_x_hi
      lda     VSPRITE_Y
      sta     turtle_bg_y
      jsr     vsprite_gfx_save_bg
      cmp     #VSPRITE_RESULT_OK
      beq     @saved                   ; save-bg ok -> stamp the turtle
      jmp     finish_vs                ; save-bg failed: publish A, don't blit
@saved:
      lda     #$01
      sta     turtle_bg_saved
      lda     LIB_ARG2                 ; angle byte 0
      sta     VSPRITE_ROTANGLE
      jsr     vsprite_gfx_rotate_blit_keyed_nowf
      jmp     finish_vs

; --- $02 TUR_ERASE: restore the saved bg (no-op if nothing saved). ---
; Always OK (if nothing to erase, still OK). Port of erase_turtle_sprite.
gfn_turtle_erase:
      jsr     turtle_do_erase
      jmp     finish_ok_nowait

; =====================================================================
; $03 TUR_OP — the whole NovaLogo turtle command engine. Op-dispatched: ARG2
; byte0 = a turtle command id (TOP_*, mirroring the legacy EXT_CMD_* values).
; Value args ride in ARG0/ARG1 as Logo 16.8 fixed point — cell byte0 = FRAC,
; byte1 = LO, byte2 = HI (the layout the foundation adapter marshals; see
; libturtle.inc). The move-math (sincos / 16x16 multiply / 24-bit accumulate) is
; a FAITHFUL port of novalogo/turtle.s (itself faithful to novalogo/extension.s);
; the render path calls the module's OWN vgc.s helpers + $00/$01/$02 render subs
; directly.
;
; Turtle state stays at $9F00 (shared RAM, accessible while the module is paged).
; Commands set LIB_STATUS = LERR_OK; reporters write a Logo 16.8 value to the
; LIB_RESULT low word and set OK.
; =====================================================================

; --- Turtle state at $9F00 (shared RAM; tests read X/Y/HEADING here). Mirrors
;     novalogo/extension.s:75-93 / turtle.s:50-64. ---
TURTLE_STATE_BASE  = $9F00
TURTLE_X_FRAC      = TURTLE_STATE_BASE + 0
TURTLE_X_LO        = TURTLE_STATE_BASE + 1
TURTLE_X_HI        = TURTLE_STATE_BASE + 2
TURTLE_Y_FRAC      = TURTLE_STATE_BASE + 3
TURTLE_Y_LO        = TURTLE_STATE_BASE + 4
TURTLE_Y_HI        = TURTLE_STATE_BASE + 5
TURTLE_HEADING_LO  = TURTLE_STATE_BASE + 6
TURTLE_HEADING_HI  = TURTLE_STATE_BASE + 7
TURTLE_PEN         = TURTLE_STATE_BASE + 8   ; 0 = down, 1 = up
TURTLE_SHOWN       = TURTLE_STATE_BASE + 9
TURTLE_COLOR       = TURTLE_STATE_BASE + 10
TURTLE_SPRITE      = TURTLE_STATE_BASE + 11
TURTLE_INITED      = TURTLE_STATE_BASE + 12
; $9F0D-$9F10 = bg bookkeeping owned by the module render BSS — leave it.
TURTLE_GFX_VISIBLE = TURTLE_STATE_BASE + 17  ; 1 = split/full graphics active

; --- Turtle constants ---
; The Logo coordinate origin (0,0) is the CENTER of the full 320x200 plane, so
; the turtle's screen-pixel home is (160,100). Internally the turtle state stays
; in screen pixels; the op boundary translates to/from centered Logo coordinates
; (XCOR/YCOR report signed, +Y up; SETXY/SETX/SETY/TOWARDS accept the same):
;   logo -> screen:  sx = lx + 160   sy = 100 - ly
;   screen -> logo:  lx = sx - 160   ly = 100 - sy
TURTLE_CENTER_X    = 160         ; screen x of Logo origin (0,0)
TURTLE_CENTER_Y    = 100         ; screen y of Logo origin (0,0) — full-plane center
TURTLE_SPLIT_Y     = 160         ; copper split: gfx rows 0-159, text below
TURTLE_MODE_GFXSPR = 3           ; VGC mode: graphics + sprites
TURTLE_MODE_TEXT   = 0           ; VGC mode: text only

; --- ARG cell sub-byte aliases (Logo 16.8: [0]=FRAC, [1]=LO, [2]=HI). ---
TA0_FRAC = LIB_ARG0+0
TA0_LO   = LIB_ARG0+1
TA0_HI   = LIB_ARG0+2
TA1_FRAC = LIB_ARG1+0
TA1_LO   = LIB_ARG1+1
TA1_HI   = LIB_ARG1+2

; --- $03 dispatcher: read op from ARG2 byte0, route via an internal RTS-trick
;     jump table to the per-op handler. Op ids are EXT_CMD values ($10..$28); we
;     subtract $10 to index a dense 25-entry table. Out-of-range op -> OK no-op
;     (commands always OK; an unknown op is a harmless no-op). ---
gfn_turtle_op:
      cld                              ; turtle math is binary, never BCD (ext_entry)
      jsr     turtle_ensure_bss        ; re-install icon/bg if the module BSS was paged out
      lda     LIB_ARG2                 ; op id (byte 0)
      sec
      sbc     #$10                     ; TOP_FD ($10) -> index 0
      bcc     @oknoop                  ; op < $10 -> no-op OK
      cmp     #(top_table_end - top_table) / 2
      bcs     @oknoop                  ; op past the table -> no-op OK
      asl                              ; *2 for the word table (max index 25 -> $32, fits 8-bit)
      tax
      lda     top_table+1,x            ; handler-1 hi
      pha
      lda     top_table,x              ; handler-1 lo
      pha
      rts                              ; jump to handler (each ends in jmp finish_*)
@oknoop:
      jmp     finish_ok_nowait

top_table:
      .word   t_fd-1                   ; $10 FD
      .word   t_bk-1                   ; $11 BK
      .word   t_rt-1                   ; $12 RT
      .word   t_lt-1                   ; $13 LT
      .word   t_cs-1                   ; $14 CS
      .word   t_pu-1                   ; $15 PU
      .word   t_pd-1                   ; $16 PD
      .word   t_st-1                   ; $17 ST
      .word   t_ht-1                   ; $18 HT
      .word   t_home-1                 ; $19 HOME
      .word   t_ts-1                   ; $1A TS
      .word   t_ss-1                   ; $1B SS
      .word   t_fs-1                   ; $1C FS
      .word   t_setxy-1                ; $1D SETXY
      .word   t_setx-1                 ; $1E SETX
      .word   t_sety-1                 ; $1F SETY
      .word   t_seth-1                 ; $20 SETH
      .word   t_xcor-1                 ; $21 XCOR (reporter)
      .word   t_ycor-1                 ; $22 YCOR (reporter)
      .word   t_heading-1              ; $23 HEADING (reporter)
      .word   t_pendownp-1             ; $24 PENDOWN? (reporter)
      .word   t_shownp-1               ; $25 SHOWN? (reporter)
      .word   t_setpc-1                ; $26 SETPC
      .word   t_setbg-1                ; $27 SETBG
      .word   t_towards-1              ; $28 TOWARDS (reporter)
top_table_end:

; =====================================================================
; reset_pos_heading — snap X/Y to viewport center, zero the fixed-point fracs
;   and the heading. Shared by turtle_init_state (full reset) and t_home @go
;   (move-to-center). (item 6 dedup)
; =====================================================================
reset_pos_heading:
      lda     #TURTLE_CENTER_X
      sta     TURTLE_X_LO
      stz     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      lda     #TURTLE_CENTER_Y
      sta     TURTLE_Y_LO
      stz     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      stz     TURTLE_HEADING_LO
      stz     TURTLE_HEADING_HI
      rts

; =====================================================================
; Logo<->screen coordinate translation. The turtle state is stored in screen
; pixels; SETXY/SETX/SETY accept centered Logo coords and XCOR/YCOR report them.
;   screen_x_from_TA0:  TURTLE_X = (TA0 lx) + 160,  frac 0   (SETX / SETXY x)
;   screen_y_from_TA0:  TURTLE_Y = 100 - (TA0 ly),  frac 0   (SETY)
;   screen_y_from_TA1:  TURTLE_Y = 100 - (TA1 ly),  frac 0   (SETXY y)
; 16-bit signed add/sub of the center constant — a negative Logo coord lands on
; the correct screen pixel (e.g. lx=-50 -> sx=110). Frac is dropped (SETXY is
; integer, matching the prior behavior).
; =====================================================================
screen_x_from_TA0:
      clc
      lda     TA0_LO
      adc     #<TURTLE_CENTER_X
      sta     TURTLE_X_LO
      lda     TA0_HI
      adc     #>TURTLE_CENTER_X
      sta     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      rts
screen_y_from_TA0:
      sec
      lda     #<TURTLE_CENTER_Y
      sbc     TA0_LO
      sta     TURTLE_Y_LO
      lda     #>TURTLE_CENTER_Y
      sbc     TA0_HI
      sta     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      rts
screen_y_from_TA1:
      sec
      lda     #<TURTLE_CENTER_Y
      sbc     TA1_LO
      sta     TURTLE_Y_LO
      lda     #>TURTLE_CENTER_Y
      sbc     TA1_HI
      sta     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      rts

; =====================================================================
; WRAP — default Logo edge behavior: the turtle reappears on the opposite edge.
; The plane is the full 320 x 200 screen.
;
; compute_wrap — from the UNWRAPPED screen position in TURTLE_X/Y (LO/HI), derive:
;   wrap_wx/wy   = position reduced into [0,320) x [0,200)  (the committed pos)
;   wrap_dxoff   = TURTLE_X - wrap_wx   (= tiles_x * 320; sign gives wrap direction)
;   wrap_dyoff   = TURTLE_Y - wrap_wy   (= tiles_y * 200)
; TURTLE_X/Y themselves are NOT modified (the pen line still needs the unwrapped
; endpoint). The frac bytes are irrelevant here — wrapping only shifts whole pixels.
; =====================================================================
TURTLE_PLANE_W = 320
TURTLE_PLANE_H = 200

compute_wrap:
      lda     TURTLE_X_LO
      sta     wrap_wx_lo
      lda     TURTLE_X_HI
      sta     wrap_wx_hi
      jsr     reduce_wx_mod_w          ; wrap_wx -> [0,320)
      sec
      lda     TURTLE_X_LO
      sbc     wrap_wx_lo
      sta     wrap_dxoff_lo
      lda     TURTLE_X_HI
      sbc     wrap_wx_hi
      sta     wrap_dxoff_hi

      lda     TURTLE_Y_LO
      sta     wrap_wy_lo
      lda     TURTLE_Y_HI
      sta     wrap_wy_hi
      jsr     reduce_wy_mod_h          ; wrap_wy -> [0,200)
      sec
      lda     TURTLE_Y_LO
      sbc     wrap_wy_lo
      sta     wrap_dyoff_lo
      lda     TURTLE_Y_HI
      sbc     wrap_wy_hi
      sta     wrap_dyoff_hi
      rts

; reduce_wx_mod_w — reduce the signed 16-bit wrap_wx into [0,320). Add/subtract
; 320 until in range (move displacements are small, so few iterations).
reduce_wx_mod_w:
@chk:
      lda     wrap_wx_hi
      bmi     @add                     ; negative -> add a plane width
      cmp     #>TURTLE_PLANE_W         ; hi vs 1
      bcc     @done                    ; hi < 1 -> < 320
      bne     @sub                     ; hi > 1 -> >= 320
      lda     wrap_wx_lo
      cmp     #<TURTLE_PLANE_W         ; hi == 1: lo vs $40
      bcc     @done
@sub:
      sec
      lda     wrap_wx_lo
      sbc     #<TURTLE_PLANE_W
      sta     wrap_wx_lo
      lda     wrap_wx_hi
      sbc     #>TURTLE_PLANE_W
      sta     wrap_wx_hi
      bra     @chk
@add:
      clc
      lda     wrap_wx_lo
      adc     #<TURTLE_PLANE_W
      sta     wrap_wx_lo
      lda     wrap_wx_hi
      adc     #>TURTLE_PLANE_W
      sta     wrap_wx_hi
      bra     @chk
@done:
      rts

; reduce_wy_mod_h — reduce the signed 16-bit wrap_wy into [0,200).
reduce_wy_mod_h:
@chk:
      lda     wrap_wy_hi
      bmi     @add                     ; negative -> add a plane height
      bne     @sub                     ; hi >= 1 -> >= 256 -> >= 200
      lda     wrap_wy_lo
      cmp     #<TURTLE_PLANE_H         ; hi == 0: lo vs 200
      bcc     @done
@sub:
      sec
      lda     wrap_wy_lo
      sbc     #<TURTLE_PLANE_H
      sta     wrap_wy_lo
      lda     wrap_wy_hi
      sbc     #>TURTLE_PLANE_H
      sta     wrap_wy_hi
      bra     @chk
@add:
      clc
      lda     wrap_wy_lo
      adc     #<TURTLE_PLANE_H
      sta     wrap_wy_lo
      lda     wrap_wy_hi
      adc     #>TURTLE_PLANE_H
      sta     wrap_wy_hi
      bra     @chk
@done:
      rts

; =====================================================================
; draw_wrapped_pen_line — draw the pen line old -> (unwrapped) TURTLE_X/Y on a
; torus: for each plane-tile the move spans, draw the line shifted by that tile
; and let the VGC clip the off-plane part. The X offsets step by +/-320 from 0 to
; wrap_dxoff; the Y offsets by +/-200 from 0 to wrap_dyoff. The common (no-wrap)
; case has dxoff = dyoff = 0 -> a single line, exactly the pre-WRAP behavior.
; =====================================================================
draw_wrapped_pen_line:
      jsr     vgc_wait_cmd             ; set the pen color once
      lda     TURTLE_COLOR
      sta     VGC_P0
      jsr     vgc_gcolor
      jsr     vgc_wait_cmd
      stz     woff_x_lo                ; off_x = 0
      stz     woff_x_hi
@xloop:
      stz     woff_y_lo                ; off_y = 0
      stz     woff_y_hi
@yloop:
      jsr     draw_one_shifted_line
      ; inner done when off_y == dyoff
      lda     woff_y_lo
      cmp     wrap_dyoff_lo
      bne     @ystep
      lda     woff_y_hi
      cmp     wrap_dyoff_hi
      beq     @xnext
@ystep:
      bit     wrap_dyoff_hi            ; N = sign of dyoff -> step direction
      bmi     @ysub
      clc
      lda     woff_y_lo
      adc     #<TURTLE_PLANE_H
      sta     woff_y_lo
      lda     woff_y_hi
      adc     #>TURTLE_PLANE_H
      sta     woff_y_hi
      bra     @yloop
@ysub:
      sec
      lda     woff_y_lo
      sbc     #<TURTLE_PLANE_H
      sta     woff_y_lo
      lda     woff_y_hi
      sbc     #>TURTLE_PLANE_H
      sta     woff_y_hi
      bra     @yloop
@xnext:
      ; outer done when off_x == dxoff
      lda     woff_x_lo
      cmp     wrap_dxoff_lo
      bne     @xstep
      lda     woff_x_hi
      cmp     wrap_dxoff_hi
      beq     @done
@xstep:
      bit     wrap_dxoff_hi
      bmi     @xsub
      clc
      lda     woff_x_lo
      adc     #<TURTLE_PLANE_W
      sta     woff_x_lo
      lda     woff_x_hi
      adc     #>TURTLE_PLANE_W
      sta     woff_x_hi
      bra     @xloop
@xsub:
      sec
      lda     woff_x_lo
      sbc     #<TURTLE_PLANE_W
      sta     woff_x_lo
      lda     woff_x_hi
      sbc     #>TURTLE_PLANE_W
      sta     woff_x_hi
      bra     @xloop
@done:
      rts

; draw_one_shifted_line — VGC line from (old - woff) to (new - woff), signed 16-bit
; endpoints (the VGC clips off-plane). old = turtle_old_x/y, new = TURTLE_X/Y.
draw_one_shifted_line:
      jsr     vgc_wait_cmd
      sec
      lda     turtle_old_x_lo
      sbc     woff_x_lo
      sta     VGC_P0
      lda     turtle_old_x_hi
      sbc     woff_x_hi
      sta     VGC_P1
      sec
      lda     turtle_old_y_lo
      sbc     woff_y_lo
      sta     VGC_P2
      lda     turtle_old_y_hi
      sbc     woff_y_hi
      sta     VGC_P3
      sec
      lda     TURTLE_X_LO
      sbc     woff_x_lo
      sta     VGC_P4
      lda     TURTLE_X_HI
      sbc     woff_x_hi
      sta     VGC_P5
      sec
      lda     TURTLE_Y_LO
      sbc     woff_y_lo
      sta     VGC_P6
      lda     TURTLE_Y_HI
      sbc     woff_y_hi
      sta     VGC_P7
      jsr     vgc_line
      jmp     vgc_wait_cmd             ; tail

; =====================================================================
; turtle_init_state — initialize persistent turtle state, then install the icon
;   into the module source buffer + reset bg bookkeeping (via gfn_turtle_init).
;   Mirrors turtle_init_state (turtle.s:98-119) / turtle_init (extension.s:562).
; =====================================================================
turtle_init_state:
      jsr     reset_pos_heading        ; X/Y -> center, heading 0
      stz     TURTLE_PEN               ; pen down
      lda     #$01
      sta     TURTLE_SHOWN
      lda     #TURTLE_COL_WHITE
      sta     TURTLE_COLOR
      lda     #$01
      sta     TURTLE_INITED
      jmp     gfn_turtle_init          ; install icon + reset bg (sets STATUS OK; we ignore)

; =====================================================================
; turtle_enter_split — copper split: gfx+sprites top, text below; mark visible.
;   Direct call into the module's copper_split_mode (the GFN_COPPER_SPLIT body).
;   NOTE (deviation): the legacy ext_ss also runs prepare_split_text to carve the
;   text window + park the cursor. That touches runtime-owned TEXTWIN/cursor state
;   the module does not own, so it is intentionally NOT ported here — the gfx-mode
;   enable (the part that makes drawing land) is the module's concern; the text
;   window split is the foundation adapter's. The gfx plane (what the tests read)
;   is written regardless of display mode.
; =====================================================================
turtle_enter_split:
      stz     VGC_P0                   ; idx = 0
      lda     #TURTLE_SPLIT_Y
      sta     VGC_P1                   ; splitY = 160
      lda     #TURTLE_MODE_GFXSPR
      sta     VGC_P2                   ; mode0 = gfx+sprites (top)
      lda     #TURTLE_MODE_TEXT
      sta     VGC_P3                   ; mode1 = text (bottom)
      jsr     copper_split_mode
      lda     #$01
      sta     TURTLE_GFX_VISIBLE
      rts

; =====================================================================
; ensure_gfx_mode — init the turtle if needed; if still in full text mode, auto
;   switch to split screen so drawing is visible + show the turtle there.
;   Mirrors ensure_gfx_mode (turtle.s:126-136).
; =====================================================================
ensure_gfx_mode:
      lda     TURTLE_INITED
      bne     @check_mode
      jsr     turtle_init_state
@check_mode:
      lda     TURTLE_GFX_VISIBLE
      bne     @done
      jsr     turtle_enter_split
      jsr     draw_turtle
@done:
      rts

; =====================================================================
; turtle_save_old — snapshot current integer position to BSS old_x/y (the line
;   start for the post-move pen line). Mirrors turtle_save_old (turtle.s:160).
; =====================================================================
turtle_save_old:
      lda     TURTLE_X_LO
      sta     turtle_old_x_lo
      lda     TURTLE_X_HI
      sta     turtle_old_x_hi
      lda     TURTLE_Y_LO
      sta     turtle_old_y_lo
      lda     TURTLE_Y_HI
      sta     turtle_old_y_hi
      rts

; =====================================================================
; draw_turtle — redraw the rotated turtle at the current position via the
;   module's own $01 render sub. gfn_turtle_draw erases the previous stamp
;   internally, so do NOT pre-erase. No-op when hidden. Mirrors draw_turtle
;   (turtle.s:176-192).
;
;   gfn_turtle_draw consumes the mailbox cells (ARG0/ARG1 lo+hi, ARG2 byte0),
;   so draw_turtle SAVES the caller's value-arg bytes first and RESTORES them
;   afterward — it is called both as a final render step AND as a pre-step inside
;   ensure_gfx_mode (first gfx entry), where it would otherwise clobber the value
;   args a move/set handler has not yet read. Keeping draw_turtle args-transparent
;   removes that ordering hazard for every handler. (The legacy draw_turtle_sprite
;   was immune because its args lived in ZP it never touched.)
; =====================================================================
draw_turtle:
      lda     TURTLE_SHOWN
      beq     @done
      ; snapshot the value-arg bytes gfn_turtle_draw overwrites
      lda     LIB_ARG0+0
      sta     t_save_a0_0
      lda     LIB_ARG0+1
      sta     t_save_a0_1
      lda     LIB_ARG1+0
      sta     t_save_a1_0
      lda     LIB_ARG1+1
      sta     t_save_a1_1
      lda     LIB_ARG2+0
      sta     t_save_a2_0
      ; marshal X/Y/angle and stamp the turtle
      lda     TURTLE_X_LO
      sta     LIB_ARG0+0
      lda     TURTLE_X_HI
      sta     LIB_ARG0+1
      lda     TURTLE_Y_LO
      sta     LIB_ARG1+0
      lda     TURTLE_Y_HI
      sta     LIB_ARG1+1
      jsr     turtle_heading_u8        ; A = NDK angle byte
      sta     LIB_ARG2+0
      jsr     gfn_turtle_draw          ; erase-old + save-bg + rotate-blit (sets STATUS)
      ; restore the caller's value-arg bytes
      lda     t_save_a0_0
      sta     LIB_ARG0+0
      lda     t_save_a0_1
      sta     LIB_ARG0+1
      lda     t_save_a1_0
      sta     LIB_ARG1+0
      lda     t_save_a1_1
      sta     LIB_ARG1+1
      lda     t_save_a2_0
      sta     LIB_ARG2+0
@done:
      rts

; =====================================================================
; turtle_render — shared post-move render: lift the old turtle, draw the pen line
;   (if pen down) old->new, then redraw the turtle at the new position. Reads BSS
;   old_x/y for the line start; TURTLE_X/Y for the end. Mirrors turtle_render
;   (turtle.s:206-246) but draws the line DIRECTLY via the module's vgc.s
;   (= draw_line, extension.s:627).
;
;   ORDER MATTERS: erase the old turtle FIRST, BEFORE the line. The turtle is a
;   gfx-plane BOB (save-bg/restore-bg), so its saved background under the old
;   footprint predates this line. If we drew the line first, the post-line
;   draw_turtle's internal restore-bg would write that stale background back over
;   the ~16px of fresh line passing under the old position — gapping the line at
;   every waypoint. The legacy ext_fd erased before draw_line for exactly this
;   reason (extension.s:273 erase precedes :398 line). With bg_saved cleared here,
;   draw_turtle's own internal erase then no-ops.
; =====================================================================
turtle_render:
      jsr     gfn_turtle_erase         ; lift the old turtle before the line (see ORDER note)
      ; WRAP: the move-math left TURTLE_X/Y as the UNWRAPPED screen position (it may
      ; sit off the plane). compute_wrap derives the wrapped target + the per-axis
      ; tile offset WITHOUT mutating TURTLE_X/Y, so the pen line below can still be
      ; drawn from old -> unwrapped-new (tile-shifted so the opposite-edge segment
      ; lands), and only THEN do we commit the wrapped position.
      jsr     compute_wrap
      lda     TURTLE_PEN
      bne     @no_line                 ; pen up ($01) -> skip line
      jsr     draw_wrapped_pen_line     ; old -> unwrapped-new, drawn wrapped across edges
@no_line:
      ; commit the wrapped position (integer part only; the sub-pixel frac is kept).
      lda     wrap_wx_lo
      sta     TURTLE_X_LO
      lda     wrap_wx_hi
      sta     TURTLE_X_HI
      lda     wrap_wy_lo
      sta     TURTLE_Y_LO
      lda     wrap_wy_hi
      sta     TURTLE_Y_HI
      jmp     draw_turtle              ; tail: redraw turtle at the wrapped position

; =====================================================================
; heading_mod360 — reduce TURTLE_HEADING to 0-359. (turtle.s:251)
; =====================================================================
heading_mod360:
@loop:
      lda     TURTLE_HEADING_HI
      beq     @done                    ; hi = 0 -> always < 360
      cmp     #>360                    ; hi > 1 -> definitely >= 360
      bne     @subtract
      lda     TURTLE_HEADING_LO
      cmp     #<360
      bcc     @done
@subtract:
      sec
      lda     TURTLE_HEADING_LO
      sbc     #<360
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      sbc     #>360
      sta     TURTLE_HEADING_HI
      bra     @loop
@done:
      rts

; =====================================================================
; turtle_heading_u8 — Logo degrees -> NDK/blitter angle byte.
;   A = floor((heading * 256) / 360). Cardinals land exactly (90->64 etc).
;   (turtle.s:276 / extension.s:771) — division via the MATH copro.
; =====================================================================
turtle_heading_u8:
      stz     MATH_DIV_N_LO
      lda     TURTLE_HEADING_LO
      sta     MATH_DIV_N_1
      lda     TURTLE_HEADING_HI
      sta     MATH_DIV_N_2
      stz     MATH_DIV_N_HI
      lda     #<360
      sta     MATH_DIV_D_LO
      lda     #>360
      sta     MATH_DIV_D_HI            ; writing D_HI triggers the divide
      lda     MATH_RES0
      rts

; =====================================================================
; Shared turtle-handler tails — collapse the repeated "jsr render/draw then
; jmp finish_ok_nowait" pairs into one call site each (a tail-call: the jsr+jmp
; pair at every handler becomes a single jmp here).
;   render_then_ok: post-move render (pen line + redraw) then STATUS=OK.
;   draw_then_ok:   redraw turtle then STATUS=OK.
; These must NOT chain (turtle_render already tail-calls draw_turtle internally;
; falling render into draw would double-stamp), so each ends in its own jmp.
;
; FRAME PACING: each visible turtle update waits for the next display frame FIRST,
; then does erase->line->blit as a unit (the blit no longer frame-waits — see
; vsprite_gfx_rotate_blit_keyed_nowf). This keeps the 60Hz display from ever
; sampling a half-erased stamp, so a fast REPEAT loop shows the turtle stepping
; through every position instead of vanishing until the move ends. One wait per
; command also preserves the old one-move-per-frame pacing.
; =====================================================================
render_then_ok:
      jsr     vsprite_wait_frame       ; sync to a frame boundary BEFORE erasing
      jsr     turtle_render
      jmp     finish_ok_nowait
draw_then_ok:
      jsr     vsprite_wait_frame
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; mul_dist_trig — A = trig value (1.7 signed sin/cos) -> t_dx_frac/lo/hi =
;   (t_dist * trig) << 1, a 16.8 fixed displacement. Shared by FD's dx (sin)
;   and dy (cos) steps. The +1.0 case (127/128) is bumped to 128 so cardinal
;   headings stay pixel-exact; the high multiplier byte is the sign extension
;   of the original trig value. Clobbers A/X. (item 3 dedup)
; =====================================================================
mul_dist_trig:
      pha                              ; save trig for sign-extend
      lda     t_dist_lo
      sta     MATH_MUL16_A_LO
      lda     t_dist_hi
      sta     MATH_MUL16_A_HI
      pla
      tax                              ; X = original trig (for sign test)
      cmp     #127                     ; +1.0 is 127/128; use 128 so cardinals stay exact
      bne     :+
      lda     #128
:
      sta     MATH_MUL16_B_LO
      txa                              ; original trig
      ora     #$7F                     ; sign of trig -> N flag
      bmi     @neg
      lda     #$00
      bra     @ext
@neg:
      lda     #$FF
@ext:
      sta     MATH_MUL16_B_HI          ; triggers multiply
      lda     MATH_RES0
      asl
      sta     t_dx_frac
      lda     MATH_RES1
      rol
      sta     t_dx_lo
      lda     MATH_RES2
      rol
      sta     t_dx_hi
      rts

; =====================================================================
; t_fd — FD: move forward by distance (ARG0 = 16.8 distance).
;   Faithful port of t_fd (turtle.s:293) / ext_fd (extension.s:262). Scratch in
;   module BSS (the render subs run inline, not via lib_call, so no clobber risk).
; =====================================================================
t_fd:
      ; Snapshot the distance (ARG0 16.8 integer part) to BSS up front so the
      ; move-math below reads it from a stable home. draw_turtle (reached via
      ; ensure_gfx_mode on first gfx-mode entry) is already args-transparent — it
      ; saves/restores LIB_ARG0/1/2 around its X/Y/angle use — so TA0 would survive
      ; regardless; this snapshot is independent defense-in-depth, not relied upon.
      lda     TA0_LO
      sta     t_dist_lo
      lda     TA0_HI
      sta     t_dist_hi

      jsr     ensure_gfx_mode
      jsr     turtle_save_old

      ; heading -> NDK angle -> sincos
      jsr     turtle_heading_u8
      sta     MATH_SINCOS_ANGLE        ; triggers sincos
      lda     MATH_RES0                ; sin (1.7 signed)
      sta     t_sin_val
      lda     MATH_RES1                ; cos (1.7 signed)
      sta     t_cos_val

      ; dy first (so the shared mul scratch t_dx_* lands on dx last):
      ;   raw_dy = distance * cos / 128 (16.8) -> t_dx_*, then dy = -raw_dy.
      lda     t_cos_val
      jsr     mul_dist_trig            ; t_dx_* = dist*cos<<1 (raw_dy)
      sec                              ; dy = -raw_dy (negate 24-bit)
      lda     #0
      sbc     t_dx_frac
      sta     t_dy_frac
      lda     #0
      sbc     t_dx_lo
      sta     t_dy_lo
      lda     #0
      sbc     t_dx_hi
      sta     t_dy_hi

      ; dx = distance * sin / 128, kept as 16.8 (frac:lo:hi) = result << 1.
      ; Done LAST so t_dx_* holds the final dx for the X += dx add below.
      lda     t_sin_val
      jsr     mul_dist_trig            ; t_dx_* = dist*sin<<1

      ; X += dx (24-bit fixed frac:lo:hi)
      clc
      lda     TURTLE_X_FRAC
      adc     t_dx_frac
      sta     TURTLE_X_FRAC
      lda     TURTLE_X_LO
      adc     t_dx_lo
      sta     TURTLE_X_LO
      lda     TURTLE_X_HI
      adc     t_dx_hi
      sta     TURTLE_X_HI
      ; Y += dy
      clc
      lda     TURTLE_Y_FRAC
      adc     t_dy_frac
      sta     TURTLE_Y_FRAC
      lda     TURTLE_Y_LO
      adc     t_dy_lo
      sta     TURTLE_Y_LO
      lda     TURTLE_Y_HI
      adc     t_dy_hi
      sta     TURTLE_Y_HI

      jmp     render_then_ok           ; pen line (if down) + redraw, then STATUS=OK

; =====================================================================
; t_bk — BK: negate the 24-bit distance, then fall into FD. (turtle.s:404)
; =====================================================================
t_bk:
      sec
      lda     #0
      sbc     TA0_FRAC
      sta     TA0_FRAC
      lda     #0
      sbc     TA0_LO
      sta     TA0_LO
      lda     #0
      sbc     TA0_HI
      sta     TA0_HI
      jmp     t_fd

; =====================================================================
; Shared heading tails for RT/LT/SETH (item 2 dedup).
;   heading_add360: add 360 while the heading is negative (signed), then fall
;     into heading_finish. Used by LT (left past 0) and SETH (negative arg).
;   heading_finish: reduce mod 360, redraw turtle, STATUS=OK.
; =====================================================================
heading_add360:
      clc
      lda     TURTLE_HEADING_LO
      adc     #<360
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      adc     #>360
      sta     TURTLE_HEADING_HI
      bmi     heading_add360
      ; fall through
heading_finish:
      jsr     heading_mod360
      jmp     draw_then_ok

; =====================================================================
; t_rt — RT: turn right by degrees. (turtle.s:420)
; =====================================================================
t_rt:
      jsr     ensure_gfx_mode
      clc
      lda     TURTLE_HEADING_LO
      adc     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      adc     TA0_HI
      sta     TURTLE_HEADING_HI
      jmp     heading_finish

; =====================================================================
; t_lt — LT: turn left by degrees (add 360 if it goes negative). (turtle.s:435)
; =====================================================================
t_lt:
      jsr     ensure_gfx_mode
      sec
      lda     TURTLE_HEADING_LO
      sbc     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      sbc     TA0_HI
      sta     TURTLE_HEADING_HI
      bmi     heading_add360           ; went negative -> add 360 then finish
      jmp     heading_finish

; =====================================================================
; t_home — HOME: move to center, heading 0 (draw line if pen down). (turtle.s:462)
; =====================================================================
t_home:
      jsr     ensure_gfx_mode
      lda     TURTLE_INITED
      bne     @go
      jsr     turtle_init_state
      jmp     draw_then_ok
@go:
      jsr     turtle_save_old
      jsr     reset_pos_heading        ; X/Y -> center, heading 0
      jmp     render_then_ok

; =====================================================================
; t_setxy — SETXY x y: move to (x,y), draw line if pen down. (turtle.s:485)
; =====================================================================
t_setxy:
      jsr     ensure_gfx_mode          ; guarantees INITED (inits if needed)
      jsr     turtle_save_old
      jsr     screen_x_from_TA0        ; TURTLE_X = lx + 160
      jsr     screen_y_from_TA1        ; TURTLE_Y = 100 - ly
      jmp     render_then_ok

; t_setx — SETX x: set X only, draw line if pen down. (turtle.s:562)
; ensure_gfx_mode already inits when needed, so the old INITED re-check (formerly
; t_set_axis_init) is dead — just save the old position. (item 8)
t_setx:
      jsr     ensure_gfx_mode          ; guarantees INITED (inits if needed)
      jsr     turtle_save_old
      jsr     screen_x_from_TA0        ; TURTLE_X = lx + 160
      jmp     render_then_ok

; t_sety — SETY y: set Y only, draw line if pen down. (turtle.s:572)
t_sety:
      jsr     ensure_gfx_mode          ; guarantees INITED (inits if needed)
      jsr     turtle_save_old
      jsr     screen_y_from_TA0        ; TURTLE_Y = 100 - ly
      jmp     render_then_ok

; =====================================================================
; t_seth — SETH degrees: set heading, normalize mod 360. (turtle.s:593)
; =====================================================================
t_seth:
      jsr     ensure_gfx_mode
      lda     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TA0_HI
      sta     TURTLE_HEADING_HI
      lda     TURTLE_HEADING_HI
      bmi     @far                     ; negative arg -> add 360 then finish
      jmp     heading_finish
@far:
      jmp     heading_add360           ; (out of bmi range -> trampoline)

; =====================================================================
; t_cs — CS / CLEARSCREEN / DRAW: reset VGC, clear gfx, split, init + show turtle.
;   (turtle.s:618 / extension.s:215)
; =====================================================================
t_cs:
      stz     VGC_BGCOL
      lda     #TURTLE_COL_WHITE
      sta     VGC_FGCOL
      stz     VGC_BORDER
      stz     VGC_GFXTRANS
      stz     VGC_SCROLLX
      stz     VGC_SCROLLY
      stz     VGC_SCROLLCTL
      lda     #TURTLE_MODE_TEXT
      sta     VGC_MODE
      jsr     vgc_gcls                 ; clear the gfx plane
      jsr     vgc_wait_cmd
      jsr     turtle_enter_split       ; copper split + GFX_VISIBLE=1
      jsr     turtle_init_state        ; reset state + reinstall icon + clear bg bookkeeping
      jmp     draw_then_ok

; =====================================================================
; t_pu / t_pd — pen up / pen down. (turtle.s:638)
; =====================================================================
t_pu:
      jsr     ensure_gfx_mode
      lda     #$01
      sta     TURTLE_PEN
      jmp     finish_ok_nowait
t_pd:
      jsr     ensure_gfx_mode
      stz     TURTLE_PEN
      jmp     finish_ok_nowait

; =====================================================================
; t_st / t_ht — show / hide turtle. (turtle.s:651)
; =====================================================================
t_st:
      jsr     ensure_gfx_mode
      lda     #$01
      sta     TURTLE_SHOWN
      jmp     draw_then_ok
t_ht:
      jsr     ensure_gfx_mode
      jsr     gfn_turtle_erase         ; restore saved bg (module $02 sub)
      stz     TURTLE_SHOWN
      jmp     finish_ok_nowait

; =====================================================================
; t_setpc — SETPC color: set pen color. Color in ARG0 byte0. (turtle.s:665)
; =====================================================================
t_setpc:
      jsr     ensure_gfx_mode
      lda     LIB_ARG0+0
      sta     TURTLE_COLOR
      jmp     finish_ok_nowait

; =====================================================================
; t_setbg — SETBG color: set VGC bg, clear gfx to it, redraw turtle. Color in
;   ARG0 byte0. GCLS clears to the current draw color, so set GCOLOR(bg) first.
;   (turtle.s:675 / extension.s:1348)
; =====================================================================
t_setbg:
      jsr     ensure_gfx_mode
      jsr     gfn_turtle_erase         ; lift the turtle before repainting
      lda     LIB_ARG0+0
      sta     VGC_BGCOL
      jsr     vgc_wait_cmd
      lda     LIB_ARG0+0
      sta     VGC_P0
      jsr     vgc_gcolor               ; draw color = bg, so GCLS paints bg
      jsr     vgc_wait_cmd
      jsr     vgc_gcls
      jsr     vgc_wait_cmd
      jmp     draw_then_ok

; =====================================================================
; t_ts / t_ss / t_fs — screen-mode ops. (extension.s:851/861/871)
;   TS: full text mode (copper off, mode 0). SS: split (gfx+spr top / text below).
;   FS: full graphics mode (copper off, mode 3). Text-window setup (prepare_split_
;   text) stays the adapter's job — see turtle_enter_split deviation note.
; =====================================================================
t_ts:
      jsr     vgc_wait_cmd
      jsr     copper_off
      stz     VGC_MODE
      stz     TURTLE_GFX_VISIBLE
      jmp     finish_ok_nowait
t_ss:
      jsr     turtle_enter_split
      jmp     finish_ok_nowait
t_fs:
      jsr     vgc_wait_cmd
      jsr     copper_off
      lda     #TURTLE_MODE_GFXSPR
      sta     VGC_MODE
      lda     #$01
      sta     TURTLE_GFX_VISIBLE
      jmp     finish_ok_nowait

; =====================================================================
; Reporters — write a Logo 16.8 value to the LIB_RESULT low word + STATUS OK.
;   t_report16: A = value HI, X = value LO -> LIB_RESULT [FRAC=0, LO, HI, 0].
;   Result cell carries the integer in LO/HI with FRAC=0 (matches the legacy
;   reporters which set EXT_RESULT_FRAC=0).
; =====================================================================
t_report16:
      stz     LIB_RESULT+0             ; FRAC = 0
      stx     LIB_RESULT+1             ; LO
      sta     LIB_RESULT+2             ; HI
      stz     LIB_RESULT+3
      jmp     finish_ok_nowait

t_xcor:
      ; lx = screen_x - 160 (signed 16-bit; left of center is negative)
      sec
      lda     TURTLE_X_LO
      sbc     #<TURTLE_CENTER_X
      tax
      lda     TURTLE_X_HI
      sbc     #>TURTLE_CENTER_X
      jmp     t_report16
t_ycor:
      ; ly = 100 - screen_y (signed 16-bit; +Y is up, below center is negative)
      sec
      lda     #<TURTLE_CENTER_Y
      sbc     TURTLE_Y_LO
      tax
      lda     #>TURTLE_CENTER_Y
      sbc     TURTLE_Y_HI
      jmp     t_report16
t_heading:
      ldx     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      jmp     t_report16
t_pendownp:
      lda     TURTLE_PEN               ; $00 down / $01 up
      eor     #$01                     ; -> down=1, up=0 (PENDOWN? truth)
      tax                              ; result lo
      lda     #0                       ; result hi
      jmp     t_report16
t_shownp:
      ldx     TURTLE_SHOWN
      lda     #0
      jmp     t_report16

; =====================================================================
; t_towards — TOWARDS x y: heading angle (degrees) towards (x,y) via ATAN2.
;   ARG0 = target x, ARG1 = target y. (extension.s:1368)
; =====================================================================
t_towards:
      ; The target rides in as centered Logo coords; convert to screen in place
      ; (TA0 = lx + 160, TA1 = 100 - ly) so the screen-space ATAN2 below is
      ; unchanged. turtle_x/turtle_y are already screen pixels.
      clc
      lda     TA0_LO
      adc     #<TURTLE_CENTER_X
      sta     TA0_LO
      lda     TA0_HI
      adc     #>TURTLE_CENTER_X
      sta     TA0_HI
      sec
      lda     #<TURTLE_CENTER_Y
      sbc     TA1_LO
      sta     TA1_LO
      lda     #>TURTLE_CENTER_Y
      sbc     TA1_HI
      sta     TA1_HI
      ; dy = turtle_y - target_y (screen Y is inverted) — set FIRST so DX_HI
      ; is the LAST write (the math copro triggers ATAN2 on MATH_ATAN_DX_HI;
      ; the legacy ext_towards wrote DX before DY, latching a stale DY — fixed
      ; here so TOWARDS computes against the intended operands).
      sec
      lda     TURTLE_Y_LO
      sbc     TA1_LO
      sta     MATH_ATAN_DY_LO
      lda     TURTLE_Y_HI
      sbc     TA1_HI
      sta     MATH_ATAN_DY_HI
      ; dx = target_x - turtle_x (signed 16-bit) — DX_HI last -> triggers ATAN2
      sec
      lda     TA0_LO
      sbc     TURTLE_X_LO
      sta     MATH_ATAN_DX_LO
      lda     TA0_HI
      sbc     TURTLE_X_HI
      sta     MATH_ATAN_DX_HI          ; writing DX_HI triggers ATAN2
      ; angle u8 (0-255) -> degrees: u8 * 360 / 256 = hi byte of (u8 * 360).
      ; The ATAN2 result is a MATH angle (east=0, CCW). Remap to a LOGO HEADING
      ; (north=0, east=90, clockwise) so SETHEADING TOWARDS [x y] points at the
      ; target: heading = (90 - mathdeg) mod 360.
      lda     MATH_RES0
      sta     MATH_MUL16_A_LO
      stz     MATH_MUL16_A_HI
      lda     #<360
      sta     MATH_MUL16_B_LO
      lda     #>360
      sta     MATH_MUL16_B_HI          ; triggers multiply; mathdeg = MATH_RES2:RES1
      ; heading = 90 - mathdeg (signed 16-bit) -> turtle_tmp0/1 (clamp scratch,
      ; unused outside render)
      sec
      lda     #<90
      sbc     MATH_RES1
      sta     turtle_tmp0
      lda     #>90
      sbc     MATH_RES2
      sta     turtle_tmp1
      bpl     @h_ok                    ; >= 0 -> already in [0,90]
      clc                              ; negative -> + 360
      lda     turtle_tmp0
      adc     #<360
      sta     turtle_tmp0
      lda     turtle_tmp1
      adc     #>360
      sta     turtle_tmp1
@h_ok:
      ldx     turtle_tmp0
      lda     turtle_tmp1
      jmp     t_report16

; turtle_icon — the built-in 16x16 upward-triangle turtle, one color (white).
; Replicated verbatim from novalogo/extension.s turtle_shape_data. ROM constant
; (CODE segment); TUR_INIT copies these 256 bytes into the BSS source buffer at
; runtime (the module reads/blits from RAM, not ROM).
turtle_icon:
.repeat 16, yy
  .repeat 16, xx
    .if yy < 1
      .byte 0
    .elseif yy > 14
      .byte 0
    .elseif yy >= 13
      .if xx >= (8 - ((((yy - 1) * 7) + 6) / 13)) && xx <= (8 + ((((yy - 1) * 6) + 6) / 13))
        .byte TURTLE_COL_WHITE
      .else
        .byte 0
      .endif
    .elseif xx = (8 - ((((yy - 1) * 7) + 6) / 13))
      .byte TURTLE_COL_WHITE
    .elseif xx = (8 + ((((yy - 1) * 6) + 6) / 13))
      .byte TURTLE_COL_WHITE
    .else
      .byte 0
    .endif
  .endrepeat
.endrepeat

; Module-BSS turtle-render state. Lives in the MODBSS band ($0420-$08FF). The 3
; 256-byte buffers (source/rotated/saved-bg) dominate; the bg bookkeeping + clamp
; scratch + move-math + arg-snapshot scratch are single bytes.
      .segment "BSS"
; BSS canary (turtle_ensure_bss): MUST be the FIRST thing in the band ($0420). The
; paged-library loader reuses this band for whichever module is resident, and every
; module's BSS starts at $0420 — so any other module that runs (e.g. the editor)
; is guaranteed to overwrite this word. A later turtle op sees the mismatch and
; re-installs the volatile icon/BOB buffers. (A canary placed LATER in the band can
; be missed when the clobbering module's BSS is shorter than the turtle's.)
turtle_bss_magic:     .res 2
turtle_source_shape:  .res 256       ; built-in icon, installed by TUR_INIT
turtle_rotated_shape: .res 256       ; blitter rotate output
turtle_saved_bg:      .res 256       ; Amiga-BOB saved background under the stamp
turtle_bg_x_lo:       .res 1         ; stamp top-left X (for the matching erase)
turtle_bg_x_hi:       .res 1
turtle_bg_y:          .res 1         ; stamp top-left Y
turtle_bg_saved:      .res 1         ; nonzero when turtle_saved_bg holds a region
turtle_tmp0:          .res 1         ; set_turtle_pos clamp scratch (tmp0)
turtle_tmp1:          .res 1         ; set_turtle_pos clamp scratch (tmp1)
; --- Turtle command-engine move-math scratch. Module BSS, not ZP: the render runs
;     inline (no lib_call), so plain RAM scratch is safe. old_x/y are the pre-move
;     integer position the post-move pen line reads. ---
t_dx_frac:            .res 1
t_dx_lo:              .res 1
t_dx_hi:              .res 1
t_dy_frac:            .res 1
t_dy_lo:              .res 1
t_dy_hi:              .res 1
t_sin_val:            .res 1
t_cos_val:            .res 1
t_dist_lo:            .res 1         ; FD/BK distance snapshot (mailbox-clobber safe)
t_dist_hi:            .res 1
turtle_old_x_lo:      .res 1
turtle_old_x_hi:      .res 1
turtle_old_y_lo:      .res 1
turtle_old_y_hi:      .res 1
; draw_turtle saves the caller's value-arg bytes here while it borrows the
; mailbox cells for the gfn_turtle_draw call (keeps draw_turtle args-transparent).
t_save_a0_0:          .res 1
t_save_a0_1:          .res 1
t_save_a1_0:          .res 1
t_save_a1_1:          .res 1
t_save_a2_0:          .res 1
; WRAP scratch: wrapped target (committed pos), per-axis tile offset (= unwrapped
; minus wrapped), and the tile-loop offset accumulators for the pen line.
wrap_wx_lo:           .res 1
wrap_wx_hi:           .res 1
wrap_wy_lo:           .res 1
wrap_wy_hi:           .res 1
wrap_dxoff_lo:        .res 1
wrap_dxoff_hi:        .res 1
wrap_dyoff_lo:        .res 1
wrap_dyoff_hi:        .res 1
woff_x_lo:            .res 1
woff_x_hi:            .res 1
woff_y_lo:            .res 1
woff_y_hi:            .res 1
      .segment "CODE"

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced() selective-emit
; keeps only the routines the turtle calls. vgc.s sets its own `.segment "CODE"`
; and pulls nova.inc (VGC_*/MATH_*) via vgc.inc; co-assembles under its .ifndef
; guards. sprite.s provides the hw-sprite driver entries (vsprite.s depends on it
; via blitter). copper.s + copper_split.s add copper_off + copper_split_mode.
; blitter.s + dma.s provide the $BA-range move-engine entries. vsprite.s provides
; the virtual-sprite save/restore/rotate-blit entries the render path uses and
; declares the VSPRITE_* state in the module-owned BSS/ZEROPAGE bands.
; ===========================================================================
      .include "vgc.s"
      .include "sprite.s"
      .include "copper.s"
      .include "copper_split.s"
      .include "blitter.s"
      .include "dma.s"
      .include "vsprite.s"

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
