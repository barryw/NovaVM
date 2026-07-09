; =====================================================================
;  NovaChess — rules_nova.s
;
;  Nova wrapper for the rules-only chess6502-engine build. Loaded into the
;  same $4000 engine slot as the full AI build; the nova_chess_* ABI stubs
;  match engine_nova.s in size/order (AI-only entries are RTS stubs) so the
;  runtime can call the shared rules surface regardless of which image is
;  resident.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.setcpu "w65c02"

.export nova_chess_engine_start
.export nova_chess_init_piece_lists
.export nova_chess_generate_legal_moves
.export nova_chess_find_best_move
.export nova_chess_make_move
.export nova_chess_begin_game
.export nova_chess_commit_move
.export nova_chess_unmake_move
.export nova_chess_is_square_attacked
.export nova_chess_check_king_in_check
.export nova_chess_record_position
.export nova_chess_clear_position_history
.export nova_chess_check_repetition
.export nova_chess_check_game_state
.export nova_chess_tt_clear
.export Board88
.export MoveCount
.export MoveListFrom
.export MoveListTo
.export currentplayer
.export difficulty

ENGINE_FIXED_PST = 0
ENGINE_TT_BASE = $9000
ENGINE_NOVA_ZP = 1
ENGINE_CPU_65C02 = 1

.segment "LOADADDR"
        .word $4000

.segment "CODE"
nova_chess_engine_start:
        jmp chess_engine_start

nova_chess_init_piece_lists:
        jmp ChessInitPieceLists

nova_chess_generate_legal_moves:
        jmp ChessGenerateLegalMoves

nova_chess_find_best_move:
        rts
        nop
        nop

nova_chess_make_move:
        jmp ChessMakeMove

nova_chess_begin_game:
        jmp ChessBeginGame

nova_chess_commit_move:
        jmp ChessCommitMove

nova_chess_unmake_move:
        jmp ChessUnmakeMove

nova_chess_is_square_attacked:
        jmp ChessIsSquareAttacked

nova_chess_check_king_in_check:
        jmp ChessCheckKingInCheck

nova_chess_record_position:
        jmp ChessRecordPosition

nova_chess_clear_position_history:
        jmp ChessClearPositionHistory

nova_chess_check_repetition:
        jmp ChessCheckRepetition

nova_chess_check_game_state:
        jmp ChessCheckGameState

nova_chess_tt_clear:
        rts
        nop
        nop

chess_engine_start:
        jsr nova_chess_tt_clear
        lda #WHITES_TURN
        sta currentplayer
        jsr ChessBeginGame
        rts

.include "constants.s"
.include "engine/rules_engine.s"
