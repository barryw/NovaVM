; page_in_checksum_stub.s — Phase-3 Step-3a HW page-in silicon proof.
;
; Runs from RAM (loaded at $9F80). With bank 1 (ext_rom) selected via ROMSWAP,
; sums the 16 KB currently mapped at $C000-$FFFF and stores the 16-bit total
; little-endian at SUM ($9FE0/$9FE1). Restores ROMSWAP=BASIC and the clobbered
; zero-page pointer before returning. Called via USR/JSR from BASIC.
;
; Must run from RAM: flipping ROMSWAP to the extension bank unmaps the running
; ROM at $C000-$FFFF, so the loop body and its return address must live below
; $A000. SUM/SAVE sit in the protected high-RAM region the BASIC driver carves
; out by lowering Ememh/Sstor before calling.
;
; On FPGA hardware irq_n/nmi_n are tied high (no interrupts), so the SEI/CLI are
; defensive only; they keep the routine correct in the Avalonia emulator (and
; under the PageInChecksumStubTests unit test) where IRQs can fire.
;
; Assemble:
;   ca65 tools/page_in_checksum_stub.s -o /tmp/stub.o
;   ld65 -C tools/page_in_checksum_stub.cfg /tmp/stub.o -o tools/page_in_checksum_stub.bin
; Behaviour is guarded by e6502UnitTests/PageInChecksumStubTests.cs.

.segment "CODE"

ROMSWAP  = $A03F
RS_EXT   = $04          ; ROMSWAP_EXTENSION — maps bank 1 (ext_rom) at $C000
RS_BASIC = $02          ; ROMSWAP_BASIC
PTR      = $FB          ; zero-page indirect pointer (saved/restored)
SUM      = $9FE0        ; 16-bit checksum result (protected high RAM)
SAVE     = $9FE2        ; saved PTR (2 bytes)

stub:
        sei                     ; defensive (no IRQ on HW; matters in emulator)
        lda     PTR
        sta     SAVE
        lda     PTR+1
        sta     SAVE+1
        lda     #$00
        sta     SUM
        sta     SUM+1
        sta     PTR             ; PTR = $C000 (A still 0)
        lda     #$C0
        sta     PTR+1
        lda     #RS_EXT
        sta     ROMSWAP         ; map bank 1 (paged module) at $C000
        ldx     #$40            ; 64 pages = 16 KB
        ldy     #$00
@byte:
        lda     (PTR),y
        clc
        adc     SUM
        sta     SUM
        bcc     @nocarry
        inc     SUM+1
@nocarry:
        iny
        bne     @byte           ; inner 256-byte page
        inc     PTR+1
        dex
        bne     @byte           ; next page (Y wrapped to 0)
        lda     #RS_BASIC
        sta     ROMSWAP         ; restore BASIC ROM at $C000
        lda     SAVE
        sta     PTR
        lda     SAVE+1
        sta     PTR+1
        cli
        rts
