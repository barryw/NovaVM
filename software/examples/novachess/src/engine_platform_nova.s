; =====================================================================
;  NovaChess — engine_platform_nova.s
;
;  Nova platform hooks for chess6502-engine: search-timer, time-check,
;  and opening-book callbacks. Keeps callback-local state out of Nova's
;  reserved NVR pseudo-registers ($20-$2F), using engine-owned scratch.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.segment "CODE"

.ifndef NOVA_ENGINE_MAX_ITER_DEPTH
NOVA_ENGINE_MAX_ITER_DEPTH = $04
.endif

EngineStartSearchTimer:
        rts

EngineCheckTime:
        lda IterDepth
        cmp #NOVA_ENGINE_MAX_ITER_DEPTH
        bcc @time_ok
        lda #$01
        sta TimeUp
        sec
        rts
@time_ok:
        clc
        rts

EngineOnSearchIteration:
        rts

EngineLookupOpeningMove:
        clc
        rts

EngineBookMoveAvoidsPawnAttack:
        sec
        rts
