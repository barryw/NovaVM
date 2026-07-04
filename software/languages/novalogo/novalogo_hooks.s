; NovaLogo editor hooks.
;
; Loaded by the Logo runtime into EDITOR_HOOK_WORK ($0800) before it calls the
; shared editor. The editor module is paged in while hooks run, so the hook table
; and nonzero hook bodies must live in low RAM rather than Logo ROM.

      .include "nova.inc"
      .include "libeditor.inc"

EDITOR_HOOK_WORK = $0800

EDITOR_HOOK_COLOR_WORD    = $63
EDITOR_HOOK_COLOR_NUMBER  = $67
EDITOR_HOOK_COLOR_COMMENT = $6C

      .segment "HOOKS"

      .word EDITOR_HOOK_WORK

editor_hooks:
editor_hooks_type:
      .word editor_type
editor_hooks_status:
      .word 0
editor_hooks_save:
      .word 0
editor_hooks_indent:
      .word editor_hook_indent
editor_hooks_hilite:
      .word editor_hook_hilite
editor_hooks_menu:
      .word 0
editor_hooks_command:
      .word 0
editor_hooks_changed:
      .word 0
editor_hooks_end:

.assert editor_hooks_type - editor_hooks = EDITOR_HOOKS_TYPEL, error, "Logo hook type offset drifted"
.assert editor_hooks_status - editor_hooks = EDITOR_HOOKS_STATUSL, error, "Logo hook status offset drifted"
.assert editor_hooks_save - editor_hooks = EDITOR_HOOKS_SAVE_VECL, error, "Logo hook save offset drifted"
.assert editor_hooks_indent - editor_hooks = EDITOR_HOOKS_INDENT_VECL, error, "Logo hook indent offset drifted"
.assert editor_hooks_hilite - editor_hooks = EDITOR_HOOKS_HILITE_VECL, error, "Logo hook hilite offset drifted"
.assert editor_hooks_menu - editor_hooks = EDITOR_HOOKS_MENU_VECL, error, "Logo hook menu offset drifted"
.assert editor_hooks_command - editor_hooks = EDITOR_HOOKS_COMMAND_VECL, error, "Logo hook command offset drifted"
.assert editor_hooks_changed - editor_hooks = EDITOR_HOOKS_CHANGED_VECL, error, "Logo hook changed offset drifted"
.assert editor_hooks_end - editor_hooks = EDITOR_HOOKS_SIZE, error, "Logo hook table size drifted"

editor_type:
      .byte "Logo", 0

editor_hook_code_start:
editor_hook_indent:
      LDA   EDITOR_HOOK_ABI_CURL
      ORA   EDITOR_HOOK_ABI_CURL+1
      BNE   :+
      LDA   #0
      RTS
:     LDA   EDITOR_HOOK_ABI_CURL
      STA   editor_hook_tmp0
      LDA   EDITOR_HOOK_ABI_CURL+1
      STA   editor_hook_tmp1
      LDA   editor_hook_tmp0
      BNE   :+
      DEC   editor_hook_tmp1
:     DEC   editor_hook_tmp0          ; skip inserted newline
      LDA   editor_hook_tmp0
      ORA   editor_hook_tmp1
      BNE   :+
      LDA   #0
      RTS
:     LDA   editor_hook_tmp0
      BNE   :+
      DEC   editor_hook_tmp1
:     DEC   editor_hook_tmp0          ; previous source char before newline
      JSR   editor_hook_load_from_offset
      CMP   #'['
      BNE   :+
      LDA   #2
      RTS
:     LDA   #0
      RTS

editor_hook_hilite:
      LDY   #0
@hl_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCC   :+
      RTS
:     LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #' '
      BEQ   @hl_advance
      CMP   #$09
      BEQ   @hl_advance
      CMP   #';'
      BEQ   @hl_comment
      CMP   #'0'
      BCC   @hl_alpha
      CMP   #'9' + 1
      BCC   @hl_number
@hl_alpha:
      CMP   #'A'
      BCC   @hl_lower
      CMP   #'Z' + 1
      BCC   @hl_word
@hl_lower:
      CMP   #'a'
      BCC   @hl_advance
      CMP   #'z' + 1
      BCC   @hl_word
@hl_advance:
      INY
      BRA   @hl_loop
@hl_word:
      LDA   #EDITOR_HOOK_COLOR_WORD
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @hl_loop
@hl_number:
      LDA   #EDITOR_HOOK_COLOR_NUMBER
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @hl_loop
@hl_comment:
      LDA   #EDITOR_HOOK_COLOR_COMMENT
@hl_comment_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @hl_done
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @hl_comment_loop
@hl_done:
      RTS

editor_hook_load_from_offset:
      LDA   EDITOR_HOOK_ABI_BUFL
      CLC
      ADC   editor_hook_tmp0
      STA   LIB_ZP
      LDA   EDITOR_HOOK_ABI_BUFL+1
      ADC   editor_hook_tmp1
      STA   LIB_ZP+1
      LDY   #0
      LDA   (LIB_ZP),Y
      RTS
editor_hook_code_end:

editor_hook_tmp0:
      .byte 0
editor_hook_tmp1:
      .byte 0

.assert editor_hook_tmp1 + 1 - editor_hooks <= $0100, error, "NovaLogo editor hooks must fit the hook workspace"
