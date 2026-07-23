; Resident Nova Pascal compiler command: disk/XRAM I/O and frontend overlay.

      .setcpu "w65c02"
      .include "nptool.inc"
      .include "libmemory.inc"
      .include "libsystem.inc"
      .include "xram.inc"
      .include "npc_frontend.inc"

SOURCE_CAP = NPC_SOURCE_CAP

      .segment "BSS"
source_len:          .res 2
source_xaddr:        .res 3
source_allocated:    .res 1
frontend_loaded:     .res 1
frontend_status:     .res 1
frontend_release:    .res 1
frontend_detail:     .res 1
generated_asm_len:   .res 2
pascal_error:        .res 1

      .segment "CODE"
      .export npc_compile_file
      .export generated_asm_len
      .export pascal_error

npc_compile_file:
      STZ   source_allocated
      STZ   frontend_loaded
      STZ   frontend_status
      STZ   frontend_release
      STZ   frontend_detail
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<npc_banner
      LDX   #>npc_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<npc_compiling
      LDX   #>npc_compiling
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   npc_allocate_source
      BEQ   :+
      JMP   @memory_error
:
      JSR   npc_load_source
      BEQ   :+
      JMP   @io_error
:
      LDA   XRAM_LENL
      STA   source_len
      LDA   XRAM_LENH
      STA   source_len+1
      JSR   npc_load_frontend
      BEQ   :+
      JMP   @overlay_error
:
      JSR   npc_clear_lib_args
      LDX   #2
@source_address:
      LDA   source_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @source_address
      LDA   source_len
      STA   LIB_ARG1
      LDA   source_len+1
      STA   LIB_ARG1+1
      STZ   NPTOOL_ARG6_LEN
      ; Invoke from resident RAM. Frontend NDK calls may page System out, so a
      ; SYS_OVL_MAIN return frame inside System would not be safe.
      JSR   NPCFE_LOAD
      STA   frontend_status
      LDA   LIB_RESULT
      STA   generated_asm_len
      LDA   LIB_RESULT+1
      STA   generated_asm_len+1
      LDA   LIB_RESULT+2
      STA   pascal_error
      LDA   LIB_RESULT+3
      STA   frontend_detail
      JSR   npc_release_frontend
      STA   frontend_release
      LDA   frontend_status
      BNE   @compile_error
      LDA   frontend_release
      BNE   @overlay_error
      LDA   #<npc_writing
      LDX   #>npc_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<npc_ok
      LDX   #>npc_ok
      JSR   nptool_print_z
      JSR   npc_release_source
      BNE   @memory_error
      LDA   #0
      RTS

@memory_error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      BRA   @fail
@io_error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      BRA   @fail
@overlay_error:
      LDA   frontend_detail
      BNE   :+
      LDA   LIB_RESULT+1
      BNE   :+
      LDA   LIB_STATUS
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      BRA   @fail
@compile_error:
      LDA   pascal_error
      CMP   #NPCFE_ERR_INCLUDE
      BNE   :+
      LDA   frontend_detail
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_COMPILE
      STA   NPTOOL_STATUS
@fail:
      JSR   npc_release_frontend
      JSR   npc_release_source
      LDA   #1
      RTS

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

npc_clear_lib_args:
      LDX   #15
@clear:
      STZ   LIB_ARG0,X
      DEX
      BPL   @clear
      RTS

npc_memory_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

npc_system_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

npc_allocate_source:
      JSR   npc_clear_lib_args
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   npc_memory_call
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

npc_load_source:
      JSR   npc_clear_lib_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1
      LDX   #2
@address:
      LDA   source_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   #<NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3
      LDA   #>NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   npc_memory_call

npc_release_source:
      LDA   source_allocated
      BEQ   @done
      STZ   source_allocated
      JSR   npc_clear_lib_args
      LDX   #2
@address:
      LDA   source_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JSR   npc_memory_call
@done:
      RTS

npc_load_frontend:
      ; System replaces the switchable $C000 bank while servicing this call,
      ; so stage the resident filename in canonical low RAM first.
      LDX   #NPCFE_NAME_LEN-1
@name:
      LDA   npcfe_name,X
      STA   NPTOOL_ARG6,X
      DEX
      BPL   @name
      JSR   npc_clear_lib_args
      LDA   #<NPTOOL_ARG6
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG6
      STA   LIB_ARG0+1
      LDA   #NPCFE_NAME_LEN
      STA   LIB_ARG1
      LDA   #<NPCFE_LOAD
      STA   LIB_ARG2
      LDA   #>NPCFE_LOAD
      STA   LIB_ARG2+1
      LDA   #<NPCFE_MAX
      STA   LIB_ARG3
      LDA   #>NPCFE_MAX
      STA   LIB_ARG3+1
      LDA   #SYS_OVL_LOAD
      JSR   npc_system_call
      BEQ   @loaded
      LDA   LIB_RESULT+1
      STA   frontend_detail
      LDA   LIB_STATUS
      RTS
@loaded:
      INC   frontend_loaded
      LDA   #0
      RTS

npc_release_frontend:
      LDA   frontend_loaded
      BEQ   @done
      STZ   frontend_loaded
      JSR   npc_clear_lib_args
      LDA   #SYS_OVL_UNLOAD
      JSR   npc_system_call
      BEQ   @done
      LDA   LIB_RESULT+1
      STA   frontend_detail
      LDA   LIB_STATUS
@done:
      RTS

      .segment "RODATA"
npcfe_name:     .byte "/NPCFE.OVL"
npc_banner:     .byte "Nova Pascal Compiler v1.0", $0D, $0A, 0
npc_compiling:  .byte "Compiling ", 0
npc_writing:    .byte "Writing ", 0
npc_ok:         .byte "Compile successful", $0D, $0A, 0
