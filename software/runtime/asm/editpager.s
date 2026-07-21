; Generic XRAM-backed document pager for the shared EDITOR module.

EDITPAGER_WINDOW_TARGET = $0C00
EDITPAGER_PAGE_CAP = 64
ep_ptr = EDITBUF_HL_PTR

      ; Keep the published EDITBUF hook mailbox at its historical addresses.
      ; The module linker places this private pager state after shared BSS.
      .segment "PAGERBSS"
ep_hooks:        .res EDITOR_HOOKS_SIZE
ep_base:         .res 3
ep_doc_len:      .res 2
ep_doc_cap:      .res 2
ep_window_off:   .res 2
ep_window_span:  .res 2
ep_page_offsets: .res EDITPAGER_PAGE_CAP * 2
ep_page_index:   .res 1
ep_dirty:        .res 1
ep_off:          .res 2
ep_dst:          .res 2
ep_count:        .res 2
ep_delta:        .res 2
ep_tail:         .res 2
ep_value:        .res 1
ep_title:        .res 2
ep_type:         .res 2
ep_buffer:       .res 2
ep_buffer_cap:   .res 2
ep_saved_winctl: .res 1
ep_saved_win3:   .res 3

      .segment "CODE"

; EDITOR_FN_EDIT_XRAM entry. The caller owns allocation and disk I/O; this
; generic module owns paging and editing only.
sys_edit_xram:
      LDA   LIB_ARG0+0
      STA   ep_base+0
      LDA   LIB_ARG0+1
      STA   ep_base+1
      LDA   LIB_ARG0+2
      STA   ep_base+2
      LDA   LIB_ARG1+0
      STA   ep_doc_len+0
      LDA   LIB_ARG1+1
      STA   ep_doc_len+1
      LDA   LIB_ARG1+2
      STA   ep_buffer+0
      LDA   LIB_ARG1+3
      STA   ep_buffer+1
      LDA   LIB_ARG2+0
      STA   ep_doc_cap+0
      LDA   LIB_ARG2+1
      STA   ep_doc_cap+1
      LDA   LIB_ARG2+2
      STA   ep_buffer_cap+0
      LDA   LIB_ARG2+3
      STA   ep_buffer_cap+1
      LDA   LIB_ARG3+0
      STA   ep_title+0
      LDA   LIB_ARG3+1
      STA   ep_title+1
      LDA   LIB_ARG3+2
      STA   ep_type+0
      LDA   LIB_ARG3+3
      STA   ep_type+1

      LDA   XMC_WINCTL
      STA   ep_saved_winctl
      ORA   #$08
      STA   XMC_WINCTL
      LDA   WIN3_LO
      STA   ep_saved_win3+0
      LDA   WIN3_MI
      STA   ep_saved_win3+1
      LDA   WIN3_HI
      STA   ep_saved_win3+2

      LDA   ep_buffer_cap+0
      ORA   ep_buffer_cap+1
      BEQ   @bad_args
      LDA   ep_doc_len+1
      CMP   ep_doc_cap+1
      BCC   @args_ok
      BNE   @bad_args
      LDA   ep_doc_len+0
      CMP   ep_doc_cap+0
      BCC   @args_ok
      BEQ   @args_ok
@bad_args:
      LDA   #LERR_EDITOR_FAIL
      STA   LIB_STATUS
      JMP   @done
@args_ok:
      JSR   ep_validate_document
      BCC   :+
      LDA   #LERR_EDITOR_NOT_TEXT
      STA   LIB_STATUS
      JMP   @done
:
      STZ   ep_window_off+0
      STZ   ep_window_off+1
      STZ   ep_page_index
      STZ   ep_page_offsets
      STZ   ep_page_offsets+1
      JSR   ep_load_window

      LDX   #EDITOR_HOOKS_SIZE-1
@clear_hooks:
      STZ   ep_hooks,X
      DEX
      BPL   @clear_hooks
      LDA   ep_type+0
      STA   ep_hooks+EDITOR_HOOKS_TYPEL
      LDA   ep_type+1
      STA   ep_hooks+EDITOR_HOOKS_TYPEH
      LDA   #<ep_command_hook
      STA   ep_hooks+EDITOR_HOOKS_COMMAND_VECL
      LDA   #>ep_command_hook
      STA   ep_hooks+EDITOR_HOOKS_COMMAND_VECH
      LDA   #<ep_changed_hook
      STA   ep_hooks+EDITOR_HOOKS_CHANGED_VECL
      LDA   #>ep_changed_hook
      STA   ep_hooks+EDITOR_HOOKS_CHANGED_VECH

      LDA   ep_buffer+0
      STA   LIB_ARG0+0
      LDA   ep_buffer+1
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   ep_window_span+0
      STA   LIB_ARG1+0
      LDA   ep_window_span+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   ep_buffer_cap+0
      STA   LIB_ARG2+0
      LDA   ep_buffer_cap+1
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   ep_title+0
      STA   LIB_ARG3+0
      LDA   ep_title+1
      STA   LIB_ARG3+1
      LDA   #<ep_hooks
      STA   LIB_ARG3+2
      LDA   #>ep_hooks
      STA   LIB_ARG3+3
      JSR   sys_edit
      LDA   LIB_STATUS
      BNE   @done
      JSR   ep_commit_window
      BCC   :+
      LDA   #LERR_EDITOR_FAIL
      STA   LIB_STATUS
      BRA   @done
:
      LDA   ep_doc_len+0
      STA   LIB_ARG1+0
      LDA   ep_doc_len+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
@done:
      LDA   ep_saved_win3+0
      STA   WIN3_LO
      LDA   ep_saved_win3+1
      STA   WIN3_MI
      LDA   ep_saved_win3+2
      STA   WIN3_HI
      LDA   ep_saved_winctl
      STA   XMC_WINCTL
      RTS

; Map ep_base + ep_off through XMC window 3. X returns the byte within page.
ep_map_off:
      CLC
      LDA   ep_base+0
      ADC   ep_off+0
      TAX
      STZ   WIN3_LO
      LDA   ep_base+1
      ADC   ep_off+1
      STA   WIN3_MI
      LDA   ep_base+2
      ADC   #0
      STA   WIN3_HI
      RTS

; Carry clear for editable ASCII text, set for control/high-bit bytes.
ep_validate_document:
      STZ   ep_off+0
      STZ   ep_off+1
@next:
      LDA   ep_off+0
      CMP   ep_doc_len+0
      BNE   @read
      LDA   ep_off+1
      CMP   ep_doc_len+1
      BEQ   @text
@read:
      JSR   ep_read_off
      CMP   #$09
      BEQ   @accepted
      CMP   #$0A
      BEQ   @accepted
      CMP   #$0D
      BEQ   @accepted
      CMP   #$20
      BCC   @binary
      CMP   #$7F
      BCS   @binary
@accepted:
      JSR   ep_inc_off
      BRA   @next
@text:
      CLC
      RTS
@binary:
      SEC
      RTS

ep_read_off:
      JSR   ep_map_off
      LDA   WIN3_BASE,X
      RTS

ep_write_dst:
      PHA
      LDA   ep_off+0
      PHA
      LDA   ep_off+1
      PHA
      LDA   ep_dst+0
      STA   ep_off+0
      LDA   ep_dst+1
      STA   ep_off+1
      JSR   ep_map_off
      PLA
      STA   ep_off+1
      PLA
      STA   ep_off+0
      PLA
      STA   WIN3_BASE,X
      RTS

ep_inc_off:
      INC   ep_off+0
      BNE   :+
      INC   ep_off+1
:     RTS

ep_load_window:
      SEC
      LDA   ep_doc_len+0
      SBC   ep_window_off+0
      STA   ep_tail+0
      LDA   ep_doc_len+1
      SBC   ep_window_off+1
      STA   ep_tail+1
      LDA   #<EDITPAGER_WINDOW_TARGET
      STA   ep_window_span+0
      LDA   #>EDITPAGER_WINDOW_TARGET
      STA   ep_window_span+1
      LDA   ep_buffer_cap+0
      CMP   ep_window_span+0
      LDA   ep_buffer_cap+1
      SBC   ep_window_span+1
      BCS   :+
      LDA   ep_buffer_cap+0
      STA   ep_window_span+0
      LDA   ep_buffer_cap+1
      STA   ep_window_span+1
:
      LDA   ep_tail+0
      CMP   ep_window_span+0
      LDA   ep_tail+1
      SBC   ep_window_span+1
      BCS   @copy
      LDA   ep_tail+0
      STA   ep_window_span+0
      LDA   ep_tail+1
      STA   ep_window_span+1
@copy:
      LDA   ep_window_off+0
      STA   ep_off+0
      LDA   ep_window_off+1
      STA   ep_off+1
      LDA   ep_buffer+0
      STA   ep_ptr+0
      LDA   ep_buffer+1
      STA   ep_ptr+1
      LDA   ep_window_span+0
      STA   ep_count+0
      LDA   ep_window_span+1
      STA   ep_count+1
@byte:
      LDA   ep_count+0
      ORA   ep_count+1
      BEQ   @align
      JSR   ep_read_off
      LDY   #0
      STA   (ep_ptr),Y
      INC   ep_ptr+0
      BNE   :+
      INC   ep_ptr+1
:     JSR   ep_inc_off
      LDA   ep_count+0
      BNE   :+
      DEC   ep_count+1
:     DEC   ep_count+0
      BRA   @byte
@align:
      ; Keep ordinary lines whole. A pathological >3 KiB line still splits.
      LDA   ep_tail+0
      CMP   ep_window_span+0
      BNE   :+
      LDA   ep_tail+1
      CMP   ep_window_span+1
      BEQ   @publish
:
      LDA   ep_window_span+0
      STA   ep_count+0
      LDA   ep_window_span+1
      STA   ep_count+1
@back:
      LDA   ep_count+0
      ORA   ep_count+1
      BEQ   @publish
      LDA   ep_count+0
      BNE   :+
      DEC   ep_count+1
:     DEC   ep_count+0
      CLC
      LDA   ep_buffer+0
      ADC   ep_count+0
      STA   ep_ptr+0
      LDA   ep_buffer+1
      ADC   ep_count+1
      STA   ep_ptr+1
      LDY   #0
      LDA   (ep_ptr),Y
      CMP   #$0A
      BNE   @back
      INC   ep_count+0
      BNE   :+
      INC   ep_count+1
:     LDA   ep_count+0
      STA   ep_window_span+0
      LDA   ep_count+1
      STA   ep_window_span+1
@publish:
      LDA   ep_window_span+0
      STA   EDITBUF_LENL
      LDA   ep_window_span+1
      STA   EDITBUF_LENH
      STZ   ep_dirty
      RTS

ep_changed_hook:
      LDA   #1
      STA   ep_dirty
      RTS

ep_command_hook:
      CMP   #EDITUI_CMD_WINDOW_NEXT
      BEQ   ep_window_next
      CMP   #EDITUI_CMD_WINDOW_PREVIOUS
      BEQ   ep_window_previous
      CMP   #EDITUI_CMD_WINDOW_FIRST
      BEQ   ep_window_first
      CMP   #EDITUI_CMD_WINDOW_LAST
      BEQ   ep_window_last
      RTS

ep_window_next:
      JSR   ep_commit_window
      BCS   @done
      CLC
      LDA   ep_window_off+0
      ADC   ep_window_span+0
      STA   ep_off+0
      LDA   ep_window_off+1
      ADC   ep_window_span+1
      STA   ep_off+1
      LDA   ep_off+0
      CMP   ep_doc_len+0
      LDA   ep_off+1
      SBC   ep_doc_len+1
      BCS   @done
      LDA   ep_off+0
      STA   ep_window_off+0
      LDA   ep_off+1
      STA   ep_window_off+1
      INC   ep_page_index
      JSR   ep_store_page_offset
      JSR   ep_load_window
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
@done:
      RTS

ep_window_previous:
      JSR   ep_commit_window
      BCS   @done
      LDA   ep_page_index
      BEQ   @done
      DEC   ep_page_index
      JSR   ep_load_page_offset
      JSR   ep_load_window
      LDA   ep_window_span+0
      STA   EDITBUF_CURL
      LDA   ep_window_span+1
      STA   EDITBUF_CURH
@done:
      RTS

ep_window_first:
      JSR   ep_commit_window
      BCS   @done
      STZ   ep_page_index
      STZ   ep_window_off+0
      STZ   ep_window_off+1
      JSR   ep_load_window
@done:
      RTS

ep_window_last:
      JSR   ep_commit_window
      BCS   @done
@next:
      CLC
      LDA   ep_window_off+0
      ADC   ep_window_span+0
      STA   ep_off+0
      LDA   ep_window_off+1
      ADC   ep_window_span+1
      STA   ep_off+1
      LDA   ep_off+0
      CMP   ep_doc_len+0
      LDA   ep_off+1
      SBC   ep_doc_len+1
      BCS   @at_end
      LDA   ep_off+0
      STA   ep_window_off+0
      LDA   ep_off+1
      STA   ep_window_off+1
      INC   ep_page_index
      JSR   ep_store_page_offset
      JSR   ep_load_window
      BRA   @next
@at_end:
      LDA   ep_window_span+0
      STA   EDITBUF_CURL
      LDA   ep_window_span+1
      STA   EDITBUF_CURH
@done:
      RTS

ep_store_page_offset:
      LDA   ep_page_index
      ASL   A
      TAX
      LDA   ep_window_off+0
      STA   ep_page_offsets,X
      LDA   ep_window_off+1
      STA   ep_page_offsets+1,X
      RTS

ep_load_page_offset:
      LDA   ep_page_index
      ASL   A
      TAX
      LDA   ep_page_offsets,X
      STA   ep_window_off+0
      LDA   ep_page_offsets+1,X
      STA   ep_window_off+1
      RTS

; Commit a changed window. Tail movement is deliberately O(n); files are at
; most 64 KiB in this ABI, and DMA chunking belongs here only if profiling says
; interactive page changes are too slow.
ep_commit_window:
      LDA   ep_dirty
      BNE   :+
      CLC
      RTS
:
      CLC
      LDA   ep_window_off+0
      ADC   ep_window_span+0
      STA   ep_tail+0
      LDA   ep_window_off+1
      ADC   ep_window_span+1
      STA   ep_tail+1

      LDA   EDITBUF_LENH
      CMP   ep_window_span+1
      BNE   @different_size
      LDA   EDITBUF_LENL
      CMP   ep_window_span+0
      BNE   @different_size
      JMP   @copy_window
@different_size:

      SEC
      LDA   EDITBUF_LENL
      SBC   ep_window_span+0
      STA   ep_delta+0
      LDA   EDITBUF_LENH
      SBC   ep_window_span+1
      STA   ep_delta+1
      BCS   :+
      JMP   @shrink
:
      CLC
      LDA   ep_doc_len+0
      ADC   ep_delta+0
      STA   ep_dst+0
      LDA   ep_doc_len+1
      ADC   ep_delta+1
      STA   ep_dst+1
      BCC   :+
      JMP   @full
:
      LDA   ep_doc_cap+0
      CMP   ep_dst+0
      LDA   ep_doc_cap+1
      SBC   ep_dst+1
      BCS   @grow_tail
      JMP   @full
@grow_tail:
      LDA   ep_doc_len+0
      STA   ep_off+0
      LDA   ep_doc_len+1
      STA   ep_off+1
@grow_byte:
      LDA   ep_off+0
      CMP   ep_tail+0
      BNE   :+
      LDA   ep_off+1
      CMP   ep_tail+1
      BEQ   @grown
:
      LDA   ep_off+0
      BNE   :+
      DEC   ep_off+1
:     DEC   ep_off+0
      JSR   ep_read_off
      STA   ep_value
      CLC
      LDA   ep_off+0
      ADC   ep_delta+0
      STA   ep_dst+0
      LDA   ep_off+1
      ADC   ep_delta+1
      STA   ep_dst+1
      LDA   ep_value
      JSR   ep_write_dst
      BRA   @grow_byte
@grown:
      CLC
      LDA   ep_doc_len+0
      ADC   ep_delta+0
      STA   ep_doc_len+0
      LDA   ep_doc_len+1
      ADC   ep_delta+1
      STA   ep_doc_len+1
      BRA   @copy_window

@shrink:
      SEC
      LDA   ep_window_span+0
      SBC   EDITBUF_LENL
      STA   ep_delta+0
      LDA   ep_window_span+1
      SBC   EDITBUF_LENH
      STA   ep_delta+1
      LDA   ep_tail+0
      STA   ep_off+0
      LDA   ep_tail+1
      STA   ep_off+1
@shrink_byte:
      LDA   ep_off+0
      CMP   ep_doc_len+0
      LDA   ep_off+1
      SBC   ep_doc_len+1
      BCS   @shrunk
      JSR   ep_read_off
      STA   ep_value
      SEC
      LDA   ep_off+0
      SBC   ep_delta+0
      STA   ep_dst+0
      LDA   ep_off+1
      SBC   ep_delta+1
      STA   ep_dst+1
      LDA   ep_value
      JSR   ep_write_dst
      JSR   ep_inc_off
      BRA   @shrink_byte
@shrunk:
      SEC
      LDA   ep_doc_len+0
      SBC   ep_delta+0
      STA   ep_doc_len+0
      LDA   ep_doc_len+1
      SBC   ep_delta+1
      STA   ep_doc_len+1

@copy_window:
      LDA   ep_window_off+0
      STA   ep_dst+0
      LDA   ep_window_off+1
      STA   ep_dst+1
      LDA   ep_buffer+0
      STA   ep_ptr+0
      LDA   ep_buffer+1
      STA   ep_ptr+1
      LDA   EDITBUF_LENL
      STA   ep_count+0
      LDA   EDITBUF_LENH
      STA   ep_count+1
@copy_byte:
      LDA   ep_count+0
      ORA   ep_count+1
      BEQ   @copied
      LDY   #0
      LDA   (ep_ptr),Y
      JSR   ep_write_dst
      INC   ep_ptr+0
      BNE   :+
      INC   ep_ptr+1
:     INC   ep_dst+0
      BNE   :+
      INC   ep_dst+1
:     LDA   ep_count+0
      BNE   :+
      DEC   ep_count+1
:     DEC   ep_count+0
      BRA   @copy_byte
@copied:
      LDA   EDITBUF_LENL
      STA   ep_window_span+0
      LDA   EDITBUF_LENH
      STA   ep_window_span+1
      STZ   ep_dirty
      CLC
      RTS
@full:
      LDA   #<ep_status_full
      STA   EDITBUF_STATUSL
      LDA   #>ep_status_full
      STA   EDITBUF_STATUSH
      SEC
      RTS

      .segment "RODATA"
ep_status_full: .byte "Document has reached its XRAM allocation limit.", 0
