; Nova Chess game-server adapter.
;
; This is the chess-specific layer above runtime/asm/gameserver.s. It keeps
; the generic lobby/table protocol out of the game code and maps engine 0x88
; chess moves onto PLAYER_ACTION payloads.

.ifndef NCHESS_NET_INC_INCLUDED
NCHESS_NET_INC_INCLUDED = 1

.include "gameserver.inc"

NCHESS_GAME_ID            = $0002
NCHESS_PROTO_MAJOR        = $01
NCHESS_PROTO_MINOR        = $00

NCHESS_ACTION_MOVE        = $01
NCHESS_ACTION_RESIGN      = $02
NCHESS_ACTION_OFFER_DRAW  = $03

NCHESS_SIDE_BLACK         = $00
NCHESS_SIDE_WHITE         = $01

NCHESS_OK                 = $00
NCHESS_ERR                = $01
NCHESS_ERR_NOT_EVENT      = $02
NCHESS_ERR_NOT_MOVE       = $03
NCHESS_ERR_BAD_STATE      = $04

.global NCHESS_MOVE_FROM
.global NCHESS_MOVE_TO
.global NCHESS_EVENT_TABLEL
.global NCHESS_EVENT_TABLEH
.global NCHESS_EVENT_ACTION
.global NCHESS_EVENT_PLYL
.global NCHESS_EVENT_PLYH
.global NCHESS_EVENT_SIDE
.global NCHESS_EVENT_FROM
.global NCHESS_EVENT_TO
.global NCHESS_EVENT_ACTORL
.global NCHESS_EVENT_ACTORH
.global NCHESS_EVENT_ACTORLEN
.global NCHESS_STATE_TABLEL
.global NCHESS_STATE_TABLEH
.global NCHESS_STATE_VERSION
.global NCHESS_STATE_PLYL
.global NCHESS_STATE_PLYH
.global NCHESS_STATE_SIDE
.global NCHESS_STATE_LAST_FROM
.global NCHESS_STATE_LAST_TO
.global NCHESS_STATE_MOVE_COUNT
.global NCHESS_RESULT

.global nchess_net_init
.global nchess_net_build_hello
.global nchess_net_build_game_list_request
.global nchess_net_build_ai_list_request
.global nchess_net_build_opponent_list_request
.global nchess_net_build_table_list_request
.global nchess_net_build_create_table
.global nchess_net_build_join_table
.global nchess_net_build_observe_table
.global nchess_net_build_leave_table
.global nchess_net_build_state_request
.global nchess_net_build_add_ai
.global nchess_net_build_move
.global nchess_net_parse_table_event
.global nchess_net_parse_state_chunk

.endif
