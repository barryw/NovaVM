; libsystem.inc — SYSTEM module (MODULE_ID_SYSTEM = $03) fn-id contract.
;
; The shared system-services module: host-neutral facilities that no single
; runtime should carry in its own 16K ROM. Built once (modules/system/system.s),
; staged to the XRAM shelf, paged into bank 1 on first lib_call.
;
; Phase A ships exactly one fn — the interactive text editor (SYS_FN_EDIT), moved
; verbatim out of the NovaLogo extension ROM. The editor is language-neutral
; (see editbuf.inc): it edits a flat text buffer in low RAM and runs entirely
; within one lib_call (keyboard + screen are MMIO, which survive the bank swap).
; No module->runtime callback is needed; the host builds any document record from
; the final buffer AFTER the editor returns.
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

; =====================================================================
; $00  editor
; =====================================================================
; SYS_FN_EDIT — run the shared EDITUI/EDITBUF modal editor on a RAM text buffer.
;   In:  ARG0 lo word  = buffer ptr        ARG0 hi word = initial cursor byte offset
;        ARG1 lo word  = current length    ARG2 lo word = capacity (bytes)
;        ARG3 lo word  = NUL-terminated title ptr
;   Out: RESULT byte0  = editbuf exit reason (EDITBUF_EXIT_*)
;        RESULT byte1  = nonzero if the user requested a save (Ctrl-S / "Save First")
;        ARG1 lo word  = final buffer length
;        LIB_STATUS    = LERR_OK
SYS_FN_EDIT   = $00

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
; =====================================================================
SYS_DIALOG_DEFAULTS = $07 ; ()                          -> set the centered default dialog geometry
SYS_DIALOG          = $08 ; (title,msg,footer,geom)     -> draw the modal dialog, no wait
SYS_DIALOG_WAIT     = $09 ; (title,msg,footer,geom)     -> draw the modal dialog, then wait one key
SYS_DIALOG_ERROR    = $0A ; (title,msg,footer,geom)     -> draw a modal error dialog + wait one key
SYS_WAIT_KEY        = $0B ; ()  reporter                -> RESULT b0 = the pressed key byte

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

; Highest implemented fn-id + 1. Gaps and ids >= SYS_FN_COUNT resolve to
; LERR_NO_FN via the dispatch bounds-check.
SYS_FN_COUNT  = $11

.endif
