; Shared Nova NVG image loader routines.

.include "nvg.inc"
.ifndef FIO_EMIT_ALL_RUNTIME
FIO_EMIT_ALL_RUNTIME = 1
.endif
.include "pager.s"
.include "blitter.s"

.ifndef NVG_IMPLEMENTATION_INCLUDED
NVG_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

nvg_header:       .res NVG_HEADER_SIZE + NVG_PALETTE_BYTES
nvg_base_l:       .res 1
nvg_base_m:       .res 1
nvg_base_h:       .res 1
nvg_dst_l:        .res 1
nvg_dst_h:        .res 1
nvg_clear_before: .res 1
nvg_tmp_l:        .res 1

      .segment "CODE"

      .export nvg_load
      .export nvg_load_at
      .export nvg_load_named
      .export nvg_load_named_at
      .export nvg_draw
      .export nvg_draw_at
      .export nvg_draw_named
      .export nvg_draw_named_at

; Clear the graphics bitmap as needed, then load the named NVG file to the
; top-left. Native NVG2 full-screen opaque images skip the clear because the
; image overwrites every pixel.
; The caller must already have populated FIO.NAME/FIO.NAMELEN.
; @label NVG.LOAD
; @kind routine
; @symbol nvg_load
; @summary Load FIO.NAME as an NVG image into graphics bitmap offset 0.
; @requires FIO_NAME FIO_NAMELEN
; @out A: 0 on success, 1 on error.
nvg_load:
      STZ   NVG_ADDRL
      STZ   NVG_ADDRH
      STZ   NVG_FILEL
      STZ   NVG_FILEM
      STZ   NVG_FILEH
      BRA   nvg_load_at

; Clear the graphics bitmap as needed, then load the named NVG file to
; NVG.ADDRL/H. Destination offsets are linear 320-byte rows:
; offset = y * 320 + x.
; The caller must already have populated FIO.NAME/FIO.NAMELEN.
; @label NVG.LOAD_AT
; @kind routine
; @symbol nvg_load_at
; @summary Load FIO.NAME as an NVG image into graphics bitmap offset NVG.ADDRL/H.
; @requires FIO_NAME FIO_NAMELEN NVG_ADDRL NVG_ADDRH
; @out A: 0 on success, 1 on error.
nvg_load_at:
      LDA   #$01
      STA   nvg_clear_before
      BRA   nvg_draw_current

; Draw the named NVG file to the top-left without clearing the graphics plane.
; @label NVG.DRAW
; @kind routine
; @symbol nvg_draw
; @summary Draw FIO.NAME as an NVG image into graphics bitmap offset 0 without clearing.
; @requires FIO_NAME FIO_NAMELEN
; @out A: 0 on success, 1 on error.
nvg_draw:
      STZ   NVG_ADDRL
      STZ   NVG_ADDRH
      STZ   NVG_FILEL
      STZ   NVG_FILEM
      STZ   NVG_FILEH
      BRA   nvg_draw_at

; Draw the named NVG file to NVG.ADDRL/H without clearing the graphics plane.
; Destination offsets are linear 320-byte rows: offset = y * 320 + x.
; The caller must already have populated FIO.NAME/FIO.NAMELEN.
; NVG.FILEL/M/H may select an image record inside a larger file.
; @label NVG.DRAW_AT
; @kind routine
; @symbol nvg_draw_at
; @summary Draw FIO.NAME as an NVG image into graphics bitmap offset NVG.ADDRL/H without clearing.
; @requires FIO_NAME FIO_NAMELEN NVG_ADDRL NVG_ADDRH NVG_FILEL NVG_FILEM NVG_FILEH
; @out A: 0 on success, 1 on error.
nvg_draw_at:
      STZ   nvg_clear_before
nvg_draw_current:
      JSR   nvg_ensure_name_extension
      BNE   @done
      JSR   nvg_remember_args
      JSR   nvg_load_header
      BNE   @done
      JSR   nvg_is_nvg2
      BCC   @bad_magic
      JMP   nvg_draw_native
@bad_magic:
      JMP   fio_set_io_error
@done:
      RTS

; Copy the pointer-based filename arguments and load to the top-left of the
; graphics bitmap.
; @label NVG.LOAD_NAMED
; @kind routine
; @symbol nvg_load_named
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then load the NVG image at offset 0.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H
; @out A: 0 on success, 1 on error.
nvg_load_named:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load
@done:
      RTS

; Copy the pointer-based filename arguments, clear as needed, and load to
; NVG.ADDRL/H.
; @label NVG.LOAD_NAMED_AT
; @kind routine
; @symbol nvg_load_named_at
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then load the NVG image at NVG.ADDRL/H.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H NVG_ADDRL NVG_ADDRH
; @out A: 0 on success, 1 on error.
nvg_load_named_at:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load_at
@done:
      RTS

; Copy pointer-based filename arguments and draw to the top-left without
; clearing the graphics plane.
; @label NVG.DRAW_NAMED
; @kind routine
; @symbol nvg_draw_named
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then draw the NVG image at offset 0 without clearing.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H
; @out A: 0 on success, 1 on error.
nvg_draw_named:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_draw
@done:
      RTS

; Copy pointer-based filename arguments and draw to NVG.ADDRL/H without
; clearing the graphics plane. NVG.FILEL/M/H may select an image record inside
; a larger file.
; @label NVG.DRAW_NAMED_AT
; @kind routine
; @symbol nvg_draw_named_at
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then draw the NVG image at NVG.ADDRL/H without clearing.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H NVG_ADDRL NVG_ADDRH NVG_FILEL NVG_FILEM NVG_FILEH
; @out A: 0 on success, 1 on error.
nvg_draw_named_at:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_draw_at
@done:
      RTS

nvg_ensure_name_extension:
      LDA   FIO_NAMELEN
      BEQ   @bad
      TAX
      LDY   #$00
@scan:
      LDA   FIO_NAME,Y
      CMP   #'.'
      BEQ   @ok
      INY
      DEX
      BNE   @scan

      LDA   FIO_NAMELEN
      CMP   #(FIO_NAME_LIMIT - 3)
      BCS   @bad
      TAY
      LDA   #'.'
      STA   FIO_NAME,Y
      INY
      LDA   #'N'
      STA   FIO_NAME,Y
      INY
      LDA   #'V'
      STA   FIO_NAME,Y
      INY
      LDA   #'G'
      STA   FIO_NAME,Y
      CLC
      LDA   FIO_NAMELEN
      ADC   #$04
      STA   FIO_NAMELEN
@ok:
      LDA   #FIO_RESULT_OK
      RTS
@bad:
      JMP   fio_set_io_error

nvg_remember_args:
      LDA   NVG_FILEL
      STA   nvg_base_l
      LDA   NVG_FILEM
      STA   nvg_base_m
      LDA   NVG_FILEH
      STA   nvg_base_h
      LDA   NVG_ADDRL
      STA   nvg_dst_l
      LDA   NVG_ADDRH
      STA   nvg_dst_h
      RTS

nvg_load_header:
      LDA   nvg_base_l
      STA   PAGER_FILEL
      LDA   nvg_base_m
      STA   PAGER_FILEM
      LDA   nvg_base_h
      STA   PAGER_FILEH
      LDA   #<nvg_header
      STA   PAGER_ADDRL
      LDA   #>nvg_header
      STA   PAGER_ADDRM
      STZ   PAGER_ADDRH
      LDA   #NVG_HEADER_SIZE
      STA   PAGER_LENL
      STZ   PAGER_LENH
      LDA   #PAGER_TARGET_RAM
      STA   PAGER_TARGET
      JMP   pager_load_current_file_page

nvg_is_nvg2:
      LDA   nvg_header
      CMP   #NVG_MAGIC_0
      BNE   @no
      LDA   nvg_header+1
      CMP   #NVG_MAGIC_1
      BNE   @no
      LDA   nvg_header+2
      CMP   #NVG_MAGIC_2
      BNE   @no
      LDA   nvg_header+3
      CMP   #NVG_MAGIC_2_VER
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

nvg_draw_native:
      JSR   nvg_validate_native
      BNE   @done
      JSR   nvg_load_native_palette
      BNE   @done
      JSR   nvg_maybe_clear_gfx
      BNE   @done
      JSR   nvg_load_native_payload
      BNE   @done
      JMP   nvg_blit_native_payload
@done:
      RTS

nvg_validate_native:
      LDA   nvg_header+14
      ORA   nvg_header+15
      BNE   @bad
      LDA   nvg_header+4
      ORA   nvg_header+5
      BEQ   @bad
      LDA   nvg_header+6
      ORA   nvg_header+7
      BEQ   @bad
      LDA   nvg_header+11
      BNE   @ok
      LDA   nvg_header+10
      CMP   #NVG_HEADER_SIZE
      BCC   @bad
@ok:
      LDA   #FIO_RESULT_OK
      RTS
@bad:
      JMP   fio_set_io_error

nvg_load_native_palette:
      LDA   nvg_header+8
      AND   #NVG_FLAG_PALETTE
      BEQ   @ok
      CLC
      LDA   nvg_base_l
      ADC   #NVG_HEADER_SIZE
      STA   PAGER_FILEL
      LDA   nvg_base_m
      ADC   #$00
      STA   PAGER_FILEM
      LDA   nvg_base_h
      ADC   #$00
      STA   PAGER_FILEH
      LDA   #<(nvg_header+NVG_HEADER_SIZE)
      STA   PAGER_ADDRL
      LDA   #>(nvg_header+NVG_HEADER_SIZE)
      STA   PAGER_ADDRM
      STZ   PAGER_ADDRH
      LDA   #NVG_PALETTE_BYTES
      STA   PAGER_LENL
      STZ   PAGER_LENH
      LDA   #PAGER_TARGET_RAM
      STA   PAGER_TARGET
      JSR   pager_load_current_file_page
      BNE   @done
      STZ   VGC_PALIDX
      LDX   #$00
@loop:
      LDA   nvg_header+NVG_HEADER_SIZE,X
      STA   VGC_PALDATA
      INX
      CPX   #NVG_PALETTE_BYTES
      BNE   @loop
      LDA   #VGC_PALMODE_CUSTOM
      STA   VGC_PALETTE
@ok:
      LDA   #FIO_RESULT_OK
@done:
      RTS

nvg_maybe_clear_gfx:
      LDA   nvg_clear_before
      BEQ   @ok
      LDA   nvg_dst_l
      ORA   nvg_dst_h
      BNE   @clear
      LDA   nvg_header+8
      AND   #NVG_FLAG_TRANSPARENT
      BNE   @clear
      LDA   nvg_header+4
      CMP   #<NVG_SCREEN_WIDTH
      BNE   @clear
      LDA   nvg_header+5
      CMP   #>NVG_SCREEN_WIDTH
      BNE   @clear
      LDA   nvg_header+6
      CMP   #<NVG_SCREEN_HEIGHT
      BNE   @clear
      LDA   nvg_header+7
      CMP   #>NVG_SCREEN_HEIGHT
      BNE   @clear
@ok:
      LDA   #FIO_RESULT_OK
      RTS
@clear:
      STZ   BLT_SRCSPACE
      STZ   BLT_SRCL
      STZ   BLT_SRCM
      STZ   BLT_SRCH
      STZ   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      STZ   BLT_DSTL
      STZ   BLT_DSTM
      STZ   BLT_DSTH
      LDA   #<NVG_SCREEN_WIDTH
      STA   BLT_WIDTHL
      LDA   #>NVG_SCREEN_WIDTH
      STA   BLT_WIDTHH
      LDA   #<NVG_SCREEN_HEIGHT
      STA   BLT_HEIGHTL
      LDA   #>NVG_SCREEN_HEIGHT
      STA   BLT_HEIGHTH
      LDA   #<NVG_GFX_STRIDE
      STA   BLT_DSTSTRL
      LDA   #>NVG_GFX_STRIDE
      STA   BLT_DSTSTRH
      STZ   BLT_FILLVALUE
      STZ   BLT_CKEY
      JMP   blitter_start_fill

nvg_load_native_payload:
      CLC
      LDA   nvg_base_l
      ADC   nvg_header+10
      STA   PAGER_FILEL
      LDA   nvg_base_m
      ADC   nvg_header+11
      STA   PAGER_FILEM
      LDA   nvg_base_h
      ADC   #$00
      STA   PAGER_FILEH
      LDA   #XRAM_NVG_STAGE_L
      STA   PAGER_ADDRL
      LDA   #XRAM_NVG_STAGE_M
      STA   PAGER_ADDRM
      LDA   #XRAM_NVG_STAGE_H
      STA   PAGER_ADDRH
      LDA   nvg_header+12
      STA   PAGER_LENL
      LDA   nvg_header+13
      STA   PAGER_LENH
      LDA   #PAGER_TARGET_XRAM
      STA   PAGER_TARGET
      JMP   pager_load_current_file_page

nvg_blit_native_payload:
      LDA   #BLT_SPACE_XRAM
      STA   BLT_SRCSPACE
      LDA   #XRAM_NVG_STAGE_L
      STA   BLT_SRCL
      LDA   #XRAM_NVG_STAGE_M
      STA   BLT_SRCM
      LDA   #XRAM_NVG_STAGE_H
      STA   BLT_SRCH
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   nvg_dst_l
      STA   BLT_DSTL
      LDA   nvg_dst_h
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   nvg_header+4
      STA   BLT_WIDTHL
      LDA   nvg_header+5
      STA   BLT_WIDTHH
      LDA   nvg_header+6
      STA   BLT_HEIGHTL
      LDA   nvg_header+7
      STA   BLT_HEIGHTH
      LDA   nvg_header+4
      CLC
      ADC   #$01
      STA   nvg_tmp_l
      LDA   nvg_header+5
      ADC   #$00
      LSR   A
      STA   BLT_SRCSTRH
      LDA   nvg_tmp_l
      ROR   A
      STA   BLT_SRCSTRL
      LDA   #<NVG_GFX_STRIDE
      STA   BLT_DSTSTRL
      LDA   #>NVG_GFX_STRIDE
      STA   BLT_DSTSTRH
      LDA   nvg_header+8
      AND   #NVG_FLAG_TRANSPARENT
      BEQ   @opaque
      LDA   nvg_header+9
      AND   #$0F
      BRA   @keyed
@opaque:
      LDA   #$FF
@keyed:
      STA   BLT_CKEY
      JMP   blitter_start_gfx4_unpack

.endif
