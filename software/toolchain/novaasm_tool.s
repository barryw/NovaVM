; NAS executable frontend: textual 65C02 assembly -> NOBJ.

      .setcpu "w65c02"
      .include "novaasm.inc"
      .include "nptool.inc"
      .include "libmemory.inc"
      .include "libsystem.inc"
      .include "xram.inc"
      .include "nas_preprocessor.inc"
      .include "nas_backend.inc"
      .include "nas_core.inc"

ASM_CAP      = $8000
ASM_LOAD_CAP = ASM_CAP + 1
INCLUDE_CAP  = $D000
INCLUDE_LOAD_CAP = INCLUDE_CAP + 1
INCLUDE_SMALL_CAP = $1000
INCLUDE_SMALL_LOAD_CAP = INCLUDE_SMALL_CAP + 1
OBJECT_CAP   = 2048
EXPANDED_CAP = $8000
PROJECT_CAP  = 2048

      .segment "ZEROPAGE"
include_name_ptr:.res 2
nas_core_zp:     .res NASCORE_ZP_SIZE

      .segment "BSS"
source_xaddr:    .res 3
source_allocated:.res 1
source_len:      .res 2
expanded_xaddr: .res 3
expanded_allocated:.res 1
backend_loaded:  .res 1
expanded_len:   .res 2
constant_xaddr: .res 3
constant_allocated:.res 1
include_count:   .res 1
include_slot:    .res 1
include_xaddr:   .res NASM_INCLUDE_DEPTH * 3
include_large:   .res NASM_INCLUDE_DEPTH
include_names:   .res NASM_INCLUDE_DEPTH * NASM_FILENAME_CAP
object_buf:      .res OBJECT_CAP
core_symbol_names:.res NASCORE_SYMBOL_NAMES_CAP
core_reloc_buffer:.res NASCORE_RELOC_CAP
project_buf:     .res PROJECT_CAP
project_left:    .res 2
project_found:   .res 1
project_expected:.res 1

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
      LDA   #<nas_assembling
      LDX   #>nas_assembling
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
      LDA   XRAM_LENH
      CMP   #>ASM_LOAD_CAP
      BNE   :+
      LDA   XRAM_LENL
      CMP   #<ASM_LOAD_CAP
      BNE   :+
      JMP   @too_large
:
      LDA   XRAM_LENL
      STA   source_len
      LDA   XRAM_LENH
      STA   source_len+1
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
      JSR   tool_alloc_constants
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
      LDA   #<object_buf
      STA   nasm_object_ptr+0
      LDA   #>object_buf
      STA   nasm_object_ptr+1
      LDA   #<OBJECT_CAP
      STA   nasm_object_cap+0
      LDA   #>OBJECT_CAP
      STA   nasm_object_cap+1
      LDA   #<nasm_include_open
      STA   NASCORE_INCLUDE_OPEN
      LDA   #>nasm_include_open
      STA   NASCORE_INCLUDE_OPEN+1
      LDA   #<nasm_include_close
      STA   NASCORE_INCLUDE_CLOSE
      LDA   #>nasm_include_close
      STA   NASCORE_INCLUDE_CLOSE+1
      LDX   #2
@constant_address:
      LDA   constant_xaddr,X
      STA   nasm_constant_ptr,X
      DEX
      BPL   @constant_address
      LDA   #<core_symbol_names
      STA   nasm_symbol_names_ptr
      LDA   #>core_symbol_names
      STA   nasm_symbol_names_ptr+1
      LDA   #<core_reloc_buffer
      STA   nasm_reloc_buffer_ptr
      LDA   #>core_reloc_buffer
      STA   nasm_reloc_buffer_ptr+1
      JSR   NASCORE_ENTRY
      BNE   @assemble_error

      LDA   #<object_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>object_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   nasm_object_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   nasm_object_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   #<nas_writing
      LDX   #>nas_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   nptool_save_arg1
      BEQ   :+
      JMP   @fail_release
:
      JSR   tool_release_backend
      JSR   tool_release_includes
      JSR   tool_release_constants
      JSR   tool_release_expanded
      JSR   tool_release_source
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
      JSR   tool_release_backend
      JSR   tool_release_includes
      JSR   tool_release_constants
      JSR   tool_release_expanded
      JSR   tool_release_source
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
      BNE   @error
      JSR   tool_clear_args
      LDX   #2
@input:
      LDA   source_xaddr,X
      STA   NASPP_INPUT_XADDR,X
      LDA   expanded_xaddr,X
      STA   NASPP_OUTPUT_XADDR,X
      DEX
      BPL   @input
      LDA   source_len
      STA   NASPP_INPUT_LEN
      LDA   source_len+1
      STA   NASPP_INPUT_LEN+1
      LDA   #<EXPANDED_CAP
      STA   NASPP_OUTPUT_CAP
      LDA   #>EXPANDED_CAP
      STA   NASPP_OUTPUT_CAP+1
      LDA   #SYS_OVL_MAIN
      JSR   tool_sys_call
      BNE   @error
      LDA   NASPP_INPUT_LEN
      STA   expanded_len
      LDA   NASPP_INPUT_LEN+1
      STA   expanded_len+1
      JSR   tool_clear_args
      LDA   #SYS_OVL_UNLOAD
      JMP   tool_sys_call
@error:
      LDA   NASPP_ERROR_LINE
      STA   NPTOOL_DIAG_LINE
      LDA   NASPP_ERROR_LINE+1
      STA   NPTOOL_DIAG_LINE+1
      LDA   NASPP_ERROR_COL
      STA   NPTOOL_DIAG_COL
      LDA   NASPP_ERROR_COL+1
      STA   NPTOOL_DIAG_COL+1
      LDA   NASPP_ERROR
      BNE   :+
      LDA   LIB_RESULT+1
:
      STA   NPTOOL_DETAIL
      LDA   #1
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

; NAS include callbacks. Each active source owns one fixed-capacity XRAM block.
nasm_include_open:
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
      LDX   include_slot
      STZ   include_large,X

@allocate:
      JSR   tool_clear_args
      JSR   tool_include_cap
      STA   LIB_ARG2
      STX   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BEQ   :+
      JMP   @bad
:
      JSR   tool_include_xindex
      LDY   #0
@address:
      LDA   LIB_RESULT,Y
      STA   include_xaddr,X
      INX
      INY
      CPY   #3
      BCC   @address

      JSR   tool_clear_args
      JSR   tool_include_name_pointer
      LDA   include_name_ptr
      STA   LIB_ARG0
      LDA   include_name_ptr+1
      STA   LIB_ARG0+1
      LDA   nasm_include_name_len
      STA   LIB_ARG1
      JSR   tool_include_xindex
      LDY   #0
@load_address:
      LDA   include_xaddr,X
      STA   LIB_ARG2,Y
      INX
      INY
      CPY   #3
      BCC   @load_address
      JSR   tool_include_cap
      STA   LIB_ARG3
      STX   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JSR   tool_mem_call
      BNE   @release_bad
      LDX   include_slot
      LDA   include_large,X
      BNE   @check_large
      LDA   XRAM_LENH
      CMP   #>INCLUDE_SMALL_LOAD_CAP
      BNE   @loaded
      LDA   XRAM_LENL
      CMP   #<INCLUDE_SMALL_LOAD_CAP
      BNE   @loaded
      JSR   tool_release_include_slot
      BNE   @bad
      LDX   include_slot
      INC   include_large,X
      BRA   @allocate
@check_large:
      LDA   XRAM_LENH
      CMP   #>INCLUDE_LOAD_CAP
      BNE   @loaded
      LDA   XRAM_LENL
      CMP   #<INCLUDE_LOAD_CAP
      BEQ   @release_bad
@loaded:
      JSR   tool_include_xindex
      LDY   #0
@result_address:
      LDA   include_xaddr,X
      STA   nasm_include_ptr,Y
      INX
      INY
      CPY   #3
      BCC   @result_address
      LDA   XRAM_LENL
      STA   nasm_include_len
      LDA   XRAM_LENH
      STA   nasm_include_len+1
      JSR   tool_include_name_pointer
      LDA   include_name_ptr
      STA   nasm_include_display_ptr
      LDA   include_name_ptr+1
      STA   nasm_include_display_ptr+1
      INC   include_count
      LDA   #0
      RTS
@release_bad:
      JSR   tool_release_include_slot
@bad:
      LDA   #1
      RTS

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
@loop:
      LDA   include_count
      BEQ   @done
      JSR   nasm_include_close
      BEQ   @loop
      DEC   include_count
      BRA   @loop
@done:
      LDA   #0
      RTS

tool_release_include_slot:
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

; Return the allocation size for include_slot in A/X.
tool_include_cap:
      LDX   include_slot
      LDA   include_large,X
      BEQ   @small
      LDA   #<INCLUDE_LOAD_CAP
      LDX   #>INCLUDE_LOAD_CAP
      RTS
@small:
      LDA   #<INCLUDE_SMALL_LOAD_CAP
      LDX   #>INCLUDE_SMALL_LOAD_CAP
      RTS

; include_slot -> X = include_xaddr byte index.
tool_include_xindex:
      LDA   include_slot
      ASL
      CLC
      ADC   include_slot
      TAX
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
nas_banner:     .byte "Nova Assembler v1.0", $0D, $0A, 0
nas_assembling: .byte "Assembling ", 0
nas_preprocessing:.byte "Preprocessing ", 0
nas_writing:    .byte "Writing ", 0
nas_ok:         .byte "Assembly successful", $0D, $0A, 0
naspp_name:     .byte "NASPP.OVL"
nasbe_name:     .byte "NASBE.OVL"
