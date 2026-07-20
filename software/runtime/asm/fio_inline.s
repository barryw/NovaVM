; =====================================================================
;  Nova NDK — fio_inline.s
;
;  Dense, inline-parameter filename setup for static assembly call sites.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "fio.inc"

.ifndef FIO_INLINE_IMPLEMENTATION_INCLUDED
FIO_INLINE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export i_fio_name

; @label I_FIO_NAME
; @kind routine
; @symbol i_fio_name
; @library fio
; @abi inline
; @summary Copy the zero-terminated filename immediately after the JSR into FIO.NAME.
; @in Inline: Filename bytes followed by zero; length must be 1..63.
; @out A: 0 on success, 1 on invalid name.
; @note The saved return address is advanced past the terminating zero.
i_fio_name:
      TSX
      CLC
      LDA   $0101,X
      ADC   #$01
      STA   FIO_ARG_NAMEPTR_L
      LDA   $0102,X
      ADC   #$00
      STA   FIO_ARG_NAMEPTR_H

      LDY   #$00
@length:
      LDA   (FIO_ARG_NAMEPTR_L),Y
      BEQ   @copy
      INY
      BNE   @length

@copy:
      STY   FIO_ARG_NAMELEN
      TYA
      CLC
      ADC   FIO_ARG_NAMEPTR_L
      STA   $0101,X
      LDA   FIO_ARG_NAMEPTR_H
      ADC   #$00
      STA   $0102,X
      JMP   fio_copy_name

.endif
