; =====================================================================
; NovaPascal resident shell and compiler ROM ($C000-$FFFF).
; NPEDIT, NAS, and NL are standard binaries loaded into the shared tool slot.
; =====================================================================

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libabi.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "fio.inc"
      .include "replline.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "nbuild.inc"
      .include "tools/npproj.inc"
      .include "tools/npc_frontend.inc"

      .import npc_compile_file
      .import nui_drain_keys
      .export source_buf

SHELL_BUFFER_CAP = $1000
OUTPUT_BASE      = $7000

      .segment "ZEROPAGE"
p_src:       .res 2
p_left:      .res 2
p_word:      .res 2
p_out_left:  .res 2
p_copy_dst:  .res 2
p_expected:  .res 1

      .segment "SOURCEBUF"
; NPC owns the final 512 bytes while its frontend is active. Shell file
; commands still see one contiguous 4 KiB buffer when the compiler is idle.
source_buf: .res NPCFE_WORK_OFFSET

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

; Print the NUL-terminated string at A/Y. print_z already clobbers A and Y, so
; no caller of this shorthand can have depended on either surviving.
print_ay:
      STA   p_word
      STY   p_word+1
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
