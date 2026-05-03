; Nova Ozmoo runtime launcher.
;
; BASIC loads this as AUTOBOOT.bin into RAM. It asks NovaHost to stream the
; Ozmoo runtime ROM into the primary $C000 bank, then jumps through that
; runtime's reset vector. BASIC-only autoboot images do not use this path.

.setcpu "65c02"

.include "fio.inc"

.segment "ZEROPAGE"
msg_ptr: .res 2

.segment "HEADER"
        .byte $00, $72

.segment "CODE"

start:
        SEI
        CLD
        JSR load_runtime
        BEQ boot_runtime

        LDA #<msg_failed
        LDY #>msg_failed
        JSR print_line
halt:
        WAI
        BRA halt

load_runtime:
        LDA #<runtime_name
        STA FIO_ARG_NAMEPTR_L
        LDA #>runtime_name
        STA FIO_ARG_NAMEPTR_H
        LDA #(runtime_name_end - runtime_name)
        STA FIO_ARG_NAMELEN
        JSR fio_copy_name
        BNE @done
        JMP fio_load_runtime
@done:
        RTS

boot_runtime:
        LDX #$FF
        TXS
        LDA #ROMSWAP_PRIMARY
        STA REG_ROMSWAP
        JMP ($FFFC)

; A/Y = pointer to null-terminated string.
print_line:
        STA msg_ptr
        STY msg_ptr + 1
        LDY #$00
@loop:
        LDA (msg_ptr),Y
        BEQ @newline
        STA VGC_CHAROUT
        INY
        BNE @loop
        INC msg_ptr + 1
        BRA @loop
@newline:
        LDA #$0D
        STA VGC_CHAROUT
        LDA #$0A
        STA VGC_CHAROUT
        RTS

.segment "RODATA"

runtime_name:
        .byte "OZMOO.BIN"
runtime_name_end:

msg_failed:
        .byte "OZMOO RUNTIME LOAD FAILED", 0

.include "fio.s"
