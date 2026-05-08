; Shared Nova tween/easing declarations.
;
; The tween state is ordinary BSS so callers can use Nova pseudo registers as
; scratch without this library owning them. Values are 16-bit, frame counts are
; 8-bit, and duration 0 means "snap to end".
;
; This ABI is deliberately register-shaped: software writes start/end/duration/
; frame/mode, evaluates, then reads value/done/progress/ease. A future math
; coprocessor can implement the same contract behind TWEEN.EVAL without forcing
; games or runtimes to change their animation code.

.ifndef TWEEN_INC_INCLUDED
TWEEN_INC_INCLUDED = 1

; @label TWEEN.MODE_LINEAR
; @kind const
; @symbol TWEEN_MODE_LINEAR
; @summary Constant for linear interpolation.
TWEEN_MODE_LINEAR      = 0
; @label TWEEN.MODE_EASE_IN
; @kind const
; @symbol TWEEN_MODE_EASE_IN
; @summary Constant for quadratic ease-in interpolation.
TWEEN_MODE_EASE_IN     = 1
; @label TWEEN.MODE_EASE_OUT
; @kind const
; @symbol TWEEN_MODE_EASE_OUT
; @summary Constant for quadratic ease-out interpolation.
TWEEN_MODE_EASE_OUT    = 2
; @label TWEEN.MODE_EASE_IN_OUT
; @kind const
; @symbol TWEEN_MODE_EASE_IN_OUT
; @summary Constant for quadratic ease-in/ease-out interpolation.
TWEEN_MODE_EASE_IN_OUT = 3

; @label TWEEN.STARTL
; @kind u8
; @symbol TWEEN_STARTL
; @summary Low byte of the tween start value.
.global TWEEN_STARTL
; @label TWEEN.STARTH
; @kind u8
; @symbol TWEEN_STARTH
; @summary High byte of the tween start value.
.global TWEEN_STARTH
; @label TWEEN.ENDL
; @kind u8
; @symbol TWEEN_ENDL
; @summary Low byte of the tween end value.
.global TWEEN_ENDL
; @label TWEEN.ENDH
; @kind u8
; @symbol TWEEN_ENDH
; @summary High byte of the tween end value.
.global TWEEN_ENDH
; @label TWEEN.DURATION
; @kind u8
; @symbol TWEEN_DURATION
; @summary Tween length in frames. Zero snaps to the end value.
.global TWEEN_DURATION
; @label TWEEN.FRAME
; @kind u8
; @symbol TWEEN_FRAME
; @summary Current tween frame, from 0 through TWEEN.DURATION.
.global TWEEN_FRAME
; @label TWEEN.MODE
; @kind u8
; @symbol TWEEN_MODE
; @summary Easing mode: 0 linear, 1 ease-in, 2 ease-out, 3 ease-in-out.
.global TWEEN_MODE
; @label TWEEN.VALUEL
; @kind u8
; @symbol TWEEN_VALUEL
; @summary Low byte of the last tween result.
.global TWEEN_VALUEL
; @label TWEEN.VALUEH
; @kind u8
; @symbol TWEEN_VALUEH
; @summary High byte of the last tween result.
.global TWEEN_VALUEH
; @label TWEEN.DONE
; @kind u8
; @symbol TWEEN_DONE
; @summary Non-zero when the tween has reached its end value.
.global TWEEN_DONE
; @label TWEEN.PROGRESS
; @kind u8
; @symbol TWEEN_PROGRESS
; @summary Last normalized progress value, 0..255.
.global TWEEN_PROGRESS
; @label TWEEN.EASE
; @kind u8
; @symbol TWEEN_EASE
; @summary Last eased progress value, 0..255.
.global TWEEN_EASE

.global tween_begin
.global tween_eval
.global tween_eval_linear
.global tween_eval_ease_in
.global tween_eval_ease_out
.global tween_eval_ease_in_out
.global tween_step
.global tween_step_linear
.global tween_step_ease_in
.global tween_step_ease_out
.global tween_step_ease_in_out

.endif
