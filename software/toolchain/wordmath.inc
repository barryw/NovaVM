; Shared unsigned 16-bit arithmetic used by NAS expression evaluators.

.ifndef WORDMATH_INC
WORDMATH_INC = 1

.macro word_multiply value_l, value_h, rhs_l, rhs_h, acc_l, acc_h
      .local loop, shift
      STZ   acc_l
      STZ   acc_h
      LDX   #16
loop:
      LDA   rhs_l
      AND   #1
      BEQ   shift
      CLC
      LDA   acc_l
      ADC   value_l
      STA   acc_l
      LDA   acc_h
      ADC   value_h
      STA   acc_h
shift:
      ASL   value_l
      ROL   value_h
      LSR   rhs_h
      ROR   rhs_l
      DEX
      BNE   loop
      LDA   acc_l
      STA   value_l
      LDA   acc_h
      STA   value_h
.endmacro

.macro word_divide value_l, value_h, rhs_l, rhs_h, acc_l, acc_h, rem_l, rem_h, operator, modulo_opcode
      .local loop, next, subtract, remainder, done
      STZ   acc_l
      STZ   acc_h
      STZ   rem_l
      STZ   rem_h
      LDX   #16
loop:
      ASL   value_l
      ROL   value_h
      ROL   rem_l
      ROL   rem_h
      ASL   acc_l
      ROL   acc_h
      LDA   rem_h
      CMP   rhs_h
      BCC   next
      BNE   subtract
      LDA   rem_l
      CMP   rhs_l
      BCC   next
subtract:
      SEC
      LDA   rem_l
      SBC   rhs_l
      STA   rem_l
      LDA   rem_h
      SBC   rhs_h
      STA   rem_h
      INC   acc_l
next:
      DEX
      BNE   loop
      LDA   operator
      CMP   #modulo_opcode
      BEQ   remainder
      LDA   acc_l
      STA   value_l
      LDA   acc_h
      STA   value_h
      BRA   done
remainder:
      LDA   rem_l
      STA   value_l
      LDA   rem_h
      STA   value_h
done:
.endmacro

.endif
