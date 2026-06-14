; Nova wrapper for the vendored chess6502-engine.
;
; The UI runtime lives in the $C000 runtime ROM. This image is loaded from disk
; into CPU RAM at $4000 and exposes the engine's Chess* public API labels there.

.setcpu "65c02"

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
.export BestMoveFrom
.export BestMoveTo
.export MoveCount
.export MoveListFrom
.export MoveListTo
.export currentplayer
.export difficulty
.export TTClear

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
        jmp ChessFindBestMove

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
        jmp TTClear

chess_engine_start:
        jsr nova_chess_tt_clear
        lda #WHITES_TURN
        sta currentplayer
        jsr ChessBeginGame
        rts

.include "constants.s"
.include "engine/engine.s"

; Minimal memory-fill service required by the engine TT clear path.
FillMemory:
        ldy #$00
        ldx fill_size + $01
        beq @frag_fill
@page_fill:
        lda fill_value
        sta (fill_to), y
        iny
        bne @page_fill
        inc fill_to + $01
        dex
        bne @page_fill
@frag_fill:
        cpy fill_size
        beq @done
        lda fill_value
        sta (fill_to), y
        iny
        bne @frag_fill
@done:
        rts

.include "engine_platform_nova.s"
