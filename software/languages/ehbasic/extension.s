; Extension ROM for NovaVM (bank 4)
; Contains overflow command handlers called from BASIC ROM via RAM trampoline.
;
; Entry point at $C000: reads ExtCmdId from zero page and dispatches.
; Handlers access hardware directly and use RAM bridges for BASIC parser helpers.

; --- Zero page communication ---
ExtCmdId        = $E4           ; command ID (set by RAM trampoline)

      .include "nova.inc"

; Short-lived extension helper scratch. Library-owned persistent state lives in
; its own linker storage, not in the NVR mailbox.
ext_ptrL        = NVR0L
ext_ptrH        = NVR0H
ext_u16h        = NVR0L
ext_u16l        = NVR0H
ext_u32_0       = NVR4L
ext_u32_1       = NVR4H
ext_u32_2       = NVR5L
ext_u32_3       = NVR5H
ext_digit       = NVR2L
ext_count       = NVR2H
ext_dig0        = NVR1L
ext_dig1        = NVR1H
ext_dig2        = NVR2L
ext_dig3        = NVR2H
ext_dig4        = NVR3L
ext_firstdig    = NVR3H
txt_addrL       = ext_u32_0
txt_addrH       = ext_u32_1
txt_tmp         = ext_u32_2

; --- RAM addresses ---
; RAM landing addresses of the trampolines copied from the LAB_vec block (see
; min_mon.asm). These MUST match the actual copy targets — re-read from
; min_mon.lst after any change to the LAB_vec block. EXT_CODE grew by 3 bytes
; (JSR ensure_ext_resident), shifting every following bridge down by 3.
EXT_RESET_VEC   = $023B         ; reset recovery routine in RAM
EXT_GTBY_VEC    = $0243         ; bridge: extension → BASIC LAB_GTBY → extension
EXT_GTWRD_VEC   = $0251         ; bridge: extension → BASIC LAB_GTWRD → extension
EXT_GTSW_VEC    = $025F         ; bridge: extension → BASIC LAB_GTSW → extension
EXT_SNERR_VEC   = $026D         ; bridge: extension → BASIC LAB_15D9 syntax err

; --- BASIC ZP routines (RAM-resident, callable from extension ROM) ---
LAB_IGBY        = $BC           ; advance + skip-spaces + return byte (A)
LAB_GBYT        = $C2           ; peek current + skip-spaces + return byte (A)

; --- BASIC ZP scratch / FAC1 ---
TPos            = $0E           ; BASIC terminal column, relative to active text window
Bpntrl          = $C3           ; BASIC execute pointer low (alias of Bpntrl_ext)
Bpntrh          = $C4
FAC1_2          = $AE           ; FAC1 mantissa2 (16-bit hi after LAB_GTWRD)
FAC1_3          = $AF           ; FAC1 mantissa3 (16-bit lo after LAB_GTWRD)

; VGC register and command constants live in runtime/asm/nova.inc.

      .segment "CODE"

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
      .word EXT_UNSUPPORTED-1 ; cmd 0: reserved
      .word EXT_SFLOAD-1      ; cmd 1: SFLOAD soundfont
      .word EXT_DIR-1         ; cmd 2: DIR listing loop
      .word EXT_PWD-1         ; cmd 3: PWD print working directory
      .word EXT_XMEM-1        ; cmd 4: XMEM status display
      .word EXT_XDIR-1        ; cmd 5: XDIR listing loop
      .word EXT_UNSUPPORTED-1 ; cmd 6: reserved
      .word EXT_UNSUPPORTED-1 ; cmd 7: reserved
      .word EXT_UNSUPPORTED-1 ; cmd 8: was HELP (removed; docs -> LaTeX books)
      .word EXT_UNSUPPORTED-1 ; cmd 9: was DMAFILL (relocated to BASIC ROM LAB_DMAFILL)
      .word EXT_UNSUPPORTED-1 ; cmd A: was BLITFILL (relocated to BASIC ROM LAB_BLITFILL)
      .word EXT_XMCCMD-1      ; cmd B: XMC command processor
      .word EXT_UNSUPPORTED-1 ; cmd C: was COPPER (relocated to BASIC ROM LAB_COPPER)
      .word EXT_XLOAD-1       ; cmd D: XLOAD file -> XRAM
      .word EXT_XSAVE-1       ; cmd E: XSAVE XRAM -> file
      .word EXT_UNSUPPORTED-1 ; cmd F: was DMACOPY (relocated to BASIC ROM LAB_DMACOPY)
      .word EXT_UNSUPPORTED-1 ; cmd 10: was BLITCOPY (relocated to BASIC ROM LAB_BLITCOPY)
      .word EXT_MMUL16L-1     ; cmd 11: MMUL16L(a,b)
      .word EXT_MMUL16H-1     ; cmd 12: MMUL16H(a,b)
      .word EXT_MDOTS16L-1    ; cmd 13: MDOTS16L(ax,ay,bx,by)
      .word EXT_MDOTS16H-1    ; cmd 14: MDOTS16H(ax,ay,bx,by)
      .word EXT_MCROSSL-1     ; cmd 15: MCROSSL(ax,ay,bx,by)
      .word EXT_MCROSSH-1     ; cmd 16: MCROSSH(ax,ay,bx,by)
      .word EXT_MMULFX-1      ; cmd 17: MMULFX(a,b)
      .word EXT_MDIST-1       ; cmd 18: MDIST(dx,dy)
      .word EXT_MSIN-1        ; cmd 19: MSIN(angle)
      .word EXT_MCOS-1        ; cmd 1A: MCOS(angle)
      .word EXT_MRND-1        ; cmd 1B: MRND
      .word EXT_MDIV-1        ; cmd 1C: MDIV(n,d)
      .word EXT_MREM-1        ; cmd 1D: MREM(n,d)
      .word EXT_MATAN2-1      ; cmd 1E: MATAN2(dy,dx)
      .word EXT_MDOTFX-1      ; cmd 1F: MDOTFX(ax,ay,bx,by)
      .word EXT_MLEN2-1       ; cmd 20: MLEN2(x,y)
      .word EXT_MSCALX-1      ; cmd 21: MSCALX(x,y,s)
      .word EXT_MSCALY-1      ; cmd 22: MSCALY(x,y,s)
      .word EXT_UNSUPPORTED-1 ; cmd 23: was ADDR (relocated to SYS_ADDR_LOOKUP)
      .word EXT_XBANK-1       ; cmd 24: XBANK
      .word EXT_XPOKE-1       ; cmd 25: XPOKE
      .word EXT_XFREE-1       ; cmd 26: XFREE
      .word EXT_XRESET-1      ; cmd 27: XRESET
      .word EXT_XALLOC-1      ; cmd 28: XALLOC
      .word EXT_XMAP-1        ; cmd 29: XMAP
      .word EXT_XUNMAP-1      ; cmd 2A: XUNMAP
      .word EXT_XPEEK-1       ; cmd 2B: XPEEK(offset)
      .word EXT_UNSUPPORTED-1 ; cmd 2C: was PLAYING (relocated to main @xtk_playing)
      .word EXT_UNSUPPORTED-1 ; cmd 2D: was MNOTE (relocated to main @xtk_mnote)
      .word EXT_UNSUPPORTED-1 ; cmd 2E: was NSTATUS (relocated to main @xtk_nstatus)
      .word EXT_UNSUPPORTED-1 ; cmd 2F: was NREADY (relocated to main @xtk_nready)
      .word EXT_UNSUPPORTED-1 ; cmd 30: was DMASTATUS (relocated to BASIC ROM @xtk_dmastatus)
      .word EXT_UNSUPPORTED-1 ; cmd 31: was DMAERR (relocated to BASIC ROM @xtk_dmaerr)
      .word EXT_UNSUPPORTED-1 ; cmd 32: was DMACOUNT (relocated to BASIC ROM @xtk_dmacount)
      .word EXT_UNSUPPORTED-1 ; cmd 33: was BLITSTATUS (relocated to BASIC ROM @xtk_blitstatus)
      .word EXT_UNSUPPORTED-1 ; cmd 34: was BLITERR (relocated to BASIC ROM @xtk_bliterr)
      .word EXT_UNSUPPORTED-1 ; cmd 35: was BLITCOUNT (relocated to BASIC ROM @xtk_blitcount)
      .word EXT_UNSUPPORTED-1 ; cmd 36: was SPRCOLL (relocated to main @xtk_sprcoll)
      .word EXT_UNSUPPORTED-1 ; cmd 37: was SPRBG (relocated to main @xtk_sprbg)
      .word EXT_VPEEK-1       ; cmd 38: VPEEK(plane,addr)  [deferred — budget]
      .word EXT_UNSUPPORTED-1 ; cmd 39: was BITSET (relocated to main LAB_BITSET)
      .word EXT_UNSUPPORTED-1 ; cmd 3A: was BITCLR (relocated to main LAB_BITCLR)
      .word EXT_UNSUPPORTED-1 ; cmd 3B: was BITTGL (relocated to main LAB_BITTGL)
      .word EXT_REVERSE-1     ; cmd 3C: REVERSE [fg,bg]    [deferred — budget]
      .word EXT_UNSUPPORTED-1 ; cmd 3D: was REVERSEOFF (relocated to main LAB_REVERSEOFF)
      .word EXT_UNSUPPORTED-1 ; cmd 3E: was FLASH (relocated to main LAB_FLASH)
      .word EXT_UNSUPPORTED-1 ; cmd 3F: was FLASHOFF (relocated to main LAB_FLASHOFF)
      .word EXT_VPOKE-1       ; cmd 40: VPOKE plane,addr,value  [deferred — budget]
      .word EXT_UNSUPPORTED-1 ; cmd 41: was FIOCLR (relocated to main LAB_FIOCLR)
      .word EXT_CLS-1         ; cmd 42: CLS active text window  [skipped — window-aware, not thin]

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
; (ADDR("label") lookup relocated to the SYSTEM module as SYS_ADDR_LOOKUP;
;  ExtTable cmd 23 is now EXT_UNSUPPORTED. The hash table moved with it.)
; =====================================================================

; =====================================================================
; DIR handler — list directory entries
; FIO_NAMELEN already set (0 = no filter, >0 = pattern in FIO_NAME).
; =====================================================================
EXT_DIR:
      JSR   fio_dir_open      ; trigger DirOpen
      BNE   @dir_done         ; no files or error
      JSR   ext_crlf          ; blank line
@dir_loop:
      JSR   fio_dir_read
      BNE   @dir_done
      ; --- right-justified file size (8 columns, 24-bit) ---
      LDA   FIO_SIZEL
      STA   ext_u32_0
      LDA   FIO_SIZEH
      STA   ext_u32_1
      LDA   FIO_SIZE2
      STA   ext_u32_2
      STZ   ext_u32_3
      JSR   ext_print_u24
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
      BRA   @dir_loop
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
; ext_print_u24 — print 24-bit unsigned in ext_u32_2..0,
;                 right-justified in an 8-character field.
; =====================================================================
ext_print_u24:
      LDA   #<(ext_pow10+8)   ; 10^7 entry, skipping 10^9 and 10^8
      STA   ext_ptrL
      LDA   #>(ext_pow10+8)
      STA   ext_ptrH
      STZ   ext_firstdig
      LDY   #8
      BRA   ext_print_u32_digits

; =====================================================================
; ext_print_u32 — print 32-bit unsigned in ext_u32_3..0,
;                 right-justified in a 10-character field.
; =====================================================================
ext_print_u32:
      LDA   #<ext_pow10
      STA   ext_ptrL
      LDA   #>ext_pow10
      STA   ext_ptrH
      STZ   ext_firstdig
      LDY   #10
ext_print_u32_digits:
@digit_loop:
      STY   ext_count
      STZ   ext_digit
@sub_loop:
      JSR   @cmp_pow
      BCC   @emit_digit       ; value < current power
      JSR   @sub_pow
      INC   ext_digit
      BRA   @sub_loop
@emit_digit:
      LDA   ext_digit
      BNE   @emit_number
      LDA   ext_firstdig
      BNE   @emit_number
      LDY   ext_count
      CPY   #1                ; always print the ones digit
      BEQ   @emit_number
      LDA   #' '
      BRA   @emit_char
@emit_number:
      LDA   #1
      STA   ext_firstdig
      LDA   ext_digit
      CLC
      ADC   #'0'
@emit_char:
      STA   VGC_CHAROUT
      CLC
      LDA   ext_ptrL
      ADC   #4
      STA   ext_ptrL
      BCC   @ptr_ok
      INC   ext_ptrH
@ptr_ok:
      LDY   ext_count
      DEY
      BNE   @digit_loop
      RTS

@cmp_pow:
      LDY   #3
@cmp_loop:
      LDA   ext_u32_0,Y
      CMP   (ext_ptrL),Y
      BNE   @cmp_done         ; carry is set iff value byte >= power byte
      DEY
      BPL   @cmp_loop
      SEC                     ; equal counts as >=
@cmp_done:
      RTS

@sub_pow:
      SEC
      LDY   #0
      LDA   ext_u32_0
      SBC   (ext_ptrL),Y
      STA   ext_u32_0
      INY
      LDA   ext_u32_1
      SBC   (ext_ptrL),Y
      STA   ext_u32_1
      INY
      LDA   ext_u32_2
      SBC   (ext_ptrL),Y
      STA   ext_u32_2
      INY
      LDA   ext_u32_3
      SBC   (ext_ptrL),Y
      STA   ext_u32_3
      RTS

ext_pow10:
      .dword 1000000000
      .dword 100000000
      .dword 10000000
      .dword 1000000
      .dword 100000
      .dword 10000
      .dword 1000
      .dword 100
      .dword 10
      .dword 1

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

EXT_UNSUPPORTED:
      JMP   EXT_SNERR_VEC

; HELP handler removed (docs -> LaTeX books). ExtTable cmd 8 now points at
; EXT_UNSUPPORTED. The HELP-only scratch equates (Bpntrl_ext/Bpntrh_ext/
; help_len_ext/TKX_PREFIX_EXT) and the HELP_BUF/REG_HELP writes went with it.

; =====================================================================
; Math coprocessor BASIC function parsers that do not fit in primary ROM.
; Returns 16-bit result in A:Y using the same convention as BASIC's LAB_AYFC:
; A = high byte, Y = low byte.
; =====================================================================

EXT_MMUL16L:
      JSR   ext_parse_mul16_s16
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MMUL16H:
      JSR   ext_parse_mul16_s16
      LDY   MATH_RES2
      LDA   MATH_RES3
      RTS

EXT_MDOTS16L:
      JSR   ext_parse_vec_ab4_s16
      LDA   #MATH_VEC_OP_DOT_S16
      STA   MATH_VEC_OP
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MDOTS16H:
      JSR   ext_parse_vec_ab4_s16
      LDA   #MATH_VEC_OP_DOT_S16
      STA   MATH_VEC_OP
      LDY   MATH_RES2
      LDA   MATH_RES3
      RTS

EXT_MCROSSL:
      JSR   ext_parse_vec_ab4_s16
      LDA   #MATH_VEC_OP_CROSS_S16
      STA   MATH_VEC_OP
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MCROSSH:
      JSR   ext_parse_vec_ab4_s16
      LDA   #MATH_VEC_OP_CROSS_S16
      STA   MATH_VEC_OP
      LDY   MATH_RES2
      LDA   MATH_RES3
      RTS

EXT_MMULFX:
      JSR   ext_parse_mulfx_s16
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MDIST:
      JSR   ext_parse_dist_s16
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MSIN:
      JSR   ext_parse_sincos_angle
      LDA   MATH_RES0
      RTS

EXT_MCOS:
      JSR   ext_parse_sincos_angle
      LDA   MATH_RES1
      RTS

EXT_MRND:
      JSR   LAB_IGBY            ; consume extension token id
      LDY   MATH_RNG
      LDA   #$00
      RTS

EXT_MDIV:
      JSR   ext_parse_div_s16
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MREM:
      JSR   ext_parse_div_s16
      LDY   MATH_RES2
      LDA   MATH_RES3
      RTS

EXT_MATAN2:
      JSR   ext_parse_atan2_s16
      LDY   MATH_RES0
      LDA   #$00
      RTS

EXT_MDOTFX:
      JSR   ext_parse_vec_ab4_s16
      LDA   #MATH_VEC_OP_DOT_FX
      STA   MATH_VEC_OP
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MLEN2:
      JSR   LAB_IGBY            ; consume extension token id, advance to x
      JSR   ext_parse_vec_a2_s16
      JSR   ext_rparen
      LDA   #MATH_VEC_OP_LEN2
      STA   MATH_VEC_OP
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MSCALX:
      JSR   ext_parse_vec_a_scalar_s16
      LDA   #MATH_VEC_OP_SCALE_FX
      STA   MATH_VEC_OP
      LDY   MATH_RES0
      LDA   MATH_RES1
      RTS

EXT_MSCALY:
      JSR   ext_parse_vec_a_scalar_s16
      LDA   #MATH_VEC_OP_SCALE_FX
      STA   MATH_VEC_OP
      LDY   MATH_RES2
      LDA   MATH_RES3
      RTS

ext_parse_mul16_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to first arg
      JSR   EXT_GTSW_VEC      ; a -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_MUL16_A_LO
      LDA   FAC1_2
      STA   MATH_MUL16_A_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; b -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_MUL16_B_LO
      LDA   FAC1_2
      STA   MATH_MUL16_B_HI   ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_mulfx_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to first arg
      JSR   EXT_GTSW_VEC      ; a -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_MULFX_A_LO
      LDA   FAC1_2
      STA   MATH_MULFX_A_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; b -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_MULFX_B_LO
      LDA   FAC1_2
      STA   MATH_MULFX_B_HI   ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_dist_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to dx
      JSR   EXT_GTSW_VEC      ; dx -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_DIST_DX_LO
      LDA   FAC1_2
      STA   MATH_DIST_DX_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; dy -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_DIST_DY_LO
      LDA   FAC1_2
      STA   MATH_DIST_DY_HI   ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_sincos_angle:
      JSR   LAB_IGBY          ; consume extension token id, advance to angle
      JSR   EXT_GTBY_VEC      ; angle -> X
      STX   MATH_SINCOS_ANGLE ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_div_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to numerator
      JSR   EXT_GTSW_VEC      ; numerator -> FAC1_3/FAC1_2
      LDY   #$00
      LDA   FAC1_3
      STA   MATH_DIV_N_LO
      LDA   FAC1_2
      STA   MATH_DIV_N_1
      BPL   @div_num_extend
      DEY
@div_num_extend:
      STY   MATH_DIV_N_2
      STY   MATH_DIV_N_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; denominator -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_DIV_D_LO
      LDA   FAC1_2
      STA   MATH_DIV_D_HI     ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_atan2_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to dy
      JSR   EXT_GTSW_VEC      ; dy -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_ATAN_DY_LO
      LDA   FAC1_2
      STA   MATH_ATAN_DY_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; dx -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_ATAN_DX_LO
      LDA   FAC1_2
      STA   MATH_ATAN_DX_HI   ; write triggers coprocessor operation
      JMP   ext_rparen

ext_parse_vec_ab4_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to ax
      JSR   ext_parse_vec_a2_s16
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; bx -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_VEC_BX_LO
      LDA   FAC1_2
      STA   MATH_VEC_BX_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; by -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_VEC_BY_LO
      LDA   FAC1_2
      STA   MATH_VEC_BY_HI
      JMP   ext_rparen

ext_parse_vec_a2_s16:
      JSR   EXT_GTSW_VEC      ; ax -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_VEC_AX_LO
      LDA   FAC1_2
      STA   MATH_VEC_AX_HI
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; ay -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_VEC_AY_LO
      LDA   FAC1_2
      STA   MATH_VEC_AY_HI
      RTS

ext_parse_vec_a_scalar_s16:
      JSR   LAB_IGBY          ; consume extension token id, advance to x
      JSR   ext_parse_vec_a2_s16
      JSR   ext_comma
      JSR   EXT_GTSW_VEC      ; scalar -> FAC1_3/FAC1_2
      LDA   FAC1_3
      STA   MATH_VEC_S_LO
      LDA   FAC1_2
      STA   MATH_VEC_S_HI
      JMP   ext_rparen

; =====================================================================
; BASIC hardware helpers relocated from the primary ROM.
; Function handlers consume the raw extended token id. Command handlers
; are entered after BASIC's statement dispatcher has already consumed it.
; =====================================================================

EXT_XPEEK:
      JSR   LAB_IGBY          ; consume extension token id, advance to offset
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   XMC_XAL
      LDA   FAC1_2
      STA   XMC_XAM
      LDA   XMC_BANK
      STA   XMC_XAH
      JSR   ext_rparen
      LDA   #XMC_CMD_GET
      STA   XMC_CMD
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BEQ   @ok
      LDX   #$01
      LDA   #$00
      TAY
      RTS
@ok:
      LDX   #$00
      LDY   XMC_DATA
      LDA   #$00
      RTS

; EXT_PLAYING / EXT_MNOTE / EXT_NSTATUS / EXT_NREADY relocated to BASIC main ROM
; (@xtk_playing / @xtk_mnote / @xtk_nstatus / @xtk_nready, direct MMIO reads of
; MUSIC_STATUS / MUSIC_NOTE1 / NIC_SLOTST0). ExtTable cmds $2C-$2F neutralized.

; EXT_DMASTATUS / EXT_DMAERR / EXT_DMACOUNT / EXT_BLITSTATUS / EXT_BLITERR /
; EXT_BLITCOUNT relocated to basic.asm (@xtk_dma*/@xtk_blit* reporters, reading
; the DMA_*/BLT_* MMIO registers directly). ExtTable cmds $30-$35 neutralized.

; EXT_SPRCOLL / EXT_SPRBG relocated to BASIC main ROM (@xtk_sprcoll / @xtk_sprbg,
; direct VGC_COLLST/VGC_COLLBG read-clear + IRQ ack). ExtTable cmds $36-$37
; neutralized.

EXT_VPEEK:
      JSR   LAB_IGBY          ; consume extension token id, advance to plane
      JSR   EXT_GTBY_VEC
      STX   VGC_P0
      JSR   ext_comma
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   VGC_P1
      LDA   FAC1_2
      STA   VGC_P2
      STZ   VGC_P4
      JSR   ext_rparen
      LDA   #VCMD_MEMREAD
      STA   VGC_CMD
      JSR   ext_vgc_wait_cmd
      LDY   VGC_P3
      LDA   #$00
      RTS

; EXT_BITSET / EXT_BITCLR / EXT_BITTGL relocated to BASIC main ROM (LAB_BITSET /
; LAB_BITCLR / LAB_BITTGL + shared basic_bit_addr_mask parser). ExtTable cmds
; $39-$3B neutralized.

EXT_REVERSE:
      JSR   LAB_GBYT          ; optional explicit reverse colours?
      BEQ   @default
      CMP   #':'
      BEQ   @default
      JSR   EXT_GTBY_VEC
      TXA
      AND   #$0F
      PHA
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      TXA
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      STA   ext_firstdig
      PLA
      ORA   ext_firstdig
      STA   VGC_TXTREVATTR
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #(VTXT_REV | VTXT_REVEX)
      STA   VGC_TXTFLAGS
      LDA   #$00
      RTS
@default:
      LDA   VGC_TXTFLAGS
      AND   #$FC
      ORA   #VTXT_REV
      STA   VGC_TXTFLAGS
      LDA   #$00
      RTS

; EXT_REVERSEOFF / EXT_FLASH / EXT_FLASHOFF relocated to BASIC main ROM
; (LAB_REVERSEOFF / LAB_FLASH / LAB_FLASHOFF, direct VGC_TXTFLAGS writes).
; ExtTable cmds $3D-$3F neutralized. (EXT_REVERSE $3C kept — fg/bg parse deferred
; for main-ROM budget.)

EXT_VPOKE:
      JSR   EXT_GTBY_VEC
      STX   VGC_P0
      JSR   ext_comma
      JSR   EXT_GTWRD_VEC
      LDA   FAC1_3
      STA   VGC_P1
      LDA   FAC1_2
      STA   VGC_P2
      JSR   ext_comma
      JSR   EXT_GTBY_VEC
      STX   VGC_P3
      STZ   VGC_P4
      LDA   #VCMD_MEMWRITE
      STA   VGC_CMD
      JSR   ext_vgc_wait_cmd
      LDA   #$00
      RTS

; EXT_FIOCLR relocated to BASIC main ROM (LAB_FIOCLR -> basic_fio_clear_error
; directly). ExtTable cmd $41 neutralized. (EXT_CLS $42 kept — window-aware, not
; a thin register poke.)

EXT_CLS:
      JSR   ext_text_window_valid
      BNE   @full
      LDA   TEXTWIN_LEFT
      BNE   @window
      LDA   TEXTWIN_TOP
      BNE   @window
      LDA   TEXTWIN_WIDTH
      CMP   #80
      BNE   @window
      LDA   TEXTWIN_HEIGHT
      CMP   #50
      BNE   @window
@full:
      LDA   #$0C
      STA   VGC_CHAROUT
      JSR   ext_vgc_wait_cmd
      STZ   TPos
      LDA   #$00
      RTS
@window:
      JSR   ext_cls_charout_window
      JSR   ext_text_window_addr

      LDA   #$20
      STA   txt_tmp
      LDA   #VGC_PLANE_CHAR
      JSR   ext_cls_fill_plane

      JSR   ext_current_text_color
      STA   txt_tmp
      LDA   #VGC_PLANE_COLOR
      JSR   ext_cls_fill_plane

      JSR   ext_current_text_attr
      STA   txt_tmp
      LDA   #VGC_PLANE_TEXTATTR
      JSR   ext_cls_fill_plane

      LDA   TEXTWIN_LEFT
      STA   VGC_CURSX
      LDA   TEXTWIN_TOP
      STA   VGC_CURSY
      STZ   TPos
      LDA   #$00
      RTS

ext_text_window_valid:
      LDA   TEXTWIN_WIDTH
      BEQ   @bad
      LDA   TEXTWIN_HEIGHT
      BEQ   @bad
      LDA   TEXTWIN_LEFT
      CMP   #80
      BCS   @bad
      CLC
      ADC   TEXTWIN_WIDTH
      BCS   @bad
      CMP   #81
      BCS   @bad
      LDA   TEXTWIN_TOP
      CMP   #50
      BCS   @bad
      CLC
      ADC   TEXTWIN_HEIGHT
      BCS   @bad
      CMP   #51
      BCS   @bad
      LDA   #$00
      RTS
@bad:
      LDA   #$01
      RTS

ext_text_window_addr:
      LDA   TEXTWIN_TOP
      STA   txt_tmp
      ASL
      ASL
      CLC
      ADC   txt_tmp           ; row * 5
      STA   txt_addrL
      STZ   txt_addrH
      ASL   txt_addrL
      ROL   txt_addrH
      ASL   txt_addrL
      ROL   txt_addrH
      ASL   txt_addrL
      ROL   txt_addrH
      ASL   txt_addrL
      ROL   txt_addrH         ; row * 80
      LDA   TEXTWIN_LEFT
      CLC
      ADC   txt_addrL
      STA   txt_addrL
      BCC   @done
      INC   txt_addrH
@done:
      RTS

ext_cls_fill_plane:
      STA   BLT_DSTSPACE
      STZ   BLT_SRCSPACE
      STZ   BLT_SRCL
      STZ   BLT_SRCM
      STZ   BLT_SRCH
      STZ   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      STZ   BLT_CKEY
      LDA   txt_addrL
      STA   BLT_DSTL
      LDA   txt_addrH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   TEXTWIN_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   TEXTWIN_HEIGHT
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #80
      STA   BLT_DSTSTRL
      STZ   BLT_DSTSTRH
      LDA   txt_tmp
      STA   BLT_FILLVALUE
      LDA   #BLT_MODE_FILL
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
@wait:
      LDA   BLT_STATUS_REG
      CMP   #BLT_STATUS_BUSY
      BEQ   @wait
      RTS

ext_cls_charout_window:
      LDA   TEXTWIN_TOP
      STA   txt_addrL         ; current logical row
      LDA   TEXTWIN_HEIGHT
      STA   txt_addrH         ; rows remaining
@row:
      LDA   TEXTWIN_LEFT
      STA   VGC_CURSX
      LDA   txt_addrL
      STA   VGC_CURSY
      LDA   TEXTWIN_WIDTH
      STA   txt_tmp           ; columns remaining

      LDA   txt_addrL
      CMP   #49
      BNE   @chars
      LDA   TEXTWIN_LEFT
      CLC
      ADC   TEXTWIN_WIDTH
      CMP   #80
      BNE   @chars
      DEC   txt_tmp           ; avoid printing bottom-right and scrolling
@chars:
      LDA   txt_tmp
      BEQ   @bottom_right
@char:
      LDA   #$20
      STA   VGC_CHAROUT
      DEC   txt_tmp
      BNE   @char
@bottom_right:
      LDA   txt_addrL
      CMP   #49
      BNE   @next
      LDA   TEXTWIN_LEFT
      CLC
      ADC   TEXTWIN_WIDTH
      CMP   #80
      BNE   @next
      LDA   #80
      STA   VGC_CURSX
      LDA   #49
      STA   VGC_CURSY
      LDA   #$08              ; backspace erases cell 79 without scrolling
      STA   VGC_CHAROUT
@next:
      INC   txt_addrL
      DEC   txt_addrH
      BNE   @row
      RTS

ext_current_text_color:
      LDA   VGC_TXTFLAGS
      AND   #VTXT_REV
      BEQ   @normal
      LDA   VGC_TXTFLAGS
      AND   #VTXT_REVEX
      BEQ   @swap
      LDA   VGC_TXTREVATTR
      RTS
@normal:
      LDA   VGC_BGCOL
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      STA   txt_tmp
      LDA   VGC_FGCOL
      AND   #$0F
      ORA   txt_tmp
      RTS
@swap:
      LDA   VGC_FGCOL
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      STA   txt_tmp
      LDA   VGC_BGCOL
      AND   #$0F
      ORA   txt_tmp
      RTS

ext_current_text_attr:
      LDX   #$00
      LDA   VGC_TXTFLAGS
      AND   #VTXT_FLASH
      BEQ   :+
      LDX   #VTXT_ATTR_FLASH
:
      LDA   VGC_TXTFLAGS
      AND   #VTXT_BOLD
      BEQ   :+
      TXA
      ORA   #VTXT_ATTR_BOLD
      TAX
:
      TXA
      RTS

; ext_parse_addr_mask removed (BIT* relocated to main basic_bit_addr_mask).
; ext_nic_status removed (NSTATUS/NREADY relocated to main, inline NIC_SLOTST0).

ext_vgc_wait_cmd:
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

; =====================================================================
; DMA / blitter BASIC parsers + handlers RELOCATED to BASIC main ROM.
; The EXT_DMACOPY/EXT_DMAFILL/EXT_BLITCOPY/EXT_BLTFILL statement handlers and
; their ext_cword / ext_dma_*_bank / ext_blt_*_bank parser helpers, plus the
; EXT_DMASTATUS.. reporters, now live in basic.asm (LAB_DMACOPY.. and the
; @xtk_dma*/@xtk_blit* reporters) and drive the DMA_*/BLT_* MMIO registers
; directly. Their ExtTable slots (cmds $09/$0A/$0F/$10 and $30-$35) are
; neutralized to EXT_UNSUPPORTED. The shared NDK implementations are linked from
; runtime/asm/build/nova.lib; this file includes only ABI headers.
; =====================================================================

AUDIO_POINTER_FILE_HELPERS = 0
NOVA_EMIT_ALL_RUNTIME = 1
      .include "audio.inc"
      .include "rng.inc"
      .include "xram.inc"
      .include "xmc.inc"

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

; --- Local helpers (shared by several extension handlers) ---

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

ext_rparen:
      JSR   LAB_GBYT
      CMP   #')'
      BNE   @no_rparen
      JSR   LAB_IGBY
      RTS
@no_rparen:
      JMP   EXT_SNERR_VEC

; (runtime_labels.inc moved to the SYSTEM module; the ADDR table no longer
;  lives in this ROM — see SYS_ADDR_LOOKUP.)

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

      .word $021C             ; NMI  -> RAM NMI handler
      .word ExtReset          ; RESET -> extension reset handler
      .word $020D             ; IRQ  -> RAM IRQ handler
