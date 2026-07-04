; net.s — NET module (MODULE_ID_NET = $06). THIN lib_call wrappers over the
; networking NDK: the NIC helper runtime (runtime/asm/nic.s) and the Nova Game
; Server client protocol runtime (runtime/asm/gameserver.s). The NDK owns all
; NIC transport + game-server framing logic; this module only marshals the
; mailbox args into the NIC_* MMIO registers / NGS_* BSS client-state fields and
; JSRs the NDK routine.
;
; Coverage: raw NIC slot connect/disconnect/listen/accept/send/recv + the
; status/ready/length reporters; the game-server session (init, buffer/string/
; timeout setters, connect/connect-default/disconnect, status/ready, keepalive),
; messaging (send/recv current, wait message/kind, ping roundtrip, parse
; envelope), and the 16 request builders (hello..opponent_list_request). Per
; ModuleNdkContractTests this module's own code never pokes SID registers or
; busy-waits the VGC frame counter — every fn wraps an NDK routine.
;
; Naming: ids are NET_* (the networking-domain library, see libnet.inc). NIC_*
; (MMIO, nova.inc/nic.inc) and NGS_* (game-server BSS state, gameserver.inc) are
; the wrapped NDK's register/state prefixes — different namespaces.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libnet.inc"
      .include "nova.inc"              ; NVR* scratch + NIC MMIO registers + VGC_FRAME
      .include "nic.inc"            ; NIC_ARG_* pointer-name pseudo-registers + result codes
      .include "gameserver.inc"    ; NGS_* BSS client-state .globals + kind/result codes

      .segment "CODE"
      lib_module_header MODULE_ID_NET, LIB_ABI_VERSION, NET_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by nova module pack into net.nmod).
;
; Each ;@ndk line maps the fn to the NDK routine it wraps; the human summary is
; single-sourced from that routine (feedback-ndk-source-of-truth). The build-time
; + CI drift guard requires the wrapper body to actually JSR that routine. Mailbox
; @arg bindings stay here — they are this wrapper's marshalling, not duplicated.
; ===========================================================================
;@module NET
;@version 1.0
;@brief Shared networking services: the nic + gameserver NDK (thin wrappers over nic.s/gameserver.s).
;
;@fn NET_CONNECT
;@ndk nic_connect
;@arg nameptr u16 pointer to host name bytes -> NIC_NAMEBUF (ARG0 byte0,1)
;@arg namelen u8 host name length 1..31 (ARG0 byte2)
;@arg rport u16 remote port -> NIC_RPORTL/H (ARG1 byte0,1)
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_DISCONNECT
;@ndk nic_disconnect
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK
;
;@fn NET_LISTEN
;@ndk nic_listen
;@arg lport u16 local port -> NIC_LPORTL/H (ARG1 byte0,1)
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK
;
;@fn NET_ACCEPT
;@ndk nic_accept
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK
;
;@fn NET_SEND
;@ndk nic_send
;@arg dataptr u16 pointer to bytes -> NIC_DMAL/H (ARG0 byte0,1)
;@arg len u8 byte count -> NIC_DMALEN (ARG0 byte2)
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK
;
;@fn NET_RECV
;@ndk nic_recv
;@arg dstptr u16 destination pointer -> NIC_DMAL/H (ARG0 byte0,1)
;@arg slot u8 NIC slot 0..3 -> NIC_SLOT (ARG3 byte0)
;@ret void
;@status LERR_OK
;
;@fn NET_STATUS
;@ndk nic_status
;@arg slot u8 NIC slot 0..3 -> X (ARG3 byte0)
;@ret u8 clamped slot status (RESULT byte0)
;@status LERR_OK
;
;@fn NET_READY
;@ndk nic_ready
;@arg slot u8 NIC slot 0..3 -> X (ARG3 byte0)
;@ret u8 1 when data/accept ready else 0 (RESULT byte0)
;@status LERR_OK
;
;@fn NET_LENGTH
;@ndk nic_length
;@ret u8 most recent NIC message length (RESULT byte0)
;@status LERR_OK
;
;@fn NET_NGS_INIT
;@ndk ngs_init
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_SET_BUFFER
;@ndk ngs_set_buffer
;@arg bufptr u16 256-byte CPU RAM frame buffer -> A/X (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_SET_STRING
;@ndk ngs_set_string
;@arg strptr u16 pointer to host/string/payload bytes -> A/X (ARG0 byte0,1)
;@arg strlen u8 string length -> Y (ARG0 byte2)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_CONNECT
;@ndk ngs_connect
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_CONNECT_DEFAULT
;@ndk ngs_connect_default
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_DISCONNECT
;@ndk ngs_disconnect
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_STATUS
;@ndk ngs_status
;@ret u8 NGS_SLOT status (RESULT byte0)
;@status LERR_OK
;
;@fn NET_NGS_READY
;@ndk ngs_ready
;@ret u8 1 when a message is queued else 0 (RESULT byte0)
;@status LERR_OK
;
;@fn NET_NGS_SET_TIMEOUT
;@ndk ngs_set_timeout
;@arg frames u16 wait timeout in video frames, 0=forever -> A/X (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_KEEPALIVE_SET_INTERVAL
;@ndk ngs_keepalive_set_interval
;@arg frames u16 keepalive ping interval in frames, 0=off -> A/X (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_KEEPALIVE_RESET
;@ndk ngs_keepalive_reset
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_KEEPALIVE_TICK
;@ndk ngs_keepalive_tick
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_WAIT_CONNECTED
;@ndk ngs_wait_connected
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_SEND_CURRENT
;@ndk ngs_send_current
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_RECV_CURRENT
;@ndk ngs_recv_current
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_WAIT_MESSAGE
;@ndk ngs_wait_message
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_WAIT_KIND
;@ndk ngs_wait_kind
;@arg kind u8 expected message kind -> A (ARG0 byte0)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_PING_ROUNDTRIP
;@ndk ngs_ping_roundtrip
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_NGS_PARSE_ENVELOPE
;@ndk ngs_parse_envelope
;@ret u8 parsed message kind (RESULT byte0)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_HELLO
;@ndk ngs_build_hello
;@arg cap u16 client capability bits -> NGS_CLIENT_CAPL/H (ARG0 byte0,1)
;@arg gmaj u8 game proto major -> NGS_GAME_PROTO_MAJOR (ARG1 byte0)
;@arg gmin u8 game proto minor -> NGS_GAME_PROTO_MINOR (ARG1 byte1)
;@ret void (handle = current string set via NET_NGS_SET_STRING)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_PING
;@ndk ngs_build_ping
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_LOBBY_CHAT
;@ndk ngs_build_lobby_chat
;@ret void (text = current string set via NET_NGS_SET_STRING)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_CHAT
;@ndk ngs_build_table_chat
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@ret void (text = current string set via NET_NGS_SET_STRING)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_USER_LIST_REQUEST
;@ndk ngs_build_user_list_request
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_GAME_LIST_REQUEST
;@ndk ngs_build_game_list_request
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_LIST_REQUEST
;@ndk ngs_build_table_list_request
;@arg game u16 filter game id, 0=unfiltered -> NGS_GAME_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_CREATE
;@ndk ngs_build_table_create
;@arg game u16 game id -> NGS_GAME_IDL/H (ARG0 byte0,1)
;@arg gmaj u8 game proto major -> NGS_GAME_PROTO_MAJOR (ARG1 byte0)
;@arg gmin u8 game proto minor -> NGS_GAME_PROTO_MINOR (ARG1 byte1)
;@ret void (table name = current string set via NET_NGS_SET_STRING)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_JOIN
;@ndk ngs_build_table_join
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@arg role u8 seat role (0=player,1=observer) -> NGS_ROLE (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_OBSERVE
;@ndk ngs_build_table_observe
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_TABLE_LEAVE
;@ndk ngs_build_table_leave
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_PLAYER_ACTION
;@ndk ngs_build_player_action
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@arg action u8 action type -> NGS_ACTION_TYPE (ARG1 byte0)
;@ret void (payload = current string set via NET_NGS_SET_STRING)
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_STATE_REQUEST
;@ndk ngs_build_state_request
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_AI_LIST_REQUEST
;@ndk ngs_build_ai_list_request
;@arg game u16 filter game id, 0=unfiltered -> NGS_GAME_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_AI_ADD_TO_TABLE
;@ndk ngs_build_ai_add_to_table
;@arg table u16 table id -> NGS_TABLE_IDL/H (ARG0 byte0,1)
;@arg provider u16 AI provider id -> NGS_VALUE_L/H (ARG1 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL
;
;@fn NET_BUILD_OPPONENT_LIST_REQUEST
;@ndk ngs_build_opponent_list_request
;@arg game u16 filter game id, 0=unfiltered -> NGS_GAME_IDL/H (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_NET_FAIL

; ---------------------------------------------------------------------------
; dispatch — fn-id router (RTS-trick). NET_FN_COUNT is small so fn*2 < 256.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #NET_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     net_jtable+1,x
      pha
      lda     net_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

net_jtable:
      .word   net_connect-1                  ; $00 NET_CONNECT
      .word   net_disconnect-1               ; $01 NET_DISCONNECT
      .word   net_listen-1                   ; $02 NET_LISTEN
      .word   net_accept-1                   ; $03 NET_ACCEPT
      .word   net_send-1                     ; $04 NET_SEND
      .word   net_recv-1                     ; $05 NET_RECV
      .word   net_status-1                   ; $06 NET_STATUS
      .word   net_ready-1                    ; $07 NET_READY
      .word   net_length-1                   ; $08 NET_LENGTH
      .word   net_ngs_init-1                 ; $09 NET_NGS_INIT
      .word   net_ngs_set_buffer-1           ; $0A NET_NGS_SET_BUFFER
      .word   net_ngs_set_string-1           ; $0B NET_NGS_SET_STRING
      .word   net_ngs_connect-1              ; $0C NET_NGS_CONNECT
      .word   net_ngs_connect_default-1      ; $0D NET_NGS_CONNECT_DEFAULT
      .word   net_ngs_disconnect-1           ; $0E NET_NGS_DISCONNECT
      .word   net_ngs_status-1               ; $0F NET_NGS_STATUS
      .word   net_ngs_ready-1                ; $10 NET_NGS_READY
      .word   net_ngs_set_timeout-1          ; $11 NET_NGS_SET_TIMEOUT
      .word   net_ngs_ka_set_interval-1      ; $12 NET_NGS_KEEPALIVE_SET_INTERVAL
      .word   net_ngs_ka_reset-1             ; $13 NET_NGS_KEEPALIVE_RESET
      .word   net_ngs_ka_tick-1              ; $14 NET_NGS_KEEPALIVE_TICK
      .word   net_ngs_wait_connected-1       ; $15 NET_NGS_WAIT_CONNECTED
      .word   net_ngs_send_current-1         ; $16 NET_NGS_SEND_CURRENT
      .word   net_ngs_recv_current-1         ; $17 NET_NGS_RECV_CURRENT
      .word   net_ngs_wait_message-1         ; $18 NET_NGS_WAIT_MESSAGE
      .word   net_ngs_wait_kind-1            ; $19 NET_NGS_WAIT_KIND
      .word   net_ngs_ping_roundtrip-1       ; $1A NET_NGS_PING_ROUNDTRIP
      .word   net_ngs_parse_envelope-1       ; $1B NET_NGS_PARSE_ENVELOPE
      .word   net_build_hello-1              ; $1C NET_BUILD_HELLO
      .word   net_build_ping-1               ; $1D NET_BUILD_PING
      .word   net_build_lobby_chat-1         ; $1E NET_BUILD_LOBBY_CHAT
      .word   net_build_table_chat-1         ; $1F NET_BUILD_TABLE_CHAT
      .word   net_build_user_list_req-1      ; $20 NET_BUILD_USER_LIST_REQUEST
      .word   net_build_game_list_req-1      ; $21 NET_BUILD_GAME_LIST_REQUEST
      .word   net_build_table_list_req-1     ; $22 NET_BUILD_TABLE_LIST_REQUEST
      .word   net_build_table_create-1       ; $23 NET_BUILD_TABLE_CREATE
      .word   net_build_table_join-1         ; $24 NET_BUILD_TABLE_JOIN
      .word   net_build_table_observe-1      ; $25 NET_BUILD_TABLE_OBSERVE
      .word   net_build_table_leave-1        ; $26 NET_BUILD_TABLE_LEAVE
      .word   net_build_player_action-1      ; $27 NET_BUILD_PLAYER_ACTION
      .word   net_build_state_request-1      ; $28 NET_BUILD_STATE_REQUEST
      .word   net_build_ai_list_req-1        ; $29 NET_BUILD_AI_LIST_REQUEST
      .word   net_build_ai_add_to_table-1    ; $2A NET_BUILD_AI_ADD_TO_TABLE
      .word   net_build_opp_list_req-1       ; $2B NET_BUILD_OPPONENT_LIST_REQUEST

; ===========================================================================
; Shared epilogues. net_finish_status maps the NDK A result (0=OK / nonzero=err)
; to LIB_STATUS and zeroes RESULT. net_finish_ok always reports OK (command
; issuers that post an MMIO command and do not return a 0/1 status). These are
; NOT ;@ndk-mapped, so the drift guard's per-wrapper JSR check ignores them.
; ===========================================================================
net_finish_status:
      cmp     #$00
      bne     net_fail
net_finish_ok:
      stz     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
net_fail:
      lda     #LERR_NET_FAIL
      sta     LIB_STATUS
      rts

; net_report_a — publish A (a reporter byte) to RESULT byte0 and report LERR_OK.
; (NOT ;@ndk-mapped; ignored by the drift JSR check.)
net_report_a:
      sta     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; net_marshal_table — ARG0 byte0/1 -> NGS_TABLE_IDL/H. Shared by the table-id
; builders. (NOT ;@ndk-mapped.)
net_marshal_table:
      LDA   LIB_ARG0+0
      STA   NGS_TABLE_IDL
      LDA   LIB_ARG0+1
      STA   NGS_TABLE_IDH
      RTS

; net_marshal_game — ARG0 byte0/1 -> NGS_GAME_IDL/H. Shared by the game-filter
; builders. (NOT ;@ndk-mapped.)
net_marshal_game:
      LDA   LIB_ARG0+0
      STA   NGS_GAME_IDL
      LDA   LIB_ARG0+1
      STA   NGS_GAME_IDH
      RTS

; ===========================================================================
; Raw NIC slot commands (nic.s; NIC_* MMIO ABI). The nic_* command issuers post
; an MMIO command and do not return a 0/1 status, so these report LERR_OK once
; the command is issued (the slot status reporters surface success/failure).
; ===========================================================================

; --- $00 NET_CONNECT: name ptr/len -> NIC_NAMEBUF, ARG1 rport, ARG3 b0 slot -> nic_connect ---
net_connect:
      LDA   LIB_ARG0+0
      STA   NIC_ARG_NAMEPTR_L
      LDA   LIB_ARG0+1
      STA   NIC_ARG_NAMEPTR_H
      LDA   LIB_ARG0+2
      STA   NIC_ARG_NAMELEN
      JSR   nic_copy_name
      CMP   #NIC_RESULT_OK
      BNE   net_fail
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      LDA   LIB_ARG1+0
      STA   NIC_RPORTL
      LDA   LIB_ARG1+1
      STA   NIC_RPORTH
      JSR   nic_connect
      JMP   net_finish_ok

; --- $01 NET_DISCONNECT: ARG3 b0 slot -> NIC_SLOT -> nic_disconnect ---
net_disconnect:
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      JSR   nic_disconnect
      JMP   net_finish_ok

; --- $02 NET_LISTEN: ARG1 lport -> NIC_LPORTL/H, ARG3 b0 slot -> nic_listen ---
net_listen:
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      LDA   LIB_ARG1+0
      STA   NIC_LPORTL
      LDA   LIB_ARG1+1
      STA   NIC_LPORTH
      JSR   nic_listen
      JMP   net_finish_ok

; --- $03 NET_ACCEPT: ARG3 b0 slot -> NIC_SLOT -> nic_accept ---
net_accept:
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      JSR   nic_accept
      JMP   net_finish_ok

; --- $04 NET_SEND: ARG0 data ptr -> NIC_DMAL/H, ARG0 b2 len, ARG3 b0 slot -> nic_send ---
net_send:
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      LDA   LIB_ARG0+0
      STA   NIC_DMAL
      LDA   LIB_ARG0+1
      STA   NIC_DMAH
      LDA   LIB_ARG0+2
      STA   NIC_DMALEN
      JSR   nic_send
      JMP   net_finish_ok

; --- $05 NET_RECV: ARG0 dst ptr -> NIC_DMAL/H, ARG3 b0 slot -> nic_recv ---
net_recv:
      LDA   LIB_ARG3+0
      STA   NIC_SLOT
      LDA   LIB_ARG0+0
      STA   NIC_DMAL
      LDA   LIB_ARG0+1
      STA   NIC_DMAH
      JSR   nic_recv
      JMP   net_finish_ok

; --- $06 NET_STATUS: ARG3 b0 slot -> X -> nic_status -> RESULT b0 = A ---
net_status:
      LDX   LIB_ARG3+0
      JSR   nic_status
      JMP   net_report_a

; --- $07 NET_READY: ARG3 b0 slot -> X -> nic_ready -> RESULT b0 = A ---
net_ready:
      LDX   LIB_ARG3+0
      JSR   nic_ready
      JMP   net_report_a

; --- $08 NET_LENGTH: () -> nic_length -> RESULT b0 = A ---
net_length:
      JSR   nic_length
      JMP   net_report_a

; ===========================================================================
; Game-server session + control (gameserver.s; NGS_* BSS-state ABI). These
; routines return A=0 (NGS_OK) / nonzero on error, so they use net_finish_status.
; ===========================================================================

; --- $09 NET_NGS_INIT: () -> ngs_init ---
net_ngs_init:
      JSR   ngs_init
      JMP   net_finish_status

; --- $0A NET_NGS_SET_BUFFER: ARG0 ptr -> A/X -> ngs_set_buffer ---
net_ngs_set_buffer:
      LDA   LIB_ARG0+0
      LDX   LIB_ARG0+1
      JSR   ngs_set_buffer
      JMP   net_finish_status

; --- $0B NET_NGS_SET_STRING: ARG0 ptr -> A/X, ARG0 b2 len -> Y -> ngs_set_string ---
net_ngs_set_string:
      LDA   LIB_ARG0+0
      LDX   LIB_ARG0+1
      LDY   LIB_ARG0+2
      JSR   ngs_set_string
      JMP   net_finish_status

; --- $0C NET_NGS_CONNECT: () -> ngs_connect ---
net_ngs_connect:
      JSR   ngs_connect
      JMP   net_finish_status

; --- $0D NET_NGS_CONNECT_DEFAULT: () -> ngs_connect_default ---
net_ngs_connect_default:
      JSR   ngs_connect_default
      JMP   net_finish_status

; --- $0E NET_NGS_DISCONNECT: () -> ngs_disconnect ---
net_ngs_disconnect:
      JSR   ngs_disconnect
      JMP   net_finish_status

; --- $0F NET_NGS_STATUS: () -> ngs_status -> RESULT b0 = A ---
net_ngs_status:
      JSR   ngs_status
      JMP   net_report_a

; --- $10 NET_NGS_READY: () -> ngs_ready -> RESULT b0 = A ---
net_ngs_ready:
      JSR   ngs_ready
      JMP   net_report_a

; --- $11 NET_NGS_SET_TIMEOUT: ARG0 frames -> A/X -> ngs_set_timeout ---
net_ngs_set_timeout:
      LDA   LIB_ARG0+0
      LDX   LIB_ARG0+1
      JSR   ngs_set_timeout
      JMP   net_finish_status

; --- $12 NET_NGS_KEEPALIVE_SET_INTERVAL: ARG0 frames -> A/X -> ngs_keepalive_set_interval ---
net_ngs_ka_set_interval:
      LDA   LIB_ARG0+0
      LDX   LIB_ARG0+1
      JSR   ngs_keepalive_set_interval
      JMP   net_finish_status

; --- $13 NET_NGS_KEEPALIVE_RESET: () -> ngs_keepalive_reset ---
net_ngs_ka_reset:
      JSR   ngs_keepalive_reset
      JMP   net_finish_status

; --- $14 NET_NGS_KEEPALIVE_TICK: () -> ngs_keepalive_tick ---
net_ngs_ka_tick:
      JSR   ngs_keepalive_tick
      JMP   net_finish_status

; --- $15 NET_NGS_WAIT_CONNECTED: () -> ngs_wait_connected ---
net_ngs_wait_connected:
      JSR   ngs_wait_connected
      JMP   net_finish_status

; ===========================================================================
; Game-server messaging (gameserver.s).
; ===========================================================================

; --- $16 NET_NGS_SEND_CURRENT: () -> ngs_send_current ---
net_ngs_send_current:
      JSR   ngs_send_current
      JMP   net_finish_status

; --- $17 NET_NGS_RECV_CURRENT: () -> ngs_recv_current ---
net_ngs_recv_current:
      JSR   ngs_recv_current
      JMP   net_finish_status

; --- $18 NET_NGS_WAIT_MESSAGE: () -> ngs_wait_message ---
net_ngs_wait_message:
      JSR   ngs_wait_message
      JMP   net_finish_status

; --- $19 NET_NGS_WAIT_KIND: ARG0 b0 kind -> A -> ngs_wait_kind ---
net_ngs_wait_kind:
      LDA   LIB_ARG0+0
      JSR   ngs_wait_kind
      JMP   net_finish_status

; --- $1A NET_NGS_PING_ROUNDTRIP: () -> ngs_ping_roundtrip ---
net_ngs_ping_roundtrip:
      JSR   ngs_ping_roundtrip
      JMP   net_finish_status

; --- $1B NET_NGS_PARSE_ENVELOPE: () -> ngs_parse_envelope -> RESULT b0 = NGS_KIND ---
net_ngs_parse_envelope:
      JSR   ngs_parse_envelope
      CMP   #$00
      BEQ   @ok
      JMP   net_fail
@ok:
      LDA   NGS_KIND
      STA   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Game-server request builders (gameserver.s). Each marshals its ARG inputs into
; the NGS_* BSS fields the builder reads, then JSRs the builder. The handle/text/
; payload string rides in the NGS_STRPTRL/H:NGS_STRLEN state set by a prior
; NET_NGS_SET_STRING. Send the built frame with NET_NGS_SEND_CURRENT.
; ===========================================================================

; --- $1C NET_BUILD_HELLO: ARG0 cap -> NGS_CLIENT_CAP*, ARG1 b0/b1 gproto -> ngs_build_hello ---
net_build_hello:
      LDA   LIB_ARG0+0
      STA   NGS_CLIENT_CAPL
      LDA   LIB_ARG0+1
      STA   NGS_CLIENT_CAPH
      LDA   LIB_ARG1+0
      STA   NGS_GAME_PROTO_MAJOR
      LDA   LIB_ARG1+1
      STA   NGS_GAME_PROTO_MINOR
      JSR   ngs_build_hello
      JMP   net_finish_status

; --- $1D NET_BUILD_PING: () -> ngs_build_ping ---
net_build_ping:
      JSR   ngs_build_ping
      JMP   net_finish_status

; --- $1E NET_BUILD_LOBBY_CHAT: () -> ngs_build_lobby_chat (text = current string) ---
net_build_lobby_chat:
      JSR   ngs_build_lobby_chat
      JMP   net_finish_status

; --- $1F NET_BUILD_TABLE_CHAT: ARG0 table -> NGS_TABLE_ID* -> ngs_build_table_chat ---
net_build_table_chat:
      JSR   net_marshal_table
      JSR   ngs_build_table_chat
      JMP   net_finish_status

; --- $20 NET_BUILD_USER_LIST_REQUEST: () -> ngs_build_user_list_request ---
net_build_user_list_req:
      JSR   ngs_build_user_list_request
      JMP   net_finish_status

; --- $21 NET_BUILD_GAME_LIST_REQUEST: () -> ngs_build_game_list_request ---
net_build_game_list_req:
      JSR   ngs_build_game_list_request
      JMP   net_finish_status

; --- $22 NET_BUILD_TABLE_LIST_REQUEST: ARG0 game -> NGS_GAME_ID* -> ngs_build_table_list_request ---
net_build_table_list_req:
      JSR   net_marshal_game
      JSR   ngs_build_table_list_request
      JMP   net_finish_status

; --- $23 NET_BUILD_TABLE_CREATE: ARG0 game -> NGS_GAME_ID*, ARG1 b0/b1 gproto -> ngs_build_table_create ---
net_build_table_create:
      JSR   net_marshal_game
      LDA   LIB_ARG1+0
      STA   NGS_GAME_PROTO_MAJOR
      LDA   LIB_ARG1+1
      STA   NGS_GAME_PROTO_MINOR
      JSR   ngs_build_table_create
      JMP   net_finish_status

; --- $24 NET_BUILD_TABLE_JOIN: ARG0 table -> NGS_TABLE_ID*, ARG1 b0 role -> ngs_build_table_join ---
net_build_table_join:
      JSR   net_marshal_table
      LDA   LIB_ARG1+0
      STA   NGS_ROLE
      JSR   ngs_build_table_join
      JMP   net_finish_status

; --- $25 NET_BUILD_TABLE_OBSERVE: ARG0 table -> NGS_TABLE_ID* -> ngs_build_table_observe ---
net_build_table_observe:
      JSR   net_marshal_table
      JSR   ngs_build_table_observe
      JMP   net_finish_status

; --- $26 NET_BUILD_TABLE_LEAVE: ARG0 table -> NGS_TABLE_ID* -> ngs_build_table_leave ---
net_build_table_leave:
      JSR   net_marshal_table
      JSR   ngs_build_table_leave
      JMP   net_finish_status

; --- $27 NET_BUILD_PLAYER_ACTION: ARG0 table -> NGS_TABLE_ID*, ARG1 b0 action -> ngs_build_player_action ---
net_build_player_action:
      JSR   net_marshal_table
      LDA   LIB_ARG1+0
      STA   NGS_ACTION_TYPE
      JSR   ngs_build_player_action
      JMP   net_finish_status

; --- $28 NET_BUILD_STATE_REQUEST: ARG0 table -> NGS_TABLE_ID* -> ngs_build_state_request ---
net_build_state_request:
      JSR   net_marshal_table
      JSR   ngs_build_state_request
      JMP   net_finish_status

; --- $29 NET_BUILD_AI_LIST_REQUEST: ARG0 game -> NGS_GAME_ID* -> ngs_build_ai_list_request ---
net_build_ai_list_req:
      JSR   net_marshal_game
      JSR   ngs_build_ai_list_request
      JMP   net_finish_status

; --- $2A NET_BUILD_AI_ADD_TO_TABLE: ARG0 table -> NGS_TABLE_ID*, ARG1 provider -> NGS_VALUE_L/H -> ngs_build_ai_add_to_table ---
net_build_ai_add_to_table:
      JSR   net_marshal_table
      LDA   LIB_ARG1+0
      STA   NGS_VALUE_L
      LDA   LIB_ARG1+1
      STA   NGS_VALUE_H
      JSR   ngs_build_ai_add_to_table
      JMP   net_finish_status

; --- $2B NET_BUILD_OPPONENT_LIST_REQUEST: ARG0 game -> NGS_GAME_ID* -> ngs_build_opponent_list_request ---
net_build_opp_list_req:
      JSR   net_marshal_game
      JSR   ngs_build_opponent_list_request
      JMP   net_finish_status

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced(nic_*/ngs_*) is
; true (selective emit keeps the image small). gameserver.s itself .includes
; nic.s (via its .include "nic.inc"), so including gameserver.s pulls both; the
; selective-emit guards keep only the wrapped routines + their transitive
; callees. NIC_SERVER_COMMANDS is forced on so nic_listen/nic_accept are emitted.
; ===========================================================================
NIC_SERVER_COMMANDS = 1
      .include "gameserver.inc"

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)
