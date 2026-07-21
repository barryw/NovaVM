; W65C02 long conditional branches.

.ifndef LONGBRANCH_INC
LONGBRANCH_INC = 1

.macro long_bcs target
      BCC   :+
      JMP   target
:
.endmacro

.macro long_bcc target
      BCS   :+
      JMP   target
:
.endmacro

.macro long_bne target
      BEQ   :+
      JMP   target
:
.endmacro

.macro long_beq target
      BNE   :+
      JMP   target
:
.endmacro

.endif
