; Shared Nova VGC palette helper declarations.

.ifndef VGC_PALETTE_INC_INCLUDED
VGC_PALETTE_INC_INCLUDED = 1

.include "xram.inc"

; Custom palettes contain 16 RGB entries: R0,G0,B0 ... R15,G15,B15.
; @label VGC.PALETTE_COLORS
; @kind const
; @symbol VGC_PALETTE_COLORS
; @summary Number of color registers in a VGC custom palette.
VGC_PALETTE_COLORS        = $10
; @label VGC.PALETTE_RGB_STRIDE
; @kind const
; @symbol VGC_PALETTE_RGB_STRIDE
; @summary Number of bytes per custom palette entry: red, green, blue.
VGC_PALETTE_RGB_STRIDE    = $03
; @label VGC.CUSTOM_PALETTE_BYTES
; @kind const
; @symbol VGC_CUSTOM_PALETTE_BYTES
; @summary Number of RGB bytes consumed by VGC.PALDATA for one full custom palette.
VGC_CUSTOM_PALETTE_BYTES  = VGC_PALETTE_COLORS * VGC_PALETTE_RGB_STRIDE

.global vgc_set_palette_c64
.global vgc_set_palette_ega
.global vgc_set_palette_custom_xram
.global vgc_upload_palette_rgb_xram

.endif
