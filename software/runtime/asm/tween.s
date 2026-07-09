; =====================================================================
;  Nova NDK — tween.s
;
;  Fixed-point tween/easing: linear and quadratic ease-in/out/in-out curves
;  over a 16-bit start/end value across TWEEN.DURATION frames. Callers set
;  TWEEN.START*, TWEEN.END*, TWEEN.DURATION, and TWEEN.FRAME, then call an
;  eval routine; the step routines increment TWEEN.FRAME first, then evaluate.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "tween.inc"

.ifndef TWEEN_IMPLEMENTATION_INCLUDED
TWEEN_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

TWEEN_STARTL:          .res 1
TWEEN_STARTH:          .res 1
TWEEN_ENDL:            .res 1
TWEEN_ENDH:            .res 1
TWEEN_DURATION:        .res 1
TWEEN_FRAME:           .res 1
TWEEN_MODE:            .res 1
TWEEN_VALUEL:          .res 1
TWEEN_VALUEH:          .res 1
TWEEN_DONE:            .res 1
TWEEN_PROGRESS:        .res 1
TWEEN_EASE:            .res 1

TWEEN_DELTAL:          .res 1
TWEEN_DELTAH:          .res 1
TWEEN_SIGN:            .res 1
TWEEN_NUML:            .res 1
TWEEN_NUMH:            .res 1
TWEEN_PROD0:           .res 1
TWEEN_PROD1:           .res 1
TWEEN_PROD2:           .res 1
TWEEN_TMP:             .res 1

      .segment "CODE"

      .export tween_begin
      .export tween_eval
      .export tween_eval_linear
      .export tween_eval_ease_in
      .export tween_eval_ease_out
      .export tween_eval_ease_in_out
      .export tween_step
      .export tween_step_linear
      .export tween_step_ease_in
      .export tween_step_ease_out
      .export tween_step_ease_in_out

; Reset frame state and publish the start value.
; @label TWEEN.BEGIN
; @kind routine
; @symbol tween_begin
; @summary Reset TWEEN.FRAME/DONE and set TWEEN.VALUE to TWEEN.START.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION
; @out TWEEN_FRAME Reset to 0.
; @out TWEEN_DONE Cleared to 0.
; @out TWEEN_VALUEL Low byte of TWEEN.START.
; @out TWEEN_VALUEH High byte of TWEEN.START.
tween_begin:
      STZ   TWEEN_FRAME
      STZ   TWEEN_DONE
      LDA   TWEEN_STARTL
      STA   TWEEN_VALUEL
      LDA   TWEEN_STARTH
      STA   TWEEN_VALUEH
      RTS

; Evaluate using TWEEN.MODE.
; @label TWEEN.EVAL
; @kind routine
; @symbol tween_eval
; @summary Evaluate the current tween frame using TWEEN.MODE.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME TWEEN_MODE
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_eval:
      LDA   TWEEN_MODE
      CMP   #TWEEN_MODE_EASE_IN
      BNE   @check_out
      JMP   tween_eval_ease_in
@check_out:
      CMP   #TWEEN_MODE_EASE_OUT
      BNE   @check_in_out
      JMP   tween_eval_ease_out
@check_in_out:
      CMP   #TWEEN_MODE_EASE_IN_OUT
      BNE   @linear
      JMP   tween_eval_ease_in_out
@linear:
      JMP   tween_eval_linear

; Evaluate with linear interpolation.
; @label TWEEN.EVAL_LINEAR
; @kind routine
; @symbol tween_eval_linear
; @summary Evaluate the current tween frame with a linear curve.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_eval_linear:
      LDA   #TWEEN_MODE_LINEAR
      STA   TWEEN_MODE
      JSR   tween_compute_progress
      LDA   TWEEN_DONE
      BEQ   @not_done
      JMP   tween_copy_end_to_value
@not_done:
      LDA   TWEEN_PROGRESS
      STA   TWEEN_EASE
      JMP   tween_apply_ease

; Evaluate with a quadratic ease-in curve.
; @label TWEEN.EVAL_EASE_IN
; @kind routine
; @symbol tween_eval_ease_in
; @summary Evaluate the current tween frame with slow-start dampening.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_eval_ease_in:
      LDA   #TWEEN_MODE_EASE_IN
      STA   TWEEN_MODE
      JSR   tween_compute_progress
      LDA   TWEEN_DONE
      BEQ   @not_done
      JMP   tween_copy_end_to_value
@not_done:
      LDA   TWEEN_PROGRESS
      JSR   tween_square_shift8
      STA   TWEEN_EASE
      JMP   tween_apply_ease

; Evaluate with a quadratic ease-out curve.
; @label TWEEN.EVAL_EASE_OUT
; @kind routine
; @symbol tween_eval_ease_out
; @summary Evaluate the current tween frame with slow-end dampening.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_eval_ease_out:
      LDA   #TWEEN_MODE_EASE_OUT
      STA   TWEEN_MODE
      JSR   tween_compute_progress
      LDA   TWEEN_DONE
      BEQ   @not_done
      JMP   tween_copy_end_to_value
@not_done:
      LDA   TWEEN_PROGRESS
      BEQ   @zero
      LDA   #$FF
      SEC
      SBC   TWEEN_PROGRESS
      JSR   tween_square_shift8
      STA   TWEEN_TMP
      LDA   #$FF
      SEC
      SBC   TWEEN_TMP
      STA   TWEEN_EASE
      JMP   tween_apply_ease
@zero:
      STZ   TWEEN_EASE
      JMP   tween_apply_ease

; Evaluate with a quadratic ease-in/ease-out curve.
; @label TWEEN.EVAL_EASE_IN_OUT
; @kind routine
; @symbol tween_eval_ease_in_out
; @summary Evaluate the current tween frame with dampening at both ends.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_eval_ease_in_out:
      LDA   #TWEEN_MODE_EASE_IN_OUT
      STA   TWEEN_MODE
      JSR   tween_compute_progress
      LDA   TWEEN_DONE
      BEQ   @not_done
      JMP   tween_copy_end_to_value
@not_done:
      LDA   TWEEN_PROGRESS
      CMP   #$80
      BCS   @ease_out_half
      JSR   tween_square_shift7
      STA   TWEEN_EASE
      JMP   tween_apply_ease
@ease_out_half:
      LDA   #$FF
      SEC
      SBC   TWEEN_PROGRESS
      JSR   tween_square_shift7
      STA   TWEEN_TMP
      LDA   #$FF
      SEC
      SBC   TWEEN_TMP
      STA   TWEEN_EASE
      JMP   tween_apply_ease

; Advance one frame and evaluate using TWEEN.MODE.
; @label TWEEN.STEP
; @kind routine
; @symbol tween_step
; @summary Increment TWEEN.FRAME by one, then evaluate using TWEEN.MODE.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME TWEEN_MODE
; @out TWEEN_FRAME Advanced frame value.
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_step:
      JSR   tween_advance_frame
      JMP   tween_eval

; Advance one frame and evaluate linearly.
; @label TWEEN.STEP_LINEAR
; @kind routine
; @symbol tween_step_linear
; @summary Increment TWEEN.FRAME by one, then evaluate with a linear curve.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_FRAME Advanced frame value.
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_step_linear:
      LDA   #TWEEN_MODE_LINEAR
      STA   TWEEN_MODE
      BRA   tween_step

; Advance one frame and evaluate with ease-in.
; @label TWEEN.STEP_EASE_IN
; @kind routine
; @symbol tween_step_ease_in
; @summary Increment TWEEN.FRAME by one, then evaluate with slow-start dampening.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_FRAME Advanced frame value.
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_step_ease_in:
      LDA   #TWEEN_MODE_EASE_IN
      STA   TWEEN_MODE
      BRA   tween_step

; Advance one frame and evaluate with ease-out.
; @label TWEEN.STEP_EASE_OUT
; @kind routine
; @symbol tween_step_ease_out
; @summary Increment TWEEN.FRAME by one, then evaluate with slow-end dampening.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_FRAME Advanced frame value.
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_step_ease_out:
      LDA   #TWEEN_MODE_EASE_OUT
      STA   TWEEN_MODE
      BRA   tween_step

; Advance one frame and evaluate with ease-in/ease-out.
; @label TWEEN.STEP_EASE_IN_OUT
; @kind routine
; @symbol tween_step_ease_in_out
; @summary Increment TWEEN.FRAME by one, then evaluate with dampening at both ends.
; @requires TWEEN_STARTL TWEEN_STARTH TWEEN_ENDL TWEEN_ENDH TWEEN_DURATION TWEEN_FRAME
; @out TWEEN_FRAME Advanced frame value.
; @out TWEEN_VALUEL Low byte of computed value.
; @out TWEEN_VALUEH High byte of computed value.
; @out TWEEN_DONE Non-zero when the end value has been reached.
; @out TWEEN_PROGRESS Normalized progress, 0..255.
; @out TWEEN_EASE Eased progress, 0..255.
tween_step_ease_in_out:
      LDA   #TWEEN_MODE_EASE_IN_OUT
      STA   TWEEN_MODE
      BRA   tween_step

tween_advance_frame:
      LDA   TWEEN_DONE
      BNE   @done
      LDA   TWEEN_FRAME
      CMP   TWEEN_DURATION
      BCS   @done
      INC   TWEEN_FRAME
@done:
      RTS

tween_compute_progress:
      STZ   TWEEN_DONE
      LDA   TWEEN_DURATION
      BEQ   @snap
      LDA   TWEEN_FRAME
      CMP   TWEEN_DURATION
      BCS   @snap

      STA   TWEEN_NUMH
      STZ   TWEEN_NUML
      SEC
      LDA   TWEEN_NUML
      SBC   TWEEN_FRAME
      STA   TWEEN_NUML
      LDA   TWEEN_NUMH
      SBC   #$00
      STA   TWEEN_NUMH
      STZ   TWEEN_PROGRESS

@div_loop:
      LDA   TWEEN_NUMH
      BNE   @subtract
      LDA   TWEEN_NUML
      CMP   TWEEN_DURATION
      BCC   @done
@subtract:
      SEC
      LDA   TWEEN_NUML
      SBC   TWEEN_DURATION
      STA   TWEEN_NUML
      LDA   TWEEN_NUMH
      SBC   #$00
      STA   TWEEN_NUMH
      INC   TWEEN_PROGRESS
      BRA   @div_loop

@snap:
      LDA   #$01
      STA   TWEEN_DONE
      LDA   #$FF
      STA   TWEEN_PROGRESS
@done:
      RTS

tween_copy_end_to_value:
      LDA   #$01
      STA   TWEEN_DONE
      LDA   TWEEN_ENDL
      STA   TWEEN_VALUEL
      LDA   TWEEN_ENDH
      STA   TWEEN_VALUEH
      RTS

tween_square_shift7:
      JSR   tween_square16
      LDX   #$07
@shift_loop:
      LSR   TWEEN_PROD1
      ROR   TWEEN_PROD0
      DEX
      BNE   @shift_loop
      LDA   TWEEN_PROD0
      RTS

tween_square_shift8:
      JSR   tween_square16
      LDA   TWEEN_PROD1
      RTS

tween_square16:
      STA   TWEEN_TMP
      STZ   TWEEN_PROD0
      STZ   TWEEN_PROD1
      LDX   TWEEN_TMP
      BEQ   @done
@mul:
      CLC
      LDA   TWEEN_PROD0
      ADC   TWEEN_TMP
      STA   TWEEN_PROD0
      LDA   TWEEN_PROD1
      ADC   #$00
      STA   TWEEN_PROD1
      DEX
      BNE   @mul
@done:
      RTS

tween_apply_ease:
      LDA   TWEEN_ENDL
      SEC
      SBC   TWEEN_STARTL
      STA   TWEEN_DELTAL
      LDA   TWEEN_ENDH
      SBC   TWEEN_STARTH
      STA   TWEEN_DELTAH
      BCS   @positive

      LDA   #$01
      STA   TWEEN_SIGN
      LDA   TWEEN_STARTL
      SEC
      SBC   TWEEN_ENDL
      STA   TWEEN_DELTAL
      LDA   TWEEN_STARTH
      SBC   TWEEN_ENDH
      STA   TWEEN_DELTAH
      BRA   @multiply

@positive:
      STZ   TWEEN_SIGN

@multiply:
      LDA   #$80
      STA   TWEEN_PROD0
      STZ   TWEEN_PROD1
      STZ   TWEEN_PROD2
      LDX   TWEEN_EASE
      BEQ   @apply
@mul:
      CLC
      LDA   TWEEN_PROD0
      ADC   TWEEN_DELTAL
      STA   TWEEN_PROD0
      LDA   TWEEN_PROD1
      ADC   TWEEN_DELTAH
      STA   TWEEN_PROD1
      LDA   TWEEN_PROD2
      ADC   #$00
      STA   TWEEN_PROD2
      DEX
      BNE   @mul

@apply:
      LDA   TWEEN_SIGN
      BNE   @subtract
      CLC
      LDA   TWEEN_STARTL
      ADC   TWEEN_PROD1
      STA   TWEEN_VALUEL
      LDA   TWEEN_STARTH
      ADC   TWEEN_PROD2
      STA   TWEEN_VALUEH
      RTS

@subtract:
      SEC
      LDA   TWEEN_STARTL
      SBC   TWEEN_PROD1
      STA   TWEEN_VALUEL
      LDA   TWEEN_STARTH
      SBC   TWEEN_PROD2
      STA   TWEEN_VALUEH
      RTS

.endif
