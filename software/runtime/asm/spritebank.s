; Nova sprite-bank (NSPR) loader. See spritebank.inc + docs/sprite-editor/NSPR.md.

.include "spritebank.inc"

.ifndef SPRITEBANK_IMPL_INCLUDED
SPRITEBANK_IMPL_INCLUDED = 1

      .segment "ZEROPAGE"
spritebank_src:   .res 2             ; ZP so (spritebank_src),Y indirection works

      .segment "BSS"
spritebank_result:      .res 1
spritebank_shape_count: .res 1
spritebank_char_count:  .res 1
spritebank_flags:       .res 1
spritebank_shapes_ptr:  .res 2
spritebank_chars_ptr:   .res 2
sb_mul_lo:              .res 1        ; scratch: shape_count*128, low byte
sb_mul_hi:              .res 1        ; scratch: shape_count*128, high byte

      .segment "CODE"

      .export spritebank_open
      .export spritebank_result
      .export spritebank_shape_count
      .export spritebank_char_count
      .export spritebank_flags
      .export spritebank_shapes_ptr
      .export spritebank_chars_ptr

; Validate the NSPR header at (spritebank_src) and index it. Sets
; spritebank_result; on OK also the count/flag/pointer fields.
; @label SPRITEBANK.OPEN
; @kind routine
; @symbol spritebank_open
; @summary Parse + index an in-memory NSPR sprite bank.
spritebank_open:
      ; --- magic "NSPR" ---
      LDY   #0
      LDA   (spritebank_src),Y
      CMP   #'N'
      BNE   @bad_magic
      INY
      LDA   (spritebank_src),Y
      CMP   #'S'
      BNE   @bad_magic
      INY
      LDA   (spritebank_src),Y
      CMP   #'P'
      BNE   @bad_magic
      INY
      LDA   (spritebank_src),Y
      CMP   #'R'
      BNE   @bad_magic

      ; --- version ---
      LDY   #4
      LDA   (spritebank_src),Y
      CMP   #SPRITEBANK_VERSION
      BNE   @bad_version

      ; --- flags(6), shape_count(7), char_count(8) ---
      LDY   #6
      LDA   (spritebank_src),Y
      STA   spritebank_flags
      LDY   #7
      LDA   (spritebank_src),Y
      STA   spritebank_shape_count
      LDY   #8
      LDA   (spritebank_src),Y
      STA   spritebank_char_count

      ; --- shapes_ptr = src + 16 (+48 if palette) ---
      CLC
      LDA   spritebank_src
      ADC   #16
      STA   spritebank_shapes_ptr
      LDA   spritebank_src+1
      ADC   #0
      STA   spritebank_shapes_ptr+1
      LDA   spritebank_flags
      AND   #$01
      BEQ   @no_palette
      CLC
      LDA   spritebank_shapes_ptr
      ADC   #48
      STA   spritebank_shapes_ptr
      LDA   spritebank_shapes_ptr+1
      ADC   #0
      STA   spritebank_shapes_ptr+1
@no_palette:

      ; --- chars_ptr = shapes_ptr + shape_count*128 ---
      ; shape_count*128 = (count>>1):((count&1)<<7)
      LDA   spritebank_shape_count
      LSR   A
      STA   sb_mul_hi
      LDA   #0
      ROR   A
      STA   sb_mul_lo
      CLC
      LDA   spritebank_shapes_ptr
      ADC   sb_mul_lo
      STA   spritebank_chars_ptr
      LDA   spritebank_shapes_ptr+1
      ADC   sb_mul_hi
      STA   spritebank_chars_ptr+1

      LDA   #SPRITEBANK_OK
      STA   spritebank_result
      RTS

@bad_magic:
      LDA   #SPRITEBANK_ERR_MAGIC
      STA   spritebank_result
      RTS
@bad_version:
      LDA   #SPRITEBANK_ERR_VERSION
      STA   spritebank_result
      RTS

.endif
