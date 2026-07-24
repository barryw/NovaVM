; Optional downward text-region scroll, split from vtext.s so consumers that
; only need the common VTEXT operations do not pay for insert-line support.

.include "vtext.inc"
.include "blitter.inc"

      .segment "BSS"

vtext_scroll_down_height: .res 1

      .segment "CODE"

      .export vtext_scroll_down

; @label VTEXT.SCROLL_DOWN
; @kind routine
; @symbol vtext_scroll_down
; @library vtext
; @summary Scroll the current VTEXT rectangle down by one text row and clear the top row.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
vtext_scroll_down:
      JSR   vtext_validate_region
      BNE   @done
      LDA   VTEXT_HEIGHT
      CMP   #$01
      BEQ   @clear

      JSR   vtext_calc_region_addr
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_copy_down_plane
      BNE   @done
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_copy_down_plane
      BNE   @done
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_copy_down_plane
      BNE   @done

@clear:
      LDA   VTEXT_HEIGHT
      STA   vtext_scroll_down_height
      LDA   #$01
      STA   VTEXT_HEIGHT
      JSR   vtext_clear_region
      PHA
      LDA   vtext_scroll_down_height
      STA   VTEXT_HEIGHT
      PLA
@done:
      RTS

vtext_blt_copy_down_plane:
      STA   BLT_SRCSPACE
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      STA   BLT_SRCL
      CLC
      ADC   #VTEXT_SCREEN_COLS
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_SRCM
      ADC   #$00
      STA   BLT_DSTM
      STZ   BLT_SRCH
      STZ   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_HEIGHT
      DEC
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRL
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRH
      STA   BLT_DSTSTRH
      JSR   blitter_start_copy
      JMP   vtext_wait_blitter
