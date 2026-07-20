; NAS disk-loaded opcode-table backend. Input is an uppercase three-character
; mnemonic plus an addressing-mode id in the fixed NASBE mailbox.

      .setcpu "w65c02"
      .include "nas_backend.inc"

.ifdef NASBE_INTEGRATED
      .include "nas_core.inc"
table_ptr = NASCORE_ZP_BASE+19
.else

      .segment "ZEROPAGE"
table_ptr: .res 2
.endif

      .segment "CODE"
      .export nasbe_main

nasbe_main:
      LDA   NASBE_MODE
      CMP   #NASBE_MODE_COUNT
      BCS   @missing
      TAX
      LDA   table_l,X
      STA   table_ptr
      LDA   table_h,X
      STA   table_ptr+1
@entry:
      LDY   #0
      LDA   (table_ptr),Y
      BEQ   @missing
      CMP   NASBE_MNEMONIC
      BNE   @next
      INY
      LDA   (table_ptr),Y
      CMP   NASBE_MNEMONIC+1
      BNE   @next
      INY
      LDA   (table_ptr),Y
      CMP   NASBE_MNEMONIC+2
      BNE   @next
      INY
      LDA   (table_ptr),Y
      SEC
      RTS
@next:
      CLC
      LDA   table_ptr
      ADC   #4
      STA   table_ptr
      BCC   @entry
      INC   table_ptr+1
      BRA   @entry
@missing:
      CLC
      RTS

      .segment "RODATA"
table_l:
      .lobytes implied_table, immediate_table, accumulator_table
      .lobytes zp_table, zpx_table, zpy_table, abs_table, absx_table, absy_table
      .lobytes indx_table, indy_table, zpind_table, absind_table, absindx_table
table_h:
      .hibytes implied_table, immediate_table, accumulator_table
      .hibytes zp_table, zpx_table, zpy_table, abs_table, absx_table, absy_table
      .hibytes indx_table, indy_table, zpind_table, absind_table, absindx_table

implied_table:
      .byte "BRK", $00, "CLC", $18, "CLD", $D8, "CLI", $58
      .byte "CLV", $B8, "DEA", $3A, "DEX", $CA, "DEY", $88
      .byte "INA", $1A, "INX", $E8, "INY", $C8, "NOP", $EA
      .byte "PHA", $48, "PHP", $08, "PHX", $DA, "PHY", $5A
      .byte "PLA", $68, "PLP", $28, "PLX", $FA, "PLY", $7A
      .byte "RTI", $40, "RTS", $60, "SEC", $38, "SED", $F8
      .byte "SEI", $78, "STP", $DB, "TAX", $AA, "TAY", $A8
      .byte "TSX", $BA, "TXA", $8A, "TXS", $9A, "TYA", $98
      .byte "WAI", $CB, 0

immediate_table:
      .byte "ADC", $69, "AND", $29, "BIT", $89, "CMP", $C9
      .byte "CPX", $E0, "CPY", $C0, "EOR", $49, "LDA", $A9
      .byte "LDX", $A2, "LDY", $A0, "ORA", $09, "SBC", $E9, 0

accumulator_table:
      .byte "ASL", $0A, "DEC", $3A, "INC", $1A
      .byte "LSR", $4A, "ROL", $2A, "ROR", $6A, 0

zp_table:
      .byte "ADC", $65, "AND", $25, "ASL", $06, "BIT", $24
      .byte "CMP", $C5, "CPX", $E4, "CPY", $C4, "DEC", $C6
      .byte "EOR", $45, "INC", $E6, "LDA", $A5, "LDX", $A6
      .byte "LDY", $A4, "LSR", $46, "ORA", $05, "ROL", $26
      .byte "ROR", $66, "SBC", $E5, "STA", $85, "STX", $86
      .byte "STY", $84, "STZ", $64, "TRB", $14, "TSB", $04, 0

zpx_table:
      .byte "ADC", $75, "AND", $35, "ASL", $16, "BIT", $34
      .byte "CMP", $D5, "DEC", $D6, "EOR", $55, "INC", $F6
      .byte "LDA", $B5, "LDY", $B4, "LSR", $56, "ORA", $15
      .byte "ROL", $36, "ROR", $76, "SBC", $F5, "STA", $95
      .byte "STY", $94, "STZ", $74, 0

zpy_table:
      .byte "LDX", $B6, "STX", $96, 0

abs_table:
      .byte "ADC", $6D, "AND", $2D, "ASL", $0E, "BIT", $2C
      .byte "CMP", $CD, "CPX", $EC, "CPY", $CC, "DEC", $CE
      .byte "EOR", $4D, "INC", $EE, "JMP", $4C, "JSR", $20
      .byte "LDA", $AD, "LDX", $AE, "LDY", $AC, "LSR", $4E
      .byte "ORA", $0D, "ROL", $2E, "ROR", $6E, "SBC", $ED
      .byte "STA", $8D, "STX", $8E, "STY", $8C, "STZ", $9C
      .byte "TRB", $1C, "TSB", $0C, 0

absx_table:
      .byte "ADC", $7D, "AND", $3D, "ASL", $1E, "BIT", $3C
      .byte "CMP", $DD, "DEC", $DE, "EOR", $5D, "INC", $FE
      .byte "LDA", $BD, "LDY", $BC, "LSR", $5E, "ORA", $1D
      .byte "ROL", $3E, "ROR", $7E, "SBC", $FD, "STA", $9D
      .byte "STZ", $9E, 0

absy_table:
      .byte "ADC", $79, "AND", $39, "CMP", $D9, "EOR", $59
      .byte "LDA", $B9, "LDX", $BE, "ORA", $19, "SBC", $F9
      .byte "STA", $99, 0

indx_table:
      .byte "ADC", $61, "AND", $21, "CMP", $C1, "EOR", $41
      .byte "LDA", $A1, "ORA", $01, "SBC", $E1, "STA", $81, 0

indy_table:
      .byte "ADC", $71, "AND", $31, "CMP", $D1, "EOR", $51
      .byte "LDA", $B1, "ORA", $11, "SBC", $F1, "STA", $91, 0

zpind_table:
      .byte "ADC", $72, "AND", $32, "CMP", $D2, "EOR", $52
      .byte "LDA", $B2, "ORA", $12, "SBC", $F2, "STA", $92, 0

absind_table:
      .byte "JMP", $6C, 0

absindx_table:
      .byte "JMP", $7C, 0
