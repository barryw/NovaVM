; XRAM shared-runtime integration fixture.
;
; Loaded at $7000 by tests/integration/xram_asm.6502 and entered through
; BASIC SYS. The fixture includes the same lib/xram.inc/lib/xram.s library that
; standalone assembly programs are expected to reuse.

.setcpu "65c02"

.include "xram.inc"

RESULT_BASE     = $2200
COPY_SRC        = $2300
COPY_DST        = $2310
FILL_DST        = $2320
FILE_SRC        = $2340
FILE_DST        = $2350

XADDR_BYTE_L    = $45
XADDR_BYTE_M    = $23
XADDR_BYTE_H    = $01
XADDR_COPY_L    = $00
XADDR_COPY_M    = $01
XADDR_COPY_H    = $02
XADDR_FILL_L    = $00
XADDR_FILL_M    = $02
XADDR_FILL_H    = $03
XADDR_FILE_SRC_L = $00
XADDR_FILE_SRC_M = $03
XADDR_FILE_SRC_H = $04
XADDR_FILE_DST_L = $10
XADDR_FILE_DST_M = $03
XADDR_FILE_DST_H = $04

.segment "CODE"

; $9000: byte, DMA copy, fill, and range-check tests.
start:
        jmp asm_xram_dma_entry

; $9003: direct file streaming tests.
        jmp asm_xram_file_entry

asm_xram_dma_entry:
        pha
        phx
        phy
        jsr asm_xram_dma_test
        ply
        plx
        pla
        rts

asm_xram_file_entry:
        pha
        phx
        phy
        jsr asm_xram_file_test
        ply
        plx
        pla
        rts

asm_xram_dma_test:
        jsr clear_results

        lda #XADDR_BYTE_L
        sta XRAM_ADDRL
        lda #XADDR_BYTE_M
        sta XRAM_ADDRM
        lda #XADDR_BYTE_H
        sta XRAM_ADDRH
        lda #$5A
        sta XRAM_DATA
        jsr xram_write8
        sta RESULT_BASE

        stz XRAM_DATA
        jsr xram_read8
        sta RESULT_BASE + 1
        lda XRAM_DATA
        sta RESULT_BASE + 2

        ldx #$00
@copy_seed:
        txa
        ora #$A0
        sta COPY_SRC,x
        stz COPY_DST,x
        inx
        cpx #$10
        bne @copy_seed

        lda #XADDR_COPY_L
        sta XRAM_ADDRL
        lda #XADDR_COPY_M
        sta XRAM_ADDRM
        lda #XADDR_COPY_H
        sta XRAM_ADDRH
        lda #<COPY_SRC
        sta XRAM_RAML
        lda #>COPY_SRC
        sta XRAM_RAMH
        lda #$10
        sta XRAM_LENL
        stz XRAM_LENH
        jsr xram_copy_from_ram
        sta RESULT_BASE + 3

        lda #<COPY_DST
        sta XRAM_RAML
        lda #>COPY_DST
        sta XRAM_RAMH
        jsr xram_copy_to_ram
        sta RESULT_BASE + 4

        lda #XADDR_FILL_L
        sta XRAM_ADDRL
        lda #XADDR_FILL_M
        sta XRAM_ADDRM
        lda #XADDR_FILL_H
        sta XRAM_ADDRH
        lda #$08
        sta XRAM_LENL
        stz XRAM_LENH
        lda #$7E
        sta XRAM_DATA
        jsr xram_fill
        sta RESULT_BASE + 5

        lda #<FILL_DST
        sta XRAM_RAML
        lda #>FILL_DST
        sta XRAM_RAMH
        jsr xram_copy_to_ram
        sta RESULT_BASE + 6

        stz XRAM_ADDRL
        stz XRAM_ADDRM
        lda #$08
        sta XRAM_ADDRH
        jsr xram_read8
        sta RESULT_BASE + 7
        lda XMC_ERRCODE
        sta RESULT_BASE + 8

        lda #$AA
        sta RESULT_BASE + $0F
        rts

asm_xram_file_test:
        ldx #$20
        lda #$00
@clear_file_results:
        sta RESULT_BASE,x
        inx
        cpx #$40
        bne @clear_file_results

        lda #$11
        sta FILE_SRC
        lda #$22
        sta FILE_SRC + 1
        lda #$33
        sta FILE_SRC + 2
        lda #$44
        sta FILE_SRC + 3
        stz FILE_DST
        stz FILE_DST + 1
        stz FILE_DST + 2
        stz FILE_DST + 3

        lda #XADDR_FILE_SRC_L
        sta XRAM_ADDRL
        lda #XADDR_FILE_SRC_M
        sta XRAM_ADDRM
        lda #XADDR_FILE_SRC_H
        sta XRAM_ADDRH
        lda #<FILE_SRC
        sta XRAM_RAML
        lda #>FILE_SRC
        sta XRAM_RAMH
        lda #$04
        sta XRAM_LENL
        stz XRAM_LENH
        jsr xram_copy_from_ram
        sta RESULT_BASE + $20

        jsr set_file_name
        jsr xram_xsave
        sta RESULT_BASE + $21

        lda #XADDR_FILE_SRC_L
        sta XRAM_ADDRL
        lda #XADDR_FILE_SRC_M
        sta XRAM_ADDRM
        lda #XADDR_FILE_SRC_H
        sta XRAM_ADDRH
        lda #$04
        sta XRAM_LENL
        stz XRAM_LENH
        stz XRAM_DATA
        jsr xram_fill
        sta RESULT_BASE + $26

        lda #XADDR_FILE_DST_L
        sta XRAM_ADDRL
        lda #XADDR_FILE_DST_M
        sta XRAM_ADDRM
        lda #XADDR_FILE_DST_H
        sta XRAM_ADDRH
        stz XRAM_LENL
        stz XRAM_LENH
        jsr set_file_name
        jsr xram_xload
        sta RESULT_BASE + $22
        lda XRAM_LENL
        sta RESULT_BASE + $23
        lda XRAM_LENH
        sta RESULT_BASE + $24

        lda #<FILE_DST
        sta XRAM_RAML
        lda #>FILE_DST
        sta XRAM_RAMH
        jsr xram_copy_to_ram
        sta RESULT_BASE + $25

        lda #$AA
        sta RESULT_BASE + $2F
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$60
        bne @loop
        rts

set_file_name:
        lda #<xram_file_name
        sta XRAM_NAMEPTR_L
        lda #>xram_file_name
        sta XRAM_NAMEPTR_H
        lda #(xram_file_name_end - xram_file_name)
        sta XRAM_NAMELEN
        rts

xram_file_name:
        .byte "XASMRT"
xram_file_name_end:

.include "xram.s"
