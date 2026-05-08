; Nova platform hooks for chess6502-engine.
;
; Nova's NVR pseudo-registers ($20-$2F) remain reserved for shared Nova APIs.
; The Nova engine build remaps the engine's old low-ZP scratch away from that
; range. These first-cut hooks still avoid NVR entirely because callback-local
; state should live in BSS or engine-owned scratch, not persistent pseudo regs.

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
