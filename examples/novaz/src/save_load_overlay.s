; NovaZ save/load UI and file-stream overlay.
;
; The base runtime owns Z-machine opcode return semantics. This overlay owns the
; user-facing slot picker plus the native .NZS file serialization.

.setcpu "w65c02"

.include "fio.inc"
.include "xram.inc"
.include "libsystem.inc"
.include "libmemory.inc"
.include "nui.inc"
.include "runtime_abi.inc"
.include "zstory.inc"
.include "zvm6.inc"

SAVE_LOAD_ROW_WIDTH = 24
SAVE_LOAD_ROW_COUNT = 10

SAVE_LOAD_MODE_SAVE    = $00
SAVE_LOAD_MODE_RESTORE = $01

SAVE_LOAD_OK      = $00
SAVE_LOAD_ERROR   = $01
SAVE_LOAD_INVALID = $02

ZVM_SAVE_FLAG_VTEXT = $01
ZVM_SAVE_FLAG_NZ6   = $02
ZVM_SAVE_FLAG_TEXT  = $04

SAVE_LOAD_DIALOG_LEFT   = 26
SAVE_LOAD_DIALOG_TOP    = 13
SAVE_LOAD_DIALOG_WIDTH  = 28
SAVE_LOAD_DIALOG_HEIGHT = 16

; NUI chrome shadows extend outside the dialog box by a few graphics pixels.
; Save one extra text cell right/down so restore-under also clears the shadow.
SAVE_LOAD_UNDER_LEFT   = SAVE_LOAD_DIALOG_LEFT
SAVE_LOAD_UNDER_TOP    = SAVE_LOAD_DIALOG_TOP
SAVE_LOAD_UNDER_WIDTH  = SAVE_LOAD_DIALOG_WIDTH + 1
SAVE_LOAD_UNDER_HEIGHT = SAVE_LOAD_DIALOG_HEIGHT + 1

SAVE_LOAD_DESC_COL  = SAVE_LOAD_DIALOG_LEFT + 2
SAVE_LOAD_DESC_ROW  = SAVE_LOAD_DIALOG_TOP + 4
SAVE_LOAD_DESC_ADDR = SAVE_LOAD_DESC_ROW * VTEXT_SCREEN_COLS + SAVE_LOAD_DESC_COL

ZVM_SAVE_HEADER_SIZE       = $40
ZVM_SAVE_PREFIX_SIZE       = $06
ZVM_SAVE_FORMAT_VERSION    = $0A
ZVM_SAVE_MAGIC0            = $00
ZVM_SAVE_MAGIC1            = $01
ZVM_SAVE_MAGIC2            = $02
ZVM_SAVE_MAGIC3            = $03
ZVM_SAVE_HDR_SIZE          = $04
ZVM_SAVE_FORMAT            = $05
ZVM_SAVE_FLAGS             = $06
ZVM_SAVE_DESC_LEN          = $07
ZVM_SAVE_DESC              = $08
ZVM_SAVE_DESC_MAX          = $14
ZVM_SAVE_STORY_VERSION     = $22
ZVM_SAVE_RELEASE_HI        = $23
ZVM_SAVE_RELEASE_LO        = $24
ZVM_SAVE_SERIAL            = $25
ZVM_SAVE_CHECKSUM_HI       = $2B
ZVM_SAVE_CHECKSUM_LO       = $2C
ZVM_SAVE_DYNAMIC_SIZE_LO   = $2D
ZVM_SAVE_DYNAMIC_SIZE_HI   = $2E
ZVM_SAVE_STATE_SIZE_LO     = $2F
ZVM_SAVE_STATE_SIZE_HI     = $30
ZVM_SAVE_DYNAMIC_OFFSET_L  = $31
ZVM_SAVE_DYNAMIC_OFFSET_M  = $32
ZVM_SAVE_DYNAMIC_OFFSET_H  = $33
ZVM_SAVE_STATE_OFFSET_L    = $34
ZVM_SAVE_STATE_OFFSET_M    = $35
ZVM_SAVE_STATE_OFFSET_H    = $36
ZVM_SAVE_RETURN_KIND       = $37
ZVM_SAVE_STORE_VAR         = $38
ZVM_SAVE_BRANCH_IF         = $39
ZVM_SAVE_BRANCH_OFF_H      = $3A
ZVM_SAVE_BRANCH_OFF_L      = $3B
ZVM_SAVE_PC_B              = $3C
ZVM_SAVE_PC_H              = $3D
ZVM_SAVE_PC_L              = $3E
ZVM_SAVE_DESC_DEFAULT_LEN  = $0A
ZVM_SAVE_STATE_SIZE        = zvm_state_end - zvm_state_start
ZVM_SAVE_VTEXT_STATE_SIZE  = VTEXT_FLAGS - VTEXT_LEFT + 1
ZVM_SAVE_NZ6_STATE_SIZE    = NZ6_SAVE_STATE_SIZE
ZVM_SAVE_TEXT_BYTES        = VTEXT_SCREEN_COLS * VTEXT_SCREEN_ROWS
ZVM_SAVE_TEXT_SCRATCH_SIZE = ZVM_SAVE_TEXT_BYTES

SAVE_LOAD_DESC_FIELD_WIDTH = ZVM_SAVE_DESC_MAX
SAVE_LOAD_DESC_CURSOR_MAX  = SAVE_LOAD_DESC_FIELD_WIDTH - 1
SAVE_LOAD_DIALOG_SHADOW = $00
SAVE_LOAD_DIALOG_BORDER = $0F
SAVE_LOAD_DIALOG_PANEL  = $0C
SAVE_LOAD_DIALOG_TEXT   = $CD

.segment "BSS"
save_load_file_id:  .res 1
save_load_tmp:      .res 1
save_load_status:   .res 1
save_load_selected: .res 1
save_load_command:  .res 1
save_load_under_config: .res 16
save_load_picker_config: .res 13
save_load_rows: .res SAVE_LOAD_ROW_WIDTH * SAVE_LOAD_ROW_COUNT
save_load_slot_map: .res SAVE_LOAD_ROW_COUNT
save_load_row_count: .res 1
save_load_row_offset: .res 1
save_load_slot_index: .res 1
save_load_desc: .res ZVM_SAVE_DESC_MAX
save_load_desc_len: .res 1
save_load_desc_offset: .res 1
save_load_saved_vtext: .res ZVM_SAVE_VTEXT_STATE_SIZE
save_load_text_xaddr_l: .res 1
save_load_text_xaddr_m: .res 1
save_load_text_xaddr_h: .res 1
save_load_text_scratch_valid: .res 1

.segment "CODE"

.export novaz_save_load_overlay_main

novaz_save_load_overlay_main:
        LDA #SAVE_LOAD_ERROR
        STA zvm_save_result

        JSR save_load_capture_vtext
        JSR save_load_set_dialog_style
        JSR save_load_pick_slot
        BEQ :+
        JMP @done
:
        LDA zvm_save_overlay_mode
        CMP #SAVE_LOAD_MODE_RESTORE
        BEQ @restore

        JSR save_load_prompt_description
        BEQ :+
        JMP @done
:
        JSR save_load_save_game
        BRA @store_result

@restore:
        JSR save_load_restore_game

@store_result:
        STA zvm_save_result
        CMP #SAVE_LOAD_INVALID
        BNE @done
        JSR save_load_show_invalid_restore

@done:
        LDA zvm_save_overlay_mode
        CMP #SAVE_LOAD_MODE_RESTORE
        BNE @restore_entry_vtext
        LDA zvm_save_result
        BEQ @return
@restore_entry_vtext:
        JSR save_load_restore_entry_vtext
@return:
        LDA #$00
        RTS

save_load_capture_vtext:
        LDX #$00
@loop:
        LDA VTEXT_LEFT,X
        STA save_load_saved_vtext,X
        INX
        CPX #ZVM_SAVE_VTEXT_STATE_SIZE
        BCC @loop
        RTS

save_load_restore_entry_vtext:
        LDX #$00
@loop:
        LDA save_load_saved_vtext,X
        STA VTEXT_LEFT,X
        INX
        CPX #ZVM_SAVE_VTEXT_STATE_SIZE
        BCC @loop
        JMP vtext_set_cursor

save_load_pick_slot:
        JSR save_load_save_under
        BNE @cancel
        JSR save_load_show_loading
        JSR save_load_prepare_picker
        BEQ @picker_ready
        JSR save_load_show_no_saves
        JSR save_load_restore_under
        BRA @cancel

@picker_ready:
        LDA #<save_load_picker_config
        STA LIB_ARG0
        LDA #>save_load_picker_config
        STA LIB_ARG0 + 1
        STZ LIB_ARG0 + 2
        STZ LIB_ARG0 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_NUI_PICK_LIST
        JSR save_load_lib_call
        STA save_load_status
        LDA LIB_RESULT
        STA save_load_selected
        LDA LIB_RESULT + 1
        STA save_load_command

        JSR save_load_restore_under

        LDA save_load_status
        BNE @cancel
        LDA save_load_command
        CMP #NUI_RESULT_OK
        BNE @cancel
        LDA save_load_selected
        CMP save_load_row_count
        BCS @cancel
        TAX
        LDA save_load_slot_map,X
        STA zvm_save_slot
        LDA #$00
        RTS

@cancel:
        LDA #$FF
        STA zvm_save_slot
        LDA #$01
        RTS

save_load_prepare_picker:
        JSR save_load_init_picker_config
        LDA zvm_save_overlay_mode
        CMP #SAVE_LOAD_MODE_RESTORE
        BEQ @restore
        JSR save_load_build_save_rows
        BRA @check
@restore:
        JSR save_load_build_restore_rows
@check:
        LDA save_load_row_count
        BEQ @empty
        LDA save_load_selected
        CMP save_load_row_count
        BCC @ok
        STZ save_load_selected
@ok:
        LDA save_load_row_count
        STA save_load_picker_config + 5
        LDA save_load_selected
        STA save_load_picker_config + 6
        LDA #$00
        RTS
@empty:
        LDA #$01
        RTS

save_load_init_picker_config:
        LDA zvm_save_overlay_mode
        CMP #SAVE_LOAD_MODE_RESTORE
        BEQ @restore
        LDA #<save_load_title_save
        STA save_load_picker_config + 0
        LDA #>save_load_title_save
        STA save_load_picker_config + 1
        BRA @title_done
@restore:
        LDA #<save_load_title_restore
        STA save_load_picker_config + 0
        LDA #>save_load_title_restore
        STA save_load_picker_config + 1
@title_done:
        LDA #<save_load_rows
        STA save_load_picker_config + 2
        LDA #>save_load_rows
        STA save_load_picker_config + 3
        LDA #SAVE_LOAD_ROW_WIDTH
        STA save_load_picker_config + 4
        STZ save_load_picker_config + 5
        STZ save_load_picker_config + 6
        LDA #SAVE_LOAD_DIALOG_LEFT
        STA save_load_picker_config + 7
        LDA #SAVE_LOAD_DIALOG_TOP
        STA save_load_picker_config + 8
        LDA #SAVE_LOAD_DIALOG_WIDTH
        STA save_load_picker_config + 9
        LDA #SAVE_LOAD_DIALOG_HEIGHT
        STA save_load_picker_config + 10
        LDA #<save_load_footer
        STA save_load_picker_config + 11
        LDA #>save_load_footer
        STA save_load_picker_config + 12
        RTS

save_load_build_save_rows:
        STZ save_load_row_count
        STZ save_load_slot_index
@loop:
        LDA save_load_slot_index
        CMP #SAVE_LOAD_ROW_COUNT
        BCS @done
        JSR save_load_scan_slot_header
        PHP
        LDA save_load_row_count
        JSR save_load_set_row_offset
        JSR save_load_clear_row
        LDA save_load_slot_index
        JSR save_load_write_slot_prefix
        PLP
        BNE @empty
        JSR save_load_copy_header_desc_to_row
        BRA @store
@empty:
        JSR save_load_copy_empty_to_row
@store:
        LDX save_load_row_count
        LDA save_load_slot_index
        STA save_load_slot_map,X
        INC save_load_row_count
        INC save_load_slot_index
        BRA @loop
@done:
        RTS

save_load_build_restore_rows:
        STZ save_load_row_count
        STZ save_load_slot_index
@loop:
        LDA save_load_slot_index
        CMP #SAVE_LOAD_ROW_COUNT
        BCS @done
        JSR save_load_scan_slot_header
        BNE @next
        LDA save_load_row_count
        JSR save_load_set_row_offset
        JSR save_load_clear_row
        LDA save_load_slot_index
        JSR save_load_write_slot_prefix
        JSR save_load_copy_header_desc_to_row
        LDX save_load_row_count
        LDA save_load_slot_index
        STA save_load_slot_map,X
        INC save_load_row_count
@next:
        INC save_load_slot_index
        BRA @loop
@done:
        RTS

save_load_scan_slot_header:
        STA zvm_save_slot
        JSR save_load_open_file
        BNE @done
        JSR save_load_read_header
        BNE @close_error
        JSR save_load_validate_header
        STA save_load_status
        JSR save_load_close_file
        LDA save_load_status
        RTS
@close_error:
        PHA
        JSR save_load_close_file
        PLA
@done:
        RTS

save_load_set_row_offset:
        STA save_load_row_offset
        ASL
        ASL
        ASL
        STA save_load_tmp
        LDA save_load_row_offset
        ASL
        ASL
        ASL
        ASL
        CLC
        ADC save_load_tmp
        STA save_load_row_offset
        RTS

save_load_clear_row:
        LDX save_load_row_offset
        LDY #SAVE_LOAD_ROW_WIDTH
        LDA #' '
@loop:
        STA save_load_rows,X
        INX
        DEY
        BNE @loop
        RTS

save_load_write_slot_prefix:
        ORA #'0'
        LDX save_load_row_offset
        STA save_load_rows,X
        INX
        LDA #' '
        STA save_load_rows,X
        INX
        STA save_load_rows,X
        RTS

save_load_copy_header_desc_to_row:
        LDX save_load_row_offset
        INX
        INX
        INX
        LDY #$00
@loop:
        LDA zvm_save_header + ZVM_SAVE_DESC,Y
        STA save_load_rows,X
        INX
        INY
        CPY #ZVM_SAVE_DESC_MAX
        BCC @loop
        RTS

save_load_copy_empty_to_row:
        LDX save_load_row_offset
        INX
        INX
        INX
        LDY #$00
@loop:
        LDA save_load_empty_desc,Y
        STA save_load_rows,X
        INX
        INY
        CPY #ZVM_SAVE_DESC_MAX
        BCC @loop
        RTS

save_load_save_under:
        JSR save_load_set_under_args
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_NUI_SAVE_UNDER_FULL
        JMP save_load_lib_call

save_load_restore_under:
        JSR save_load_set_under_args
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_NUI_RESTORE_UNDER_FULL
        JMP save_load_lib_call

save_load_set_dialog_style:
        LDA #SAVE_LOAD_DIALOG_SHADOW
        STA LIB_ARG0 + 0
        LDA #SAVE_LOAD_DIALOG_BORDER
        STA LIB_ARG0 + 1
        LDA #SAVE_LOAD_DIALOG_PANEL
        STA LIB_ARG0 + 2
        LDA #SAVE_LOAD_DIALOG_TEXT
        STA LIB_ARG0 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_NUI_SET_STYLE
        JMP save_load_lib_call

save_load_show_loading:
        LDA zvm_save_overlay_mode
        CMP #SAVE_LOAD_MODE_RESTORE
        BEQ @restore
        LDA #<save_load_title_save
        STA LIB_ARG0
        LDA #>save_load_title_save
        STA LIB_ARG0 + 1
        BRA @title_done
@restore:
        LDA #<save_load_title_restore
        STA LIB_ARG0
        LDA #>save_load_title_restore
        STA LIB_ARG0 + 1
@title_done:
        LDA #<save_load_loading
        STA LIB_ARG1
        LDA #>save_load_loading
        STA LIB_ARG1 + 1
        LDA #<save_load_blank_footer
        STA LIB_ARG2
        LDA #>save_load_blank_footer
        STA LIB_ARG2 + 1
        LDA #SAVE_LOAD_DIALOG_LEFT
        STA LIB_ARG3 + 0
        LDA #SAVE_LOAD_DIALOG_TOP
        STA LIB_ARG3 + 1
        LDA #SAVE_LOAD_DIALOG_WIDTH
        STA LIB_ARG3 + 2
        LDA #$07
        STA LIB_ARG3 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_DIALOG
        JMP save_load_lib_call

save_load_set_under_args:
        LDA #SAVE_LOAD_UNDER_LEFT
        STA save_load_under_config + 0
        LDA #SAVE_LOAD_UNDER_TOP
        STA save_load_under_config + 1
        LDA #SAVE_LOAD_UNDER_WIDTH
        STA save_load_under_config + 2
        LDA #SAVE_LOAD_UNDER_HEIGHT
        STA save_load_under_config + 3
        LDA zstory_cache_base_l
        STA save_load_under_config + 4
        STA save_load_under_config + 7
        STA save_load_under_config + 10
        STA save_load_under_config + 13
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_SAVE_CHAR_OFF_M
        STA save_load_under_config + 5
        LDA zstory_cache_base_h
        ADC #$00
        STA save_load_under_config + 6
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_SAVE_COLOR_OFF_M
        STA save_load_under_config + 8
        LDA zstory_cache_base_h
        ADC #$00
        STA save_load_under_config + 9
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_SAVE_ATTR_OFF_M
        STA save_load_under_config + 11
        LDA zstory_cache_base_h
        ADC #$00
        STA save_load_under_config + 12
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_SAVE_GFX_OFF_M
        STA save_load_under_config + 14
        LDA zstory_cache_base_h
        ADC #$00
        STA save_load_under_config + 15
        LDA #<save_load_under_config
        STA LIB_ARG0 + 0
        LDA #>save_load_under_config
        STA LIB_ARG0 + 1
        STZ LIB_ARG0 + 2
        STZ LIB_ARG0 + 3
        RTS

save_load_show_invalid_restore:
        JSR save_load_save_under
        LDA #<save_load_error_title
        STA LIB_ARG0
        LDA #>save_load_error_title
        STA LIB_ARG0 + 1
        LDA #<save_load_error_invalid
        STA LIB_ARG1
        LDA #>save_load_error_invalid
        STA LIB_ARG1 + 1
        LDA #<save_load_error_footer
        STA LIB_ARG2
        LDA #>save_load_error_footer
        STA LIB_ARG2 + 1
        LDA #SAVE_LOAD_UNDER_LEFT
        STA LIB_ARG3 + 0
        LDA #SAVE_LOAD_UNDER_TOP
        STA LIB_ARG3 + 1
        LDA #SAVE_LOAD_DIALOG_WIDTH
        STA LIB_ARG3 + 2
        LDA #$07
        STA LIB_ARG3 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_DIALOG_ERROR
        JSR save_load_lib_call
        JMP save_load_restore_under

save_load_show_no_saves:
        LDA #<save_load_title_restore
        STA LIB_ARG0
        LDA #>save_load_title_restore
        STA LIB_ARG0 + 1
        LDA #<save_load_no_saves
        STA LIB_ARG1
        LDA #>save_load_no_saves
        STA LIB_ARG1 + 1
        LDA #<save_load_error_footer
        STA LIB_ARG2
        LDA #>save_load_error_footer
        STA LIB_ARG2 + 1
        LDA #SAVE_LOAD_DIALOG_LEFT
        STA LIB_ARG3 + 0
        LDA #SAVE_LOAD_DIALOG_TOP
        STA LIB_ARG3 + 1
        LDA #SAVE_LOAD_DIALOG_WIDTH
        STA LIB_ARG3 + 2
        LDA #$07
        STA LIB_ARG3 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_DIALOG_ERROR
        JMP save_load_lib_call

save_load_prompt_description:
        JSR save_load_init_description
        JSR save_load_save_under
        BEQ :+
        JMP @cancel
:
        LDA #<save_load_desc_title
        STA LIB_ARG0
        LDA #>save_load_desc_title
        STA LIB_ARG0 + 1
        LDA #<save_load_desc_msg
        STA LIB_ARG1
        LDA #>save_load_desc_msg
        STA LIB_ARG1 + 1
        LDA #<save_load_footer
        STA LIB_ARG2
        LDA #>save_load_footer
        STA LIB_ARG2 + 1
        LDA #SAVE_LOAD_DIALOG_LEFT
        STA LIB_ARG3 + 0
        LDA #SAVE_LOAD_DIALOG_TOP
        STA LIB_ARG3 + 1
        LDA #SAVE_LOAD_DIALOG_WIDTH
        STA LIB_ARG3 + 2
        LDA #$09
        STA LIB_ARG3 + 3
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_DIALOG
        JSR save_load_lib_call
        BEQ :+
        JMP @restore_cancel
:
        JSR save_load_draw_description
@key:
        LDA #MODULE_ID_SYSTEM
        LDX #SYS_WAIT_KEY
        JSR save_load_lib_call
        BEQ :+
        JMP @restore_cancel
:
        LDA LIB_RESULT
        CMP #NUI_KEY_ENTER
        BNE :+
        JMP @ok
:
        CMP #NUI_KEY_ESCAPE
        BNE :+
        JMP @restore_cancel
:
        CMP #$08
        BEQ @backspace
        CMP #$7F
        BEQ @backspace
        CMP #$20
        BCC @key
        CMP #$7F
        BCS @key
        STA save_load_tmp
        LDX save_load_desc_len
        CPX #ZVM_SAVE_DESC_MAX
        BCS @key
        LDA save_load_desc_offset
        PHA
        LDA save_load_tmp
        STA save_load_desc,X
        INC save_load_desc_len
        JSR save_load_calc_desc_offset
        PLA
        CMP save_load_desc_offset
        BNE @redraw
        LDA save_load_desc_len
        SEC
        SBC #$01
        SEC
        SBC save_load_desc_offset
        TAX
        LDA save_load_tmp
        JSR save_load_write_description_cell
        JSR save_load_sync_description_cursor
        JMP @key
@backspace:
        LDX save_load_desc_len
        BEQ @key
        LDA save_load_desc_offset
        PHA
        DEX
        STX save_load_desc_len
        LDA #' '
        STA save_load_desc,X
        JSR save_load_calc_desc_offset
        PLA
        CMP save_load_desc_offset
        BNE @redraw
        LDA save_load_desc_len
        SEC
        SBC save_load_desc_offset
        TAX
        LDA #' '
        JSR save_load_write_description_cell
        JSR save_load_sync_description_cursor
        JMP @key
@redraw:
        JSR save_load_draw_description
        JMP @key
@ok:
        LDA save_load_desc_len
        BNE @restore_ok
        JSR save_load_use_default_desc
@restore_ok:
        JSR save_load_restore_under
        LDA #$00
        RTS
@restore_cancel:
        JSR save_load_restore_under
@cancel:
        LDA #$01
        RTS

save_load_init_description:
        JSR save_load_clear_description
        LDA zvm_save_slot
        JSR save_load_scan_slot_header
        BNE @done
        JSR save_load_copy_header_desc_to_desc
@done:
        RTS

save_load_clear_description:
        STZ save_load_desc_len
        LDX #$00
        LDA #' '
@loop:
        STA save_load_desc,X
        INX
        CPX #ZVM_SAVE_DESC_MAX
        BCC @loop
        RTS

save_load_copy_header_desc_to_desc:
        LDA zvm_save_header + ZVM_SAVE_DESC_LEN
        CMP #ZVM_SAVE_DESC_MAX + 1
        BCC :+
        LDA #ZVM_SAVE_DESC_MAX
:
        STA save_load_desc_len
        LDX #$00
@loop:
        LDA zvm_save_header + ZVM_SAVE_DESC,X
        STA save_load_desc,X
        INX
        CPX #ZVM_SAVE_DESC_MAX
        BCC @loop
        RTS

save_load_use_default_desc:
        LDA #ZVM_SAVE_DESC_DEFAULT_LEN
        STA save_load_desc_len
        LDX #$00
@loop:
        LDA save_load_default_desc,X
        STA save_load_desc,X
        INX
        CPX #ZVM_SAVE_DESC_MAX
        BCC @loop
        RTS

save_load_draw_description:
        JSR save_load_calc_desc_offset
        LDA #VGC_PLANE_CHAR
        STA VGC_VPLANE
        LDA #<SAVE_LOAD_DESC_ADDR
        STA VGC_VADDRL
        LDA #>SAVE_LOAD_DESC_ADDR
        STA VGC_VADDRH
        LDA #VGC_VRAM_AUTOINC
        STA VGC_VCTRL
        LDX #$00
@loop:
        TXA
        CLC
        ADC save_load_desc_offset
        TAY
        CPY save_load_desc_len
        BCS @space
        LDA save_load_desc,Y
        BRA @write
@space:
        LDA #' '
@write:
        STA VGC_VDATA
        INX
        CPX #SAVE_LOAD_DESC_FIELD_WIDTH
        BCC @loop
        STZ VGC_VCTRL
        JMP save_load_sync_description_cursor

save_load_calc_desc_offset:
        STZ save_load_desc_offset
        LDA save_load_desc_len
        CMP #(SAVE_LOAD_DESC_FIELD_WIDTH + 1)
        BCC @done
        SEC
        SBC #SAVE_LOAD_DESC_FIELD_WIDTH
        STA save_load_desc_offset
@done:
        RTS

save_load_write_description_cell:
        STA save_load_tmp
        TXA
        CLC
        ADC #<SAVE_LOAD_DESC_ADDR
        STA VGC_VADDRL
        LDA #>SAVE_LOAD_DESC_ADDR
        ADC #$00
        STA VGC_VADDRH
        LDA #VGC_PLANE_CHAR
        STA VGC_VPLANE
        LDA #VGC_VRAM_AUTOINC
        STA VGC_VCTRL
        LDA save_load_tmp
        STA VGC_VDATA
        STZ VGC_VCTRL
        RTS

save_load_sync_description_cursor:
        LDA save_load_desc_len
        SEC
        SBC save_load_desc_offset
        CMP #SAVE_LOAD_DESC_FIELD_WIDTH
        BCC :+
        LDA #SAVE_LOAD_DESC_CURSOR_MAX
:
        CLC
        ADC #SAVE_LOAD_DESC_COL
        STA VGC_CURSX
        LDA #SAVE_LOAD_DESC_ROW
        STA VGC_CURSY
        RTS

save_load_lib_call:
        STA LIB_MOD_ID
        STX LIB_FN_ID
        JSR LIB_LOADER_BAND
        LDA LIB_STATUS
        RTS

save_load_save_game:
        JSR save_load_fill_header
        JSR save_load_alloc_text_scratch
        BNE @done
        JSR save_load_create_file
        BNE @release
        JSR save_load_write_header
        BNE @close
        JSR save_load_write_dynamic_xram
        BNE @close
        JSR save_load_write_vm_state
        BNE @close
        JSR save_load_write_text_state
@close:
        PHA
        JSR save_load_close_file
        STA save_load_tmp
        PLA
        BNE @release
        LDA save_load_tmp
@release:
        PHA
        JSR save_load_release_text_scratch
        PLA
@done:
        RTS

save_load_restore_game:
        JSR save_load_open_file
        BNE @done
        JSR save_load_read_header
        BNE @close
        JSR save_load_validate_header
        BNE @invalid

        JSR save_load_alloc_text_scratch
        BNE @close
        JSR save_load_read_dynamic_xram
        BNE @close
        JSR save_load_read_vm_state
        BNE @close
        JSR save_load_read_text_state
        BNE @close
        JSR save_load_restore_display_after_load
        LDA zvm_save_header + ZVM_SAVE_BRANCH_IF
        STA zvm_branch_if
        LDA zvm_save_header + ZVM_SAVE_BRANCH_OFF_H
        STA zvm_branch_off_h
        LDA zvm_save_header + ZVM_SAVE_BRANCH_OFF_L
        STA zvm_branch_off_l
        LDA zvm_save_header + ZVM_SAVE_PC_B
        STA zvm_pc_b
        LDA zvm_save_header + ZVM_SAVE_PC_H
        STA zvm_pc_h
        LDA zvm_save_header + ZVM_SAVE_PC_L
        STA zvm_pc_l
        LDA #SAVE_LOAD_OK
        BRA @close
@invalid:
        LDA #SAVE_LOAD_INVALID
@close:
        PHA
        JSR save_load_close_file
        STA save_load_tmp
        JSR save_load_release_text_scratch
        PLA
        BNE @done
        LDA save_load_tmp
@done:
        RTS

save_load_restore_display_after_load:
        LDA zstory_version
        CMP #$06
        BEQ @v6
        JMP zvm_restore_display_after_load
@v6:
        LDA #NZ6_OP_RESTORE_DISPLAY
        JSR NZ6_ENTRY
        RTS

save_load_write_header:
        LDA #<zvm_save_header
        STA XRAM_RAML
        LDA #>zvm_save_header
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENH
        JMP save_load_write_file_block

save_load_read_header:
        LDA #<zvm_save_header
        STA XRAM_RAML
        LDA #>zvm_save_header
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENH
        JMP save_load_read_file_block

save_load_write_vm_state:
        LDA #<zvm_state_start
        STA XRAM_RAML
        LDA #>zvm_state_start
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_STATE_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_write_file_block
        BNE @done
        JSR save_load_write_vtext_state
        BNE @done
        JSR save_load_write_nz6_state_if_needed
@done:
        RTS

save_load_read_vm_state:
        LDA #<zvm_state_start
        STA XRAM_RAML
        LDA #>zvm_state_start
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_STATE_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_read_file_block
        BNE @done
        JSR save_load_read_vtext_state
        BNE @done
        JSR save_load_read_nz6_state_if_needed
@done:
        RTS

save_load_write_vtext_state:
        LDA #<save_load_saved_vtext
        STA XRAM_RAML
        LDA #>save_load_saved_vtext
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_VTEXT_STATE_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_VTEXT_STATE_SIZE
        STA XRAM_LENH
        JMP save_load_write_file_block

save_load_read_vtext_state:
        LDA #<VTEXT_LEFT
        STA XRAM_RAML
        LDA #>VTEXT_LEFT
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_VTEXT_STATE_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_VTEXT_STATE_SIZE
        STA XRAM_LENH
        JMP save_load_read_file_block

save_load_write_nz6_state_if_needed:
        LDA zstory_version
        CMP #$06
        BEQ :+
        LDA #SAVE_LOAD_OK
        RTS
:
        LDA #<NZ6_SAVE_DISPLAY_STATE_BASE
        STA XRAM_RAML
        LDA #>NZ6_SAVE_DISPLAY_STATE_BASE
        STA XRAM_RAMH
        LDA #<NZ6_SAVE_DISPLAY_STATE_SIZE
        STA XRAM_LENL
        LDA #>NZ6_SAVE_DISPLAY_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_write_file_block
        BNE @done
        LDA #<NZ6_SAVE_GFX_STATE_BASE
        STA XRAM_RAML
        LDA #>NZ6_SAVE_GFX_STATE_BASE
        STA XRAM_RAMH
        LDA #<NZ6_SAVE_GFX_STATE_SIZE
        STA XRAM_LENL
        LDA #>NZ6_SAVE_GFX_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_write_file_block
@done:
        RTS

save_load_read_nz6_state_if_needed:
        LDA zstory_version
        CMP #$06
        BEQ :+
        LDA #SAVE_LOAD_OK
        RTS
:
        LDA #<NZ6_SAVE_DISPLAY_STATE_BASE
        STA XRAM_RAML
        LDA #>NZ6_SAVE_DISPLAY_STATE_BASE
        STA XRAM_RAMH
        LDA #<NZ6_SAVE_DISPLAY_STATE_SIZE
        STA XRAM_LENL
        LDA #>NZ6_SAVE_DISPLAY_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_read_file_block
        BNE @done
        LDA #<NZ6_SAVE_GFX_STATE_BASE
        STA XRAM_RAML
        LDA #>NZ6_SAVE_GFX_STATE_BASE
        STA XRAM_RAMH
        LDA #<NZ6_SAVE_GFX_STATE_SIZE
        STA XRAM_LENL
        LDA #>NZ6_SAVE_GFX_STATE_SIZE
        STA XRAM_LENH
        JSR save_load_read_file_block
@done:
        RTS

save_load_alloc_text_scratch:
        LDA zstory_cache_base_l
        STA save_load_text_xaddr_l
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_SAVE_TEXT_OFF_M
        STA save_load_text_xaddr_m
        LDA zstory_cache_base_h
        ADC #$00
        STA save_load_text_xaddr_h
        LDA #$01
        STA save_load_text_scratch_valid
        LDA #SAVE_LOAD_OK
        RTS

save_load_release_text_scratch:
        STZ save_load_text_scratch_valid
        LDA #SAVE_LOAD_OK
        RTS

save_load_set_text_scratch_xram:
        LDA save_load_text_xaddr_l
        STA XRAM_ADDRL
        LDA save_load_text_xaddr_m
        STA XRAM_ADDRM
        LDA save_load_text_xaddr_h
        STA XRAM_ADDRH
        RTS

save_load_set_text_len:
        LDA #<ZVM_SAVE_TEXT_BYTES
        STA XRAM_LENL
        LDA #>ZVM_SAVE_TEXT_BYTES
        STA XRAM_LENH
        RTS

save_load_write_text_state:
        LDA #BLT_SPACE_VGC_CHAR
        JSR save_load_write_text_plane
        BNE @done
        LDA #BLT_SPACE_VGC_COLOR
        JSR save_load_write_text_plane
        BNE @done
        LDA #BLT_SPACE_VGC_TEXTATTR
        JSR save_load_write_text_plane
@done:
        RTS

save_load_read_text_state:
        LDA #BLT_SPACE_VGC_CHAR
        JSR save_load_read_text_plane
        BNE @done
        LDA #BLT_SPACE_VGC_COLOR
        JSR save_load_read_text_plane
        BNE @done
        LDA #BLT_SPACE_VGC_TEXTATTR
        JSR save_load_read_text_plane
@done:
        RTS

save_load_write_text_plane:
        STA save_load_tmp
        JSR save_load_set_text_scratch_xram
        LDA save_load_tmp
        JSR save_load_blit_text_plane_to_xram
        BNE @done
        JSR save_load_set_text_scratch_xram
        JSR save_load_set_text_len
        JMP save_load_write_xram_file_block
@done:
        RTS

save_load_read_text_plane:
        STA save_load_tmp
        JSR save_load_set_text_scratch_xram
        JSR save_load_set_text_len
        JSR save_load_read_xram_file_block
        BNE @done
        LDA save_load_tmp
        JMP save_load_blit_xram_to_text_plane
@done:
        RTS

save_load_blit_text_plane_to_xram:
        STA BLT_SRCSPACE
        LDA #BLT_SPACE_XRAM
        STA BLT_DSTSPACE
        STZ BLT_SRCL
        STZ BLT_SRCM
        STZ BLT_SRCH
        LDA save_load_text_xaddr_l
        STA BLT_DSTL
        LDA save_load_text_xaddr_m
        STA BLT_DSTM
        LDA save_load_text_xaddr_h
        STA BLT_DSTH
        LDA #<VTEXT_SCREEN_COLS
        STA BLT_WIDTHL
        STZ BLT_WIDTHH
        LDA #VTEXT_SCREEN_ROWS
        STA BLT_HEIGHTL
        STZ BLT_HEIGHTH
        LDA #<VTEXT_SCREEN_COLS
        STA BLT_SRCSTRL
        STA BLT_DSTSTRL
        LDA #>VTEXT_SCREEN_COLS
        STA BLT_SRCSTRH
        STZ BLT_DSTSTRH
        JMP blitter_start_copy

save_load_blit_xram_to_text_plane:
        STA BLT_DSTSPACE
        LDA #BLT_SPACE_XRAM
        STA BLT_SRCSPACE
        LDA save_load_text_xaddr_l
        STA BLT_SRCL
        LDA save_load_text_xaddr_m
        STA BLT_SRCM
        LDA save_load_text_xaddr_h
        STA BLT_SRCH
        STZ BLT_DSTL
        STZ BLT_DSTM
        STZ BLT_DSTH
        LDA #<VTEXT_SCREEN_COLS
        STA BLT_WIDTHL
        STZ BLT_WIDTHH
        LDA #VTEXT_SCREEN_ROWS
        STA BLT_HEIGHTL
        STZ BLT_HEIGHTH
        LDA #<VTEXT_SCREEN_COLS
        STA BLT_SRCSTRL
        STA BLT_DSTSTRL
        STZ BLT_SRCSTRH
        LDA #>VTEXT_SCREEN_COLS
        STA BLT_DSTSTRH
        JMP blitter_start_copy

save_load_use_slot_name:
        LDA zvm_save_slot
        ORA #'0'
        STA save_load_slot_name + 5
        LDA #<save_load_slot_name
        STA XRAM_NAMEPTR_L
        LDA #>save_load_slot_name
        STA XRAM_NAMEPTR_H
        LDA #save_load_slot_name_end - save_load_slot_name
        STA XRAM_NAMELEN
        RTS

save_load_create_file:
        STZ save_load_file_id
        JSR save_load_use_slot_name
        JSR fio_copy_name
        BNE @done
        LDA #FIO_FILE_ACCESS_WRITE
        STA FIO_DIRTYPE
        JSR fio_fcreate
        BNE @done
        LDA FIO_SRCL
        STA save_load_file_id
        LDA #SAVE_LOAD_OK
@done:
        RTS

save_load_open_file:
        STZ save_load_file_id
        JSR save_load_use_slot_name
        JSR fio_copy_name
        BNE @done
        LDA #FIO_FILE_ACCESS_READ
        STA FIO_DIRTYPE
        JSR fio_fopen
        BNE @done
        LDA FIO_SRCL
        STA save_load_file_id
        LDA #SAVE_LOAD_OK
@done:
        RTS

save_load_close_file:
        LDA save_load_file_id
        BEQ @ok
        STA FIO_SRCL
        STZ FIO_SRCH
        JSR fio_fclose
        PHA
        STZ save_load_file_id
        PLA
        RTS
@ok:
        LDA #SAVE_LOAD_OK
        RTS

save_load_write_file_block:
        LDA save_load_file_id
        STA FIO_SRCL
        STZ FIO_SRCH
        LDA XRAM_RAML
        STA FIO_ENDL
        LDA XRAM_RAMH
        STA FIO_ENDH
        LDA XRAM_LENL
        STA FIO_GLENL
        LDA XRAM_LENH
        STA FIO_GLENH
        LDA #FIO_FILE_TARGET_RAM
        STA FIO_DIRTYPE
        JMP fio_fwrite

save_load_read_file_block:
        LDA save_load_file_id
        STA FIO_SRCL
        STZ FIO_SRCH
        LDA XRAM_RAML
        STA FIO_ENDL
        LDA XRAM_RAMH
        STA FIO_ENDH
        LDA XRAM_LENL
        STA FIO_GLENL
        LDA XRAM_LENH
        STA FIO_GLENH
        LDA #FIO_FILE_TARGET_RAM
        STA FIO_DIRTYPE
        JSR fio_fread
        BNE @done
        LDA FIO_SIZEL
        CMP XRAM_LENL
        BNE @short
        LDA FIO_SIZEH
        CMP XRAM_LENH
        BNE @short
        LDA #SAVE_LOAD_OK
@done:
        RTS
@short:
        LDA #SAVE_LOAD_ERROR
        RTS

save_load_set_dynamic_xram_args:
        LDA zstory_dynamic_base_l
        STA XRAM_ADDRL
        LDA zstory_dynamic_base_m
        STA XRAM_ADDRM
        LDA zstory_dynamic_base_h
        STA XRAM_ADDRH
        LDA zstory_static_lo
        STA XRAM_LENL
        LDA zstory_static_hi
        STA XRAM_LENH
        RTS

save_load_write_dynamic_xram:
        JSR save_load_set_dynamic_xram_args
        JMP save_load_write_xram_file_block

save_load_read_dynamic_xram:
        JSR save_load_set_dynamic_xram_args
        JMP save_load_read_xram_file_block

save_load_write_xram_file_block:
        LDA save_load_file_id
        STA FIO_SRCL
        STZ FIO_SRCH
        LDA XRAM_ADDRL
        STA FIO_GADDRL
        LDA XRAM_ADDRM
        STA FIO_GADDRH
        LDA XRAM_ADDRH
        STA FIO_GSPACE
        LDA XRAM_LENL
        STA FIO_GLENL
        LDA XRAM_LENH
        STA FIO_GLENH
        LDA #FIO_FILE_TARGET_XRAM
        STA FIO_DIRTYPE
        JMP fio_fwrite

save_load_read_xram_file_block:
        LDA save_load_file_id
        STA FIO_SRCL
        STZ FIO_SRCH
        LDA XRAM_ADDRL
        STA FIO_GADDRL
        LDA XRAM_ADDRM
        STA FIO_GADDRH
        LDA XRAM_ADDRH
        STA FIO_GSPACE
        LDA XRAM_LENL
        STA FIO_GLENL
        LDA XRAM_LENH
        STA FIO_GLENH
        LDA #FIO_FILE_TARGET_XRAM
        STA FIO_DIRTYPE
        JSR fio_fread
        BNE @done
        LDA FIO_SIZEL
        CMP XRAM_LENL
        BNE @short
        LDA FIO_SIZEH
        CMP XRAM_LENH
        BNE @short
        LDA #SAVE_LOAD_OK
@done:
        RTS
@short:
        LDA #SAVE_LOAD_ERROR
        RTS

save_load_fill_header:
        LDX #$00
@clear:
        STZ zvm_save_header,X
        INX
        CPX #ZVM_SAVE_HEADER_SIZE
        BCC @clear

        LDX #$00
@prefix:
        LDA save_load_header_prefix,X
        STA zvm_save_header,X
        INX
        CPX #ZVM_SAVE_PREFIX_SIZE
        BCC @prefix

        LDA save_load_desc_len
        STA zvm_save_header + ZVM_SAVE_DESC_LEN
        LDX #$00
@desc:
        LDA save_load_desc,X
        STA zvm_save_header + ZVM_SAVE_DESC,X
        INX
        CPX #ZVM_SAVE_DESC_MAX
        BCC @desc

        JSR save_load_expected_flags
        STA zvm_save_header + ZVM_SAVE_FLAGS
        LDA zstory_static_lo
        STA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_LO
        LDA zstory_static_hi
        STA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_HI
        LDA #<ZVM_SAVE_STATE_SIZE
        STA zvm_save_header + ZVM_SAVE_STATE_SIZE_LO
        LDA #>ZVM_SAVE_STATE_SIZE
        STA zvm_save_header + ZVM_SAVE_STATE_SIZE_HI
        LDA #ZVM_SAVE_HEADER_SIZE
        STA zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_L
        STZ zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_M
        STZ zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_H
        LDA zstory_static_lo
        CLC
        ADC #ZVM_SAVE_HEADER_SIZE
        STA zvm_save_header + ZVM_SAVE_STATE_OFFSET_L
        LDA zstory_static_hi
        ADC #$00
        STA zvm_save_header + ZVM_SAVE_STATE_OFFSET_M
        LDA #$00
        ADC #$00
        STA zvm_save_header + ZVM_SAVE_STATE_OFFSET_H
        LDA zstory_version
        STA zvm_save_header + ZVM_SAVE_STORY_VERSION
        LDA zstory_release_hi
        STA zvm_save_header + ZVM_SAVE_RELEASE_HI
        LDA zstory_release_lo
        STA zvm_save_header + ZVM_SAVE_RELEASE_LO
        LDX #$00
@serial:
        LDA zstory_serial,X
        STA zvm_save_header + ZVM_SAVE_SERIAL,X
        INX
        CPX #$06
        BCC @serial
        LDA zstory_checksum_hi
        STA zvm_save_header + ZVM_SAVE_CHECKSUM_HI
        LDA zstory_checksum_lo
        STA zvm_save_header + ZVM_SAVE_CHECKSUM_LO
        LDA zvm_branch_if
        STA zvm_save_header + ZVM_SAVE_BRANCH_IF
        LDA zvm_branch_off_h
        STA zvm_save_header + ZVM_SAVE_BRANCH_OFF_H
        LDA zvm_branch_off_l
        STA zvm_save_header + ZVM_SAVE_BRANCH_OFF_L
        LDA zvm_pc_b
        STA zvm_save_header + ZVM_SAVE_PC_B
        LDA zvm_pc_h
        STA zvm_save_header + ZVM_SAVE_PC_H
        LDA zvm_pc_l
        STA zvm_save_header + ZVM_SAVE_PC_L
        LDA zvm_save_return_kind
        STA zvm_save_header + ZVM_SAVE_RETURN_KIND
        LDA zvm_save_store_var
        STA zvm_save_header + ZVM_SAVE_STORE_VAR
        RTS

save_load_expected_flags:
        LDA #(ZVM_SAVE_FLAG_VTEXT | ZVM_SAVE_FLAG_TEXT)
        STA save_load_tmp
        LDA zstory_version
        CMP #$06
        BNE :+
        LDA save_load_tmp
        ORA #ZVM_SAVE_FLAG_NZ6
        STA save_load_tmp
:
        LDA save_load_tmp
        RTS

save_load_validate_header:
        LDX #$00
@prefix:
        LDA zvm_save_header,X
        CMP save_load_header_prefix,X
        BNE @bad
        INX
        CPX #ZVM_SAVE_PREFIX_SIZE
        BCC @prefix

        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_LO
        CMP zstory_static_lo
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_HI
        CMP zstory_static_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_STATE_SIZE_LO
        CMP #<ZVM_SAVE_STATE_SIZE
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_STATE_SIZE_HI
        CMP #>ZVM_SAVE_STATE_SIZE
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_DESC_LEN
        CMP #ZVM_SAVE_DESC_MAX + 1
        BCS @bad
        JSR save_load_expected_flags
        CMP zvm_save_header + ZVM_SAVE_FLAGS
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_L
        CMP #ZVM_SAVE_HEADER_SIZE
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_M
        ORA zvm_save_header + ZVM_SAVE_DYNAMIC_OFFSET_H
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_STORY_VERSION
        CMP zstory_version
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_RELEASE_HI
        CMP zstory_release_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_RELEASE_LO
        CMP zstory_release_lo
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_CHECKSUM_HI
        CMP zstory_checksum_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_CHECKSUM_LO
        CMP zstory_checksum_lo
        BNE @bad
        LDX #$00
@serial:
        LDA zvm_save_header + ZVM_SAVE_SERIAL,X
        CMP zstory_serial,X
        BNE @bad
        INX
        CPX #$06
        BCC @serial
        LDA #SAVE_LOAD_OK
        RTS
@bad:
        LDA #SAVE_LOAD_INVALID
        RTS

.segment "RODATA"

save_load_title_save:
        .byte "SAVE GAME", 0

save_load_title_restore:
        .byte "RESTORE GAME", 0

save_load_desc_title:
        .byte "DESCRIPTION", 0

save_load_desc_msg:
        .byte "Description:", 0

save_load_footer:
        .byte "ENTER OK  ESC CANCEL", 0

save_load_error_title:
        .byte "RESTORE FAILED", 0

save_load_error_invalid:
        .byte "Cannot restore: invalid game or version.", 0

save_load_no_saves:
        .byte "No saved games.", 0

save_load_loading:
        .byte "Reading save slots...", 0

save_load_blank_footer:
        .byte 0

save_load_error_footer:
        .byte "PRESS ANY KEY", 0

save_load_slot_name:
        .byte "SAVE00.NZS"
save_load_slot_name_end:

save_load_header_prefix:
        .byte "NZS1", ZVM_SAVE_HEADER_SIZE, ZVM_SAVE_FORMAT_VERSION

save_load_default_desc:
        .byte "Saved game          "

save_load_empty_desc:
        .byte "<empty>             "
