; Nova Game Server shared-runtime integration fixture.
;
; Loaded at $7000 by integration tests and entered through BASIC SYS. The
; fixture exercises frame builders and parsers without requiring a live socket.

.setcpu "w65c02"

.include "gameserver.inc"

RESULT_BASE = $2290
SEND_BUF    = $2300
RECV_BUF    = $2400

      .segment "CODE"

start:
        jmp asm_gameserver_entry

asm_gameserver_entry:
        pha
        phx
        phy
        jsr asm_gameserver_test
        ply
        plx
        pla
        rts

asm_gameserver_test:
        jsr clear_results

        jsr ngs_init
        lda #<SEND_BUF
        ldx #>SEND_BUF
        jsr ngs_set_buffer

        lda #<handle
        ldx #>handle
        ldy #handle_end - handle
        jsr ngs_set_string
        lda #$01
        sta NGS_GAME_PROTO_MAJOR
        stz NGS_GAME_PROTO_MINOR
        jsr ngs_build_hello
        sta RESULT_BASE
        lda NGS_LEN
        sta RESULT_BASE + 1
        lda SEND_BUF
        sta RESULT_BASE + 2
        lda SEND_BUF + 1
        sta RESULT_BASE + 3
        lda SEND_BUF + 2
        sta RESULT_BASE + 4
        lda SEND_BUF + 4
        sta RESULT_BASE + 5
        lda SEND_BUF + 7
        sta RESULT_BASE + 6

        jsr ngs_build_game_list_request
        sta RESULT_BASE + 7
        lda NGS_LEN
        sta RESULT_BASE + 8
        lda SEND_BUF + 1
        sta RESULT_BASE + 9
        lda SEND_BUF + 2
        sta RESULT_BASE + 10

        lda #$01
        sta NGS_GAME_IDL
        stz NGS_GAME_IDH
        lda #$01
        sta NGS_GAME_PROTO_MAJOR
        stz NGS_GAME_PROTO_MINOR
        lda #<table_name
        ldx #>table_name
        ldy #table_name_end - table_name
        jsr ngs_set_string
        jsr ngs_build_table_create
        sta RESULT_BASE + 11
        lda NGS_LEN
        sta RESULT_BASE + 12
        lda SEND_BUF + 1
        sta RESULT_BASE + 13
        lda SEND_BUF + 4
        sta RESULT_BASE + 14
        lda SEND_BUF + 5
        sta RESULT_BASE + 15

        lda #$34
        sta NGS_TABLE_IDL
        lda #$12
        sta NGS_TABLE_IDH
        lda #$07
        sta NGS_ACTION_TYPE
        lda #<action_payload
        ldx #>action_payload
        ldy #action_payload_end - action_payload
        jsr ngs_set_string
        jsr ngs_build_player_action
        sta RESULT_BASE + 16
        lda NGS_LEN
        sta RESULT_BASE + 17
        lda SEND_BUF + 1
        sta RESULT_BASE + 18
        lda SEND_BUF + 4
        sta RESULT_BASE + 19
        lda SEND_BUF + 5
        sta RESULT_BASE + 20
        lda SEND_BUF + 6
        sta RESULT_BASE + 21
        lda SEND_BUF + 7
        sta RESULT_BASE + 22

        jsr seed_welcome_frame
        lda #<RECV_BUF
        ldx #>RECV_BUF
        jsr ngs_set_buffer
        lda #welcome_frame_end - RECV_BUF
        sta NGS_LAST_LEN
        jsr ngs_parse_envelope
        sta RESULT_BASE + 23
        lda NGS_KIND
        sta RESULT_BASE + 24
        lda NGS_RECV_SEQ
        sta RESULT_BASE + 25
        lda NGS_FLAGS
        sta RESULT_BASE + 26
        lda NGS_REMAIN
        sta RESULT_BASE + 27
        jsr ngs_read_u16
        lda NGS_VALUE_L
        sta RESULT_BASE + 28
        lda NGS_VALUE_H
        sta RESULT_BASE + 29
        jsr ngs_read_string
        lda NGS_STRLEN
        sta RESULT_BASE + 30
        lda NGS_STRPTRL
        sta NVR5L
        lda NGS_STRPTRH
        sta NVR5H
        ldy #$00
        lda (NVR5L),Y
        sta RESULT_BASE + 31

        jsr seed_error_frame
        lda #error_frame_end - RECV_BUF
        sta NGS_LAST_LEN
        jsr ngs_parse_envelope
        jsr ngs_parse_error
        sta RESULT_BASE + 32
        lda NGS_ERROR_CODE
        sta RESULT_BASE + 33
        lda NGS_ERROR_TEXTLEN
        sta RESULT_BASE + 34
        lda NGS_ERROR_TEXTPTRL
        sta NVR5L
        lda NGS_ERROR_TEXTPTRH
        sta NVR5H
        ldy #$00
        lda (NVR5L),Y
        sta RESULT_BASE + 35

        lda #$AA
        sta RESULT_BASE + $3F
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$40
        bne @loop
        rts

seed_welcome_frame:
        lda #NGS_SERVER_PROTO_MAJOR
        sta RECV_BUF
        lda #NGS_KIND_WELCOME
        sta RECV_BUF + 1
        lda #$09
        sta RECV_BUF + 2
        lda #NGS_FLAG_RESPONSE
        sta RECV_BUF + 3
        lda #$34
        sta RECV_BUF + 4
        lda #$12
        sta RECV_BUF + 5
        lda #handle_end - handle
        sta RECV_BUF + 6
        ldx #$00
@copy_handle:
        lda handle,x
        sta RECV_BUF + 7,x
        inx
        cpx #handle_end - handle
        bne @copy_handle
        lda #server_name_end - server_name
        sta RECV_BUF + 7 + (handle_end - handle)
        ldx #$00
@copy_server:
        lda server_name,x
        sta RECV_BUF + 8 + (handle_end - handle),x
        inx
        cpx #server_name_end - server_name
        bne @copy_server
welcome_frame_end = RECV_BUF + 8 + (handle_end - handle) + (server_name_end - server_name)
        rts

seed_error_frame:
        lda #NGS_SERVER_PROTO_MAJOR
        sta RECV_BUF
        lda #NGS_KIND_ERROR
        sta RECV_BUF + 1
        lda #$0A
        sta RECV_BUF + 2
        lda #(NGS_FLAG_RESPONSE | NGS_FLAG_ERROR)
        sta RECV_BUF + 3
        lda #NGS_ERRCODE_TIMEOUT
        sta RECV_BUF + 4
        lda #error_text_end - error_text
        sta RECV_BUF + 5
        ldx #$00
@copy_error:
        lda error_text,x
        sta RECV_BUF + 6,x
        inx
        cpx #error_text_end - error_text
        bne @copy_error
error_frame_end = RECV_BUF + 6 + (error_text_end - error_text)
        rts

handle:
        .byte "nova01"
handle_end:

table_name:
        .byte "Test"
table_name_end:

server_name:
        .byte "Server"
server_name_end:

action_payload:
        .byte $DE, $AD, $BE, $EF
action_payload_end:

error_text:
        .byte "timeout"
error_text_end:

.include "gameserver.s"
.include "nic.s"
