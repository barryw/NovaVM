; Nova sprite-bank (NSPR) loader. See spritebank.inc + docs/sprite-editor/NSPR.md.

.include "spritebank.inc"
.include "xram.inc"

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
spritebank_shapes_len:  .res 2        ; shape_count * 128
spritebank_chars_ptr:   .res 2
spritebank_xram_base:   .res 3        ; flat 24-bit XRAM destination for the pool

      .segment "CODE"

      .export spritebank_open
      .export spritebank_load_shapes
      .export spritebank_result
      .export spritebank_shape_count
      .export spritebank_char_count
      .export spritebank_flags
      .export spritebank_shapes_ptr
      .export spritebank_shapes_len
      .export spritebank_chars_ptr
      .export spritebank_xram_base

; Validate the NSPR header at (spritebank_src) and index it. Sets
; spritebank_result; on OK also the count/flag/pointer/length fields.
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

      ; --- shapes_len = shape_count * 128 = (count>>1):((count&1)<<7) ---
      LDA   spritebank_shape_count
      LSR   A
      STA   spritebank_shapes_len+1
      LDA   #0
      ROR   A
      STA   spritebank_shapes_len+0

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

      ; --- chars_ptr = shapes_ptr + shapes_len ---
      CLC
      LDA   spritebank_shapes_ptr
      ADC   spritebank_shapes_len+0
      STA   spritebank_chars_ptr
      LDA   spritebank_shapes_ptr+1
      ADC   spritebank_shapes_len+1
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

; DMA the shape pool (spritebank_shapes_ptr, spritebank_shapes_len) from CPU RAM
; to XRAM at spritebank_xram_base (caller sets it first). Call after
; spritebank_open. Shape i then lives at xram_base + i*128, ready for
; anim_load_xram_shapes.
; @label SPRITEBANK.LOAD_SHAPES
; @kind routine
; @symbol spritebank_load_shapes
; @summary Stream the NSPR shape pool into XRAM.
spritebank_load_shapes:
      LDA   spritebank_shapes_ptr
      STA   XRAM_RAML
      LDA   spritebank_shapes_ptr+1
      STA   XRAM_RAMH
      LDA   spritebank_xram_base+0
      STA   XRAM_ADDRL
      LDA   spritebank_xram_base+1
      STA   XRAM_ADDRM
      LDA   spritebank_xram_base+2
      STA   XRAM_ADDRH
      LDA   spritebank_shapes_len+0
      STA   XRAM_LENL
      LDA   spritebank_shapes_len+1
      STA   XRAM_LENH
      JMP   xram_copy_from_ram         ; tail-call; its RTS returns to our caller

.endif
