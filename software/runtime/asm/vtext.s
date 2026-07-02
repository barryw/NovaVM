; Shared NovaVM text-region runtime.
;
; Provides small rectangular text regions over the VGC char/color/attribute
; planes. Domain runtimes such as NovaZ decide what a region means; this code
; only clips, clears, writes, and scrolls rectangular text areas.

.include "vtext.inc"
.include "blitter.inc"

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
VTEXT_TOPROW:     .res 1       ; visible-row -> physical-row base
VTEXT_SCROLL_TOP: .res 1       ; first visible row affected by VTEXT_TOPROW
VTEXT_SCROLL_ROWS:.res 1       ; 0 means full-screen ring
VTEXT_SCROLL_HOOKL:.res 1      ; optional automatic-scroll replacement
VTEXT_SCROLL_HOOKH:.res 1
VTEXT_TABLEL:     .res 1
VTEXT_TABLEH:     .res 1
VTEXT_REGION_ID:  .res 1

      .segment "ZEROPAGE"

VTEXT_ADDRL:      .res 1
VTEXT_ADDRH:      .res 1
VTEXT_TMP:        .res 1
VTEXT_STRL:       .res 1
VTEXT_STRH:       .res 1
VTEXT_RUNLEN:     .res 1

      .segment "CODE"

      .export vtext_define_region
      .export vtext_select_region
      .export vtext_store_region
      .export vtext_validate_region
      .export vtext_set_cursor
      .export vtext_sync_cursor
      .export vtext_home
      .export vtext_put_char
      .export vtext_put_run
      .export vtext_puts
      .export vtext_print_at
      .export vtext_put_hex_nibble
      .export vtext_put_hex_byte
      .export vtext_read_char
      .export vtext_newline
      .export vtext_set_scroll_hook
      .export vtext_clear_scroll_hook
      .export vtext_clear_region
      .export vtext_fill_style_region
      .export vtext_expose_gfx_spaces_region
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

; @label VTEXT.DEFINE_REGION
; @kind routine
; @symbol vtext_define_region
; @summary Store the current VTEXT region state into the caller-owned region table slot.
; @requires VTEXT_TABLEL VTEXT_TABLEH VTEXT_REGION_ID VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS
; @out A: 0 on success, 1 on error.
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

; @label VTEXT.SELECT_REGION
; @kind routine
; @symbol vtext_select_region
; @summary Load a region table slot into the current VTEXT state and sync the visible cursor.
; @requires VTEXT_TABLEL VTEXT_TABLEH VTEXT_REGION_ID
; @out A: 0 on success, 1 on error.
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

; @label VTEXT.STORE_REGION
; @kind routine
; @symbol vtext_store_region
; @summary Validate and sync the current cursor, then store the current VTEXT state into the selected table slot.
; @requires VTEXT_TABLEL VTEXT_TABLEH VTEXT_REGION_ID VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS
; @out A: 0 on success, 1 on error.
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
; @label VTEXT.VALIDATE_REGION
; @kind routine
; @symbol vtext_validate_region
; @summary Validate that the current VTEXT rectangle is non-empty and inside the 80x50 text screen.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT
; @out A: 0 on success, 1 on error.
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

; @label VTEXT.SYNC_CURSOR
; @kind routine
; @symbol vtext_sync_cursor
; @summary Copy the current region-relative cursor to the absolute VGC cursor registers.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_CURX VTEXT_CURY
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

; @label VTEXT.SET_CURSOR
; @kind routine
; @symbol vtext_set_cursor
; @summary Validate the current region and cursor, then sync the visible VGC cursor.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY
; @out A: 0 on success, 1 on error.
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

; @label VTEXT.HOME
; @kind routine
; @symbol vtext_home
; @summary Move the current VTEXT cursor to the upper-left cell of the current region.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT
; @out A: 0 on success, 1 on error.
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

; VTEXT_ADDR = physical_row(TOP + CURY) * 80 + LEFT + CURX
vtext_calc_addr:
      LDA   VTEXT_TOP
      CLC
      ADC   VTEXT_CURY
      STA   VTEXT_TMP

      LDA   VTEXT_SCROLL_ROWS
      BEQ   @full_ring

      LDA   VTEXT_TMP
      CMP   VTEXT_SCROLL_TOP
      BCC   @row_ready
      SEC
      SBC   VTEXT_SCROLL_TOP
      CMP   VTEXT_SCROLL_ROWS
      BCS   @row_ready
      CLC
      ADC   VTEXT_TOPROW
      CMP   VTEXT_SCROLL_ROWS
      BCC   @inside_ready
      SBC   VTEXT_SCROLL_ROWS
@inside_ready:
      CLC
      ADC   VTEXT_SCROLL_TOP
      STA   VTEXT_TMP
      BRA   @row_mapped

@full_ring:
      LDA   VTEXT_TMP
      CLC
      ADC   VTEXT_TOPROW
      CMP   #VTEXT_SCREEN_ROWS
      BCC   @store_full
      SBC   #VTEXT_SCREEN_ROWS
@store_full:
      STA   VTEXT_TMP
      BRA   @row_mapped

@row_ready:
      LDA   VTEXT_TMP

@row_mapped:

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

vtext_select_vram_addr_inc:
      STA   VGC_VRAM_PLANE
      LDA   VTEXT_ADDRL
      STA   VGC_VRAM_ADDRL
      LDA   VTEXT_ADDRH
      STA   VGC_VRAM_ADDRH
      LDA   #VTEXT_VRAM_AUTOINC
      STA   VGC_VRAM_CTRL
      RTS

vtext_write_current_cell:
      JSR   vtext_calc_addr

      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_COLOR
      STA   VGC_VRAM_DATA

      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_ATTR
      STA   VGC_VRAM_DATA

      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_CHAR
      STA   VGC_VRAM_DATA

      JMP   vtext_ok

; ---------------------------------------------------------------------
; Character output
; ---------------------------------------------------------------------
; @label VTEXT.PUT_CHAR
; @kind routine
; @symbol vtext_put_char
; @summary Print one character or control byte at the current VTEXT cursor.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS VTEXT_CHAR
; @out A: 0 on success, 1 on error.
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

; @label VTEXT.PUTS
; @kind routine
; @symbol vtext_puts
; @summary Print a zero-terminated string into the current VTEXT region.
; @in A/Y: Pointer to the zero-terminated string, low byte in A and high byte in Y.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
vtext_puts:
      STA   VTEXT_STRL
      STY   VTEXT_STRH
@loop:
      LDY   #$00
      LDA   (VTEXT_STRL),Y
      BEQ   @ok
      STA   VTEXT_CHAR
      JSR   vtext_put_char
      BNE   @done
      INC   VTEXT_STRL
      BNE   @loop
      INC   VTEXT_STRH
      BRA   @loop
@ok:
      LDA   #VTEXT_OK
@done:
      RTS

; @label VTEXT.PRINT_AT
; @kind routine
; @symbol vtext_print_at
; @summary Position the cursor at VTEXT_CURX,VTEXT_CURY in the current region,
;          then print a NUL-terminated string there (region-aware via vtext_puts).
;          The reusable "print string at (x,y)" primitive for the editor UI,
;          dialogs, and applications.
; @in A: string pointer low byte.
; @in Y: string pointer high byte.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 if the cursor was outside the region (nothing printed).
vtext_print_at:
      STA   VTEXT_STRL            ; hold the pointer across set_cursor (clobbers A)
      STY   VTEXT_STRH
      JSR   vtext_set_cursor
      BNE   @pa_done
      LDA   VTEXT_STRL
      LDY   VTEXT_STRH
      JMP   vtext_puts
@pa_done:
      RTS

; @label VTEXT.PUT_RUN
; @kind routine
; @symbol vtext_put_run
; @summary Write a same-style printable byte run on the current text row.
; @in A/Y: Pointer to the run bytes, low byte in A and high byte in Y.
; @in X: Number of bytes to write.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
; @note Does not interpret controls, wrap, or scroll. The run must fit on the
;       current row. If it exactly fills the row, the cursor is left on the final
;       cell because there is no valid one-past-right cursor position.
vtext_put_run:
      STA   VTEXT_STRL
      STY   VTEXT_STRH
      STX   VTEXT_RUNLEN
      BEQ   @ok

      JSR   vtext_validate_region
      BNE   @done
      LDA   VTEXT_CURY
      CMP   VTEXT_HEIGHT
      BCS   @bad
      LDA   VTEXT_CURX
      CMP   VTEXT_WIDTH
      BCS   @bad
      LDA   VTEXT_WIDTH
      SEC
      SBC   VTEXT_CURX
      CMP   VTEXT_RUNLEN
      BCC   @bad

      JSR   vtext_calc_addr

      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_select_vram_addr_inc
      LDY   #$00
      LDA   VTEXT_COLOR
@color_loop:
      STA   VGC_VRAM_DATA
      INY
      CPY   VTEXT_RUNLEN
      BCC   @color_loop

      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_select_vram_addr_inc
      LDY   #$00
      LDA   VTEXT_ATTR
@attr_loop:
      STA   VGC_VRAM_DATA
      INY
      CPY   VTEXT_RUNLEN
      BCC   @attr_loop

      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr_inc
      LDY   #$00
@char_loop:
      LDA   (VTEXT_STRL),Y
      STA   VGC_VRAM_DATA
      INY
      CPY   VTEXT_RUNLEN
      BCC   @char_loop

      STZ   VGC_VRAM_CTRL
      LDA   VTEXT_CURX
      CLC
      ADC   VTEXT_RUNLEN
      CMP   VTEXT_WIDTH
      BCC   @store_cursor
      LDA   VTEXT_WIDTH
      SEC
      SBC   #$01
@store_cursor:
      STA   VTEXT_CURX
      JMP   vtext_set_cursor
@bad:
      LDA   #VTEXT_ERR
      RTS
@ok:
      LDA   #VTEXT_OK
@done:
      RTS

; @label VTEXT.PUT_HEX_NIBBLE
; @kind routine
; @symbol vtext_put_hex_nibble
; @summary Print the low nibble of A as one uppercase hexadecimal digit.
; @in A: Value whose low nibble should be printed.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS
; @out A: 0 on success, 1 on error.
vtext_put_hex_nibble:
      AND   #$0F
      TAX
      LDA   vtext_hex_digits,X
      STA   VTEXT_CHAR
      JMP   vtext_put_char

; @label VTEXT.PUT_HEX_BYTE
; @kind routine
; @symbol vtext_put_hex_byte
; @summary Print A as two uppercase hexadecimal digits.
; @in A: Byte value to print.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS
; @out A: 0 on success, 1 on error.
vtext_put_hex_byte:
      PHA
      LSR
      LSR
      LSR
      LSR
      JSR   vtext_put_hex_nibble
      BNE   @fail
      PLA
      JMP   vtext_put_hex_nibble
@fail:
      PLA
      LDA   #VTEXT_ERR
      RTS

; @label VTEXT.READ_CHAR
; @kind routine
; @symbol vtext_read_char
; @summary Read the character byte at VTEXT.CURX/CURY in the current region.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY
; @out A: Character byte from the VGC character plane.
vtext_read_char:
      JSR   vtext_calc_addr
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr
      LDA   VGC_VRAM_DATA
      LDA   VGC_VRAM_DATA
      RTS

; @label VTEXT.NEWLINE
; @kind routine
; @symbol vtext_newline
; @summary Move to the start of the next VTEXT row, scrolling when enabled.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR VTEXT_FLAGS
; @out A: 0 on success, 1 on error.
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
      JSR   vtext_scroll_region
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

; @label VTEXT.CLEAR_SCROLL_HOOK
; @kind routine
; @symbol vtext_clear_scroll_hook
; @summary Restore default text-only automatic scrolling.
; @out A: 0.
vtext_clear_scroll_hook:
      LDA   #$00
      TAY

; @label VTEXT.SET_SCROLL_HOOK
; @kind routine
; @symbol vtext_set_scroll_hook
; @summary Install an automatic-scroll hook for the current process.
; @in A: Low byte of hook address.
; @in Y: High byte of hook address.
; @out A: 0.
; @details The hook is called instead of vtext_scroll_up when VTEXT newline
;          handling scrolls a region. Hooks must scroll the current text
;          planes themselves and return VTEXT_OK/VTEXT_ERR in A. This is for
;          mixed text/graphics apps that need to batch all visible planes from
;          one VTEXT-owned scroll event.
vtext_set_scroll_hook:
      STA   VTEXT_SCROLL_HOOKL
      STY   VTEXT_SCROLL_HOOKH
      JMP   vtext_ok

vtext_scroll_region:
      LDA   VTEXT_SCROLL_HOOKL
      ORA   VTEXT_SCROLL_HOOKH
      BEQ   @default
      JMP   (VTEXT_SCROLL_HOOKL)
@default:
      JMP   vtext_scroll_up

; ---------------------------------------------------------------------
; Clears and scrolling
; ---------------------------------------------------------------------
; @label VTEXT.CLEAR_REGION
; @kind routine
; @symbol vtext_clear_region
; @summary Fill the current VTEXT rectangle with spaces and the current color/attribute.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
vtext_clear_region:
      JSR   vtext_validate_region
      BNE   @done
      JSR   vtext_calc_region_addr

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
      BNE   @done

      LDA   #$20
      STA   VTEXT_TMP
      LDX   VTEXT_HEIGHT
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
@done:
      RTS

; @label VTEXT.FILL_STYLE_REGION
; @kind routine
; @symbol vtext_fill_style_region
; @summary Fill only the color and text-attribute planes of the current VTEXT region.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
vtext_fill_style_region:
      JSR   vtext_validate_region
      BNE   @done
      JSR   vtext_calc_region_addr

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

; @label VTEXT.EXPOSE_GFX_SPACES_REGION
; @kind routine
; @symbol vtext_expose_gfx_spaces_region
; @summary Apply the current color/attribute only to space cells in the current VTEXT region.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
; @note This is for split text/graphics compositing: callers can set VTEXT_COLOR
;       to a text-background key so blank cells reveal the graphics plane without
;       deleting non-space text.
vtext_expose_gfx_spaces_region:
      JSR   vtext_validate_region
      BEQ   :+
      JMP   @done
:

      LDA   VTEXT_CURX
      PHA
      LDA   VTEXT_CURY
      PHA
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   VTEXT_HEIGHT
      STA   VTEXT_RUNLEN

@row_loop:
      JSR   vtext_calc_addr
      LDA   VTEXT_ADDRL
      STA   VTEXT_STRL
      LDA   VTEXT_ADDRH
      STA   VTEXT_STRH

      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr_inc
      LDA   VGC_VRAM_DATA              ; prime VRAM read latch
      LDY   #$00
@cell_loop:
      LDA   VGC_VRAM_DATA
      CMP   #$20
      BNE   @next_cell

      TYA
      CLC
      ADC   VTEXT_STRL
      STA   VTEXT_ADDRL
      LDA   VTEXT_STRH
      ADC   #$00
      STA   VTEXT_ADDRH
      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_COLOR
      STA   VGC_VRAM_DATA

      TYA
      CLC
      ADC   VTEXT_STRL
      STA   VTEXT_ADDRL
      LDA   VTEXT_STRH
      ADC   #$00
      STA   VTEXT_ADDRH
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_select_vram_addr
      LDA   VTEXT_ATTR
      STA   VGC_VRAM_DATA

      TYA
      CLC
      ADC   #$01
      CLC
      ADC   VTEXT_STRL
      STA   VTEXT_ADDRL
      LDA   VTEXT_STRH
      ADC   #$00
      STA   VTEXT_ADDRH
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_select_vram_addr_inc
      LDA   VGC_VRAM_DATA              ; prime next-cell read after plane switch

@next_cell:
      INY
      CPY   VTEXT_WIDTH
      BCC   @cell_loop

      INC   VTEXT_CURY
      DEC   VTEXT_RUNLEN
      BNE   @row_loop

      STZ   VGC_VRAM_CTRL
      PLA
      STA   VTEXT_CURY
      PLA
      STA   VTEXT_CURX
      JMP   vtext_ok
@done:
      RTS

; @label VTEXT.CLEAR_LINE
; @kind routine
; @symbol vtext_clear_line
; @summary Clear the current VTEXT row across the full region width.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
vtext_clear_line:
      JSR   vtext_set_cursor
      BNE   @done
      LDA   VTEXT_CURX
      PHA
      STZ   VTEXT_CURX
      JSR   vtext_calc_addr
      PLA
      STA   VTEXT_CURX

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
      BNE   @done

      LDA   #$20
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
@done:
      RTS

; @label VTEXT.SCROLL_UP
; @kind routine
; @symbol vtext_scroll_up
; @summary Scroll the current VTEXT rectangle up by one text row and clear the bottom row.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
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

      LDA   #VTEXT_PLANE_COLOR
      JSR   vtext_blt_copy_up_plane
      BNE   @done
      LDA   #VTEXT_PLANE_TEXTATTR
      JSR   vtext_blt_copy_up_plane
      BNE   @done
      LDA   #VTEXT_PLANE_CHAR
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
      BNE   @done

      LDA   #$20
      STA   VTEXT_TMP
      LDX   #$01
      LDA   #VTEXT_PLANE_CHAR
      JSR   vtext_blt_fill_plane
@done:
      RTS

vtext_blt_fill_plane:
      STA   BLT_DSTSPACE
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
      JSR   blitter_fill
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

; @label VTEXT.WAIT_BLITTER
; @kind routine
; @symbol vtext_wait_blitter
; @summary Wait for the current blitter operation and convert the result to the VTEXT 0/1 status convention.
; @out A: 0 on success, 1 on error.
vtext_wait_blitter:
      JSR   blitter_wait

vtext_blitter_result:
      CMP   #BLITTER_RESULT_OK
      BEQ   @ok
      JMP   vtext_err
@ok:
      JMP   vtext_ok

vtext_hex_digits:
      .byte "0123456789ABCDEF"

.endif
