; Nova Chess game-server adapter implementation.

.include "nchess_net.inc"
.include "gameserver.s"

.ifndef NCHESS_NET_IMPLEMENTATION_INCLUDED
NCHESS_NET_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

NCHESS_MOVE_FROM:       .res 1
NCHESS_MOVE_TO:         .res 1
NCHESS_EVENT_TABLEL:    .res 1
NCHESS_EVENT_TABLEH:    .res 1
NCHESS_EVENT_ACTION:    .res 1
NCHESS_EVENT_PLYL:      .res 1
NCHESS_EVENT_PLYH:      .res 1
NCHESS_EVENT_SIDE:      .res 1
NCHESS_EVENT_FROM:      .res 1
NCHESS_EVENT_TO:        .res 1
NCHESS_EVENT_ACTORL:    .res 1
NCHESS_EVENT_ACTORH:    .res 1
NCHESS_EVENT_ACTORLEN:  .res 1
NCHESS_STATE_TABLEL:    .res 1
NCHESS_STATE_TABLEH:    .res 1
NCHESS_STATE_VERSION:   .res 1
NCHESS_STATE_PLYL:      .res 1
NCHESS_STATE_PLYH:      .res 1
NCHESS_STATE_SIDE:      .res 1
NCHESS_STATE_LAST_FROM: .res 1
NCHESS_STATE_LAST_TO:   .res 1
NCHESS_STATE_MOVE_COUNT:.res 1
NCHESS_RESULT:          .res 1
nchess_move_payload:    .res 2

      .segment "CODE"

      .export nchess_net_init
      .export nchess_net_build_hello
      .export nchess_net_build_game_list_request
      .export nchess_net_build_ai_list_request
      .export nchess_net_build_opponent_list_request
      .export nchess_net_build_table_list_request
      .export nchess_net_build_create_table
      .export nchess_net_build_join_table
      .export nchess_net_build_observe_table
      .export nchess_net_build_leave_table
      .export nchess_net_build_state_request
      .export nchess_net_build_add_ai
      .export nchess_net_build_move
      .export nchess_net_parse_table_event
      .export nchess_net_parse_state_chunk

nchess_ok:
      STZ   NCHESS_RESULT
      LDA   #NCHESS_OK
      RTS

nchess_error:
      STA   NCHESS_RESULT
      RTS

nchess_finish_ngs:
      LDA   NGS_RESULT
      STA   NCHESS_RESULT
      RTS

nchess_set_game:
      LDA   #<NCHESS_GAME_ID
      STA   NGS_GAME_IDL
      LDA   #>NCHESS_GAME_ID
      STA   NGS_GAME_IDH
      LDA   #NCHESS_PROTO_MAJOR
      STA   NGS_GAME_PROTO_MAJOR
      LDA   #NCHESS_PROTO_MINOR
      STA   NGS_GAME_PROTO_MINOR
      RTS

; @label NCHESS.NET_INIT
; @kind routine
; @symbol nchess_net_init
; @summary Initialize generic game-server state with the Nova Chess game id/protocol.
nchess_net_init:
      JSR   ngs_init
      JSR   nchess_set_game
      JMP   nchess_ok

; @label NCHESS.BUILD_HELLO
; @kind routine
; @symbol nchess_net_build_hello
; @summary Build HELLO using NGS_STRPTRL/H:NGS_STRLEN as the requested handle.
nchess_net_build_hello:
      JSR   nchess_set_game
      JSR   ngs_build_hello
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_GAME_LIST
; @kind routine
; @symbol nchess_net_build_game_list_request
; @summary Build GAME_LIST_REQUEST.
nchess_net_build_game_list_request:
      JSR   ngs_build_game_list_request
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_AI_LIST
; @kind routine
; @symbol nchess_net_build_ai_list_request
; @summary Build AI_LIST_REQUEST filtered to chess providers.
nchess_net_build_ai_list_request:
      JSR   nchess_set_game
      JSR   ngs_build_ai_list_request
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_OPPONENT_LIST
; @kind routine
; @symbol nchess_net_build_opponent_list_request
; @summary Build OPPONENT_LIST_REQUEST filtered to chess-compatible opponents.
nchess_net_build_opponent_list_request:
      JSR   nchess_set_game
      JSR   ngs_build_opponent_list_request
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_TABLE_LIST
; @kind routine
; @symbol nchess_net_build_table_list_request
; @summary Build TABLE_LIST_REQUEST filtered to chess tables.
nchess_net_build_table_list_request:
      JSR   nchess_set_game
      JSR   ngs_build_table_list_request
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_CREATE_TABLE
; @kind routine
; @symbol nchess_net_build_create_table
; @summary Build TABLE_CREATE for a chess table using NGS_STRPTRL/H table name.
nchess_net_build_create_table:
      JSR   nchess_set_game
      JSR   ngs_build_table_create
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_JOIN_TABLE
; @kind routine
; @symbol nchess_net_build_join_table
; @summary Build TABLE_JOIN as a player using NGS_TABLE_IDL/H.
nchess_net_build_join_table:
      LDA   #NGS_TABLE_ROLE_PLAYER
      STA   NGS_ROLE
      JSR   ngs_build_table_join
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_OBSERVE_TABLE
; @kind routine
; @symbol nchess_net_build_observe_table
; @summary Build TABLE_OBSERVE using NGS_TABLE_IDL/H.
nchess_net_build_observe_table:
      JSR   ngs_build_table_observe
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_LEAVE_TABLE
; @kind routine
; @symbol nchess_net_build_leave_table
; @summary Build TABLE_LEAVE using NGS_TABLE_IDL/H.
nchess_net_build_leave_table:
      JSR   ngs_build_table_leave
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_STATE_REQUEST
; @kind routine
; @symbol nchess_net_build_state_request
; @summary Build STATE_REQUEST using NGS_TABLE_IDL/H.
nchess_net_build_state_request:
      JSR   ngs_build_state_request
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_ADD_AI
; @kind routine
; @symbol nchess_net_build_add_ai
; @summary Build AI_ADD_TO_TABLE using NGS_TABLE_IDL/H and NGS_VALUE_L/H provider id.
nchess_net_build_add_ai:
      JSR   ngs_build_ai_add_to_table
      JMP   nchess_finish_ngs

; @label NCHESS.BUILD_MOVE
; @kind routine
; @symbol nchess_net_build_move
; @summary Build PLAYER_ACTION/MOVE from NCHESS_MOVE_FROM and NCHESS_MOVE_TO.
nchess_net_build_move:
      LDA   NCHESS_MOVE_FROM
      STA   nchess_move_payload
      LDA   NCHESS_MOVE_TO
      STA   nchess_move_payload + 1
      LDA   #<nchess_move_payload
      STA   NGS_STRPTRL
      LDA   #>nchess_move_payload
      STA   NGS_STRPTRH
      LDA   #$02
      STA   NGS_STRLEN
      LDA   #NCHESS_ACTION_MOVE
      STA   NGS_ACTION_TYPE
      JSR   ngs_build_player_action
      JMP   nchess_finish_ngs

; @label NCHESS.PARSE_TABLE_EVENT
; @kind routine
; @symbol nchess_net_parse_table_event
; @summary Parse a TABLE_EVENT move into NCHESS_EVENT_* fields.
nchess_net_parse_table_event:
      LDA   NGS_KIND
      CMP   #NGS_KIND_TABLE_EVENT
      BEQ   @kind_ok
      LDA   #NCHESS_ERR_NOT_EVENT
      JMP   nchess_error
@kind_ok:
      JSR   ngs_read_u16
      LDA   NGS_RESULT
      BEQ   :+
      JMP   @ngs_err
:
      LDA   NGS_VALUE_L
      STA   NCHESS_EVENT_TABLEL
      LDA   NGS_VALUE_H
      STA   NCHESS_EVENT_TABLEH

      JSR   ngs_read_byte
      STA   NCHESS_EVENT_ACTION
      LDA   NGS_RESULT
      BEQ   :+
      JMP   @ngs_err
:
      LDA   NCHESS_EVENT_ACTION
      CMP   #NCHESS_ACTION_MOVE
      BEQ   @move_event
      LDA   #NCHESS_ERR_NOT_MOVE
      JMP   nchess_error

@move_event:
      JSR   ngs_read_string
      LDA   NGS_RESULT
      BEQ   :+
      JMP   @ngs_err
:
      LDA   NGS_STRPTRL
      STA   NCHESS_EVENT_ACTORL
      LDA   NGS_STRPTRH
      STA   NCHESS_EVENT_ACTORH
      LDA   NGS_STRLEN
      STA   NCHESS_EVENT_ACTORLEN
      LDA   NGS_REMAIN
      CMP   #$05
      BCS   @move_payload_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error

@move_payload_ok:
      LDA   NCHESS_EVENT_ACTORL
      CLC
      ADC   NCHESS_EVENT_ACTORLEN
      STA   NVR6L
      LDA   NCHESS_EVENT_ACTORH
      ADC   #$00
      STA   NVR6H
      LDY   #$00
      LDA   (NVR6L),Y
      STA   NCHESS_EVENT_PLYL
      INY
      LDA   (NVR6L),Y
      STA   NCHESS_EVENT_PLYH
      INY
      LDA   (NVR6L),Y
      STA   NCHESS_EVENT_SIDE
      INY
      LDA   (NVR6L),Y
      STA   NCHESS_EVENT_FROM
      INY
      LDA   (NVR6L),Y
      STA   NCHESS_EVENT_TO
      LDA   NCHESS_EVENT_SIDE
      CMP   #$02
      BCC   @side_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error
@side_ok:
      LDA   NCHESS_EVENT_FROM
      AND   #$88
      BEQ   @from_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error
@from_ok:
      LDA   NCHESS_EVENT_TO
      AND   #$7F
      STA   NVR7L
      AND   #$88
      BEQ   @to_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error
@to_ok:
      LDA   NCHESS_EVENT_FROM
      CMP   NVR7L
      BNE   @move_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error
@move_ok:
      JMP   nchess_ok

@ngs_err:
      LDA   #NCHESS_ERR
      JMP   nchess_error

; @label NCHESS.PARSE_STATE_CHUNK
; @kind routine
; @symbol nchess_net_parse_state_chunk
; @summary Parse chess STATE_CHUNK header into NCHESS_STATE_* fields.
nchess_net_parse_state_chunk:
      LDA   NGS_KIND
      CMP   #NGS_KIND_STATE_CHUNK
      BEQ   @kind_ok
      LDA   #NCHESS_ERR_BAD_STATE
      JMP   nchess_error
@kind_ok:
      JSR   ngs_read_u16
      LDA   NGS_RESULT
      BNE   @ngs_err
      LDA   NGS_VALUE_L
      STA   NCHESS_STATE_TABLEL
      LDA   NGS_VALUE_H
      STA   NCHESS_STATE_TABLEH

      JSR   ngs_read_byte
      STA   NCHESS_STATE_VERSION
      LDA   NGS_RESULT
      BNE   @ngs_err
      JSR   ngs_read_u16
      LDA   NGS_RESULT
      BNE   @ngs_err
      LDA   NGS_VALUE_L
      STA   NCHESS_STATE_PLYL
      LDA   NGS_VALUE_H
      STA   NCHESS_STATE_PLYH

      JSR   ngs_read_byte
      STA   NCHESS_STATE_SIDE
      LDA   NGS_RESULT
      BNE   @ngs_err
      JSR   ngs_read_byte
      STA   NCHESS_STATE_LAST_FROM
      LDA   NGS_RESULT
      BNE   @ngs_err
      JSR   ngs_read_byte
      STA   NCHESS_STATE_LAST_TO
      LDA   NGS_RESULT
      BNE   @ngs_err
      JSR   ngs_read_byte
      STA   NCHESS_STATE_MOVE_COUNT
      LDA   NGS_RESULT
      BNE   @ngs_err
      JMP   nchess_ok

@ngs_err:
      LDA   #NCHESS_ERR
      JMP   nchess_error

.endif
