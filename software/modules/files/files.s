; files.s — FILES module (MODULE_ID_FILES = $04). THIN lib_call wrappers over the
; file-I/O NDK (runtime/asm/fio.s) and the file pager (runtime/asm/pager.s). The
; NDK owns all file/host transport logic; this module only marshals the mailbox
; args into the FIO/PAGER pseudo-registers and JSRs the NDK routine.
;
; Coverage: file save/load, directory listing + delete, VGC graphics save/load,
; directory navigation (cd/mkdir/rmdir/pwd), runtime-ROM load, the raw-command
; exec alias, the host RNG reporter, and the file-slice pager. Per
; ModuleNdkContractTests this module's own code never pokes SID registers or
; busy-waits the VGC frame counter — every fn wraps an NDK routine.
;
; Naming: ids are FILE_* (the file-domain library, see libfiles.inc). FIO_* is
; the MMIO register prefix from nova.inc — a different namespace.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libfiles.inc"
      .include "nova.inc"              ; NVR* scratch + FIO MMIO registers
      .include "fio.inc"             ; FIO_ARG_* pointer-filename pseudo-registers
      .include "pager.inc"           ; PAGER_* pseudo-registers + target selectors

      .segment "CODE"
      lib_module_header MODULE_ID_FILES, LIB_ABI_VERSION, FILE_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by nova module pack into files.nmod).
;
; Each ;@ndk line maps the fn to the NDK routine it wraps; the human summary is
; single-sourced from that routine (feedback-ndk-source-of-truth). The build-time
; + CI drift guard requires the wrapper body to actually JSR that routine. Mailbox
; @arg bindings stay here — they are this wrapper's marshalling, not duplicated.
; ===========================================================================
;@module FILES
;@version 1.0
;@brief Shared file services: the fio + pager NDK (thin wrappers over fio.s/pager.s).
;
;@fn FILE_SAVE
;@ndk fio_save
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1 byte0)
;@arg src u16 first RAM address to save -> FIO_SRCL/H (ARG2)
;@arg end u16 last RAM address to save -> FIO_ENDL/H (ARG3)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_LOAD
;@ndk fio_load
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1 byte0)
;@arg dest u16 destination RAM address -> FIO_SRCL/H (ARG2)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_DIR_OPEN
;@ndk fio_dir_open
;@arg nameptr u16 pointer to optional filter name, ignored when namelen=0 (ARG0)
;@arg namelen u8 filter length 0..63; 0 lists everything (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_DIR_READ
;@ndk fio_dir_read
;@ret void (entry data lands in the FIO directory result registers)
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_DELETE
;@ndk fio_delete
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_GSAVE
;@ndk fio_gsave
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1 byte0)
;@arg gspace u8 VGC memory space selector -> FIO_GSPACE (ARG2 byte0)
;@arg gaddr u16 VGC source address -> FIO_GADDRL/H (ARG2 byte1, byte2)
;@arg glen u16 byte count -> FIO_GLENL/H (ARG3)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_GLOAD
;@ndk fio_gload
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1 byte0)
;@arg gspace u8 VGC memory space selector -> FIO_GSPACE (ARG2 byte0)
;@arg gaddr u16 VGC destination address -> FIO_GADDRL/H (ARG2 byte1, byte2)
;@arg glen u16 byte count -> FIO_GLENL/H (ARG3)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_CD
;@ndk fio_cd
;@arg nameptr u16 pointer to directory name (ARG0)
;@arg namelen u8 name length 1..63 (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_MKDIR
;@ndk fio_mkdir
;@arg nameptr u16 pointer to directory name (ARG0)
;@arg namelen u8 name length 1..63 (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_RMDIR
;@ndk fio_rmdir
;@arg nameptr u16 pointer to directory name (ARG0)
;@arg namelen u8 name length 1..63 (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_PWD
;@ndk fio_pwd
;@ret void (current directory lands in FIO.NAME/FIO.NAMELEN)
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_LOAD_RUNTIME
;@ndk fio_load_runtime
;@arg nameptr u16 pointer to runtime image filename (ARG0)
;@arg namelen u8 name length 1..63 (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_RUN
;@ndk fio_run
;@arg cmd u8 raw FIO command byte -> A (ARG0 byte0)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_RNG
;@ndk fio_rng
;@ret u32 32 host-backed random bits from FIO_RNG0..3 (RESULT bytes 0..3)
;@status LERR_OK, LERR_FIO_FAIL
;
;@fn FILE_PAGE
;@ndk pager_load_file_page
;@arg nameptr u16 pointer to filename bytes (ARG0 byte0, byte1)
;@arg namelen u8 filename length 1..63 (ARG0 byte2)
;@arg offset u24 byte offset inside the file -> PAGER_FILEL/M/H (ARG1 byte0,1,2)
;@arg target u8 0=XRAM, 1=CPU RAM, 2=VGC memory -> PAGER_TARGET (ARG1 byte3)
;@arg destaddr u24 destination address/space -> PAGER_ADDRL/M/H (ARG2 byte0,1,2)
;@arg len u16 byte count -> PAGER_LENL/H (ARG3)
;@ret void
;@status LERR_OK, LERR_FIO_FAIL

; ---------------------------------------------------------------------------
; dispatch — fn-id router (RTS-trick). FILE_FN_COUNT is small so fn*2 < 256.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #FILE_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     file_jtable+1,x
      pha
      lda     file_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

file_jtable:
      .word   file_save-1              ; $00 FILE_SAVE
      .word   file_load-1              ; $01 FILE_LOAD
      .word   file_dir_open-1          ; $02 FILE_DIR_OPEN
      .word   file_dir_read-1          ; $03 FILE_DIR_READ
      .word   file_delete-1            ; $04 FILE_DELETE
      .word   file_gsave-1             ; $05 FILE_GSAVE
      .word   file_gload-1             ; $06 FILE_GLOAD
      .word   file_cd-1                ; $07 FILE_CD
      .word   file_mkdir-1             ; $08 FILE_MKDIR
      .word   file_rmdir-1             ; $09 FILE_RMDIR
      .word   file_pwd-1               ; $0A FILE_PWD
      .word   file_load_runtime-1      ; $0B FILE_LOAD_RUNTIME
      .word   file_run-1               ; $0C FILE_RUN
      .word   file_rng-1               ; $0D FILE_RNG
      .word   file_page-1              ; $0E FILE_PAGE
      .word   file_fopen-1             ; $0F FILE_FOPEN
      .word   file_fcreate-1           ; $10 FILE_FCREATE
      .word   file_fclose-1            ; $11 FILE_FCLOSE
      .word   file_fread-1             ; $12 FILE_FREAD
      .word   file_fwrite-1            ; $13 FILE_FWRITE
      .word   file_fseek-1             ; $14 FILE_FSEEK
      .word   file_ftell-1             ; $15 FILE_FTELL
      .word   file_fsize-1             ; $16 FILE_FSIZE
      .word   file_fresize-1           ; $17 FILE_FRESIZE
      .word   file_fflush-1            ; $18 FILE_FFLUSH
      .word   file_fstatus-1           ; $19 FILE_FSTATUS
      .word   file_fdelete-1           ; $1A FILE_FDELETE
      .word   file_frename-1           ; $1B FILE_FRENAME
      .word   file_dir_list-1          ; $1C FILE_DIR_LIST

; ===========================================================================
; Shared epilogues. file_finish_status maps the NDK A result (0=OK / 1=err) to
; LIB_STATUS and zeroes RESULT. These are NOT ;@ndk-mapped, so the drift guard's
; per-wrapper JSR check ignores them.
; ===========================================================================
file_finish_status:
      cmp     #$00
      bne     @fail
      stz     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_FIO_FAIL
      sta     LIB_STATUS
      rts

file_finish_result_fio_src:
      cmp     #$00
      bne     @fail
      LDA     FIO_SRCL
      STA     LIB_RESULT
      LDA     FIO_SRCH
      STA     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_FIO_FAIL
      sta     LIB_STATUS
      rts

file_finish_result_fio_size:
      cmp     #$00
      bne     @fail
      LDA     FIO_SIZEL
      STA     LIB_RESULT
      LDA     FIO_SIZEH
      STA     LIB_RESULT+1
      LDA     FIO_SIZE2
      STA     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_FIO_FAIL
      sta     LIB_STATUS
      rts

; file_marshal_name — marshal the pointer-filename ABI shared by the name-taking
; fns: ARG0 word -> FIO_ARG_NAMEPTR_L/H, ARG1 byte0 -> FIO_ARG_NAMELEN, then copy
; into FIO.NAME via the NDK. Returns A=0/1 from fio_copy_name (1 on bad/empty
; name). NOT ;@ndk-mapped, so the per-wrapper drift JSR check ignores it.
file_marshal_name:
      LDA   LIB_ARG0+0
      STA   FIO_ARG_NAMEPTR_L          ; = NVR4H
      LDA   LIB_ARG0+1
      STA   FIO_ARG_NAMEPTR_H          ; = NVR5L
      LDA   LIB_ARG1+0
      STA   FIO_ARG_NAMELEN            ; = NVR4L
      JMP   fio_copy_name

; ===========================================================================
; File save / load.
; ===========================================================================

; --- $00 FILE_SAVE: name + ARG2=src, ARG3=end -> fio_save ---
file_save:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      LDA   LIB_ARG2+0
      STA   FIO_SRCL
      LDA   LIB_ARG2+1
      STA   FIO_SRCH
      LDA   LIB_ARG3+0
      STA   FIO_ENDL
      LDA   LIB_ARG3+1
      STA   FIO_ENDH
      LDA   LIB_ARG1+1               ; file type rides the namelen cell's high byte
      STA   FIO_DIRTYPE              ; -> the host stamps it into the .ndi dir entry
      JSR   fio_save
      JMP   file_finish_status

; --- $01 FILE_LOAD: name + ARG2=dest -> fio_load ---
file_load:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      LDA   LIB_ARG2+0
      STA   FIO_SRCL                   ; load destination address
      LDA   LIB_ARG2+1
      STA   FIO_SRCH
      JSR   fio_load
      JMP   file_finish_status

; Shared bad-name epilogue: fio_copy_name already set FIO_STATUS/ERRCODE; report
; failure to the mailbox. (Not ;@ndk-mapped; ignored by the drift JSR check.)
file_name_fail:
      LDA   #LERR_FIO_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Directory listing + delete.
; ===========================================================================

; --- $02 FILE_DIR_OPEN: optional name filter (namelen=0 lists all) -> fio_dir_open ---
file_dir_open:
      LDA   LIB_ARG1+0                 ; filter length
      BEQ   @nofilter
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      BRA   @issue
@nofilter:
      STZ   FIO_NAMELEN                ; no filter -> list everything
@issue:
      JSR   fio_dir_open
      JMP   file_finish_status

; --- $03 FILE_DIR_READ: () -> fio_dir_read (entry -> FIO dir result regs) ---
file_dir_read:
      JSR   fio_dir_read
      JMP   file_finish_status

; --- $1C FILE_DIR_LIST: optional filter -> open + iterate + print whole listing.
; Keeps the bank-0 BASIC DIR handler thin: this module (bank-1, with VGC MMIO)
; does the open/read loop and prints a header + one NAME / TYPE / SIZE row per
; entry (dirs show blank size). Shared by BASIC, novaforth and novalogo DIR.
file_dir_list:
      LDA   LIB_ARG1+0                 ; filter length (0 = list everything)
      BEQ   @fdl_nofilter
      JSR   file_marshal_name
      BEQ   @fdl_open
      JMP   file_name_fail
@fdl_nofilter:
      STZ   FIO_NAMELEN
@fdl_open:
      JSR   fdl_header                 ; column header + rule (once)
      LDA   #FIO_DIR_FLAG_FULLNAME
      STA   FIO_DIRTYPE
      JSR   fio_dir_open               ; open the listing (entries fetched below)
@fdl_loop:
      JSR   fio_dir_read               ; A=0 -> entry ready, !=0 -> end of dir
      CMP   #$00
      BNE   @fdl_done
      LDY   #$00                       ; --- NAME, left, padded to 18 cols ---
@fdl_nm:
      CPY   FIO_NAMELEN
      BCS   @fdl_nmpad
      CPY   #32
      BCS   @fdl_type
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BRA   @fdl_nm
@fdl_nmpad:
      CPY   #34
      BCS   @fdl_type
      LDA   #' '
      STA   VGC_CHAROUT
      INY
      BRA   @fdl_nmpad
@fdl_type:
      LDA   FIO_DIRTYPE                ; --- TYPE (first 7 of the 8-byte field) ---
      CMP   #$0B
      BCC   @fdl_typeok
      LDA   #$02                       ; out of range -> BIN
@fdl_typeok:
      ASL   a
      ASL   a
      ASL   a
      ASL   a                          ; type * 16
      TAX
      LDY   #$0E
@fdl_tl:
      LDA   fdl_strs,X
      STA   VGC_CHAROUT
      INX
      DEY
      BNE   @fdl_tl
      JSR   fdl_size                   ; --- SIZE (right-justified decimal) ---
      LDA   #$0D                        ; Keep CR for compatibility with older VGCs;
      STA   VGC_CHAROUT                 ; LF is Nova's logical newline.
      LDA   #$0A
      STA   VGC_CHAROUT
      BRA   @fdl_loop                  ; read + print the next entry
@fdl_done:
      STZ   LIB_STATUS                 ; listing completed OK
      RTS

; Column header + rule line (printed once before the entries).
fdl_header:
      LDX   #$00
@fh:  LDA   fdl_hdr,X
      BEQ   @fhd
      STA   VGC_CHAROUT
      INX
      BRA   @fh
@fhd: RTS

; Print the full 24-bit FIO size as right-justified decimal (leading spaces), with
; a one-column gap in front. A directory entry (type DIR) prints blanks.
fdl_size:
      LDA   #' '
      STA   VGC_CHAROUT
      LDA   FIO_DIRTYPE
      CMP   #FIO_TYPE_DIR
      BNE   @fs_num
      LDX   #$08                       ; dir: 8 blanks
@fs_blank:
      LDA   #' '
      STA   VGC_CHAROUT
      DEX
      BNE   @fs_blank
      RTS
@fs_num:
      LDA   FIO_SIZEL
      STA   NVR0L
      LDA   FIO_SIZEH
      STA   NVR0H
      LDA   FIO_SIZE2
      STA   NVR1L
      LDX   #$00                       ; place index 0..7 (10000000..1)
      STZ   NVR1H                      ; "printed a nonzero digit" flag
@fs_place:
      LDY   #$00                       ; digit value at this place
@fs_sub:
      LDA   NVR0L
      SEC
      SBC   pow10_lo,X
      STA   NVR2L
      LDA   NVR0H
      SBC   pow10_mid,X
      STA   NVR2H
      LDA   NVR1L
      SBC   pow10_hi,X
      BCC   @fs_emit                   ; NVR0 < pow10 -> place done
      STA   NVR3L
      LDA   NVR2L
      STA   NVR0L
      LDA   NVR2H
      STA   NVR0H
      LDA   NVR3L
      STA   NVR1L
      INY
      BRA   @fs_sub
@fs_emit:
      CPX   #$07                       ; the 1's place -> always a digit
      BEQ   @fs_dig
      CPY   #$00
      BNE   @fs_dig
      LDA   NVR1H                      ; leading zero with nothing printed yet?
      BNE   @fs_dig
      LDA   #' '                       ; -> blank instead of '0'
      STA   VGC_CHAROUT
      BRA   @fs_next
@fs_dig:
      TYA
      ORA   #'0'
      STA   VGC_CHAROUT
      LDA   #$01
      STA   NVR1H
@fs_next:
      INX
      CPX   #$08
      BCC   @fs_place
      RTS

pow10_lo:  .byte $80, $40, $A0, $10, $E8, $64, $0A, $01
pow10_mid: .byte $96, $42, $86, $27, $03, $00, $00, $00
pow10_hi:  .byte $98, $0F, $01, $00, $00, $00, $00, $00

fdl_hdr:
      .byte "NAME                              TYPE           SIZE", $0D, $0A
      .byte "--------------------------------  -------------- --------", $0D, $0A, $00

fdl_strs:                              ; 11 x 16-byte type fields (print first 14)
      .byte "BAS             "
      .byte "SID             "
      .byte "BIN             "
      .byte "MID             "
      .byte "GFX             "
      .byte "DIR             "
      .byte "FORTH           "
      .byte "LOGO            "
      .byte "PASCAL SOURCE   "
      .byte "ASM             "
      .byte "PASCAL PROJECT  "

; --- $04 FILE_DELETE: name -> fio_delete ---
file_delete:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_delete
      JMP   file_finish_status

; ===========================================================================
; Graphics / VGC memory save + load.
; ===========================================================================

; --- $05 FILE_GSAVE: name + ARG2=space/gaddr, ARG3=glen -> fio_gsave ---
file_gsave:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   file_marshal_gregs
      JSR   fio_gsave
      JMP   file_finish_status

; --- $06 FILE_GLOAD: name + ARG2=space/gaddr, ARG3=glen -> fio_gload ---
file_gload:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   file_marshal_gregs
      JSR   fio_gload
      JMP   file_finish_status

; file_marshal_gregs — ARG2 byte0=space, byte1/2=gaddr, ARG3 word=glen into the
; FIO graphics registers. Shared by gsave/gload. (Not ;@ndk-mapped.)
file_marshal_gregs:
      LDA   LIB_ARG2+0
      STA   FIO_GSPACE
      LDA   LIB_ARG2+1
      STA   FIO_GADDRL
      LDA   LIB_ARG2+2
      STA   FIO_GADDRH
      LDA   LIB_ARG3+0
      STA   FIO_GLENL
      LDA   LIB_ARG3+1
      STA   FIO_GLENH
      RTS

; ===========================================================================
; Directory navigation.
; ===========================================================================

; --- $07 FILE_CD: name -> fio_cd ---
file_cd:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_cd
      JMP   file_finish_status

; --- $08 FILE_MKDIR: name -> fio_mkdir ---
file_mkdir:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_mkdir
      JMP   file_finish_status

; --- $09 FILE_RMDIR: name -> fio_rmdir ---
file_rmdir:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_rmdir
      JMP   file_finish_status

; --- $0A FILE_PWD: () -> fio_pwd (CWD -> FIO.NAME/FIO.NAMELEN) ---
file_pwd:
      JSR   fio_pwd
      JMP   file_finish_status

; ===========================================================================
; Runtime ROM load + raw-command exec alias.
; ===========================================================================

; --- $0B FILE_LOAD_RUNTIME: name -> fio_load_runtime ---
file_load_runtime:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_load_runtime
      JMP   file_finish_status

; --- $0C FILE_RUN: ARG0 byte0 = raw FIO command -> A -> fio_run ---
file_run:
      LDA   LIB_ARG0+0                 ; FIO command byte
      JSR   fio_run
      JMP   file_finish_status

; ===========================================================================
; Host RNG reporter — publish FIO_RNG0..3 to LIB_RESULT bytes 0..3.
; ===========================================================================

; --- $0D FILE_RNG: () -> RESULT = 32 random bits -> fio_rng ---
file_rng:
      JSR   fio_rng
      CMP   #$00
      BNE   @fail
      LDA   FIO_RNG0
      STA   LIB_RESULT
      LDA   FIO_RNG1
      STA   LIB_RESULT+1
      LDA   FIO_RNG2
      STA   LIB_RESULT+2
      LDA   FIO_RNG3
      STA   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_FIO_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; File pager — stream a file slice into XRAM / CPU RAM / VGC memory.
; pager_load_file_page does fio_copy_name itself, so we only marshal the FIO_ARG
; name pseudo-registers plus the PAGER offset/target/addr/len cells, then JSR it.
; ===========================================================================

; --- $0E FILE_PAGE: see ;@arg packing -> pager_load_file_page ---
file_page:
      LDA   LIB_ARG0+0
      STA   FIO_ARG_NAMEPTR_L          ; = PAGER_NAMEPTR_L = NVR4H
      LDA   LIB_ARG0+1
      STA   FIO_ARG_NAMEPTR_H          ; = PAGER_NAMEPTR_H = NVR5L
      LDA   LIB_ARG0+2
      STA   FIO_ARG_NAMELEN            ; = PAGER_NAMELEN = NVR4L
      LDA   LIB_ARG1+0
      STA   PAGER_FILEL                ; 24-bit file offset
      LDA   LIB_ARG1+1
      STA   PAGER_FILEM
      LDA   LIB_ARG1+2
      STA   PAGER_FILEH
      LDA   LIB_ARG1+3
      STA   PAGER_TARGET               ; 0=XRAM, 1=CPU RAM, 2=VGC
      LDA   LIB_ARG2+0
      STA   PAGER_ADDRL                ; destination address / space
      LDA   LIB_ARG2+1
      STA   PAGER_ADDRM
      LDA   LIB_ARG2+2
      STA   PAGER_ADDRH
      LDA   LIB_ARG3+0
      STA   PAGER_LENL
      LDA   LIB_ARG3+1
      STA   PAGER_LENH
      JSR   pager_load_file_page
      JMP   file_finish_result_fio_size

; ===========================================================================
; Low-level byte-stream file handles.
; ===========================================================================

file_fopen:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      LDA   LIB_ARG2+0
      STA   FIO_DIRTYPE
      JSR   fio_fopen
      JMP   file_finish_result_fio_src

file_fcreate:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      LDA   LIB_ARG2+0
      STA   FIO_DIRTYPE
      JSR   fio_fcreate
      JMP   file_finish_result_fio_src

file_fclose:
      JSR   file_marshal_fileid
      JSR   fio_fclose
      JMP   file_finish_status

file_fread:
      JSR   file_marshal_file_transfer
      JSR   fio_fread
      JMP   file_finish_result_fio_size

file_fwrite:
      JSR   file_marshal_file_transfer
      JSR   fio_fwrite
      JMP   file_finish_result_fio_size

file_fseek:
      JSR   file_marshal_fileid
      JSR   file_marshal_size_arg1
      JSR   fio_fseek
      JMP   file_finish_status

file_ftell:
      JSR   file_marshal_fileid
      JSR   fio_ftell
      JMP   file_finish_result_fio_size

file_fsize:
      JSR   file_marshal_fileid
      JSR   fio_fsize
      JMP   file_finish_result_fio_size

file_fresize:
      JSR   file_marshal_fileid
      JSR   file_marshal_size_arg1
      JSR   fio_fresize
      JMP   file_finish_status

file_fflush:
      JSR   file_marshal_fileid
      JSR   fio_fflush
      JMP   file_finish_status

file_fstatus:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_fstatus
      JMP   file_finish_result_fio_src

file_fdelete:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      JSR   fio_fdelete_exact
      JMP   file_finish_status

file_frename:
      JSR   file_marshal_name
      BEQ   :+
      JMP   file_name_fail
:
      LDA   LIB_ARG2+0
      STA   FIO_ENDL
      LDA   LIB_ARG2+1
      STA   FIO_ENDH
      LDA   LIB_ARG3+0
      STA   FIO_GLENL
      LDA   LIB_ARG3+1
      STA   FIO_GLENH
      JSR   fio_frename
      JMP   file_finish_status

file_marshal_fileid:
      LDA   LIB_ARG0+0
      STA   FIO_SRCL
      LDA   LIB_ARG0+1
      STA   FIO_SRCH
      RTS

file_marshal_file_transfer:
      JSR   file_marshal_fileid
      LDA   LIB_ARG1+0
      STA   FIO_ENDL
      LDA   LIB_ARG1+1
      STA   FIO_ENDH
      LDA   LIB_ARG2+0
      STA   FIO_GLENL
      LDA   LIB_ARG2+1
      STA   FIO_GLENH
      RTS

file_marshal_size_arg1:
      LDA   LIB_ARG1+0
      STA   FIO_SIZEL
      LDA   LIB_ARG1+1
      STA   FIO_SIZEH
      LDA   LIB_ARG1+2
      STA   FIO_SIZE2
      RTS

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced(fio_*/pager_*)
; is true (selective emit keeps the image small). fio.s is included first so its
; selective-emit guards see this module's fio_* references; pager.s then re-
; includes fio.s (its .ifndef guard makes that a no-op) and emits the pager body.
; ===========================================================================
      .include "fio.inc"
      .include "pager.inc"

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)
