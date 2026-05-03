; NIC shared-runtime integration fixture.
;
; Loaded at $7000 by tests/integration/nic_asm.6502 and entered through
; BASIC SYS. The fixture includes the same lib/nic.inc/lib/nic.s library that
; standalone assembly programs are expected to reuse.

.setcpu "65c02"

.include "nic.inc"

RESULT_BASE = $2280
SEND_BUF    = $22A0

.segment "CODE"

start:
        jmp asm_nic_entry

asm_nic_entry:
        pha
        phx
        phy
        jsr asm_nic_test
        ply
        plx
        pla
        rts

asm_nic_test:
        jsr clear_results

        lda #host_name_end - host_name
        sta NIC_ARG_NAMELEN
        lda #<host_name
        sta NIC_ARG_NAMEPTR_L
        lda #>host_name
        sta NIC_ARG_NAMEPTR_H
        jsr nic_copy_name
        sta RESULT_BASE

        lda NIC_NAMEBUF
        sta RESULT_BASE + 1
        lda NIC_NAMEBUF + (host_name_end - host_name)
        sta RESULT_BASE + 2

        stz NIC_ARG_NAMELEN
        jsr nic_copy_name
        sta RESULT_BASE + 3

        ldx #0
        jsr nic_status
        sta RESULT_BASE + 4

        ldx #0
        jsr nic_ready
        sta RESULT_BASE + 5

        stz NIC_SLOT
        jsr nic_disconnect
        lda NIC_CMD
        sta RESULT_BASE + 6

        lda #'O'
        sta SEND_BUF
        lda #'K'
        sta SEND_BUF + 1
        lda #$0A
        sta SEND_BUF + 2
        lda #<SEND_BUF
        sta NIC_DMAL
        lda #>SEND_BUF
        sta NIC_DMAH
        lda #3
        sta NIC_DMALEN
        stz NIC_SLOT
        jsr nic_send
        lda NIC_CMD
        sta RESULT_BASE + 7

        ldx #0
        jsr nic_status
        sta RESULT_BASE + 8

        jsr nic_length
        sta RESULT_BASE + 9

        lda #$AA
        sta RESULT_BASE + $0F
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$20
        bne @loop
        rts

host_name:
        .byte "nova.test"
host_name_end:

.include "nic.s"
