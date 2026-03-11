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

; --- FileIO registers ---
FIO_NAMELEN     = $B9A3         ; filename length
FIO_SIZEL       = $B9A8         ; dir entry size low
FIO_SIZEH       = $B9A9         ; dir entry size high
FIO_DIRTYPE     = $B9AF         ; dir entry type (0=BAS,1=SID,2=BIN,3=MID,4=GFX,5=DIR)
FIO_NAME        = $B9B0         ; filename buffer (64 bytes)

; --- XMC registers ---
XMC_CMD         = $BA00         ; command register
XMC_STATUS      = $BA01         ; status
XMC_BANK        = $BA0C         ; current bank
XMC_BANKS       = $BA0D         ; total banks
XMC_USEDL       = $BA0E         ; used pages low
XMC_USEDH       = $BA0F         ; used pages high
XMC_FREEL       = $BA10         ; free pages low
XMC_FREEH       = $BA11         ; free pages high
XMC_NAMELEN     = $BA12         ; name length
XMC_DIRCOUNTL   = $BA14         ; named block count low
XMC_DIRCOUNTH   = $BA15         ; named block count high
XMC_LENL        = $BA09         ; block length low
XMC_LENH        = $BA0A         ; block length high
XMC_NAME        = $BA24         ; name buffer
XMC_CMD_STATS   = $07           ; refresh stats
XMC_CMD_NDIRO   = $0E           ; named dir open
XMC_CMD_NDIRR   = $0F           ; named dir read
XMC_OK          = $02           ; success status

; --- Constants ---
FIO_CMD_SFLOAD  = $15           ; soundfont load command
FIO_CMD_DIROPEN = $03           ; open directory listing
FIO_CMD_DIRREAD = $04           ; read next directory entry
FIO_CMD_PWD     = $26           ; print working directory
FIO_OK          = $02           ; status: success
ROMSWAP_NCCEDIT = $03           ; NCC editor event

; --- Extension command IDs ---
EXT_CMD_NCC     = $00
EXT_CMD_SFLOAD  = $01
EXT_CMD_DIR     = $02
EXT_CMD_PWD     = $03
EXT_CMD_XMEM    = $04
EXT_CMD_XDIR    = $05

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
      .word EXT_DIR-1         ; cmd 2: DIR listing loop
      .word EXT_PWD-1         ; cmd 3: PWD print working directory
      .word EXT_XMEM-1        ; cmd 4: XMEM status display
      .word EXT_XDIR-1        ; cmd 5: XDIR listing loop

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
; DIR handler — list directory entries
; FIO_NAMELEN already set (0 = no filter, >0 = pattern in FIO_NAME).
; =====================================================================
EXT_DIR:
      LDA   #FIO_CMD_DIROPEN
      STA   FIO_CMD           ; trigger DirOpen
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @dir_done         ; no files or error
      JSR   ext_crlf          ; blank line
@dir_loop:
      ; --- right-justified file size (5 columns) ---
      LDA   FIO_SIZEH
      LDX   FIO_SIZEL
      JSR   ext_print_u16     ; print AX as decimal, right-justified in 5 cols
      ; --- file type ---
      LDA   FIO_DIRTYPE
      ASL
      TAX
      LDA   @tab_dtype,X
      STA   $FE               ; ZP temp low
      LDA   @tab_dtype+1,X
      STA   $FF               ; ZP temp high
      LDY   #$00
@dir_tloop:
      LDA   ($FE),Y
      BEQ   @dir_tname
      STA   VGC_CHAROUT
      INY
      BNE   @dir_tloop
@dir_tname:
      ; --- filename ---
      LDY   #$00
@dir_pname:
      CPY   FIO_NAMELEN
      BCS   @dir_nl
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @dir_pname
@dir_nl:
      JSR   ext_crlf
      ; advance to next entry
      LDA   #FIO_CMD_DIRREAD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @dir_loop
@dir_done:
      LDA   #$00
      RTS

@tab_dtype:
      .word @s_bas, @s_sid, @s_bin, @s_mid, @s_gfx, @s_dir
@s_bas: .byte "  BAS  ",0
@s_sid: .byte "  SID  ",0
@s_bin: .byte "  BIN  ",0
@s_mid: .byte "  MID  ",0
@s_gfx: .byte "  GFX  ",0
@s_dir: .byte "  DIR  ",0

; =====================================================================
; ext_print_u16 — print 16-bit unsigned in A(high) X(low),
;                 right-justified in a 5-character field.
; Uses ZP $F0-$F6 as scratch.
; =====================================================================
ext_print_u16:
      STA   $F0               ; value high
      STX   $F1               ; value low
      ; Extract 5 decimal digits into $F2(10000s)..$F6(1s)
      LDX   #$00
@d10k:
      SEC
      LDA   $F1
      SBC   #<10000
      PHA
      LDA   $F0
      SBC   #>10000
      BCC   @d10kd
      STA   $F0
      PLA
      STA   $F1
      INX
      BCS   @d10k             ; always branches (just subtracted successfully)
@d10kd:
      PLA
      STX   $F2               ; 10000s digit
      LDX   #$00
@d1k:
      SEC
      LDA   $F1
      SBC   #<1000
      PHA
      LDA   $F0
      SBC   #>1000
      BCC   @d1kd
      STA   $F0
      PLA
      STA   $F1
      INX
      BCS   @d1k
@d1kd:
      PLA
      STX   $F3               ; 1000s digit
      LDX   #$00
@d100:
      SEC
      LDA   $F1
      SBC   #100
      BCC   @d100d
      STA   $F1
      INX
      BNE   @d100             ; always branches (100s digit max 9)
@d100d:
      STX   $F4               ; 100s digit
      LDX   #$00
@d10:
      LDA   $F1
      CMP   #10
      BCC   @d10d
      SBC   #10
      STA   $F1
      INX
      BNE   @d10              ; always branches
@d10d:
      STX   $F5               ; 10s digit
      LDA   $F1
      STA   $F6               ; 1s digit
      ; Find first non-zero digit (index in X), always print at least ones
      LDX   #$00
@findnz:
      CPX   #$04
      BCS   @print            ; always print last digit
      LDA   $F2,X
      BNE   @print
      INX
      BNE   @findnz           ; always branches
@print:
      ; Print X leading spaces
      STX   $F0               ; first significant digit index
      LDY   #$00
      CPX   #$00
      BEQ   @digits
@spc:
      LDA   #' '
      STA   VGC_CHAROUT
      INY
      CPY   $F0
      BCC   @spc
@digits:
      LDX   $F0
@dloop:
      LDA   $F2,X
      ORA   #'0'
      STA   VGC_CHAROUT
      INX
      CPX   #$05
      BCC   @dloop
      RTS

; =====================================================================
; PWD handler — print working directory
; =====================================================================
EXT_PWD:
      LDA   #FIO_CMD_PWD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @pwd_done
      LDY   #$00
@pwd_lp:
      CPY   FIO_NAMELEN
      BCS   @pwd_nl
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @pwd_lp
@pwd_nl:
      JSR   ext_crlf
@pwd_done:
      LDA   #$00
      RTS

; =====================================================================
; XMEM handler — print expansion memory status
; =====================================================================
EXT_XMEM:
      LDA   #XMC_CMD_STATS
      STA   XMC_CMD
      ; print "<banks>"
      LDA   #$00
      LDX   XMC_BANKS
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " BANKS, ",0
      ; print "<banks*64>" as KB
      LDA   XMC_BANKS
      STA   $F0
      LDA   #$00
      STA   $F1
      LDY   #$06
@xm_kb:
      ASL   $F0
      ROL   $F1
      DEY
      BNE   @xm_kb
      LDA   $F1
      LDX   $F0
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " KB XRAM, BANK ",0
      ; print selected bank
      LDA   #$00
      LDX   XMC_BANK
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte ", USED ",0
      LDA   XMC_USEDH
      LDX   XMC_USEDL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte ", FREE ",0
      LDA   XMC_FREEH
      LDX   XMC_FREEL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " PAGES, NAMED ",0
      LDA   XMC_DIRCOUNTH
      LDX   XMC_DIRCOUNTL
      JSR   ext_print_u16
      JSR   ext_crlf
      LDA   #$00
      RTS

; =====================================================================
; XDIR handler — list named XRAM blocks
; =====================================================================
EXT_XDIR:
      LDA   #XMC_CMD_NDIRO
      STA   XMC_CMD
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BNE   @xdir_done
@xdir_loop:
      LDA   XMC_LENH
      LDX   XMC_LENL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " BYTES  ",0
      LDY   #$00
@xdir_name:
      CPY   XMC_NAMELEN
      BCS   @xdir_nl
      LDA   XMC_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @xdir_name
@xdir_nl:
      JSR   ext_crlf
      LDA   #XMC_CMD_NDIRR
      STA   XMC_CMD
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BEQ   @xdir_loop
@xdir_done:
      LDA   #$00
      RTS

; =====================================================================
; ext_print_str — print inline null-terminated string
; String data follows the JSR instruction. Returns to byte after null.
; =====================================================================
ext_print_str:
      PLA                     ; low byte of return addr (points to string-1)
      STA   $FC
      PLA                     ; high byte
      STA   $FD
      LDY   #$01              ; skip past low byte of return addr
@ps_loop:
      LDA   ($FC),Y
      BEQ   @ps_done
      STA   VGC_CHAROUT
      INY
      BNE   @ps_loop
@ps_done:
      ; fix return address: $FC/$FD + Y = address of null byte
      ; RTS needs addr-1 on stack, and Y points to null, so push $FC+Y-1...
      ; Actually: $FC = retaddr_low, $FD = retaddr_high (from PLA)
      ; The string starts at retaddr+1. Null is at retaddr+Y.
      ; We need to return to retaddr+Y+1, so push retaddr+Y (RTS adds 1)
      TYA
      CLC
      ADC   $FC
      TAX                     ; low byte
      LDA   #$00
      ADC   $FD               ; high byte with carry
      PHA                     ; push high
      TXA
      PHA                     ; push low
      RTS                     ; return past the string

; =====================================================================
; ext_crlf — print CR LF
; =====================================================================
ext_crlf:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

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
