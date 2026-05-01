; xram_shared_runtime_demo.s
;
; Standalone assembly example for the shared XRAM runtime. Load at $9000
; and run from BASIC with SYS 36864.
;
; Assemble from the repository root:
;   ca65 --cpu 65c02 -I ehbasic -o /tmp/xram_demo.o docs/programs/xram_shared_runtime_demo.s
;   ld65 -C docs/programs/diag9000.cfg -o /tmp/xram_demo.bin /tmp/xram_demo.o

.setcpu "65c02"

.include "lib/xram.inc"

BUFFER      = $2300
RESTORED    = $2310
MSG_LEN     = message_end - message

.segment "CODE"

start:
        pha
        phx
        phy

        ldx #$00
copy_message:
        lda message,x
        sta BUFFER,x
        stz RESTORED,x
        inx
        cpx #MSG_LEN
        bne copy_message

        ; Store BUFFER in flat XRAM address $010000.
        stz XRAM_ADDRL
        stz XRAM_ADDRM
        lda #$01
        sta XRAM_ADDRH
        lda #<BUFFER
        sta XRAM_RAML
        lda #>BUFFER
        sta XRAM_RAMH
        lda #MSG_LEN
        sta XRAM_LENL
        stz XRAM_LENH
        jsr xram_copy_from_ram
        bne error

        ; Fetch it back into RESTORED.
        lda #<RESTORED
        sta XRAM_RAML
        lda #>RESTORED
        sta XRAM_RAMH
        lda #MSG_LEN
        sta XRAM_LENL
        stz XRAM_LENH
        jsr xram_copy_to_ram
        bne error

        ldx #$00
print_prefix:
        lda ok_prefix,x
        beq print_restored
        sta VGC_CHAROUT
        inx
        bra print_prefix

print_restored:
        ldx #$00
print_loop:
        lda RESTORED,x
        sta VGC_CHAROUT
        inx
        cpx #MSG_LEN
        bne print_loop
        lda #$0D
        sta VGC_CHAROUT
        bra done

error:
        ldx #$00
error_loop:
        lda err_text,x
        beq done
        sta VGC_CHAROUT
        inx
        bra error_loop

done:
        ply
        plx
        pla
        rts

ok_prefix:
        .byte "XRAM OK: ",0
err_text:
        .byte "XRAM ERROR",13,0
message:
        .byte "NOVA XRAM"
message_end:

.include "lib/xram.s"
