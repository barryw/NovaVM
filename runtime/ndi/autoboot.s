; Generic NovaVM language/runtime launcher (shared by every language floppy).
;
; BASIC's autoboot loads this as AUTOBOOT.bin at $7200, then JMPs to it. It asks
; NovaHost to stream the floppy's runtime ROM (always stored as RUNTIME.BIN) into
; the primary $C000 bank, then jumps through that runtime's reset vector. Each
; language floppy (novaforth.ndi, novalogo.ndi, ...) packs its own runtime under
; the name RUNTIME.BIN, so this one launcher is reused verbatim.

.setcpu "w65c02"

.include "fio.inc"

.segment "ZEROPAGE"
msg_ptr: .res 2

.segment "HEADER"
        .byte $00, $72                  ; load address $7200

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
        wai
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
        .byte "RUNTIME.BIN"
runtime_name_end:

msg_failed:
        .byte "RUNTIME LOAD FAILED", 0

; FIO implementation is linked from runtime/asm/build/nova.lib.
