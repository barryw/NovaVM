; Shared two-window XRAM stream. Callers provide the conventional
; input_base/input_pos/input_len/input_addr and output_addr/output_len/output_cap
; state. Window 2 stays on input while window 3 stays on output.

.ifndef XRAMSTREAM_INC
XRAMSTREAM_INC = 1

.macro xram_stream_impl error_byte, output_error, backing_mode
input_at_end:
.ifnblank backing_mode
      LDA   backing_mode
      BEQ   @xram
      JMP   large_input_at_end
@xram:
.endif
      LDA   input_pos+1
      CMP   input_len+1
      BCC   @no
      BNE   @yes
      LDA   input_pos
      CMP   input_len
      BCC   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

map_input:
.ifnblank backing_mode
      LDA   backing_mode
      BEQ   @xram
      JMP   large_map_input
@xram:
.endif
      CLC
      LDA   input_base
      ADC   input_pos
      STA   input_addr
      LDA   input_base+1
      ADC   input_pos+1
      STA   input_addr+1
      LDA   input_base+2
      ADC   #0
      STA   input_addr+2
      STZ   XMC_W2AL
      LDA   input_addr+1
      STA   XMC_W2AM
      LDA   input_addr+2
      STA   XMC_W2AH
      RTS

read_input_byte:
.ifnblank backing_mode
      LDA   backing_mode
      BEQ   @xram
      JMP   large_read_input_byte
@xram:
.endif
      LDX   input_addr
      LDA   XRAM_WIN2_BASE,X
      RTS

increment_input:
.ifnblank backing_mode
      LDA   backing_mode
      BEQ   @xram
      JMP   large_increment_input
@xram:
.endif
      INC   input_addr
      BNE   @position
      INC   input_addr+1
      LDA   input_addr+1
      STA   XMC_W2AM
      BNE   @position
      INC   input_addr+2
      LDA   input_addr+2
      STA   XMC_W2AH
@position:
      INC   input_pos
      BNE   @done
      INC   input_pos+1
@done:
      RTS

map_output:
.ifnblank backing_mode
      LDA   backing_mode
      BEQ   @xram
      RTS
@xram:
.endif
      LDA   output_addr+1
      STA   WIN3_MI
      LDA   output_addr+2
      STA   WIN3_HI
      RTS

emit_byte:
.ifnblank backing_mode
      STA   byte_value
      LDA   backing_mode
      BEQ   @xram
      LDA   byte_value
      JMP   large_emit_byte
@xram:
      LDA   byte_value
.endif
      STA   byte_value
      LDA   output_len+1
      CMP   output_cap+1
      BCC   @write
      BNE   @full
      LDA   output_len
      CMP   output_cap
      BCS   @full
@write:
      LDX   output_addr
      LDA   byte_value
      STA   WIN3_BASE,X
      INC   output_addr
      BNE   @length
      INC   output_addr+1
      LDA   output_addr+1
      STA   WIN3_MI
      BNE   @length
      INC   output_addr+2
      LDA   output_addr+2
      STA   WIN3_HI
@length:
      INC   output_len
      BNE   @ok
      INC   output_len+1
@ok:
      LDA   #0
      RTS
@full:
      LDA   #output_error
      STA   error_byte
      LDA   #1
      RTS
.endmacro

.endif
