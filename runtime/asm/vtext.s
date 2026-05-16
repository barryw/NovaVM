; Shared NovaVM text-region runtime.
;
; Provides small rectangular text regions over the VGC char/color/attribute
; planes. Domain runtimes such as NovaZ decide what a region means; this code
; only clips, clears, writes, and scrolls rectangular text areas.

.include "vtext.inc"
.include "blitter.s"

.ifndef VTEXT_IMPLEMENTATION_INCLUDED
VTEXT_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

VTEXT_LEFT:       .res 1
VTEXT_TOP:        .res 1
VTEXT_WIDTH:      .res 1
VTEXT_HEIGHT:     .res 1
VTEXT_CURX:       .res 1
VTEXT_CURY:       .res 1
VTEXT_COLOR:      .res 1
VTEXT_ATTR:       .res 1
VTEXT_CHAR:       .res 1
VTEXT_FLAGS:      .res 1
VTEXT_TABLEL:     .res 1
VTEXT_TABLEH:     .res 1
VTEXT_REGION_ID:  .res 1

      .segment "ZEROPAGE"

VTEXT_ADDRL:      .res 1
VTEXT_ADDRH:      .res 1
VTEXT_TMP:        .res 1

      .segment "CODE"

      .export vtext_define_region
      .export vtext_select_region
      .export vtext_store_region
      .export vtext_validate_region
      .export vtext_set_cursor
      .export vtext_sync_cursor
      .export vtext_home
      .export vtext_put_char
      .export vtext_newline
      .export vtext_clear_region
      .export vtext_clear_line
      .export vtext_scroll_up
      .export vtext_wait_blitter

vtext_ok:
      LDA   #VTEXT_OK
      RTS

vtext_err:
      LDA   #VTEXT_ERR
      RTS

; ---------------------------------------------------------------------
; Optional region-table helpers
; ---------------------------------------------------------------------
vtext_region_slot:
      LDA   VTEXT_TABLEL
      STA   VTEXT_ADDRL
      LDA   VTEXT_TABLEH
      STA   VTEXT_ADDRH

      LDA   VTEXT_REGION_ID
      ASL
      STA   VTEXT_TMP          ; id * 2
      ASL
      ASL                      ; id * 8
      CLC
      ADC   VTEXT_TMP          ; id * 10
      CLC
      ADC   VTEXT_ADDRL
      STA   VTEXT_ADDRL
      BCC   @done
      INC   VTEXT_ADDRH
@done:
      RTS

vtext_define_region:
      JSR   vtext_validate_region
      BNE   @done
      JSR   vtext_region_slot
      LDY   #VTEXT_REGION_LEFT
      LDA   VTEXT_LEFT
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_TOP
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_WIDTH
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_HEIGHT
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_CURX
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_CURY
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_COLOR
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_ATTR
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   VTEXT_FLAGS
      STA   (VTEXT_ADDRL),Y
      INY
      LDA   #$00
      STA   (VTEXT_ADDRL),Y
      LDA   #VTEXT_OK
@done:
      RTS

vtext_select_region:
      JSR   vtext_region_slot
      LDY   #VTEXT_REGION_LEFT
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_LEFT
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_TOP
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_WIDTH
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_HEIGHT
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_CURX
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_CURY
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_COLOR
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_ATTR
      INY
      LDA   (VTEXT_ADDRL),Y
      STA   VTEXT_FLAGS
      JSR   vtext_validate_region
      BNE   @done
      JMP   vtext_set_cursor
@done:
      RTS

vtext_store_region:
      JSR   vtext_validate_region
      BNE   @done
      JSR   vtext_set_cursor
      BNE   @done
      JMP   vtext_define_region
@done:
      RTS

; ---------------------------------------------------------------------
; Validation and cursor helpers
; ---------------------------------------------------------------------
vtext_validate_region:
      LDA   VTEXT_WIDTH
      BEQ   @bad
      LDA   VTEXT_HEIGHT
      BEQ   @bad
      LDA   VTEXT_LEFT
      CMP   #VTEXT_SCREEN_COLS
      BCS   @bad
      CLC
      ADC   VTEXT_WIDTH
      BCS   @bad
      CMP   #VTEXT_SCREEN_COLS + 1
      BCS   @bad
      LDA   VTEXT_TOP
      CMP   #VTEXT_SCREEN_ROWS
      BCS   @bad
      CLC
      ADC   VTEXT_HEIGHT
      BCS   @bad
      CMP   #VTEXT_SCREEN_ROWS + 1
      BCS   @bad
      JMP   vtext_ok
@bad:
      JMP   vtext_err

vtext_sync_cursor:
      LDA   VTEXT_LEFT
      CLC
      ADC   VTEXT_CURX
      STA   VGC_CURSX
      LDA   VTEXT_TOP
      CLC
      ADC   VTEXT_CURY
      STA   VGC_CURSY
      RTS

vtext_set_cursor:
      JSR   vtext_validate_region
      BNE   @done
      LDA   VTEXT_CURX
      CMP   VTEXT_WIDTH
      BCS   @bad
      LDA   VTEXT_CURY
      CMP   VTEXT_HEIGHT
      BCS   @bad
      JSR   vtext_sync_cursor
      JMP   vtext_ok
@bad:
      JMP   vtext_err
@done:
      RTS

vtext_home:
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JMP   vtext_set_cursor

; ---------------------------------------------------------------------
; Addressing and VRAM writes
; ---------------------------------------------------------------------
vtext_calc_region_addr:
      LDA   VTEXT_CURX
      PHA
      LDA   VTEXT_CURY
      PHA
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JSR   vtext_calc_addr
      PLA
      STA   VTEXT_CURY
      PLA
      STA   VTEXT_CURX
      RTS

; VTEXT_ADDR = (TOP + CURY) * 80 + LEFT + CURX
vtext_calc_addr:
      LDA   VTEXT_TOP
      CLC
      ADC   VTEXT_CURY
      STA   VTEXT_TMP

      ASL
      ASL
      CLC
      ADC   VTEXT_TMP          ; row * 5
      STA   VTEXT_ADDRL
      STZ   VTEXT_ADDRH

      ASL   VTEXT_ADDRL
      ROL   VTEXT_ADDRH
      ASL   VTEXT_ADDRL
      ROL   VTEXT_ADDRH
      ASL   VTEXT_ADDRL
      ROL   VTEXT_ADDRH
      ASL   VTEXT_ADDRL
      ROL   VTEXT_ADDRH        ; row * 80

      LDA   VTEXT_LEFT
      CLC
      ADC   VTEXT_CURX
      CLC
      ADC   VTEXT_ADDRL
      STA   VTEXT_ADDRL
      BCC   @done
      INC   VTEXT_ADDRH
@done:
      RTS

vtext_select_vram_addr:
      STA   VGC_VRAM_PLANE
      LDA   VTEXT_ADDRL
      STA   VGC_VRAM_ADDRL
      LDA   VTEXT_ADDRH
      STA   VGC_VRAM_ADDRH
      LDA   #$00
      STA   VGC_VRAM_CTRL
      RTS

vtext_write_current_cell:
      JSR   vtext_calc_addr

      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_CHAR
      STA   VGC_VRAM_DATA

      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_COLOR
      STA   VGC_VRAM_DATA

      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_ATTR
      STA   VGC_VRAM_DATA

      JMP   vtext_ok

; ---------------------------------------------------------------------
; Character output
; ---------------------------------------------------------------------
vtext_put_char:
      JSR   vtext_validate_region
      BNE   @done
      LDA   VTEXT_CHAR
      CMP   #$0D
      BEQ   @cr
      CMP   #$0A
      BEQ   @lf
      CMP   #$08
      BEQ   @bs
      CMP   #$0C
      BEQ   @ff
      CMP   #$20
      BCC   @ok

      JSR   vtext_set_cursor
      BNE   @done
      JSR   vtext_write_current_cell
      BNE   @done
      JMP   vtext_advance_char
@cr:
      STZ   VTEXT_CURX
      JMP   vtext_set_cursor
@ok:
      JMP   vtext_ok
@lf:
      JMP   vtext_newline
@bs:
      LDA   VTEXT_CURX
      BEQ   @ok
      DEC   VTEXT_CURX
      JMP   vtext_set_cursor
@ff:
      JSR   vtext_clear_region
      BNE   @done
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JMP   vtext_set_cursor
@done:
      RTS

vtext_advance_char:
      INC   VTEXT_CURX
      LDA   VTEXT_CURX
      CMP   VTEXT_WIDTH
      BCC   @ok

      LDA   VTEXT_FLAGS
      AND   #VTEXT_FLAG_WRAP
      BNE   @wrap
      LDA   VTEXT_WIDTH
      SEC
      SBC   #$01
      STA   VTEXT_CURX
      JMP   vtext_set_cursor
@wrap:
      STZ   VTEXT_CURX
      JMP   vtext_advance_line
@ok:
      JMP   vtext_set_cursor

vtext_newline:
      JSR   vtext_validate_region
      BNE   @done
      STZ   VTEXT_CURX
      JMP   vtext_advance_line
@done:
      RTS

vtext_advance_line:
      INC   VTEXT_CURY
      LDA   VTEXT_CURY
      CMP   VTEXT_HEIGHT
      BCC   @ok

      LDA   VTEXT_FLAGS
      AND   #VTEXT_FLAG_SCROLL
      BEQ   @clamp
      JSR   vtext_scroll_up
      BNE   @done
@clamp:
      LDA   VTEXT_HEIGHT
      SEC
      SBC   #$01
      STA   VTEXT_CURY
@ok:
      JMP   vtext_set_cursor
@done:
      RTS

; ---------------------------------------------------------------------
; Clears and scrolling
; ---------------------------------------------------------------------
vtext_clear_region:
      JSR   vtext_validate_region
      BNE   @done
      JSR   vtext_calc_region_addr

      LDA   #$20
      STA   VTEXT_TMP
      LDX   VTEXT_HEIGHT
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_COLOR
      STA   VTEXT_TMP
      LDX   VTEXT_HEIGHT
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_ATTR
      STA   VTEXT_TMP
      LDX   VTEXT_HEIGHT
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_fill_plane
@done:
      RTS

vtext_clear_line:
      JSR   vtext_set_cursor
      BNE   @done
      LDA   VTEXT_CURX
      PHA
      STZ   VTEXT_CURX
      JSR   vtext_calc_addr
      PLA
      STA   VTEXT_CURX

      LDA   #$20
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_COLOR
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_ATTR
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_fill_plane
@done:
      RTS

vtext_scroll_up:
      JSR   vtext_validate_region
      BNE   @done
      LDA   VTEXT_HEIGHT
      CMP   #$01
      BNE   @copy_rows
      JMP   vtext_clear_region

@copy_rows:
      JSR   vtext_calc_region_addr
      LDA   VTEXT_HEIGHT
      SEC
      SBC   #$01
      STA   VTEXT_TMP

      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_copy_up_plane
      BNE   @done
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_copy_up_plane
      BNE   @done
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_copy_up_plane
      BNE   @done

      LDA   VTEXT_CURX
      PHA
      LDA   VTEXT_CURY
      PHA
      STZ   VTEXT_CURX
      LDA   VTEXT_HEIGHT
      SEC
      SBC   #$01
      STA   VTEXT_CURY
      JSR   vtext_calc_addr
      PLA
      STA   VTEXT_CURY
      PLA
      STA   VTEXT_CURX

      LDA   #$20
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_COLOR
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_fill_plane
      BNE   @done

      LDA   VTEXT_ATTR
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_fill_plane
@done:
      RTS

vtext_blt_fill_plane:
      STA   BLT_DSTSPACE
      STZ   BLT_SRCSPACE
      STZ   BLT_SRCL
      STZ   BLT_SRCM
      STZ   BLT_SRCH
      STZ   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      STZ   BLT_CKEY
      LDA   VTEXT_ADDRL
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      STX   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRH
      LDA   VTEXT_TMP
      STA   BLT_FILLVALUE
      JSR   blitter_start_fill
      JMP   vtext_blitter_result

vtext_blt_copy_up_plane:
      STA   BLT_SRCSPACE
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      CLC
      ADC   #VTEXT_SCREEN_COLS
      STA   BLT_SRCL
      LDA   VTEXT_ADDRH
      ADC   #$00
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   VTEXT_ADDRL
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_TMP
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRL
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRH
      STA   BLT_DSTSTRH
      JSR   blitter_start_copy
      JMP   vtext_blitter_result

vtext_wait_blitter:
      JSR   blitter_wait

vtext_blitter_result:
      CMP   #BLITTER_RESULT_OK
      BEQ   @ok
      JMP   vtext_err
@ok:
      JMP   vtext_ok

.endif
