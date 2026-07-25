; Shared NDK memory/system calls for resident Nova tool frontends.
;
; The including tool provides a zero-page memory_ptr. Allocation owners are
; four contiguous bytes: xaddr[3], followed by an allocated flag.

tool_mem_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; Callers clear the mailbox, set LIB_ARG2 to the exact allocation size, and
; pass A/X pointing at { xaddr[3], allocated }.
tool_alloc_xram:
      STA   memory_ptr
      STX   memory_ptr+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDY   #2
@address:
      LDA   LIB_RESULT,Y
      STA   (memory_ptr),Y
      DEY
      BPL   @address
      LDY   #3
      LDA   #1
      STA   (memory_ptr),Y
      LDA   #0
@done:
      RTS

; A/X points at { xaddr[3], allocated }; LIB_ARG2 retains its allocation size.
tool_release_xram:
      STA   memory_ptr
      STX   memory_ptr+1
      LDY   #3
      LDA   (memory_ptr),Y
      BEQ   @done
      LDA   #0
      STA   (memory_ptr),Y
      DEY
@address:
      LDA   (memory_ptr),Y
      STA   LIB_ARG0,Y
      DEY
      BPL   @address
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_sys_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS
