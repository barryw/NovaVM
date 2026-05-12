; Nova Game Server live integration fixture.
;
; Loaded at $7000 by GameServer6502RuntimeIntegrationTests. This exercises the
; real 6502 gameserver.s runtime over the emulated NIC against TcpGameServer.

.setcpu "65c02"

.include "gameserver.inc"

RESULT_BASE = $22D0
FRAME_BUF   = $2400

      .segment "BSS"

frame_seen: .res 1

      .segment "CODE"

start:
        jmp asm_gameserver_live_entry

asm_gameserver_live_entry:
        pha
        phx
        phy
        jsr asm_gameserver_live_test
        ply
        plx
        pla
        rts

asm_gameserver_live_test:
        jsr clear_results

        jsr ngs_init
        lda #<FRAME_BUF
        ldx #>FRAME_BUF
        jsr ngs_set_buffer
        sta RESULT_BASE

        jsr ngs_connect_default
        sta RESULT_BASE + 1
        beq :+
        jmp @done
:

        jsr ngs_wait_connected
        sta RESULT_BASE + 2
        beq :+
        jmp @done
:

        lda #<handle
        ldx #>handle
        ldy #handle_end - handle
        jsr ngs_set_string
        lda #$01
        sta NGS_GAME_PROTO_MAJOR
        stz NGS_GAME_PROTO_MINOR
        jsr ngs_build_hello
        sta RESULT_BASE + 3
        beq :+
        jmp @done
:

        jsr ngs_send_current
        sta RESULT_BASE + 4
        beq :+
        jmp @done
:

        lda #NGS_KIND_WELCOME
        jsr ngs_wait_kind
        sta RESULT_BASE + 5
        beq :+
        jmp @done
:
        lda NGS_KIND
        sta RESULT_BASE + 6
        lda NGS_RECV_SEQ
        sta RESULT_BASE + 7
        lda NGS_FLAGS
        sta RESULT_BASE + 8
        jsr ngs_read_u16
        lda NGS_VALUE_L
        sta RESULT_BASE + 9
        lda NGS_VALUE_H
        sta RESULT_BASE + 10
        jsr ngs_read_string
        lda NGS_STRLEN
        sta RESULT_BASE + 11
        lda NGS_STRPTRL
        sta NVR5L
        lda NGS_STRPTRH
        sta NVR5H
        ldy #$00
        lda (NVR5L),Y
        sta RESULT_BASE + 12

        jsr ngs_build_game_list_request
        sta RESULT_BASE + 13
        beq :+
        jmp @done
:
        jsr ngs_send_current
        sta RESULT_BASE + 14
        beq :+
        jmp @done
:
        lda #NGS_KIND_GAME_LIST_ITEM
        jsr ngs_wait_kind
        sta RESULT_BASE + 15
        beq :+
        jmp @done
:
        lda NGS_KIND
        sta RESULT_BASE + 16
        lda NGS_RECV_SEQ
        sta RESULT_BASE + 17
        lda NGS_FLAGS
        sta RESULT_BASE + 18
        jsr ngs_read_u16
        lda NGS_VALUE_L
        sta RESULT_BASE + 19
        lda NGS_VALUE_H
        sta RESULT_BASE + 20

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
        sta RESULT_BASE + 21
        beq :+
        jmp @done
:
        jsr ngs_send_current
        sta RESULT_BASE + 22
        beq :+
        jmp @done
:
        lda #NGS_KIND_TABLE_CREATED
        jsr ngs_wait_kind
        sta RESULT_BASE + 23
        beq :+
        jmp @done
:
        lda NGS_KIND
        sta RESULT_BASE + 24
        lda NGS_RECV_SEQ
        sta RESULT_BASE + 25
        lda NGS_FLAGS
        sta RESULT_BASE + 26
        jsr ngs_read_u16
        lda NGS_VALUE_L
        sta RESULT_BASE + 27
        sta NGS_TABLE_IDL
        lda NGS_VALUE_H
        sta RESULT_BASE + 28
        sta NGS_TABLE_IDH

        jsr ngs_ping_roundtrip
        sta RESULT_BASE + 29
        lda NGS_KIND
        sta RESULT_BASE + 30
        lda NGS_RECV_SEQ
        sta RESULT_BASE + 31
        lda NGS_FLAGS
        sta RESULT_BASE + 32

        lda #$02
        ldx #$00
        jsr ngs_keepalive_set_interval
        jsr wait_next_frame
        jsr ngs_keepalive_tick
        sta RESULT_BASE + 33
        jsr wait_next_frame
        jsr ngs_keepalive_tick
        sta RESULT_BASE + 34
        lda NGS_KEEPALIVE_PENDING
        sta RESULT_BASE + 35

        lda #NGS_KIND_PONG
        jsr ngs_wait_kind
        sta RESULT_BASE + 36
        lda NGS_KIND
        sta RESULT_BASE + 37
        lda NGS_KEEPALIVE_PENDING
        sta RESULT_BASE + 38

        lda #$AA
        sta RESULT_BASE + $3F

@done:
        jsr ngs_disconnect
        rts

wait_next_frame:
        lda VGC_FRAME
        sta frame_seen
@loop:
        lda VGC_FRAME
        cmp frame_seen
        beq @loop
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

handle:
        .byte "nova6502"
handle_end:

table_name:
        .byte "Live"
table_name_end:

.include "gameserver.s"
