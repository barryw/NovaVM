; =====================================================================
; NovaPascal resident shell and compiler ROM ($C000-$FFFF).
; NPEDIT, NAS, and NL are standard binaries loaded into the shared tool slot.
; =====================================================================

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libabi.inc"
      .include "libfiles.inc"
      .include "fio.inc"
      .include "replline.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "tools/npproj.inc"

      .import npc_compile_file

SHELL_BUFFER_CAP = $1000
OUTPUT_BASE      = $7000

      .segment "ZEROPAGE"
p_src:       .res 2
p_left:      .res 2
p_word:      .res 2
p_out_left:  .res 2
p_copy_dst:  .res 2
p_expected:  .res 1

      .segment "BSS"
source_buf: .res SHELL_BUFFER_CAP

      .segment "CODE"

cold_start:
      CLD
      LDX   #$FF
      TXS
      LDA   #ROMSWAP_PRIMARY
      STA   LIB_HOME_BANK
      STZ   LIB_RESIDENT
      LDA   #VCMD_GCLS
      STA   VGC_CMD
      JMP   shell_start

print_z:
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @done
      STA   VGC_CHAROUT
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@done:
      RTS

      .include "shell.s"

      .segment "RODATA"
hex_digits: .byte "0123456789ABCDEF"

      .segment "MONITOR"
      .org $FFD7
reset_vector:
      CLD
      LDX   #$FF
      TXS
      JMP   cold_start
nmi_stub:
      RTI
irq_stub:
      RTI

      .segment "VECTORS"
      .org $FFFA
      .word nmi_stub
      .word reset_vector
      .word irq_stub
