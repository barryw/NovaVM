; =====================================================================
; NovaPascal — editor-first 6502-native vertical slice.
;
; The compiler accepts one program containing one writeln string, emits
; textual 65C02 assembly, invokes Nova's own assembler and linker, then runs
; the linked program from RAM.
; =====================================================================

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libabi.inc"
      .include "libeditor.inc"
      .include "novaasm.inc"
      .include "novalink.inc"

SOURCE_CAP  = 2048
ASM_CAP     = 3072
OBJECT_CAP  = 1024
LINK_CAP    = 1024
OUTPUT_BASE = $7000

PASCAL_OK         = 0
PASCAL_ERR_SYNTAX = 1
PASCAL_ERR_OUTPUT = 2

      .segment "ZEROPAGE"
p_src:       .res 2
p_left:      .res 2
p_word:      .res 2
p_out:       .res 2
p_out_left:  .res 2
p_copy_dst:  .res 2
p_tmp:       .res 2
p_expected:  .res 1
p_char:      .res 1

      .segment "BSS"
source_buf:       .res SOURCE_CAP
source_len:       .res 2
editor_hooks:     .res EDITOR_HOOKS_SIZE
editor_title:     .res 40
editor_type:      .res 8
      .export generated_asm
      .export generated_asm_len
      .export object_buf
      .export linked_buf
generated_asm:     .res ASM_CAP
generated_asm_len: .res 2
object_buf:        .res OBJECT_CAP
linked_buf:        .res LINK_CAP
pascal_error:      .res 1

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
      JSR   init_editor_state
      JSR   init_source

main_loop:
      JSR   open_editor
      LDA   LIB_STATUS
      CMP   #LERR_OK
      BNE   editor_failed
      LDA   LIB_RESULT+1
      BEQ   main_loop

      LDA   #$0C
      STA   VGC_CHAROUT
      LDA   #<msg_compiling
      STA   p_word
      LDA   #>msg_compiling
      STA   p_word+1
      JSR   print_z

      JSR   pascal_compile
      BNE   compile_failed
      JSR   assemble_generated
      BNE   assemble_failed
      JSR   link_object
      BNE   link_failed
      JSR   load_linked_program

      LDA   #<msg_running
      STA   p_word
      LDA   #>msg_running
      STA   p_word+1
      JSR   print_z
      JSR   OUTPUT_BASE
      LDA   #<msg_done
      STA   p_word
      LDA   #>msg_done
      STA   p_word+1
      JSR   print_z
      JSR   wait_key
      BRA   main_loop

editor_failed:
      LDA   #<msg_editor_error
      LDY   #>msg_editor_error
      BRA   show_error
compile_failed:
      LDA   #<msg_compile_error
      LDY   #>msg_compile_error
      BRA   show_error
assemble_failed:
      LDA   #<msg_assemble_error
      LDY   #>msg_assemble_error
      BRA   show_error
link_failed:
      LDA   #<msg_link_error
      LDY   #>msg_link_error
show_error:
      STA   p_word
      STY   p_word+1
      JSR   print_z
      JSR   wait_key
      BRA   main_loop

init_editor_state:
      LDA   #<editor_title_rom
      STA   p_src
      LDA   #>editor_title_rom
      STA   p_src+1
      LDA   #<editor_title
      STA   p_copy_dst
      LDA   #>editor_title
      STA   p_copy_dst+1
      JSR   copy_z
      LDA   #<editor_type_rom
      STA   p_src
      LDA   #>editor_type_rom
      STA   p_src+1
      LDA   #<editor_type
      STA   p_copy_dst
      LDA   #>editor_type
      STA   p_copy_dst+1
      JSR   copy_z

      LDX   #0
      LDA   #0
@clear_hooks:
      STA   editor_hooks,X
      INX
      CPX   #EDITOR_HOOKS_SIZE
      BNE   @clear_hooks
      LDA   #<editor_type
      STA   editor_hooks+EDITOR_HOOKS_TYPEL
      LDA   #>editor_type
      STA   editor_hooks+EDITOR_HOOKS_TYPEH
      RTS

init_source:
      LDA   #<hello_source
      STA   p_src
      LDA   #>hello_source
      STA   p_src+1
      LDA   #<source_buf
      STA   p_copy_dst
      LDA   #>source_buf
      STA   p_copy_dst+1
      LDY   #0
@copy:
      LDA   (p_src),Y
      BEQ   @done
      STA   (p_copy_dst),Y
      INC   p_src
      BNE   :+
      INC   p_src+1
:     INC   p_copy_dst
      BNE   :+
      INC   p_copy_dst+1
:     INC   source_len
      BNE   @copy
      INC   source_len+1
      BRA   @copy
@done:
      RTS

open_editor:
      LDA   #<source_buf
      STA   LIB_ARG0+0
      LDA   #>source_buf
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   source_len
      STA   LIB_ARG1+0
      LDA   source_len+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2+0
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   #<editor_title
      STA   LIB_ARG3+0
      LDA   #>editor_title
      STA   LIB_ARG3+1
      LDA   #<editor_hooks
      STA   LIB_ARG3+2
      LDA   #>editor_hooks
      STA   LIB_ARG3+3
      LDA   #MODULE_ID_EDITOR
      STA   LIB_MOD_ID
      LDA   #EDITOR_FN_EDIT
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_ARG1+0
      STA   source_len
      LDA   LIB_ARG1+1
      STA   source_len+1
      RTS

; ---------------------------------------------------------------------
; Pascal -> assembly. Accepted grammar:
;   program <identifier>; begin writeln('<ASCII>'); end.
; Whitespace and keyword case are insignificant.
; ---------------------------------------------------------------------
pascal_compile:
      STZ   pascal_error
      STZ   generated_asm_len
      STZ   generated_asm_len+1
      LDA   #<source_buf
      STA   p_src
      LDA   #>source_buf
      STA   p_src+1
      LDA   source_len
      STA   p_left
      LDA   source_len+1
      STA   p_left+1
      LDA   #<generated_asm
      STA   p_out
      LDA   #>generated_asm
      STA   p_out+1
      LDA   #<ASM_CAP
      STA   p_out_left
      LDA   #>ASM_CAP
      STA   p_out_left+1

      LDA   #<kw_program
      STA   p_word
      LDA   #>kw_program
      STA   p_word+1
      JSR   p_expect_word
      BCC   :+
      JMP   p_syntax_error
:
      JSR   p_identifier
      BCC   :+
      JMP   p_syntax_error
:
      LDA   #';'
      JSR   p_expect_char
      BCC   :+
      JMP   p_syntax_error
:
      LDA   #<kw_begin
      STA   p_word
      LDA   #>kw_begin
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      LDA   #<kw_writeln
      STA   p_word
      LDA   #>kw_writeln
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      LDA   #'('
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #$27
      JSR   p_expect_char
      BCS   p_syntax_error

@string:
      JSR   p_next
      BCC   p_syntax_error
      CMP   #$27
      BEQ   @string_done
      CMP   #$20
      BCC   p_syntax_error
      CMP   #$7F
      BCS   p_syntax_error
      JSR   p_emit_print_char
      BCS   p_output_error
      BRA   @string
@string_done:
      LDA   #')'
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #';'
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #<kw_end
      STA   p_word
      LDA   #>kw_end
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      LDA   #'.'
      JSR   p_expect_char
      BCS   p_syntax_error
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   p_syntax_error

      LDA   #$0D
      JSR   p_emit_print_char
      BCS   p_output_error
      LDA   #$0A
      JSR   p_emit_print_char
      BCS   p_output_error
      LDA   #<asm_rts
      STA   p_word
      LDA   #>asm_rts
      STA   p_word+1
      JSR   p_emit_text
      BCS   p_output_error
      LDA   #PASCAL_OK
      RTS

p_syntax_error:
      LDA   #PASCAL_ERR_SYNTAX
      BRA   p_compile_fail
p_output_error:
      LDA   #PASCAL_ERR_OUTPUT
p_compile_fail:
      STA   pascal_error
      LDA   #1
      RTS

p_emit_print_char:
      STA   p_char
      LDA   #<asm_lda
      STA   p_word
      LDA   #>asm_lda
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDA   p_char
      LSR
      LSR
      LSR
      LSR
      TAX
      LDA   hex_digits,X
      JSR   p_emit
      BCS   @fail
      LDA   p_char
      AND   #$0F
      TAX
      LDA   hex_digits,X
      JSR   p_emit
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_sta
      STA   p_word
      LDA   #>asm_sta
      STA   p_word+1
      JSR   p_emit_text
@fail:
      RTS

p_emit_text:
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @done
      JSR   p_emit
      BCS   @done
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@done:
      RTS

p_emit:
      PHA
      LDA   p_out_left
      ORA   p_out_left+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (p_out),Y
      INC   p_out
      BNE   :+
      INC   p_out+1
:     LDA   p_out_left
      BNE   :+
      DEC   p_out_left+1
:     DEC   p_out_left
      INC   generated_asm_len
      BNE   :+
      INC   generated_asm_len+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

p_expect_word:
      JSR   p_skip_ws
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @boundary
      STA   p_expected
      JSR   p_read_upper
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@boundary:
      JSR   p_peek_upper
      BCC   @ok
      JSR   p_is_ident
      BCS   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_identifier:
      JSR   p_skip_ws
      JSR   p_peek_upper
      BCC   @bad
      JSR   p_is_ident_start
      BCC   @bad
@loop:
      JSR   p_next
      JSR   p_peek_upper
      BCC   @ok
      JSR   p_is_ident
      BCS   @loop
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_expect_char:
      STA   p_expected
      JSR   p_skip_ws
      JSR   p_next
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

p_skip_ws:
@loop:
      JSR   p_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BNE   @done
@eat:
      JSR   p_next
      BRA   @loop
@done:
      RTS

p_peek_upper:
      JSR   p_peek
      BCC   @done
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

p_read_upper:
      JSR   p_next
      BCC   @done
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

p_is_ident_start:
      CMP   #'A'
      BCC   @underscore
      CMP   #'Z'+1
      BCC   @yes
@underscore:
      CMP   #'_'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

p_is_ident:
      JSR   p_is_ident_start
      BCS   @yes
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

p_peek:
      LDA   p_left
      ORA   p_left+1
      BEQ   @eof
      LDY   #0
      LDA   (p_src),Y
      SEC
      RTS
@eof:
      CLC
      RTS

p_next:
      JSR   p_peek
      BCC   @eof
      PHA
      INC   p_src
      BNE   :+
      INC   p_src+1
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      PLA
      SEC
@eof:
      RTS

assemble_generated:
      LDA   #<generated_asm
      STA   nasm_source_ptr
      LDA   #>generated_asm
      STA   nasm_source_ptr+1
      LDA   generated_asm_len
      STA   nasm_source_len
      LDA   generated_asm_len+1
      STA   nasm_source_len+1
      LDA   #<object_buf
      STA   nasm_object_ptr
      LDA   #>object_buf
      STA   nasm_object_ptr+1
      LDA   #<OBJECT_CAP
      STA   nasm_object_cap
      LDA   #>OBJECT_CAP
      STA   nasm_object_cap+1
      JMP   nasm_assemble

link_object:
      LDA   #<object_buf
      STA   nlink_object_ptr
      LDA   #>object_buf
      STA   nlink_object_ptr+1
      LDA   #<linked_buf
      STA   nlink_output_ptr
      LDA   #>linked_buf
      STA   nlink_output_ptr+1
      LDA   #<LINK_CAP
      STA   nlink_output_cap
      LDA   #>LINK_CAP
      STA   nlink_output_cap+1
      LDA   #<OUTPUT_BASE
      STA   nlink_load_base
      LDA   #>OUTPUT_BASE
      STA   nlink_load_base+1
      JMP   nlink_link

load_linked_program:
      LDA   #<(linked_buf+2)
      STA   p_src
      LDA   #>(linked_buf+2)
      STA   p_src+1
      LDA   #<OUTPUT_BASE
      STA   p_copy_dst
      LDA   #>OUTPUT_BASE
      STA   p_copy_dst+1
      SEC
      LDA   nlink_output_len
      SBC   #2
      STA   p_left
      LDA   nlink_output_len+1
      SBC   #0
      STA   p_left+1
@copy:
      LDA   p_left
      ORA   p_left+1
      BEQ   @done
      LDY   #0
      LDA   (p_src),Y
      STA   (p_copy_dst),Y
      INC   p_src
      BNE   :+
      INC   p_src+1
:     INC   p_copy_dst
      BNE   :+
      INC   p_copy_dst+1
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      BRA   @copy
@done:
      RTS

copy_z:
      LDY   #0
@loop:
      LDA   (p_src),Y
      STA   (p_copy_dst),Y
      BEQ   @done
      INC   p_src
      BNE   :+
      INC   p_src+1
:     INC   p_copy_dst
      BNE   @loop
      INC   p_copy_dst+1
      BRA   @loop
@done:
      RTS

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

wait_key:
      LDA   VGC_CHARIN
      BEQ   wait_key
      RTS

      .segment "RODATA"
kw_program: .byte "PROGRAM", 0
kw_begin:   .byte "BEGIN", 0
kw_writeln: .byte "WRITELN", 0
kw_end:     .byte "END", 0
hex_digits: .byte "0123456789ABCDEF"
asm_lda:    .byte "LDA #$", 0
asm_sta:    .byte "STA $A00E", $0A, 0
asm_rts:    .byte "RTS", $0A, 0

editor_title_rom: .byte "HELLO.PAS | Ctrl-S Alt-X Compile", 0
editor_type_rom:  .byte "Pascal", 0
hello_source:
      .byte "program Hello;", $0A
      .byte "begin", $0A
      .byte "  writeln('Hello from NovaPascal');", $0A
      .byte "end.", $0A, 0

msg_compiling:     .byte "NovaPascal: compiling to assembly...", $0D, $0A, 0
msg_running:       .byte "Linked at $7000. Running:", $0D, $0A, 0
msg_done:          .byte $0D, $0A, "Done. Press any key to edit.", 0
msg_editor_error:  .byte "EDITOR ERROR. Press any key.", 0
msg_compile_error: .byte "PASCAL SYNTAX ERROR. Press any key.", 0
msg_assemble_error:.byte "ASSEMBLER ERROR. Press any key.", 0
msg_link_error:    .byte "LINKER ERROR. Press any key.", 0

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
