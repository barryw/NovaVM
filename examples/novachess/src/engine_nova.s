; Nova wrapper for the vendored chess6502-engine.
;
; The UI runtime lives in the $C000 runtime ROM. This image is loaded from disk
; into CPU RAM at $4000 and exposes the engine's Chess* public API labels there.

.setcpu "65c02"

.export chess_engine_start
.export ChessInitPieceLists
.export ChessGenerateLegalMoves
.export ChessFindBestMove
.export ChessMakeMove
.export ChessBeginGame
.export ChessCommitMove
.export ChessUnmakeMove
.export ChessIsSquareAttacked
.export ChessCheckKingInCheck
.export ChessRecordPosition
.export ChessClearPositionHistory
.export ChessCheckRepetition
.export ChessCheckGameState
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
ENGINE_TT_BASE = $A200
ENGINE_NOVA_ZP = 1
ENGINE_CPU_65C02 = 1

.segment "LOADADDR"
        .word $4000

.segment "CODE"
chess_engine_start:
        jsr TTClear
        lda #WHITES_TURN
        sta currentplayer
        jsr ChessBeginGame
        rts

.include "constants.s"

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
.include "engine/engine.s"
