; Shared Nova VGC palette helper routines.

.include "vgc_palette.inc"

.ifndef VGC_PALETTE_IMPLEMENTATION_INCLUDED
VGC_PALETTE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_upload_palette_rgb_xram

; Stream 16 RGB888 palette entries from XRAM to the VGC custom palette.
; Input:
;   XRAM_ADDRL/M/H = first palette byte (48 bytes: R,G,B repeated 16 times)
;   A              = palette mode to store after upload
; Return:
;   A = 0 on success, 1 on XRAM read error
; @label VGC.UPLOAD_PALETTE_RGB_XRAM
; @kind routine
; @symbol vgc_upload_palette_rgb_xram
; @summary Upload a 16-entry RGB888 custom VGC palette from XRAM.
; @in XRAM_ADDRL/M/H: Source address of 48 RGB bytes.
; @in A: Palette mode to select after upload.
vgc_upload_palette_rgb_xram:
      PHA
      STZ   VGC_PALIDX
      LDY   #$30
@loop:
      JSR   xram_read8
      BNE   @error
      LDA   XRAM_DATA
      STA   VGC_PALDATA
      INC   XRAM_ADDRL
      BNE   :+
      INC   XRAM_ADDRM
      BNE   :+
      INC   XRAM_ADDRH
:
      DEY
      BNE   @loop
      PLA
      STA   VGC_PALETTE
      LDA   #$00
      RTS
@error:
      PLA
      LDA   #$01
      RTS

.endif
