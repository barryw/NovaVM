; Shared Nova Game Server client protocol runtime.

.include "gameserver.inc"

.ifndef NIC_SERVER_COMMANDS
NIC_SERVER_COMMANDS = 0
.endif

.include "nic.s"

.ifndef GAMESERVER_IMPLEMENTATION_INCLUDED
GAMESERVER_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

NGS_SLOT:             .res 1
NGS_PORTL:            .res 1
NGS_PORTH:            .res 1
NGS_SEQ:              .res 1
NGS_BUFL:             .res 1
NGS_BUFH:             .res 1
NGS_LEN:              .res 1
NGS_LAST_LEN:         .res 1
NGS_VERSION:          .res 1
NGS_KIND:             .res 1
NGS_RECV_SEQ:         .res 1
NGS_FLAGS:            .res 1
NGS_RESULT:           .res 1
NGS_READL:            .res 1
NGS_READH:            .res 1
NGS_REMAIN:           .res 1
NGS_STRPTRL:          .res 1
NGS_STRPTRH:          .res 1
NGS_STRLEN:           .res 1
NGS_VALUE_L:          .res 1
NGS_VALUE_H:          .res 1
NGS_CLIENT_CAPL:      .res 1
NGS_CLIENT_CAPH:      .res 1
NGS_GAME_IDL:         .res 1
NGS_GAME_IDH:         .res 1
NGS_GAME_PROTO_MAJOR: .res 1
NGS_GAME_PROTO_MINOR: .res 1
NGS_TABLE_IDL:        .res 1
NGS_TABLE_IDH:        .res 1
NGS_ROLE:             .res 1
NGS_ACTION_TYPE:      .res 1
NGS_TIMEOUT_L:        .res 1
NGS_TIMEOUT_H:        .res 1
NGS_LAST_STATUS:      .res 1
NGS_EXPECTED_KIND:    .res 1
NGS_ERROR_CODE:       .res 1
NGS_ERROR_TEXTPTRL:   .res 1
NGS_ERROR_TEXTPTRH:   .res 1
NGS_ERROR_TEXTLEN:    .res 1
NGS_KEEPALIVE_INTERVAL_L:.res 1
NGS_KEEPALIVE_INTERVAL_H:.res 1
NGS_KEEPALIVE_REMAIN_L:  .res 1
NGS_KEEPALIVE_REMAIN_H:  .res 1
NGS_KEEPALIVE_LAST_FRAME:.res 1
NGS_KEEPALIVE_PENDING:   .res 1

      .segment "CODE"

      .export ngs_init
      .export ngs_set_buffer
      .export ngs_set_string
      .export ngs_connect
      .export ngs_connect_default
      .export ngs_disconnect
      .export ngs_status
      .export ngs_ready
      .export ngs_send_current
      .export ngs_recv_current
      .export ngs_set_timeout
      .export ngs_keepalive_set_interval
      .export ngs_keepalive_reset
      .export ngs_keepalive_tick
      .export ngs_wait_connected
      .export ngs_wait_message
      .export ngs_wait_kind
      .export ngs_parse_error
      .export ngs_ping_roundtrip
      .export ngs_begin_message
      .export ngs_write_byte
      .export ngs_write_u16
      .export ngs_write_string
      .export ngs_write_bytes
      .export ngs_parse_envelope
      .export ngs_read_byte
      .export ngs_read_u16
      .export ngs_read_string
      .export ngs_build_hello
      .export ngs_build_ping
      .export ngs_build_lobby_chat
      .export ngs_build_table_chat
      .export ngs_build_user_list_request
      .export ngs_build_game_list_request
      .export ngs_build_table_list_request
      .export ngs_build_table_create
      .export ngs_build_table_join
      .export ngs_build_table_observe
      .export ngs_build_table_leave
      .export ngs_build_player_action
      .export ngs_build_state_request
      .export ngs_build_ai_list_request
      .export ngs_build_ai_add_to_table
      .export ngs_build_opponent_list_request

ngs_ok:
      STZ   NGS_RESULT
      LDA   #NGS_OK
      RTS

ngs_error:
      STA   NGS_RESULT
      RTS

; @label NGS.INIT
; @kind routine
; @symbol ngs_init
; @summary Initialize game-server client state and default port 6503.
ngs_init:
      STZ   NGS_SLOT
      LDA   #<NGS_DEFAULT_PORT
      STA   NGS_PORTL
      LDA   #>NGS_DEFAULT_PORT
      STA   NGS_PORTH
      STZ   NGS_SEQ
      STZ   NGS_BUFL
      STZ   NGS_BUFH
      STZ   NGS_LEN
      STZ   NGS_LAST_LEN
      LDA   #NGS_SERVER_PROTO_MAJOR
      STA   NGS_VERSION
      STZ   NGS_KIND
      STZ   NGS_RECV_SEQ
      STZ   NGS_FLAGS
      STZ   NGS_READL
      STZ   NGS_READH
      STZ   NGS_REMAIN
      STZ   NGS_STRPTRL
      STZ   NGS_STRPTRH
      STZ   NGS_STRLEN
      STZ   NGS_VALUE_L
      STZ   NGS_VALUE_H
      STZ   NGS_CLIENT_CAPL
      STZ   NGS_CLIENT_CAPH
      STZ   NGS_GAME_IDL
      STZ   NGS_GAME_IDH
      STZ   NGS_GAME_PROTO_MAJOR
      STZ   NGS_GAME_PROTO_MINOR
      STZ   NGS_TABLE_IDL
      STZ   NGS_TABLE_IDH
      STZ   NGS_ROLE
      STZ   NGS_ACTION_TYPE
      LDA   #<NGS_DEFAULT_TIMEOUT
      STA   NGS_TIMEOUT_L
      LDA   #>NGS_DEFAULT_TIMEOUT
      STA   NGS_TIMEOUT_H
      STZ   NGS_LAST_STATUS
      STZ   NGS_EXPECTED_KIND
      STZ   NGS_ERROR_CODE
      STZ   NGS_ERROR_TEXTPTRL
      STZ   NGS_ERROR_TEXTPTRH
      STZ   NGS_ERROR_TEXTLEN
      LDA   #<NGS_KEEPALIVE_DEFAULT
      STA   NGS_KEEPALIVE_INTERVAL_L
      LDA   #>NGS_KEEPALIVE_DEFAULT
      STA   NGS_KEEPALIVE_INTERVAL_H
      STZ   NGS_KEEPALIVE_PENDING
      JSR   ngs_keepalive_reset
      JMP   ngs_ok

; Input: A/X = frame buffer pointer.
; @label NGS.SET_BUFFER
; @kind routine
; @symbol ngs_set_buffer
; @summary Set the 256-byte CPU RAM frame buffer.
ngs_set_buffer:
      STA   NGS_BUFL
      STX   NGS_BUFH
      JMP   ngs_ok

; Input: A/X = pointer, Y = length.
; @label NGS.SET_STRING
; @kind routine
; @symbol ngs_set_string
; @summary Set the pointer/length pair used by host, string, and raw payload routines.
ngs_set_string:
      STA   NGS_STRPTRL
      STX   NGS_STRPTRH
      STY   NGS_STRLEN
      JMP   ngs_ok

; @label NGS.CONNECT
; @kind routine
; @symbol ngs_connect
; @summary Connect NGS_SLOT to NGS_STRPTRL/H:NGS_STRLEN at NGS_PORTL/H.
ngs_connect:
      LDA   NGS_STRLEN
      STA   NIC_ARG_NAMELEN
      LDA   NGS_STRPTRL
      STA   NIC_ARG_NAMEPTR_L
      LDA   NGS_STRPTRH
      STA   NIC_ARG_NAMEPTR_H
      JSR   nic_copy_name
      BEQ   @name_ok
      LDA   #NGS_ERR
      JMP   ngs_error
@name_ok:
      LDA   NGS_SLOT
      STA   NIC_SLOT
      LDA   NGS_PORTL
      STA   NIC_RPORTL
      LDA   NGS_PORTH
      STA   NIC_RPORTH
      JSR   nic_connect
      JMP   ngs_wait_command_clear

; @label NGS.CONNECT_DEFAULT
; @kind routine
; @symbol ngs_connect_default
; @summary Connect NGS_SLOT to the default Nova game-server endpoint.
ngs_connect_default:
      LDA   #<NGS_DEFAULT_PORT
      STA   NGS_PORTL
      LDA   #>NGS_DEFAULT_PORT
      STA   NGS_PORTH
      LDA   #<ngs_default_host
      LDX   #>ngs_default_host
      LDY   #ngs_default_host_end - ngs_default_host
      JSR   ngs_set_string
      JMP   ngs_connect

; @label NGS.DISCONNECT
; @kind routine
; @symbol ngs_disconnect
; @summary Disconnect NGS_SLOT.
ngs_disconnect:
      LDA   NGS_SLOT
      STA   NIC_SLOT
      JSR   nic_disconnect
      JMP   ngs_wait_command_clear

; @label NGS.STATUS
; @kind routine
; @symbol ngs_status
; @summary Return NGS_SLOT status in A.
ngs_status:
      LDX   NGS_SLOT
      JMP   nic_status

; @label NGS.READY
; @kind routine
; @symbol ngs_ready
; @summary Return A=1 when NGS_SLOT has a queued message, otherwise A=0.
ngs_ready:
      LDX   NGS_SLOT
      JMP   nic_ready

; Input: A/X = timeout in video frames.
; @label NGS.SET_TIMEOUT
; @kind routine
; @symbol ngs_set_timeout
; @summary Set shared wait timeout in video frames; zero waits indefinitely.
ngs_set_timeout:
      STA   NGS_TIMEOUT_L
      STX   NGS_TIMEOUT_H
      JMP   ngs_ok

; Input: A/X = keepalive interval in video frames. Zero disables keepalive ticks.
; @label NGS.KEEPALIVE_SET_INTERVAL
; @kind routine
; @symbol ngs_keepalive_set_interval
; @summary Set the client keepalive ping interval used by NGS.KEEPALIVE_TICK.
ngs_keepalive_set_interval:
      STA   NGS_KEEPALIVE_INTERVAL_L
      STX   NGS_KEEPALIVE_INTERVAL_H
      JMP   ngs_keepalive_reset

; @label NGS.KEEPALIVE_RESET
; @kind routine
; @symbol ngs_keepalive_reset
; @summary Reset the keepalive countdown to NGS_KEEPALIVE_INTERVAL_L/H.
ngs_keepalive_reset:
      LDA   NGS_KEEPALIVE_INTERVAL_L
      STA   NGS_KEEPALIVE_REMAIN_L
      LDA   NGS_KEEPALIVE_INTERVAL_H
      STA   NGS_KEEPALIVE_REMAIN_H
      LDA   VGC_FRAME
      STA   NGS_KEEPALIVE_LAST_FRAME
      JMP   ngs_ok

; @label NGS.KEEPALIVE_TICK
; @kind routine
; @symbol ngs_keepalive_tick
; @summary Send periodic PING frames while an app is blocked in UI/input code.
ngs_keepalive_tick:
      STZ   NGS_RESULT
      LDA   NGS_KEEPALIVE_INTERVAL_L
      ORA   NGS_KEEPALIVE_INTERVAL_H
      BEQ   @ok
      LDA   VGC_FRAME
      CMP   NGS_KEEPALIVE_LAST_FRAME
      BEQ   @ok
      STA   NGS_KEEPALIVE_LAST_FRAME
      LDA   NGS_KEEPALIVE_REMAIN_L
      ORA   NGS_KEEPALIVE_REMAIN_H
      BEQ   @send
      LDA   NGS_KEEPALIVE_REMAIN_L
      BNE   @dec_low
      DEC   NGS_KEEPALIVE_REMAIN_H
@dec_low:
      DEC   NGS_KEEPALIVE_REMAIN_L
      LDA   NGS_KEEPALIVE_REMAIN_L
      ORA   NGS_KEEPALIVE_REMAIN_H
      BNE   @ok
@send:
      JSR   ngs_build_ping
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_send_current
      LDA   NGS_RESULT
      BNE   @done
      LDA   #$01
      STA   NGS_KEEPALIVE_PENDING
@ok:
      JMP   ngs_ok
@done:
      LDA   NGS_RESULT
      RTS

ngs_begin_wait:
      STZ   NGS_RESULT
      LDA   NGS_TIMEOUT_L
      STA   NVR7L
      LDA   NGS_TIMEOUT_H
      STA   NVR7H
      LDA   VGC_FRAME
      STA   NVR5L
      RTS

ngs_wait_tick:
      LDA   VGC_FRAME
      CMP   NVR5L
      BEQ   @continue
      STA   NVR5L
      LDA   NGS_TIMEOUT_L
      ORA   NGS_TIMEOUT_H
      BEQ   @continue
      LDA   NVR7L
      ORA   NVR7H
      BEQ   @timeout
      LDA   NVR7L
      BNE   @dec_low
      DEC   NVR7H
@dec_low:
      DEC   NVR7L
@continue:
      STZ   NGS_RESULT
      LDA   #NGS_OK
      RTS
@timeout:
      LDA   #NGS_ERR_TIMEOUT
      JMP   ngs_error

ngs_status_error:
      LDA   NGS_LAST_STATUS
      AND   #NIC_ST_REMOTE_CLOSED
      BEQ   @generic
      LDA   #NGS_ERR_REMOTE_CLOSED
      JMP   ngs_error
@generic:
      LDA   #NGS_ERR
      JMP   ngs_error

ngs_wait_command_clear:
      JSR   ngs_begin_wait
@loop:
      LDA   NIC_CMD
      BEQ   @ok
      JSR   ngs_wait_tick
      LDA   NGS_RESULT
      BEQ   @loop
      RTS
@ok:
      JMP   ngs_ok

; @label NGS.WAIT_CONNECTED
; @kind routine
; @symbol ngs_wait_connected
; @summary Wait for NGS_SLOT to connect or fail within NGS_TIMEOUT_L/H frames.
ngs_wait_connected:
      JSR   ngs_begin_wait
@loop:
      JSR   ngs_status
      STA   NGS_LAST_STATUS
      AND   #NIC_ST_CONNECTED
      BNE   @ok
      LDA   NGS_LAST_STATUS
      AND   #(NIC_ST_ERROR | NIC_ST_REMOTE_CLOSED)
      BEQ   @status_ok
      JMP   ngs_status_error
@status_ok:
      JSR   ngs_wait_tick
      LDA   NGS_RESULT
      BEQ   @loop
      RTS
@ok:
      JMP   ngs_ok

ngs_next_sequence:
      INC   NGS_SEQ
      BNE   @done
      INC   NGS_SEQ
@done:
      RTS

ngs_frame_ptr:
      LDA   NGS_LEN
      CLC
      ADC   NGS_BUFL
      STA   NVR6L
      LDA   NGS_BUFH
      ADC   #$00
      STA   NVR6H
      RTS

; Input: A = message kind, X = flags.
; @label NGS.BEGIN_MESSAGE
; @kind routine
; @symbol ngs_begin_message
; @summary Begin a request frame and allocate the next non-zero sequence byte.
ngs_begin_message:
      STA   NGS_KIND
      STX   NGS_FLAGS
      JSR   ngs_next_sequence
      LDA   NGS_BUFL
      STA   NVR6L
      LDA   NGS_BUFH
      STA   NVR6H
      LDY   #$00
      LDA   #NGS_SERVER_PROTO_MAJOR
      STA   (NVR6L),Y
      INY
      LDA   NGS_KIND
      STA   (NVR6L),Y
      INY
      LDA   NGS_SEQ
      STA   (NVR6L),Y
      INY
      LDA   NGS_FLAGS
      STA   (NVR6L),Y
      LDA   #NGS_ENVELOPE_SIZE
      STA   NGS_LEN
      JMP   ngs_ok

; Input: A = byte.
; @label NGS.WRITE_BYTE
; @kind routine
; @symbol ngs_write_byte
; @summary Append one payload byte to the current frame.
ngs_write_byte:
      PHA
      LDA   NGS_LEN
      BEQ   @full
      JSR   ngs_frame_ptr
      LDY   #$00
      PLA
      STA   (NVR6L),Y
      INC   NGS_LEN
      JMP   ngs_ok
@full:
      PLA
      LDA   #NGS_ERR_FRAME_FULL
      JMP   ngs_error

; Input: A/X = little-endian u16.
; @label NGS.WRITE_U16
; @kind routine
; @symbol ngs_write_u16
; @summary Append a little-endian 16-bit value.
ngs_write_u16:
      STA   NGS_VALUE_L
      STX   NGS_VALUE_H
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_VALUE_H
      JSR   ngs_write_byte
@done:
      LDA   NGS_RESULT
      RTS

ngs_append_from_ptr:
      LDA   NVR7L
      BEQ   @ok
@loop:
      LDY   #$00
      LDA   (NVR5L),Y
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      INC   NVR5L
      BNE   @next
      INC   NVR5H
@next:
      DEC   NVR7L
      BNE   @loop
@ok:
      JMP   ngs_ok
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.WRITE_STRING
; @kind routine
; @symbol ngs_write_string
; @summary Append NGS_STRLEN plus bytes from NGS_STRPTRL/H.
ngs_write_string:
      LDA   NGS_STRLEN
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_STRPTRL
      STA   NVR5L
      LDA   NGS_STRPTRH
      STA   NVR5H
      LDA   NGS_STRLEN
      STA   NVR7L
      JSR   ngs_append_from_ptr
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.WRITE_BYTES
; @kind routine
; @symbol ngs_write_bytes
; @summary Append raw bytes from NGS_STRPTRL/H without a length prefix.
ngs_write_bytes:
      LDA   NGS_STRPTRL
      STA   NVR5L
      LDA   NGS_STRPTRH
      STA   NVR5H
      LDA   NGS_STRLEN
      STA   NVR7L
      JSR   ngs_append_from_ptr
      LDA   NGS_RESULT
      RTS

ngs_write_client_name:
      LDA   #ngs_client_name_end - ngs_client_name
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   #<ngs_client_name
      STA   NVR5L
      LDA   #>ngs_client_name
      STA   NVR5H
      LDA   #ngs_client_name_end - ngs_client_name
      STA   NVR7L
      JSR   ngs_append_from_ptr
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.SEND_CURRENT
; @kind routine
; @symbol ngs_send_current
; @summary Send the current frame through NGS_SLOT using NIC DMA registers.
ngs_send_current:
      LDA   NGS_LEN
      BEQ   @send
      CMP   #NGS_ENVELOPE_SIZE
      BCS   @send
      LDA   #NGS_ERR_SHORT_FRAME
      JMP   ngs_error
@send:
      LDA   NGS_SLOT
      STA   NIC_SLOT
      LDA   NGS_BUFL
      STA   NIC_DMAL
      LDA   NGS_BUFH
      STA   NIC_DMAH
      LDA   NGS_LEN
      STA   NIC_DMALEN
      JSR   nic_send
      JSR   ngs_wait_command_clear
      LDA   NGS_RESULT
      BNE   @done
      LDA   NIC_DMAERR            ; a range/busy DMA fault sent no bytes
      BEQ   @sent
      LDA   #NGS_ERR
      STA   NGS_RESULT
      BRA   @done
@sent:
      JSR   ngs_keepalive_reset
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.RECV_CURRENT
; @kind routine
; @symbol ngs_recv_current
; @summary Receive and parse one queued frame from NGS_SLOT into NGS_BUFL/H.
ngs_recv_current:
      JSR   ngs_ready
      BNE   @ready
      LDA   #NGS_ERR_NO_DATA
      JMP   ngs_error
@ready:
      LDA   NGS_SLOT
      STA   NIC_SLOT
      LDA   NGS_BUFL
      STA   NIC_DMAL
      LDA   NGS_BUFH
      STA   NIC_DMAH
      JSR   nic_recv
      JSR   ngs_wait_command_clear
      LDA   NGS_RESULT
      BNE   @done
      LDA   NIC_DMAERR            ; a range/busy DMA fault received no bytes
      BEQ   @received
      LDA   #NGS_ERR
      STA   NGS_RESULT
      BRA   @done
@received:
      JSR   nic_length
      STA   NGS_LAST_LEN
      JMP   ngs_parse_envelope
@done:
      RTS

; @label NGS.PARSE_ERROR
; @kind routine
; @symbol ngs_parse_error
; @summary Parse current ERROR payload into NGS_ERROR_CODE and NGS_ERROR_TEXT*.
ngs_parse_error:
      LDA   NGS_KIND
      CMP   #NGS_KIND_ERROR
      BEQ   @kind_ok
      LDA   #NGS_ERR
      JMP   ngs_error
@kind_ok:
      JSR   ngs_read_byte
      STA   NGS_ERROR_CODE
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_read_string
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_STRPTRL
      STA   NGS_ERROR_TEXTPTRL
      LDA   NGS_STRPTRH
      STA   NGS_ERROR_TEXTPTRH
      LDA   NGS_STRLEN
      STA   NGS_ERROR_TEXTLEN
      LDA   #NGS_ERR_PROTOCOL_ERROR
      JMP   ngs_error
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.WAIT_MESSAGE
; @kind routine
; @symbol ngs_wait_message
; @summary Wait for and receive one frame; protocol ERROR frames are parsed and returned as NGS_ERR_PROTOCOL_ERROR.
ngs_wait_message:
      JSR   ngs_begin_wait
@loop:
      JSR   ngs_ready
      BNE   @recv
      JSR   ngs_status
      STA   NGS_LAST_STATUS
      AND   #(NIC_ST_ERROR | NIC_ST_REMOTE_CLOSED)
      BEQ   @status_ok
      JMP   ngs_status_error
@status_ok:
      JSR   ngs_wait_tick
      LDA   NGS_RESULT
      BEQ   @loop
      RTS
@recv:
      JSR   ngs_recv_current
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_KIND
      CMP   #NGS_KIND_PONG
      BNE   :+
      STZ   NGS_KEEPALIVE_PENDING
      JSR   ngs_keepalive_reset
:
      LDA   NGS_KIND
      CMP   #NGS_KIND_ERROR
      BNE   @ok
      JMP   ngs_parse_error
@ok:
      JMP   ngs_ok
@done:
      LDA   NGS_RESULT
      RTS

; Input: A = expected message kind.
; @label NGS.WAIT_KIND
; @kind routine
; @symbol ngs_wait_kind
; @summary Wait for a non-error frame of the requested kind, skipping unrelated events.
ngs_wait_kind:
      STA   NGS_EXPECTED_KIND
@loop:
      JSR   ngs_wait_message
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_KIND
      CMP   NGS_EXPECTED_KIND
      BNE   @loop
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.PING_ROUNDTRIP
; @kind routine
; @symbol ngs_ping_roundtrip
; @summary Send PING and wait for PONG using the shared timeout/error handling.
ngs_ping_roundtrip:
      JSR   ngs_build_ping
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_send_current
      LDA   NGS_RESULT
      BNE   @done
      LDA   #NGS_KIND_PONG
      JSR   ngs_wait_kind
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.PARSE_ENVELOPE
; @kind routine
; @symbol ngs_parse_envelope
; @summary Parse the frame envelope at NGS_BUFL/H using NGS_LAST_LEN.
ngs_parse_envelope:
      LDA   NGS_LAST_LEN
      BEQ   @length_ok
      CMP   #NGS_ENVELOPE_SIZE
      BCS   @length_ok
      LDA   #NGS_ERR_SHORT_FRAME
      JMP   ngs_error
@length_ok:
      LDA   NGS_BUFL
      STA   NVR6L
      LDA   NGS_BUFH
      STA   NVR6H
      LDY   #$00
      LDA   (NVR6L),Y
      STA   NGS_VERSION
      INY
      LDA   (NVR6L),Y
      STA   NGS_KIND
      INY
      LDA   (NVR6L),Y
      STA   NGS_RECV_SEQ
      INY
      LDA   (NVR6L),Y
      STA   NGS_FLAGS
      LDA   NGS_LAST_LEN
      BEQ   @full_frame
      SEC
      SBC   #NGS_ENVELOPE_SIZE
      STA   NGS_REMAIN
      BRA   @set_read
@full_frame:
      LDA   #NGS_MAX_MESSAGE_PAYLOAD
      STA   NGS_REMAIN
@set_read:
      LDA   NGS_BUFL
      CLC
      ADC   #NGS_ENVELOPE_SIZE
      STA   NGS_READL
      LDA   NGS_BUFH
      ADC   #$00
      STA   NGS_READH
      JMP   ngs_ok

ngs_inc_read:
      INC   NGS_READL
      BNE   @done
      INC   NGS_READH
@done:
      RTS

; @label NGS.READ_BYTE
; @kind routine
; @symbol ngs_read_byte
; @summary Read one byte from the parsed payload into A.
ngs_read_byte:
      LDA   NGS_REMAIN
      BNE   @has_byte
      LDA   #NGS_ERR_SHORT_FRAME
      STA   NGS_RESULT
      LDA   #$00
      RTS
@has_byte:
      LDA   NGS_READL
      STA   NVR6L
      LDA   NGS_READH
      STA   NVR6H
      LDY   #$00
      LDA   (NVR6L),Y
      PHA
      JSR   ngs_inc_read
      DEC   NGS_REMAIN
      STZ   NGS_RESULT
      PLA
      RTS

; @label NGS.READ_U16
; @kind routine
; @symbol ngs_read_u16
; @summary Read a little-endian u16 into NGS_VALUE_L/H and A/X.
ngs_read_u16:
      JSR   ngs_read_byte
      STA   NGS_VALUE_L
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_read_byte
      STA   NGS_VALUE_H
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_VALUE_L
      LDX   NGS_VALUE_H
      RTS
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.READ_STRING
; @kind routine
; @symbol ngs_read_string
; @summary Read a length-prefixed string pointer into NGS_STRPTRL/H:NGS_STRLEN.
ngs_read_string:
      JSR   ngs_read_byte
      STA   NGS_STRLEN
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_REMAIN
      CMP   NGS_STRLEN
      BCS   @has_string
      LDA   #NGS_ERR_BAD_STRING
      JMP   ngs_error
@has_string:
      LDA   NGS_READL
      STA   NGS_STRPTRL
      LDA   NGS_READH
      STA   NGS_STRPTRH
      LDA   NGS_READL
      CLC
      ADC   NGS_STRLEN
      STA   NGS_READL
      BCC   @same_page
      INC   NGS_READH
@same_page:
      LDA   NGS_REMAIN
      SEC
      SBC   NGS_STRLEN
      STA   NGS_REMAIN
      JMP   ngs_ok
@done:
      LDA   NGS_RESULT
      RTS

ngs_begin_kind:
      LDX   #NGS_FLAG_NONE
      JMP   ngs_begin_message

; @label NGS.BUILD_HELLO
; @kind routine
; @symbol ngs_build_hello
; @summary Build HELLO using NGS_STRPTRL/H:NGS_STRLEN as the requested handle.
ngs_build_hello:
      LDA   #NGS_KIND_HELLO
      JSR   ngs_begin_kind
      LDA   #NGS_SERVER_PROTO_MINOR
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_CLIENT_CAPL
      LDX   NGS_CLIENT_CAPH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_write_string
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_write_client_name
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_GAME_PROTO_MAJOR
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_GAME_PROTO_MINOR
      JSR   ngs_write_byte
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_PING
; @kind routine
; @symbol ngs_build_ping
; @summary Build an empty PING request.
ngs_build_ping:
      LDA   #NGS_KIND_PING
      JMP   ngs_begin_kind

; @label NGS.BUILD_LOBBY_CHAT
; @kind routine
; @symbol ngs_build_lobby_chat
; @summary Build LOBBY_CHAT from NGS_STRPTRL/H:NGS_STRLEN.
ngs_build_lobby_chat:
      LDA   #NGS_KIND_LOBBY_CHAT
      JSR   ngs_begin_kind
      JMP   ngs_write_string

; @label NGS.BUILD_TABLE_CHAT
; @kind routine
; @symbol ngs_build_table_chat
; @summary Build TABLE_CHAT with NGS_TABLE_IDL/H and string text.
ngs_build_table_chat:
      LDA   #NGS_KIND_TABLE_CHAT
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_write_string
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_USER_LIST_REQUEST
; @kind routine
; @symbol ngs_build_user_list_request
; @summary Build USER_LIST_REQUEST.
ngs_build_user_list_request:
      LDA   #NGS_KIND_USER_LIST_REQUEST
      JMP   ngs_begin_kind

; @label NGS.BUILD_GAME_LIST_REQUEST
; @kind routine
; @symbol ngs_build_game_list_request
; @summary Build GAME_LIST_REQUEST.
ngs_build_game_list_request:
      LDA   #NGS_KIND_GAME_LIST_REQUEST
      JMP   ngs_begin_kind

; @label NGS.BUILD_TABLE_LIST_REQUEST
; @kind routine
; @symbol ngs_build_table_list_request
; @summary Build TABLE_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.
ngs_build_table_list_request:
      LDA   #NGS_KIND_TABLE_LIST_REQUEST
      JSR   ngs_begin_kind
      LDA   NGS_GAME_IDL
      ORA   NGS_GAME_IDH
      BEQ   @done
      LDA   NGS_GAME_IDL
      LDX   NGS_GAME_IDH
      JSR   ngs_write_u16
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_TABLE_CREATE
; @kind routine
; @symbol ngs_build_table_create
; @summary Build TABLE_CREATE using game id/version and NGS_STRPTRL/H table name.
ngs_build_table_create:
      LDA   #NGS_KIND_TABLE_CREATE
      JSR   ngs_begin_kind
      LDA   NGS_GAME_IDL
      LDX   NGS_GAME_IDH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_GAME_PROTO_MAJOR
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_GAME_PROTO_MINOR
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_write_string
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_TABLE_JOIN
; @kind routine
; @symbol ngs_build_table_join
; @summary Build TABLE_JOIN with NGS_TABLE_IDL/H and NGS_ROLE.
ngs_build_table_join:
      LDA   #NGS_KIND_TABLE_JOIN
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_ROLE
      JSR   ngs_write_byte
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_TABLE_OBSERVE
; @kind routine
; @symbol ngs_build_table_observe
; @summary Build TABLE_OBSERVE with NGS_TABLE_IDL/H.
ngs_build_table_observe:
      LDA   #NGS_KIND_TABLE_OBSERVE
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JMP   ngs_write_u16

; @label NGS.BUILD_TABLE_LEAVE
; @kind routine
; @symbol ngs_build_table_leave
; @summary Build TABLE_LEAVE with NGS_TABLE_IDL/H.
ngs_build_table_leave:
      LDA   #NGS_KIND_TABLE_LEAVE
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JMP   ngs_write_u16

; @label NGS.BUILD_PLAYER_ACTION
; @kind routine
; @symbol ngs_build_player_action
; @summary Build PLAYER_ACTION with table id, action type, and raw payload bytes.
ngs_build_player_action:
      LDA   #NGS_KIND_PLAYER_ACTION
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_ACTION_TYPE
      JSR   ngs_write_byte
      LDA   NGS_RESULT
      BNE   @done
      JSR   ngs_write_bytes
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_STATE_REQUEST
; @kind routine
; @symbol ngs_build_state_request
; @summary Build STATE_REQUEST with NGS_TABLE_IDL/H.
ngs_build_state_request:
      LDA   #NGS_KIND_STATE_REQUEST
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JMP   ngs_write_u16

; @label NGS.BUILD_AI_LIST_REQUEST
; @kind routine
; @symbol ngs_build_ai_list_request
; @summary Build AI_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.
ngs_build_ai_list_request:
      LDA   #NGS_KIND_AI_LIST_REQUEST
      JSR   ngs_begin_kind
      LDA   NGS_GAME_IDL
      ORA   NGS_GAME_IDH
      BEQ   @done
      LDA   NGS_GAME_IDL
      LDX   NGS_GAME_IDH
      JSR   ngs_write_u16
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_AI_ADD_TO_TABLE
; @kind routine
; @symbol ngs_build_ai_add_to_table
; @summary Build AI_ADD_TO_TABLE with NGS_TABLE_IDL/H and NGS_VALUE_L/H provider id.
ngs_build_ai_add_to_table:
      LDA   #NGS_KIND_AI_ADD_TO_TABLE
      JSR   ngs_begin_kind
      LDA   NGS_TABLE_IDL
      LDX   NGS_TABLE_IDH
      JSR   ngs_write_u16
      LDA   NGS_RESULT
      BNE   @done
      LDA   NGS_VALUE_L
      LDX   NGS_VALUE_H
      JSR   ngs_write_u16
@done:
      LDA   NGS_RESULT
      RTS

; @label NGS.BUILD_OPPONENT_LIST_REQUEST
; @kind routine
; @symbol ngs_build_opponent_list_request
; @summary Build OPPONENT_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.
ngs_build_opponent_list_request:
      LDA   #NGS_KIND_OPPONENT_LIST_REQUEST
      JSR   ngs_begin_kind
      LDA   NGS_GAME_IDL
      ORA   NGS_GAME_IDH
      BEQ   @done
      LDA   NGS_GAME_IDL
      LDX   NGS_GAME_IDH
      JSR   ngs_write_u16
@done:
      LDA   NGS_RESULT
      RTS

ngs_client_name:
      .byte "nova-6502"
ngs_client_name_end:

ngs_default_host:
      .byte "nova-game-server"
ngs_default_host_end:

.endif
