; =====================================================================
;  NovaLogo — tokens.s
;
;  Token node format and type constants shared by the tokenizer and eval.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Token type constants
TOK_WORD     = $01            ; payload: length byte + chars
TOK_NUMBER   = $02            ; payload: 3 bytes (hi, lo, frac) — 16.8 fixed-point
TOK_LBRACKET = $03            ; no payload
TOK_RBRACKET = $04            ; no payload
TOK_VARREF   = $05            ; payload: length byte + chars (name after :)
TOK_QUOTE    = $06            ; payload: length byte + chars (word after ")
TOK_INFIX    = $07            ; payload: 1 byte (operator char)
TOK_LPAREN   = $08            ; no payload
TOK_RPAREN   = $09            ; no payload

; Token node layout:
;   Offset 0: tag byte (TOK_* constant)
;   Offset 1: next pointer low byte
;   Offset 2: next pointer high byte ($00/$00 = end of list)
;   Offset 3+: payload (varies by type)
;
; Total size: 3 + payload_size
;   TOK_WORD/VARREF/QUOTE: 3 + 1 + len (length byte + chars)
;   TOK_NUMBER: 3 + 3 = 6 bytes
;   TOK_LBRACKET/RBRACKET/LPAREN/RPAREN: 3 bytes (no payload)
;   TOK_INFIX: 3 + 1 = 4 bytes

TOK_TAG      = 0              ; offset to tag byte
TOK_NEXT_LO  = 1              ; offset to next pointer low
TOK_NEXT_HI  = 2              ; offset to next pointer high
TOK_PAYLOAD  = 3              ; offset to start of payload
