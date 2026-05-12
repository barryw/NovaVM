; Nova Chess network-mode overlay.

.setcpu "65c02"

.include "nchess_net.inc"
.include "engine_labels.inc"
.include "network_kernel_labels.inc"

GAME_CHECKMATE = $02

.segment "BSS"
network_table_idl:   .res 1
network_table_idh:   .res 1
network_frame:       .res NGS_MAX_FRAME_PAYLOAD

.segment "CODE"

.export network_overlay_main
.export network_overlay_unload

network_overlay_main:
        JSR network_setup_game
        BEQ @loop
        LDA #$01
        RTS
@loop:
        JSR KERNEL_UPDATE_CLOCK
        JSR ENGINE_CHESS_CHECK_GAME_STATE
        STA KERNEL_GAME_STATE
        CMP #GAME_CHECKMATE
        BCS @game_over

        JSR network_human_turn
        BEQ @loop
        LDA #$01
        RTS
@game_over:
        JSR KERNEL_SHOW_GAME_RESULT
        LDA #$00
        RTS

network_overlay_unload:
        JSR KERNEL_INPUT_CLEAR_IDLE_HOOK
        JSR ngs_disconnect
        LDA #$00
        RTS

network_input_idle:
        JMP ngs_keepalive_tick

network_set_input_idle:
        LDA #<network_input_idle
        LDX #>network_input_idle
        JMP KERNEL_INPUT_SET_IDLE_HOOK

network_human_turn:
@retry:
        JSR network_set_input_idle
        JSR KERNEL_PROMPT_HUMAN_MOVE
        BEQ @move_ready
        CMP #$02
        BEQ @error
        JSR KERNEL_SET_STATUS_INVALID
        LDX #45
        JSR KERNEL_WAIT_DEMO_FRAMES
        BRA @retry
@move_ready:
        JSR KERNEL_SET_STATUS_MOVING
        JSR KERNEL_COMMIT_CURRENT_MOVE
        JSR network_send_current_move
        BNE @error
        LDA KERNEL_GAME_STATE
        CMP #GAME_CHECKMATE
        BCS @ok
        JSR KERNEL_SET_STATUS_THINKING
        JSR network_wait_opponent_move
        BNE @error
        JSR KERNEL_SET_STATUS_MOVING
        JSR KERNEL_COMMIT_CURRENT_MOVE
@ok:
        LDA #$00
        RTS
@error:
        LDA #$01
        RTS

network_setup_game:
        JSR KERNEL_SET_STATUS_NETWORK
        JSR nchess_net_init
        LDA #<network_frame
        LDX #>network_frame
        JSR ngs_set_buffer
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_disconnect

        JSR ngs_connect_default
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_wait_connected
        BEQ :+
        JMP @error
:

        LDA #<game_server_handle
        LDX #>game_server_handle
        LDY #game_server_handle_end - game_server_handle
        JSR ngs_set_string
        JSR nchess_net_build_hello
        LDA NCHESS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_send_current
        BEQ :+
        JMP @error
:
        LDA #NGS_KIND_WELCOME
        JSR ngs_wait_kind
        BEQ :+
        JMP @error
:

        LDA #<game_server_table_name
        LDX #>game_server_table_name
        LDY #game_server_table_name_end - game_server_table_name
        JSR ngs_set_string
        JSR nchess_net_build_create_table
        LDA NCHESS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_send_current
        BEQ :+
        JMP @error
:
        LDA #NGS_KIND_TABLE_CREATED
        JSR ngs_wait_kind
        BEQ :+
        JMP @error
:
        JSR ngs_read_u16
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        LDA NGS_VALUE_L
        STA network_table_idl
        STA NGS_TABLE_IDL
        LDA NGS_VALUE_H
        STA network_table_idh
        STA NGS_TABLE_IDH

        LDA #$00
        RTS
@error:
        JSR ngs_disconnect
        LDA #$01
        RTS

network_send_current_move:
        LDA network_table_idl
        STA NGS_TABLE_IDL
        LDA network_table_idh
        STA NGS_TABLE_IDH
        LDA KERNEL_SELF_FROM
        STA NCHESS_MOVE_FROM
        LDA KERNEL_SELF_TO
        STA NCHESS_MOVE_TO
        JSR nchess_net_build_move
        LDA NCHESS_RESULT
        BNE @error
        JMP ngs_send_current
@error:
        LDA #$01
        RTS

network_wait_opponent_move:
@wait_event:
        LDA #NGS_KIND_TABLE_EVENT
        JSR ngs_wait_kind
        BNE @error
        JSR nchess_net_parse_table_event
        LDA NCHESS_RESULT
        BNE @wait_event
        LDA NCHESS_EVENT_TABLEL
        CMP network_table_idl
        BNE @wait_event
        LDA NCHESS_EVENT_TABLEH
        CMP network_table_idh
        BNE @wait_event
        JSR network_event_actor_is_self
        BNE @wait_event
        LDA NCHESS_EVENT_FROM
        CMP KERNEL_SELF_FROM
        BNE @opponent
        LDA NCHESS_EVENT_TO
        CMP KERNEL_SELF_TO
        BEQ @wait_event
@opponent:
        LDA NCHESS_EVENT_FROM
        STA KERNEL_SELF_FROM
        LDA NCHESS_EVENT_TO
        STA KERNEL_SELF_TO
        LDA #$00
        RTS
@error:
        LDA #$01
        RTS

network_event_actor_is_self:
        LDA NCHESS_EVENT_ACTORLEN
        CMP #(game_server_handle_end - game_server_handle)
        BNE @not_self
        LDA NCHESS_EVENT_ACTORL
        STA NVR6L
        LDA NCHESS_EVENT_ACTORH
        STA NVR6H
        LDY #$00
@loop:
        CPY #(game_server_handle_end - game_server_handle)
        BEQ @self
        LDA (NVR6L),Y
        CMP game_server_handle,Y
        BNE @not_self
        INY
        BRA @loop
@self:
        LDA #$01
        RTS
@not_self:
        LDA #$00
        RTS

.segment "RODATA"

game_server_handle:
        .byte "nova-chess"
game_server_handle_end:
game_server_table_name:
        .byte "Nova Chess"
game_server_table_name_end:

.include "nchess_net.s"
