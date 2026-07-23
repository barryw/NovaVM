; NAS executable frontend: textual 65C02 assembly -> NOBJ.

      .setcpu "w65c02"
      .include "novaasm.inc"
      .include "longbranch.inc"
      .include "nptool.inc"
      .include "libmemory.inc"
      .include "libfiles.inc"
      .include "libsystem.inc"
      .include "xram.inc"
      .include "nas_preprocessor.inc"
      .include "nas_backend.inc"
      .include "nas_core.inc"

ASM_CAP      = $8000
ASM_LOAD_CAP = ASM_CAP
INCLUDE_CACHE_CAP = NASCORE_INCLUDE_STREAM_CAP
OBJECT_CAP   = NASCORE_OBJECT_CAP
EXPANDED_CAP = $8000
PROJECT_CAP  = 2048

      .segment "ZEROPAGE"
include_name_ptr:.res 2
nas_core_zp:     .res NASCORE_ZP_SIZE
.assert nas_core_zp = NASCORE_ZP_BASE, error, "NAS core zero-page ABI moved"
include_path_ptr:.res 2
stream_word_ptr: .res 2

      .segment "BSS"
source_xaddr:    .res 3
source_allocated:.res 1
expanded_xaddr: .res 3
expanded_allocated:.res 1
preprocessor_loaded:.res 1
backend_loaded:  .res 1
constant_xaddr: .res 3
constant_allocated:.res 1
symbol_xaddr:   .res 3
symbol_allocated:.res 1
object_xaddr:   .res 3
object_allocated:.res 1
reloc_xaddr:    .res 3
reloc_allocated:.res 1
include_count:   .res 1
include_slot:    .res 1
include_leaf_len:.res 1
include_load_len:.res 1
include_path_len:.res 1
include_cache_xaddr:.res 3
include_cache_allocated:.res 1
include_xaddr:   .res NASM_INCLUDE_DEPTH * 3
include_size_l:  .res NASM_INCLUDE_DEPTH
include_size_h:  .res NASM_INCLUDE_DEPTH
include_allocated:.res NASM_INCLUDE_DEPTH
include_names:   .res NASM_INCLUDE_DEPTH * NASM_FILENAME_CAP

      .segment "NOINIT"
workspace_buf:   .res PROJECT_CAP
; Project options are consumed before assembly, so the object output can reuse
; the same lower-RAM workspace without growing NAS's resident footprint.
project_buf      = workspace_buf
stream_buffer    = workspace_buf
stream_input_id  = workspace_buf+$0100
stream_output_id = workspace_buf+$0102
stream_input_open = workspace_buf+$0104
stream_output_open = workspace_buf+$0105
stream_left      = workspace_buf+$0106
stream_index     = workspace_buf+$0108
stream_line_state = workspace_buf+$0109
stream_token_len = workspace_buf+$010A
stream_token     = workspace_buf+$010B
expanded_len     = workspace_buf+$0120
preprocessor_status = workspace_buf+$0122
preprocessor_detail = workspace_buf+$0123
preprocessor_error = workspace_buf+$0124
release_error    = workspace_buf+$0125
release_detail   = workspace_buf+$0126
include_release_error = workspace_buf+$0127
project_left     = workspace_buf+$0128
project_found    = workspace_buf+$012A
project_expected = workspace_buf+$012B

      .segment "CODE"
      .export tool_main
      .export nasm_include_open
      .export nasm_include_close

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      STZ   NPTOOL_DIAG_LINE
      STZ   NPTOOL_DIAG_LINE+1
      STZ   NPTOOL_DIAG_COL
      STZ   NPTOOL_DIAG_COL+1
      STZ   NPTOOL_ARG2_LEN
      STZ   NASCORE_INCLUDE_MODE
      LDA   #<nas_banner
      LDX   #>nas_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   tool_bad_args
:
      JSR   nas_project_options
      BEQ   :+
      JMP   @fail
:
      LDA   #<nasm_include_open
      STA   NASCORE_INCLUDE_OPEN
      LDA   #>nasm_include_open
      STA   NASCORE_INCLUDE_OPEN+1
      LDA   #<nasm_include_close
      STA   NASCORE_INCLUDE_CLOSE
      LDA   #>nasm_include_close
      STA   NASCORE_INCLUDE_CLOSE+1
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_PREPROCESS_ONLY
      BEQ   @assemble_message
      LDA   #<nas_preprocessing
      LDX   #>nas_preprocessing
      BRA   @print_source
@assemble_message:
      LDA   #<nas_assembling
      LDX   #>nas_assembling
@print_source:
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   tool_alloc_source
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_load_source
      BEQ   :+
      JMP   @io_error
:
      LDA   XRAM_LENL
      STA   expanded_len
      LDA   XRAM_LENH
      STA   expanded_len+1
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_SOURCE_PREPROCESSED
      BEQ   @check_source_size
      ; The shell's assemble phase already names the preprocessed source.
      ; Let NASCORE page it from disk through FILE_PAGE instead of retaining
      ; a redundant second 32 KiB XRAM copy. The source allocation is its page
      ; cache and remains owned by source_allocated until tool_release_all.
      LDX   #2
@stream_address:
      LDA   source_xaddr,X
      STA   expanded_xaddr,X
      DEX
      BPL   @stream_address
      LDA   #<NASCORE_STREAM_SENTINEL
      STA   expanded_len
      LDA   #>NASCORE_STREAM_SENTINEL
      STA   expanded_len+1
      JMP   @assemble
@check_source_size:
      LDA   XRAM_LENH
      CMP   #>ASM_LOAD_CAP
      BNE   @source_loaded
      LDA   XRAM_LENL
      CMP   #<ASM_LOAD_CAP
      BNE   @source_loaded
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_PREPROCESS_ONLY
      long_beq @large_assembly
      JSR   tool_stream_preprocess_identity
      CMP   #0
      long_beq @large_preprocess_ok
      CMP   #2
      long_beq @too_large
      JMP   @io_error
@large_preprocess_ok:
      JSR   tool_release_all
      long_bne @memory_error
      LDA   #<nas_preprocess_ok
      LDX   #>nas_preprocess_ok
      JSR   nptool_print_z
      LDA   #0
      RTS
@large_assembly:
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_SOURCE_PREPROCESSED
      long_beq @too_large
      JMP   @assemble
@source_loaded:
      JSR   tool_alloc_expanded
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_preprocess
      BEQ   :+
      LDA   #NPTOOL_ERR_ASSEMBLE
      STA   NPTOOL_STATUS
      JMP   @fail_release
:
      ; Preprocessing has produced a self-contained expanded stream. Release
      ; the original immediately so large NDK includes can use those pages
      ; while the backend owns its symbol/object workspaces.
      JSR   tool_release_source
      long_bne @memory_error
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_PREPROCESS_ONLY
      BEQ   @assemble
      JSR   tool_save_expanded
      BEQ   :+
      JMP   @io_error
:
      JSR   tool_release_all
      BEQ   :+
      JMP   @memory_error
:
      LDA   #<nas_preprocess_ok
      LDX   #>nas_preprocess_ok
      JSR   nptool_print_z
      LDA   #0
      RTS
@assemble:
      JSR   tool_alloc_constants
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_alloc_symbols
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_alloc_object
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_alloc_relocations
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_load_backend
      BEQ   :+
      LDA   LIB_RESULT+1
      BNE   @backend_detail
      LDA   LIB_STATUS
@backend_detail:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_ASSEMBLE
      STA   NPTOOL_STATUS
      JMP   @fail_release
:
      LDA   expanded_xaddr+0
      STA   nasm_source_ptr+0
      LDA   expanded_xaddr+1
      STA   nasm_source_ptr+1
      LDA   expanded_xaddr+2
      STA   nasm_source_ptr+2
      LDA   expanded_len
      STA   nasm_source_len+0
      LDA   expanded_len+1
      STA   nasm_source_len+1
      LDA   #<NPTOOL_ARG0
      STA   nasm_source_name_ptr
      LDA   #>NPTOOL_ARG0
      STA   nasm_source_name_ptr+1
      LDA   NPTOOL_ARG0_LEN
      STA   nasm_source_name_len
      LDA   object_xaddr+0
      STA   nasm_object_ptr+0
      LDA   object_xaddr+1
      STA   nasm_object_ptr+1
      LDA   object_xaddr+2
      STA   nasm_object_ptr_h
      LDA   #<OBJECT_CAP
      STA   nasm_object_cap+0
      LDA   #>OBJECT_CAP
      STA   nasm_object_cap+1
      LDX   #2
@constant_address:
      LDA   constant_xaddr,X
      STA   nasm_constant_ptr,X
      DEX
      BPL   @constant_address
      LDA   symbol_xaddr
      STA   nasm_symbol_names_ptr
      LDA   symbol_xaddr+1
      STA   nasm_symbol_names_ptr+1
      LDA   symbol_xaddr+2
      STA   nasm_symbol_names_ptr_h
      LDA   reloc_xaddr
      STA   nasm_reloc_buffer_ptr
      LDA   reloc_xaddr+1
      STA   nasm_reloc_buffer_ptr+1
      LDA   reloc_xaddr+2
      STA   nasm_reloc_buffer_ptr_h
      JSR   NASCORE_ENTRY
      BNE   @assemble_error

      LDA   #<nas_writing
      LDX   #>nas_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   tool_save_object
      BEQ   :+
      JMP   @fail_release
:
      JSR   tool_release_all
      BEQ   :+
      JMP   @memory_error
:
      LDA   #<nas_ok
      LDX   #>nas_ok
      JSR   nptool_print_z
      LDA   #0
      RTS

@assemble_error:
      LDA   nasm_error
      STA   NPTOOL_DETAIL
      LDA   nasm_error_line
      STA   NPTOOL_DIAG_LINE
      LDA   nasm_error_line+1
      STA   NPTOOL_DIAG_LINE+1
      LDA   nasm_error_column
      STA   NPTOOL_DIAG_COL
      LDA   nasm_error_column+1
      STA   NPTOOL_DIAG_COL+1
      LDA   nasm_error_name_len
      STA   NPTOOL_ARG2_LEN
      LDA   nasm_error_name_ptr
      STA   include_name_ptr
      LDA   nasm_error_name_ptr+1
      STA   include_name_ptr+1
      LDY   #0
@error_name:
      CPY   NPTOOL_ARG2_LEN
      BCS   @error_name_done
      LDA   (include_name_ptr),Y
      STA   NPTOOL_ARG2,Y
      INY
      BRA   @error_name
@error_name_done:
      LDA   #0
      STA   NPTOOL_ARG2,Y
      LDA   #NPTOOL_ERR_ASSEMBLE
      STA   NPTOOL_STATUS
      BRA   @fail_release
@too_large:
      LDA   #NPTOOL_IO_TOO_LARGE
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      BRA   @fail_release
@io_error:
      LDA   FIO_ERRCODE
      BNE   :+
      LDA   LIB_STATUS
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
@fail_release:
      JSR   tool_release_all
@fail:
      LDA   #1
      RTS

@memory_error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      JMP   @fail_release

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

tool_clear_args:
      LDX   #15
@loop:
      STZ   LIB_ARG0,X
      DEX
      BPL   @loop
      RTS

tool_mem_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

tool_sys_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

tool_files_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; A large source that contains no preprocessor directives is already its own
; expanded form. Copy it with bounded RAM pages instead of imposing an XRAM
; size ceiling. Sources that do use preprocessing retain the full overlay path.
tool_stream_preprocess_identity:
      JSR   stream_scan_source
      CMP   #0
      BNE   @done
      JSR   stream_open_input
      BNE   @io
      JSR   stream_open_output
      BNE   @io
@copy:
      JSR   stream_read_block
      BNE   @io
      LDA   stream_left
      ORA   stream_left+1
      BEQ   @close
      JSR   stream_write_block
      BNE   @io
      BRA   @copy
@close:
      JSR   stream_close_input
      BNE   @io
      JSR   stream_close_output
      BNE   @io
      LDA   #0
@done:
      RTS
@io:
      JSR   stream_close_input
      JSR   stream_close_output
      JSR   stream_delete_output
      LDA   #1
      RTS

; Return 0 when an identity copy is safe, 2 when the real preprocessor is
; required, and 1 on file I/O failure.
stream_scan_source:
      JSR   stream_open_input
      BNE   @io
      STZ   stream_line_state
      STZ   stream_token_len
@block:
      JSR   stream_read_block
      BNE   @io
      LDA   stream_left
      ORA   stream_left+1
      BEQ   @eof
      STZ   stream_index
@byte:
      LDX   stream_index
      LDA   stream_buffer,X
      JSR   stream_scan_byte
      BCS   @needs_full
      INC   stream_index
      LDA   stream_left
      BNE   :+
      DEC   stream_left+1
:     DEC   stream_left
      LDA   stream_left
      ORA   stream_left+1
      BNE   @byte
      BRA   @block
@eof:
      LDA   stream_line_state
      CMP   #1
      BNE   @safe
      JSR   stream_directive_is_risky
      BCS   @needs_full
@safe:
      JSR   stream_close_input
      BNE   @io_done
      LDA   #0
      RTS
@needs_full:
      JSR   stream_close_input
      LDA   #2
      RTS
@io:
      JSR   stream_close_input
@io_done:
      LDA   #1
      RTS

; state 0=start of line, 1=collecting a leading dot-directive, 2=ordinary.
stream_scan_byte:
      STA   preprocessor_detail
      LDA   stream_line_state
      CMP   #1
      BEQ   @token
      CMP   #2
      BEQ   @ordinary
      LDA   preprocessor_detail
      CMP   #$0A
      BEQ   @safe
      CMP   #$0D
      BEQ   @safe
      CMP   #' '
      BEQ   @safe
      CMP   #$09
      BEQ   @safe
      CMP   #'.'
      BNE   @mark_ordinary
      LDA   #1
      STA   stream_line_state
      STZ   stream_token_len
      BRA   @safe
@mark_ordinary:
      LDA   #2
      STA   stream_line_state
      BRA   @safe
@ordinary:
      LDA   preprocessor_detail
      CMP   #$0A
      BNE   @safe
      STZ   stream_line_state
      BRA   @safe
@token:
      LDA   preprocessor_detail
      CMP   #$0A
      BEQ   @finish_line
      CMP   #$0D
      BEQ   @finish_token
      CMP   #' '
      BEQ   @finish_token
      CMP   #$09
      BEQ   @finish_token
      CMP   #'a'
      BCC   :+
      CMP   #'z'+1
      BCS   :+
      AND   #$DF
:     LDX   stream_token_len
      CPX   #16
      BCS   @risky
      STA   stream_token,X
      INC   stream_token_len
@safe:
      CLC
      RTS
@finish_line:
      JSR   stream_directive_is_risky
      BCS   @risky
      STZ   stream_line_state
      CLC
      RTS
@finish_token:
      JSR   stream_directive_is_risky
      BCS   @risky
      LDA   #2
      STA   stream_line_state
      CLC
      RTS
@risky:
      SEC
      RTS

stream_directive_is_risky:
      LDX   #0
@word:
      LDA   stream_risky_words_lo,X
      STA   stream_word_ptr
      LDA   stream_risky_words_hi,X
      STA   stream_word_ptr+1
      ORA   stream_word_ptr
      BEQ   @safe
      PHX
      JSR   stream_token_equals
      PLX
      BCS   @risky
      INX
      BRA   @word
@risky:
      SEC
      RTS
@safe:
      CLC
      RTS

stream_token_equals:
      LDY   #0
@byte:
      LDA   (stream_word_ptr),Y
      BEQ   @end
      CPY   stream_token_len
      BCS   @no
      CMP   stream_token,Y
      BNE   @no
      INY
      BRA   @byte
@end:
      CPY   stream_token_len
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

stream_open_input:
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_READ
      STA   LIB_ARG2
      LDA   #FILE_FOPEN
      JSR   tool_files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   stream_input_id
      LDA   LIB_RESULT+1
      STA   stream_input_id+1
      INC   stream_input_open
      LDA   #0
@done:
      RTS

stream_open_output:
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      LDA   #FILE_FCREATE
      JSR   tool_files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   stream_output_id
      LDA   LIB_RESULT+1
      STA   stream_output_id+1
      INC   stream_output_open
      LDA   #0
@done:
      RTS

stream_read_block:
      JSR   tool_clear_args
      LDA   stream_input_id
      STA   LIB_ARG0
      LDA   stream_input_id+1
      STA   LIB_ARG0+1
      LDA   #<stream_buffer
      STA   LIB_ARG1
      LDA   #>stream_buffer
      STA   LIB_ARG1+1
      STZ   LIB_ARG2
      LDA   #1
      STA   LIB_ARG2+1
      LDA   #FILE_FREAD
      JSR   tool_files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   stream_left
      LDA   LIB_RESULT+1
      STA   stream_left+1
      LDA   #0
@done:
      RTS

stream_write_block:
      JSR   tool_clear_args
      LDA   stream_output_id
      STA   LIB_ARG0
      LDA   stream_output_id+1
      STA   LIB_ARG0+1
      LDA   #<stream_buffer
      STA   LIB_ARG1
      LDA   #>stream_buffer
      STA   LIB_ARG1+1
      LDA   stream_left
      STA   LIB_ARG2
      LDA   stream_left+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   tool_files_call
      BNE   @done
      LDA   LIB_RESULT
      CMP   stream_left
      BNE   @bad
      LDA   LIB_RESULT+1
      CMP   stream_left+1
      BNE   @bad
      LDA   #0
@done:
      RTS
@bad:
      LDA   #1
      RTS

stream_close_input:
      LDA   stream_input_open
      BEQ   @ok
      STZ   stream_input_open
      JSR   tool_clear_args
      LDA   stream_input_id
      STA   LIB_ARG0
      LDA   stream_input_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   tool_files_call
@ok:
      LDA   #0
      RTS

stream_close_output:
      LDA   stream_output_open
      BEQ   @ok
      STZ   stream_output_open
      JSR   tool_clear_args
      LDA   stream_output_id
      STA   LIB_ARG0
      LDA   stream_output_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   tool_files_call
@ok:
      LDA   #0
      RTS

stream_delete_output:
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JMP   tool_files_call

tool_release_all:
      STZ   release_error
      JSR   tool_release_preprocessor
      JSR   tool_record_release
      JSR   tool_release_backend
      JSR   tool_record_release
      JSR   tool_release_includes
      JSR   tool_record_release
      JSR   tool_release_relocations
      JSR   tool_record_release
      JSR   tool_release_object
      JSR   tool_record_release
      JSR   tool_release_symbols
      JSR   tool_record_release
      JSR   tool_release_constants
      JSR   tool_record_release
      JSR   tool_release_expanded
      JSR   tool_record_release
      JSR   tool_release_source
      JSR   tool_record_release
      LDA   release_error
      BEQ   @done
      LDA   release_detail
      STA   LIB_STATUS
      LDA   #1
@done:
      RTS

tool_record_release:
      CMP   #0
      BEQ   @done
      PHA
      LDA   release_error
      BNE   @discard
      INC   release_error
      PLA
      STA   release_detail
      RTS
@discard:
      PLA
@done:
      RTS

tool_alloc_expanded:
      JSR   tool_clear_args
      LDA   #<EXPANDED_CAP
      STA   LIB_ARG2
      LDA   #>EXPANDED_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   expanded_xaddr,X
      DEX
      BPL   @copy
      INC   expanded_allocated
      LDA   #0
@done:
      RTS

tool_release_expanded:
      LDA   expanded_allocated
      BEQ   @done
      STZ   expanded_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   expanded_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<EXPANDED_CAP
      STA   LIB_ARG2
      LDA   #>EXPANDED_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_preprocess:
      STZ   preprocessor_error
      LDA   #<nas_preprocessing
      LDX   #>nas_preprocessing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   tool_clear_args
      LDA   #<naspp_name
      STA   LIB_ARG0
      LDA   #>naspp_name
      STA   LIB_ARG0+1
      LDA   #NASPP_NAME_LEN
      STA   LIB_ARG1
      LDA   #<NASPP_LOAD
      STA   LIB_ARG2
      LDA   #>NASPP_LOAD
      STA   LIB_ARG2+1
      LDA   #<NASPP_MAX
      STA   LIB_ARG3
      LDA   #>NASPP_MAX
      STA   LIB_ARG3+1
      LDA   #SYS_OVL_LOAD
      JSR   tool_sys_call
      BEQ   :+
      LDA   LIB_RESULT+1
      STA   preprocessor_detail
      BRA   @error
:
      INC   preprocessor_loaded
      JSR   tool_clear_args
      LDX   #2
@input:
      LDA   source_xaddr,X
      STA   NASPP_INPUT_XADDR,X
      LDA   expanded_xaddr,X
      STA   NASPP_OUTPUT_XADDR,X
      DEX
      BPL   @input
      LDA   expanded_len
      STA   NASPP_INPUT_LEN
      LDA   expanded_len+1
      STA   NASPP_INPUT_LEN+1
      LDA   #<EXPANDED_CAP
      STA   NASPP_OUTPUT_CAP
      LDA   #>EXPANDED_CAP
      STA   NASPP_OUTPUT_CAP+1
      ; NASPP resolves includes through callbacks that use other NDK modules.
      ; Invoke the NDK-loaded overlay from resident RAM so those nested calls
      ; cannot page the System module out from beneath SYS_OVL_MAIN's return.
      JSR   NASPP_LOAD
      STA   preprocessor_status
      LDA   NASPP_ERROR
      STA   preprocessor_detail
      LDA   NASPP_INPUT_LEN
      STA   expanded_len
      LDA   NASPP_INPUT_LEN+1
      STA   expanded_len+1
      LDA   NASPP_ERROR
      STA   preprocessor_error
      LDA   NASPP_ERROR_LINE
      STA   NPTOOL_DIAG_LINE
      LDA   NASPP_ERROR_LINE+1
      STA   NPTOOL_DIAG_LINE+1
      LDA   NASPP_ERROR_COL
      STA   NPTOOL_DIAG_COL
      LDA   NASPP_ERROR_COL+1
      STA   NPTOOL_DIAG_COL+1
      JSR   tool_release_preprocessor
      ORA   preprocessor_status
      BNE   @error
      LDA   #0
      RTS
@error:
      LDA   preprocessor_error
      BNE   :+
      LDA   preprocessor_detail
      BNE   :+
      LDA   LIB_RESULT+1
      BNE   :+
      LDA   LIB_STATUS
:
      STA   NPTOOL_DETAIL
      LDA   #1
      RTS

tool_save_expanded:
      LDA   #<nas_writing
      LDX   #>nas_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDX   #2
@address:
      LDA   expanded_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   expanded_len
      STA   LIB_ARG3
      LDA   expanded_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XSAVE
      JMP   tool_mem_call

tool_release_preprocessor:
      LDA   preprocessor_loaded
      BEQ   @done
      STZ   preprocessor_loaded
      JSR   tool_clear_args
      LDA   #SYS_OVL_UNLOAD
      JMP   tool_sys_call
@done:
      LDA   #0
      RTS

tool_load_backend:
      JSR   tool_clear_args
      LDA   #<nasbe_name
      STA   LIB_ARG0
      LDA   #>nasbe_name
      STA   LIB_ARG0+1
      LDA   #NASBE_NAME_LEN
      STA   LIB_ARG1
      LDA   #<NASBE_LOAD
      STA   LIB_ARG2
      LDA   #>NASBE_LOAD
      STA   LIB_ARG2+1
      LDA   #<NASBE_MAX
      STA   LIB_ARG3
      LDA   #>NASBE_MAX
      STA   LIB_ARG3+1
      LDA   #SYS_OVL_LOAD
      JSR   tool_sys_call
      BNE   @done
      INC   backend_loaded
      LDA   #0
@done:
      RTS

tool_release_backend:
      LDA   backend_loaded
      BEQ   @done
      STZ   backend_loaded
      JSR   tool_clear_args
      LDA   #SYS_OVL_UNLOAD
      JMP   tool_sys_call
@done:
      LDA   #0
      RTS

tool_alloc_constants:
      JSR   tool_clear_args
      LDA   #<NASCORE_CONSTANT_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_CONSTANT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   constant_xaddr,X
      DEX
      BPL   @copy
      INC   constant_allocated
      LDA   #0
@done:
      RTS

tool_release_constants:
      LDA   constant_allocated
      BEQ   @done
      STZ   constant_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   constant_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<NASCORE_CONSTANT_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_CONSTANT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_alloc_symbols:
      JSR   tool_clear_args
      LDA   #<NASCORE_SYMBOL_WORK_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_SYMBOL_WORK_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   symbol_xaddr,X
      DEX
      BPL   @copy
      INC   symbol_allocated
      LDA   #0
@done:
      RTS

tool_release_symbols:
      LDA   symbol_allocated
      BEQ   @done
      STZ   symbol_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   symbol_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<NASCORE_SYMBOL_WORK_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_SYMBOL_WORK_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_alloc_object:
      JSR   tool_clear_args
      LDA   #<NASCORE_OBJECT_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_OBJECT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   object_xaddr,X
      DEX
      BPL   @copy
      INC   object_allocated
      LDA   #0
@done:
      RTS

tool_release_object:
      LDA   object_allocated
      BEQ   @done
      STZ   object_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   object_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<NASCORE_OBJECT_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_OBJECT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_alloc_relocations:
      JSR   tool_clear_args
      LDA   #<NASCORE_RELOC_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_RELOC_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   reloc_xaddr,X
      DEX
      BPL   @copy
      INC   reloc_allocated
      LDA   #0
@done:
      RTS

tool_release_relocations:
      LDA   reloc_allocated
      BEQ   @done
      STZ   reloc_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   reloc_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<NASCORE_RELOC_CAP
      STA   LIB_ARG2
      LDA   #>NASCORE_RELOC_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_save_object:
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDX   #2
@address:
      LDA   object_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   nasm_object_len
      STA   LIB_ARG3
      LDA   nasm_object_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XSAVE
      JMP   tool_mem_call

tool_alloc_source:
      JSR   tool_clear_args
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG2+0
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   source_xaddr,X
      STA   expanded_xaddr,X
      DEX
      BPL   @copy
      INC   source_allocated
      LDA   #0
@done:
      RTS

tool_load_source:
      JSR   tool_clear_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1+0
      LDX   #2
@address:
      LDA   source_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG3+0
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   tool_mem_call

tool_release_source:
      LDA   source_allocated
      BEQ   @done
      STZ   source_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   source_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG2+0
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

; NAS include callbacks. Every include is streamed through one bounded cache;
; nested readers refill it after returning instead of retaining whole files.
nasm_include_open:
      LDA   nasm_include_name_len
      STA   include_leaf_len
      STA   include_load_len
      LDA   include_count
      CMP   #NASM_INCLUDE_DEPTH
      BCC   :+
      JMP   @bad
:
      STA   include_slot
      JSR   tool_include_name_pointer
      LDY   #0
@name:
      CPY   nasm_include_name_len
      BCS   @name_done
      LDA   nasm_include_name,Y
      STA   (include_name_ptr),Y
      INY
      BRA   @name
@name_done:
      LDA   #0
      STA   (include_name_ptr),Y

      JSR   tool_probe_include_current
      BEQ   @found
      LDA   NPTOOL_DETAIL
      CMP   #FIO_ERR_NOTFOUND
      long_bne @bad
      JSR   tool_include_name_pointer
      LDY   #0
      LDA   (include_name_ptr),Y
      CMP   #'/'
      long_beq @bad
      JSR   tool_probe_include_paths
      BEQ   @found
      LDA   NPTOOL_DETAIL
      CMP   #FIO_ERR_NOTFOUND
      long_bne @bad
      JSR   tool_restore_include_leaf
      LDA   include_load_len
      CMP   #NPTOOL_ARG_CAP-1
      long_bcs @bad
      TAY
@root_shift:
      DEY
      LDA   (include_name_ptr),Y
      INY
      STA   (include_name_ptr),Y
      DEY
      BNE   @root_shift
      LDA   #'/'
      STA   (include_name_ptr),Y
      INC   include_load_len
      JSR   tool_probe_include_current
      long_bne @bad
@found:
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   NASCORE_INCLUDE_MODE
      CMP   #NASCORE_INCLUDE_BUFFERED
      BNE   @stream
      JSR   tool_prepare_buffered_include
      long_bne @bad
      BRA   @publish
@stream:
      JSR   tool_alloc_include_cache
      long_bne @bad
      LDX   #0
      LDY   #0
@address:
      LDA   include_cache_xaddr,X
      STA   nasm_include_ptr,Y
      INX
      INY
      CPY   #3
      BCC   @address
      LDA   #<NASCORE_STREAM_SENTINEL
      STA   nasm_include_len
      LDA   #>NASCORE_STREAM_SENTINEL
      STA   nasm_include_len+1
@publish:
      LDA   include_load_len
      STA   nasm_include_display_len
      JSR   tool_include_name_pointer
      LDA   include_name_ptr
      STA   nasm_include_display_ptr
      LDA   include_name_ptr+1
      STA   nasm_include_display_ptr+1
      INC   include_count
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

; Search the two generic include directories supplied by the caller. Paths are
; disk-root-relative; NAS has no knowledge of the project option that produced
; them.
tool_probe_include_paths:
      LDY   NPTOOL_ARG3_LEN
      BEQ   @second
      LDA   #<NPTOOL_ARG3
      LDX   #>NPTOOL_ARG3
      JSR   tool_probe_include_path
      BEQ   @done
      LDA   NPTOOL_DETAIL
      CMP   #FIO_ERR_NOTFOUND
      BNE   @fail
@second:
      LDY   NPTOOL_ARG5_LEN
      BEQ   @fail
      LDA   #<NPTOOL_ARG5
      LDX   #>NPTOOL_ARG5
      JSR   tool_probe_include_path
@done:
      RTS
@fail:
      LDA   #1
      RTS

; A/X points at a path and Y is its length. Build /path/original-name in the
; active include slot and query it through the shared toolchain/NDK file API.
tool_probe_include_path:
      STA   include_path_ptr
      STX   include_path_ptr+1
      STY   include_path_len
      TYA
      CLC
      ADC   include_leaf_len
      ADC   #2
      CMP   #NASM_FILENAME_CAP
      BCS   @bad
      STA   include_load_len
      JSR   tool_include_name_pointer
      LDY   #0
      LDA   #'/'
      STA   (include_name_ptr),Y
@path:
      CPY   include_path_len
      BCS   @separator
      LDA   (include_path_ptr),Y
      INY
      STA   (include_name_ptr),Y
      BRA   @path
@separator:
      INY
      LDA   #'/'
      STA   (include_name_ptr),Y
      INY
      LDX   #0
@leaf:
      CPX   include_leaf_len
      BCS   @terminate
      LDA   nasm_include_name,X
      STA   (include_name_ptr),Y
      INX
      INY
      BRA   @leaf
@terminate:
      LDA   #0
      STA   (include_name_ptr),Y
      JMP   tool_probe_include_current
@bad:
      LDA   #1
      RTS

tool_restore_include_leaf:
      LDA   include_leaf_len
      STA   include_load_len
      JSR   tool_include_name_pointer
      LDY   #0
@copy_leaf:
      CPY   include_leaf_len
      BCS   @terminate
      LDA   nasm_include_name,Y
      STA   (include_name_ptr),Y
      INY
      BRA   @copy_leaf
@terminate:
      LDA   #0
      STA   (include_name_ptr),Y
      RTS

tool_probe_include_current:
      JSR   tool_include_name_pointer
      LDA   NASCORE_INCLUDE_MODE
      CMP   #NASCORE_INCLUDE_BUFFERED
      BEQ   @size
      LDA   include_name_ptr
      LDX   include_name_ptr+1
      LDY   include_load_len
      JMP   nptool_probe_named
@size:
      LDA   include_name_ptr
      LDX   include_name_ptr+1
      LDY   include_load_len
      JMP   nptool_size_named

tool_prepare_buffered_include:
      LDX   include_slot
      LDA   NPTOOL_IO_LEN
      STA   include_size_l,X
      LDA   NPTOOL_IO_LEN+1
      STA   include_size_h,X
      ORA   include_size_l,X
      BNE   @allocate
      INC   include_size_l,X
@allocate:
      JSR   tool_clear_args
      JSR   tool_include_cap
      STA   LIB_ARG2
      STX   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @fail
      JSR   tool_include_xindex
      LDY   #0
@save_address:
      LDA   LIB_RESULT,Y
      STA   include_xaddr,X
      INX
      INY
      CPY   #3
      BCC   @save_address
      LDX   include_slot
      INC   include_allocated,X
      JSR   tool_load_buffered_include
      BNE   @release_bad
      JSR   tool_include_xindex
      LDY   #0
@publish_address:
      LDA   include_xaddr,X
      STA   nasm_include_ptr,Y
      INX
      INY
      CPY   #3
      BCC   @publish_address
      LDA   XRAM_LENL
      STA   nasm_include_len
      LDA   XRAM_LENH
      STA   nasm_include_len+1
      LDA   #0
      RTS
@release_bad:
      JSR   tool_release_include_slot
@fail:
      LDA   #1
      RTS

tool_load_buffered_include:
      JSR   tool_clear_args
      JSR   tool_include_name_pointer
      LDA   include_name_ptr
      STA   LIB_ARG0
      LDA   include_name_ptr+1
      STA   LIB_ARG0+1
      LDA   include_load_len
      STA   LIB_ARG1
      JSR   tool_include_xindex
      LDY   #0
@address:
      LDA   include_xaddr,X
      STA   LIB_ARG2,Y
      INX
      INY
      CPY   #3
      BCC   @address
      JSR   tool_include_cap
      STA   LIB_ARG3
      STX   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   tool_mem_call

nasm_include_close:
      LDA   include_count
      BEQ   @bad
      DEC
      STA   include_slot
      JSR   tool_release_include_slot
      BNE   @bad
      DEC   include_count
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

tool_release_includes:
@close:
      LDA   include_count
      BEQ   @cache
      JSR   nasm_include_close
      BNE   @fail
      BRA   @close
@cache:
      LDA   include_cache_allocated
      BEQ   @done
      STZ   include_cache_allocated
      JSR   tool_clear_args
      LDX   #2
@address:
      LDA   include_cache_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<INCLUDE_CACHE_CAP
      STA   LIB_ARG2+0
      LDA   #>INCLUDE_CACHE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

tool_release_include_slot:
      LDX   include_slot
      LDA   include_allocated,X
      BEQ   @done
      STZ   include_allocated,X
      JSR   tool_clear_args
      JSR   tool_include_xindex
      LDY   #0
@address:
      LDA   include_xaddr,X
      STA   LIB_ARG0,Y
      INX
      INY
      CPY   #3
      BCC   @address
      JSR   tool_include_cap
      STA   LIB_ARG2
      STX   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

; Return the buffered allocation size for include_slot in A/X.
tool_include_cap:
      LDX   include_slot
      LDA   include_size_h,X
      PHA
      LDA   include_size_l,X
      PLX
      RTS

; include_slot -> X = include_xaddr byte index.
tool_include_xindex:
      LDA   include_slot
      ASL
      CLC
      ADC   include_slot
      TAX
      RTS

tool_alloc_include_cache:
      LDA   include_cache_allocated
      BNE   @done
      JSR   tool_clear_args
      LDA   #<INCLUDE_CACHE_CAP
      STA   LIB_ARG2+0
      LDA   #>INCLUDE_CACHE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @fail
      LDX   #2
@address:
      LDA   LIB_RESULT,X
      STA   include_cache_xaddr,X
      DEX
      BPL   @address
      INC   include_cache_allocated
@done:
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

; include_slot -> include_name_ptr for its fixed 64-byte filename slot.
tool_include_name_pointer:
      LDA   include_slot
      STA   include_name_ptr
      STZ   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      ASL   include_name_ptr
      ROL   include_name_ptr+1
      CLC
      LDA   include_name_ptr
      ADC   #<include_names
      STA   include_name_ptr
      LDA   include_name_ptr+1
      ADC   #>include_names
      STA   include_name_ptr+1
      RTS

      .include "nas_project.inc"

      .segment "RODATA"
stream_risky_words_lo:
      .byte <stream_kw_macro, <stream_kw_endmacro, <stream_kw_define
      .byte <stream_kw_undefine, <stream_kw_if, <stream_kw_ifdef
      .byte <stream_kw_ifndef, <stream_kw_elseif, <stream_kw_else
      .byte <stream_kw_endif, <stream_kw_includetext, 0
stream_risky_words_hi:
      .byte >stream_kw_macro, >stream_kw_endmacro, >stream_kw_define
      .byte >stream_kw_undefine, >stream_kw_if, >stream_kw_ifdef
      .byte >stream_kw_ifndef, >stream_kw_elseif, >stream_kw_else
      .byte >stream_kw_endif, >stream_kw_includetext, 0
stream_kw_macro:      .byte "MACRO", 0
stream_kw_endmacro:   .byte "ENDMACRO", 0
stream_kw_define:     .byte "DEFINE", 0
stream_kw_undefine:   .byte "UNDEFINE", 0
stream_kw_if:         .byte "IF", 0
stream_kw_ifdef:      .byte "IFDEF", 0
stream_kw_ifndef:     .byte "IFNDEF", 0
stream_kw_elseif:     .byte "ELSEIF", 0
stream_kw_else:       .byte "ELSE", 0
stream_kw_endif:      .byte "ENDIF", 0
stream_kw_includetext:.byte "INCLUDETEXT", 0
nas_banner:     .byte "Nova Assembler v1.0", $0D, $0A, 0
nas_assembling: .byte "Assembling ", 0
nas_preprocessing:.byte "Preprocessing ", 0
nas_writing:    .byte "Writing ", 0
nas_ok:         .byte "Assembly complete", $0D, $0A, 0
nas_preprocess_ok:.byte "Preprocess complete", $0D, $0A, 0
naspp_name:     .byte "/NASPP.OVL"
nasbe_name:     .byte "/NASBE.OVL"
