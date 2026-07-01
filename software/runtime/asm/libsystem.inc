; libsystem.inc — SYSTEM module (MODULE_ID_SYSTEM = $03) fn-id contract.
;
; The shared system-services module: host-neutral facilities that no single
; runtime should carry in its own 16K ROM. Built once (modules/system/system.s),
; staged to the XRAM shelf, paged into bank 1 on first lib_call.
;
; The full-screen source editor lives in MODULE_ID_EDITOR (libeditor.inc). The
; original SYS_FN_EDIT slot is intentionally left unimplemented so the remaining
; SYSTEM fn ids stay stable.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_SYSTEM
;   LIB_FN_ID  ($0301) = one of the SYS_FN_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code

.ifndef LIBSYSTEM_INC
LIBSYSTEM_INC = 1

.include "libabi.inc"            ; MODULE_ID_SYSTEM, LIB_* mailbox cells, LERR_*

; --- Status code returned when a wrapped fallible NDK routine reports failure ---
; The rng_* / nui_* / overlay_* NDK routines return A=0 on success / A!=0 on error
; (and leave detail in their own result cell — RNG in A only, NUI in NUI_RESULT,
; OVERLAY in OVL_RESULT). The wrapper maps A!=0 -> LIB_STATUS = LERR_SYS_FAIL,
; A==0 -> LERR_OK. Reporters publish their value/keycode to LIB_RESULT with OK.
LERR_SYS_FAIL  = $8D   ; rng_*/nui_*/overlay_* NDK routine returned A!=0

; $00 is the retired SYS_FN_EDIT slot. Call lib_call(EDITOR, EDITOR_FN_EDIT).
SYS_FN_EDIT_RETIRED = $00

; --- timing (moved from the NovaLogo extension ROM in Phase B) ---
SYS_FN_WAIT     = $01   ; (n:ARG0 frames)  -> busy-wait N video frames
SYS_FN_WAITVBL  = $02   ; ()               -> wait for the next vertical blank
SYS_FN_TIMER    = $03   ; ()  reporter     -> RESULT = VGC frame counter (8-bit)

; =====================================================================
; Host random-number service (rng.s, host-backed entropy via FIO.CMD_RNG).
; All three reporters share one host call; they differ only in how many of the
; returned bytes the wrapper publishes to LIB_RESULT. A=0 success / A=1 error.
; =====================================================================
SYS_RNG8        = $04   ; ()  reporter     -> RESULT b0       = 8 random bits
SYS_RNG16       = $05   ; ()  reporter     -> RESULT b0..1    = 16 random bits
SYS_RNG32       = $06   ; ()  reporter     -> RESULT b0..3    = 32 random bits

; =====================================================================
; Modal application UI (nui.s). Dialog geometry + title/message/footer string
; pointers ride in the mailbox; the wrapper stages them into the NUI_* BSS the
; NDK reads. A=0 (NUI_OK) success / A=1 (NUI_ERR) error, except WAIT_KEY which
; returns the pressed key byte in A (always LERR_OK).
;   ARG0 b0/b1 = title  ptr   ARG1 b0/b1 = message ptr   ARG2 b0/b1 = footer ptr
;   ARG3 b0=left b1=top b2=width b3=height (all-zero => keep current geometry)
; Save-under calls use caller-owned XRAM buffers so UI code does not reserve or
; overwrite any runtime-specific XRAM. Each buffer must hold width*height bytes:
;   ARG0 b0=left b1=top b2=width b3=height
;   ARG1 b0..2 = char-plane XRAM address
;   ARG2 b0..2 = color-plane XRAM address
;   ARG3 b0..2 = text-attr-plane XRAM address
; Full save-under uses a config struct and also preserves graphics pixels. Use
; this for dialogs drawn over graphics-plane content:
;   ARG0 b0/b1 = config ptr
;     +0 left +1 top +2 width +3 height
;     +4 char u24 +7 color u24 +10 textattr u24 +13 gfx u24
;   The text buffers are width*height bytes. The gfx buffer is
;   (width*4)*(height*4) bytes because NUI geometry is in text cells.
; List picker uses a caller-owned config struct so the four mailbox args stay
; stable as the control grows:
;   ARG0 b0/b1 = config ptr
;     +0 u16 title ptr      +2 u16 fixed-width rows ptr
;     +4 u8 row width       +5 u8 row count      +6 u8 selected
;     +7 u8 left            +8 u8 top            +9 u8 width
;    +10 u8 height         +11 u16 footer ptr
;   Out: RESULT b0 = selected row, RESULT b1 = NUI_RESULT_OK/CANCEL
; UI style is caller-controlled palette indexes:
;   ARG0 b0=shadow, b1=top/left border, b2=panel,
;        b3=packed text color (bg/fg).
; =====================================================================
SYS_DIALOG_DEFAULTS = $07 ; ()                          -> set the centered default dialog geometry
SYS_DIALOG          = $08 ; (title,msg,footer,geom)     -> draw the modal dialog, no wait
SYS_DIALOG_WAIT     = $09 ; (title,msg,footer,geom)     -> draw the modal dialog, then wait one key
SYS_DIALOG_ERROR    = $0A ; (title,msg,footer,geom)     -> draw a modal error dialog + wait one key
SYS_WAIT_KEY        = $0B ; ()  reporter                -> RESULT b0 = the pressed key byte
SYS_NUI_SAVE_UNDER    = $13 ; (geom,charbuf,colorbuf,attrbuf) -> save text planes into XRAM
SYS_NUI_RESTORE_UNDER = $14 ; (geom,charbuf,colorbuf,attrbuf) -> restore saved text planes
SYS_NUI_PICK_LIST     = $15 ; (configptr) reporter -> RESULT b0 selected, b1 OK/CANCEL
SYS_NUI_SAVE_UNDER_FULL    = $16 ; (configptr) -> save text + graphics planes into XRAM
SYS_NUI_RESTORE_UNDER_FULL = $17 ; (configptr) -> restore text + graphics planes from XRAM
SYS_NUI_SET_STYLE          = $18 ; (shadow,border,panel,text) -> set NUI chrome/text colors

; =====================================================================
; Fixed-address overlay manager (overlay.s, streams a NOVO overlay into CPU RAM
; via the pager and calls its ABI entry vectors). A=0 (OVL_OK) success / A=1
; (OVL_ERR) error; the detail OVL_ERR_* code is published to RESULT b1 on error.
;   LOAD: ARG0 b0/b1 = name ptr   ARG1 b0 = name len   ARG2 b0/b1 = load address
;         ARG3 b0/b1 = max slot length (payload + BSS must fit)
; =====================================================================
SYS_OVL_LOAD    = $0C   ; (name,len,load,maxlen)       -> load+activate a fixed overlay
SYS_OVL_UNLOAD  = $0D   ; ()                           -> unload the active overlay
SYS_OVL_INIT    = $0E   ; ()                           -> call the overlay init entry (optional)
SYS_OVL_MAIN    = $0F   ; ()                           -> call the overlay main entry (required)
SYS_OVL_TICK    = $10   ; ()                           -> call the overlay tick entry (optional)

; =====================================================================
; Runtime label lookup (sys_addr_lookup, backs BASIC's ADDR("LABEL")).
; The generated runtime_labels.inc hash table (MMIO registers + BASIC-ROM
; routine addresses) lives in this module's 16K image instead of the EhBASIC
; extension ROM. The wrapper hashes the UPPERCASED name (x33), walks the bucket
; chain (entry = len byte + 16-bit LE value + name bytes), and returns the raw
; 16-bit value as data (correct even though BASIC ROM is unmapped while bank 1
; is paged in).
;   In:  ARG0 b0/b1 = name ptr   ARG0 b2 = name len   (ARG0 b3 = 0)
;   Out (hit):  RESULT b0 = addr low, b1 = addr high; LIB_STATUS = LERR_OK
;   Out (miss / len 0): LIB_STATUS = LERR_SYS_FAIL ($8D); RESULT ignored
; =====================================================================
SYS_ADDR_LOOKUP = $11   ; (nameptr,namelen) reporter -> RESULT b0..1 = 16-bit address

; =====================================================================
; C64-style full-screen line editor (sys_screen_readline, backs BASIC's
; LAB_1357). Runs the whole interactive edit loop against VGC MMIO so it
; works identically on Avalonia and FPGA: poll VGC_CHARIN, echo printables /
; move the cursor / backspace directly on the char-plane screen window, and on
; ENTER read the physical screen row under the cursor into the caller's buffer
; and return its length. The submitted line carries no trailing NUL (matching
; the legacy LAB_1357 contract); trailing spaces on the row are trimmed.
;   In:  ARG0 b0/b1 = destination buffer ptr   ARG0 b2 = max length
;   Out: RESULT b0 = submitted line length; buffer filled (no trailing NUL);
;        LIB_STATUS = LERR_OK. (RESULT b1 reserved for a future exit reason.)
; =====================================================================
SYS_SCREEN_READLINE = $12 ; (bufptr,maxlen) -> RESULT b0 = line length, buffer filled

; Highest implemented fn-id + 1. Gaps and ids >= SYS_FN_COUNT resolve to
; LERR_NO_FN via the dispatch bounds-check.
SYS_FN_COUNT  = $19

.endif
