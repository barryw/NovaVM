; Nova sprite-bank (NSPR) loader. See spritebank.inc + docs/sprite-editor/NSPR.md.

.include "spritebank.inc"
.include "xram.inc"

.ifndef SPRITEBANK_IMPL_INCLUDED
SPRITEBANK_IMPL_INCLUDED = 1

      .segment "ZEROPAGE"
spritebank_src:   .res 2             ; ZP so (spritebank_src),Y indirection works
spritebank_walk:  .res 2             ; ZP scratch pointer for table walking

      .segment "BSS"
spritebank_result:      .res 1
spritebank_shape_count: .res 1
spritebank_char_count:  .res 1
spritebank_flags:       .res 1
spritebank_shapes_ptr:  .res 2
spritebank_shapes_len:  .res 2        ; shape_count * 128
spritebank_chars_ptr:   .res 2
spritebank_xram_base:   .res 3        ; flat 24-bit XRAM destination for the pool

spritebank_char_ptr:        .res 2
spritebank_char_part_count: .res 1
spritebank_char_parts_ptr:  .res 2
spritebank_char_anim_count: .res 1
spritebank_char_anims_ptr:  .res 2
sb_seek_index:  .res 1
sb_tmp_parts:   .res 1
sb_tmp_anims:   .res 1
sb_tmp_frames:  .res 1

      .segment "CODE"

      .export spritebank_open
      .export spritebank_load_shapes
      .export spritebank_char_seek
      .export spritebank_char_ptr
      .export spritebank_char_part_count
      .export spritebank_char_parts_ptr
      .export spritebank_char_anim_count
      .export spritebank_char_anims_ptr
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

; Walk the character table to character A and fill spritebank_char_*.
; @label SPRITEBANK.CHAR_SEEK
; @kind routine
; @symbol spritebank_char_seek
; @summary Locate character A and expose its parts/animations.
; @in A: character index (0-based).
spritebank_char_seek:
      STA   sb_seek_index
      LDA   spritebank_chars_ptr
      STA   spritebank_walk
      LDA   spritebank_chars_ptr+1
      STA   spritebank_walk+1
@skip_chars:                            ; count in memory: sb_skip_char clobbers X
      LDA   sb_seek_index
      BEQ   @found
      JSR   sb_skip_char
      DEC   sb_seek_index
      BRA   @skip_chars
@found:
      LDA   spritebank_walk            ; record start
      STA   spritebank_char_ptr
      LDA   spritebank_walk+1
      STA   spritebank_char_ptr+1
      LDA   #8                         ; past name -> part_count
      JSR   sb_advance
      LDY   #0
      LDA   (spritebank_walk),Y
      STA   spritebank_char_part_count
      LDA   #1                         ; -> parts block
      JSR   sb_advance
      LDA   spritebank_walk
      STA   spritebank_char_parts_ptr
      LDA   spritebank_walk+1
      STA   spritebank_char_parts_ptr+1
      LDX   spritebank_char_part_count ; skip parts (part_count*3)
@skip_parts:
      BEQ   @at_anims
      LDA   #3
      JSR   sb_advance
      DEX
      BRA   @skip_parts
@at_anims:
      LDY   #0
      LDA   (spritebank_walk),Y
      STA   spritebank_char_anim_count
      LDA   #1                         ; -> animations block
      JSR   sb_advance
      LDA   spritebank_walk
      STA   spritebank_char_anims_ptr
      LDA   spritebank_walk+1
      STA   spritebank_char_anims_ptr+1
      RTS

; Advance spritebank_walk past one whole character record (name + parts +
; animations, walking each variable-length section by loop).
sb_skip_char:
      LDA   #8
      JSR   sb_advance                 ; name
      LDY   #0
      LDA   (spritebank_walk),Y        ; part_count
      STA   sb_tmp_parts
      LDA   #1
      JSR   sb_advance
      LDX   sb_tmp_parts               ; skip parts (part_count*3)
@parts:
      BEQ   @after_parts
      LDA   #3
      JSR   sb_advance
      DEX
      BRA   @parts
@after_parts:
      LDY   #0
      LDA   (spritebank_walk),Y        ; anim_count
      STA   sb_tmp_anims
      LDA   #1
      JSR   sb_advance
@anims:
      LDA   sb_tmp_anims
      BEQ   @done
      LDA   #8
      JSR   sb_advance                 ; anim name
      LDY   #0
      LDA   (spritebank_walk),Y        ; frame_count
      STA   sb_tmp_frames
      LDA   #3                         ; past frame_count + ticks + flags
      JSR   sb_advance
      LDX   sb_tmp_frames              ; skip frames (frame_count*part_count)
@frames:
      BEQ   @after_frames
      LDA   sb_tmp_parts
      JSR   sb_advance
      DEX
      BRA   @frames
@after_frames:
      DEC   sb_tmp_anims
      BRA   @anims
@done:
      RTS

; spritebank_walk += A (unsigned 8-bit).
sb_advance:
      CLC
      ADC   spritebank_walk
      STA   spritebank_walk
      BCC   @done
      INC   spritebank_walk+1
@done:
      RTS

.endif
