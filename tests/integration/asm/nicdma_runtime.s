; NIC DMA error-propagation fixture.
;
; Drives ngs_send_current against a bus double that faults the payload DMA
; (NIC_DMAERR = RANGE). A send whose DMA failed transferred no bytes, so the
; routine must surface a nonzero NGS_RESULT instead of reporting success.
; NicDmaAssemblyRuntimeTests supplies the faulting bus.

.setcpu "w65c02"

.include "gameserver.inc"

RESULT   = $2200
FRAMEBUF = $3000

.segment "CODE"

start:
        jmp run_send

run_send:
        stz NGS_SLOT
        lda #<FRAMEBUF
        sta NGS_BUFL
        lda #>FRAMEBUF
        sta NGS_BUFH
        lda #NGS_ENVELOPE_SIZE     ; minimal valid frame length
        sta NGS_LEN
        stz NGS_RESULT
        jsr ngs_send_current
        sta RESULT + 0             ; A = returned NGS_RESULT
        lda NGS_RESULT
        sta RESULT + 1
        lda #$AA
        sta RESULT + 2
        rts

.include "gameserver.s"
