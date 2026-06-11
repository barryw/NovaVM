; Shared Nova VGC palette helper routines.

.include "vgc_palette.inc"

.ifndef VGC_PALETTE_IMPLEMENTATION_INCLUDED
VGC_PALETTE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_set_palette_c64
      .export vgc_set_palette_ega
      .export vgc_set_palette_custom_xram
      .export vgc_upload_palette_rgb_xram

; Select the default Nova/C64-style fixed VGC palette.
; @label VGC.SET_PALETTE_C64
; @kind routine
; @symbol vgc_set_palette_c64
; @summary Select the default Nova/C64-style fixed VGC palette.
; @out A: 0 on success.
vgc_set_palette_c64:
      LDA   #VGC_PALMODE_C64
      STA   VGC_PALETTE
      LDA   #$00
      RTS

; Select the fixed IBM EGA VGC palette.
; @label VGC.SET_PALETTE_EGA
; @kind routine
; @symbol vgc_set_palette_ega
; @summary Select the fixed IBM EGA VGC palette.
; @out A: 0 on success.
vgc_set_palette_ega:
      LDA   #VGC_PALMODE_EGA
      STA   VGC_PALETTE
      LDA   #$00
      RTS

; Stream 16 RGB888 palette entries from XRAM and select custom palette mode.
; Input:
;   XRAM_ADDRL/M/H = first palette byte (48 bytes: R,G,B repeated 16 times)
; Return:
;   A = 0 on success, 1 if the source address is outside XRAM
; @label VGC.SET_PALETTE_CUSTOM_XRAM
; @kind routine
; @symbol vgc_set_palette_custom_xram
; @summary Upload a 16-entry RGB888 custom VGC palette from XRAM and select it.
; @in XRAM_ADDRL/M/H: Source address of 48 RGB bytes.
; @out A: 0 on success, 1 if the source address is outside XRAM.
vgc_set_palette_custom_xram:
      LDA   #VGC_PALMODE_CUSTOM
      BRA   vgc_upload_palette_rgb_xram

; Stream 16 RGB888 palette entries from XRAM to the VGC custom palette.
; Input:
;   XRAM_ADDRL/M/H = first palette byte (48 bytes: R,G,B repeated 16 times)
;   A              = palette mode to store after upload
; Return:
;   A = 0 on success, 1 if the source address is outside XRAM
; @label VGC.UPLOAD_PALETTE_RGB_XRAM
; @kind routine
; @symbol vgc_upload_palette_rgb_xram
; @summary Upload a 16-entry RGB888 custom VGC palette from XRAM.
; @in XRAM_ADDRL/M/H: Source address of 48 RGB bytes.
; @in A: Palette mode to select after upload.
; @out A: 0 on success, 1 if the source address is outside XRAM.
vgc_upload_palette_rgb_xram:
      PHA
      STZ   VGC_PALIDX
      LDY   #VGC_CUSTOM_PALETTE_BYTES
@loop:
      LDA   XRAM_ADDRH
      CMP   #XRAM_CAPACITY_HIGH
      BCS   @error
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   XRAM_ADDRM
      STA   WIN3_MI
      LDA   XRAM_ADDRH
      STA   WIN3_HI
      LDX   XRAM_ADDRL
      LDA   XRAM_WIN3_BASE,X
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
