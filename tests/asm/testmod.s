; testmod.s — minimal paged library for loader proofs. id=$7F, 2 functions.
      .include "libabi.inc"
      .include "libmod.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_TEST, LIB_ABI_VERSION, 2

dispatch:
      lda     LIB_FN_ID
      cmp     #2
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

; FN 0 ECHO: RESULT = ARG0 (32-bit copy)
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

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
