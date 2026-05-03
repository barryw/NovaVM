; Shared NovaVM XMC metadata and command processor.
;
; This layer implements the XMC register command contract used by BASIC and
; available to other runtimes that want the named-block allocator on top of
; the core flat XRAM routines.

.include "xram.inc"

.ifndef XRAM_IMPLEMENTATION_INCLUDED
      .import xram_xmc_read8
      .import xram_xmc_write8
      .import xram_xmc_copy_from_ram
      .import xram_xmc_copy_to_ram
      .import xram_xmc_fill
.endif

      .export xmc_process
      .export xmc_command_status
      .export xmc_select_bank
      .export xmc_put_byte
      .export xmc_stash_raw
      .export xmc_fetch_raw
      .export xmc_release
      .export xmc_reset_usage
      .export xmc_alloc_block
      .export xmc_named_stash
      .export xmc_named_fetch
      .export xmc_named_delete
      .export xmc_dir_open
      .export xmc_dir_read
      .export xmc_window_bit
      .export xmc_set_window_addr_current_bank
      .export xmc_map_window
      .export xmc_unmap_window

      .segment "CODE"

; ---------------------------------------------------------------------
; XMC command processor and named-block metadata
; ---------------------------------------------------------------------
.ifndef META_PAGES
META_PAGES      = 5
.endif
.ifndef TOTAL_PAGES
TOTAL_PAGES     = 2048
.endif

; =====================================================================
; xmc_process — internal XMC command dispatcher
; Reads XMC_CMD ($BA00) and executes the requested operation.
; Sets XMC_STATUS and XMC_ERRCODE on completion.
; =====================================================================
; @label XMC.PROCESS
; @kind routine
; @symbol xmc_process
; @summary Process XMC_CMD and update XMC_STATUS/XMC_ERRCODE.
; @requires XMC_CMD
xmc_process:
      LDA   XMC_CMD
      ; Build index: cmd * 2 for word table lookup
      ; Commands 1-15 map to table entries 0-14
      BEQ   @xmc_nop             ; cmd 0 = no-op
      CMP   #$10
      BCS   @xmc_nop             ; cmd >= 16 = no-op
      SEC
      SBC   #$01                 ; cmd 1 -> index 0
      ASL                        ; * 2 for word table
      TAX
      LDA   xmc_jtab+1,X
      PHA
      LDA   xmc_jtab,X
      PHA
      RTS                        ; dispatch via RTS trick
@xmc_nop:
      JMP   xmc_ok

; Jump table for XMC commands 1-15 (address-1 for RTS trick)
xmc_jtab:
      .word xmc_getbyte-1        ; cmd $01: GetByte
      .word xmc_putbyte-1        ; cmd $02: PutByte
      .word xmc_stash-1          ; cmd $03: Stash
      .word xmc_fetch-1          ; cmd $04: Fetch
      .word xmc_fill-1           ; cmd $05: Fill
      .word xmc_ok-1             ; cmd $06: (unused)
      .word xmc_stats-1          ; cmd $07: Stats
      .word xmc_resetusage-1     ; cmd $08: ResetUsage
      .word xmc_ok-1             ; cmd $09: Release (no-op in bump allocator)
      .word xmc_alloc-1          ; cmd $0A: Alloc
      .word xmc_nstash-1         ; cmd $0B: NStash
      .word xmc_nfetch-1         ; cmd $0C: NFetch
      .word xmc_ndelete-1        ; cmd $0D: NDelete
      .word xmc_ndiropen-1       ; cmd $0E: NDirOpen
      .word xmc_ndirread-1       ; cmd $0F: NDirRead

; --- Helper: set XMC status OK ---
xmc_ok:
      LDA   #XMC_OK
      STA   XMC_STATUS
      LDA   #$00
      STA   XMC_ERRCODE
      RTS

; =====================================================================
; GetByte — read XRAM[XA] into XMC_DATA
; =====================================================================
xmc_getbyte:
      JMP   xram_xmc_read8

; =====================================================================
; PutByte — write XMC_DATA to XRAM[XA]
; =====================================================================
xmc_putbyte:
      JMP   xram_xmc_write8

; =====================================================================
; Stash — copy RAM → XRAM using the shared DMA-backed XRAM runtime
; XMC_RAML/H = source, XMC_XAL/M/H = dest, XMC_LENL/H = count
; =====================================================================
xmc_stash:
      JMP   xram_xmc_copy_from_ram

; =====================================================================
; Fetch — copy XRAM → RAM using the shared DMA-backed XRAM runtime
; XMC_XAL/M/H = source, XMC_RAML/H = dest, XMC_LENL/H = count
; =====================================================================
xmc_fetch:
      JMP   xram_xmc_copy_to_ram

; =====================================================================
; Fill — fill XRAM with XMC_DATA using the shared DMA-backed XRAM runtime
; XMC_XAL/M/H = start, XMC_LENL/H = count, XMC_DATA = fill byte
; =====================================================================
xmc_fill:
      JMP   xram_xmc_fill

; =====================================================================
; XRAM metadata layout (bump allocator + directory)
;
; Page 0 ($000000): Control block
;   $00-$02: next_free_addr (24-bit LE)
;   $03:     next_handle (1-255)
;   $04:     dir_count (active named entries)
;   $0F:     magic sentinel ($A5 = initialized)
;
; Pages 1-4 ($000100-$0004FF): Directory (32 entries × 32 bytes)
;   Per entry: name[24], handle, xaddrL, xaddrM, xaddrH, lenL, lenH, pagesL, pagesH
;
; Data starts at $000500 (page 5). META_PAGES = 5.
; =====================================================================

MAGIC_VAL       = $A5
CTRL_NEXTFREE   = $00           ; 3 bytes
CTRL_NEXTHDL    = $03           ; 1 byte
CTRL_DIRCOUNT   = $04           ; 1 byte
CTRL_MAGIC      = $0F           ; 1 byte
DIR_ENTRY_SIZE  = 32
DIR_MAX_ENTRIES = 32
DIR_NAME_LEN    = 24
DIR_OFF_HANDLE  = 24
DIR_OFF_XAL     = 25
DIR_OFF_XAM     = 26
DIR_OFF_XAH     = 27
DIR_OFF_LENL    = 28
DIR_OFF_LENH    = 29
DIR_OFF_PGSL    = 30
DIR_OFF_PGSH    = 31
DATA_START_LO   = $00
DATA_START_MI   = $05
DATA_START_HI   = $00

      .segment "BSS"

; Private XMC metadata state. These are deliberately not NVR aliases; NVR is
; reserved for transient caller arguments and scratch.
xmc_eidx:       .res 1          ; directory entry index (0-31)
xmc_dircur:     .res 1          ; directory cursor for NDirOpen/Read
xmc_npgL:       .res 1          ; pages needed low
xmc_npgH:       .res 1          ; pages needed high
xmc_tmp:        .res 1          ; scratch
xmc_tmp2:       .res 1          ; scratch

      .segment "CODE"

; ---------------------------------------------------------------------
; Assembly-facing convenience wrappers
; ---------------------------------------------------------------------

; @label XMC.COMMAND_STATUS
; @kind routine
; @symbol xmc_command_status
; @summary Process XMC_CMD and return A=0 on XMC_OK, A=1 otherwise.
; @requires XMC_CMD
; @out A: 0 on success, 1 on error.
xmc_command_status:
      JSR   xmc_process
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BEQ   @ok
      LDA   #$01
      RTS
@ok:
      LDA   #$00
      RTS

xmc_run_cmd:
      STA   XMC_CMD
      JMP   xmc_command_status

; @label XMC.SELECT_BANK
; @kind routine
; @symbol xmc_select_bank
; @summary Select the current 64KB XRAM bank.
; @in X: Bank index, 0..XMC_BANKS-1.
; @out A: 0 on success, 1 if the bank is out of range.
xmc_select_bank:
      CPX   XMC_BANKS
      BCC   @ok
      JMP   xmc_window_bad
@ok:
      STX   XMC_BANK
      JMP   xmc_ok

; @label XMC.PUT_BYTE
; @kind routine
; @symbol xmc_put_byte
; @summary Write XMC_DATA to XMC_XAL/M/H.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_DATA
xmc_put_byte:
      LDA   #XMC_CMD_PUT
      JMP   xmc_run_cmd

; @label XMC.STASH_RAW
; @kind routine
; @symbol xmc_stash_raw
; @summary Copy RAM to XRAM using XMC_RAML/H, XMC_XAL/M/H, and XMC_LENL/H.
; @requires XMC_RAML XMC_RAMH XMC_XAL XMC_XAM XMC_XAH XMC_LENL XMC_LENH
xmc_stash_raw:
      LDA   #XMC_CMD_STASH
      JMP   xmc_run_cmd

; @label XMC.FETCH_RAW
; @kind routine
; @symbol xmc_fetch_raw
; @summary Copy XRAM to RAM using XMC_XAL/M/H, XMC_RAML/H, and XMC_LENL/H.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_RAML XMC_RAMH XMC_LENL XMC_LENH
xmc_fetch_raw:
      LDA   #XMC_CMD_FETCH
      JMP   xmc_run_cmd

; @label XMC.RELEASE
; @kind routine
; @symbol xmc_release
; @summary Release an XMC allocation range.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_LENL XMC_LENH
xmc_release:
      LDA   #XMC_CMD_REL
      JMP   xmc_run_cmd

; @label XMC.RESET_USAGE
; @kind routine
; @symbol xmc_reset_usage
; @summary Reset XRAM usage and named allocation metadata.
xmc_reset_usage:
      LDA   #XMC_CMD_RSTUS
      JMP   xmc_run_cmd

; @label XMC.ALLOC
; @kind routine
; @symbol xmc_alloc_block
; @summary Allocate an XRAM block using XMC_LENL/H.
; @requires XMC_LENL XMC_LENH
; @out XMC_XAL: Allocated address low byte.
; @out XMC_XAM: Allocated address middle byte.
; @out XMC_XAH: Allocated address high byte.
xmc_alloc_block:
      LDA   #XMC_CMD_ALLOC
      JMP   xmc_run_cmd

; @label XMC.NAMED_STASH
; @kind routine
; @symbol xmc_named_stash
; @summary Allocate/update a named XRAM block and copy RAM into it.
; @requires XMC_NAME XMC_NAMELEN XMC_RAML XMC_RAMH XMC_LENL XMC_LENH
xmc_named_stash:
      LDA   #XMC_CMD_NSTSH
      JMP   xmc_run_cmd

; @label XMC.NAMED_FETCH
; @kind routine
; @symbol xmc_named_fetch
; @summary Fetch a named XRAM block into RAM.
; @requires XMC_NAME XMC_NAMELEN XMC_RAML XMC_RAMH
xmc_named_fetch:
      LDA   #XMC_CMD_NFETC
      JMP   xmc_run_cmd

; @label XMC.NAMED_DELETE
; @kind routine
; @symbol xmc_named_delete
; @summary Delete a named XRAM block.
; @requires XMC_NAME XMC_NAMELEN
xmc_named_delete:
      LDA   #XMC_CMD_NDEL
      JMP   xmc_run_cmd

; @label XMC.DIR_OPEN
; @kind routine
; @symbol xmc_dir_open
; @summary Open the named XRAM block directory.
xmc_dir_open:
      LDA   #XMC_CMD_NDIRO
      JMP   xmc_run_cmd

; @label XMC.DIR_READ
; @kind routine
; @symbol xmc_dir_read
; @summary Read the next named XRAM block directory entry.
xmc_dir_read:
      LDA   #XMC_CMD_NDIRR
      JMP   xmc_run_cmd

; @label XMC.WINDOW_BIT
; @kind routine
; @symbol xmc_window_bit
; @summary Convert window index 0..3 to its XMC_WINCTL bit mask.
; @in X: Window index, 0..3.
; @out A: Window bit mask.
; @out C: Clear on success, set on invalid index.
xmc_window_bit:
      CPX   #$04
      BCS   @bad
      LDA   #$01
@shl:
      DEX
      BMI   @ok
      ASL
      BNE   @shl
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; @label XMC.SET_WINDOW_ADDR
; @kind routine
; @symbol xmc_set_window_addr_current_bank
; @summary Set a mapped window base from XMC_XAL/M and the selected XMC bank.
; @in X: Window index, 0..3.
; @requires XMC_XAL XMC_XAM XMC_BANK
xmc_set_window_addr_current_bank:
      CPX   #$00
      BNE   @set1
      LDA   XMC_XAL
      STA   XMC_W0AL
      LDA   XMC_XAM
      STA   XMC_W0AM
      LDA   XMC_BANK
      STA   XMC_W0AH
      RTS
@set1:
      CPX   #$01
      BNE   @set2
      LDA   XMC_XAL
      STA   XMC_W1AL
      LDA   XMC_XAM
      STA   XMC_W1AM
      LDA   XMC_BANK
      STA   XMC_W1AH
      RTS
@set2:
      CPX   #$02
      BNE   @set3
      LDA   XMC_XAL
      STA   XMC_W2AL
      LDA   XMC_XAM
      STA   XMC_W2AM
      LDA   XMC_BANK
      STA   XMC_W2AH
      RTS
@set3:
      LDA   XMC_XAL
      STA   XMC_W3AL
      LDA   XMC_XAM
      STA   XMC_W3AM
      LDA   XMC_BANK
      STA   XMC_W3AH
      RTS

; @label XMC.MAP_WINDOW
; @kind routine
; @symbol xmc_map_window
; @summary Map window X to XMC_XAL/M in the selected XMC bank.
; @in X: Window index, 0..3.
; @requires XMC_XAL XMC_XAM XMC_BANK
xmc_map_window:
      STX   xmc_tmp
      JSR   xmc_window_bit
      BCS   xmc_window_bad
      STA   xmc_tmp2
      LDX   xmc_tmp
      JSR   xmc_set_window_addr_current_bank
      LDA   XMC_WINCTL
      ORA   xmc_tmp2
      STA   XMC_WINCTL
      JMP   xmc_ok

; @label XMC.UNMAP_WINDOW
; @kind routine
; @symbol xmc_unmap_window
; @summary Disable a mapped XRAM window.
; @in X: Window index, 0..3.
xmc_unmap_window:
      JSR   xmc_window_bit
      BCS   xmc_window_bad
      EOR   #$FF
      AND   XMC_WINCTL
      STA   XMC_WINCTL
      JMP   xmc_ok

xmc_window_bad:
      LDA   #XMC_ERR_RANGE
      JSR   xmc_err
      LDA   #$01
      RTS

; --- Helper: set XMC error status ---
xmc_err:
      ; A = error code
      PHA
      LDA   #$03
      STA   XMC_STATUS
      PLA
      STA   XMC_ERRCODE
      RTS

; --- Helper: map window 3 to XRAM page N (A=mid, X=high) ---
xmc_map_w3_ax:
      PHA
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      PLA
      STX   WIN3_HI
      STA   WIN3_MI
      LDA   #$00
      STA   WIN3_LO
      RTS

; --- Helper: map window 3 to control page (page 0) ---
xmc_map_ctrl:
      LDA   #$00
      TAX
      JMP   xmc_map_w3_ax

; --- Helper: map window 3 to directory page for entry xmc_eidx ---
; Entry N is at XRAM page (1 + N/8), offset (N%8)*32
xmc_map_dir_entry:
      LDA   xmc_eidx
      LSR
      LSR
      LSR                        ; A = N/8
      CLC
      ADC   #$01                 ; page = 1 + N/8
      LDX   #$00
      JMP   xmc_map_w3_ax

; --- Helper: get window offset for entry xmc_eidx → Y ---
xmc_dir_offset:
      LDA   xmc_eidx
      AND   #$07                 ; N % 8
      ASL
      ASL
      ASL
      ASL
      ASL                        ; * 32
      TAY
      RTS

; --- Helper: ensure metadata initialized ---
xmc_init_check:
      JSR   xmc_map_ctrl
      LDA   WIN3_BASE + CTRL_MAGIC
      CMP   #MAGIC_VAL
      BEQ   @init_done
      ; First use — initialize metadata
      ; Clear all of pages 0-4 (1280 bytes) via the shared DMA XRAM fill.
      LDA   XMC_XAL
      PHA
      LDA   XMC_XAM
      PHA
      LDA   XMC_XAH
      PHA
      LDA   XMC_LENL
      PHA
      LDA   XMC_LENH
      PHA
      LDA   XMC_DATA
      PHA
      STZ   XMC_XAL
      STZ   XMC_XAM
      STZ   XMC_XAH
      LDA   #<1280
      STA   XMC_LENL
      LDA   #>1280
      STA   XMC_LENH
      STZ   XMC_DATA
      JSR   xram_xmc_fill
      PLA
      STA   XMC_DATA
      PLA
      STA   XMC_LENH
      PLA
      STA   XMC_LENL
      PLA
      STA   XMC_XAH
      PLA
      STA   XMC_XAM
      PLA
      STA   XMC_XAL
      ; Set control block defaults
      JSR   xmc_map_ctrl
      LDA   #DATA_START_LO
      STA   WIN3_BASE + CTRL_NEXTFREE
      LDA   #DATA_START_MI
      STA   WIN3_BASE + CTRL_NEXTFREE + 1
      LDA   #DATA_START_HI
      STA   WIN3_BASE + CTRL_NEXTFREE + 2
      LDA   #$01
      STA   WIN3_BASE + CTRL_NEXTHDL
      LDA   #$00
      STA   WIN3_BASE + CTRL_DIRCOUNT
      LDA   #MAGIC_VAL
      STA   WIN3_BASE + CTRL_MAGIC
@init_done:
      RTS

; =====================================================================
; Stats — compute XRAM usage from control block
; =====================================================================
xmc_stats:
      JSR   xmc_init_check
      JSR   xmc_map_ctrl
      ; used_pages = (next_free - $500) / 256 + META_PAGES
      ; Since data starts at $500, next_free mid byte = pages used (+ $500 offset)
      LDA   WIN3_BASE + CTRL_NEXTFREE + 1
      SEC
      SBC   #DATA_START_MI       ; subtract $05
      STA   XMC_USEDL
      LDA   WIN3_BASE + CTRL_NEXTFREE + 2
      SBC   #DATA_START_HI
      STA   XMC_USEDH
      ; Add META_PAGES to used count
      LDA   XMC_USEDL
      CLC
      ADC   #META_PAGES
      STA   XMC_USEDL
      LDA   XMC_USEDH
      ADC   #$00
      STA   XMC_USEDH
      ; free = TOTAL_PAGES - used
      LDA   #<TOTAL_PAGES
      SEC
      SBC   XMC_USEDL
      STA   XMC_FREEL
      LDA   #>TOTAL_PAGES
      SBC   XMC_USEDH
      STA   XMC_FREEH
      ; Count named entries
      LDA   WIN3_BASE + CTRL_DIRCOUNT
      STA   XMC_DIRCOUNTL
      LDA   #$00
      STA   XMC_DIRCOUNTH
      JMP   xmc_ok

; =====================================================================
; ResetUsage — clear all allocations and directory
; =====================================================================
xmc_resetusage:
      ; Reset magic to force re-init on next use
      JSR   xmc_map_ctrl
      LDA   #$00
      STA   WIN3_BASE + CTRL_MAGIC
      JSR   xmc_init_check
      JMP   xmc_ok

; =====================================================================
; Alloc — allocate pages from bump pointer
; Input: XMC_LENL/H = bytes needed
; Output: XMC_XAL/M/H = allocated address, XMC_HANDLE
; =====================================================================
xmc_alloc:
      JSR   xmc_init_check
      ; Calculate pages = (len + 255) / 256 = (lenH) + (lenL > 0 ? 1 : 0)
      LDA   XMC_LENH
      STA   xmc_npgH
      LDA   XMC_LENL
      BEQ   @al_nornd
      INC   xmc_npgH            ; round up if low byte nonzero
      CLC                        ; but if lenH was $FF, this overflows — cap at $FF
@al_nornd:
      LDA   xmc_npgH
      STA   xmc_npgL             ; pages low = npgH (since pages = bytes/256 rounded up)
      LDA   #$00
      STA   xmc_npgH             ; pages high = 0 for <=255 pages (max 64KB alloc)
      ; Check we have space: next_free + pages*256 <= $080000 (512KB)
      JSR   xmc_map_ctrl
      ; Capture next_free as result address
      LDA   WIN3_BASE + CTRL_NEXTFREE
      STA   XMC_XAL
      LDA   WIN3_BASE + CTRL_NEXTFREE + 1
      STA   XMC_XAM
      LDA   WIN3_BASE + CTRL_NEXTFREE + 2
      STA   XMC_XAH
      ; Advance: next_free += pages * 256 (= pages in mid byte)
      LDA   WIN3_BASE + CTRL_NEXTFREE + 1
      CLC
      ADC   xmc_npgL
      STA   WIN3_BASE + CTRL_NEXTFREE + 1
      LDA   WIN3_BASE + CTRL_NEXTFREE + 2
      ADC   #$00
      STA   WIN3_BASE + CTRL_NEXTFREE + 2
      ; Check overflow: if high byte >= $08, out of space (512KB = $080000)
      CMP   #$08
      BCS   @al_nospace
      ; Allocate handle
      LDA   WIN3_BASE + CTRL_NEXTHDL
      STA   XMC_HANDLE
      INC   WIN3_BASE + CTRL_NEXTHDL
      LDA   WIN3_BASE + CTRL_NEXTHDL
      BNE   @al_hdlok
      LDA   #$01                 ; wrap from 0 to 1
      STA   WIN3_BASE + CTRL_NEXTHDL
@al_hdlok:
      ; Find free directory entry
      JSR   xmc_dir_find_free
      BCS   @al_nospace          ; no free entries
      ; Write directory entry
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset       ; Y = offset within page
      LDA   XMC_HANDLE
      STA   WIN3_BASE + DIR_OFF_HANDLE,Y
      LDA   XMC_XAL
      STA   WIN3_BASE + DIR_OFF_XAL,Y
      LDA   XMC_XAM
      STA   WIN3_BASE + DIR_OFF_XAM,Y
      LDA   XMC_XAH
      STA   WIN3_BASE + DIR_OFF_XAH,Y
      LDA   XMC_LENL
      STA   WIN3_BASE + DIR_OFF_LENL,Y
      LDA   XMC_LENH
      STA   WIN3_BASE + DIR_OFF_LENH,Y
      LDA   xmc_npgL
      STA   WIN3_BASE + DIR_OFF_PGSL,Y
      LDA   #$00
      STA   WIN3_BASE + DIR_OFF_PGSH,Y
      ; Clear name (unnamed alloc)
      LDX   #DIR_NAME_LEN-1
@al_clrnm:
      STA   WIN3_BASE,Y
      INY
      DEX
      BPL   @al_clrnm
      JMP   xmc_ok
@al_nospace:
      LDA   #XMC_ERR_NOSPACE
      JMP   xmc_err

; --- Find free directory entry → xmc_eidx, carry clear=found ---
xmc_dir_find_free:
      LDA   #$00
      STA   xmc_eidx
@dff_loop:
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset
      LDA   WIN3_BASE + DIR_OFF_HANDLE,Y
      BEQ   @dff_found           ; handle=0 means free
      INC   xmc_eidx
      LDA   xmc_eidx
      CMP   #DIR_MAX_ENTRIES
      BCC   @dff_loop
      SEC                        ; not found
      RTS
@dff_found:
      CLC
      RTS

; --- Find directory entry by name → xmc_eidx, carry clear=found ---
xmc_dir_find_name:
      LDA   #$00
      STA   xmc_eidx
@dfn_loop:
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset
      LDA   WIN3_BASE + DIR_OFF_HANDLE,Y
      BEQ   @dfn_next            ; skip free entries
      ; Compare name: XMC_NAME[0..NAMELEN-1] vs entry name
      TYA
      TAX                        ; X = base offset
      LDY   #$00
@dfn_cmp:
      CPY   XMC_NAMELEN
      BCS   @dfn_cmpend
      LDA   XMC_NAME,Y
      CMP   WIN3_BASE,X
      BNE   @dfn_next
      INX
      INY
      BNE   @dfn_cmp
@dfn_cmpend:
      ; Lengths must match: entry name[NAMELEN] should be 0 (null)
      LDA   WIN3_BASE,X
      BEQ   @dfn_found           ; entry name ends here too
@dfn_next:
      INC   xmc_eidx
      LDA   xmc_eidx
      CMP   #DIR_MAX_ENTRIES
      BCC   @dfn_loop
      SEC
      RTS
@dfn_found:
      CLC
      RTS

; =====================================================================
; NStash — named stash: allocate (or reuse) block, copy RAM → XRAM
; Input: XMC_NAME/NAMELEN, XMC_RAML/H, XMC_LENL/H
; =====================================================================
xmc_nstash:
      JSR   xmc_init_check
      LDA   XMC_NAMELEN
      BEQ   @ns_badname
      CMP   #DIR_NAME_LEN+1
      BCS   @ns_badname
      ; Check if name already exists
      JSR   xmc_dir_find_name
      BCS   @ns_new              ; not found → allocate new
      ; Found existing — delete it first, then allocate fresh
      JSR   xmc_dir_delete_entry
@ns_new:
      ; Allocate block
      JSR   xmc_alloc
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BNE   @ns_done             ; alloc failed, status already set
      ; Write name into directory entry
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset       ; Y = page offset
      LDX   #$00
@ns_wrnm:
      CPX   XMC_NAMELEN
      BCS   @ns_pad
      LDA   XMC_NAME,X
      STA   WIN3_BASE,Y
      INY
      INX
      BNE   @ns_wrnm
@ns_pad:
      CPX   #DIR_NAME_LEN
      BCS   @ns_nameset
      LDA   #$00
      STA   WIN3_BASE,Y
      INY
      INX
      BNE   @ns_pad
@ns_nameset:
      ; Update dir_count
      JSR   xmc_map_ctrl
      INC   WIN3_BASE + CTRL_DIRCOUNT
      ; Copy RAM → XRAM via blitter (XMC_XAL/M/H already set by alloc)
      JSR   xmc_stash
@ns_done:
      RTS
@ns_badname:
      LDA   #XMC_ERR_NAME
      JMP   xmc_err

; --- Delete directory entry at xmc_eidx ---
xmc_dir_delete_entry:
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset
      ; Clear the entry (32 bytes of zeros)
      LDX   #DIR_ENTRY_SIZE
@dde_clr:
      LDA   #$00
      STA   WIN3_BASE,Y
      INY
      DEX
      BNE   @dde_clr
      ; Decrement dir_count
      JSR   xmc_map_ctrl
      DEC   WIN3_BASE + CTRL_DIRCOUNT
      RTS

; =====================================================================
; NFetch — named fetch: look up block, copy XRAM → RAM
; Input: XMC_NAME/NAMELEN, XMC_RAML/H, XMC_LENL/H (0=all)
; =====================================================================
xmc_nfetch:
      JSR   xmc_init_check
      LDA   XMC_NAMELEN
      BEQ   @nf_badname
      JSR   xmc_dir_find_name
      BCS   @nf_notfound
      ; Read entry address and length
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset
      LDA   WIN3_BASE + DIR_OFF_XAL,Y
      STA   XMC_XAL
      LDA   WIN3_BASE + DIR_OFF_XAM,Y
      STA   XMC_XAM
      LDA   WIN3_BASE + DIR_OFF_XAH,Y
      STA   XMC_XAH
      LDA   WIN3_BASE + DIR_OFF_HANDLE,Y
      STA   XMC_HANDLE
      ; If requested length is 0, use block's full length
      LDA   XMC_LENL
      ORA   XMC_LENH
      BNE   @nf_haslen
      LDA   WIN3_BASE + DIR_OFF_LENL,Y
      STA   XMC_LENL
      LDA   WIN3_BASE + DIR_OFF_LENH,Y
      STA   XMC_LENH
@nf_haslen:
      ; Copy XRAM → RAM via blitter
      JMP   xmc_fetch
@nf_notfound:
      LDA   #XMC_ERR_NF
      JMP   xmc_err
@nf_badname:
      LDA   #XMC_ERR_NAME
      JMP   xmc_err

; =====================================================================
; NDelete — delete named block
; Input: XMC_NAME/NAMELEN
; =====================================================================
xmc_ndelete:
      JSR   xmc_init_check
      LDA   XMC_NAMELEN
      BEQ   @nd_badname
      JSR   xmc_dir_find_name
      BCS   @nd_notfound
      JSR   xmc_dir_delete_entry
      JMP   xmc_ok
@nd_notfound:
      LDA   #XMC_ERR_NF
      JMP   xmc_err
@nd_badname:
      LDA   #XMC_ERR_NAME
      JMP   xmc_err

; =====================================================================
; NDirOpen — start directory enumeration, emit first entry
; =====================================================================
xmc_ndiropen:
      JSR   xmc_init_check
      LDA   #$00
      STA   xmc_dircur
      ; Find first named entry
      JMP   xmc_dir_emit_next

; =====================================================================
; NDirRead — emit next directory entry
; =====================================================================
xmc_ndirread:
      INC   xmc_dircur
      ; fall through

; --- Emit directory entry at or after xmc_dircur, advance cursor ---
xmc_dir_emit_next:
@den_loop:
      LDA   xmc_dircur
      CMP   #DIR_MAX_ENTRIES
      BCS   @den_eod
      STA   xmc_eidx
      JSR   xmc_map_dir_entry
      JSR   xmc_dir_offset
      ; Check if entry is active and named
      LDA   WIN3_BASE + DIR_OFF_HANDLE,Y
      BEQ   @den_skip            ; free entry
      LDA   WIN3_BASE,Y          ; first byte of name
      BEQ   @den_skip            ; unnamed entry
      ; Emit this entry to XMC registers
      LDA   WIN3_BASE + DIR_OFF_HANDLE,Y
      STA   XMC_HANDLE
      LDA   WIN3_BASE + DIR_OFF_XAL,Y
      STA   XMC_XAL
      LDA   WIN3_BASE + DIR_OFF_XAM,Y
      STA   XMC_XAM
      LDA   WIN3_BASE + DIR_OFF_XAH,Y
      STA   XMC_XAH
      LDA   WIN3_BASE + DIR_OFF_LENL,Y
      STA   XMC_LENL
      LDA   WIN3_BASE + DIR_OFF_LENH,Y
      STA   XMC_LENH
      ; Copy name to XMC_NAME
      TYA
      TAX                        ; X = base offset of name in window
      LDY   #$00
@den_cpnm:
      LDA   WIN3_BASE,X
      STA   XMC_NAME,Y
      BEQ   @den_nmend           ; null terminator
      INX
      INY
      CPY   #DIR_NAME_LEN
      BCC   @den_cpnm
@den_nmend:
      STY   XMC_NAMELEN
      JMP   xmc_ok
@den_skip:
      INC   xmc_dircur
      JMP   @den_loop
@den_eod:
      LDA   #XMC_ERR_EOD
      JMP   xmc_err
