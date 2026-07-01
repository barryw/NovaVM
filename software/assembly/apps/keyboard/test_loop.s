; test_loop.s -- Minimal SYS $9000 loop test
; Proves: LOAD works, SYS enters, main loop holds, ESC exits to BASIC.
;
; Load address: $9000   Invoke: SYS $9000
; Build: ca65 --cpu w65c02 -I ../../../runtime/asm test_loop.s -o test_loop.o
;        ld65 -C keyboard.cfg -o test_loop.bin test_loop.o vgc_wait.o

.include "novavm.inc"

.import vgc_wait_cmd

.segment "ZEROPAGE"
zp_frame:       .res 1
zp_counter_lo:  .res 1
zp_counter_hi:  .res 1

.segment "BSS"
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_cursor:    .res 1

.segment "HEADER"
    .byte $00, $90

.segment "CODE"

main:
    lda RegMode
    sta save_mode
    lda RegBgCol
    sta save_bgcol
    lda RegFgCol
    sta save_fgcol
    lda RegCursorEnable
    sta save_cursor

    lda #1
    sta RegMode
    lda #0
    sta RegBgCol
    lda #1
    sta RegFgCol
    stz RegCursorEnable

    lda #CmdGcls
    sta RegCmd
    jsr vgc_wait_cmd

    jsr clear_text

    ; Title
    lda #10
    sta RegCursorY
    lda #25
    sta RegCursorX
    ldx #0
@title:
    lda str_title,x
    beq @status_label
    sta RegCharOut
    inx
    bra @title

@status_label:
    lda #14
    sta RegCursorY
    lda #25
    sta RegCursorX
    ldx #0
@slbl:
    lda str_status,x
    beq @exit_label
    sta RegCharOut
    inx
    bra @slbl

@exit_label:
    lda #18
    sta RegCursorY
    lda #24
    sta RegCursorX
    ldx #0
@elbl:
    lda str_exit,x
    beq @init_loop
    sta RegCharOut
    inx
    bra @elbl

@init_loop:
    stz zp_counter_lo
    stz zp_counter_hi
    lda RegStatus
    sta zp_frame

@loop:
    ; Wait vsync
@wv:
    lda RegStatus
    cmp zp_frame
    beq @wv
    sta zp_frame

    ; Increment and display frame counter
    inc zp_counter_lo
    bne @show
    inc zp_counter_hi
@show:
    lda #14
    sta RegCursorY
    lda #38
    sta RegCursorX
    lda zp_counter_hi
    jsr print_hex
    lda zp_counter_lo
    jsr print_hex

    ; Check key
    lda RegCharIn
    beq @loop
    cmp #$1B
    beq @exit
    cmp #'Q'
    beq @exit
    cmp #'q'
    beq @exit
    bra @loop

@exit:
    lda #CmdGcls
    sta RegCmd
    jsr vgc_wait_cmd

    lda save_mode
    sta RegMode
    lda save_bgcol
    sta RegBgCol
    lda save_fgcol
    sta RegFgCol
    lda save_cursor
    sta RegCursorEnable

    jsr clear_text
    rts

; Print A as 2 hex digits
print_hex:
    pha
    lsr
    lsr
    lsr
    lsr
    tax
    lda hex_chars,x
    sta RegCharOut
    pla
    and #$0F
    tax
    lda hex_chars,x
    sta RegCharOut
    rts

clear_text:
    ldy #0
@row:
    sty RegCursorY
    ldx #0
@col:
    stx RegCursorX
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @col
    iny
    cpy #50
    bcc @row
    stz RegCursorX
    stz RegCursorY
    rts

.segment "RODATA"

hex_chars: .byte "0123456789ABCDEF"
str_title:  .byte "LOOP TEST - SYS $9000", 0
str_status: .byte "FRAME COUNT: ", 0
str_exit:   .byte "PRESS ESC/Q TO EXIT", 0
