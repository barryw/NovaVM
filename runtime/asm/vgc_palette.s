; Shared Nova VGC palette helper routines.

.include "vgc_palette.inc"

.ifndef VGC_PALETTE_IMPLEMENTATION_INCLUDED
VGC_PALETTE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_set_palette_c64
      .export vgc_set_palette_ega
      .export vgc_set_palette_custom_xram
      .export vgc_upload_palette_rgb_xram

; Load the default Nova/C64-style palette into the active VGC palette.
; @label VGC.SET_PALETTE_C64
; @kind routine
; @symbol vgc_set_palette_c64
; @summary Load the default Nova/C64-style palette into the active VGC palette.
; @out A: 0 on success.
vgc_set_palette_c64:
      STZ   VGC_PALIDX
      LDX   #$00
@loop:
      LDA   vgc_palette_c64_rgb,X
      STA   VGC_PALDATA
      INX
      CPX   #VGC_CUSTOM_PALETTE_BYTES
      BNE   @loop
      LDA   #$00
      RTS

; Load the IBM EGA palette into the active VGC palette.
; @label VGC.SET_PALETTE_EGA
; @kind routine
; @symbol vgc_set_palette_ega
; @summary Load the IBM EGA palette into the active VGC palette.
; @out A: 0 on success.
vgc_set_palette_ega:
      STZ   VGC_PALIDX
      LDX   #$00
@loop:
      LDA   vgc_palette_ega_rgb,X
      STA   VGC_PALDATA
      INX
      CPX   #VGC_CUSTOM_PALETTE_BYTES
      BNE   @loop
      LDA   #$00
      RTS

; Stream 16 RGB888 palette entries from XRAM into the active VGC palette.
; Input:
;   XRAM_ADDRL/M/H = first palette byte (48 bytes: R,G,B repeated 16 times)
; Return:
;   A = 0 on success, 1 if the source address is outside XRAM
; @label VGC.SET_PALETTE_CUSTOM_XRAM
; @kind routine
; @symbol vgc_set_palette_custom_xram
; @summary Upload a 16-entry RGB888 palette from XRAM into the active VGC palette.
; @in XRAM_ADDRL/M/H: Source address of 48 RGB bytes.
; @out A: 0 on success, 1 if the source address is outside XRAM.
vgc_set_palette_custom_xram:
      JMP   vgc_upload_palette_rgb_xram

; Stream 16 RGB888 palette entries from XRAM to the active VGC palette.
; Input:
;   XRAM_ADDRL/M/H = first palette byte (48 bytes: R,G,B repeated 16 times)
; Return:
;   A = 0 on success, 1 if the source address is outside XRAM
; @label VGC.UPLOAD_PALETTE_RGB_XRAM
; @kind routine
; @symbol vgc_upload_palette_rgb_xram
; @summary Upload a 16-entry RGB888 palette from XRAM into the active VGC palette.
; @in XRAM_ADDRL/M/H: Source address of 48 RGB bytes.
; @out A: 0 on success, 1 if the source address is outside XRAM.
vgc_upload_palette_rgb_xram:
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
      LDA   #$00
      RTS
@error:
      LDA   #$01
      RTS

vgc_palette_c64_rgb:
      .byte $00,$00,$00, $FF,$FF,$FF, $88,$00,$00, $AA,$FF,$EE
      .byte $CC,$44,$CC, $00,$CC,$55, $00,$00,$AA, $EE,$EE,$77
      .byte $DD,$88,$55, $66,$44,$00, $FF,$77,$77, $33,$33,$33
      .byte $77,$77,$77, $AA,$FF,$66, $00,$88,$FF, $BB,$BB,$BB
vgc_palette_c64_rgb_end:
      .assert vgc_palette_c64_rgb_end - vgc_palette_c64_rgb = VGC_CUSTOM_PALETTE_BYTES, error, "C64 palette table must be 48 bytes"

vgc_palette_ega_rgb:
      .byte $00,$00,$00, $00,$00,$AA, $00,$AA,$00, $00,$AA,$AA
      .byte $AA,$00,$00, $AA,$00,$AA, $AA,$55,$00, $AA,$AA,$AA
      .byte $55,$55,$55, $55,$55,$FF, $55,$FF,$55, $55,$FF,$FF
      .byte $FF,$55,$55, $FF,$55,$FF, $FF,$FF,$55, $FF,$FF,$FF
vgc_palette_ega_rgb_end:
      .assert vgc_palette_ega_rgb_end - vgc_palette_ega_rgb = VGC_CUSTOM_PALETTE_BYTES, error, "EGA palette table must be 48 bytes"

.endif
