; =====================================================================
;  Nova NDK — libnet.inc
;
;  NET module (MODULE_ID_NET = $06) fn-id contract. Home of the NIC helper
;  runtime (nic.s) and the Nova Game Server client protocol runtime
;  (gameserver.s): raw NIC slot connect/listen/accept/send/recv + status
;  reporters, plus the game-server session, messaging, and the 16 request
;  builders. Every fn is a THIN lib_call wrapper that marshals the mailbox
;  and JSRs an NDK routine.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Note the prefix: ids use NET_* (the networking-domain library). NIC_*/NGS_* are
; reserved for the MMIO register / game-server BSS-state blocks (nic.inc, nova.inc,
; gameserver.inc) — those are different namespaces.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_NET
;   LIB_FN_ID  ($0301) = one of the NET_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result (reporters only)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; MAILBOX MARSHALLING (each fn's ;@arg in net.s documents exactly what it uses):
;   Slot/port/name args ride in the ARG cells and are copied into the NIC_* MMIO
;   registers (NIC_SLOT, NIC_RPORTL/H, NIC_LPORTL/H, NIC_DMAL/H, NIC_DMALEN) or
;   the NGS_* BSS client-state fields (NGS_SLOT, NGS_GAME_IDL/H, NGS_TABLE_IDL/H,
;   NGS_ROLE, NGS_ACTION_TYPE, NGS_VALUE_L/H, ...) before the JSR. Host names and
;   payload/string args ride as a CPU pointer (ARG byte0/byte1) + length (byte2)
;   and are staged into NIC_NAMEBUF (via nic_copy_name) or NGS_STRPTRL/H:NGS_STRLEN
;   (via ngs_set_string) per the wrapped routine's contract.
;
; STATUS MAPPING: the NGS_* routines return A=0 (NGS_OK) on success / nonzero on
;   error (NGS_RESULT holds the same code); the wrapper maps A!=0 -> LIB_STATUS =
;   LERR_NET_FAIL, A==0 -> LERR_OK. The raw nic_* command issuers do not return a
;   0/1 status (they post an MMIO command), so their wrappers report LERR_OK once
;   the command is issued. Reporters publish their result cell to LIB_RESULT with
;   LIB_STATUS=OK.

.ifndef LIBNET_INC
LIBNET_INC = 1

.include "libabi.inc"            ; MODULE_ID_NET, LIB_* mailbox cells, LERR_*

; --- Status code returned when a wrapped NGS_* NDK routine reports failure ---
LERR_NET_FAIL  = $8C   ; ngs_* NDK routine returned A!=0 (NIC/protocol error / bad arg)

; =====================================================================
; Raw NIC slot commands + reporters (nic.s; NIC_* MMIO ABI)
; =====================================================================
NET_CONNECT      = $00  ; (nameptr:ARG0 u16, namelen:ARG0 b2, rport:ARG1 u16, slot:ARG3 b0) -> connect slot
NET_DISCONNECT   = $01  ; (slot:ARG3 b0)                                      -> disconnect slot
NET_LISTEN       = $02  ; (lport:ARG1 u16, slot:ARG3 b0)                      -> listen on local port
NET_ACCEPT       = $03  ; (slot:ARG3 b0)                                      -> accept pending connection
NET_SEND         = $04  ; (dataptr:ARG0 u16, len:ARG0 b2, slot:ARG3 b0)       -> send bytes on slot
NET_RECV         = $05  ; (dstptr:ARG0 u16, slot:ARG3 b0)                     -> receive into dst
NET_STATUS       = $06  ; (slot:ARG3 b0)                                      -> RESULT b0 = clamped slot status
NET_READY        = $07  ; (slot:ARG3 b0)                                      -> RESULT b0 = 1 when data/accept ready
NET_LENGTH       = $08  ; ()                                                  -> RESULT b0 = last NIC message length

; =====================================================================
; Game-server session + control (gameserver.s; NGS_* BSS-state ABI)
; =====================================================================
NET_NGS_INIT     = $09  ; ()                                                  -> init client state, default port 6503
NET_NGS_SET_BUFFER = $0A ; (bufptr:ARG0 u16)                                  -> set 256-byte frame buffer
NET_NGS_SET_STRING = $0B ; (strptr:ARG0 u16, strlen:ARG0 b2)                  -> set host/string/payload ptr+len
NET_NGS_CONNECT  = $0C  ; ()                                                  -> connect NGS_SLOT using current string
NET_NGS_CONNECT_DEFAULT = $0D ; ()                                            -> connect to default Nova endpoint
NET_NGS_DISCONNECT = $0E ; ()                                                 -> disconnect NGS_SLOT
NET_NGS_STATUS   = $0F  ; ()                                                  -> RESULT b0 = NGS_SLOT status
NET_NGS_READY    = $10  ; ()                                                  -> RESULT b0 = 1 when a message is queued
NET_NGS_SET_TIMEOUT = $11 ; (frames:ARG0 u16)                                 -> set shared wait timeout (0=forever)
NET_NGS_KEEPALIVE_SET_INTERVAL = $12 ; (frames:ARG0 u16)                      -> set keepalive ping interval (0=off)
NET_NGS_KEEPALIVE_RESET = $13 ; ()                                            -> reset keepalive countdown
NET_NGS_KEEPALIVE_TICK = $14 ; ()                                             -> send periodic PING when due
NET_NGS_WAIT_CONNECTED = $15 ; ()                                             -> wait for NGS_SLOT to connect/fail

; =====================================================================
; Game-server messaging (gameserver.s)
; =====================================================================
NET_NGS_SEND_CURRENT = $16 ; ()                                               -> send the current built frame
NET_NGS_RECV_CURRENT = $17 ; ()                                               -> receive + parse one queued frame
NET_NGS_WAIT_MESSAGE = $18 ; ()                                               -> wait for + receive one frame
NET_NGS_WAIT_KIND = $19 ; (kind:ARG0 b0)                                      -> wait for a frame of the given kind
NET_NGS_PING_ROUNDTRIP = $1A ; ()                                             -> send PING, wait for PONG
NET_NGS_PARSE_ENVELOPE = $1B ; ()                                             -> RESULT b0 = parsed message kind

; =====================================================================
; Game-server request builders (gameserver.s; build into the current frame)
; Each marshals its inputs into the NGS_* BSS fields the builder reads, then
; JSRs the builder. Send the built frame afterwards with NET_NGS_SEND_CURRENT.
; =====================================================================
NET_BUILD_HELLO  = $1C  ; (cap:ARG0 u16, gmaj:ARG1 b0, gmin:ARG1 b1)          -> HELLO (handle = current string)
NET_BUILD_PING   = $1D  ; ()                                                  -> empty PING
NET_BUILD_LOBBY_CHAT = $1E ; ()                                               -> LOBBY_CHAT (text = current string)
NET_BUILD_TABLE_CHAT = $1F ; (table:ARG0 u16)                                 -> TABLE_CHAT (text = current string)
NET_BUILD_USER_LIST_REQUEST = $20 ; ()                                        -> USER_LIST_REQUEST
NET_BUILD_GAME_LIST_REQUEST = $21 ; ()                                        -> GAME_LIST_REQUEST
NET_BUILD_TABLE_LIST_REQUEST = $22 ; (game:ARG0 u16)                          -> TABLE_LIST_REQUEST (0=unfiltered)
NET_BUILD_TABLE_CREATE = $23 ; (game:ARG0 u16, gmaj:ARG1 b0, gmin:ARG1 b1)    -> TABLE_CREATE (name = current string)
NET_BUILD_TABLE_JOIN = $24 ; (table:ARG0 u16, role:ARG1 b0)                   -> TABLE_JOIN
NET_BUILD_TABLE_OBSERVE = $25 ; (table:ARG0 u16)                              -> TABLE_OBSERVE
NET_BUILD_TABLE_LEAVE = $26 ; (table:ARG0 u16)                                -> TABLE_LEAVE
NET_BUILD_PLAYER_ACTION = $27 ; (table:ARG0 u16, action:ARG1 b0)              -> PLAYER_ACTION (payload = current string)
NET_BUILD_STATE_REQUEST = $28 ; (table:ARG0 u16)                              -> STATE_REQUEST
NET_BUILD_AI_LIST_REQUEST = $29 ; (game:ARG0 u16)                             -> AI_LIST_REQUEST (0=unfiltered)
NET_BUILD_AI_ADD_TO_TABLE = $2A ; (table:ARG0 u16, provider:ARG1 u16)         -> AI_ADD_TO_TABLE
NET_BUILD_OPPONENT_LIST_REQUEST = $2B ; (game:ARG0 u16)                       -> OPPONENT_LIST_REQUEST (0=unfiltered)

; Highest implemented fn-id + 1. Gaps / ids >= NET_FN_COUNT -> LERR_NO_FN.
NET_FN_COUNT   = $2C

.endif
