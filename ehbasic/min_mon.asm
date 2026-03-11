; minimal monitor for EhBASIC and 6502 simulator V1.05
; tabs converted to space, tabwidth=6

; To run EhBASIC on the simulator load and assemble [F7] this file, start the simulator
; running [F6] then start the code with the RESET [CTRL][SHIFT]R. Just selecting RUN
; will do nothing, you'll still have to do a reset to run the code.

      .include "basic.asm"

; put the IRQ and NMI code in RAM so that it can be changed

IRQ_vec     = VEC_SV+2        ; IRQ code vector
NMI_vec     = IRQ_vec+$0A     ; NMI code vector
; EXT_vec defined in basic.asm (= VEC_SV+$16 = $0221)
EXT_RST     = EXT_vec+$12     ; extension ROM reset recovery in RAM

; setup for the 6502 simulator environment

IO_AREA     = $F000           ; set I/O area for this monitor

ACIAsimwr   = $A00E           ; VGC CHAROUT
ACIAsimrd   = $A00F           ; VGC CHARIN

; --- Data block copied to RAM at boot (vectors + interrupt + extension code) ---
; This block is stored in ROM (CODE segment) and copied to VEC_IN at init.

LAB_vec
      .word ACIAin            ; byte in from simulated ACIA
      .word ACIAout           ; byte out to simulated ACIA
      .word LAB_FLOAD         ; load vector for EhBASIC
      .word LAB_FSAVE         ; save vector for EhBASIC

; EhBASIC IRQ support

IRQ_CODE
      PHA                     ; save A
      LDA   IrqBase           ; get the IRQ flag byte
      LSR                     ; shift the set b7 to b6, and on down ...
      ORA   IrqBase           ; OR the original back in
      STA   IrqBase           ; save the new IRQ flag byte
      PLA                     ; restore A
      RTI

; EhBASIC NMI support

NMI_CODE
      PHA                     ; save A
      LDA   NmiBase           ; get the NMI flag byte
      LSR                     ; shift the set b7 to b6, and on down ...
      ORA   NmiBase           ; OR the original back in
      STA   NmiBase           ; save the new NMI flag byte
      PLA                     ; restore A
      RTI

; Extension ROM call trampoline (runs from RAM)
; Called with command ID in A. Swaps to extension ROM, calls $C000,
; swaps back to BASIC ROM, returns with handler result in A.

EXT_CODE
      STA   ExtCmdId          ; save command ID to ZP
      LDA   #$04              ; RomSwapExtension
      STA   $A03F             ; swap to extension ROM
      JSR   $C000             ; call extension entry point
      PHA                     ; save handler return value
      LDA   #$02              ; RomSwapBasic
      STA   $A03F             ; swap back to BASIC ROM
      PLA                     ; restore handler return value
      RTS

; Extension ROM reset recovery (runs from RAM)
; If RESET fires while extension ROM is active, extension ROM's reset
; handler jumps here. Swaps back to BASIC ROM and continues reset.

EXT_RESET_CODE
      LDA   #$02              ; RomSwapBasic
      STA   $A03F             ; swap to BASIC ROM
      JMP   $FFBD             ; jump to BASIC reset handler (RES_vec)

END_CODE

; --- Monitor code (in MONITOR segment at $FFBD) ---

      .segment "MONITOR"
      .org    $FFBD

; reset vector points here

RES_vec
      CLD                     ; clear decimal mode
      LDX   #$FF              ; empty stack
      TXS                     ; set the stack

; set up vectors, interrupt code, and extension trampoline — copy to page 2

      LDY   #END_CODE-LAB_vec ; set index/count
LAB_stlp
      LDA   LAB_vec-1,Y       ; get byte from ROM data block
      STA   VEC_IN-1,Y        ; save to RAM
      DEY                     ; decrement index/count
      BNE   LAB_stlp          ; loop if more to do

      JMP   LAB_COLD          ; always do EhBASIC cold start

; byte out to simulated ACIA

ACIAout
      STA   ACIAsimwr         ; save byte to simulated ACIA
      RTS

; byte in from simulated ACIA

ACIAin
      LDA   ACIAsimrd         ; get byte from simulated ACIA
      BEQ   LAB_nobyw         ; branch if no byte waiting

      SEC                     ; flag byte received
      RTS

LAB_nobyw
      CLC                     ; flag no byte received
      RTS

; system vectors

      .segment "VECTORS"
      .org    $FFFA

      .word NMI_vec           ; NMI vector
      .word RES_vec           ; RESET vector
      .word IRQ_vec           ; IRQ vector
