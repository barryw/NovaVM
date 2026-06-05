; testmod.s — minimal paged library for loader proofs. id=$7F, 3 functions.
      .include "libabi.inc"
      .include "libmod.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_TEST, LIB_ABI_VERSION, 3

;@module TESTMOD
;@version 1.0
;@brief Minimal paged library for loader proofs (echo / add / sum-bytes).

dispatch:
      lda     LIB_FN_ID
      cmp     #3
      bcs     bad_fn
      asl
      tax
      lda     jtable+1,x
      pha
      lda     jtable,x
      pha
      rts
bad_fn:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

jtable:
      .word   fn_echo-1               ; FN 0: ECHO
      .word   fn_add-1                ; FN 1: ADD
      .word   fn_sum-1                ; FN 2: SUM

; FN 0 ECHO: RESULT = ARG0 (32-bit copy)
;@fn ECHO 0
;@brief Copy ARG0 to RESULT verbatim (32-bit).
;@arg value s32 the value to echo back
;@ret s32 the same value
;@status LERR_OK
fn_echo:
      ldx     #3
@c:   lda     LIB_ARG0,x
      sta     LIB_RESULT,x
      dex
      bpl     @c
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; FN 1 ADD: RESULT = ARG0 + ARG1 (32-bit LE)
; Y counts the bytes down (DEY touches only N/Z, never C) so the inter-byte
; carry survives to the next ADC; CPX would have cleared it.
;@fn ADD 1
;@brief 32-bit add: RESULT = ARG0 + ARG1.
;@arg a s32 first addend
;@arg b s32 second addend
;@ret s32 the 32-bit little-endian sum
;@status LERR_OK
fn_add:
      clc
      ldx     #0
      ldy     #4
@a:   lda     LIB_ARG0,x
      adc     LIB_ARG1,x
      sta     LIB_RESULT,x
      inx
      dey
      bne     @a
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; FN 2 SUM: ARG0 is BYTES(ptr16,len16) — LIB_ARG0/+1=ptr, LIB_ARG0+2/+3=len.
; RESULT (32-bit LE INT) = sum of those `len` main-RAM bytes. Walks (LIB_ZP),Y.
;@fn SUM 2
;@brief Sum a run of main-RAM bytes.
;@arg buf bytes ptr16 in ARG0 low word, len16 in ARG0 high word
;@ret s32 the 32-bit sum of the len bytes
;@effect Reads `len` bytes of main RAM via (LIB_ZP),Y.
;@status LERR_OK
; 16-bit length: LIB_SCRATCH/+1 is a down-counter; @loop exits the moment it hits
; zero (correct for len=0, <256, and >256). Each byte is folded into the 32-bit
; accumulator BEFORE the pointer/counter are touched, so the carry chain that
; ADC/INC mem would otherwise clobber stays self-contained per byte.
fn_sum:
      lda     #0                       ; RESULT = 0
      sta     LIB_RESULT
      sta     LIB_RESULT+1
      sta     LIB_RESULT+2
      sta     LIB_RESULT+3
      lda     LIB_ARG0                 ; ptr -> ZP pointer pair
      sta     LIB_ZP
      lda     LIB_ARG0+1
      sta     LIB_ZP+1
      lda     LIB_ARG0+2               ; len -> 16-bit down-counter
      sta     LIB_SCRATCH
      lda     LIB_ARG0+3
      sta     LIB_SCRATCH+1
      ldy     #0
@loop:
      lda     LIB_SCRATCH              ; remaining == 0 ?  (lo|hi)
      ora     LIB_SCRATCH+1
      beq     @done
      lda     (LIB_ZP),y               ; fetch byte, fold into 32-bit RESULT
      clc
      adc     LIB_RESULT
      sta     LIB_RESULT
      bcc     @adv                     ; no carry out of byte 0 → nothing to propagate
      inc     LIB_RESULT+1
      bne     @adv
      inc     LIB_RESULT+2
      bne     @adv
      inc     LIB_RESULT+3
@adv:
      inc     LIB_ZP                   ; advance source pointer (16-bit)
      bne     @dec
      inc     LIB_ZP+1
@dec:
      lda     LIB_SCRATCH              ; remaining -= 1 (16-bit)
      bne     @declo
      dec     LIB_SCRATCH+1
@declo:
      dec     LIB_SCRATCH
      jmp     @loop
@done:
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
