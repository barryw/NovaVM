; minimal monitor for EhBASIC and 6502 simulator V1.05
; tabs converted to space, tabwidth=6

; To run EhBASIC on the simulator load and assemble [F7] this file, start the simulator
; running [F6] then start the code with the RESET [CTRL][SHIFT]R. Just selecting RUN
; will do nothing, you'll still have to do a reset to run the code.

      .include "basic.asm"

; put the IRQ and MNI code in RAM so that it can be changed

IRQ_vec     = VEC_SV+2        ; IRQ code vector
NMI_vec     = IRQ_vec+$0A     ; NMI code vector

; setup for the 6502 simulator environment

IO_AREA     = $F000           ; set I/O area for this monitor

ACIAsimwr   = $A00E           ; VGC CHAROUT
ACIAsimrd   = $A00F           ; VGC CHARIN

; now the code. all this does is set up the vectors and interrupt code
; then jump directly to BASIC cold start.

      .segment "MONITOR"
      .org    $FF80

; reset vector points here

RES_vec
      CLD                     ; clear decimal mode
      LDX   #$FF              ; empty stack
      TXS                     ; set the stack

; set up vectors and interrupt code, copy them to page 2

      LDY   #END_CODE-LAB_vec ; set index/count
LAB_stlp
      LDA   LAB_vec-1,Y       ; get byte from interrupt code
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

; vector tables

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

END_CODE

; system vectors

      .segment "VECTORS"
      .org    $FFFA

      .word NMI_vec           ; NMI vector
      .word RES_vec           ; RESET vector
      .word IRQ_vec           ; IRQ vector
