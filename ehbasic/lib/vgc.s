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
      .export vgc_cls
      .export vgc_set_fg
      .export vgc_set_bg
      .export vgc_set_border
      .export vgc_locate
      .export vgc_set_mode
      .export vgc_set_font
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
; @label VGC.COMMAND
; @kind routine
; @symbol vgc_cmd
; @summary Issue a raw VGC command without waiting for completion.
; @in A: VGC command byte.
vgc_cmd:
      STA   VGC_CMD
      RTS

; Wait until the command busy bit clears.
; @label VGC.WAIT_COMMAND
; @kind routine
; @symbol vgc_wait_cmd
; @summary Wait for the active VGC command to complete.
vgc_wait_cmd:
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

; Input: A = VGC command. Issue and wait for completion.
.ifndef VGC_NO_EXEC
; @label VGC.EXEC
; @kind routine
; @symbol vgc_exec
; @summary Issue a raw VGC command and wait for completion.
; @in A: VGC command byte.
vgc_exec:
      JSR   vgc_cmd
      JMP   vgc_wait_cmd
.endif

; Wait for the next frame counter tick.
; @label VGC.VSYNC
; @kind routine
; @symbol vgc_vsync
; @summary Wait until the VGC frame counter advances.
; @out A: Last observed frame counter value.
vgc_vsync:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

; @label VGC.CLS
; @kind routine
; @symbol vgc_cls
; @summary Clear the text screen through the VGC character output register.
vgc_cls:
      LDA   #$0C
      STA   VGC_CHAROUT
      RTS

; @label VGC.SET_FG
; @kind routine
; @symbol vgc_set_fg
; @summary Set the current text foreground colour.
; @in X: Foreground colour byte.
vgc_set_fg:
      STX   VGC_FGCOL
      RTS

; @label VGC.SET_BG
; @kind routine
; @symbol vgc_set_bg
; @summary Set the current text background colour.
; @in X: Background colour byte.
vgc_set_bg:
      STX   VGC_BGCOL
      RTS

; @label VGC.SET_BORDER
; @kind routine
; @symbol vgc_set_border
; @summary Set the VGC border colour.
; @in X: Border colour byte.
vgc_set_border:
      STX   VGC_BORDER
      RTS

; @label VGC.LOCATE
; @kind routine
; @symbol vgc_locate
; @summary Set the text cursor position from VGC.P0/VGC.P1.
; @in VGC_P0: Cursor X column.
; @in VGC_P1: Cursor Y row.
vgc_locate:
      LDA   VGC_P0
      STA   VGC_CURSX
      LDA   VGC_P1
      STA   VGC_CURSY
      RTS

; @label VGC.SET_MODE
; @kind routine
; @symbol vgc_set_mode
; @summary Set the active VGC graphics/text mode.
; @in X: Mode byte.
vgc_set_mode:
      STX   VGC_MODE
      RTS

; @label VGC.SET_FONT
; @kind routine
; @symbol vgc_set_font
; @summary Select the active text font slot.
; @in X: Font slot, 0..7.
vgc_set_font:
      STX   VGC_FONT
      RTS

; @label VGC.MEM_READ
; @kind routine
; @symbol vgc_mem_read
; @summary Read one byte from VGC memory using VGC.P0..VGC.P4.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P4
; @out VGC_P3: Read byte.
vgc_mem_read:
      LDA   #VCMD_MEMREAD
      .byte $2C

; @label VGC.MEM_WRITE
; @kind routine
; @symbol vgc_mem_write
; @summary Write one byte to VGC memory using VGC.P0..VGC.P4.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4
vgc_mem_write:
      LDA   #VCMD_MEMWRITE
      STA   VGC_CMD
      JMP   vgc_wait_cmd

; Enable reverse text by swapping the current foreground/background colours.
; @label VGC.REVERSE
; @kind routine
; @symbol vgc_reverse_default
; @summary Enable reverse text using the current foreground/background colours.
vgc_reverse_default:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #VTXT_REV
      STA   VGC_TXTFLAGS
      RTS

; Input: A = packed reverse attribute, low nibble fg and high nibble bg.
; @label VGC.REVERSE_EXPLICIT
; @kind routine
; @symbol vgc_reverse_explicit
; @summary Enable reverse text with explicit packed foreground/background colours.
; @in A: Packed reverse attribute, low nibble foreground and high nibble background.
vgc_reverse_explicit:
      STA   VGC_TXTREVATTR
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #(VTXT_REV | VTXT_REVEX)
      STA   VGC_TXTFLAGS
      RTS

; @label VGC.REVERSE_OFF
; @kind routine
; @symbol vgc_reverse_off
; @summary Disable reverse text.
vgc_reverse_off:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      STA   VGC_TXTFLAGS
      RTS

; @label VGC.FLASH_ON
; @kind routine
; @symbol vgc_flash_on
; @summary Enable flashing text.
vgc_flash_on:
      LDA   VGC_TXTFLAGS
      ORA   #VTXT_FLASH
      STA   VGC_TXTFLAGS
      RTS

; @label VGC.FLASH_OFF
; @kind routine
; @symbol vgc_flash_off
; @summary Disable flashing text.
vgc_flash_off:
      LDA   VGC_TXTFLAGS
      AND   #$FB
      STA   VGC_TXTFLAGS
      RTS

.ifndef VGC_NO_PRIMITIVES
; @label VGC.GCLS
; @kind routine
; @symbol vgc_gcls
; @summary Issue the VGC graphics clear command.
vgc_gcls:
      LDA   #VCMD_GCLS
      JMP   vgc_cmd

; @label VGC.GCOLOR
; @kind routine
; @symbol vgc_gcolor
; @summary Issue the VGC graphics colour command using VGC.P0/P1.
; @requires VGC_P0 VGC_P1
vgc_gcolor:
      LDA   #VCMD_GCOLOR
      JMP   vgc_cmd

; @label VGC.PLOT
; @kind routine
; @symbol vgc_plot
; @summary Issue the VGC plot command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
vgc_plot:
      LDA   #VCMD_PLOT
      JMP   vgc_cmd

; @label VGC.UNPLOT
; @kind routine
; @symbol vgc_unplot
; @summary Issue the VGC unplot command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
vgc_unplot:
      LDA   #VCMD_UNPLOT
      JMP   vgc_cmd

; @label VGC.LINE
; @kind routine
; @symbol vgc_line
; @summary Issue the VGC line command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
vgc_line:
      LDA   #VCMD_LINE
      JMP   vgc_cmd

; @label VGC.CIRCLE
; @kind routine
; @symbol vgc_circle
; @summary Issue the VGC circle/ellipse command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
vgc_circle:
      LDA   #VCMD_CIRCLE
      JMP   vgc_cmd

; @label VGC.GTEXT
; @kind routine
; @symbol vgc_gtext
; @summary Issue the VGC graphics text command using VGC.P0..VGC.P5 and FIO.NAME.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 FIO_NAME FIO_NAMELEN
vgc_gtext:
      LDA   #VCMD_GTEXT
      JMP   vgc_cmd

; @label VGC.RECT
; @kind routine
; @symbol vgc_rect
; @summary Issue the VGC rectangle command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
vgc_rect:
      LDA   #VCMD_RECT
      JMP   vgc_cmd

; @label VGC.FILL
; @kind routine
; @symbol vgc_fill
; @summary Issue the VGC filled rectangle command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
vgc_fill:
      LDA   #VCMD_FILL
      JMP   vgc_cmd

; @label VGC.PAINT
; @kind routine
; @symbol vgc_paint
; @summary Issue the VGC paint command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
vgc_paint:
      LDA   #VCMD_PAINT
      JMP   vgc_cmd
.endif

.endif
