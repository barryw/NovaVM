; Nova Game Server shared client protocol interface.
;
; This layer sits above lib/nic.s. It builds and parses the compact binary
; game-server frames carried by one Nova NIC message.
;
; Calling convention:
;   Buffer:      Set NGS_BUFL/H to a 256-byte CPU RAM frame buffer.
;   Host/text:   Set NGS_STRPTRL/H and NGS_STRLEN before routines that need
;                a host name, handle, chat text, table name, or raw payload.
;   IDs:         Set NGS_GAME_IDL/H and NGS_TABLE_IDL/H for table/game calls.
;   Routines:    Return A=0 on success unless documented otherwise. NGS_RESULT
;                holds the same result code. X/Y and NVR scratch bytes may be
;                clobbered.
;
; The fixed envelope is:
;   byte 0: server protocol major
;   byte 1: message kind
;   byte 2: sequence
;   byte 3: flags
; Payload bytes follow at offset 4.

.ifndef GAMESERVER_INC_INCLUDED
GAMESERVER_INC_INCLUDED = 1

.include "nic.inc"

NGS_OK              = $00
NGS_ERR             = $01
NGS_ERR_FRAME_FULL  = $02
NGS_ERR_SHORT_FRAME = $03
NGS_ERR_BAD_STRING  = $04
NGS_ERR_NO_DATA     = $05
NGS_ERR_TIMEOUT     = $06
NGS_ERR_REMOTE_CLOSED = $07
NGS_ERR_PROTOCOL_ERROR = $08

NGS_SERVER_PROTO_MAJOR = $01
NGS_SERVER_PROTO_MINOR = $00
NGS_DEFAULT_PORT       = $1967
NGS_ENVELOPE_SIZE      = $04
NGS_MAX_FRAME_PAYLOAD  = $0100
NGS_MAX_MESSAGE_PAYLOAD = $00FC
NGS_DEFAULT_TIMEOUT    = $FFFF
NGS_KEEPALIVE_DEFAULT  = $4650  ; 5 minutes at 60 Hz.

NGS_KIND_HELLO             = $01
NGS_KIND_WELCOME           = $02
NGS_KIND_ERROR             = $03
NGS_KIND_PING              = $04
NGS_KIND_PONG              = $05
NGS_KIND_LOBBY_CHAT        = $10
NGS_KIND_USER_LIST_REQUEST = $11
NGS_KIND_USER_LIST_ITEM    = $12
NGS_KIND_USER_JOINED       = $13
NGS_KIND_USER_LEFT         = $14
NGS_KIND_TABLE_CHAT        = $15
NGS_KIND_GAME_LIST_REQUEST = $20
NGS_KIND_GAME_LIST_ITEM    = $21
NGS_KIND_TABLE_LIST_REQUEST = $30
NGS_KIND_TABLE_LIST_ITEM   = $31
NGS_KIND_TABLE_CREATE      = $32
NGS_KIND_TABLE_JOIN        = $33
NGS_KIND_TABLE_LEAVE       = $34
NGS_KIND_TABLE_OBSERVE     = $35
NGS_KIND_TABLE_CREATED     = $36
NGS_KIND_TABLE_JOINED      = $37
NGS_KIND_TABLE_LEFT        = $38
NGS_KIND_TABLE_EVENT       = $40
NGS_KIND_PLAYER_ACTION     = $41
NGS_KIND_STATE_REQUEST     = $42
NGS_KIND_STATE_CHUNK       = $43
NGS_KIND_AI_LIST_REQUEST   = $50
NGS_KIND_AI_LIST_ITEM      = $51
NGS_KIND_AI_ADD_TO_TABLE   = $52
NGS_KIND_OPPONENT_LIST_REQUEST = $53
NGS_KIND_OPPONENT_LIST_ITEM = $54

NGS_FLAG_NONE     = $00
NGS_FLAG_RESPONSE = $01
NGS_FLAG_ERROR    = $02
NGS_FLAG_MORE     = $04
NGS_FLAG_EVENT    = $08

NGS_ERRCODE_UNKNOWN              = $00
NGS_ERRCODE_BAD_MESSAGE          = $01
NGS_ERRCODE_UNSUPPORTED_PROTOCOL = $02
NGS_ERRCODE_HANDLE_IN_USE        = $03
NGS_ERRCODE_NOT_REGISTERED       = $04
NGS_ERRCODE_UNKNOWN_GAME         = $05
NGS_ERRCODE_UNKNOWN_TABLE        = $06
NGS_ERRCODE_BAD_ACTION           = $07
NGS_ERRCODE_TABLE_FULL           = $08
NGS_ERRCODE_UNSUPPORTED_MESSAGE  = $09
NGS_ERRCODE_INVALID_HANDLE       = $0A
NGS_ERRCODE_NOT_SEATED           = $0B
NGS_ERRCODE_NOT_PARTICIPANT      = $0C
NGS_ERRCODE_UNKNOWN_AI_PROVIDER  = $0D
NGS_ERRCODE_TIMEOUT              = $0E

NGS_TABLE_ROLE_PLAYER   = $00
NGS_TABLE_ROLE_OBSERVER = $01

NGS_TABLE_STATE_WAITING  = $00
NGS_TABLE_STATE_PLAYING  = $01
NGS_TABLE_STATE_FINISHED = $02

NGS_OPPONENT_HUMAN = $00
NGS_OPPONENT_AI    = $01

; Linker-owned client state.
.global NGS_SLOT
.global NGS_PORTL
.global NGS_PORTH
.global NGS_SEQ
.global NGS_BUFL
.global NGS_BUFH
.global NGS_LEN
.global NGS_LAST_LEN
.global NGS_VERSION
.global NGS_KIND
.global NGS_RECV_SEQ
.global NGS_FLAGS
.global NGS_RESULT
.global NGS_READL
.global NGS_READH
.global NGS_REMAIN
.global NGS_STRPTRL
.global NGS_STRPTRH
.global NGS_STRLEN
.global NGS_VALUE_L
.global NGS_VALUE_H
.global NGS_CLIENT_CAPL
.global NGS_CLIENT_CAPH
.global NGS_GAME_IDL
.global NGS_GAME_IDH
.global NGS_GAME_PROTO_MAJOR
.global NGS_GAME_PROTO_MINOR
.global NGS_TABLE_IDL
.global NGS_TABLE_IDH
.global NGS_ROLE
.global NGS_ACTION_TYPE
.global NGS_TIMEOUT_L
.global NGS_TIMEOUT_H
.global NGS_LAST_STATUS
.global NGS_EXPECTED_KIND
.global NGS_ERROR_CODE
.global NGS_ERROR_TEXTPTRL
.global NGS_ERROR_TEXTPTRH
.global NGS_ERROR_TEXTLEN
.global NGS_KEEPALIVE_INTERVAL_L
.global NGS_KEEPALIVE_INTERVAL_H
.global NGS_KEEPALIVE_REMAIN_L
.global NGS_KEEPALIVE_REMAIN_H
.global NGS_KEEPALIVE_LAST_FRAME
.global NGS_KEEPALIVE_PENDING

; Core setup and NIC wrappers.
.global ngs_init
.global ngs_set_buffer
.global ngs_set_string
.global ngs_connect
.global ngs_connect_default
.global ngs_disconnect
.global ngs_status
.global ngs_ready
.global ngs_send_current
.global ngs_recv_current
.global ngs_set_timeout
.global ngs_keepalive_set_interval
.global ngs_keepalive_reset
.global ngs_keepalive_tick
.global ngs_wait_connected
.global ngs_wait_message
.global ngs_wait_kind
.global ngs_parse_error
.global ngs_ping_roundtrip

; Low-level frame writer/parser.
.global ngs_begin_message
.global ngs_write_byte
.global ngs_write_u16
.global ngs_write_string
.global ngs_write_bytes
.global ngs_parse_envelope
.global ngs_read_byte
.global ngs_read_u16
.global ngs_read_string

; Generic game-server request builders.
.global ngs_build_hello
.global ngs_build_ping
.global ngs_build_lobby_chat
.global ngs_build_table_chat
.global ngs_build_user_list_request
.global ngs_build_game_list_request
.global ngs_build_table_list_request
.global ngs_build_table_create
.global ngs_build_table_join
.global ngs_build_table_observe
.global ngs_build_table_leave
.global ngs_build_player_action
.global ngs_build_state_request
.global ngs_build_ai_list_request
.global ngs_build_ai_add_to_table
.global ngs_build_opponent_list_request

.endif
