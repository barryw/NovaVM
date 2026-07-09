; =====================================================================
;  Nova NDK — vtext_mixed.s
;
;  Mixed text/graphics VTEXT helpers, kept separate from vtext.s so
;  plain-text users do not pay for gfx-plane scrolling. Scrolls and fills
;  a VTEXT cell rectangle and its matching 320x200 gfx rectangle as one
;  transaction, for runtimes that render text over the graphics plane.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "vtext.inc"
.include "blitter.inc"

.ifndef VTEXT_MIXED_IMPLEMENTATION_INCLUDED
VTEXT_MIXED_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

VTEXT_MIXED_ROWS: .res 1
VTEXT_MIXED_FILL: .res 1
VTEXT_MIXED_COPY: .res 1
VTEXT_MIXED_DSTL: .res 1
VTEXT_MIXED_DSTM: .res 1
VTEXT_MIXED_WIDTHL: .res 1
VTEXT_MIXED_WIDTHH: .res 1
VTEXT_MIXED_STRIPH: .res 1
VTEXT_GFX_LEFTL: .res 1
VTEXT_GFX_LEFTH: .res 1
VTEXT_GFX_TOP: .res 1
VTEXT_GFX_WIDTHL: .res 1
VTEXT_GFX_WIDTHH: .res 1
VTEXT_GFX_HEIGHT: .res 1
VTEXT_GFX_SCROLL_ROWS: .res 1
VTEXT_GFX_FILL: .res 1
VTEXT_GFX_COPY: .res 1
VTEXT_GFX_DSTL: .res 1
VTEXT_GFX_DSTM: .res 1
VTEXT_GFX_DELTAL: .res 1
VTEXT_GFX_DELTAH: .res 1
VTEXT_GFX_CALC_ROWS: .res 1

      .segment "CODE"

      .export vtext_scroll_mixed_up
      .export vtext_scroll_composite_up
      .export vtext_fill_gfx_region
      .export vtext_scroll_gfx_pixels_up

vtext_mixed_ok:
      LDA   #VTEXT_OK
      RTS

vtext_mixed_err:
      LDA   #VTEXT_ERR
      RTS

vtext_mixed_cell_to_gfx_region:
      LDA   VTEXT_LEFT
      STA   VTEXT_GFX_LEFTL
      STZ   VTEXT_GFX_LEFTH
      ASL   VTEXT_GFX_LEFTL
      ROL   VTEXT_GFX_LEFTH
      ASL   VTEXT_GFX_LEFTL
      ROL   VTEXT_GFX_LEFTH

      LDA   VTEXT_TOP
      ASL   A
      ASL   A
      STA   VTEXT_GFX_TOP

      LDA   VTEXT_WIDTH
      STA   VTEXT_GFX_WIDTHL
      STZ   VTEXT_GFX_WIDTHH
      ASL   VTEXT_GFX_WIDTHL
      ROL   VTEXT_GFX_WIDTHH
      ASL   VTEXT_GFX_WIDTHL
      ROL   VTEXT_GFX_WIDTHH

      LDA   VTEXT_HEIGHT
      ASL   A
      ASL   A
      STA   VTEXT_GFX_HEIGHT
      RTS

; @label VTEXT.SCROLL_MIXED_UP
; @kind routine
; @symbol vtext_scroll_mixed_up
; @summary Scroll the current VTEXT rectangle and matching gfx rectangle up by A text rows.
; @in A: Number of text rows to scroll upward.
; @in X: Gfx-plane fill colour for the newly exposed bottom strip.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
; @details Treats the VTEXT cell rectangle as a 4px-per-cell gfx rectangle,
;          then issues VCMD_SCROLLMIXED so hardware scrolls graphics, color,
;          text attributes, and characters as one vblank-aligned transaction.
;          It matches framebuffer interpreters such as Frotz, where text and
;          graphics are already one pixel surface.
vtext_scroll_mixed_up:
      STA   VTEXT_MIXED_ROWS
      STX   VTEXT_MIXED_FILL
      JSR   vtext_mixed_validate_region
      BNE   @done
      LDA   VTEXT_MIXED_ROWS
      BEQ   @ok

      JSR   vtext_mixed_cell_to_gfx_region
      LDA   VTEXT_MIXED_ROWS
      LDX   VTEXT_MIXED_FILL
      JMP   vtext_scroll_composite_up
@ok:
      JMP   vtext_mixed_ok
@done:
      RTS

; @label VTEXT.SCROLL_COMPOSITE_UP
; @kind routine
; @symbol vtext_scroll_composite_up
; @summary Transactionally scroll current VTEXT planes and configured gfx rectangle up by A text rows.
; @in A: Number of text rows to scroll upward. The gfx rectangle scrolls by A*4 pixel rows.
; @in X: Gfx-plane fill colour for the newly exposed bottom strip.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @requires VTEXT_GFX_LEFTL/H VTEXT_GFX_TOP VTEXT_GFX_WIDTHL/H VTEXT_GFX_HEIGHT
; @out A: 0 on success, 1 on invalid geometry.
; @details Issues VCMD_SCROLLMIXED so hardware scrolls graphics, character,
;          colour, and text-attribute planes as one command.
vtext_scroll_composite_up:
      STA   VTEXT_MIXED_ROWS
      STX   VTEXT_MIXED_FILL
      JSR   vtext_mixed_validate_region
      BEQ   :+
      JMP   @done
:
      JSR   vtext_gfx_validate_region
      BEQ   :+
      JMP   @done
:
      LDA   VTEXT_MIXED_ROWS
      BEQ   @ok
      ASL   A
      ASL   A
      STA   VTEXT_GFX_SCROLL_ROWS

      LDA   VTEXT_LEFT
      STA   VGC_P0
      LDA   VTEXT_TOP
      STA   VGC_P1
      LDA   VTEXT_WIDTH
      STA   VGC_P2
      LDA   VTEXT_HEIGHT
      STA   VGC_P3
      LDA   VTEXT_MIXED_ROWS
      STA   VGC_P4
      LDA   VTEXT_MIXED_FILL
      STA   VGC_P5
      LDA   VTEXT_GFX_LEFTL
      STA   VGC_P6
      LDA   VTEXT_GFX_LEFTH
      STA   VGC_P7
      LDA   VTEXT_GFX_TOP
      STA   VGC_P8
      LDA   VTEXT_GFX_WIDTHL
      STA   VGC_P9
      LDA   VTEXT_GFX_WIDTHH
      STA   VGC_P10
      LDA   VTEXT_GFX_HEIGHT
      STA   VGC_P11
      LDA   VTEXT_GFX_SCROLL_ROWS
      STA   VGC_P12
      LDA   VTEXT_COLOR
      STA   VGC_P13
      LDA   VTEXT_ATTR
      STA   VGC_P14
      LDA   #VCMD_SCROLLMIXED
      STA   VGC_CMD
      LDY   #$20
@arm:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      DEY
      BNE   @arm
      BRA   @ok
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
@ok:
      JMP   vtext_mixed_ok
@done:
      RTS

; @label VTEXT.FILL_GFX_REGION
; @kind routine
; @symbol vtext_fill_gfx_region
; @summary Fill the graphics pixels under the current VTEXT cell rectangle.
; @in X: Gfx-plane fill colour.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT
; @out A: 0 on success, 1 on error.
; @details This is the graphics-plane half of making a mixed text/gfx cell
;          rectangle look like solid background again. It fills
;          left/top/width/height converted from 4px VTEXT cells to gfx pixels;
;          callers can then restyle text spaces opaque or transparent as their
;          compositor requires.
vtext_fill_gfx_region:
      STX   VTEXT_GFX_FILL
      JSR   vtext_mixed_validate_region
      BNE   @done

      JSR   vtext_mixed_cell_to_gfx_region
      JSR   vtext_gfx_validate_region
      BNE   @done
      JSR   vtext_gfx_calc_addr
      LDA   VTEXT_GFX_HEIGHT
      STA   VTEXT_GFX_COPY
      JMP   vtext_gfx_fill
@done:
      RTS

; @label VTEXT.SCROLL_GFX_PIXELS_UP
; @kind routine
; @symbol vtext_scroll_gfx_pixels_up
; @summary Scroll an arbitrary VGC gfx pixel rectangle up by A pixel rows.
; @in A: Number of graphics pixel rows to scroll upward.
; @in X: Gfx-plane fill colour for the newly exposed bottom strip.
; @requires VTEXT_GFX_LEFTL/H VTEXT_GFX_TOP VTEXT_GFX_WIDTHL/H VTEXT_GFX_HEIGHT
; @out A: 0 on success, 1 on error.
; @details This is the graphics-plane half of a transactional mixed scroll.
;          Callers whose logical pixel window is not identical to the current
;          4px VTEXT cell rectangle can scroll the exact gfx window with this
;          helper, then scroll the text planes separately.
vtext_scroll_gfx_pixels_up:
      STA   VTEXT_GFX_SCROLL_ROWS
      STX   VTEXT_GFX_FILL
      JSR   vtext_gfx_validate_region
      BNE   @done
      LDA   VTEXT_GFX_SCROLL_ROWS
      BEQ   @ok

      JSR   vtext_mixed_wait_frame
      JSR   vtext_gfx_pixels_up
      BNE   @done
@ok:
      JMP   vtext_mixed_ok
@done:
      RTS

vtext_mixed_validate_region:
      LDA   VTEXT_WIDTH
      BEQ   @err
      LDA   VTEXT_HEIGHT
      BEQ   @err
      LDA   VTEXT_LEFT
      CLC
      ADC   VTEXT_WIDTH
      BCS   @err
      CMP   #VTEXT_SCREEN_COLS + 1
      BCS   @err
      LDA   VTEXT_TOP
      CLC
      ADC   VTEXT_HEIGHT
      BCS   @err
      CMP   #VTEXT_SCREEN_ROWS + 1
      BCS   @err
      JMP   vtext_mixed_ok
@err:
      JMP   vtext_mixed_err

vtext_gfx_validate_region:
      LDA   VTEXT_GFX_WIDTHL
      ORA   VTEXT_GFX_WIDTHH
      BEQ   @err
      LDA   VTEXT_GFX_HEIGHT
      BEQ   @err

      ; left + width must be <= 320.
      CLC
      LDA   VTEXT_GFX_LEFTL
      ADC   VTEXT_GFX_WIDTHL
      STA   VTEXT_GFX_DSTL
      LDA   VTEXT_GFX_LEFTH
      ADC   VTEXT_GFX_WIDTHH
      BCS   @err
      STA   VTEXT_GFX_DSTM
      CMP   #$02
      BCS   @err
      CMP   #$01
      BNE   @x_ok
      LDA   VTEXT_GFX_DSTL
      CMP   #<(VTEXT_GFX_ROW_PIXELS + 1)
      BCS   @err
@x_ok:
      ; top + height must be <= 200.
      CLC
      LDA   VTEXT_GFX_TOP
      ADC   VTEXT_GFX_HEIGHT
      BCS   @err
      CMP   #(VTEXT_SCREEN_ROWS * VTEXT_GFX_CELL_PIXELS + 1)
      BCS   @err
      JMP   vtext_mixed_ok
@err:
      JMP   vtext_mixed_err

vtext_mixed_wait_frame:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

vtext_gfx_calc_addr:
      ; dst px addr = top*320 + left. 320 = 256 + 64.
      LDA   VTEXT_GFX_LEFTL
      STA   VTEXT_GFX_DSTL
      LDA   VTEXT_GFX_LEFTH
      STA   VTEXT_GFX_DSTM

      LDA   VTEXT_GFX_TOP
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      CLC
      ADC   VTEXT_GFX_DSTL
      STA   VTEXT_GFX_DSTL
      BCC   :+
      INC   VTEXT_GFX_DSTM
:
      LDA   VTEXT_GFX_TOP
      LSR   A
      LSR   A
      CLC
      ADC   VTEXT_GFX_DSTM
      STA   VTEXT_GFX_DSTM
      LDA   VTEXT_GFX_TOP
      CLC
      ADC   VTEXT_GFX_DSTM
      STA   VTEXT_GFX_DSTM
      RTS

vtext_gfx_calc_delta:
      ; A rows -> rows*320 in VTEXT_GFX_DELTAL/H. 320 = 256 + 64.
      STA   VTEXT_GFX_CALC_ROWS
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      STA   VTEXT_GFX_DELTAL
      LDA   VTEXT_GFX_CALC_ROWS
      LSR   A
      LSR   A
      CLC
      ADC   VTEXT_GFX_CALC_ROWS
      STA   VTEXT_GFX_DELTAH
      RTS

vtext_gfx_pixels_up:
      JSR   vtext_gfx_calc_addr

      ; copy rows = height - rows. <= 0 clears the full rect.
      LDA   VTEXT_GFX_HEIGHT
      SEC
      SBC   VTEXT_GFX_SCROLL_ROWS
      BCS   :+
      JMP   @fill_all
:
      BNE   :+
      JMP   @fill_all
:
      STA   VTEXT_GFX_COPY

      LDA   VTEXT_GFX_SCROLL_ROWS
      JSR   vtext_gfx_calc_delta

      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_SRCSPACE
      STA   BLT_DSTSPACE
      CLC
      LDA   VTEXT_GFX_DSTL
      ADC   VTEXT_GFX_DELTAL
      STA   BLT_SRCL
      LDA   VTEXT_GFX_DSTM
      ADC   VTEXT_GFX_DELTAH
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   VTEXT_GFX_DSTL
      STA   BLT_DSTL
      LDA   VTEXT_GFX_DSTM
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   #<VTEXT_GFX_ROW_PIXELS
      STA   BLT_SRCSTRL
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_GFX_ROW_PIXELS
      STA   BLT_SRCSTRH
      STA   BLT_DSTSTRH
      LDA   VTEXT_GFX_WIDTHL
      STA   BLT_WIDTHL
      LDA   VTEXT_GFX_WIDTHH
      STA   BLT_WIDTHH
      LDA   VTEXT_GFX_COPY
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      JSR   blitter_start_copy
      CMP   #BLITTER_RESULT_OK
      BEQ   :+
      JMP   vtext_mixed_err
:

      LDA   VTEXT_GFX_COPY
      JSR   vtext_gfx_calc_delta
      CLC
      LDA   VTEXT_GFX_DSTL
      ADC   VTEXT_GFX_DELTAL
      STA   VTEXT_GFX_DSTL
      LDA   VTEXT_GFX_DSTM
      ADC   VTEXT_GFX_DELTAH
      STA   VTEXT_GFX_DSTM
      LDA   VTEXT_GFX_SCROLL_ROWS
      BRA   @fill
@fill_all:
      LDA   VTEXT_GFX_HEIGHT
@fill:
      STA   VTEXT_GFX_COPY
      JMP   vtext_gfx_fill

vtext_gfx_fill:
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   VTEXT_GFX_DSTL
      STA   BLT_DSTL
      LDA   VTEXT_GFX_DSTM
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   #<VTEXT_GFX_ROW_PIXELS
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_GFX_ROW_PIXELS
      STA   BLT_DSTSTRH
      LDA   VTEXT_GFX_WIDTHL
      STA   BLT_WIDTHL
      LDA   VTEXT_GFX_WIDTHH
      STA   BLT_WIDTHH
      LDA   VTEXT_GFX_COPY
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   VTEXT_GFX_FILL
      STA   BLT_FILLVALUE
      JSR   blitter_fill
      CMP   #BLITTER_RESULT_OK
      BNE   @err
      JMP   vtext_mixed_ok
@err:
      JMP   vtext_mixed_err

vtext_mixed_gfx_up:
      ; dst px addr = top*1280 + left*4. Since 1280=$0500, the row offset is
      ; top*5 in the middle byte.
      LDA   VTEXT_LEFT
      STA   VTEXT_MIXED_DSTL
      STZ   VTEXT_MIXED_DSTM
      ASL   VTEXT_MIXED_DSTL
      ROL   VTEXT_MIXED_DSTM
      ASL   VTEXT_MIXED_DSTL
      ROL   VTEXT_MIXED_DSTM
      LDA   VTEXT_TOP
      ASL
      ASL
      CLC
      ADC   VTEXT_TOP
      CLC
      ADC   VTEXT_MIXED_DSTM
      STA   VTEXT_MIXED_DSTM

      ; width px = VTEXT_WIDTH * 4.
      LDA   VTEXT_WIDTH
      STA   VTEXT_MIXED_WIDTHL
      STZ   VTEXT_MIXED_WIDTHH
      ASL   VTEXT_MIXED_WIDTHL
      ROL   VTEXT_MIXED_WIDTHH
      ASL   VTEXT_MIXED_WIDTHL
      ROL   VTEXT_MIXED_WIDTHH

      ; copy rows = VTEXT_HEIGHT - rows. <= 0 clears the full rect.
      LDA   VTEXT_HEIGHT
      SEC
      SBC   VTEXT_MIXED_ROWS
      BCC   @fill_all
      BEQ   @fill_all
      STA   VTEXT_MIXED_COPY

      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_SRCSPACE
      STA   BLT_DSTSPACE
      LDA   VTEXT_MIXED_DSTL
      STA   BLT_SRCL
      STA   BLT_DSTL
      LDA   VTEXT_MIXED_ROWS
      ASL
      ASL
      CLC
      ADC   VTEXT_MIXED_ROWS           ; rows * 5 middle-byte stride
      CLC
      ADC   VTEXT_MIXED_DSTM
      STA   BLT_SRCM
      LDA   VTEXT_MIXED_DSTM
      STA   BLT_DSTM
      STZ   BLT_SRCH
      STZ   BLT_DSTH
      LDA   #<VTEXT_GFX_ROW_PIXELS
      STA   BLT_SRCSTRL
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_GFX_ROW_PIXELS
      STA   BLT_SRCSTRH
      STA   BLT_DSTSTRH
      LDA   VTEXT_MIXED_WIDTHL
      STA   BLT_WIDTHL
      LDA   VTEXT_MIXED_WIDTHH
      STA   BLT_WIDTHH
      LDA   VTEXT_MIXED_COPY
      ASL
      ASL
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      JSR   blitter_start_copy
      CMP   #BLITTER_RESULT_OK
      BNE   @err

      ; Fill the strip exposed at the bottom:
      ; dst += (height - rows) * 1280, strip height = rows * 4 px.
      LDA   VTEXT_MIXED_COPY
      ASL
      ASL
      CLC
      ADC   VTEXT_MIXED_COPY
      CLC
      ADC   VTEXT_MIXED_DSTM
      STA   VTEXT_MIXED_DSTM
      LDA   VTEXT_MIXED_ROWS
      BRA   @fill
@fill_all:
      LDA   VTEXT_HEIGHT
@fill:
      ASL
      ASL
      STA   VTEXT_MIXED_STRIPH
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   VTEXT_MIXED_DSTL
      STA   BLT_DSTL
      LDA   VTEXT_MIXED_DSTM
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   #<VTEXT_GFX_ROW_PIXELS
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_GFX_ROW_PIXELS
      STA   BLT_DSTSTRH
      LDA   VTEXT_MIXED_WIDTHL
      STA   BLT_WIDTHL
      LDA   VTEXT_MIXED_WIDTHH
      STA   BLT_WIDTHH
      LDA   VTEXT_MIXED_STRIPH
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   VTEXT_MIXED_FILL
      STA   BLT_FILLVALUE
      JSR   blitter_fill
      CMP   #BLITTER_RESULT_OK
      BNE   @err
      JMP   vtext_mixed_ok
@err:
      JMP   vtext_mixed_err

.endif
