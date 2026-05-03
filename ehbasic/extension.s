; Extension ROM for NovaVM (bank 4)
; Contains overflow command handlers called from BASIC ROM via RAM trampoline.
;
; Entry point at $C000: reads ExtCmdId from zero page and dispatches.
; Handlers access hardware registers directly — never call BASIC ROM routines.

; --- Zero page communication ---
ExtCmdId        = $E4           ; command ID (set by RAM trampoline)

      .include "lib/nova.inc"

; Short-lived extension helper scratch. Keep NVR4-NVR6 for XMC metadata state.
ext_ptrL        = NVR0L
ext_ptrH        = NVR0H
ext_u16h        = NVR0L
ext_u16l        = NVR0H
ext_dig0        = NVR1L
ext_dig1        = NVR1H
ext_dig2        = NVR2L
ext_dig3        = NVR2H
ext_dig4        = NVR3L
ext_firstdig    = NVR3H

addr_entryL     = ext_ptrL
addr_entryH     = ext_ptrH
addr_nameL      = ext_dig0
addr_nameH      = ext_dig1
addr_len        = ext_dig2
addr_ch         = ext_dig3
addr_hash       = ext_dig4

; --- RAM addresses ---
EXT_RESET_VEC   = $0233         ; reset recovery routine in RAM
EXT_GTBY_VEC    = $023B         ; bridge: extension → BASIC LAB_GTBY → extension
EXT_GTWRD_VEC   = $0249         ; bridge: extension → BASIC LAB_GTWRD → extension
EXT_SNERR_VEC   = $0257         ; bridge: extension → BASIC LAB_15D9 syntax err

; --- BASIC ZP routines (RAM-resident, callable from extension ROM) ---
LAB_IGBY        = $BC           ; advance + skip-spaces + return byte (A)
LAB_GBYT        = $C2           ; peek current + skip-spaces + return byte (A)

; --- BASIC ZP scratch / FAC1 ---
Bpntrl          = $C3           ; BASIC execute pointer low (alias of Bpntrl_ext)
Bpntrh          = $C4
FAC1_2          = $AE           ; FAC1 mantissa2 (16-bit hi after LAB_GTWRD)
FAC1_3          = $AF           ; FAC1 mantissa3 (16-bit lo after LAB_GTWRD)

; --- BASIC token bytes (sequential from $80, see basic.asm:321) ---
TK_END_VAL      = $80
TK_ON_VAL       = $93
TK_LIST_VAL     = $A1
TK_CLEAR_VAL    = $A2
TK_GMODE_VAL    = $B5
TK_OFF_VAL      = $CB

; VGC register and command constants live in lib/nova.inc.

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
      .word EXT_TSAVE-1       ; cmd 6: TSAVE tile file
      .word EXT_TLOAD-1       ; cmd 7: TLOAD tile file
      .word EXT_HELP-1        ; cmd 8: HELP command
      .word EXT_DMAFILL-1     ; cmd 9: DMAFILL
      .word EXT_BLTFILL-1     ; cmd A: BLITFILL
      .word EXT_XMCCMD-1      ; cmd B: XMC command processor
      .word EXT_COPPER-1      ; cmd C: COPPER subcommand parser
      .word EXT_XLOAD-1       ; cmd D: XLOAD file -> XRAM
      .word EXT_XSAVE-1       ; cmd E: XSAVE XRAM -> file
      .word EXT_DMACOPY-1     ; cmd F: DMACOPY
      .word EXT_BLITCOPY-1    ; cmd 10: BLITCOPY
      .word EXT_TILESIZE-1    ; cmd 11: TILESIZE
      .word EXT_TMIRROR-1     ; cmd 12: TMIRROR
      .word EXT_TTRANS-1      ; cmd 13: TTRANS
      .word EXT_TDEF-1        ; cmd 14: TDEF
      .word EXT_TPUT-1        ; cmd 15: TPUT
      .word EXT_TATTR-1       ; cmd 16: TATTR
      .word EXT_TFILL-1       ; cmd 17: TFILL
      .word EXT_TROW-1        ; cmd 18: TROW
      .word EXT_TCOL-1        ; cmd 19: TCOL
      .word EXT_TNTLOAD-1     ; cmd 1A: TNTLOAD
      .word EXT_TCLS-1        ; cmd 1B: TCLS
      .word EXT_TBUF-1        ; cmd 1C: TBUF
      .word EXT_TBSET-1       ; cmd 1D: TBSET
      .word EXT_TBPUT-1       ; cmd 1E: TBPUT
      .word EXT_TBFILL-1      ; cmd 1F: TBFILL
      .word EXT_TSCROLL-1     ; cmd 20: TSCROLL
      .word EXT_TPAL-1        ; cmd 21: TPAL
      .word EXT_TPALC-1       ; cmd 22: TPALC
      .word EXT_ADDR-1        ; cmd 23: ADDR("label") lookup
      .word EXT_XBANK-1       ; cmd 24: XBANK
      .word EXT_XPOKE-1       ; cmd 25: XPOKE
      .word EXT_XFREE-1       ; cmd 26: XFREE
      .word EXT_XRESET-1      ; cmd 27: XRESET
      .word EXT_XALLOC-1      ; cmd 28: XALLOC
      .word EXT_XMAP-1        ; cmd 29: XMAP
      .word EXT_XUNMAP-1      ; cmd 2A: XUNMAP

; =====================================================================
; SFLOAD handler — issue FIO_CMD_SFLOAD and return status
; Filename already in FIO_NAME/FIO_NAMELEN (set by BASIC ROM stub).
; Returns: A=0 success, A=errcode on error.
; =====================================================================
EXT_SFLOAD:
      JSR   audio_sfload      ; trigger soundfont load
      BEQ   @ok
      LDA   FIO_ERRCODE       ; nonzero error code
      RTS
@ok:
      LDA   #$00
      RTS

; =====================================================================
; ADDR("label") lookup — label string pointer/length are supplied by BASIC
; in EXT_ARG_PTRL/H and EXT_ARG_LEN. Returns X=0, A=high, Y=low on success;
; X=1 on miss so BASIC can raise Function Call error.
; =====================================================================
EXT_ADDR:
      LDA   EXT_ARG_LEN
      BEQ   @not_found
      JSR   ext_label_hash
      AND   #(RUNTIME_LABEL_BUCKET_COUNT-1)
      ASL
      TAX
      LDA   RuntimeLabelBuckets,X
      STA   addr_entryL
      LDA   RuntimeLabelBuckets+1,X
      STA   addr_entryH

@entry_loop:
      LDY   #$00
      LDA   (addr_entryL),Y
      BEQ   @not_found
      STA   addr_len
      CMP   EXT_ARG_LEN
      BNE   @next_entry

      CLC
      LDA   addr_entryL
      ADC   #$03              ; skip len + 16-bit value
      STA   addr_nameL
      LDA   addr_entryH
      ADC   #$00
      STA   addr_nameH

      LDX   #$00
@cmp_loop:
      CPX   addr_len
      BEQ   @found
      TXA
      TAY
      LDA   (EXT_ARG_PTRL),Y
      JSR   ext_upper
      STA   addr_ch
      LDA   (addr_nameL),Y
      CMP   addr_ch
      BNE   @next_entry
      INX
      BRA   @cmp_loop

@found:
      LDY   #$01
      LDA   (addr_entryL),Y   ; value low
      STA   addr_ch
      INY
      LDA   (addr_entryL),Y   ; value high
      LDY   addr_ch
      LDX   #$00
      RTS

@next_entry:
      CLC
      LDA   addr_entryL
      ADC   addr_len
      STA   addr_entryL
      BCC   @add_entry_header
      INC   addr_entryH
@add_entry_header:
      CLC
      LDA   addr_entryL
      ADC   #$03
      STA   addr_entryL
      BCC   @entry_loop
      INC   addr_entryH
      BRA   @entry_loop

@not_found:
      LDX   #$01
      LDY   #$00
      TYA
      RTS

ext_label_hash:
      STZ   addr_hash
      LDY   #$00
@hash_loop:
      CPY   EXT_ARG_LEN
      BEQ   @hash_done
      LDA   (EXT_ARG_PTRL),Y
      JSR   ext_upper
      STA   addr_ch
      LDA   addr_hash
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   addr_hash
      CLC
      ADC   addr_ch
      STA   addr_hash
      INY
      BRA   @hash_loop
@hash_done:
      LDA   addr_hash
      RTS

ext_upper:
      CMP   #'a'
      BCC   @upper_done
      CMP   #'z'+1
      BCS   @upper_done
      AND   #$DF
@upper_done:
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
      JSR   fio_dir_open      ; trigger DirOpen
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
      STA   ext_ptrL
      LDA   @tab_dtype+1,X
      STA   ext_ptrH
      LDY   #$00
@dir_tloop:
      LDA   (ext_ptrL),Y
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
      JSR   fio_dir_read
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
; =====================================================================
ext_print_u16:
      STA   ext_u16h
      STX   ext_u16l
      ; Extract 5 decimal digits into ext_dig0(10000s)..ext_dig4(1s)
      LDX   #$00
@d10k:
      SEC
      LDA   ext_u16l
      SBC   #<10000
      PHA
      LDA   ext_u16h
      SBC   #>10000
      BCC   @d10kd
      STA   ext_u16h
      PLA
      STA   ext_u16l
      INX
      BCS   @d10k             ; always branches (just subtracted successfully)
@d10kd:
      PLA
      STX   ext_dig0          ; 10000s digit
      LDX   #$00
@d1k:
      SEC
      LDA   ext_u16l
      SBC   #<1000
      PHA
      LDA   ext_u16h
      SBC   #>1000
      BCC   @d1kd
      STA   ext_u16h
      PLA
      STA   ext_u16l
      INX
      BCS   @d1k
@d1kd:
      PLA
      STX   ext_dig1          ; 1000s digit
      LDX   #$00
@d100:
      SEC
      LDA   ext_u16l
      SBC   #100
      BCC   @d100d
      STA   ext_u16l
      INX
      BNE   @d100             ; always branches (100s digit max 9)
@d100d:
      STX   ext_dig2          ; 100s digit
      LDX   #$00
@d10:
      LDA   ext_u16l
      CMP   #10
      BCC   @d10d
      SBC   #10
      STA   ext_u16l
      INX
      BNE   @d10              ; always branches
@d10d:
      STX   ext_dig3          ; 10s digit
      LDA   ext_u16l
      STA   ext_dig4          ; 1s digit
      ; Find first non-zero digit (index in X), always print at least ones
      LDX   #$00
@findnz:
      CPX   #$04
      BCS   @print            ; always print last digit
      LDA   ext_dig0,X
      BNE   @print
      INX
      BNE   @findnz           ; always branches
@print:
      ; Print X leading spaces
      STX   ext_firstdig      ; first significant digit index
      LDY   #$00
      CPX   #$00
      BEQ   @digits
@spc:
      LDA   #' '
      STA   VGC_CHAROUT
      INY
      CPY   ext_firstdig
      BCC   @spc
@digits:
      LDX   ext_firstdig
@dloop:
      LDA   ext_dig0,X
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
      JSR   fio_pwd
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
      JSR   xmc_process         ; process locally (FPGA has no hardware XMC)
      ; print "<banks>"
      LDA   #$00
      LDX   XMC_BANKS
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " BANKS, ",0
      ; print "<banks*64>" as KB
      LDA   XMC_BANKS
      STA   ext_u16l
      LDA   #$00
      STA   ext_u16h
      LDY   #$06
@xm_kb:
      ASL   ext_u16l
      ROL   ext_u16h
      DEY
      BNE   @xm_kb
      LDA   ext_u16h
      LDX   ext_u16l
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
      JSR   xmc_dir_open
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
      JSR   xmc_dir_read
      BEQ   @xdir_loop
@xdir_done:
      LDA   #$00
      RTS

; =====================================================================
; TSAVE handler — save tile state to .tile file
; Filename already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=errcode on error (Z flag set accordingly).
; =====================================================================
EXT_TSAVE:
      LDA   #FIO_CMD_TSAVE
      JSR   fio_exec
      BEQ   @tsave_ok
      LDA   FIO_ERRCODE
      RTS
@tsave_ok:
      LDA   #$00
      RTS

; =====================================================================
; TLOAD handler — load tile state from .tile file
; Filename already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=errcode on error (Z flag set accordingly).
; =====================================================================
EXT_TLOAD:
      LDA   #FIO_CMD_TLOAD
      JSR   fio_exec
      BEQ   @tload_ok
      LDA   FIO_ERRCODE
      RTS
@tload_ok:
      LDA   #$00
      RTS

; =====================================================================
; HELP handler — moved from BASIC ROM to save space.
; Reads BASIC text at (Bpntrl),Y and copies raw bytes to help buffer.
; The C# help system handles detokenization.
; =====================================================================
Bpntrl_ext      = $C3           ; BASIC execute pointer low
Bpntrh_ext      = $C4           ; BASIC execute pointer high
help_len_ext    = $E2           ; help keyword length scratch
TKX_PREFIX_EXT  = $01           ; extended token prefix

EXT_HELP:
      LDY   #$00
      LDA   (Bpntrl_ext),Y
      BEQ   @help_noarg
      CMP   #':'
      BEQ   @help_noarg
      CMP   #'"'
      BEQ   @help_quoted
      LDX   #$00
@help_raw:
      LDA   (Bpntrl_ext),Y
      BEQ   @help_rawdone
      CMP   #':'
      BEQ   @help_rawdone
      STA   HELP_BUF,X
      INY
      INX
      CPX   #$10
      BCC   @help_raw
@help_rawdone:
      TYA
      CLC
      ADC   Bpntrl_ext
      STA   Bpntrl_ext
      BCC   @help_noinc
      INC   Bpntrh_ext
@help_noinc:
      STX   help_len_ext
      JMP   @help_trigger
@help_quoted:
      INY
      LDX   #$00
@help_qloop:
      LDA   (Bpntrl_ext),Y
      BEQ   @help_qdone
      CMP   #'"'
      BEQ   @help_qclose
      STA   HELP_BUF,X
      INY
      INX
      CPX   #$10
      BCC   @help_qloop
@help_qclose:
      INY
@help_qdone:
      TYA
      CLC
      ADC   Bpntrl_ext
      STA   Bpntrl_ext
      BCC   @help_noinc2
      INC   Bpntrh_ext
@help_noinc2:
      STX   help_len_ext
@help_trigger:
      LDY   help_len_ext
@help_zero:
      CPY   #$10
      BCS   @help_search
      LDA   #$00
      STA   HELP_BUF,Y
      INY
      BNE   @help_zero
@help_search:
      LDA   #$02
      STA   REG_HELP
      LDA   #$00
      RTS
@help_noarg:
      LDA   #$01
      STA   REG_HELP
      LDA   #$00
      RTS

; =====================================================================
; DMA / blitter BASIC parsers.
; These live in extension ROM because they are shared operation setup, not
; BASIC core syntax. Returns A=0 success, A=1 error.
; =====================================================================

; ext_cword — parse comma + 16-bit word, store L/H to address in X:A.
ext_cword:
      STX   ext_ptrL
      STA   ext_ptrH
      JSR   ext_comma
      JSR   EXT_GTWRD_VEC
      LDY   #$00
      LDA   FAC1_3
      STA   (ext_ptrL),Y
      INY
      LDA   FAC1_2
      STA   (ext_ptrL),Y
      RTS

ext_dma_src_bank:
      STZ   DMA_SRCH
      LDA   DMA_SRCSPACE
      CMP   #DMA_SPACE_XRAM
      BNE   @done
      LDA   XMC_BANK
      STA   DMA_SRCH
@done:
      RTS

ext_dma_dst_bank:
      STZ   DMA_DSTH
      LDA   DMA_DSTSPACE
      CMP   #DMA_SPACE_XRAM
      BNE   @done
      LDA   XMC_BANK
      STA   DMA_DSTH
@done:
      RTS

ext_blt_src_bank:
      STZ   BLT_SRCH
      LDA   BLT_SRCSPACE
      CMP   #BLT_SPACE_XRAM
      BNE   @done
      LDA   XMC_BANK
      STA   BLT_SRCH
@done:
      RTS

ext_blt_dst_bank:
      STZ   BLT_DSTH
      LDA   BLT_DSTSPACE
      CMP   #BLT_SPACE_XRAM
      BNE   @done
      LDA   XMC_BANK
      STA   BLT_DSTH
@done:
      RTS

EXT_DMACOPY:
      JSR   EXT_GTBY_VEC
      STX   DMA_SRCSPACE
      LDX   #<DMA_SRCL
      LDA   #>DMA_SRCL
      JSR   ext_cword
      JSR   ext_dma_src_bank
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   DMA_DSTSPACE
      LDX   #<DMA_DSTL
      LDA   #>DMA_DSTL
      JSR   ext_cword
      JSR   ext_dma_dst_bank
      LDX   #<DMA_LENL
      LDA   #>DMA_LENL
      JSR   ext_cword
      STZ   DMA_LENH
      JMP   dma_copy

EXT_DMAFILL:
      JSR   EXT_GTBY_VEC
      STX   DMA_DSTSPACE
      LDX   #<DMA_DSTL
      LDA   #>DMA_DSTL
      JSR   ext_cword
      JSR   ext_dma_dst_bank
      LDX   #<DMA_LENL
      LDA   #>DMA_LENL
      JSR   ext_cword
      STZ   DMA_LENH
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   DMA_FILLVALUE
      JMP   dma_fill

EXT_BLITCOPY:
      JSR   EXT_GTBY_VEC
      STX   BLT_SRCSPACE
      LDX   #<BLT_SRCL
      LDA   #>BLT_SRCL
      JSR   ext_cword
      JSR   ext_blt_src_bank
      LDX   #<BLT_SRCSTRL
      LDA   #>BLT_SRCSTRL
      JSR   ext_cword
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   BLT_DSTSPACE
      LDX   #<BLT_DSTL
      LDA   #>BLT_DSTL
      JSR   ext_cword
      JSR   ext_blt_dst_bank
      LDX   #<BLT_DSTSTRL
      LDA   #>BLT_DSTSTRL
      JSR   ext_cword
      LDX   #<BLT_WIDTHL
      LDA   #>BLT_WIDTHL
      JSR   ext_cword
      LDX   #<BLT_HEIGHTL
      LDA   #>BLT_HEIGHTL
      JSR   ext_cword
      STZ   BLT_FILLVALUE
      JMP   blitter_copy

EXT_BLTFILL:
      JSR   EXT_GTBY_VEC
      STX   BLT_DSTSPACE
      LDX   #<BLT_DSTL
      LDA   #>BLT_DSTL
      JSR   ext_cword
      JSR   ext_blt_dst_bank
      LDX   #<BLT_DSTSTRL
      LDA   #>BLT_DSTSTRL
      JSR   ext_cword
      LDX   #<BLT_WIDTHL
      LDA   #>BLT_WIDTHL
      JSR   ext_cword
      LDX   #<BLT_HEIGHTL
      LDA   #>BLT_HEIGHTL
      JSR   ext_cword
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   BLT_FILLVALUE
      JMP   blitter_fill

      .include "lib/blitter.s"
      .include "lib/copper.s"
      .include "lib/tile.s"
      .include "lib/audio.s"
      .include "lib/xram.s"
      .include "lib/xmc.s"

; =====================================================================
; XLOAD/XSAVE handlers — bridge BASIC's FIO-style arguments into the
; shared XRAM pseudo-register ABI, then call the assembly-facing runtime.
; Filename is already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=FIO_ERRCODE on error.
; =====================================================================
EXT_XLOAD:
      JSR   ext_xfio_load_args
      JSR   xram_xload
      BEQ   @ok
      LDA   FIO_ERRCODE
      BNE   @ret
      LDA   #FIO_ERR_IO
@ret: RTS
@ok:  LDA   #$00
      RTS

EXT_XSAVE:
      JSR   ext_xfio_load_args
      JSR   xram_xsave
      BEQ   @ok
      LDA   FIO_ERRCODE
      BNE   @ret
      LDA   #FIO_ERR_IO
@ret: RTS
@ok:  LDA   #$00
      RTS

ext_xfio_load_args:
      LDA   FIO_GADDRL
      STA   XRAM_ADDRL
      LDA   FIO_GADDRH
      STA   XRAM_ADDRM
      LDA   FIO_GSPACE
      STA   XRAM_ADDRH
      LDA   FIO_GLENL
      STA   XRAM_LENL
      LDA   FIO_GLENH
      STA   XRAM_LENH
      LDA   FIO_NAMELEN
      STA   XRAM_NAMELEN
      LDA   #<FIO_NAME
      STA   XRAM_NAMEPTR_L
      LDA   #>FIO_NAME
      STA   XRAM_NAMEPTR_H
      RTS

; =====================================================================
; ext_print_str — print inline null-terminated string
; String data follows the JSR instruction. Returns to byte after null.
; =====================================================================
ext_print_str:
      PLA                     ; low byte of return addr (points to string-1)
      STA   ext_ptrL
      PLA                     ; high byte
      STA   ext_ptrH
      LDY   #$01              ; skip past low byte of return addr
@ps_loop:
      LDA   (ext_ptrL),Y
      BEQ   @ps_done
      STA   VGC_CHAROUT
      INY
      BNE   @ps_loop
@ps_done:
      ; fix return address: ext_ptr + Y = address of null byte
      ; RTS needs addr-1 on stack, and Y points to null, so push ext_ptr+Y.
      ; Actually: ext_ptr = retaddr (from PLA)
      ; The string starts at retaddr+1. Null is at retaddr+Y.
      ; We need to return to retaddr+Y+1, so push retaddr+Y (RTS adds 1)
      TYA
      CLC
      ADC   ext_ptrL
      TAX                     ; low byte
      LDA   #$00
      ADC   ext_ptrH          ; high byte with carry
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
; EXT_XMCCMD — entry point for XMC command processing from BASIC ROM
; Called via EXT_vec. Returns A=0 ok, A=1 error (for BNE in CHKOK).
; =====================================================================
EXT_XMCCMD:
      JMP   xmc_command_status

ext_xmc_setoff:
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   XMC_XAL
      LDA   FAC1_2
      STA   XMC_XAM
      LDA   XMC_BANK
      STA   XMC_XAH
      RTS

ext_xmc_getlen:
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   XMC_LENL
      LDA   FAC1_2
      STA   XMC_LENH
      RTS

EXT_XBANK:
      JSR   EXT_GTBY_VEC
      JMP   xmc_select_bank

EXT_XPOKE:
      JSR   ext_xmc_setoff
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   XMC_DATA
      JMP   xmc_put_byte

EXT_XFREE:
      JSR   ext_xmc_setoff
      JSR   ext_comma
      JSR   ext_xmc_getlen
      JMP   xmc_release

EXT_XRESET:
      JMP   xmc_reset_usage

EXT_XALLOC:
      JSR   ext_xmc_getlen
      JMP   xmc_alloc_block

EXT_XMAP:
      JSR   EXT_GTBY_VEC
      STX   ext_firstdig
      JSR   ext_comma
      JSR   ext_xmc_setoff
      LDX   ext_firstdig
      JMP   xmc_map_window

EXT_XUNMAP:
      JSR   EXT_GTBY_VEC
      JMP   xmc_unmap_window

; =====================================================================
; Tile engine command handlers.
; BASIC stubs dispatch here; these parsers use BASIC's RAM-resident scanner
; bridges, then issue commands through lib/tile.s.
; =====================================================================

ext_vsync:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

ext_tile_b0c:
      JSR   EXT_GTBY_VEC      ; byte -> TileP0, then consume comma
      STX   TileP0
      JMP   ext_comma

ext_tile_wad:
      JSR   EXT_GTWRD_VEC     ; word -> TileAddrL/H
      LDA   FAC1_3
      STA   TileAddrL
      LDA   FAC1_2
      STA   TileAddrH
      RTS

EXT_TILESIZE:
      JSR   EXT_GTBY_VEC
      JSR   ext_vsync
      JMP   tile_set_size

EXT_TMIRROR:
      JSR   EXT_GTBY_VEC
      JSR   ext_vsync
      JMP   tile_set_mirror

EXT_TTRANS:
      JSR   EXT_GTBY_VEC
      JSR   ext_vsync
      JMP   tile_set_trans_color

EXT_TDEF:
      JSR   ext_tile_b0c
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   NVR7L
      LDA   FAC1_2
      STA   NVR7H
      JSR   LAB_GBYT
      CMP   #','
      BEQ   @td_bulk
      LDA   NVR7L
      STA   TileAddrL
      LDA   NVR7H
      STA   TileAddrH
      JSR   ext_vsync
      JMP   tile_def
@td_bulk:
      LDA   NVR7L
      STA   TileP1
      JSR   ext_comma
      JSR   ext_tile_wad
      JSR   ext_vsync
      JMP   tile_def_bulk

EXT_TPUT:
      LDA   #TileCmdPut
      .byte $2C
EXT_TATTR:
      LDA   #TileCmdAttr
      PHA
      JSR   EXT_GTBY_VEC
      STX   TileP0
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP1
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP2
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP3
      JSR   ext_vsync
      PLA
      JMP   tile_cmd

EXT_TFILL:
      JSR   ext_tile_b0c
      JSR   EXT_GTBY_VEC
      STX   TileP1
      JSR   ext_vsync
      JMP   tile_fill

EXT_TROW:
      LDA   #TileCmdRow
      .byte $2C
EXT_TCOL:
      LDA   #TileCmdCol
      PHA
      JSR   EXT_GTBY_VEC
      STX   TileP0
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP1
      JSR   ext_comma
      JSR   ext_tile_wad
      JSR   ext_vsync
      PLA
      JMP   tile_cmd

EXT_TNTLOAD:
      JSR   ext_tile_b0c
      JSR   ext_tile_wad
      JSR   ext_vsync
      JMP   tile_load

EXT_TCLS:
      JSR   ext_vsync
      JMP   tile_cls

EXT_TBUF:
      JSR   EXT_GTBY_VEC
      STX   TileP0
      JMP   tile_buf_fill

EXT_TBSET:
      LDA   #TileCmdBufSet
      .byte $2C
EXT_TBPUT:
      LDA   #TileCmdBufPut
      PHA
      JSR   EXT_GTBY_VEC
      STX   TileP0
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP1
      PLA
      JMP   tile_cmd

EXT_TBFILL:
      JSR   EXT_GTBY_VEC
      STX   TileP0
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP1
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP2
      JMP   tile_buf_range

EXT_TSCROLL:
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   NVR0L
      LDA   FAC1_2
      STA   NVR0H
      JSR   ext_comma
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   NVR1L
      LDA   FAC1_2
      STA   NVR1H
      JSR   ext_vsync
      JMP   tile_set_scroll

EXT_TPAL:
      JSR   EXT_GTBY_VEC
      STX   TilePalP0
      JSR   ext_comma
      JSR   ext_tile_wad
      JSR   ext_vsync
      JMP   tile_pal

EXT_TPALC:
      JSR   EXT_GTBY_VEC
      STX   TilePalP0
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TilePalP1
      JSR   ext_comma
      JSR   ext_tile_b0c
      JSR   EXT_GTBY_VEC
      STX   TileP1
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   TileP2
      JSR   ext_vsync
      JMP   tile_palc

; =====================================================================
; EXT_COPPER — COPPER subcommand parser.
; Relocated from BASIC ROM (LAB_COPPER) to free ~390 bytes there.
; Original handler lived in basic.asm and used LAB_GBYT / LAB_IGBY ZP
; helpers (still callable from extension ROM since they are RAM-
; resident) plus LAB_GTBY / LAB_GTWRD / LAB_1C01 / LAB_15D9 ROM helpers
; (NOT callable here — replaced with the EXT_GTBY / EXT_GTWRD /
; EXT_1C01 / EXT_SNERR_VEC bridges).
;
; Subcommand grammar:
;   COPPER CLEAR | ON | OFF
;   COPPER LIST n | LIST END
;   COPPER USE n
;   COPPER ADD x, y, reg, value
;     reg = MODE | BGCOL | SCROLLX | SCROLLY |
;           SPRX(n) | SPRXH(n) | SPRY(n) |
;           SPRSHAPE(n) | SPRFLAGS(n) | SPRPRI(n) |
;           <8-bit numeric expression>
; =====================================================================
EXT_COPPER:
      JSR   LAB_GBYT          ; peek next byte (ZP routine, skips spaces)
      CMP   #TK_CLEAR_VAL
      BEQ   @c_clear
      CMP   #TK_ON_VAL
      BEQ   @c_on
      CMP   #TK_OFF_VAL
      BEQ   @c_off
      CMP   #'A'
      BEQ   @c_add
      CMP   #TK_LIST_VAL
      BEQ   @c_list
      CMP   #'U'
      BEQ   @c_use
      JMP   EXT_SNERR_VEC     ; syntax error

; --- COPPER CLEAR ---
@c_clear:
      JSR   LAB_IGBY          ; consume CLEAR token
      JMP   copper_clear

; --- COPPER ON ---
@c_on:
      JSR   LAB_IGBY          ; consume ON token
      JMP   copper_on

; --- COPPER OFF ---
@c_off:
      JSR   LAB_IGBY          ; consume OFF token
      JMP   copper_off

; --- COPPER LIST n / COPPER LIST END ---
@c_list:
      JSR   LAB_IGBY          ; consume LIST token
      JSR   LAB_GBYT          ; peek next
      CMP   #TK_END_VAL
      BEQ   @c_list_end
      JSR   EXT_GTBY_VEC      ; bridge: parse 8-bit expr → X
      STX   VGC_P0
      JMP   copper_list
@c_list_end:
      JSR   LAB_IGBY          ; consume END token
      JMP   copper_list_end

; --- COPPER USE n ---
@c_use:
      LDX   #3
      JSR   ext_skipx         ; USE
      JSR   EXT_GTBY_VEC      ; list index → X
      STX   VGC_P0
      JMP   copper_use

; --- COPPER ADD x, y, reg, value ---
@c_add:
      LDX   #3
      JSR   ext_skipx         ; ADD
      JSR   EXT_GTWRD_VEC     ; x (16-bit) → FAC1_3 (lo) / FAC1_2 (hi)
      LDA   FAC1_3
      STA   VGC_P0
      LDA   FAC1_2
      STA   VGC_P1
      JSR   ext_comma         ; comma
      JSR   EXT_GTBY_VEC      ; y (8-bit) → X
      STX   VGC_P2
      JSR   ext_comma         ; comma
      ; parse register name
      JSR   LAB_GBYT          ; peek
      CMP   #'B'
      BEQ   @c_bgcol
      CMP   #TK_GMODE_VAL
      BEQ   @c_mode
      CMP   #'S'
      BEQ   @c_s_dispatch
      ; numeric register index fallback
      JSR   EXT_GTBY_VEC      ; 8-bit expression → X
      TXA
      JMP   @c_store_idx

@c_bgcol:
      LDX   #5
      JSR   ext_skipx         ; BGCOL
      LDA   #COPPER_REG_BGCOL
      JMP   @c_store_idx

@c_mode:
      JSR   LAB_IGBY          ; consume MODE token
      LDA   #COPPER_REG_MODE
      JMP   @c_store_idx

@c_s_dispatch:
      JSR   LAB_IGBY          ; consume S
      JSR   LAB_GBYT          ; peek next: C=scroll, P=sprite
      CMP   #'C'
      BEQ   @c_scroll
      CMP   #'P'
      BEQ   @c_spr
      JMP   EXT_SNERR_VEC

@c_scroll:
      LDX   #5
      JSR   ext_skipx         ; CROLL
      JSR   LAB_GBYT          ; peek X or Y
      CMP   #'X'
      BEQ   @c_scrollx
      CMP   #'Y'
      BEQ   @c_scrolly
      JMP   EXT_SNERR_VEC

@c_scrollx:
      JSR   LAB_IGBY          ; consume X
      LDA   #COPPER_REG_SCROLLX
      JMP   @c_store_idx

@c_scrolly:
      JSR   LAB_IGBY          ; consume Y
      LDA   #COPPER_REG_SCROLLY
      JMP   @c_store_idx

@c_store_idx:
      JSR   copper_set_reg_index
      JMP   @c_store_val

      ; --- Sprite register names: SPR + field + (n) ---
@c_spr:
      JSR   LAB_IGBY          ; consume P
      JSR   LAB_IGBY          ; consume R
      JSR   LAB_GBYT          ; peek field start: X, Y, S, F, P
      CMP   #'X'
      BEQ   @cs_x
      CMP   #'Y'
      BEQ   @cs_y
      CMP   #'S'
      BEQ   @cs_shape
      CMP   #'F'
      BEQ   @cs_flags
      CMP   #'P'
      BEQ   @cs_pri
      JMP   EXT_SNERR_VEC

@cs_x:
      JSR   LAB_IGBY          ; consume X
      JSR   LAB_GBYT          ; peek H or (
      CMP   #'H'
      BEQ   @cs_xh
      LDA   #VGC_SPR_XL_OFF
      JMP   @c_spr_idx
@cs_xh:
      JSR   LAB_IGBY          ; consume H
      LDA   #VGC_SPR_XH_OFF
      JMP   @c_spr_idx

@cs_y:
      JSR   LAB_IGBY          ; consume Y
      JSR   LAB_GBYT          ; peek H or (
      CMP   #'H'
      BEQ   @cs_yh
      LDA   #VGC_SPR_YL_OFF
      JMP   @c_spr_idx
@cs_yh:
      JSR   LAB_IGBY          ; consume H
      LDA   #VGC_SPR_YH_OFF
      JMP   @c_spr_idx

@cs_shape:
      LDX   #5
      JSR   ext_skipx         ; SHAPE
      LDA   #VGC_SPR_SHAPE_OFF
      JMP   @c_spr_idx

@cs_flags:
      LDX   #5
      JSR   ext_skipx         ; FLAGS
      LDA   #VGC_SPR_FLAGS_OFF
      JMP   @c_spr_idx

@cs_pri:
      LDX   #3
      JSR   ext_skipx         ; PRI
      LDA   #VGC_SPR_PRI_OFF
      ; fall through

      ; A = field offset, parse (n) and compute VGC_SPR_BASE + n*8 + A
@c_spr_idx:
      PHA                     ; save field offset
      JSR   LAB_IGBY          ; consume '('
      JSR   EXT_GTBY_VEC      ; sprite index → X
      CPX   #VGC_MAX_SPRITES
      BCC   @cs_idx_ok
      JMP   EXT_SNERR_VEC
@cs_idx_ok:
      STX   VGC_P3            ; temporary sprite index
      JSR   LAB_IGBY          ; consume ')'
      PLA                     ; A = field offset
      LDX   VGC_P3
      JSR   copper_set_sprite_reg

@c_store_val:
      JSR   ext_comma         ; comma
      JSR   EXT_GTBY_VEC      ; value → X
      STX   VGC_P5
      JMP   copper_add

; --- Local helpers used only by EXT_COPPER ---

; ext_skipx — skip X bytes of BASIC text (LAB_SKIPX equivalent)
ext_skipx:
      JSR   LAB_IGBY          ; advance + skip spaces
      DEX
      BNE   ext_skipx
      RTS

; ext_comma — consume comma (LAB_1C01 equivalent). Triggers syntax
; error via EXT_SNERR_VEC if next non-space char isn't ','.
ext_comma:
      JSR   LAB_GBYT          ; peek (skips spaces)
      CMP   #','
      BNE   @no_comma
      JSR   LAB_IGBY          ; consume comma
      RTS
@no_comma:
      JMP   EXT_SNERR_VEC

      .include "runtime_labels.inc"

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
