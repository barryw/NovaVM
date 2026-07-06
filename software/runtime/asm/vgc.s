; Shared Nova VGC helper routines.
;
; BASIC should parse language syntax and load VGC registers; assembly callers
; can use the same routines directly after setting VGC_P* arguments.

.include "vgc.inc"

.ifndef VGC_IMPLEMENTATION_INCLUDED
VGC_IMPLEMENTATION_INCLUDED = 1

.ifdef NOVA_EMIT_ALL_RUNTIME
VGC_EMIT_ALL = 1
.else
VGC_EMIT_ALL = 0
.endif

      .segment "CODE"

      .export vgc_cmd
      .export vgc_wait_cmd
.ifndef VGC_NO_EXEC
      .export vgc_exec
.endif
      .export vgc_vsync
; vgc_wait_frames is selective for source-included legacy consumers, but the
; canonical NDK archive must provide every symbol declared in vgc.inc.
.if VGC_EMIT_ALL .OR .referenced(vgc_wait_frames)
      .export vgc_wait_frames
.endif
      .export vgc_cls
      .export vgc_set_fg
      .export vgc_set_bg
      .export vgc_set_border
      .export vgc_display_on
      .export vgc_display_off
      .export vgc_irq_install
      .export vgc_irq_enable
      .export vgc_irq_disable
      .export vgc_irq_ack
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
      LDY   #$20
@arm:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      DEY
      BNE   @arm
      RTS
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

; Wait A video frames (busy-waits vgc_vsync A times). A=0 returns immediately.
; @label VGC.WAIT_FRAMES
; @kind routine
; @symbol vgc_wait_frames
; @summary Wait A video frames by busy-waiting the frame counter A times.
; @in A: frame count (0 returns immediately)
.if VGC_EMIT_ALL .OR .referenced(vgc_wait_frames)
vgc_wait_frames:
      TAX
      BEQ   @vwf_done
@vwf_loop:
      JSR   vgc_vsync          ; clobbers A, preserves X
      DEX
      BNE   @vwf_loop
@vwf_done:
      RTS
.endif

; @label VGC.CLS
; @kind routine
; @symbol vgc_cls
; @summary Clear the text screen through the VGC character output register and wait for completion.
vgc_cls:
.ifdef VGC_CLS_EXT
      LDA   #EXT_CMD_CLS
      JMP   EXT_vec
.else
      LDA   #$0C
      STA   VGC_CHAROUT
      JMP   vgc_wait_cmd
.endif

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

; @label VGC.SET_CHARBG
; @kind routine
; @symbol vgc_set_charbg
; @summary Give subsequently printed characters an OPAQUE background colour.
;   Each emitted character captures this colour into its own cell, independent
;   of the screen background. Call vgc_charbg_off to return to transparent.
; @in X: Background colour byte (0-15).
vgc_set_charbg:
      STX   VGC_TEXT_BG
      RTS

; @label VGC.CHARBG_OFF
; @kind routine
; @symbol vgc_charbg_off
; @summary Make subsequently printed characters TRANSPARENT (the default):
;   each character's cell shows the live screen background (VGC_BGCOL / copper)
;   rather than a colour baked in at print time.
vgc_charbg_off:
      LDA   #VGC_TEXT_BG_TRANSPARENT
      STA   VGC_TEXT_BG
      RTS

; @label VGC.DISPLAY_ON
; @kind routine
; @symbol vgc_display_on
; @summary Restore full VGC output brightness.
vgc_display_on:
      LDA   #$0F
      STA   VGC_DIMMER
      RTS

; @label VGC.DISPLAY_OFF
; @kind routine
; @symbol vgc_display_off
; @summary Blank VGC output while keeping video timing active.
vgc_display_off:
      LDA   #$00
      STA   VGC_DIMMER
      RTS

; @label VGC.IRQ_INSTALL
; @kind routine
; @symbol vgc_irq_install
; @summary Install the CPU IRQ vector used when enabled VGC IRQ sources fire.
; @in A: Handler address high byte.
; @in Y: Handler address low byte.
vgc_irq_install:
      PHP
      SEI
      STY   CPU_IRQ_VECTOR
      STA   CPU_IRQ_VECTOR+1
      PLP
      RTS

; @label VGC.IRQ_ENABLE
; @kind routine
; @symbol vgc_irq_enable
; @summary Enable one or more VGC IRQ source bits and enable maskable CPU IRQs.
; @in A: VGC IRQ source mask.
vgc_irq_enable:
      ORA   VGC_IRQ_ENABLE
      STA   VGC_IRQ_ENABLE
      CLI
      RTS

; @label VGC.IRQ_DISABLE
; @kind routine
; @symbol vgc_irq_disable
; @summary Disable one or more VGC IRQ source bits.
; @in A: VGC IRQ source mask.
vgc_irq_disable:
      EOR   #$FF
      AND   VGC_IRQ_ENABLE
      STA   VGC_IRQ_ENABLE
      RTS

; @label VGC.IRQ_ACK
; @kind routine
; @symbol vgc_irq_ack
; @summary Acknowledge pending VGC IRQ source bits.
; @in A: VGC IRQ source mask.
vgc_irq_ack:
      STA   VGC_IRQ_STATUS
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
; @summary Issue the VGC graphics colour command using VGC.P0.
; @requires VGC_P0
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
