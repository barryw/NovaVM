; Shared Nova NVG image loader routines.

.include "nvg.inc"
.include "fio.s"

.ifndef NVG_IMPLEMENTATION_INCLUDED
NVG_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export nvg_load
      .export nvg_load_at
      .export nvg_load_named
      .export nvg_load_named_at

; Clear the graphics bitmap, then load the named NVG file to the top-left.
; The caller must already have populated FIO.NAME/FIO.NAMELEN.
; @label NVG.LOAD
; @kind routine
; @symbol nvg_load
; @summary Clear graphics bitmap memory, then decode FIO.NAME as an NVG1 file into graphics bitmap offset 0.
; @requires FIO_NAME FIO_NAMELEN
; @out A: 0 on success, 1 on error.
nvg_load:
      STZ   NVG_ADDRL
      STZ   NVG_ADDRH
      BRA   nvg_load_at

; Clear the graphics bitmap, then load the named NVG file to NVG.ADDRL/H.
; Destination offsets are linear 320-byte rows: offset = y * 320 + x.
; The caller must already have populated FIO.NAME/FIO.NAMELEN.
; @label NVG.LOAD_AT
; @kind routine
; @symbol nvg_load_at
; @summary Clear graphics bitmap memory, then decode FIO.NAME as an NVG1 file into graphics bitmap offset NVG.ADDRL/H.
; @requires FIO_NAME FIO_NAMELEN NVG_ADDRL NVG_ADDRH
; @out A: 0 on success, 1 on error.
nvg_load_at:
      LDA   #VGC_PLANE_GFX
      STA   FIO_GSPACE
      STZ   FIO_GLENL
      STZ   FIO_GLENH
      LDA   #FIO_CMD_NVGLOAD
      JMP   fio_exec

; Copy the pointer-based filename arguments and load to the top-left of the
; graphics bitmap.
; @label NVG.LOAD_NAMED
; @kind routine
; @symbol nvg_load_named
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then clear graphics bitmap memory and decode the NVG1 file at offset 0.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H
; @out A: 0 on success, 1 on error.
nvg_load_named:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load
@done:
      RTS

; Copy the pointer-based filename arguments, clear the bitmap, and load to NVG.ADDRL/H.
; @label NVG.LOAD_NAMED_AT
; @kind routine
; @symbol nvg_load_named_at
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then clear graphics bitmap memory and decode the NVG1 file at NVG.ADDRL/H.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H NVG_ADDRL NVG_ADDRH
; @out A: 0 on success, 1 on error.
nvg_load_named_at:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load_at
@done:
      RTS

.endif
