; =====================================================================
;  Nova NDK — vgc.s
;
;  VGC helper routines: command issue/wait, vsync, text colour/mode/font/
;  cursor control, reverse/flash attributes, per-character opaque background,
;  dimmer, IRQ install/enable/ack, VGC memory read/write, and graphics
;  primitives (plot, line, circle, rect, fill, paint, gtext). Assembly callers
;  set VGC_P* arguments and call these directly.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "vgc.inc"

.ifndef VGC_IMPLEMENTATION_INCLUDED
VGC_IMPLEMENTATION_INCLUDED = 1

.ifdef NOVA_STRIP_UNUSED
VGC_EMIT_ALL = 0
.else
.ifdef VGC_STRIP_UNUSED
VGC_EMIT_ALL = 0
.else
.ifdef NOVA_EMIT_ALL_RUNTIME
VGC_EMIT_ALL = 1
.else
.ifdef VGC_EMIT_ALL_RUNTIME
VGC_EMIT_ALL = 1
.else
VGC_EMIT_ALL = 0
.endif
.endif
.endif
.endif

.if VGC_EMIT_ALL = 0
.if .referenced(vgc_exec)
      .refto vgc_issue
      .refto vgc_wait_cmd
.endif
.if .referenced(vgc_wait_frames)
      .refto vgc_vsync
.endif
.if .referenced(vgc_cls) .OR .referenced(vgc_mem_read) .OR .referenced(vgc_mem_write)
      .refto vgc_wait_cmd
.endif
.if .referenced(vgc_gcls) .OR .referenced(vgc_gcolor) .OR .referenced(vgc_plot) .OR .referenced(vgc_unplot) .OR .referenced(vgc_line) .OR .referenced(vgc_circle) .OR .referenced(vgc_gtext) .OR .referenced(vgc_rect) .OR .referenced(vgc_fill) .OR .referenced(vgc_paint)
      .refto vgc_issue
.endif
.endif

      .segment "CODE"

.if VGC_EMIT_ALL .OR .referenced(vgc_issue)
      .export vgc_issue
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_wait_cmd)
      .export vgc_wait_cmd
.endif
.ifndef VGC_NO_EXEC
.if VGC_EMIT_ALL .OR .referenced(vgc_exec)
      .export vgc_exec
.endif
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_vsync)
      .export vgc_vsync
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_wait_frames)
      .export vgc_wait_frames
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_cls)
      .export vgc_cls
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_clear_eol)
      .export vgc_clear_eol
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_fg)
      .export vgc_set_fg
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_bg)
      .export vgc_set_bg
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_border)
      .export vgc_set_border
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_charbg)
      .export vgc_set_charbg
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_charbg_off)
      .export vgc_charbg_off
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_display_on)
      .export vgc_display_on
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_display_off)
      .export vgc_display_off
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_install)
      .export vgc_irq_install
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_enable)
      .export vgc_irq_enable
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_disable)
      .export vgc_irq_disable
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_ack)
      .export vgc_irq_ack
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_locate)
      .export vgc_locate
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_mode)
      .export vgc_set_mode
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_set_font)
      .export vgc_set_font
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_mem_read)
      .export vgc_mem_read
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_mem_write) .OR .referenced(vgc_mem_read)
      .export vgc_mem_write
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_default)
      .export vgc_reverse_default
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_explicit)
      .export vgc_reverse_explicit
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_off)
      .export vgc_reverse_off
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_flash_on)
      .export vgc_flash_on
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_flash_off)
      .export vgc_flash_off
.endif
.ifndef VGC_NO_PRIMITIVES
.if VGC_EMIT_ALL .OR .referenced(vgc_gcls)
      .export vgc_gcls
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_gcolor)
      .export vgc_gcolor
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_plot)
      .export vgc_plot
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_unplot)
      .export vgc_unplot
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_line)
      .export vgc_line
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_circle)
      .export vgc_circle
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_gtext)
      .export vgc_gtext
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_rect)
      .export vgc_rect
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_fill)
      .export vgc_fill
.endif
.if VGC_EMIT_ALL .OR .referenced(vgc_paint)
      .export vgc_paint
.endif
.endif

; Input: A = VGC command. Does not wait for completion.
; @label VGC.COMMAND
; @kind routine
; @symbol vgc_issue
; @summary Issue a raw VGC command without waiting for completion.
; @in A: VGC command byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_issue)
vgc_issue:
      STA   VGC_CMD
      RTS
.endif

; Wait until the command busy bit clears.
; @label VGC.WAIT_COMMAND
; @kind routine
; @symbol vgc_wait_cmd
; @summary Wait for the active VGC command to complete.
.if VGC_EMIT_ALL .OR .referenced(vgc_wait_cmd)
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
.endif

; Input: A = VGC command. Issue and wait for completion.
.ifndef VGC_NO_EXEC
; @label VGC.EXEC
; @kind routine
; @symbol vgc_exec
; @summary Issue a raw VGC command and wait for completion.
; @in A: VGC command byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_exec)
vgc_exec:
      JSR   vgc_issue
      JMP   vgc_wait_cmd
.endif
.endif

; Wait for the next frame counter tick.
; @label VGC.VSYNC
; @kind routine
; @symbol vgc_vsync
; @summary Wait until the VGC frame counter advances.
; @out A: Last observed frame counter value.
.if VGC_EMIT_ALL .OR .referenced(vgc_vsync)
vgc_vsync:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS
.endif

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
.if VGC_EMIT_ALL .OR .referenced(vgc_cls)
vgc_cls:
.ifdef VGC_CLS_EXT
      LDA   #EXT_CMD_CLS
      JMP   EXT_vec
.else
      LDA   #$0C
      STA   VGC_CHAROUT
      JMP   vgc_wait_cmd
.endif
.endif

; @label VGC.CLEAR_EOL
; @kind routine
; @symbol vgc_clear_eol
; @summary Clear from the current text cursor through the end of its row, preserving the cursor.
; @out A: VGC_RESULT_OK.
.if VGC_EMIT_ALL .OR .referenced(vgc_clear_eol)
vgc_clear_eol:
      LDA   VGC_CURSX
      CMP   #NOVA_SCREEN_COLS
      BCS   @done
      PHA
      LDA   VGC_CURSY
      PHA
      LDA   #NOVA_SCREEN_COLS
      SEC
      SBC   VGC_CURSX
      TAX
@clear:
      LDA   #' '
      STA   VGC_CHAROUT
      DEX
      BNE   @clear
      PLA
      STA   VGC_CURSY
      PLA
      STA   VGC_CURSX
@done:
      LDA   #VGC_RESULT_OK
      RTS
.endif

; @label VGC.SET_FG
; @kind routine
; @symbol vgc_set_fg
; @summary Set the current text foreground colour.
; @in X: Foreground colour byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_set_fg)
vgc_set_fg:
      STX   VGC_FGCOL
      RTS
.endif

; @label VGC.SET_BG
; @kind routine
; @symbol vgc_set_bg
; @summary Set the current text background colour.
; @in X: Background colour byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_set_bg)
vgc_set_bg:
      STX   VGC_BGCOL
      RTS
.endif

; @label VGC.SET_BORDER
; @kind routine
; @symbol vgc_set_border
; @summary Set the VGC border colour.
; @in X: Border colour byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_set_border)
vgc_set_border:
      STX   VGC_BORDER
      RTS
.endif

; @label VGC.SET_CHARBG
; @kind routine
; @symbol vgc_set_charbg
; @summary Give subsequently printed characters an OPAQUE background colour.
;   Each emitted character captures this colour into its own cell, independent
;   of the screen background. Call vgc_charbg_off to return to transparent.
; @in X: Background colour byte (0-15).
.if VGC_EMIT_ALL .OR .referenced(vgc_set_charbg)
vgc_set_charbg:
      STX   VGC_TEXT_BG
      RTS
.endif

; @label VGC.CHARBG_OFF
; @kind routine
; @symbol vgc_charbg_off
; @summary Make subsequently printed characters TRANSPARENT (the default):
;   each character's cell shows the live screen background (VGC_BGCOL / copper)
;   rather than a colour baked in at print time.
.if VGC_EMIT_ALL .OR .referenced(vgc_charbg_off)
vgc_charbg_off:
      LDA   #VGC_TEXT_BG_TRANSPARENT
      STA   VGC_TEXT_BG
      RTS
.endif

; @label VGC.DISPLAY_ON
; @kind routine
; @symbol vgc_display_on
; @summary Restore full VGC output brightness.
.if VGC_EMIT_ALL .OR .referenced(vgc_display_on)
vgc_display_on:
      LDA   #$0F
      STA   VGC_DIMMER
      RTS
.endif

; @label VGC.DISPLAY_OFF
; @kind routine
; @symbol vgc_display_off
; @summary Blank VGC output while keeping video timing active.
.if VGC_EMIT_ALL .OR .referenced(vgc_display_off)
vgc_display_off:
      LDA   #$00
      STA   VGC_DIMMER
      RTS
.endif

; @label VGC.IRQ_INSTALL
; @kind routine
; @symbol vgc_irq_install
; @summary Install the CPU IRQ vector used when enabled VGC IRQ sources fire.
; @in A: Handler address high byte.
; @in Y: Handler address low byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_install)
vgc_irq_install:
      PHP
      SEI
      STY   CPU_IRQ_VECTOR
      STA   CPU_IRQ_VECTOR+1
      PLP
      RTS
.endif

; @label VGC.IRQ_ENABLE
; @kind routine
; @symbol vgc_irq_enable
; @summary Enable one or more VGC IRQ source bits and enable maskable CPU IRQs.
; @in A: VGC IRQ source mask.
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_enable)
vgc_irq_enable:
      ORA   VGC_IRQ_ENABLE
      STA   VGC_IRQ_ENABLE
      CLI
      RTS
.endif

; @label VGC.IRQ_DISABLE
; @kind routine
; @symbol vgc_irq_disable
; @summary Disable one or more VGC IRQ source bits.
; @in A: VGC IRQ source mask.
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_disable)
vgc_irq_disable:
      EOR   #$FF
      AND   VGC_IRQ_ENABLE
      STA   VGC_IRQ_ENABLE
      RTS
.endif

; @label VGC.IRQ_ACK
; @kind routine
; @symbol vgc_irq_ack
; @summary Acknowledge pending VGC IRQ source bits.
; @in A: VGC IRQ source mask.
.if VGC_EMIT_ALL .OR .referenced(vgc_irq_ack)
vgc_irq_ack:
      STA   VGC_IRQ_STATUS
      RTS
.endif

; @label VGC.LOCATE
; @kind routine
; @symbol vgc_locate
; @summary Set the text cursor position from VGC.P0/VGC.P1.
; @in VGC_P0: Cursor X column.
; @in VGC_P1: Cursor Y row.
.if VGC_EMIT_ALL .OR .referenced(vgc_locate)
vgc_locate:
      LDA   VGC_P0
      STA   VGC_CURSX
      LDA   VGC_P1
      STA   VGC_CURSY
      RTS
.endif

; @label VGC.SET_MODE
; @kind routine
; @symbol vgc_set_mode
; @summary Set the active VGC graphics/text mode.
; @in X: Mode byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_set_mode)
vgc_set_mode:
      STX   VGC_MODE
      RTS
.endif

; @label VGC.SET_FONT
; @kind routine
; @symbol vgc_set_font
; @summary Select the active text font slot.
; @in X: Font slot, 0..7.
.if VGC_EMIT_ALL .OR .referenced(vgc_set_font)
vgc_set_font:
      STX   VGC_FONT
      RTS
.endif

; @label VGC.MEM_READ
; @kind routine
; @symbol vgc_mem_read
; @summary Read one byte from VGC memory using VGC.P0..VGC.P4.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P4
; @out VGC_P3: Read byte.
.if VGC_EMIT_ALL .OR .referenced(vgc_mem_read)
vgc_mem_read:
      LDA   #VCMD_MEMREAD
      .byte $2C
.endif

; @label VGC.MEM_WRITE
; @kind routine
; @symbol vgc_mem_write
; @summary Write one byte to VGC memory using VGC.P0..VGC.P4.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4
.if VGC_EMIT_ALL .OR .referenced(vgc_mem_write) .OR .referenced(vgc_mem_read)
vgc_mem_write:
      LDA   #VCMD_MEMWRITE
      STA   VGC_CMD
      JMP   vgc_wait_cmd
.endif

; Enable reverse text by swapping the current foreground/background colours.
; @label VGC.REVERSE
; @kind routine
; @symbol vgc_reverse_default
; @summary Enable reverse text using the current foreground/background colours.
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_default)
vgc_reverse_default:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #VTXT_REV
      STA   VGC_TXTFLAGS
      RTS
.endif

; Input: A = packed reverse attribute, low nibble fg and high nibble bg.
; @label VGC.REVERSE_EXPLICIT
; @kind routine
; @symbol vgc_reverse_explicit
; @summary Enable reverse text with explicit packed foreground/background colours.
; @in A: Packed reverse attribute, low nibble foreground and high nibble background.
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_explicit)
vgc_reverse_explicit:
      STA   VGC_TXTREVATTR
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #(VTXT_REV | VTXT_REVEX)
      STA   VGC_TXTFLAGS
      RTS
.endif

; @label VGC.REVERSE_OFF
; @kind routine
; @symbol vgc_reverse_off
; @summary Disable reverse text.
.if VGC_EMIT_ALL .OR .referenced(vgc_reverse_off)
vgc_reverse_off:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      STA   VGC_TXTFLAGS
      RTS
.endif

; @label VGC.FLASH_ON
; @kind routine
; @symbol vgc_flash_on
; @summary Enable flashing text.
.if VGC_EMIT_ALL .OR .referenced(vgc_flash_on)
vgc_flash_on:
      LDA   VGC_TXTFLAGS
      ORA   #VTXT_FLASH
      STA   VGC_TXTFLAGS
      RTS
.endif

; @label VGC.FLASH_OFF
; @kind routine
; @symbol vgc_flash_off
; @summary Disable flashing text.
.if VGC_EMIT_ALL .OR .referenced(vgc_flash_off)
vgc_flash_off:
      LDA   VGC_TXTFLAGS
      AND   #$FB
      STA   VGC_TXTFLAGS
      RTS
.endif

.ifndef VGC_NO_PRIMITIVES
; @label VGC.GCLS
; @kind routine
; @symbol vgc_gcls
; @summary Issue the VGC graphics clear command.
.if VGC_EMIT_ALL .OR .referenced(vgc_gcls)
vgc_gcls:
      LDA   #VCMD_GCLS
      JMP   vgc_issue
.endif

; @label VGC.GCOLOR
; @kind routine
; @symbol vgc_gcolor
; @summary Issue the VGC graphics colour command using VGC.P0.
; @requires VGC_P0
.if VGC_EMIT_ALL .OR .referenced(vgc_gcolor)
vgc_gcolor:
      LDA   #VCMD_GCOLOR
      JMP   vgc_issue
.endif

; @label VGC.PLOT
; @kind routine
; @symbol vgc_plot
; @summary Issue the VGC plot command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
.if VGC_EMIT_ALL .OR .referenced(vgc_plot)
vgc_plot:
      LDA   #VCMD_PLOT
      JMP   vgc_issue
.endif

; @label VGC.UNPLOT
; @kind routine
; @symbol vgc_unplot
; @summary Issue the VGC unplot command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
.if VGC_EMIT_ALL .OR .referenced(vgc_unplot)
vgc_unplot:
      LDA   #VCMD_UNPLOT
      JMP   vgc_issue
.endif

; @label VGC.LINE
; @kind routine
; @symbol vgc_line
; @summary Issue the VGC line command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
.if VGC_EMIT_ALL .OR .referenced(vgc_line)
vgc_line:
      LDA   #VCMD_LINE
      JMP   vgc_issue
.endif

; @label VGC.CIRCLE
; @kind routine
; @symbol vgc_circle
; @summary Issue the VGC circle/ellipse command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
.if VGC_EMIT_ALL .OR .referenced(vgc_circle)
vgc_circle:
      LDA   #VCMD_CIRCLE
      JMP   vgc_issue
.endif

; @label VGC.GTEXT
; @kind routine
; @symbol vgc_gtext
; @summary Issue the VGC graphics text command using VGC.P0..VGC.P5 and FIO.NAME.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 FIO_NAME FIO_NAMELEN
.if VGC_EMIT_ALL .OR .referenced(vgc_gtext)
vgc_gtext:
      LDA   #VCMD_GTEXT
      JMP   vgc_issue
.endif

; @label VGC.RECT
; @kind routine
; @symbol vgc_rect
; @summary Issue the VGC rectangle command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
.if VGC_EMIT_ALL .OR .referenced(vgc_rect)
vgc_rect:
      LDA   #VCMD_RECT
      JMP   vgc_issue
.endif

; @label VGC.FILL
; @kind routine
; @symbol vgc_fill
; @summary Issue the VGC filled rectangle command using VGC.P0..VGC.P7.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7
.if VGC_EMIT_ALL .OR .referenced(vgc_fill)
vgc_fill:
      LDA   #VCMD_FILL
      JMP   vgc_issue
.endif

; @label VGC.PAINT
; @kind routine
; @symbol vgc_paint
; @summary Issue the VGC paint command using VGC.P0..VGC.P3.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
.if VGC_EMIT_ALL .OR .referenced(vgc_paint)
vgc_paint:
      LDA   #VCMD_PAINT
      JMP   vgc_issue
.endif
.endif

.endif
