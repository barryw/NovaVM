; xramcheck.s -- Hardware smoke test for the XRAM path the toolchain depends on.
;
; The Pascal compiler, assembler and linker all stage source through XRAM: the
; NDK Memory module allocates a block (an XMC ALLOC command, executed by the
; ROM's software XMC engine over the XMC windows) and every refill reads it
; back through those windows. When that path is wrong the tools fail in ways
; that point everywhere but here -- "file:1:1: error: Pascal syntax error" for
; every source, "Not enough XRAM." from the assembler -- so this program tests
; the two layers directly and prints what it saw.
;
;   Build: make -C software/assembly xramcheck
;   Run:   RUN XRAMCHK.BIN   (NovaPascal shell)   or   SYS from BASIC

.include "nova.inc"
.include "libabi.inc"
.include "libmemory.inc"
.include "libfiles.inc"

WIN0 = $BC00

.segment "HEADER"
        .word   $8000                   ; load address prefix the shell reads

.segment "ZEROPAGE"
zp_ptr:      .res 2
zp_count:    .res 1

.segment "BSS"
saved_winctl: .res 1
readback:     .res 4

.segment "CODE"
start:
        LDA     #<msg_banner
        LDY     #>msg_banner
        JSR     print

        ; ---- Layer 1: the XMC window itself -----------------------------
        ; Map window 0 onto the top XRAM page (the allocator hands out low
        ; addresses first, so the top page is the least likely to be live).
        LDA     XMC_WINCTL
        STA     saved_winctl
        ORA     #$01
        STA     XMC_WINCTL
        STZ     XMC_W0AL
        LDA     #$FF
        STA     XMC_W0AM
        LDA     #$07
        STA     XMC_W0AH

        LDX     #0
@write: LDA     pattern,X
        STA     WIN0,X
        INX
        CPX     #4
        BNE     @write

        LDX     #0
@read:  LDA     WIN0,X
        STA     readback,X
        INX
        CPX     #4
        BNE     @read

        LDA     #<msg_window
        LDY     #>msg_window
        JSR     print
        LDX     #0
@show:  LDA     readback,X
        JSR     print_hex
        LDA     #' '
        STA     VGC_CHAROUT
        INX
        CPX     #4
        BNE     @show

        LDX     #0
@cmp:   LDA     readback,X
        CMP     pattern,X
        BNE     @win_bad
        INX
        CPX     #4
        BNE     @cmp
        LDA     #<msg_ok
        LDY     #>msg_ok
        JSR     print
        JMP     @alloc
@win_bad:
        LDA     #<msg_bad
        LDY     #>msg_bad
        JSR     print

        ; ---- Layer 2: the allocator the tools actually call --------------
@alloc:
        LDA     saved_winctl
        STA     XMC_WINCTL

        LDA     #<msg_alloc
        LDY     #>msg_alloc
        JSR     print
        LDX     #0
        LDA     #0
@clear: STA     LIB_ARG0,X
        INX
        CPX     #16
        BNE     @clear
        LDA     #$00                    ; request 16 KiB, what NPC asks for
        STA     LIB_ARG2+0
        LDA     #$40
        STA     LIB_ARG2+1
        LDA     #MEM_ALLOC
        STA     LIB_FN_ID
        LDA     #MODULE_ID_MEMORY
        STA     LIB_MOD_ID
        JSR     LIB_LOADER_BAND

        LDA     LIB_STATUS
        JSR     print_hex
        LDA     #' '
        STA     VGC_CHAROUT
        LDA     LIB_RESULT+2
        JSR     print_hex
        LDA     LIB_RESULT+1
        JSR     print_hex
        LDA     LIB_RESULT+0
        JSR     print_hex
        JSR     newline

        LDA     LIB_STATUS
        BEQ     @alloc_ok
        LDA     #<msg_bad
        LDY     #>msg_bad
        JSR     print
        JMP     @done
@alloc_ok:
        LDA     #<msg_ok
        LDY     #>msg_ok
        JSR     print

        ; ---- Layer 3: host -> XRAM -> CPU, the direction the tools live on --
        ; MEM_XLOAD has the host stream a file straight into XRAM; the compiler
        ; then reads it back through the windows. If the two sides do not see
        ; one memory, every source page reads as zeros and every build dies on
        ; the first character with a syntax error.
        LDA     #<msg_xload
        LDY     #>msg_xload
        JSR     print
        LDX     #0
        LDA     #0
@clear2:
        STA     LIB_ARG0,X
        INX
        CPX     #16
        BNE     @clear2
        LDA     #<probe_name
        STA     LIB_ARG0+0
        LDA     #>probe_name
        STA     LIB_ARG0+1
        LDA     #probe_name_len
        STA     LIB_ARG1+0
        LDA     #$00                    ; XRAM $070000: clear of the low arena
        STA     LIB_ARG2+0
        STZ     LIB_ARG2+1
        LDA     #$07
        STA     LIB_ARG2+2
        STZ     LIB_ARG3+0
        STZ     LIB_ARG3+1
        LDA     #MEM_XLOAD
        STA     LIB_FN_ID
        LDA     #MODULE_ID_MEMORY
        STA     LIB_MOD_ID
        JSR     LIB_LOADER_BAND
        LDA     LIB_STATUS
        JSR     print_hex
        LDA     #' '
        STA     VGC_CHAROUT

        LDA     XMC_WINCTL
        ORA     #$01
        STA     XMC_WINCTL
        STZ     XMC_W0AL
        STZ     XMC_W0AM
        LDA     #$07
        STA     XMC_W0AH
        LDX     #0
@dump:  LDA     WIN0,X
        CMP     #$20
        BCC     @dot
        CMP     #$7F
        BCS     @dot
        STA     VGC_CHAROUT
        BRA     @next_dump
@dot:   LDA     #'.'
        STA     VGC_CHAROUT
@next_dump:
        INX
        CPX     #16
        BNE     @dump
        JSR     newline

        ; ---- Layer 4: FILE_PAGE, the call NPC refills its source cache with -
        LDA     #<msg_page
        LDY     #>msg_page
        JSR     print
        LDX     #0
        LDA     #0
@clear3:
        STA     LIB_ARG0,X
        INX
        CPX     #16
        BNE     @clear3
        LDA     #<probe_name
        STA     LIB_ARG0+0
        LDA     #>probe_name
        STA     LIB_ARG0+1
        LDA     #probe_name_len
        STA     LIB_ARG0+2
        STZ     LIB_ARG1+0              ; file offset 0
        STZ     LIB_ARG1+1
        STZ     LIB_ARG1+2
        LDA     #$00                    ; target 0 = XRAM
        STA     LIB_ARG1+3
        STZ     LIB_ARG2+0              ; XRAM $060000
        STZ     LIB_ARG2+1
        LDA     #$06
        STA     LIB_ARG2+2
        LDA     #$20                    ; 32 bytes
        STA     LIB_ARG3+0
        STZ     LIB_ARG3+1
        LDA     #FILE_PAGE
        STA     LIB_FN_ID
        LDA     #MODULE_ID_FILES
        STA     LIB_MOD_ID
        JSR     LIB_LOADER_BAND
        LDA     LIB_STATUS
        JSR     print_hex
        LDA     #' '
        STA     VGC_CHAROUT
        LDA     LIB_RESULT+0
        JSR     print_hex
        LDA     #' '
        STA     VGC_CHAROUT

        LDA     XMC_WINCTL
        ORA     #$01
        STA     XMC_WINCTL
        STZ     XMC_W0AL
        STZ     XMC_W0AM
        LDA     #$06
        STA     XMC_W0AH
        LDX     #0
@dump2: LDA     WIN0,X
        CMP     #$20
        BCC     @dot2
        CMP     #$7F
        BCS     @dot2
        STA     VGC_CHAROUT
        BRA     @next2
@dot2:  LDA     #'.'
        STA     VGC_CHAROUT
@next2: INX
        CPX     #16
        BNE     @dump2
        JSR     newline

@done:
        LDA     #<msg_done
        LDY     #>msg_done
        JSR     print
        LDA     #MEM_EXIT_IMAGE
        STA     LIB_FN_ID
        LDA     #MODULE_ID_MEMORY
        STA     LIB_MOD_ID
        JSR     LIB_LOADER_BAND
        RTS

; ---- helpers -------------------------------------------------------------
print:  STA     zp_ptr
        STY     zp_ptr+1
        LDY     #0
@loop:  LDA     (zp_ptr),Y
        BEQ     @end
        STA     VGC_CHAROUT
        INY
        BNE     @loop
@end:   RTS

newline:
        LDA     #$0D
        STA     VGC_CHAROUT
        LDA     #$0A
        STA     VGC_CHAROUT
        RTS

print_hex:
        PHA
        LSR     A
        LSR     A
        LSR     A
        LSR     A
        JSR     print_nib
        PLA
        AND     #$0F
print_nib:
        CMP     #10
        BCC     @digit
        CLC
        ADC     #'A'-10
        STA     VGC_CHAROUT
        RTS
@digit: CLC
        ADC     #'0'
        STA     VGC_CHAROUT
        RTS

pattern:      .byte $A5, $5A, $C3, $3C
msg_banner:   .byte "XRAM check", $0D, $0A, 0
msg_window:   .byte "window r/w: ", 0
msg_alloc:    .byte "alloc status/addr: ", 0
msg_ok:       .byte "OK", $0D, $0A, 0
msg_bad:      .byte "FAILED", $0D, $0A, 0
msg_xload:    .byte "host->xram: ", 0
msg_page:     .byte "file_page:  ", 0
msg_done:     .byte "XRAM check done", $0D, $0A, 0
probe_name:   .byte "HELLO/MAIN.PAS"
probe_name_len = 14
