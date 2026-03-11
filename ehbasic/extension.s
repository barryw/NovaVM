; Extension ROM for NovaVM (bank 4)
; Contains overflow command handlers called from BASIC ROM via RAM trampoline.
;
; Entry point at $C000: reads ExtCmdId from zero page and dispatches.
; Handlers access hardware registers directly — never call BASIC ROM routines.

; --- Zero page communication ---
ExtCmdId        = $E4           ; command ID (set by RAM trampoline)

; --- Hardware registers ---
VGC_CHAROUT     = $A00E         ; write character to screen
VGC_CHARIN      = $A00F         ; read character (0 = none)
REG_ROMSWAP     = $A03F         ; ROM bank switch register
FIO_CMD         = $B9A0         ; FileIO command register
FIO_STATUS      = $B9A1         ; FileIO status (0=idle, 2=ok, 3=error)
FIO_ERRCODE     = $B9A2         ; FileIO error code

; --- Constants ---
FIO_CMD_SFLOAD  = $15           ; soundfont load command
FIO_OK          = $02           ; status: success
ROMSWAP_NCCEDIT = $03           ; NCC editor event

; --- Extension command IDs ---
EXT_CMD_NCC     = $00
EXT_CMD_SFLOAD  = $01

; --- RAM addresses ---
EXT_RESET_VEC   = $0233         ; reset recovery routine in RAM

      .segment "CODE"
      .org    $C000

; =====================================================================
; Extension entry point — called via JSR $C000 from RAM trampoline
; =====================================================================
ExtEntry:
      LDA   ExtCmdId          ; read command ID
      ASL                     ; * 2 for word-sized table
      TAX
      LDA   ExtTable+1,X      ; high byte of handler-1
      PHA
      LDA   ExtTable,X        ; low byte of handler-1
      PHA
      RTS                     ; dispatch via RTS trick

; --- Dispatch table (entries are handler_address - 1) ---
ExtTable:
      .word EXT_NCC-1         ; cmd 0: NCC confirmation dialog
      .word EXT_SFLOAD-1      ; cmd 1: SFLOAD soundfont

; =====================================================================
; SFLOAD handler — issue FIO_CMD_SFLOAD and return status
; Filename already in FIO_NAME/FIO_NAMELEN (set by BASIC ROM stub).
; Returns: A=0 success, A=errcode on error.
; =====================================================================
EXT_SFLOAD:
      LDA   #FIO_CMD_SFLOAD
      STA   FIO_CMD           ; trigger soundfont load
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @ok
      LDA   FIO_ERRCODE       ; nonzero error code
      RTS
@ok:
      LDA   #$00
      RTS

; =====================================================================
; NCC handler — confirmation dialog then trigger NCC editor
; =====================================================================
EXT_NCC:
      LDY   #$00
@msg1:
      LDA   @ncc_msg1,Y
      BEQ   @msg1_done
      STA   VGC_CHAROUT
      INY
      BNE   @msg1
@msg1_done:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT

      LDY   #$00
@msg2:
      LDA   @ncc_msg2,Y
      BEQ   @msg2_done
      STA   VGC_CHAROUT
      INY
      BNE   @msg2
@msg2_done:

@wait_key:
      LDA   VGC_CHARIN
      BEQ   @wait_key
      CMP   #'Y'
      BEQ   @do_ncc
      CMP   #'y'
      BEQ   @do_ncc

      ; Not Y — echo char + CRLF, return
      STA   VGC_CHAROUT
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      LDA   #$00
      RTS

@do_ncc:
      STA   VGC_CHAROUT       ; echo Y
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      LDA   #ROMSWAP_NCCEDIT
      STA   REG_ROMSWAP
      LDA   #$00
      RTS

@ncc_msg1: .byte "THIS WILL CLEAR THE BASIC PROGRAM",0
@ncc_msg2: .byte "ARE YOU SURE? (Y/N) ",0

; =====================================================================
; Reset handler — if RESET fires while extension ROM is active,
; jump to RAM recovery code that swaps back to BASIC ROM.
; =====================================================================
ExtReset:
      CLD
      LDX   #$FF
      TXS
      JMP   EXT_RESET_VEC     ; RAM routine swaps to BASIC + JMP $FFBD

; =====================================================================
; CPU vectors
; =====================================================================
      .segment "VECTORS"
      .org    $FFFA

      .word $0217             ; NMI  -> RAM NMI handler
      .word ExtReset          ; RESET -> extension reset handler
      .word $020D             ; IRQ  -> RAM IRQ handler
