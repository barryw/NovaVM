; NovaPascal editor executable. Alt-X returns through tool_main to the shell.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "editbuf.inc"
      .include "editui.inc"
      .include "libeditor.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "libsystem.inc"
      .include "nui.inc"
      .include "nptool.inc"
      .include "longbranch.inc"

      .define tool_clear_lib_args nptool_clear_args

SOURCE_CAP = $1000
DOCUMENT_CAP = $FFFF
PICKER_NAME_CAP = 64
PICKER_ROWS_SIZE = 24 * 36

; Remembered caret lines, keyed by file name. The table is paged out to a named
; XRAM block so it outlives the tool, which is reloaded from disk every time the
; shell opens the editor. Eight slots, oldest evicted first.
POS_SLOTS    = 8
POS_NAME_CAP = 13
POS_ENTRY    = POS_NAME_CAP + 2       ; name, then a one-based 16-bit line
POS_TABLE    = POS_SLOTS * POS_ENTRY

; Project-wide search. The needle and the position of the last hit are paged out
; the same way, so F7 walks hit to hit across editor sessions instead of
; restarting at the top of the project every time.
SEARCH_CAP   = 24
SEARCH_FILES = 16
SEARCH_NAME  = 14

      .segment "ZEROPAGE"
search_ptr:         .res 2            ; indirect cursor over search_names

      .segment "BSS"
source_len:         .res 2
document_xaddr:     .res 3
document_allocated: .res 1
editor_type_ptr:    .res 2
editor_hooks:       .res EDITOR_HOOKS_SIZE
editor_hl_color:    .res 1
editor_exit:        .res 1
editor_command_action:.res 1
editor_saved:       .res 1
picker_name:        .res PICKER_NAME_CAP
picker_name_len:    .res 1
picker_cwd:         .res PICKER_NAME_CAP
picker_cwd_len:     .res 1
picker_rows:        .res PICKER_ROWS_SIZE
pos_table:          .res POS_TABLE
pos_slot:           .res 1            ; byte offset of the slot matching ARG0
pos_tmp:            .res 1
pos_line:           .res 2            ; caret line the editor reported

; Paged out as one block, in this order.
search_state:
search_needle:      .res SEARCH_CAP
search_needle_len:  .res 1
search_file:        .res 1            ; index of the file holding the last hit
search_line:        .res 2            ; line of the last hit, one-based
search_state_end:

search_cfg:         .res 13
search_names:       .res SEARCH_FILES * SEARCH_NAME
search_count:       .res 1
search_index:       .res 1
search_scan_line:   .res 2
search_typed:       .res SEARCH_CAP
search_typed_len:   .res 1
search_hit:         .res 1            ; the goto below came from a search, not a build


      .segment "NOINIT"
source_buf:         .res SOURCE_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<source_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>source_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<SOURCE_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>SOURCE_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   @loaded_ram
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_DETAIL
      CMP   #NPTOOL_IO_TOO_LARGE
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      JSR   tool_alloc_document
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_load_document
      BEQ   :+
      JMP   @memory_error
:
      BRA   @edit

@loaded_ram:
      JSR   nptool_validate_text
      BEQ   :+
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      JSR   tool_alloc_document
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_copy_document
      BEQ   :+
      JMP   @memory_error
:

@edit:
      STZ   editor_command_action
      ; A failed build leaves its location in NPTOOL_DIAG_*; carry it into the
      ; editor so reopening lands on the offending line instead of the top.
      LDA   NPTOOL_DIAG_LINE+0
      STA   editor_hooks+EDITOR_HOOKS_GOTO_LINEL
      LDA   NPTOOL_DIAG_LINE+1
      STA   editor_hooks+EDITOR_HOOKS_GOTO_LINEH
      LDA   NPTOOL_DIAG_COL+0
      STA   editor_hooks+EDITOR_HOOKS_GOTO_COL
      ; Say why the cursor moved. The status bar already carries line:col, so
      ; the message only has to name the cause.
      STZ   editor_hooks+EDITOR_HOOKS_STATUSL
      STZ   editor_hooks+EDITOR_HOOKS_STATUSH
      LDA   search_hit
      BNE   :+                         ; a search hit explains itself
      LDA   editor_hooks+EDITOR_HOOKS_GOTO_LINEL
      ORA   editor_hooks+EDITOR_HOOKS_GOTO_LINEH
      BEQ   :+
      LDA   #<editor_build_failed
      STA   editor_hooks+EDITOR_HOOKS_STATUSL
      LDA   #>editor_build_failed
      STA   editor_hooks+EDITOR_HOOKS_STATUSH
:     LDA   editor_hooks+EDITOR_HOOKS_GOTO_LINEL
      ORA   editor_hooks+EDITOR_HOOKS_GOTO_LINEH
      BNE   :+
      JSR   pos_recall                 ; no diagnostic: reopen where we left off
:
      JSR   editor_select_type
      LDA   editor_type_ptr+0
      STA   editor_hooks+EDITOR_HOOKS_TYPEL
      LDA   editor_type_ptr+1
      STA   editor_hooks+EDITOR_HOOKS_TYPEH
      LDA   #<editor_command_hook
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECL
      LDA   #>editor_command_hook
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECH
      LDA   document_xaddr+0
      STA   LIB_ARG0+0
      LDA   document_xaddr+1
      STA   LIB_ARG0+1
      LDA   document_xaddr+2
      STA   LIB_ARG0+2
      LDA   #EDITOR_XRAM_FLAG_HOOKS
      STA   LIB_ARG0+3
      LDA   source_len+0
      STA   LIB_ARG1+0
      LDA   source_len+1
      STA   LIB_ARG1+1
      LDA   #<source_buf
      STA   LIB_ARG1+2
      LDA   #>source_buf
      STA   LIB_ARG1+3
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2+2
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+3
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG3+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG3+1
      LDA   #<editor_hooks
      STA   LIB_ARG3+2
      LDA   #>editor_hooks
      STA   LIB_ARG3+3
      LDA   #MODULE_ID_EDITOR
      STA   LIB_MOD_ID
      LDA   #EDITOR_FN_EDIT_XRAM
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      long_bne @editor_error
      LDA   LIB_RESULT+0
      STA   editor_exit
      LDA   LIB_ARG1+0
      STA   source_len+0
      LDA   LIB_ARG1+1
      STA   source_len+1
      LDA   LIB_RESULT+2               ; capture before a save can reuse the mailbox
      STA   pos_line+0
      LDA   LIB_RESULT+3
      STA   pos_line+1
      LDA   LIB_RESULT+1
      long_beq @after_save
      LDA   #1
      STA   editor_saved
      LDA   source_len+0
      ORA   source_len+1
      BEQ   @save_empty
      JSR   tool_save_document
      long_bne @memory_error
      JMP   @after_save
@save_empty:
      STZ   NPTOOL_IO_LEN+0
      STZ   NPTOOL_IO_LEN+1
      JSR   nptool_save_arg0
      long_bne @fail
@after_save:
      JSR   pos_record
      LDA   editor_exit
      CMP   #EDITBUF_EXIT_COMMAND
      BNE   @ok
      LDA   editor_command_action
      BEQ   @ok
      CMP   #EDITUI_CMD_BUILD
      BEQ   @toolchain
      CMP   #EDITUI_CMD_RUN
      BEQ   @toolchain
      CMP   #EDITUI_CMD_SEARCH_PROJECT
      BNE   @picker
      JSR   tool_search_project
      BCC   @edit_again                ; cancelled or nothing found: stay put
      JSR   tool_load_current_document
      BNE   @fail
      JMP   @edit
@edit_again:
      JMP   @edit
@picker:
      JSR   tool_pick_file
      CMP   #2
      BEQ   @fail
      CMP   #1
      BEQ   @reload
      JSR   tool_use_picker_name
@reload:
      JSR   tool_load_current_document
      BNE   @fail
      JMP   @edit
      ; F9/Ctrl-F9: the buffer is saved above, so hand the request to the shell.
      ; It owns the compiler and reopens us once the step finishes.
@toolchain:
      CMP   #EDITUI_CMD_BUILD
      BNE   :+
      LDA   #NPTOOL_EDIT_BUILD
      BRA   :++
:     LDA   #NPTOOL_EDIT_RUN
:     STA   NPTOOL_DETAIL
      JSR   tool_release_document
      BNE   @memory_error
      LDA   #0
      RTS
@ok:
      LDA   editor_saved
      STA   NPTOOL_DETAIL
      JSR   tool_release_document
      BNE   @memory_error
      LDA   #0
      RTS

@editor_error:
      CMP   #LERR_EDITOR_NOT_TEXT
      BNE   :+
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
      BRA   @fail
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_EDITOR
      STA   NPTOOL_STATUS
@fail:
      JSR   tool_release_document
      LDA   #1
      RTS

@memory_error:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      BRA   @fail

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

tool_mem_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

tool_alloc_document:
      JSR   tool_clear_lib_args
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   document_xaddr,X
      DEX
      BPL   @copy
      INC   document_allocated
      LDA   #0
@done:
      RTS

tool_copy_document:
      LDA   source_len+0
      ORA   source_len+1
      BEQ   @done
      JSR   tool_clear_lib_args
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<source_buf
      STA   LIB_ARG1+0
      LDA   #>source_buf
      STA   LIB_ARG1+1
      LDA   source_len+0
      STA   LIB_ARG2+0
      LDA   source_len+1
      STA   LIB_ARG2+1
      LDA   #MEM_COPY_FROM_RAM
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_load_document:
      JSR   tool_clear_lib_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1+0
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   source_len+0
      STA   LIB_ARG3+0
      LDA   source_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   tool_mem_call

tool_save_document:
      JSR   tool_clear_lib_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1+0
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   source_len+0
      STA   LIB_ARG3+0
      LDA   source_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XSAVE
      JMP   tool_mem_call

tool_release_document:
      LDA   document_allocated
      BEQ   @done
      STZ   document_allocated
      JSR   tool_clear_lib_args
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

; Replace the allocated XRAM document with the text file named by ARG0.
tool_load_current_document:
      LDA   #<source_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>source_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<SOURCE_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>SOURCE_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   @loaded_ram
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BNE   @failed
      LDA   NPTOOL_DETAIL
      CMP   #NPTOOL_IO_TOO_LARGE
      BNE   @failed
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      JSR   tool_load_document
      BEQ   @ok
      BRA   @memory_error
@loaded_ram:
      JSR   nptool_validate_text
      BNE   @not_text
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      JSR   tool_copy_document
      BEQ   @ok
@memory_error:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      LDA   #1
      RTS
@not_text:
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
@failed:
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

tool_save_picker_cwd:
      JSR   tool_clear_lib_args
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_PWD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @done
      LDA   FIO_NAMELEN
      CMP   #PICKER_NAME_CAP
      BCS   @bad
      STA   picker_cwd_len
      TAX
      STZ   picker_cwd,X
@copy:
      DEX
      BMI   @ok
      LDA   FIO_NAME,X
      STA   picker_cwd,X
      BRA   @copy
@ok:
      LDA   #0
@done:
      RTS
@bad:
      LDA   #LERR_FIO_FAIL
      STA   LIB_STATUS
      RTS

tool_restore_picker_cwd:
      JSR   tool_clear_lib_args
      LDA   #<picker_cwd
      STA   LIB_ARG0+0
      LDA   #>picker_cwd
      STA   LIB_ARG0+1
      LDA   picker_cwd_len
      STA   LIB_ARG1+0
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_CD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; Return A=0 for a selected file, 1 for cancel, 2 for an NDK failure.
tool_pick_file:
      JSR   tool_save_picker_cwd
      BNE   @error
      JSR   tool_clear_lib_args
      LDA   #<picker_config
      STA   LIB_ARG0+0
      LDA   #>picker_config
      STA   LIB_ARG0+1
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      LDA   #SYS_NUI_FILE_PICKER
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @restore_error
      LDA   LIB_RESULT+1
      CMP   #NUI_RESULT_CANCEL
      BEQ   @cancel
      LDA   LIB_RESULT+0
      BEQ   @restore_error
      STA   picker_name_len
      LDA   #0
      RTS
@cancel:
      JSR   tool_restore_picker_cwd
      BNE   @error
      LDA   #1
      RTS
@restore_error:
      LDA   LIB_STATUS
      BNE   :+
      LDA   #LERR_SYS_FAIL
:     STA   NPTOOL_DETAIL
      JSR   tool_restore_picker_cwd
      BRA   @publish_error
@error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
@publish_error:
      LDA   #NPTOOL_ERR_EDITOR
      STA   NPTOOL_STATUS
      LDA   #2
      RTS

tool_use_picker_name:
      LDA   picker_name_len
      STA   NPTOOL_ARG0_LEN
      TAX
      STZ   NPTOOL_ARG0,X
@copy:
      DEX
      BMI   @done
      LDA   picker_name,X
      STA   NPTOOL_ARG0,X
      BRA   @copy
@done:
      RTS

; =====================================================================
; Project-wide search: walk the directory hit by hit.
; =====================================================================

search_args:
      JSR   nptool_clear_args
      LDA   #<search_block_name
      STA   LIB_ARG0+0
      LDA   #>search_block_name
      STA   LIB_ARG0+1
      LDA   #search_block_name_end-search_block_name
      STA   LIB_ARG1+0
      LDA   #<search_state
      STA   LIB_ARG1+2
      LDA   #>search_state
      STA   LIB_ARG1+3
      LDA   #search_state_end-search_state
      STA   LIB_ARG2+0
      STZ   LIB_ARG2+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      RTS

search_load:
      JSR   search_args
      LDA   #MEM_NAMED_FETCH
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BEQ   @ok
      STZ   search_needle_len          ; nothing stored yet
      STZ   search_file
      STZ   search_line+0
      STZ   search_line+1
@ok:
      RTS

search_save:
      JSR   search_args
      LDA   #MEM_NAMED_STASH
      STA   LIB_FN_ID
      JMP   LIB_LOADER_BAND

; Ask for the needle, seeded with the previous one.
search_prompt:
      LDY   #0
@seed:
      CPY   search_needle_len
      BCS   @term
      LDA   search_needle,Y
      STA   search_typed,Y
      INY
      BRA   @seed
@term:
      LDA   #0
      STA   search_typed,Y
      LDA   #<search_title
      STA   search_cfg+0
      LDA   #>search_title
      STA   search_cfg+1
      LDA   #<search_label
      STA   search_cfg+2
      LDA   #>search_label
      STA   search_cfg+3
      LDA   #<search_typed
      STA   search_cfg+4
      LDA   #>search_typed
      STA   search_cfg+5
      LDA   #SEARCH_CAP
      STA   search_cfg+6
      LDA   #10
      STA   search_cfg+7               ; left
      LDA   #7
      STA   search_cfg+8               ; top
      LDA   #58
      STA   search_cfg+9               ; width
      LDA   #8
      STA   search_cfg+10              ; height
      STZ   search_cfg+11              ; no footer line
      STZ   search_cfg+12
      JSR   tool_clear_lib_args
      LDA   #<search_cfg
      STA   LIB_ARG0+0
      LDA   #>search_cfg
      STA   LIB_ARG0+1
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      LDA   #SYS_NUI_TEXT_INPUT
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @cancel
      LDA   LIB_RESULT+1
      BNE   @cancel                    ; NUI_RESULT_CANCEL
      LDA   LIB_RESULT+0
      STA   search_typed_len
      BEQ   @cancel                    ; empty needle searches for nothing
      CLC
      RTS
@cancel:
      SEC
      RTS

; A different needle restarts the walk.
search_apply_needle:
      LDA   search_typed_len
      CMP   search_needle_len
      BNE   @fresh
      LDY   #0
@same:
      CPY   search_typed_len
      BCS   @keep
      LDA   search_typed,Y
      CMP   search_needle,Y
      BNE   @fresh
      INY
      BRA   @same
@fresh:
      STZ   search_file
      STZ   search_line+0
      STZ   search_line+1
      LDA   search_typed_len
      STA   search_needle_len
      LDY   #0
@copy:
      CPY   search_typed_len
      BCS   @keep
      LDA   search_typed,Y
      STA   search_needle,Y
      INY
      BRA   @copy
@keep:
      RTS

; Collect the directory's file names up front: the listing handle does not
; survive the file reads the scan needs.
search_collect:
      STZ   search_count
      JSR   nptool_clear_args
      LDA   #FIO_DIR_FLAG_FULLNAME
      STA   FIO_DIRTYPE
      LDA   #FILE_DIR_OPEN
      JSR   nptool_files_call
      BNE   @done
@read:
      LDA   search_count
      CMP   #SEARCH_FILES
      BCS   @done
      JSR   nptool_clear_args
      LDA   #FILE_DIR_READ
      JSR   nptool_files_call
      BNE   @done
      LDA   FIO_DIRTYPE
      CMP   #FIO_TYPE_DIR
      BEQ   @read                      ; nested directories are not searched
      LDA   FIO_NAMELEN
      BEQ   @read
      CMP   #SEARCH_NAME
      BCS   @read                      ; name will not fit a slot
      JSR   search_slot_ptr
      LDY   #0
@name:
      CPY   FIO_NAMELEN
      BCS   @term
      LDA   FIO_NAME,Y
      STA   (search_ptr),Y
      INY
      BRA   @name
@term:
      LDA   #0
      STA   (search_ptr),Y
      INC   search_count
      BRA   @read
@done:
      RTS

; search_ptr = search_names + search_count * SEARCH_NAME
search_slot_ptr:
      LDA   #<search_names
      STA   search_ptr+0
      LDA   #>search_names
      STA   search_ptr+1
      LDX   search_count
      BEQ   @done
@add:
      CLC
      LDA   search_ptr+0
      ADC   #SEARCH_NAME
      STA   search_ptr+0
      BCC   :+
      INC   search_ptr+1
:     DEX
      BNE   @add
@done:
      RTS

; Copy slot search_index into NPTOOL_ARG0 so the loader can read it.
search_select_name:
      LDA   search_count
      PHA
      LDA   search_index
      STA   search_count
      JSR   search_slot_ptr
      PLA
      STA   search_count
      LDY   #0
@copy:
      LDA   (search_ptr),Y
      STA   NPTOOL_ARG0,Y
      BEQ   @done
      INY
      CPY   #SEARCH_NAME
      BCC   @copy
      LDA   #0
      STA   NPTOOL_ARG0,Y
@done:
      STY   NPTOOL_ARG0_LEN
      RTS

; Scan source_buf for the needle. Carry set on a hit, with search_scan_line
; holding its one-based line. `skip` in A is the line to search past.
search_scan_buffer:
      STA   pos_tmp                    ; lines at or below this are already seen
      LDA   #1
      STA   search_scan_line+0
      STZ   search_scan_line+1
      LDX   #0
@byte:
      CPX   NPTOOL_IO_LEN+0
      BNE   :+
      LDA   NPTOOL_IO_LEN+1
      BEQ   @miss
:     LDA   source_buf,X
      CMP   #$0A
      BNE   @try
      INC   search_scan_line+0
      BNE   @next
      INC   search_scan_line+1
      BRA   @next
@try:
      LDY   #0
@match:
      CPY   search_needle_len
      BCS   @hit
      LDA   source_buf,X
      CMP   search_needle,Y
      BNE   @next
      INX
      BEQ   @miss                      ; ran off the buffer mid-match
      INY
      BRA   @match
@hit:
      LDA   search_scan_line+1
      BNE   @accept
      LDA   search_scan_line+0
      CMP   pos_tmp
      BEQ   @next
      BCC   @next
@accept:
      SEC
      RTS
@next:
      INX
      BNE   @byte
@miss:
      CLC
      RTS

; Walk files from the last hit onward. Carry set means NPTOOL_ARG0 now names a
; file and the hook table points at the matching line.
search_walk:
      LDA   search_file
      STA   search_index
@file:
      LDA   search_index
      CMP   search_count
      BCS   @none
      JSR   search_select_name
      JSR   nptool_load_arg0
      BNE   @skip_file
      LDA   search_index
      CMP   search_file
      BNE   @from_top
      LDA   search_line+0               ; same file: continue past the last hit
      BRA   @scan
@from_top:
      LDA   #0
@scan:
      JSR   search_scan_buffer
      BCC   @skip_file
      LDA   search_index
      STA   search_file
      LDA   search_scan_line+0
      STA   search_line+0
      LDA   search_scan_line+1
      STA   search_line+1
      STA   NPTOOL_DIAG_LINE+1
      LDA   search_scan_line+0
      STA   NPTOOL_DIAG_LINE+0
      LDA   #1
      STA   NPTOOL_DIAG_COL+0
      STZ   NPTOOL_DIAG_COL+1
      INC   search_hit
      JSR   search_save
      SEC
      RTS
@skip_file:
      INC   search_index
      STZ   search_line+0               ; a new file starts from its top
      STZ   search_line+1
      LDA   search_index
      STA   search_file
      BRA   @file
@none:
      STZ   search_file
      STZ   search_line+0
      STZ   search_line+1
      JSR   search_save
      CLC
      RTS

; Entry point: prompt, then jump to the next hit. Carry set = reopen ARG0.
tool_search_project:
      JSR   search_load
      JSR   search_prompt
      BCS   @cancel
      JSR   search_apply_needle
      JSR   search_collect
      JSR   search_walk
      BCS   @found
      LDA   #<search_none
      STA   editor_hooks+EDITOR_HOOKS_STATUSL
      LDA   #>search_none
      STA   editor_hooks+EDITOR_HOOKS_STATUSH
@cancel:
      CLC
      RTS
@found:
      STZ   NPTOOL_STATUS              ; probe failures during the scan are not ours
      STZ   NPTOOL_DETAIL
      SEC
      RTS

; =====================================================================
; Remembered caret position, keyed by file name.
; =====================================================================

; Set up ARG0/ARG1 for the named XRAM block that holds the table.
pos_args:
      JSR   nptool_clear_args
      LDA   #<pos_block_name
      STA   LIB_ARG0+0
      LDA   #>pos_block_name
      STA   LIB_ARG0+1
      LDA   #pos_block_name_end-pos_block_name
      STA   LIB_ARG1+0
      LDA   #<pos_table
      STA   LIB_ARG1+2
      LDA   #>pos_table
      STA   LIB_ARG1+3
      LDA   #<POS_TABLE
      STA   LIB_ARG2+0
      LDA   #>POS_TABLE
      STA   LIB_ARG2+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      RTS

; Read the table in. A missing block just leaves the cleared BSS copy, which
; reads as "no remembered positions".
pos_load:
      JSR   pos_args
      LDA   #MEM_NAMED_FETCH
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BEQ   @ok
      LDX   #0                        ; unreadable: start from empty
      TXA
@clear:
      STA   pos_table,X
      INX
      CPX   #POS_TABLE
      BNE   @clear
@ok:
      RTS

pos_save:
      JSR   pos_args
      LDA   #MEM_NAMED_STASH
      STA   LIB_FN_ID
      JMP   LIB_LOADER_BAND

; pos_find: locate the slot whose name matches NPTOOL_ARG0. Carry set = found,
; pos_slot holds its byte offset either way (an empty slot when not found, or
; the last slot when the table is full).
pos_find:
      STZ   pos_slot
      LDX   #0
@slot:
      LDY   #0
@byte:
      LDA   pos_table,X
      CMP   NPTOOL_ARG0,Y
      BNE   @next
      CMP   #0
      BEQ   @hit                      ; both ended together
      INX
      INY
      CPY   #POS_NAME_CAP
      BCC   @byte
      BRA   @hit
@next:
      LDA   pos_slot
      CLC
      ADC   #POS_ENTRY
      STA   pos_slot
      CMP   #POS_TABLE
      BCS   @miss
      TAX
      BRA   @slot
@hit:
      LDA   pos_slot
      TAX
      SEC
      RTS
@miss:
      ; No entry. Reuse the first empty slot, else evict slot zero.
      LDX   #0
      STZ   pos_slot
@empty:
      LDA   pos_table,X
      BEQ   @take
      LDA   pos_slot
      CLC
      ADC   #POS_ENTRY
      STA   pos_slot
      CMP   #POS_TABLE
      BCS   @evict
      TAX
      BRA   @empty
@take:
      LDX   pos_slot
      CLC
      RTS
@evict:
      STZ   pos_slot
      LDX   #0
      CLC
      RTS

; pos_recall: seed the hook table's goto line from the remembered position.
pos_recall:
      JSR   pos_load
      JSR   pos_find
      BCC   @none
      TXA
      CLC
      ADC   #POS_NAME_CAP
      TAX
      LDA   pos_table,X
      STA   editor_hooks+EDITOR_HOOKS_GOTO_LINEL
      LDA   pos_table+1,X
      STA   editor_hooks+EDITOR_HOOKS_GOTO_LINEH
      LDA   #1
      STA   editor_hooks+EDITOR_HOOKS_GOTO_COL
@none:
      RTS

; pos_record: store the caret line the editor reported, then write the table
; back. A zero line means the module could not describe the position.
pos_record:
      LDA   pos_line+0
      ORA   pos_line+1
      BEQ   @skip
      LDA   pos_line+0
      STA   pos_tmp
      LDA   pos_line+1
      PHA
      JSR   pos_find
      TXA
      PHA
      LDY   #0
@name:
      LDA   NPTOOL_ARG0,Y
      STA   pos_table,X
      BEQ   @pad
      INX
      INY
      CPY   #POS_NAME_CAP-1
      BCC   @name
      LDA   #0
      STA   pos_table,X
@pad:
      PLA
      CLC
      ADC   #POS_NAME_CAP
      TAX
      LDA   pos_tmp
      STA   pos_table,X
      PLA
      STA   pos_table+1,X
      JMP   pos_save
@skip:
      RTS

editor_command_hook:
      CMP   #EDITUI_CMD_OPEN
      BEQ   @open
      CMP   #EDITUI_CMD_BUILD
      BEQ   @open
      CMP   #EDITUI_CMD_RUN
      BEQ   @open
      CMP   #EDITUI_CMD_SEARCH_PROJECT
      BEQ   @open
      CMP   #EDITUI_CMD_BUFFER_LIST
      BNE   @ignored
@open:
      STA   editor_command_action
      LDA   #EDITBUF_EXIT_COMMAND
      STA   EDITOR_HOOK_ABI_RESULT
      SEC
      RTS
@ignored:
      CLC
      RTS

editor_set_type:
      STA   editor_type_ptr+0
      STX   editor_type_ptr+1
      RTS

editor_select_type:
      STZ   editor_hooks+EDITOR_HOOKS_HILITE_VECL
      STZ   editor_hooks+EDITOR_HOOKS_HILITE_VECH
      LDA   #<editor_type_text
      LDX   #>editor_type_text
      JSR   editor_set_type
      LDX   NPTOOL_ARG0_LEN
      CPX   #2
      BCC   @done
      DEX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BNE   @three
      DEX
      LDA   NPTOOL_ARG0,X
      CMP   #'.'
      BNE   @three
      JMP   @assembly
@three:
      LDX   NPTOOL_ARG0_LEN
      CPX   #4
      BCC   @done
      DEX
      DEX
      DEX
      DEX
      LDA   NPTOOL_ARG0,X
      CMP   #'.'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BEQ   @check_pascal
      CMP   #'N'
      BEQ   @check_project
      CMP   #'A'
      BEQ   @check_asm
      CMP   #'I'
      BEQ   @check_inc
      CMP   #'C'
      BEQ   @check_cfg
      CMP   #'L'
      BEQ   @check_lnk
@done:
      RTS
@check_lnk:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'N'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'K'
      BEQ   @linker
      BRA   @done
@check_cfg:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'F'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'G'
      BEQ   @linker
      BRA   @done
@check_pascal:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'A'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BEQ   @pascal
      BRA   @done
@check_project:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BEQ   @project
      BRA   @done
@check_asm:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'M'
      BEQ   @assembly
      BRA   @done
@check_inc:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'N'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'C'
      BEQ   @assembly
      JMP   @done
@linker:
      LDA   #<editor_type_linker
      LDX   #>editor_type_linker
      JMP   editor_set_type
@pascal:
      LDA   #<editor_pascal_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECL
      LDA   #>editor_pascal_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECH
      LDA   #<editor_type_pascal
      LDX   #>editor_type_pascal
      JMP   editor_set_type
@project:
      LDA   #<editor_type_project
      LDX   #>editor_type_project
      JMP   editor_set_type
@assembly:
      LDA   #<editor_type_assembly
      LDX   #>editor_type_assembly
      JMP   editor_set_type

; Pascal lexer hook. The shared editor owns rendering; NPEDIT only assigns
; lexical colors for Pascal identifiers, literals, strings, and comments.
editor_pascal_hilite:
      LDY   #0
@next:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #$7B
      long_beq @brace_comment
      CMP   #'('
      BEQ   @maybe_paren_comment
      CMP   #'/'
      BEQ   @maybe_line_comment
      CMP   #$27
      long_beq @string
      CMP   #'$'
      long_beq @number
      JSR   editor_pascal_is_digit
      long_bcs @number
      JSR   editor_pascal_is_alpha
      long_bcs @word
      INY
      BRA   @next

@maybe_line_comment:
      INY
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'/'
      BNE   @single_char
      DEY
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      JMP   editor_pascal_color_to_eol

@maybe_paren_comment:
      INY
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'*'
      BNE   @single_char
      DEY
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      STA   editor_hl_color
@paren_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #'*'
      BNE   @paren_loop
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'/'
      BNE   @paren_loop
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      JMP   @next

@single_char:
      DEY
      INY
      JMP   @next

@brace_comment:
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      STA   editor_hl_color
@brace_loop:
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #'}'
      long_beq @next
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCC   @brace_loop
      RTS

@string:
      LDA   #EDITOR_PASCAL_COLOR_STRING
      STA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
@string_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #$27
      BNE   @string_loop
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #$27
      long_bne @next
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @string_loop

@word:
      LDA   #EDITOR_PASCAL_COLOR_WORD
      STA   editor_hl_color
@word_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      JSR   editor_pascal_is_ident
      long_bcc @next
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @word_loop

@number:
      LDA   #EDITOR_PASCAL_COLOR_NUMBER
      STA   editor_hl_color
@number_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'$'
      BEQ   @number_color
      JSR   editor_pascal_is_ident
      long_bcc @next
@number_color:
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @number_loop
@done:
      RTS

editor_pascal_color_to_eol:
      STA   editor_hl_color
@loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

editor_pascal_is_ident:
      CMP   #'_'
      BEQ   @yes
      JSR   editor_pascal_is_digit
      BCS   @yes
      JMP   editor_pascal_is_alpha
@yes:
      SEC
      RTS

editor_pascal_is_alpha:
      CMP   #'A'
      BCC   @lower
      CMP   #'Z' + 1
      BCC   @yes
@lower:
      CMP   #'a'
      BCC   @no
      CMP   #'z' + 1
      BCC   @yes
@yes:
      SEC
      RTS
@no:
      CLC
      RTS
editor_pascal_is_digit:
      CMP   #'0'
      BCC   @no
      CMP   #'9' + 1
      BCS   @no
      SEC
      RTS
@no:
      CLC
      RTS

      .segment "RODATA"
EDITOR_PASCAL_COLOR_WORD    = $63
EDITOR_PASCAL_COLOR_STRING  = $65
EDITOR_PASCAL_COLOR_NUMBER  = $67
EDITOR_PASCAL_COLOR_COMMENT = $6C
search_block_name:
      .byte "__NPED.SRC"
search_block_name_end:
search_title:
      .byte "Search project", 0
search_label:
      .byte "Find:", 0
search_none:
      .byte "No more matches in this directory", 0

pos_block_name:
      .byte "__NPED.POS"
pos_block_name_end:

editor_build_failed:
      .byte "Build failed - cursor is on the error", 0
editor_type_text:     .byte "Text", 0
editor_type_pascal:   .byte "Pascal Source", 0
editor_type_project:  .byte "Pascal Project", 0
editor_type_assembly: .byte "Assembly Source", 0
editor_type_linker:   .byte "Linker Config", 0
picker_title:         .byte "Open Text File", 0
picker_footer:        .byte "Enter Open   Esc Cancel", 0
picker_config:
      .word picker_title
      .word picker_name
      .byte PICKER_NAME_CAP, NUI_FILE_MODE_OPEN, 0, 0, 0, 0, 0, 0
      .word picker_footer
      .word picker_rows
