; Shared Nova VGC helper routines.
;
; BASIC should parse language syntax and load VGC registers; assembly callers
; can use the same routines directly after setting VGC_P* arguments.

.include "vgc.inc"

.ifndef VGC_IMPLEMENTATION_INCLUDED
VGC_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_cmd
      .export vgc_wait_cmd
.ifndef VGC_NO_EXEC
      .export vgc_exec
.endif
      .export vgc_vsync
      .export vgc_mem_read
      .export vgc_mem_write
      .export vgc_reverse_default
      .export vgc_reverse_explicit
      .export vgc_reverse_off
      .export vgc_flash_on
      .export vgc_flash_off
.ifndef VGC_NO_PRIMITIVES
      .export vgc_gcls
      .export vgc_gcolor
      .export vgc_plot
      .export vgc_unplot
      .export vgc_line
      .export vgc_circle
      .export vgc_gtext
      .export vgc_rect
      .export vgc_fill
      .export vgc_paint
.endif

; Input: A = VGC command. Does not wait for completion.
vgc_cmd:
      STA   VGC_CMD
      RTS

; Wait until the command busy bit clears.
vgc_wait_cmd:
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

; Input: A = VGC command. Issue and wait for completion.
.ifndef VGC_NO_EXEC
vgc_exec:
      JSR   vgc_cmd
      JMP   vgc_wait_cmd
.endif

; Wait for the next frame counter tick.
vgc_vsync:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

vgc_mem_read:
      LDA   #VCMD_MEMREAD
      .byte $2C

vgc_mem_write:
      LDA   #VCMD_MEMWRITE
      STA   VGC_CMD
      JMP   vgc_wait_cmd

; Enable reverse text by swapping the current foreground/background colours.
vgc_reverse_default:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #VTXT_REV
      STA   VGC_TXTFLAGS
      RTS

; Input: A = packed reverse attribute, low nibble fg and high nibble bg.
vgc_reverse_explicit:
      STA   VGC_TXTREVATTR
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #(VTXT_REV | VTXT_REVEX)
      STA   VGC_TXTFLAGS
      RTS

vgc_reverse_off:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      STA   VGC_TXTFLAGS
      RTS

vgc_flash_on:
      LDA   VGC_TXTFLAGS
      ORA   #VTXT_FLASH
      STA   VGC_TXTFLAGS
      RTS

vgc_flash_off:
      LDA   VGC_TXTFLAGS
      AND   #$FB
      STA   VGC_TXTFLAGS
      RTS

.ifndef VGC_NO_PRIMITIVES
vgc_gcls:
      LDA   #VCMD_GCLS
      JMP   vgc_cmd

vgc_gcolor:
      LDA   #VCMD_GCOLOR
      JMP   vgc_cmd

vgc_plot:
      LDA   #VCMD_PLOT
      JMP   vgc_cmd

vgc_unplot:
      LDA   #VCMD_UNPLOT
      JMP   vgc_cmd

vgc_line:
      LDA   #VCMD_LINE
      JMP   vgc_cmd

vgc_circle:
      LDA   #VCMD_CIRCLE
      JMP   vgc_cmd

vgc_gtext:
      LDA   #VCMD_GTEXT
      JMP   vgc_cmd

vgc_rect:
      LDA   #VCMD_RECT
      JMP   vgc_cmd

vgc_fill:
      LDA   #VCMD_FILL
      JMP   vgc_cmd

vgc_paint:
      LDA   #VCMD_PAINT
      JMP   vgc_cmd
.endif

.endif
