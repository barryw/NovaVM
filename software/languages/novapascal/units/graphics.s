; NovaGraphics Pascal unit adapter over the canonical VGC NDK.

.include "graphics.inc"

.ifndef NOVA_GRAPHICS_UNIT_IMPLEMENTATION_INCLUDED
NOVA_GRAPHICS_UNIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export graphicsopen
      .export graphicsclear
      .export graphicscolor
      .export graphicsfill
      .export graphicstile16
      .export graphicstile4x8
      .export graphicswait
      .export graphicsclose

graphicsopen:
      LDX   #VGC_MODE_GFX_ONLY
      JSR   vgc_set_mode
      STZ   VGC_P0
      JSR   vgc_gcolor
      JSR   vgc_wait_cmd
      JSR   vgc_gcls
      JMP   vgc_wait_cmd

graphicsclear:
      STA   VGC_P0
      JSR   vgc_gcolor
      JSR   vgc_wait_cmd
      JSR   vgc_gcls
      JMP   vgc_wait_cmd

graphicscolor:
      STA   VGC_P0
      JSR   vgc_gcolor
      JMP   vgc_wait_cmd

; Caller pushes X0, Y0, X1, Y1 before JSR; the return address occupies the
; first two bytes above SP while the routine runs.
graphicsfill:
      TSX
      LDA   $0106,X
      STA   VGC_P0
      STZ   VGC_P1
      LDA   $0105,X
      STA   VGC_P2
      STZ   VGC_P3
      LDA   $0104,X
      STA   VGC_P4
      STZ   VGC_P5
      LDA   $0103,X
      STA   VGC_P6
      STZ   VGC_P7
      JSR   vgc_fill
      JMP   vgc_wait_cmd

; Fill one cell in a full-width 20-by-12 grid. Column and row are caller-
; pushed bytes; the grid is centered vertically in the 320-by-200 plane.
graphicstile16:
      TSX
      LDA   $0104,X
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      STA   VGC_P0
      LDA   #$00
      ADC   #$00
      STA   VGC_P1
      LDA   VGC_P0
      CLC
      ADC   #15
      STA   VGC_P4
      LDA   VGC_P1
      ADC   #$00
      STA   VGC_P5
      LDA   $0103,X
      ASL   A
      ASL   A
      ASL   A
      ASL   A
      CLC
      ADC   #4
      STA   VGC_P2
      STZ   VGC_P3
      CLC
      ADC   #15
      STA   VGC_P6
      STZ   VGC_P7
      JSR   vgc_fill
      JMP   vgc_wait_cmd

; Fill one cell in a full-screen 80-by-25 grid. A cell is four physical pixels
; wide by eight high. Column and row are caller-pushed bytes.
graphicstile4x8:
      TSX
      STZ   VGC_P1
      LDA   $0104,X
      ASL   A
      ROL   VGC_P1
      ASL   A
      ROL   VGC_P1
      STA   VGC_P0
      CLC
      ADC   #3
      STA   VGC_P4
      LDA   VGC_P1
      ADC   #$00
      STA   VGC_P5
      LDA   $0103,X
      ASL   A
      ASL   A
      ASL   A
      STA   VGC_P2
      STZ   VGC_P3
      CLC
      ADC   #7
      STA   VGC_P6
      STZ   VGC_P7
      JSR   vgc_fill
      JMP   vgc_wait_cmd

graphicswait:
      JMP   vgc_wait_frames

graphicsclose:
      JSR   vgc_wait_cmd
      LDX   #VGC_MODE_TEXT
      JMP   vgc_set_mode

.include "vgc.s"

.endif
